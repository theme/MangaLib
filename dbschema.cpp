#include "dbschema.h"

DBSchema::DBSchema(QObject *parent) :
    QObject(parent)
{
}

QStringList DBSchema::tables() const
{
    return jsondoc.object().keys();
}

QList<QString> DBSchema::fields(QString tableName) const
{
    QJsonArray table = jsondoc.object().value(tableName).toArray();
    QList< QString > li;
    for (int i = 0; i < table.size(); ++i){
        QJsonObject field = table.at(i).toObject();
        QJsonObject::const_iterator it = field.constBegin();
        li.append( it.key() );
    }

    return li;
}

QString DBSchema::type(QString tableName, QString fieldName) const
{
    QJsonArray table = jsondoc.object().value(tableName).toArray();
    for (int i = 0; i < table.size(); ++i){
        QJsonObject field = table.at(i).toObject();
        QJsonObject::const_iterator it = field.constBegin();
        if (it.key() == fieldName ){
            return it.value().toString();
        }
    }
    return QString();
}

bool DBSchema::isReady() const
{
    return (!jsondoc.isNull()) && (!jsondoc.isEmpty());
}

QString DBSchema::createTableSql(QString tableName) const
{
    QJsonArray table = jsondoc.object().value(tableName).toArray();
    QString sql("create table ");
    sql += tableName + "(";
    for (int i = 0; i < table.size(); ++i){
        QJsonObject field = table.at(i).toObject();
        QJsonObject::const_iterator it = field.constBegin();
        sql += it.key() + " " +it.value().toString();
        if (i+1 < table.size()){
            sql += ", ";
        } else {
            sql += ")";
        }
    }
    qDebug() << sql;
    return sql;
}

bool DBSchema::parseJsonFile(QString file)
{
    QFile loadFile(file);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open db schema file: " + file;
        return false;
    }

    QByteArray jsonData = loadFile.readAll();
    jsondoc = QJsonDocument::fromJson(jsonData);

    return this->isReady();
}
