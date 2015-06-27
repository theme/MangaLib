#include "dbsm.h"

DBSM::DBSM(QObject *parent) :
    QStateMachine(parent),
    dbf_(new QFile(parent))
{
    // new states
    s_offline_ = new QState();
    s_offline_no_file_ = new QState(s_offline_);
    s_offline_got_file_ = new QState(s_offline_);
    s_online_ = new QState();

    // set transition on states
    s_offline_no_file_->addTransition(this, SIGNAL(sigGotDBFile(QString)),
                                      s_offline_got_file_);

    connect( s_offline_got_file_, SIGNAL(entered()), this, SLOT(initDB()));

    s_offline_got_file_->addTransition(this, SIGNAL(sigInitOK()), s_online_);

    // output status signal
    connect(s_online_, SIGNAL(entered()), this, SIGNAL(sigOnline()));
    connect(s_offline_, SIGNAL(entered()), this, SIGNAL(sigOffline()));

    // assemble SM
    this->addState(s_offline_);
    s_offline_->setInitialState(s_offline_no_file_);
    this->addState(s_online_);
    this->setInitialState(s_offline_);

    this->start();
}

QSqlDatabase &DBSM::getDB()
{
    return db_;
}

void DBSM::openDBFile(QString fpath)
{
    dbf_->setFileName(fpath);

    if (!dbf_->open(QIODevice::ReadWrite)){
        dbf_->close();
        emit sigStatusMsg(tr("Can not read write DB file") + dbf_->fileName());
    } else {
        dbf_->close();
        emit sigStatusMsg(tr("opened DB file") + dbf_->fileName());
        emit sigGotDBFile(dbf_->fileName());
    }
}

QSqlError DBSM::initDB()
{
    emit sigStatusMsg(tr("(init DB) ") + dbf_->fileName());

    // open DB
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        emit sigError("Unable to load database", "no SQLITE driver");
        return QSqlError("no SQLite driver",
                         "unable to load database",
                         QSqlError::UnknownError);
    }

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(dbf_->fileName());

    if (!db_.open()){
        emit sigError("db open failed", "Unknown why");
       return db_.lastError();
    }

    emit sigStatusMsg(tr("(open DB) ") + dbf_->fileName());
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
    QStringList tables = db_.tables();
    if (tables.contains("books", Qt::CaseInsensitive)
       && tables.contains("authors", Qt::CaseInsensitive)
       && tables.contains("files", Qt::CaseInsensitive))
    {
        emit sigInitOK();
        emit sigStatusMsg("(db online) " + dbf_->fileName());
       return QSqlError();  // NO ERROR
    }

    emit sigStatusMsg("creating tables in DB...");
    QSqlQuery q(db_);
    if (!q.exec(QLatin1String("create table books("      // books
                             "id integer primary key, "
                             "isbn integer,"
                             "title varchar, "
                             "author integer, "
                             "year integer, "
                             "rank integer)")))
    {
        emit sigStatusMsg("create table books error. | "+ q.lastError().text());
       return q.lastError();
    }
    if (!q.exec(QLatin1String("create table authors("    // authors
                             "id integer primary key, "
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
                             "id integer primary key,"
                             "fn varchar,"
                             "sha1 varchar,"
                             "md5 varchar)")))
    {
        emit sigStatusMsg("create table files error. | "+ q.lastError().text());
       return q.lastError();
    }

    emit sigStatusMsg("init DB ok.");
    emit sigInitOK();
    return QSqlError();
}
