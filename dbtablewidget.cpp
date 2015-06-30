#include "dbtablewidget.h"
#include "ui_dbtablewidget.h"

DBTableWidget::DBTableWidget(QString name,
                             QSqlDatabase& db,
                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DBTableWidget)
{
    ui->setupUi(this);

    sql_table_model_ = new QSqlTableModel(parent, db);
    sql_table_model_->setTable(name);
    sql_table_model_->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sql_table_model_->select();
    ui->tableView->setModel(sql_table_model_);
}

DBTableWidget::~DBTableWidget()
{
    delete ui;
}

void DBTableWidget::on_insertButton_clicked()
{
    QModelIndex in  = ui->tableView->currentIndex();
    if (!in.isValid()){
        in = ui->tableView->rootIndex().child(0,0);
    }
    sql_table_model_->insertRow(in.row(), in.parent());
}

void DBTableWidget::on_removeButton_clicked()
{
    QModelIndex in  = ui->tableView->currentIndex();
    if (in.isValid()){
        sql_table_model_->removeRow(in.row(),in.parent());
    }
}

void DBTableWidget::on_submitButton_clicked()
{
    sql_table_model_->submitAll();
}
