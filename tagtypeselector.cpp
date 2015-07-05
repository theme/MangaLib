#include "tagtypeselector.h"
#include "ui_tagtypeselector.h"

TagTypeSelector::TagTypeSelector(QString tagName, QStringList options, QString current, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagTypeSelector),
    current_type_(current),
    opts_(options)
{
    ui->setupUi(this);
    ui->tagName->setText(tagName);

    QRadioButton* w;
    QString t;
    for (int i = 0; i < options.size(); ++i){
        t = options.at(i);
        w = new QRadioButton(t);
        if (current == t){
            w->setChecked(true);
        }

        type_buttons_.insert(t, w);

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
    QString t;
    for (int i=0; i< opts_.size(); ++i){
        t = opts_.at(i);
        w = type_buttons_.value(t);

        if (w->isChecked()){
            if (t != current_type_){
                current_type_ = t;
                emit sigTypeChanged(current_type_, t);
            } else {
                emit sigTypeChoosed(t);
            }
        }
    }
}
