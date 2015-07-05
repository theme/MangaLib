#ifndef TAGTYPESELECTOR_H
#define TAGTYPESELECTOR_H

#include <QWidget>
#include <QRadioButton>

namespace Ui {
class TagTypeSelector;
}

class TagTypeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit TagTypeSelector(QString tagName,
                             QStringList options,
                             QString current,
                             QWidget *parent = 0);
    ~TagTypeSelector();
signals:
    void sigTypeChoosed(QString t);
    void sigTypeChanged(QString from, QString to);

private slots:
    void onButtonChecked();

private:
    Ui::TagTypeSelector *ui;
    QStringList opts_;
    QHash<QString, QRadioButton* > type_buttons_;
    QString current_type_;
};

#endif // TAGTYPESELECTOR_H
