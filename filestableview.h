#ifndef FILESTABLEVIEW_H
#define FILESTABLEVIEW_H

#include <QTableView>
#include <QHeaderView>

class FilesTableView : public QTableView
{
    Q_OBJECT
public:
    FilesTableView(QWidget *parent = 0);
signals:
    void sigFocusOut();
protected:
    void focusOutEvent(QFocusEvent * event);
};

#endif // FILESTABLEVIEW_H
