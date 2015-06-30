#ifndef DBTABLEWIDGET_H
#define DBTABLEWIDGET_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class DBTableWidget;
}

class DBTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DBTableWidget(QString name,
                           QSqlDatabase& db,
                           QWidget *parent = 0);
    ~DBTableWidget();

private slots:
    void on_insertButton_clicked();

    void on_removeButton_clicked();

    void on_submitButton_clicked();

private:
    Ui::DBTableWidget *ui;
    QString name_;
    QSqlTableModel* sql_table_model_;
};

#endif // DBTABLEWIDGET_H
