#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // settings
    QCoreApplication::setOrganizationName("theme");
    QCoreApplication::setOrganizationDomain("github.com/theme");
    QCoreApplication::setApplicationName("MangaLib");
    QSettings settings;

    db_ = new SQLiteDB(":/dbschema.json", this);
    hp_ = new HashPool(db_,this);
    tp_ = new TagPool(db_,this);
    rp_ = new RankPool(db_, hp_,this);

    // clip board
    clipboard_ = QApplication::clipboard();
    connect(clipboard_, SIGNAL(dataChanged()),
            this, SLOT(onClipboardChanged()));

    // menus
    createActions();
    createMenus(); // ui::menu
    createTrayIcon(); // ui: tray icon

    // ui: File explorer
    FSmixDBmodel *fmixd_;
    fmixd_ = new FSmixDBmodel(hp_, rp_, this);
    fmixd_->setRootPath(QDir::rootPath());
    fmixd_->setFilter(QDir::Files);
    fmixd_->setNameFilters(QStringList()
                           << "*.zip"
                           << "*.cb?"
                           << "*.rar");
    fmixd_->setNameFilterDisables(false);
    file_exp_widget_ = new FileExplorer(fmixd_, this);
    ui->topTabWidget->addTab(file_exp_widget_, "&Explorer");

    connect(this, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    QString expath = settings.value("file_exp/lastPath").toString();
    QFileInfo expi( expath );
    if ( expi.isDir() ) {
        file_exp_widget_->setPath(expath);  // recover last time path
    }

    // ui: tags
    FileTagsWidget *w = new FileTagsWidget(tp_,this);
    w->hide();
    ui->centralWidget->layout()->addWidget(w);
    connect(file_exp_widget_, SIGNAL(sigFilePath(QString)),
            w, SLOT(setFileName(QString)));
    connect(file_exp_widget_, SIGNAL(sigFileNameSelected(bool)),
            w, SLOT(setVisible(bool)));
    file_tags_widget_ = w;

    // ui: Library ( DB ) View
    connect(db_, SIGNAL(sigOpened()), this, SLOT(loadDBTabs()));
    connect(db_, SIGNAL(sigOpened(bool)), closeAct, SLOT(setEnabled(bool)));
    connect(db_, SIGNAL(sigOpened(bool)), dbManAct, SLOT(setEnabled(bool)));
    connect(db_, SIGNAL(sigOpened(bool)), tags2DBAct, SLOT(setEnabled(bool)));
    connect(db_, SIGNAL(sigClosed()), this, SLOT(removeDBTabs()));
    connect(db_, SIGNAL(sigClosed(bool)), closeAct, SLOT(setDisabled(bool)));
    connect(db_, SIGNAL(sigClosed(bool)), dbManAct, SLOT(setDisabled(bool)));
    connect(db_, SIGNAL(sigClosed(bool)), tags2DBAct, SLOT(setDisabled(bool)));

    // auto open "~/mangalib.db"

    QString sqlfile = settings.value("db/sqliteFile").toString();

    if( QFile::exists(sqlfile) ){
        db_->open(sqlfile);
    } else if( QFile::exists(QDir::homePath() + "/mangalib.db")){
        db_->open(QDir::homePath() + "/mangalib.db");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDBfile(QString fn)
{
    if ( fn.isEmpty() ){
        QString selfilter = tr("DB Files (*.db)");
        fn = QFileDialog::getSaveFileName( this,
                                           tr("Choose DB file"),
                                           QDir::homePath(),
                                           tr("DB Files (*.db)"),
                                           &selfilter,
                                           QFileDialog::DontConfirmOverwrite );
    }

    if (!fn.isEmpty()){
        db_->open(fn);
        QSettings sett;
        sett.setValue("db/sqliteFile", fn);
        sett.sync();
    } else {
        emit sigStatusMsg(tr("no file selected."));
    }
}

void MainWindow::closeDBconnection()
{
    db_->closeDB();
}

void MainWindow::loadDBTabs()
{
    QStringList dbtables = db_->tables();
    QStringList tables, filter;
    filter << "file" << "tag" << "book" << "author";
    for (int i = 0; i < filter.size(); ++i){
        if (dbtables.contains(filter.at(i))){
            tables.append(filter.at(i));
        }
    }
    for( int i = 0; i < tables.size(); ++i){
        DBTableWidget* w = new DBTableWidget(tables.at(i),
                                             db_->conn(),
                                             ui->topTabWidget);

        QString tname = QString("( &") + QString::number(i+1) + " ) "
                + tables.at(i);
        int tabIndex = ui->topTabWidget->addTab(w,tname);
        db_table_widgets_hash_.insert(w,tabIndex);
    }
}

void MainWindow::removeDBTabs()
{
    QHash<DBTableWidget*,int>::iterator iter;
    for (iter = db_table_widgets_hash_.begin(); iter != db_table_widgets_hash_.end(); ++iter){
        ui->topTabWidget->removeTab(iter.value());
        iter.key()->deleteLater();
    }
    db_table_widgets_hash_.clear();
}

void MainWindow::openDBManager()
{
    DBManager *w = new DBManager(db_, this);
    connect(w, SIGNAL(finished(int)), w, SLOT(deleteLater()));
    w->showMaximized();
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open Sqlite DB file"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Choose and open a Sqlite DB file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDBfile()));

    closeAct = new QAction(tr("&Close DB connection"), this);
    closeAct->setShortcut(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close current DB connection"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeDBconnection()));

    quitAct = new QAction(QIcon(":/icons/quit.svg"),tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(onQuitAct()));

    dbManAct = new QAction(tr("DB &Manager"), this);
    dbManAct->setStatusTip(tr("Open DB Manager"));
    connect(dbManAct, SIGNAL(triggered()), this, SLOT(openDBManager()));

    tags2DBAct = new QAction(tr("insert &Tags to DB tables"), this);
    tags2DBAct->setStatusTip(tr("insert Tags to DB tables, where tag type = table name."));
    connect(tags2DBAct, SIGNAL(triggered()), tp_, SLOT(insertTags2Table()));
}

void MainWindow::createMenus()
{
    DBMenu = menuBar()->addMenu(tr("&Database"));
    DBMenu->addAction(openAct);
    DBMenu->addAction(closeAct);
    DBMenu->addSeparator();
    DBMenu->addAction(dbManAct);
    DBMenu->addSeparator();
    DBMenu->addAction(tags2DBAct);
    DBMenu->addAction(quitAct);

}

void MainWindow::createTrayIcon()
{
    tray_menu_ = new QMenu(this);
    tray_menu_->addAction(quitAct);

    tray_icon_ = new QSystemTrayIcon(this);
    tray_icon_->setIcon(QIcon(":/icons/icon.svg"));
    tray_icon_->setContextMenu(tray_menu_);
    tray_icon_->show();
    connect(tray_icon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::on_topTabWidget_currentChanged(int index)
{
    if ( ui->topTabWidget->widget(index) != file_exp_widget_ ) {
        file_tags_widget_->setHidden(true);
    }
}

void MainWindow::onQuitAct()
{
    // remember current file explorer path
    QSettings sett;
    sett.setValue("file_exp/lastPath", file_exp_widget_->currentPath());
    sett.sync();
    close();
}

void MainWindow::onClipboardChanged()
{
    tray_icon_->showMessage("clipboard",clipboard_->text(),QSystemTrayIcon::Information,1000);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        tray_icon_->contextMenu()->show();
//        tray_icon_->contextMenu()->popup(QCursor::pos());
        break;
    default:
        break;
    }
}
