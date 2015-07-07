#ifndef FSFILTERPROXYMODEL_H
#define FSFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FSfilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FSfilterProxyModel(QObject *parent = 0);

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
};

#endif // FSFILTERPROXYMODEL_H
