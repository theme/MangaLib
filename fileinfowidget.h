#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include "hashthread.h"
#include "dbschema.h"
#include "lrline.h"

namespace Ui {
class FileInfoWidget;
}

class FileInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoWidget(const DBSchema *schema,
                            QSqlDatabase &db,
                            QWidget *parent = 0);
    ~FileInfoWidget();
    QString getValue(QString field, bool local = true);

public slots:
    void setFile(QString f);

signals:
    void sigSaved2DB();
    void sigGotHash(QString algoName, QString v, QString f);

private slots:
    void cacheFileHash(QString hash, QString fpath);
    void updateUiFileHashingPercent(int percent, QString fpath);
    QSqlError save2db();
    void queryDB(QString fieldName, QString v);

private:
    void populateUi();
    QString getHash(QString fpath);
    void clearCache();
    void setValue(QString fieldName, QString v, bool local = true);
    Ui::FileInfoWidget *ui;
    QFileInfo finfo;   // local file info

    // db file schema
    const DBSchema *dbschema_;
    QHash< QString, LRline* > field_widgets_;

    // db ref
    QSqlDatabase &db_;

    // hash
    HashThread *hash_thread_;
    QHash<QString, QString> hash_cache_; // <fpath, hash>
};

#endif // FILEINFOWIDGET_H
