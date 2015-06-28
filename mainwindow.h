#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QTableView>
#include "db/dbsm.h"
#include "sqltablewidget.h"

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
    void sigOpenDBFile(QString fpath);

private slots:
    void onDirSelectChanged(QModelIndex current, QModelIndex previous);
    void setCurrentAbsPath(QString absPath);
    void onUIPathEdited();
    void openDBFile();
    void onDBError(QString what, QString why);
    void displayDB();   // load db tables to view
    void enableLibView();
    void disableLibView();

    void on_insertButton_clicked();

    void on_submitButton_clicked();

    void on_removeButton_clicked();

private:
    // ui::Menu
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QAction *openAct;
    QAction *quitAct;

    // ui::Explorer
    Ui::MainWindow *ui;
    QFileSystemModel *dir_model_;
    QItemSelectionModel *dir_selection_model_;
    QFileSystemModel *files_model_;
    QString current_abs_path_;

    // DB satate machine
    DBSM *dbsm_;
};

#endif // MAINWINDOW_H
