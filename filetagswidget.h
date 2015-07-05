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
    explicit FileTagsWidget(QString fn,
                            TagPool *tp,
                            QWidget *parent = 0);
    ~FileTagsWidget();

private:
    Ui::FileTagsWidget *ui;
    QHash<QString, TagTypeSelector* > tag_type_selectors_;
    TagPool *tp_;
};

#endif // FILETAGSWIDGET_H
