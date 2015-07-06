#include "lrline.h"
#include "ui_lrline.h"

LRline::LRline(QWidget *parent, bool hasProgress) :
    QWidget(parent),
    ui(new Ui::LocalAndDBinfoLine)
{
    ui->setupUi(this);
    ui->localProgress->setHidden(!hasProgress);
    ui->remoteProgress->setHidden(!hasProgress);

    connect(ui->localValue, SIGNAL(textChanged(QString)),
            this, SLOT(hintIfEqual()));
    connect(ui->removeValue, SIGNAL(textChanged(QString)),
            this, SLOT(hintIfEqual()));
}

LRline::~LRline()
{
    delete ui;
}

void LRline::setLocalValue(QString s)
{
    if (s.isEmpty())
        return;
    ui->localProgress->hide();
    ui->localValue->setText(s);
    hintIfEqual();
}

void LRline::setRemoteValue(QString s)
{
    if (s.isEmpty())
        return;
    ui->remoteProgress->hide();
    ui->removeValue->setText(s);
    hintIfEqual();
}

void LRline::setLocalProgress(int p)
{
    ui->localProgress->setValue(p);
    ui->localProgress->setHidden(false);
}

void LRline::setRemoteProgress(int p)
{
    ui->remoteProgress->setValue(p);
    ui->remoteProgress->setHidden(false);
}

void LRline::hintIfEqual()
{
    if ( !ui->localValue->text().isEmpty() && isEqual()){
        ui->fieldName->setStyleSheet("QLabel { background-color: green; color: yellow }");
    } else {
        ui->fieldName->setStyleSheet("QLabel { }");
    }
}

void LRline::clear()
{
    ui->localValue->clear();
    ui->removeValue->clear();
    ui->fieldName->setStyleSheet("QLabel { }");
    ui->localProgress->hide();
    ui->remoteProgress->hide();
}

bool LRline::isEqual() const
{
    return ui->localValue->text() == ui->removeValue->text();
}

QString LRline::value(bool local)
{
    if (local){
        return ui->localValue->text();
    } else {
        return ui->removeValue->text();
    }
}

void LRline::setName(QString s)
{
    ui->fieldName->setText(s);
}
