#include "filestableview.h"

FilesTableView::FilesTableView(QWidget *parent):
    QTableView(parent)
{
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

void FilesTableView::focusOutEvent(QFocusEvent *event)
{
    QTableView::focusOutEvent(event);
    emit sigFocusOut();
}

