#include "rankpool.h"

RankPool::RankPool(SQLiteDB *db, HashPool *hp, QObject *parent) :
    QObject(parent),
    db_(db),hp_(hp)
{
    connect( db_, SIGNAL(sigOpened()), this, SLOT(loadRankFromDB()));
    connect( db_, SIGNAL(sigClosed()), this, SLOT(clearCache()));
}

int RankPool::getFileRank(QString fpath)
{
    QFileInfo fi(fpath);
    if (!fi.isFile()){
        return -1;
    }

    QString md5 = hp_->getFileHash(QCryptographicHash::Md5,fpath);

    QString key = cacheKeyFile(fi.size(), md5);
    if (rank_cache_.contains(key)){
        return rank_cache_.value(key);
    }

    QStringList cols, vs;
    cols << "size";
    vs << QString::number(fi.size());
    if (!md5.isEmpty()){
        cols << "md5";
        vs << md5;
    }
    int rank = db_->query1valueInt("rank","rank",cols,vs);
    if (rank >= 0){
        rank_cache_.insert(key,rank);
    }
    return rank;
}

bool RankPool::setRank(QString fpath, int rank)
{
    QFileInfo fi(fpath);
    if (!fi.isFile()){
        return false;
    }

    QString md5 = hp_->getFileHash(QCryptographicHash::Md5, fpath);
    if (md5.isEmpty()){
        qDebug() << "RankPool::setRank(): md5 needed";
        return false;
    }

    QStringList cols, vs;
    cols << "md5" << "size";
    vs << md5 << QString::number(fi.size());

    if( getFileRank(fpath) < 0){    // net exist, insert
        cols << "rank" << "name" << "type";
        vs << QString::number(rank) << fi.fileName() << "file";
        return db_->insert("rank", cols, vs);
    } else {    // update
        return db_->update("rank", cols, vs, "rank", QString::number(rank));
    }
}

bool RankPool::isRankable() const
{
    return db_->isOpen();
}

void RankPool::loadRankFromDB()
{
    QSqlQuery q = db_->select("rank");
    while(q.next()){
        if ( q.value("type") == "file"){
            rank_cache_.insert(cacheKeyFile(q.value("size").toInt(),
                                            q.value("md5").toString()),
                               q.value("rank").toInt());
        } else {
            rank_cache_.insert(q.value("type").toString()+q.value("name").toString(),
                               q.value("rank").toInt());
        }
    }
}

void RankPool::clearCache()
{
    rank_cache_.clear();
}

QString RankPool::cacheKeyFile(int size, QString md5)
{
    return "file"+ QString::number(size)+ md5;
}
