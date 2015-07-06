#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QtDebug>

#include <QWidget>
#include <QFileSystemModel>
#include <QShortcut>
#include <QCompleter>
#include <QDirModel>

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
    QString path() const;
signals:
    void sigPath(QString path);
    void sigFilePath(QString fpath);
    void sigFileNameSelected(bool select = true);

private slots:
    void onCurrentDirChanged(QModelIndex current, QModelIndex previous);
    void onCurrentFileChanged(QModelIndex current, QModelIndex previous);
    void setPath(QString path);

    void on_pathEdit_editingFinished();

private:
    Ui::FileExplorer *ui;
    QFileInfo finfo_;
    QFileSystemModel *dir_model_;
    QFileSystemModel *files_model_;
    QShortcut *sc_editpath_;
};

#endif // FILEEXPLORER_H
