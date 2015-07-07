#ifndef STAREDITOR_H
#define STAREDITOR_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "rank/starrating.h"

namespace Ui {
class StarEditor;
}

class StarEditor : public QWidget
{
    Q_OBJECT

public:
    explicit StarEditor(QWidget *parent = 0);
    ~StarEditor();

    QSize sizeHint() const;
    void setStarRating(const StarRating &starRating) {
        myStarRating = starRating;
    }
    StarRating starRating() { return myStarRating; }

signals:
    void editingFinished();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    int starAtPosition(int x);

    StarRating myStarRating;
    Ui::StarEditor *ui;
};

#endif // STAREDITOR_H
