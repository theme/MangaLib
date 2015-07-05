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
    void sigTypeChoosed(QString tagName, QString type, QString oldType = QString());

private slots:
    void onButtonToggled(bool checked);

private:
    Ui::TagTypeSelector *ui;
    QHash<QString, QRadioButton* > type_buttons_;
    QString tag_name_;
    TagPool *tp_;
    QStringList opts_;
    QString current_type_;
};

#endif // TAGTYPESELECTOR_H
