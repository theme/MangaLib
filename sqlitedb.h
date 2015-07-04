#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include "dbschema.h"

class SQLiteDB : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDB(const DBSchema *schema,
                      QObject *parent = 0);
    QStringList tables() const;
    QStringList fields(QString tableName) const;
    QString type(QString tableName, QString fieldName) const;

    QSqlRecord query1record(QString tn, QString col, QString v) const;

    bool insert(QString tn, const QStringList& cols, const QStringList& vs);
    bool update(QString tn, const QStringList& cols, const QStringList& vs, QString key, QString v);

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
