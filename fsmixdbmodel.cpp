#include "fsmixdbmodel.h"
#include <QDebug>

FSmixDBmodel::FSmixDBmodel(SQLiteDB *db, HashPool *hp, QObject *parent):
    QFileSystemModel(parent),db_(db), hp_(hp)
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
        switch (index.column()){
        case 4:
            return QVariant(hp_->getFileHash(QCryptographicHash::Md5,this->filePath(index)));
            break;
        case 5:
            return QVariant();
            break;
        }
    }
    return QFileSystemModel::data(index, role);
}

QVariant FSmixDBmodel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole){
        switch (section){
        case 4:
            return QVariant(QString("md5"));
            break;
        case 5:
            return QVariant(QString("DB rank"));
            break;
        }
    }
    return QFileSystemModel::headerData(section, orientation, role);
}

Qt::ItemFlags FSmixDBmodel::flags(const QModelIndex &index) const
{
    switch (index.column()){
    case 4:
        if ( db_->isOpen() ){
            return QFileSystemModel::flags(index);
        } else {
            return QFileSystemModel::flags(index) & ~Qt::ItemIsEnabled;
        }
        break;
    default:
        return QFileSystemModel::flags(index);
    }
}
