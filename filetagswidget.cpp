#include "filetagswidget.h"
#include "ui_filetagswidget.h"

FileTagsWidget::FileTagsWidget(TagPool *tp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileTagsWidget), tp_(tp)
{
    ui->setupUi(this);
}

FileTagsWidget::~FileTagsWidget()
{
    delete ui;
}

void FileTagsWidget::setFileName(QString fn)
{
    this->clearSelectors();
    QStringList tags = tp_->tagsInString(fn);
    this->addSelectorList(tags);
}

void FileTagsWidget::addSelector(QString tag)
{

    TagTypeSelector *w = new TagTypeSelector(tag,tp_,this);
    tag_type_selectors_.insert(tag, w);
    tags_.append(tag);
    ui->tagsLayout->addWidget(w);
}

void FileTagsWidget::addSelectorList(QStringList tagli)
{
    QString t;
    foreach (t, tagli){
        this->addSelector(t);
    }
}

void FileTagsWidget::removeSelector(QString tag)
{
    TagTypeSelector *w;
    while( tags_.contains(tag) ){
        tags_.removeOne(tag);
        w = tag_type_selectors_.take(tag);
        ui->tagsLayout->removeWidget(w);
        w->deleteLater();
    }
}

void FileTagsWidget::clearSelectors()
{
    TagTypeSelector *w;
    QString t;
    for (int i= 0; i < tags_.size(); ++i){
        t = tags_.at(i);
        w = tag_type_selectors_.take(t);
        ui->tagsLayout->removeWidget(w);
        w->deleteLater();
    }
    tags_.clear();
    tag_type_selectors_.clear();
}
