#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDebug>

#include <QMainWindow>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>
#include <QStateMachine>
#include <QState>
#include "fileexplorer.h"
#include "fileinfowidget.h"
#include "dbschema.h"
#include "sqlitedb.h"
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

private slots:
    void openDBfile();
    void closeDBconnection();
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
    FileExplorer *file_exp_widget_;

    // ui: File info
    FileInfoWidget *file_info_widget_;

    // DB
    DBSchema *dbschema_;
    SQLiteDB *db_;

    // ui: DB tabs
    QHash<DBTableWidget*, int> db_table_widgets_hash_;
};

#endif // MAINWINDOW_H
