#ifndef LOCALANDDBINFOLINE_H
#define LOCALANDDBINFOLINE_H

#include <QWidget>

namespace Ui {
class LocalAndDBinfoLine;
}

class LRline : public QWidget
{
    Q_OBJECT

public:
    explicit LRline(QWidget *parent = 0);
    ~LRline();
    bool isEqual() const;

public slots:
    void setName(QString s);
    void setLocalValue(QString s);
    void setRemoveValue(QString s);
private:
    Ui::LocalAndDBinfoLine *ui;
};

#endif // LOCALANDDBINFOLINE_H
