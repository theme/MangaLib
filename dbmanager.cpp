#include "dbmanager.h"
#include "ui_dbmanager.h"

DBManager::DBManager(SQLiteDB *db,
                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DBManager),
    db_(db)
{
    ui->setupUi(this);
    this->loadDBTabs();
}

DBManager::~DBManager()
{
    delete ui;
}

void DBManager::loadDBTabs()
{
    QStringList tables = db_->tables();
    for( int i = 0; i < tables.size(); ++i){
        DBTableWidget* w = new DBTableWidget(tables.at(i),
                                             db_->conn(),
                                             ui->tablesTab);

        QString tname = QString("( &") + QString::number(i+1) + " ) "
                + tables.at(i);
        ui->tablesTab->addTab(w,tname);
    }
    ui->dbFileName->setText(db_->conn().databaseName());
}
