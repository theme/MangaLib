#include "tagtypeselector.h"
#include "ui_tagtypeselector.h"

TagTypeSelector::TagTypeSelector(QString tagName, TagPool *tp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagTypeSelector),
    tag_name_(tagName),
    tp_(tp)
{
    ui->setupUi(this);
    ui->tagName->setText(tagName);

    QRadioButton* w;
    QString o;
    opts_ = tp_->typeOptions();
    current_type_ = tp_->tagType(tagName);
    for (int i = 0; i < opts_.size(); ++i){
        o = opts_.at(i);
        w = new QRadioButton(o);
        if ( current_type_ == o){
            w->setChecked(true);
        }

        type_buttons_.insert(o, w);

        ui->verticalLayout->addWidget(w);
        connect(w, SIGNAL(pressed()),
                this, SLOT(onButtonChecked()));
    }
}

TagTypeSelector::~TagTypeSelector()
{
    delete ui;
}

void TagTypeSelector::onButtonChecked()
{
    QRadioButton* w;
    QString o;
    for (int i=0; i< opts_.size(); ++i){
        o = opts_.at(i);
        w = type_buttons_.value(o);

        if (w->isChecked()){
            if (o != current_type_){
                current_type_ = o;
                emit sigTypeChanged(current_type_, o, tag_name_);
            } else {
                emit sigTypeChoosed(o, tag_name_);
            }
        }
    }
}
