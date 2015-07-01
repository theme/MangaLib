#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ui::menu
    createActions();
    createMenus();

    // ui: File explorer
    file_exp_widget_ = new FileExplorer(this);
    ui->topTabWidget->addTab(file_exp_widget_, "&Explorer");

    connect(this, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    // ui: File info
    file_info_widget_ = new FileInfoWidget(this);
    ui->topTabWidget->addTab(file_info_widget_, "&File info");
    connect(file_exp_widget_, SIGNAL(sigFilePath(QString)),
            file_info_widget_, SLOT(setFile(QString)));

    // DB
    db_ = new SQLiteDB(parent);

    // ui: Library ( DB ) View
    connect(db_, SIGNAL(sigOpened()), this, SLOT(loadDBTabs()));
    connect(db_, SIGNAL(sigOpened(bool)), closeAct, SLOT(setEnabled(bool)));
    connect(db_, SIGNAL(sigClosed()), this, SLOT(removeDBTabs()));
    connect(db_, SIGNAL(sigClosed(bool)), closeAct, SLOT(setDisabled(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDBfile()
{
    QString selfilter = tr("DB Files (*.db)");
    QString fn = QFileDialog::getSaveFileName( this,
                                               tr("Choose DB file"),
                                               QDir::homePath(),
                                               tr("DB Files (*.db)"),
                                               &selfilter,
                                               QFileDialog::DontConfirmOverwrite );

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
    QStringList tables = db_->tables();
    for( int i = 0; i < tables.size(); ++i){
        DBTableWidget* w = new DBTableWidget(tables.at(i),
                                               db_->connection(),
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
}

void MainWindow::createMenus()
{
    DBMenu = menuBar()->addMenu(tr("&Database"));
    DBMenu->addAction(openAct);
    DBMenu->addAction(closeAct);
    DBMenu->addSeparator();
    DBMenu->addAction(quitAct);
}
