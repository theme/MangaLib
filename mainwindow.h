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
#include "hashthread.h"

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
    void sigStatusMsg(QString, int t = 2000);
    // DB
    void sigDBopened();
    void sigDBclosed();

private slots:
    void onDirSelectChanged(QModelIndex current, QModelIndex previous);
    void showFileInfo(QModelIndex index);
    void onFileFocusOut();
    void setCurrentAbsPath(QString absPath);
    void onUIPathEdited();
    // File
    QString getHash(QString fpath);
    void updateFileHash(QString hash, QString fpath);
    void clearCache();

    // DB
    QSqlError openDB(QString fn = QString()); // open db: sqlite file
    void closeDB();
    void onDBError(QString what, QString why);
    void loadDBTabs();   // load db tables to view
    void removeDBTabs();

private:
    // ui::Menu
    void createActions();
    void createMenus();
    QMenu *DBMenu;
    QAction *openAct;
    QAction *closeAct;  // close DB
    QAction *clearCacheAct; // clear hash cache
    QAction *quitAct;

    // ui::Explorer
    Ui::MainWindow *ui;
    QFileSystemModel *dir_model_;
    QItemSelectionModel *dir_selection_model_;
    QFileSystemModel *files_model_;
    QString current_abs_path_;
    QShortcut *sc_editpath_;

    // File info
    HashThread *hash_thread_;
    QHash<QString, QString> hash_cache_; // <fpath, hash>

    // DB
    QSqlDatabase db_;
    QStateMachine* dbsm_;
    QState *state_opened_;
    QState *state_closed_;
    QHash<SQLTableWidget*, int> db_table_widgets_hash_;
};

#endif // MAINWINDOW_H
