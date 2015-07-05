#ifndef TAGTYPESELECTOR_H
#define TAGTYPESELECTOR_H

#include <QWidget>
#include <QRadioButton>
#include "tagpool.h"

namespace Ui {
class TagTypeSelector;
}

class TagTypeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit TagTypeSelector(QString tagName,
                             TagPool *tp,
                             QWidget *parent = 0);
    ~TagTypeSelector();
signals:
    void sigTypeChoosed(QString t, QString tagName);
    void sigTypeChanged(QString from, QString to, QString tagName);

private slots:
    void onButtonChecked();

private:
    Ui::TagTypeSelector *ui;
    QHash<QString, QRadioButton* > type_buttons_;
    QString tag_name_;
    TagPool *tp_;
    QStringList opts_;
    QString current_type_;
};

#endif // TAGTYPESELECTOR_H
