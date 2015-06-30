#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDebug>
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
#include "dbschema.h"
#include "fileexplorer.h"
#include "fileinfowidget.h"
#include "dbtablewidget.h"

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
    void sigStatusMsg(QString, int t = 2000);
    // DB
    void sigDBopened();
    void sigDBclosed();

private slots:
    // DB
    QSqlError openDB(QString fn = QString()); // open db: sqlite file
    void closeDB();
    void onDBError(QString what, QString why);
    void loadDBTabs();   // load db tables to view
    void removeDBTabs();

private:
    Ui::MainWindow *ui;
    // ui::Menu
    void createActions();
    void createMenus();
    QMenu *DBMenu;
    QAction *openAct;
    QAction *closeAct;  // close DB
    QAction *quitAct;

    // ui: File explorer
    FileExplorer* file_exp_widget_;

    // ui: File info
    FileInfoWidget* file_info_widget_;

    // DB
    DBSchema dbschema_;
    QSqlDatabase db_;
    QStateMachine* dbsm_;
    QState *state_opened_;
    QState *state_closed_;
    QHash<DBTableWidget*, int> db_table_widgets_hash_;
};

#endif // MAINWINDOW_H
