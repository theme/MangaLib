#ifndef FSMIXDBMODEL_H
#define FSMIXDBMODEL_H

#include <QObject>
#include <QFileSystemModel>

class FSmixDBmodel : public QFileSystemModel
{
    Q_OBJECT
public:
    FSmixDBmodel(QObject *parent = 0);
};

#endif // FSMIXDBMODEL_H
