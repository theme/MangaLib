#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QFileInfo>
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
                            QWidget *parent = 0);
    ~FileInfoWidget();

public slots:
    void setFile(QString f);
    void cacheFileHash(QString hash, QString fpath);
    void updateUiFileHash(QString fpath);
    void updateUiFileHashingPercent(int percent, QString fpath);

    void setValue(QString field, QString value, bool local = true);

private:
    void populateUi();
    QString getHash(QString fpath);
    void clearCache();
    Ui::FileInfoWidget *ui;
    QFileInfo finfo;   // local file info

    // db file schema
    const DBSchema *schema_;
    QHash< QString, LRline* > field_widgets_;

    // hash
    HashThread *hash_thread_;
    QHash<QString, QString> hash_cache_; // <fpath, hash>
};

#endif // FILEINFOWIDGET_H
