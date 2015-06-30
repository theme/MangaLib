#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        hash_thread_(0)
{
    ui->setupUi(this);
    ui->fileInfoWidget->hide();

    // ui: menu
    createActions();
    createMenus();

    // ui: explorer
    sc_editpath_ = new QShortcut(QKeySequence(tr("Ctrl+L", "Edit path")),
                                 this);
    connect(sc_editpath_, SIGNAL(activated()), ui->pathEdit, SLOT(setFocus()));
    current_abs_path_ = QDir::currentPath();
    dir_model_ = new QFileSystemModel(this);
    files_model_ = new QFileSystemModel(this);
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
    connect(ui->filesView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(showFileInfo(QModelIndex)));
    connect(ui->filesView, SIGNAL(sigFocusOut()),this, SLOT(onFileFocusOut()));
    connect(ui->pathEdit, SIGNAL(editingFinished()),
            this, SLOT(onUIPathEdited()));
    connect(this, SIGNAL(sigCurrentAbsPath(QString)),
            this, SLOT(setCurrentAbsPath(QString)));
    connect(this, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    // DB state machine
    dbsm_ = new QStateMachine(this);
    state_opened_ = new QState(dbsm_);
    state_closed_ = new QState(dbsm_);
    dbsm_->setInitialState(state_closed_);
    state_closed_->addTransition(this, SIGNAL(sigDBopened()), state_opened_);
    state_opened_->addTransition(this, SIGNAL(sigDBclosed()), state_closed_);
    dbsm_->start();

    // ui: Library ( DB ) View
    connect(state_opened_, SIGNAL(entered()), this, SLOT(loadDBTabs()));
    state_opened_->assignProperty(closeAct, "enabled", true);
    connect(state_closed_, SIGNAL(entered()), this, SLOT(removeDBTabs()));
    state_closed_->assignProperty(closeAct, "enabled", false);
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

void MainWindow::showFileInfo(QModelIndex index)
{
    if (!index.isValid()){
        ui->fileInfoWidget->hide();
    }
    else {
        QString fpath = files_model_->fileInfo(index).filePath();
        QFileInfo fi(fpath);
        if (fi.isFile()){
            ui->fileInfoWidget->show();
            ui->fhash->setText(getHash(fpath));
        }
    }
}

void MainWindow::onFileFocusOut()
{
    ui->fileInfoWidget->hide();
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

QString MainWindow::getHash(QString fpath)
{
    if (!hash_cache_.contains(fpath)) {
        hash_thread_ = new HashThread(fpath,QCryptographicHash::Md5,this);

        connect(hash_thread_, SIGNAL(finished()),
                hash_thread_, SLOT(deleteLater()));
        connect(hash_thread_, SIGNAL(sigHash(QString,QString)),
                this, SLOT(updateFileHash(QString,QString)));
        connect(hash_thread_, SIGNAL(sigHashError(QString,QString)),
                this, SIGNAL(sigStatusMsg(QString)));

        hash_cache_.insert(fpath, "Hashing...");// Necessary ( guard too many threads )
        hash_thread_->start();
    }
    return hash_cache_.value(fpath);
}

void MainWindow::updateFileHash(QString hash, QString fpath)
{
    hash_cache_.insert(fpath, hash);
}

void MainWindow::clearCache()
{
    hash_cache_.clear();
}

void MainWindow::onDBError(QString what, QString why)
{
    QMessageBox::critical(this, what, why);
}

void MainWindow::loadDBTabs()
{
    QStringList tables = db_.tables();
    for( int i = 0; i < tables.size(); ++i){
        DBTableWidget* w = new DBTableWidget(tables.at(i),
                                               db_,
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

QSqlError MainWindow::openDB(QString fn)
{
    if( fn.isEmpty() ){
        QString selfilter = tr("DB Files (*.db)");
        fn = QFileDialog::getSaveFileName( this,
                                           tr("Choose DB file"),
                                           current_abs_path_,
                                           tr("DB Files (*.db)"),
                                           &selfilter,
                                           QFileDialog::DontConfirmOverwrite );
    }

    if (fn.isEmpty()){
        emit sigStatusMsg(tr("no file selected."));
        return QSqlError();
    }

    qDebug() << "openDB() fn: " << fn;

    if (db_.isValid()){
        qDebug() << "openDB() when db_.isValid () : db_.connectionName()" << db_.connectionName();
        if (fn == db_.connectionName()){ // same db
            emit sigStatusMsg(tr("same DB file, do nothing."));
            return QSqlError();
        }
        else {
            this->closeDB();
        }
    }

    emit sigStatusMsg(tr("(open DB file ...) ") + fn);

    // open DB
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        return QSqlError("no SQLite driver", "unable to load database",
                         QSqlError::UnknownError);
    }

    db_ = QSqlDatabase::addDatabase("QSQLITE", fn);
    db_.setDatabaseName(fn);


    if (!db_.open()){
        return db_.lastError();
    }

    emit sigStatusMsg(tr("(DB file opened) fn: ") + fn);

    // ensure we have needed table
    QStringList tables = db_.tables();
    if (tables.contains("books", Qt::CaseInsensitive)
                    && tables.contains("authors", Qt::CaseInsensitive)
                    && tables.contains("files", Qt::CaseInsensitive))
    {
        emit sigStatusMsg("(DB tables all right) fn: " + fn);
        emit sigDBopened();
        return QSqlError();  // NO ERROR
    }

    emit sigStatusMsg("(creating tables in DB...) fn: " + fn);
    QSqlQuery q(db_);
    if (!q.exec(QLatin1String("create table books("      // books
                              "isbn integer,"
                              "title varchar, "
                              "author varchar, "
                              "year integer, "
                              "rank integer)")))
    {
        emit sigStatusMsg("create table books error. | "+ q.lastError().text());
        return q.lastError();
    }
    if (!q.exec(QLatin1String("create table authors("    // authors
                              "realname varchar,"
                              "penname varchar,"
                              "circle varchar,"
                              "country varchar,"
                              "location varchar)")))
    {
        emit sigStatusMsg("create table authors error. | "+ q.lastError().text());
        return q.lastError();
    }
    if (!q.exec(QLatin1String("create table files("  // files
                              "fn varchar,"
                              "md5 varchar,"
                              "sha1 varchar)")))
    {
        emit sigStatusMsg("create table files error. | "+ q.lastError().text());
        return q.lastError();
    }
    emit sigStatusMsg("(DB ready.) fn: " + fn);
    emit sigDBopened();
    return QSqlError();
}

void MainWindow::closeDB()
{
    if (db_.isValid()){
        db_.close();
        db_.removeDatabase(db_.connectionName());
        emit sigDBclosed();
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open DB file"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a DB file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openDB()));

    closeAct = new QAction(tr("&Close DB file"), this);
    closeAct->setShortcut(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close current DB file"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeDB()));

    clearCacheAct = new QAction(tr("Clea&r cache"), this);
    clearCacheAct->setShortcut(QKeySequence::Close);
    clearCacheAct->setStatusTip(tr("clear all caches"));
    connect(clearCacheAct, SIGNAL(triggered()), this, SLOT(clearCache()));

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
    DBMenu->addAction(clearCacheAct);
    DBMenu->addSeparator();
    DBMenu->addAction(quitAct);
}
