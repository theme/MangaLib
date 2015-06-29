#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QShortcut>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QTableView>
#include <QStateMachine>
#include <QState>
#include <QtSql>
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
    // DB
    void sigDBError(QString what, QString why);
    void sigDBopened();
    void sigDBclosed();

private slots:
    void onDirSelectChanged(QModelIndex current, QModelIndex previous);
    void onFileSelectChanged(QModelIndex current, QModelIndex previous);
    void onFileFocusOut();
    void setCurrentAbsPath(QString absPath);
    void onUIPathEdited();
    // DB
    QSqlError openDBFile();
    void closeDBfile();
    void onDBError(QString what, QString why);
    void displayDB();   // load db tables to view
    // DB view
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
    QAction *closeAct;  // close DB
    QAction *quitAct;

    // ui::Explorer
    Ui::MainWindow *ui;
    QFileSystemModel *dir_model_;
    QItemSelectionModel *dir_selection_model_;
    QFileSystemModel *files_model_;
    QString current_abs_path_;
    QShortcut *sc_editpath_;

    // DB
    QSqlDatabase db_;
    QStateMachine* dbsm_;
    QState *state_opened_;
    QState *state_closed_;
};

#endif // MAINWINDOW_H
