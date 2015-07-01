#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
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
    void cacheFileHash(QString hash, QString fpath);
    void updateUiFileHashingPercent(int percent, QString fpath);

    void setValue(QString field, QString getValue, bool local = true);

signals:
    void sigSaved();

private slots:
    QSqlError save2db();

private:
    void populateUi();
    QString getHash(QString fpath);
    void clearCache();
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
