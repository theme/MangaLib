#include "filestableview.h"

FilesTableView::FilesTableView(QWidget *parent):
    QTableView(parent)
{

}

void FilesTableView::focusOutEvent(QFocusEvent *event)
{
    QTableView::focusOutEvent(event);
    emit sigFocusOut();
}

