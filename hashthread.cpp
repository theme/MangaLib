#include "hashthread.h"

QSemaphore HashThread::sem_(1);

HashThread::HashThread(QString fpath, enum QCryptographicHash::Algorithm algo, QObject *parent):
    QThread(parent), fpath_(fpath), algo_(algo)
{

}

QString HashThread::fpath()
{
    return fpath_;
}

void HashThread::run()
{
    sem_.acquire();
    QFile f(fpath_);
    qint64 size = f.size();
    char buf[BUFSIZE];
    qint64 rc = 0; // read count
    int len;

    if (f.open(QFile::ReadOnly)){
        QCryptographicHash hash(algo_);
        while(!f.atEnd()){
            len = f.read(buf, BUFSIZE);
            hash.addData(buf, len);   //100K
            rc += len;
            emit sigHashingPercent(rc*100/size,fpath_);
        }
        hash_ = hash.result().toHex().toUpper();
        emit sigHash(hash_, fpath_);
    } else {
        emit sigHashingError(f.errorString(), fpath_);
    }
    f.close();
    sem_.release();
}

