#ifndef RANKPOOL_H
#define RANKPOOL_H

#include <QObject>
#include "sqlitedb.h"
#include "hashpool.h"

class RankPool : public QObject
{
    Q_OBJECT
public:
    explicit RankPool(SQLiteDB *db,
                      HashPool *hp,
                      QObject *parent = 0);
    int getFileRank(QString fpath);
    // save rank of file into DB
    // limits controled by DB schema
    bool setRank(QString fpath, int rank);

signals:

public slots:

private:
    void loadRankFromDB();
    QString cacheKeyFile(int size, QString md5);
    SQLiteDB * db_;
    HashPool *hp_;
    QHash< QString, int> rank_cache_;
};

#endif // RANKPOOL_H
