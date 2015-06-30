#ifndef DBSCHEMA_H
#define DBSCHEMA_H

#include <QtDebug>

#include <QObject>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class DBSchema : public QObject
{
    Q_OBJECT
public:
    explicit DBSchema(QObject *parent = 0);
    QStringList tables() const;
    bool isReady() const;
    QString createTableSql(QString tableName) const;

signals:

public slots:
    bool parseJsonFile(QString file);
private:
    QJsonDocument jsondoc;
};

#endif // DBSCHEMA_H
