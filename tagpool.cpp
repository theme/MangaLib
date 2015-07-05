#include "tagpool.h"

TagPool::TagPool(SQLiteDB *db, QObject *parent) :
    QObject(parent),db_(db)
{
    connect(db_, SIGNAL(sigOpened()),
            this, SLOT(loadDBtags()));
}

QString TagPool::tagType(QString tag)
{
    if (tagcache_.contains(tag)){
        return tagcache_.value(tag);
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
            if (tagcache_.contains(t)){   // hit cache
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

QStringList TagPool::typeOptions() const
{
    QStringList li = db_->schema()->tables();
    li.removeAll("file");
    return li;
}

void TagPool::handleTagTypeChange(QString tagName, QString type, QString oldType)
{
    QStringList cols, vs;
    cols << "name" << "type";
    vs << tagName <<  type;
    if( db_->hitValue("tag", "name", tagName) ){ // update
        db_->update("tag", cols, vs, "name", tagName );
    } else { // insert
        db_->insert("tag", cols, vs);
    }
}

void TagPool::loadDBtags()
{
    QStringList names = db_->allTableNameDotValuesOfField("name", "file");
    QString ndv, n, v;
    int pos;
    for (int i = 0; i < names.size(); ++i){
        ndv = names.at(i);
        pos = ndv.indexOf(".");
        n  = ndv.left(pos);
        v = ndv.mid(pos+1);
        tagcache_.insert(v, n);
    }

    QSqlQuery q = db_->selectAll("tag");
    while(q.next()){
        tagcache_.insert(q.record().value(0).toString(), q.record().value(1).toString());
    }
}

QString TagPool::queryType(QString tag)
{
    if (!db_->isOpen())
        return QString();

    return db_->whichTableContainsName(tag);
}
