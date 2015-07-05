#include "tagpool.h"

TagPool::TagPool(SQLiteDB *db, QObject *parent) :
    QObject(parent),db_(db)
{
}

QString TagPool::type(QString tag)
{
    if (tcache_.contains(tag)){
        return tcache_.value(tag);
    } else {
        return queryType(tag);
    }
}

QStringList TagPool::tagsInString(QString str) const
{
    QStringList sps = QString("[ ] ( ) 「 」 【 】 .").split(' ');
    sps.append(" ");

    QStringList tags, tmps, tgt;
    QString t,s;
    tags.append(str);

    for (int i = 0; i< sps.size(); ++i){
        s = sps.at(i);
        // process tag list
        for (int j=0; j< tags.size(); ++j){
            t = tags.at(j);
            if (t.isEmpty())
                continue;
            // split
            tmps.append(t.split(s));
        }

        tags.clear();
        for (int j=0; j<tmps.size(); ++j){
            t = tmps.at(j);
            if (tcache_.contains(t)){   // hit cache
                tgt.append(t);
            } else {
                tags.append(t); // next round
            }
        }
        tmps.clear();
    }

    tgt.append(tags);

    // debug
    for (int k=0; k < tgt.size(); ++k){
        qDebug() <<"... "<<  tgt.at(k);
    }
    return tgt;
}

void TagPool::loadDBtags()
{

}

QString TagPool::queryType(QString tag)
{
    if (!db_->isOpen())
        return QString();

    return db_->whichTableContains(tag);
}
