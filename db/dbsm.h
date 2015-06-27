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

signals:
    void sigStatusMsg(QString, int t = 0);
    void sigGotDBFile(QString fn);
    void sigError(QString what, QString why);
    void sigInitOK();
    void sigOnline();
    void sigOffline();

public slots:
    QSqlError initDB();
    void openDBFile(QString fpath);

private:
    QState* s_offline_;
    QState*   s_offline_no_file_;
    QState*   s_offline_got_file_;
    QState* s_online_;

    QFile *dbf_;	// db file
    QSqlDatabase db_;
};

#endif // DBSM_H
