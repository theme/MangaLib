#ifndef RANKDELEGATE_H
#define RANKDELEGATE_H

#include <QStyledItemDelegate>
#include "rank/stareditor.h"

class RankDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit RankDelegate(QObject *parent = 0);


public slots:


    // QAbstractItemDelegate interface

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:
private slots:
    void commitAndCloseEditor();
};

#endif // RANKDELEGATE_H
