#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QObject>
#include <QtSql>
#include "dbschema.h"

class SQLiteDB : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDB(DBSchema const *schema,
                      QObject *parent = 0);
    QStringList tables() const;
    QSqlDatabase &connection();

signals:
    void sigOpened(bool is = true);
    void sigClosed(bool is = true);
    void sigStatusMsg(QString msg);

public slots:
    QSqlError open(QString fn);
    void closeDB();

private slots:

private:
    DBSchema const *dbschema_;
    QSqlDatabase db_;
    QStateMachine* dbsm_;
    QState *state_opened_;
    QState *state_closed_;
};

#endif // SQLITEDB_H
