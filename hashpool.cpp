#include "hashpool.h"

HashPool::HashPool(SQLiteDB *db, QObject *parent) : QObject(parent), db_(db)
{
    connect(this, SIGNAL(sigHash(int,QString,QString)),
            this, SLOT(saveHash2DB(int,QString,QString)));
}

QString HashPool::getFileHash(QCryptographicHash::Algorithm algo, QString fpath)
{
    QFileInfo fi(fpath);
    if (!fi.isFile())
        return QString();
    // search DB
    if (!file_hashes_.contains(QString::number(algo)+fpath)){
        QString h = this->queryFileHash(algo,
                                        fi.fileName(),
                                        QString::number(fi.size()),
                                        fi.lastModified().toString(Qt::ISODate));
        if (!h.isEmpty()){
//            qDebug() << "HashPool::getFileHash() hit DB";
            file_hashes_.insert(QString::number(algo)+fpath, h);
        } else {
//            qDebug() << "HashPool::getFileHash() miss DB" << h;
        }
    }
    // calculate
    if (!file_hashes_.contains(QString::number(algo)+fpath)) {
        qDebug() << "HashPool::getFileHash() threading ..." << fi.filePath();
        HashThread *t = new HashThread(algo,fpath,this);
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        connect(t, SIGNAL(sigHash(int,QString,QString)),
                this, SLOT(cacheFileHash(int,QString,QString)));

        connect(t, SIGNAL(sigHashingPercent(int, int,QString)),
                this, SIGNAL(sigHashingPercent(int, int,QString)));

        file_hashes_.insert(QString::number(algo)+fpath, ""); // nessary guard too many threads
        t->start();
        return "";
    }
    return file_hashes_.value(QString::number(algo)+fpath);
}

void HashPool::cacheFileHash(int algo, QString hash, QString fpath)
{
    file_hashes_.insert(QString::number(algo)+fpath, hash);
    emit sigHash(algo, hash, fpath);
}

QString HashPool::queryFileHash(QCryptographicHash::Algorithm algo, QString fname, QString size, QString mtime)
{
    QStringList cols, vs;
    cols << "name" << "size" << "timestamp";
    vs << fname << size << mtime;
    return db_->query1valueString("file", this->algoName(algo), cols, vs);
}

QString HashPool::algoName(QCryptographicHash::Algorithm a) const
{
    switch (a) {
    case QCryptographicHash::Md4:
        return "md4";
        break;
    case QCryptographicHash::Md5:
        return "md5";
        break;
    case QCryptographicHash::Sha1:
        return "sha1";
        break;
    case QCryptographicHash::Sha256:
        return "sha256";
        break;
    case QCryptographicHash::Sha224:
        return "sha224";
        break;
    case QCryptographicHash::Sha384:
        return "sha384";
        break;
    case QCryptographicHash::Sha512:
        return "sha512";
        break;
    default:
        throw "unknownHashName";
        break;
    }
}

void HashPool::saveHash2DB(int algo, QString hash, QString fpath)
{
    QFileInfo fi(fpath);

    QStringList cols, vs;
    cols << "name" << "size" << "timestamp";
    vs << fi.fileName() << QString::number(fi.size()) << fi.lastModified().toString(Qt::ISODate);
    QString h = db_->query1valueString("file",
                                 this->algoName(static_cast<QCryptographicHash::Algorithm>(algo)),
                                 cols,
                                 vs);
    if( h.isEmpty() || h != hash){
        cols << this->algoName(static_cast<QCryptographicHash::Algorithm>(algo));
        vs  << hash;
        db_->insert("file", cols, vs);
        qDebug() << "HashPool::saveHash2DB()" << hash << " " << fi.fileName();
    }
}
