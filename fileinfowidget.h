#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QFileInfo>
#include "hashthread.h"

namespace Ui {
class FileInfoWidget;
}

class FileInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoWidget(QWidget *parent = 0);
    ~FileInfoWidget();

public slots:
    void setFile(QString f);
    void cacheFileHash(QString hash, QString fpath);
    void updateUiFileHash(QString hash, QString fpath);
    void updateUiFileHashingPercent(int percent, QString fpath);

private slots:
    void on_save2dbButton_clicked();

private:
    QString getHash(QString fpath);
    void clearCache();
    Ui::FileInfoWidget *ui;
    QFileInfo finfo;   // local file info

    // hash
    HashThread *hash_thread_;
    QHash<QString, QString> hash_cache_; // <fpath, hash>
};

#endif // FILEINFOWIDGET_H
