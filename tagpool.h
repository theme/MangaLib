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
    QString type(QString tag);
    QStringList tagsInString(QString s) const;
signals:

public slots:

private slots:
    void loadDBtags();

private:
    QString queryType(QString tag);
    QHash< QString, QString > tagcache_;
    SQLiteDB *db_;
};

#endif // TAGPOOL_H
