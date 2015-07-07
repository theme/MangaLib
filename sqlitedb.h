#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include "dbschema.h"

class SQLiteDB : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDB(QString schemafile,
                      QObject *parent = 0);
    bool isOpen() const;

    QStringList tables() const;
    QStringList fields(QString tableName) const;
    QString fieldType(QString tableName, QString fieldName) const;

    QSqlRecord query1record(QString tn, QString where_col, QString where_v) const;
    QString query1valueString(QString table, QString filed, QStringList where_cols, QStringList where_vs)const;
    int query1valueInt(QString table, QString filed, QStringList where_cols, QStringList where_vs)const;
    bool hitValue(QString tn, QString where_col, QString where_v) const;
    QString whichTableContainsName(QString name) const;
    QStringList allTableNameDotValuesOfField(QString fieldName,
                                             QString exceptTable = QString()) const;

    QSqlQuery select(QString tableName,
                     const QStringList& cols = QStringList(),
                     const QStringList& vs = QStringList()) const;
    bool insert(QString tn, const QStringList& cols, const QStringList& vs);
    bool update(QString tn, const QStringList& cols, const QStringList& vs,
                QString where_key, QString where_v);
    bool remove(QString tn, const QStringList& cols, const QStringList& vs);
    bool removeAll(QString tn, QString fieldName, QString v);

    QSqlDatabase &conn();
    const DBSchema *schema() const;

signals:
    void sigOpened(bool is = true);
    void sigClosed(bool is = true);
    void sigStatusMsg(QString msg);

public slots:
    QSqlError open(QString fn);
    void closeDB();

private slots:

private:
    const DBSchema *dbschema_;
    QSqlDatabase db_;
    QStateMachine* dbsm_;
    QState *state_opened_;
    QState *state_closed_;
};

#endif // SQLITEDB_H
