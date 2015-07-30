#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDebug>

#include <QMainWindow>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QClipboard>

#include "sqlitedb.h"
#include "fileexplorer.h"
#include "dbtablewidget.h"
#include "fsmixdbmodel.h"
#include "tagpool.h"
#include "dbmanager.h"
#include "rankpool.h"

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
    void openDBfile(QString fn = QString());
    void closeDBconnection();
    void loadDBTabs();   // load db tables to view
    void removeDBTabs();
    void openDBManager();
    void onQuitAct();
    void onClipboardChanged();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_topTabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    // ui::Menu
    void createActions();
    void createMenus();
    void createTrayIcon();
    QMenu *DBMenu;
    QAction *openAct;
    QAction *closeAct;  // close DB
    QAction *dbManAct;
    QAction *tags2DBAct;
    QAction *quitAct;

    // ui: File explorer
    FileExplorer *file_exp_widget_;

    // ui: tags
    FileTagsWidget *file_tags_widget_;

    // DB
    SQLiteDB *db_;

    // file hash pool
    HashPool *hp_;

    TagPool *tp_;

    RankPool *rp_;

    // ui: DB tabs
    QHash<DBTableWidget*, int> db_table_widgets_hash_;

    // ui: tray icon
    QSystemTrayIcon *tray_icon_;
    QClipboard *clipboard_;
    QMenu *tray_menu_;
};

#endif // MAINWINDOW_H
