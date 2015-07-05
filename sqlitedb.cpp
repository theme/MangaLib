#include "sqlitedb.h"

SQLiteDB::SQLiteDB(QString schemafile,
                   QObject *parent) :
    QObject(parent)
{
    // schema
    dbschema_ = new DBSchema(schemafile,this);

    // DB state machine
    dbsm_ = new QStateMachine(this);
    state_opened_ = new QState(dbsm_);
    state_closed_ = new QState(dbsm_);
    dbsm_->setInitialState(state_closed_);
    state_closed_->addTransition(this, SIGNAL(sigOpened()), state_opened_);
    state_opened_->addTransition(this, SIGNAL(sigClosed()), state_closed_);
    dbsm_->start();
}

bool SQLiteDB::isOpen() const
{
    return db_.isOpen();
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

QString SQLiteDB::fieldType(QString tableName, QString fieldName) const
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

QSqlRecord SQLiteDB::query1record(QString tn, QString col, QString v) const
{
    if ( col.isEmpty() || v.isEmpty() )
        return QSqlRecord();

    QSqlQuery q(db_);
    QString sql = "select * from " + tn + " where " + col + " = " + "'" + v + "'";
    if (!q.exec(sql)){
        QString msg = "error: SQLiteDB::query1record() "+ sql;
        qDebug()  << msg;
        qDebug() << q.lastError().text();
    }
    q.first();
    return q.record();
}

QStringList SQLiteDB::allTableNameDotValuesOfField(QString fieldName, QString exceptTable) const
{
    QStringList names, fields;
    QStringList tables = dbschema_->tables();
    QString tn;
    for (int i = 0 ; i < tables.size(); ++i){
        tn = tables.at(i);
        if ( tn == exceptTable )
            continue;
        fields = dbschema_->fields(tn);
        if (fields.contains(fieldName)){
            QString sql = "SELECT * FROM " + tn;
            QSqlQuery q(db_);
            if (!q.exec(sql)){
                QString msg = "error: SQLiteDB::allNames() "+ sql;
                qDebug()  << msg;
                qDebug() << q.lastError().text();
            }
            while(q.next()){
                QVariant v = q.record().value(fields.indexOf(fieldName));
                QString s = v.toString();
                if (!s.isEmpty())
                    names.append(tn + "." +s);
            }
            q.finish();
        }
    }
    return names;
}

QSqlQuery SQLiteDB::selectAll(QString tableName) const
{
    if ( tableName.isEmpty() )
        return QSqlQuery();

    QSqlQuery q(db_);
    QString sql = "select * from " + tableName;
    if (!q.exec(sql)){
        QString msg = "error: SQLiteDB::selectAll() "+ sql;
        qDebug()  << msg;
        qDebug() << q.lastError().text();
    }
    return q;
}

bool SQLiteDB::hitValue(QString tn, QString col, QString v) const
{
    QSqlRecord rec = this->query1record(tn, col, v);
    return !rec.value(col).toString().isEmpty();
}

QString SQLiteDB::whichTableContainsName(QString name) const
{
    QStringList tables = dbschema_->tables();
    for (int i = 0 ; i < tables.size(); ++i){
        if (hitValue(tables.at(i), "name", name))
            return tables.at(i);
    }
    return QString();
}

bool SQLiteDB::insert(QString tn, const QStringList &cols, const QStringList &vs)
{
    QStringList a, b;
    for (int i = 0; i < cols.size(); ++i){
        a.append("'" + cols.at(i) + "'");
        b.append("'" + vs.at(i) + "'");
    }

    QString sql;
    sql = "INSERT INTO " + tn + " ( " + a.join(" , ") + " ) ";
    sql += "VALUES ( " + b.join(" , ") + " ) ";

    qDebug() << sql;
    QSqlQuery q(db_);
    if (!q.exec(sql)){
        QString msg = "error: SQLiteDB::insert() "+ sql;
            qDebug()  << msg;
            qDebug() << q.lastError().text();
        return false;
    }
    return true;
}

bool SQLiteDB::update(QString tn, const QStringList &cols, const QStringList &vs, QString key, QString v)
{
    QString sql;
    sql = " UPDATE " + tn + " SET ";
    QStringList assigns;
    for (int i =0; i< cols.size(); ++i){
        assigns.append(" '" + cols.at(i) + "' = '" + vs.at(i) + "' ");
    }
    sql += assigns.join(",");
    sql += " WHERE " + key + " = '" + v + "'";

    qDebug() << sql;
    QSqlQuery q(db_);
    if (!q.exec(sql)){
        QString msg = "error: SQLiteDB::update() "+ sql;
            qDebug()  << msg;
            qDebug() << q.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase &SQLiteDB::conn()
{
    return db_;
}

const DBSchema *SQLiteDB::schema() const
{
    return dbschema_;
}

QSqlError SQLiteDB::open(QString fn)
{
    qDebug() << "SQLiteDB::open() " << fn;

    if (db_.isValid()){
        qDebug() << "openDB() when db_.isValid () : db_.connectionName()" << db_.connectionName();
        if (fn == db_.connectionName()){ // same db
            qDebug() << "SQLiteDB::open() same DB file, do nothing.";
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
