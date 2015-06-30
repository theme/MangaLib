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

    file_exp_ = new FileExplorer(parent);
    ui->topTabWidget->addTab(file_exp_, "&Explorer");

    connect(this, SIGNAL(sigStatusMsg(QString, int)),
            ui->statusBar, SLOT(showMessage(QString,int)));

    // DB schema
    dbschema_.parseJsonFile(":/dbschema.json");

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
    QStringList schema_tables = dbschema_.tables();
    while (!schema_tables.isEmpty()){
        QString stn = schema_tables.takeFirst();
        if (!tables.contains(stn,Qt::CaseInsensitive)){ // need table
            QSqlQuery q(db_);
            if (!q.exec(dbschema_.createTableSql(stn))){
                QString msg = "create table error. |"+stn +"|"+ q.lastError().text();
                emit sigStatusMsg(msg);
                qDebug()  << msg;
                qDebug() << q.lastError();
                return q.lastError();
            }
        }
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
