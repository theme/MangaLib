#ifndef SQLTABLEWIDGET_H
#define SQLTABLEWIDGET_H

#include <QTableView>
#include <QSqlTableModel>

class SQLTableWidget : public QTableView
{
    Q_OBJECT
public:
    explicit SQLTableWidget(QString name,
                            QSqlDatabase& db,
                            QWidget *parent = 0);
    QSqlTableModel* model();
signals:

public slots:

private:
    QString name_;
    QSqlTableModel* sql_table_model_;
};

#endif // SQLTABLEWIDGET_H
