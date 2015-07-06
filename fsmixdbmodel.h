#ifndef FSMIXDBMODEL_H
#define FSMIXDBMODEL_H

#include <QObject>
#include <QFileSystemModel>
#include "rankpool.h"

class FSmixDBmodel : public QFileSystemModel
{
    Q_OBJECT
public:
    FSmixDBmodel(HashPool *hp,
                 RankPool *rp,
                 QObject *parent);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    enum DBcol {
        DB_RANK_COL = 0,
        DB_MD5_COL
    };

    int colIndex(enum DBcol col) const;
    bool isDBcol(int col) const;

    QList< int > db_col_indiecs_;
    QStringList db_col_names_;
    int fscolnum_;
    HashPool *hp_;
    RankPool *rp_;
};

#endif // FSMIXDBMODEL_H
