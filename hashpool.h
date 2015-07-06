#ifndef HASHPOOL_H
#define HASHPOOL_H

#include <QObject>
#include <QHash>
#include "hashthread.h"
#include "sqlitedb.h"

class HashPool : public QObject
{
    Q_OBJECT
public:
    explicit HashPool(SQLiteDB *db,
                      QObject *parent = 0);
    QString getFileHash(QCryptographicHash::Algorithm algo, QString fpath);
    QString algoName(QCryptographicHash::Algorithm a) const;

signals:
    void sigHash(int algo, QString hashString, QString fpath);
    void sigHashingPercent(int algo, int percent, QString fpath);

private slots:
    void cacheFileHash(int algo, QString hash, QString fpath);
    QString queryFileHash(QCryptographicHash::Algorithm algo, QString fname, QString size, QString mtime);

private:
    QHash< QString, QString > file_hashes_;
    SQLiteDB *db_;
};

#endif // HASHPOOL_H
