#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QFileInfo>
#include <QDateTime>
#include "lrline.h"
#include "sqlitedb.h"
#include "hashpool.h"
#include "tagpool.h"
#include "filetagswidget.h"

namespace Ui {
class FileInfoWidget;
}

class FileInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileInfoWidget(SQLiteDB *db,
                            HashPool *hp,
                            TagPool *tp,
                            QWidget *parent = 0);
    ~FileInfoWidget();
    QString getValue(QString field, bool local = true);

public slots:
    void setFile(QString f);
    void showHide(bool show = true);

signals:
    void sigSaved2DB();
    void sigFileChanged(QString old, QString now);

private slots:
    void handleGotHash(int algo, QString hash, QString fpath);
    void updateHashingProgress(int algo, int percent, QString fpath);
    bool update2db(bool update = true);
    void updateFromDB(QString fieldName, QString v, QString fpath);
    void updateLocalValue(QString fieldName, QString v, QString fpath);

private:
    void populateUi();
    void setValue(QString fieldName, QString v, bool local = true);
    void clearValueAll();
    void setProgress(QString fieldName, int p, bool local = true);
    Ui::FileInfoWidget *ui;
    QFileInfo finfo_;

    // will populate
    QHash< QString, LRline* > field_widgets_;

    // db ref
    SQLiteDB *db_;

    // hash
    HashPool *hp_;

    TagPool *tp_;
};

#endif // FILEINFOWIDGET_H
