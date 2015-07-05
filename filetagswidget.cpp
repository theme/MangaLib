#include "filetagswidget.h"
#include "ui_filetagswidget.h"

FileTagsWidget::FileTagsWidget(QString fn, TagPool *tp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileTagsWidget), tp_(tp)
{
    ui->setupUi(this);
    ui->fileName->setText(fn);

    QStringList tags = tp_->tagsInString(fn);
    QString t;
    for (int i=0; i< tags.size(); ++i){
        t = tags.at(i);
        TagTypeSelector *w = new TagTypeSelector(t,tp_->typeOptions(), tp_->tagType(t),this);
        tag_type_selectors_.insert(t, w);
        ui->tagsLayout->addWidget(w);
    }
}

FileTagsWidget::~FileTagsWidget()
{
    delete ui;
}
