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
    void sigHash(int algo, QString hashString, QString fpath);
    void sigHashingError(int algo, QString errorString, QString fpath);
    void sigHashingPercent(int algo, int percent, QString fpath);

private:
    void run();
    QString fpath_;
    QCryptographicHash::Algorithm algo_;
    QString hash_;
    static const uint BUFSIZE = 40960; // 40k

    static QSemaphore sem_;
};

#endif // HASHTHREAD_H
