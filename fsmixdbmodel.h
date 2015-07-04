#ifndef FSMIXDBMODEL_H
#define FSMIXDBMODEL_H

#include <QObject>
#include <QFileSystemModel>
#include "sqlitedb.h"

class FSmixDBmodel : public QFileSystemModel
{
    Q_OBJECT
public:
    FSmixDBmodel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // DB
    void setDB(SQLiteDB* db);
private:
    QList< int > db_col_indiecs_;
    QStringList db_col_names_;
    int fscolnum_;
    SQLiteDB *db_;
};

#endif // FSMIXDBMODEL_H
