#ifndef DBSM_H
#define DBSM_H

/* states:
 * 1. s_offline ( no db opened )
 *      s_offline_no_file
 *      s_offline_got_file
 *
 * 2. s_online ( db opened, ready to write )
 */

#include <QStateMachine>
#include <QState>
#include <QtSql>
#include <QFile>

class DBSM : public QStateMachine
{
    Q_OBJECT
public:
    explicit DBSM(QObject *parent = 0);
    QSqlDatabase& getDB();

signals:
    void sigStatusMsg(QString, int t = 0);
    void sigError(QString what, QString why);
    void sigDBopened();
    void sigDBclosed();
    void sigOnline();
    void sigOffline();

public slots:
    QSqlError openDB(QString fname);
    void closeDB();

private:
    QState* s_offline_;
    QState* s_online_;

    QSqlDatabase db_;
};

#endif // DBSM_H
