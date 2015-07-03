#include "hashthread.h"

QSemaphore HashThread::sem_(1);

HashThread::HashThread(enum QCryptographicHash::Algorithm algo, QString fpath, QObject *parent):
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
            emit sigHashingPercent(algoName(algo_), rc*100/size,fpath_);
        }
        hash_ = hash.result().toHex().toUpper();
        emit sigHash(algoName(algo_), hash_, fpath_);
    } else {
        emit sigHashingError(algoName(algo_), f.errorString(), fpath_);
    }
    f.close();
    sem_.release();
}

QString HashThread::algoName(QCryptographicHash::Algorithm a)
{
    switch (a) {
    case QCryptographicHash::Md5:
        return "md5";
        break;
    case QCryptographicHash::Sha1:
        return "sha1";
        break;
    case QCryptographicHash::Sha256:
        return "sha256";
        break;
    default:
        break;
    }
}
