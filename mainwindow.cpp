#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dir_model_(new QFileSystemModel(parent)),
    files_model_(new QFileSystemModel(parent)),
    current_abs_path_(QDir::currentPath())
{
    ui->setupUi(this);

    createActions();
    createMenus();

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

QSqlError MainWindow::initDB()
{
    QString selfilter = tr("DB Files (*.db)");
    dbfn_.setFileName(QFileDialog::getSaveFileName(this,
        tr("Choose DB file"), current_abs_path_, tr("DB Files (*.db)"),
                      &selfilter, QFileDialog::DontConfirmOverwrite));

    if (!dbfn_.open(QIODevice::ReadWrite)){
        emit sigStatusMsg(tr("Can not open DB file") + dbfn_.fileName());
        dbfn_.close();
        return QSqlError("","can not open db file",
                         QSqlError::UnknownError);
    }

    emit sigStatusMsg(tr("(Database) ") + dbfn_.fileName());

    // open DB
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
            QMessageBox::critical(this, "Unable to load database", "This demo needs the SQLITE driver");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbfn_.fileName());

    if (!db.open())
       return db.lastError();

    // ensure we have needed table
    /*
     * files: record files
     * books: record books
     *
     * 	ISSN: identifies periodical publications such as magazines.
     *        different ISSN is assigned to each media type.
     *        e.g. print ISSN (p-ISSN) and electronic ISSN (e-ISSN or eISSN).
     *  ISMN: identifies musical scores.
     *
     *
     */
    QStringList tables = db.tables();
    if (tables.contains("books", Qt::CaseInsensitive)
       && tables.contains("authors", Qt::CaseInsensitive)
       && tables.contains("files", Qt::CaseInsensitive))
    {
        emit sigStatusMsg(dbfn_.fileName());
       return QSqlError();  // NO ERROR
    }

    emit sigStatusMsg("creating tables in DB...");
    QSqlQuery q;
    if (!q.exec(QLatin1String("create table books("      // books
                             "id integer primary key, "
                             "isbn integer,"
                             "title varchar, "
                             "author integer, "
                             "year integer, "
                             "rank integer)")))
       return q.lastError();
    if (!q.exec(QLatin1String("create table authors("    // authors
                             "id integer primary key, "
                             "realname varchar,"
                             "penname varchar,"
                             "circle varchar,"
                             "country varchar,"
                             "location varchar)")))
       return q.lastError();
    if (!q.exec(QLatin1String("create table files("  // files
                             "id integer primary key,"
                             "fn varchar,"
                             "sha1 varchar,"
                             "md5 varchar)")))
       return q.lastError();

    return QSqlError();
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a DB file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(initDB()));

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
