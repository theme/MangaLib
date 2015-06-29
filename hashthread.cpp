#include "hashthread.h"

HashThread::HashThread(QString fpath, enum QCryptographicHash::Algorithm algo, QObject *parent):
    QThread(parent), fpath_(fpath), algo_(algo)
{
}

void HashThread::run()
{
    QFile f(fpath_);
    if (f.open(QFile::ReadOnly)){
        QCryptographicHash hash(algo_);
        while(!f.atEnd()){
            hash.addData(f.read(102400));   //100K
            if(this->isInterruptionRequested()){
                goto end;
            }
        }
        hash_ = hash.result().toHex().toUpper();
        emit sigHash(hash_, fpath_);
    } else {
        emit sigHashError(f.errorString(), fpath_);
    }
    end:
        f.close();
}

