#include "fsfilterproxymodel.h"

FSfilterProxyModel::FSfilterProxyModel(QObject *parent):
    QSortFilterProxyModel(parent)
{
}


bool FSfilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    if ( source_column != 0 )   // name only
        return false;
    return true;
}
