#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QtSql>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void sigCurrentAbsPath(QString abspath);
    void sigStatusMsg(QString, int t = 0);
public slots:
    void onDirSelectChanged(QModelIndex current, QModelIndex previous);
    void setCurrentAbsPath(QString absPath);
    void onUIPathEdited();
private slots:
    QSqlError initDB();
private:private:
    //Menu
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QAction *openAct;
    QAction *quitAct;

    //Explorer
    Ui::MainWindow *ui;
    QFileSystemModel *dir_model_;
    QItemSelectionModel *dir_selection_model_;
    QFileSystemModel *files_model_;
    QString current_abs_path_;

    //DB
    QFile dbfn_;
};

#endif // MAINWINDOW_H
