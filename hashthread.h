#ifndef HASHTHREAD_H
#define HASHTHREAD_H

#include <QObject>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QThread>

class HashThread : public QThread
{
    Q_OBJECT
public:
    HashThread(QString fpath,
               enum QCryptographicHash::Algorithm algo,
               QObject *parent = 0);
signals:
    void sigHash(QString hashString, QString fpath);
    void sigHashError(QString errorString, QString fpath);

private:
    void run();
    QString fpath_;
    enum QCryptographicHash::Algorithm algo_;
    QString hash_;
};

#endif // HASHTHREAD_H
