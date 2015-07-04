#ifndef HASHPOOL_H
#define HASHPOOL_H

#include <QObject>
#include <QHash>
#include "hashthread.h"

class HashPool : public QObject
{
    Q_OBJECT
public:
    explicit HashPool(QObject *parent = 0);
    QString getFileHash(QCryptographicHash::Algorithm algo, QString fpath);
    QString algoName(QCryptographicHash::Algorithm a) const;

signals:
    void sigHash(int algo, QString hashString, QString fpath);
    void sigHashingPercent(int algo, int percent, QString fpath);

private slots:
    void cacheFileHash(int algo, QString hash, QString fpath);
private:
    QHash< QString, QString > file_hashes_;
};

#endif // HASHPOOL_H
