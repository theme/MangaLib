#include "stareditor.h"
#include "ui_stareditor.h"

StarEditor::StarEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StarEditor)
{
    ui->setupUi(this);

    setMouseTracking(true);
    setAutoFillBackground(true);
}

StarEditor::~StarEditor()
{
    delete ui;
}

QSize StarEditor::sizeHint() const
{
    return myStarRating.sizeHint();
}

void StarEditor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    myStarRating.paint(&painter, rect(), this->palette(),
                       StarRating::Editable);
}

void StarEditor::mouseMoveEvent(QMouseEvent *event)
{
    int star = starAtPosition(event->x());

    if (star != myStarRating.starCount() && star != -1) {
        myStarRating.setStarCount(star);
        update();
    }
}

void StarEditor::mouseReleaseEvent(QMouseEvent *event)
{
    emit editingFinished();
}

int StarEditor::starAtPosition(int x)
{
    int star = (x / (myStarRating.sizeHint().width()
                     / myStarRating.maxStarCount())) + 1;
    if (star <= 0 || star > myStarRating.maxStarCount())
        return -1;

    return star;
}
