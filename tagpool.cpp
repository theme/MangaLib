#include "tagpool.h"

TagPool::TagPool(SQLiteDB *db, QObject *parent) :
    QObject(parent),db_(db)
{
}

QString TagPool::type(QString tag)
{
    if (tp_.contains(tag)){
        return tp_.value(tag);
    } else {
        return queryType(tag);
    }
}

QString TagPool::queryType(QString tag)
{
    if (!db_->isOpen())
        return QString();

    return db_->whichTableContains(tag);
}
