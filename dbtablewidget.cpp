#include "dbtablewidget.h"
#include "ui_dbtablewidget.h"

#include <QDebug>

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
        sql_table_model_->insertRow(0);
    } else{
        sql_table_model_->insertRow(in.row());
    }
}

void DBTableWidget::on_removeButton_clicked()
{
    QModelIndexList li  = ui->tableView->selectionModel()->selectedIndexes();
    while (!li.isEmpty()){
        sql_table_model_->removeRow(li.takeFirst().row());
    }
}

void DBTableWidget::on_submitButton_clicked()
{
    sql_table_model_->submitAll();
}

void DBTableWidget::on_cancelButton_clicked()
{
    sql_table_model_->revertAll();
}
