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
    QStringList tgts;

    // rip file extension name
    QRegExp rx(".*(?=(\\.(zip|rar|cb[zr]))$)");
    int pos = 0;
    if ((pos = rx.indexIn(str, pos)) != -1){
        str = rx.cap();
    }
    qDebug() << str;

    // regex section take out
    QStringList expli;
    QMap<int,QString> caps;
    expli << "([^\\[\\]]+)(?=\\])"      // [*]
          << "([^\\(\\)]+)(?=\\))"      // (*)
          << "([^【】]+)(?=】)"      // 【 】
          << "([^（）]+)(?=）)"      // （）
          << "([^\\[\\]\\(\\)【】（）~\\s\\-]+)";
    for ( int i =0; i< expli.size(); ++i){
        rx.setPattern(expli.at(i));
        pos = 0;
        while ((pos = rx.indexIn(str, pos)) != -1) {
            if (!caps.contains(pos)){
                caps.insert(pos,rx.cap());
            }
            pos += rx.matchedLength();
        }
    }
    auto e = caps.begin();
    while( e != caps.end()){
        tgts << e.value();
        ++e;
    }
    qDebug() << tgts;

    // return results
    return tgts;
}

QStringList TagPool::typeOptions() const
{
    QStringList li = db_->schema()->tables();
    li.removeAll("file");
    li.removeAll("rank");
    li.removeAll("tag");
    li.removeAll("link");
    return li;
}

void TagPool::handleTagTypeChange(QString tagName, QString type, QString oldType)
{
    if ( tagName.isEmpty())
        return;
    if (type.isEmpty()){    //remove
        db_->remove("tag", "name", tagName);
        tagcache_.remove(tagName);
    } else {
        QStringList cols, vs;
        cols << "name" << "type";
        vs << tagName <<  type;
        if( db_->hitValue("tag", "name", tagName) ){ // exist, update
            db_->update("tag", cols, vs, "name", tagName );
        } else {
            db_->insert("tag", cols, vs);
        }
    }
}

void TagPool::insertTags2Table()
{
    QSqlQuery q = db_->selectAll("tag");
    QStringList cols, vs;
    while(q.next()){
        cols << "name";
        vs << q.record().value(0).toString();
        db_->insert(q.record().value(1).toString(),cols,vs);
        cols.clear();
        vs.clear();
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
