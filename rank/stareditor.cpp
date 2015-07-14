#include "stareditor.h"

StarEditor::StarEditor(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    setAutoFillBackground(true);
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
    int starWidth = (myStarRating.sizeHint().width()
                     / myStarRating.maxStarCount());
    int star = (x / starWidth) + 1;

    if ( x < starWidth / 2 )
        return 0;

    if (star <= 0 || star > myStarRating.maxStarCount())
        return -1;

    return star;
}
