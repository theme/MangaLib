#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ui: menu
    createActions();
    createMenus();

    // ui: explorer
    current_abs_path_ = QDir::currentPath();
    dir_model_ = new QFileSystemModel(parent);
    files_model_ = new QFileSystemModel(parent);
    dir_model_->setRootPath(QDir::rootPath());
    dir_model_->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    files_model_->setRootPath(current_abs_path_);
    files_model_->setFilter(QDir::Files);
    files_model_->setNameFilters(QStringList()
                                 << "*.zip"
                                 << "*.cb?"
                                 << "*.rar");
    files_model_->setNameFilterDisables(true);

    ui->dirView->setModel(dir_model_);
    ui->filesView->setModel(files_model_);

    dir_selection_model_ = ui->dirView->selectionModel();

    connect(dir_selection_model_, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onDirSelectChanged(QModelIndex,QModelIndex)));
    connect(ui->pathEdit, SIGNAL(editingFinished()),
            this, SLOT(onUIPathEdited()));
    connect(this, SIGNAL(sigCurrentAbsPath(QString)),
            this, SLOT(setCurrentAbsPath(QString)));
    connect(this, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    // Database State Machine
    dbsm_ = new DBSM(parent);
    connect(this, SIGNAL(sigOpenDBFile(QString)),
            dbsm_, SLOT(openDBFile(QString)));
    connect(dbsm_, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    // ui: Library ( DB ) View
    connect(dbsm_, SIGNAL(sigOnline()), this, SLOT(enableLibView()));
    connect(dbsm_, SIGNAL(sigOffline()), this, SLOT(disableLibView()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDirSelectChanged(QModelIndex current, QModelIndex previous)
{
    setCurrentAbsPath( dir_model_->filePath(dir_selection_model_->currentIndex()) );
    ui->dirView->resizeColumnToContents(current.column());
}

void MainWindow::setCurrentAbsPath(QString absPath)
{
    current_abs_path_ = absPath;
    ui->dirView->setCurrentIndex(dir_model_->index(current_abs_path_));
    ui->filesView->setRootIndex(files_model_->setRootPath(current_abs_path_));
    ui->pathEdit->setText(current_abs_path_);
}

void MainWindow::onUIPathEdited()
{
    if(dir_model_->index(ui->pathEdit->text()).isValid()){
        emit sigCurrentAbsPath(ui->pathEdit->text());
    }
}

void MainWindow::onDBError(QString what, QString why)
{
    QMessageBox::critical(this, what, why);
}

void MainWindow::enableLibView()
{
    ui->tablesTabWidget->setEnabled(true);
}

void MainWindow::disableLibView()
{
    ui->tablesTabWidget->setDisabled(true);
}

void MainWindow::openDBFile()
{
    QString selfilter = tr("DB Files (*.db)");  // selection filter
    QString fn = QFileDialog::getSaveFileName(
                            this,
                            tr("Choose DB file"), current_abs_path_, tr("DB Files (*.db)"),
                            &selfilter, QFileDialog::DontConfirmOverwrite);
    emit sigOpenDBFile(fn); // handle by DBSM
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open DB File"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a DB file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDBFile()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);
}
