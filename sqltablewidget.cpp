#include "sqltablewidget.h"

SQLTableWidget::SQLTableWidget(QString name, QSqlDatabase &db, QWidget *parent) :
    QTableView(parent),name_(name)
{
    sql_table_model_ = new QSqlTableModel(parent, db);
    sql_table_model_->setTable(name);
    sql_table_model_->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sql_table_model_->select();
    sql_table_model_->sort(1, Qt::AscendingOrder);
    this->setModel(sql_table_model_);
}

QSqlTableModel *SQLTableWidget::model()
{
    return sql_table_model_;
}
