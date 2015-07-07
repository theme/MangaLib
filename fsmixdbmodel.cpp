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
        case DB_RANK_COL:
            if (rp_->getFileRank(this->filePath(index))<0)
                return QVariant();
            return QVariant::fromValue(StarRating(rp_->getFileRank(this->filePath(index))));
            break;
        case DB_MD5_COL:
            return QVariant(hp_->getFileHash(QCryptographicHash::Md5,this->filePath(index)));
            break;
        }
    }
    return QFileSystemModel::data(index, role);
}

QVariant FSmixDBmodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        switch (section - QFileSystemModel::columnCount()){
        case DB_RANK_COL:
            return QVariant(QString("Rank"));
            break;
        case DB_MD5_COL:
            return QVariant(QString("Md5"));
            break;
        }
    }
    return QFileSystemModel::headerData(section, orientation, role);
}

Qt::ItemFlags FSmixDBmodel::flags(const QModelIndex &index) const
{
    switch (index.column() - QFileSystemModel::columnCount()){
    case DB_RANK_COL:
        if ( rp_->isReady() && !this->isDir(index)){
            return QFileSystemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled;
        } else {
            return QFileSystemModel::flags(index) & ~Qt::ItemIsEditable;
        }
        break;
    default:
        return QFileSystemModel::flags(index);
    }
}

bool FSmixDBmodel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    StarRating starRating;
    switch (index.column() - QFileSystemModel::columnCount()){
    case DB_RANK_COL:
        starRating = qvariant_cast<StarRating>(value);
        return rp_->setRank(this->filePath(index), starRating.starCount());
    default:
        return QFileSystemModel::setData(index,value,role);
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
