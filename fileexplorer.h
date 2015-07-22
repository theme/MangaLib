#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QtDebug>

#include <QWidget>
#include <QFileSystemModel>
#include <QShortcut>
#include <QCompleter>
#include <QDirModel>
#include "rankdelegate.h"
#include "fsfilterproxymodel.h"
#include "filetagswidget.h"

namespace Ui {
class FileExplorer;
}

class FileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit FileExplorer(QFileSystemModel* filesmodel,
                          QWidget *parent = 0);
    ~FileExplorer();
    QString currentPath() const;
public slots:
    void setPath(QString path);

signals:
    void sigPath(QString path);
    void sigFilePath(QString fpath);
    void sigFileNameSelected(bool select = true);

private slots:
    void onCurrentDirChanged(QModelIndex current, QModelIndex previous);
    void onCurrentFileChanged(QModelIndex current, QModelIndex previous);

    void on_pathEdit_editingFinished();

private:
    Ui::FileExplorer *ui;
    QFileInfo finfo_;
    QFileSystemModel *dir_model_;
    FSfilterProxyModel *dir_proxy_model_;
    QFileSystemModel *files_model_;
    QShortcut *sc_editpath_;
};

#endif // FILEEXPLORER_H
