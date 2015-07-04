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
signals:
    void sigHash(QString algoName, QString hashString, QString fpath);
    void sigHashingError(QString algoName, QString errorString, QString fpath);
    void sigHashingPercent(QString algoName, int percent, QString fpath);

private slots:
    void cacheFileHash(QString algo, QString hash, QString fpath);
private:
    QHash< QString, QString > file_hashes_;
};

#endif // HASHPOOL_H
