#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QWidget>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QShortcut>
#include <QCompleter>
#include <QDirModel>
#include "filestableview.h"

namespace Ui {
class FileExplorer;
}

class FileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit FileExplorer(QWidget *parent = 0);
    ~FileExplorer();
    QString path() const;

signals:
    void sigPath(QString path);
    void sigFilePath(QString fpath);

private slots:
    void onDirSelectChanged(QModelIndex current, QModelIndex previous);
    void setPath(QString path);

    void on_pathEdit_editingFinished();

private:
    Ui::FileExplorer *ui;
    QFileInfo finfo_;
    QFileSystemModel *dir_model_;
    QItemSelectionModel *dir_selection_model_;
    QFileSystemModel *files_model_;
    QShortcut *sc_editpath_;
};

#endif // FILEEXPLORER_H
