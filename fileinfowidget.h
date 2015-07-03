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
    void cacheFileHash(QString algo, QString hash, QString fpath);
    void updateHashingProgress(QString algo, int percent, QString fpath);
    QSqlError update2db();
    QSqlRecord queryDB(QString fieldName, QString v);
    void updateLocalValue(QString fieldName, QString v, QString fpath);

private:
    void populateUi();
    QString getHash(QString algo, QString fpath);
    void setValue(QString fieldName, QString v, bool local = true);
    void clearValueAll();
    void setProgress(QString fieldName, int p, bool local = true);
    Ui::FileInfoWidget *ui;
    QFileInfo finfo;   // local file info

    // db file schema
    const DBSchema *dbschema_;
    QHash< QString, LRline* > field_widgets_;

    // db ref
    QSqlDatabase &db_;

    // hash
    HashThread *hash_thread_;
    QHash< QString, QHash<QString, QString> > caches_;
};

#endif // FILEINFOWIDGET_H
