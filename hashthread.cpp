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
    int len, p = 0, np = 0;
    if (f.open(QFile::ReadOnly)){
        QCryptographicHash hash(algo_);
        while(!f.atEnd()){
            len = f.read(buf, BUFSIZE);
            hash.addData(buf, len);   //100K
            rc += len;
            np = rc*100/size;
            if ( p != np){
                emit sigHashingPercent(algo_, np,fpath_);
                p = np;
            }
        }
        hash_ = hash.result().toHex().toUpper();
        emit sigHash(algo_, hash_, fpath_);
    } else {
        emit sigHashingError(algo_, f.errorString(), fpath_);
    }
    f.close();
    sem_.release();
}
