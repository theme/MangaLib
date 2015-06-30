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

private slots:
    void onPathEdited();
    void onDirSelectChanged(QModelIndex current, QModelIndex previous);
    void setPath(QString path);

private:
    Ui::FileExplorer *ui;
    QFileSystemModel *dir_model_;
    QItemSelectionModel *dir_selection_model_;
    QFileSystemModel *files_model_;
    QShortcut *sc_editpath_;
};

#endif // FILEEXPLORER_H
