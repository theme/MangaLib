#include "hashpool.h"

HashPool::HashPool(QObject *parent) : QObject(parent)
{

}

QString HashPool::getFileHash(QCryptographicHash::Algorithm algo, QString fpath)
{
    if (!file_hashes_.contains(algo+fpath)) {
        HashThread *t = new HashThread(algo,fpath,this);
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        connect(t, SIGNAL(sigHash(QString,QString,QString)),
                this, SLOT(cacheFileHash(QString,QString,QString)));

        connect(t, SIGNAL(sigHashingPercent(QString, int,QString)),
                this, SIGNAL(sigHashingPercent(QString, int,QString)));

        file_hashes_.insert(algo+fpath, "");// Necessary ( guard too many threads )
        t->start();
    }
    return file_hashes_.value(algo+fpath);
}

void HashPool::cacheFileHash(QString algo, QString hash, QString fpath)
{
    file_hashes_.insert(algo+fpath, hash);
    emit sigHash(algo, hash, fpath);
}

