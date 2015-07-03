#ifndef HASHTHREAD_H
#define HASHTHREAD_H

#include <QObject>
#include <QThread>
#include <QCryptographicHash>
#include <QFile>
#include <QSemaphore>

class HashThread : public QThread
{
    Q_OBJECT
public:
    HashThread(QCryptographicHash::Algorithm algo,
               QString fpath,
               QObject *parent = 0);
    QString fpath();
signals:
    void sigHash(QString algoName, QString hashString, QString fpath);
    void sigHashingError(QString algoName, QString errorString, QString fpath);
    void sigHashingPercent(QString algoName, int percent, QString fpath);

private:
    void run();
    QString algoName(QCryptographicHash::Algorithm a);
    QString fpath_;
    QCryptographicHash::Algorithm algo_;
    QString hash_;
    static const uint BUFSIZE = 40960; // 40k

    static QSemaphore sem_;
};

#endif // HASHTHREAD_H
