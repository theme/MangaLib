#include "hashthread.h"

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
    QFile f(fpath_);
    if (f.open(QFile::ReadOnly)){
        emit sigHashError(tr("Hashing..."), fpath_);
        QCryptographicHash hash(algo_);
        while(!f.atEnd()){
            hash.addData(f.read(102400));   //100K
        }
        hash_ = hash.result().toHex().toUpper();
        emit sigHash(hash_, fpath_);
    } else {
        emit sigHashError(f.errorString(), fpath_);
    }
    f.close();
}

