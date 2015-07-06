#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QDialog>
#include "dbtablewidget.h"
#include "sqlitedb.h"

namespace Ui {
class DBManager;
}

class DBManager : public QDialog
{
    Q_OBJECT

public:
    explicit DBManager(SQLiteDB *db,
                       QWidget *parent = 0);
    ~DBManager();

private:
    void loadDBTabs();
    Ui::DBManager *ui;
    SQLiteDB *db_;
};

#endif // DBMANAGER_H
