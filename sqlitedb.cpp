#include "sqlitedb.h"

SQLiteDB::SQLiteDB(QObject *parent) :
    QObject(parent)
{
    // DB schema
    dbschema_.parseJsonFile(":/dbschema.json");

    // DB state machine
    dbsm_ = new QStateMachine(this);
    state_opened_ = new QState(dbsm_);
    state_closed_ = new QState(dbsm_);
    dbsm_->setInitialState(state_closed_);
    state_closed_->addTransition(this, SIGNAL(sigOpened()), state_opened_);
    state_opened_->addTransition(this, SIGNAL(sigClosed()), state_closed_);
    dbsm_->start();
}

QStringList SQLiteDB::tables() const
{
    return db_.tables();
}

QSqlDatabase &SQLiteDB::connection()
{
    return db_;
}

QSqlError SQLiteDB::open(QString fn)
{
    qDebug() << "SQLiteDB::open() " << fn;

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
    emit sigOpened();
    return QSqlError();
}

void SQLiteDB::closeDB()
{
    if (db_.isValid()){
        db_.close();
        db_.removeDatabase(db_.connectionName());
        emit sigClosed();
    }
}
