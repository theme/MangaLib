#include "fsmixdbmodel.h"
#include <QDebug>

FSmixDBmodel::FSmixDBmodel(QObject *parent):
    QFileSystemModel(parent)
{
    // how many coloums are there in parent model?
    // 4
    fscolnum_ = QFileSystemModel::columnCount();

    // How many coloums are there in db?
    // By schema

    // What of them do I need in view?
    // book rank, author rank

}

int FSmixDBmodel::rowCount(const QModelIndex &parent) const
{
    return QFileSystemModel::rowCount(parent);
}

int FSmixDBmodel::columnCount(const QModelIndex &parent) const
{
    return QFileSystemModel::columnCount(parent) +1;
}

QVariant FSmixDBmodel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole){
        switch (index.column()){
        case 4:
            return QVariant(QString("rank!?!?"));
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
            return QVariant(QString("rank!?!?"));
            break;
        }
    }
    return QFileSystemModel::headerData(section, orientation, role);
}

Qt::ItemFlags FSmixDBmodel::flags(const QModelIndex &index) const
{
    if ( !false ){ // ! db online
        switch (index.column()){
        case 4:
            return QFileSystemModel::flags(index) & ~Qt::ItemIsEnabled;
        }
    }
    return QFileSystemModel::flags(index);
}

