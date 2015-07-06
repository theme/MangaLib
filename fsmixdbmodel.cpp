#include "fsmixdbmodel.h"
#include <QDebug>

FSmixDBmodel::FSmixDBmodel(HashPool *hp, RankPool *rp, QObject *parent):
    QFileSystemModel(parent), rp_(rp), hp_(hp)
{
}

int FSmixDBmodel::rowCount(const QModelIndex &parent) const
{
    return QFileSystemModel::rowCount(parent);
}

int FSmixDBmodel::columnCount(const QModelIndex &parent) const
{
    return QFileSystemModel::columnCount(parent) +2;
}

QVariant FSmixDBmodel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole){
        switch (index.column() - QFileSystemModel::columnCount()){
        case DB_MD5_COL:
            return QVariant(hp_->getFileHash(QCryptographicHash::Md5,this->filePath(index)));
            break;
        case DB_RANK_COL:
            return QVariant(rp_->getFileRank(this->filePath(index)));
            break;
        }
    }
    return QFileSystemModel::data(index, role);
}

QVariant FSmixDBmodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        switch (section - QFileSystemModel::columnCount()){
        case DB_MD5_COL:
            return QVariant(QString("Md5"));
            break;
        case DB_RANK_COL:
            return QVariant(QString("Rank"));
            break;
        }
    }
    return QFileSystemModel::headerData(section, orientation, role);
}

Qt::ItemFlags FSmixDBmodel::flags(const QModelIndex &index) const
{
    switch (index.column() - QFileSystemModel::columnCount()){
    case DB_RANK_COL:
        if ( rp_->isRankable() ){
            return QFileSystemModel::flags(index);
        } else {
            return QFileSystemModel::flags(index) & ~Qt::ItemIsEnabled;
        }
        break;
    default:
        return QFileSystemModel::flags(index);
    }
}

int FSmixDBmodel::colIndex(FSmixDBmodel::DBcol col) const
{
    return QFileSystemModel::columnCount() + col;
}

bool FSmixDBmodel::isDBcol(int col) const
{
    return col < QFileSystemModel::columnCount();
}
