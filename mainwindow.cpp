#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    hp_ = new HashPool(this);
    db_ = new SQLiteDB(":/dbschema.json", this);
    tp_ = new TagPool(db_,this);

    // ui::menu
    createActions();
    createMenus();

    // ui: File explorer
    FSmixDBmodel *fmixd_;
    fmixd_ = new FSmixDBmodel(db_, hp_, this);
    fmixd_->setRootPath(QDir::rootPath());
    fmixd_->setFilter(QDir::Files);
    fmixd_->setNameFilters(QStringList()
                                 << "*.zip"
                                 << "*.cb?"
                                 << "*.rar");
    fmixd_->setNameFilterDisables(false);
//    fmixd_->setReadOnly(false);
    file_exp_widget_ = new FileExplorer(fmixd_, this);
    ui->topTabWidget->addTab(file_exp_widget_, "&Explorer");

    connect(this, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    // ui: File info
    file_info_widget_ = new FileInfoWidget(db_, hp_, tp_, this);
    file_exp_widget_->layout()->addWidget(file_info_widget_);
//    ui->topTabWidget->addTab(file_info_widget_, "&File info");
    connect(file_exp_widget_, SIGNAL(sigFilePath(QString)),
            file_info_widget_, SLOT(setFile(QString)));

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
    if( QFile::exists(QDir::homePath() + "/mangalib.db")){
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

void MainWindow::saveLocalFileInfo()
{
//    QSqlQuery q(db_);

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

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

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
