#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QFileInfo>
#include <QDateTime>
#include "hashthread.h"
#include "lrline.h"
#include "sqlitedb.h"
#include "hashpool.h"

namespace Ui {
class FileInfoWidget;
}

class FileInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoWidget(SQLiteDB *db,
                            HashPool *hp,
                            QWidget *parent = 0);
    ~FileInfoWidget();
    QString getValue(QString field, bool local = true);

public slots:
    void setFile(QString f);

signals:
    void sigSaved2DB();

private slots:
    void handleGotHash(QString algo, QString hash, QString fpath);
    void updateHashingProgress(QString algo, int percent, QString fpath);
    bool isInDB();
    bool update2db(bool update = true);
    void updateFromDB(QString fieldName, QString v);
    void updateLocalValue(QString fieldName, QString v, QString fpath);

private:
    void populateUi();
    QString getHash(QString algo, QString fpath);
    void setValue(QString fieldName, QString v, bool local = true);
    void clearValueAll();
    void setProgress(QString fieldName, int p, bool local = true);
    Ui::FileInfoWidget *ui;
    QFileInfo finfo;

    // will populate
    QHash< QString, LRline* > field_widgets_;

    // db ref
    SQLiteDB *db_;

    // hash
    HashPool *hp_;
};

#endif // FILEINFOWIDGET_H
