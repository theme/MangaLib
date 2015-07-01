#include "lrline.h"
#include "ui_lrline.h"

LRline::LRline(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalAndDBinfoLine)
{
    ui->setupUi(this);
}

LRline::~LRline()
{
    delete ui;
}

void LRline::setLocalValue(QString s)
{
    ui->localValue->setText(s);
}

void LRline::setRemoveValue(QString s)
{
    ui->removeValue->setText(s);
}

bool LRline::isEqual() const
{
    return ui->localValue->text() == ui->removeValue->text();
}


void LRline::setName(QString s)
{
    ui->fieldName->setText(s);
}
