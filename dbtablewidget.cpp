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
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setModel(sql_table_model_);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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
    if (!sql_table_model_->submitAll()){
        qDebug() << sql_table_model_->lastError();
    }
}

void DBTableWidget::on_cancelButton_clicked()
{
    sql_table_model_->revertAll();
}

void DBTableWidget::on_dupButton_clicked()
{
    QModelIndexList li  = ui->tableView->selectionModel()->selectedIndexes();
    // get sorted row numbers
    QList<int> rows;
    while (!li.isEmpty()){
        QModelIndex in = li.takeFirst();
        if (!rows.contains(in.row())){
            rows.append(in.row());
        }
    }
    qSort(rows.begin(),rows.end());

    // do duplicate
    int c = 0;  // only works when rows is sorted ascending.
    while (!rows.isEmpty()){
        int r = rows.takeFirst();
        QSqlRecord rec = sql_table_model_->record(r + c);
        sql_table_model_->insertRecord(r + c, rec);
        ++c;
    }
}
