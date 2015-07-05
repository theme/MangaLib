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
        connect(w, SIGNAL(toggled(bool)),
                this, SLOT(onButtonToggled()));
    }

    connect(this, SIGNAL(sigTypeChoosed(QString,QString,QString)),
            tp_, SLOT(handleTagTypeChange(QString,QString,QString)));
}

TagTypeSelector::~TagTypeSelector()
{

    disconnect(this, SIGNAL(sigTypeChoosed(QString,QString,QString)),
               tp_, SLOT(handleTagTypeChange(QString,QString,QString)));
    delete ui;
}

void TagTypeSelector::onButtonToggled()
{
    QRadioButton* w;
    QString o;
    for (int i=0; i< opts_.size(); ++i){
        o = opts_.at(i);
        w = type_buttons_.value(o);

        if (w->isChecked() && o != current_type_){
            emit sigTypeChoosed(tag_name_, o, current_type_);
            current_type_ = o;
        }
    }
}
