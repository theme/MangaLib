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
//    qDebug() << str;

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
    QMap<int,QString>::const_iterator e = caps.begin();
    while( e != caps.end()){
        tgts << e.value();
        ++e;
    }
//    qDebug() << tgts;

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
        this->removeTag(tagName,type);
    } else {
        this->addTag(tagName,type);
    }
}

void TagPool::insertTags2Table()
{
    QSqlQuery q = db_->select("tag");
    QString ttype, tn;
    while(q.next()){
        ttype = q.record().value(1).toString();
        tn = q.record().value(0).toString();
        if (!db_->hitValue(ttype,"name",tn)){
            QStringList cols, vs;
            cols << "name";
            vs << tn;
            db_->insert(ttype,cols,vs);
        }
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

    QSqlQuery q = db_->select("tag");
    while(q.next()){
        tagcache_.insert(q.record().value(0).toString(), q.record().value(1).toString());
    }
}

void TagPool::addTag(QString tagName, QString type)
{
    QStringList cols, vs;
    cols << "name" << "type";
    vs << tagName <<  type;
    if( db_->hitValue("tag", "name", tagName) ){ // exist, update
        db_->update("tag", cols, vs, "name", tagName );
    } else {
        db_->insert("tag", cols, vs);
    }
    tagcache_.insert(tagName,type);
}

void TagPool::removeTag(QString tagName, QString type)
{
    db_->removeAll("tag", "name", tagName);
    tagcache_.remove(tagName);
}

QString TagPool::queryType(QString tag)
{
    if (!db_->isOpen())
        return QString();

    return db_->whichTableContainsName(tag);
}
