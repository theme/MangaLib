#ifndef TAGPOOL_H
#define TAGPOOL_H

#include <QObject>
#include <QHash>
#include "sqlitedb.h"

class TagPool : public QObject
{
    Q_OBJECT
public:
    explicit TagPool(SQLiteDB *db,
                     QObject *parent = 0);
    // return type of tag
    // return empty when unknown
    QString tagType(QString tag);
    QStringList tagsInString(QString s) const;

    QStringList typeOptions()const;
signals:

public slots:
    void handleTagTypeChange(QString tagName, QString type, QString oldType);
    void insertTags2Table();

private slots:
    void loadDBtags();

private:
    QString queryType(QString tag);
    QHash< QString, QString > tagcache_;
    SQLiteDB *db_;
};

#endif // TAGPOOL_H
