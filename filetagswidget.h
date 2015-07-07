#ifndef FILETAGSWIDGET_H
#define FILETAGSWIDGET_H

#include <QWidget>
#include "tagtypeselector.h"
#include "tagpool.h"

namespace Ui {
class FileTagsWidget;
}

class FileTagsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileTagsWidget(TagPool *tp,
                            QWidget *parent = 0);
    ~FileTagsWidget();
    void setFileName(QString fn);

private:
    void addSelector(QString tag);
    void addSelectorList(QStringList tagli);
    void removeSelector(QString tag);
    void clearSelectors();
    Ui::FileTagsWidget *ui;
    QHash<QString, TagTypeSelector* > tag_type_selectors_;
    QList<QString> tags_;
    TagPool *tp_;
};

#endif // FILETAGSWIDGET_H
