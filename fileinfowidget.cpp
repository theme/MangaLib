#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"

FileInfoWidget::FileInfoWidget(SQLiteDB *db, HashPool *hp, TagPool *tp,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoWidget),
    db_(db), hp_(hp), tp_(tp)
{
    ui->setupUi(this);
    this->hide();
    ui->layout->setContentsMargins(1,1,1,1);
    ui->layout->setSpacing(1);
    ui->layout->setMargin(1);
    this->populateUi();

    connect(hp_, SIGNAL(sigHash(int,QString,QString)),
            this, SLOT(handleGotHash(int,QString,QString)));
    connect(hp_, SIGNAL(sigHashingPercent(int,int,QString)),
            this, SLOT(updateHashingProgress(int,int,QString)));
}

FileInfoWidget::~FileInfoWidget()
{
    delete ui;
}

QString FileInfoWidget::getValue(QString field, bool local)
{
    if (! db_->schema()->fields("file").contains(field) )
        return QString();

    LRline *line = field_widgets_.value(field);
    return line->value(local);
}

void FileInfoWidget::setFile(QString f)
{
    QFileInfo fi(f);
    if (!fi.isFile()){
        this->hide();
        return;
    }
    this->show();

    if (fi.filePath() != finfo_.filePath()){
        emit sigFileChanged(finfo_.filePath(), fi.filePath());
        finfo_.setFile(f);


        FileTagsWidget *w = new FileTagsWidget(tp_,this);
        w->setFileName(finfo_.fileName());
        ui->layout->addWidget(w);
        connect(this, SIGNAL(sigFileChanged(QString,QString)),
                w, SLOT(deleteLater()));
    }

    this->clearValueAll();
    // local
    this->setValue("name", finfo_.fileName());
    this->setValue("md5", hp_->getFileHash(QCryptographicHash::Md5, finfo_.filePath()));
    this->setValue("size", QString::number(finfo_.size()));
    this->setValue("timestamp", finfo_.lastModified().toString(Qt::ISODate));
    // db
    if( !this->getValue("md5").isEmpty()){
        this->updateFromDB("md5",this->getValue("md5"), finfo_.filePath());
    }
    if( !this->getValue("size").isEmpty()){
        this->updateFromDB("size",this->getValue("size"), finfo_.filePath());
    }
}

void FileInfoWidget::showHide(bool show)
{
    if (show){
        this->show();
    } else {
        this->hide();
    }
}

void FileInfoWidget::handleGotHash(int algo, QString hash, QString fpath)
{
    updateLocalValue(hp_->algoName(static_cast<QCryptographicHash::Algorithm>(algo)), hash, fpath);
    updateFromDB(hp_->algoName(static_cast<QCryptographicHash::Algorithm>(algo)),hash, fpath);
}

void FileInfoWidget::updateHashingProgress(int algo, int percent, QString fpath)
{
    if( fpath == finfo_.filePath()){
        this->setProgress(hp_->algoName(static_cast<QCryptographicHash::Algorithm>(algo)), percent);
    }
}

void FileInfoWidget::setValue(QString fieldName, QString v, bool local )
{
    QStringList sf = db_->schema()->fields("file");
    if (!sf.contains(fieldName))
        return;

    LRline* w = field_widgets_.value(fieldName);
    if ( local ){
        w->setLocalValue(v);
    } else {
        w->setRemoteValue(v);
    }

    if( !v.isEmpty() ){
        w->show();
    } else {
        w->hide();
    }
}

void FileInfoWidget::clearValueAll()
{
    QHashIterator<QString, LRline*> i(field_widgets_);
    LRline *w;
    while (i.hasNext()){
        i.next();
        w = i.value();
        w->clear();
        w->hide();
    }
}

void FileInfoWidget::setProgress(QString fieldName, int p, bool local)
{
    QStringList sf = db_->schema()->fields("file");
    if (!sf.contains(fieldName))
        return;

    LRline* w = field_widgets_.value(fieldName);
    if ( local ){
        w->setLocalProgress(p);
    } else {
        w->setRemoteProgress(p);
    }
}

void FileInfoWidget::updateFromDB(QString fieldName, QString v, QString fpath)
{
    if (!(finfo_.filePath() == fpath))
        return;

    if ( fieldName.isEmpty() || v.isEmpty() )
        return;

    QSqlRecord rec = db_->query1record("file", fieldName, v);
    if (rec.isEmpty())
        return;

    for ( int i = 0; i< rec.count(); ++i){
        this->setValue(rec.fieldName(i), rec.value(i).toString(), false);
    }
}

void FileInfoWidget::updateLocalValue(QString fieldName, QString v, QString fpath)
{
    if (!(finfo_.filePath() == fpath))
        return;
    else
        this->setValue(fieldName, v);
}

void FileInfoWidget::populateUi()
{
    QStringList fields = db_->schema()->fields("file");
    LRline* line;
    for (int i = 0; i < fields.size(); ++i){
        line = new LRline(this);
        line->setName(fields.at(i));
        ui->layout->addWidget(line);
        line->hide();
        field_widgets_.insert(fields.at(i), line);
    }
}
