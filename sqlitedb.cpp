#include "sqlitedb.h"

SQLiteDB::SQLiteDB(const DBSchema *schema,
                   QObject *parent) :
    QObject(parent), dbschema_(schema)
{
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

QStringList SQLiteDB::fields(QString tableName) const
{
    QStringList dbfields;
    QString sql("PRAGMA table_info( " + tableName + ")");
    QSqlQuery q(db_);
    if (!q.exec(sql)){
        QString msg = "query table_info error. |"+tableName +"|"+ q.lastError().text();
        qDebug() << msg;
        return QStringList();
    }
    while(q.next()){
        QSqlRecord rec = q.record();
        dbfields.append(rec.value(1).toString());
    }
    return dbfields;
}

QString SQLiteDB::type(QString tableName, QString fieldName) const
{
    QString sql("PRAGMA table_info( " + tableName + ")");
    QSqlQuery q(db_);
    if (!q.exec(sql)){
        QString msg = "query table_info error. |"+tableName +"|"+ q.lastError().text();
        qDebug() << msg;
        return QString();
    }
    QSqlRecord rec;
    while(q.next()){
        rec = q.record();
        if ( fieldName == rec.value(1).toString() ){
            return rec.value(2).toString();
        }
    }
    return QString();
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

    // ensure we have needed tables
    QStringList tables = db_.tables();
    QStringList schema_tables = dbschema_->tables();
    while (!schema_tables.isEmpty()){
        QString stn = schema_tables.takeFirst();
        if (!tables.contains(stn,Qt::CaseInsensitive)){ // need table
            QStringList fli = dbschema_->fields(stn);
            QSqlQuery q(db_);
            QString sql("CREATE TABLE ");
            sql += stn + "(";
            for (int i = 0; i < fli.size(); ++i){
                sql += fli.at(i) + " " + dbschema_->type(stn,fli.at(i));
                if (i+1 < fli.size()){
                    sql += ", ";
                } else {
                    sql += ")";
                }
            }
            qDebug() << sql;
            if (!q.exec(sql)){
                QString msg = "create table error. |"+stn +"|"+ q.lastError().text();
                emit sigStatusMsg(msg);
                qDebug()  << msg;
                qDebug() << q.lastError();
                return q.lastError();
            }
        } else {    // table already exist, ensure fields
            QStringList dbfields;
            QStringList dbfieldtype;
            // get fields in db table
            QString sql("PRAGMA table_info( " + stn + ")");
            QSqlQuery q(db_);
            if (!q.exec(sql)){
                QString msg = "query table_info error. |"+stn +"|"+ q.lastError().text();
                qDebug() << msg;
                return q.lastError();
            }
            while(q.next()){
                QSqlRecord rec = q.record();
                dbfields.append(rec.value(1).toString());
                dbfieldtype.append(rec.value(2).toString());
            }
            q.finish();
            // alter tabel, insert missing columns
            QStringList schemafields = dbschema_->fields(stn);
            for( int i = 0 ; i< schemafields.size(); ++i) {
                QString fieldname = schemafields.at(i);
                if(!dbfields.contains(fieldname)){
                    sql = "ALTER TABLE "+ stn + " ADD COLUMN ";
                    sql += fieldname + " " + dbschema_->type(stn, fieldname);
                    if (!q.exec(sql)){
                        QString msg = "db add column error. |"+ stn +"| "
                                + fieldname + " " + q.lastError().text();
                        qDebug() << msg;
                        return q.lastError();
                    }
                    q.finish();
                }
            }
            // now we have all columns
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
