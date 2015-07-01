#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"

FileInfoWidget::FileInfoWidget(const DBSchema *schema,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoWidget),
    schema_(schema)
{
    ui->setupUi(this);
    this->populateUi();
}

FileInfoWidget::~FileInfoWidget()
{
    delete ui;
}

void FileInfoWidget::setFile(QString f)
{
    finfo.setFile(f);
    if (!finfo.isFile())
        return;

    this->setValue("name", finfo.fileName());

    this->setValue("md5", this->getHash(finfo.filePath()));

    this->setValue("size", QString::number(finfo.size()));
}

void FileInfoWidget::cacheFileHash(QString hash, QString fpath)
{
    hash_cache_.insert(fpath, hash);
}

void FileInfoWidget::updateUiFileHash(QString fpath)
{
    this->setValue("md5", this->getHash(fpath));
}

void FileInfoWidget::updateUiFileHashingPercent(int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        hash_cache_.insert(fpath, "Hashing..." + QString::number(percent) + "%");
        updateUiFileHash(fpath);
    }
}

void FileInfoWidget::setValue(QString field, QString value, bool local )
{
    QStringList sf = schema_->fields("file");
    if (!sf.contains(field))
        return;

    LRline* w = field_widgets_.value(field);
    if ( local ){
        w->setLocalValue(value);
    } else {
        w->setRemoveValue(value);
    }
}

QString FileInfoWidget::getHash(QString fpath)
{
    if (!hash_cache_.contains(fpath)) {
        hash_thread_ = new HashThread(fpath,QCryptographicHash::Md5,this);

        connect(hash_thread_, SIGNAL(finished()),
                hash_thread_, SLOT(deleteLater()));
        connect(hash_thread_, SIGNAL(sigHash(QString,QString)),
                this, SLOT(cacheFileHash(QString,QString)));
        connect(hash_thread_, SIGNAL(sigHash(QString,QString)),
                this, SLOT(updateUiFileHash(QString,QString)));
        connect(hash_thread_, SIGNAL(sigHashingPercent(int,QString)),
                this, SLOT(updateUiFileHashingPercent(int,QString)));

        hash_cache_.insert(fpath, "Hashing...");// Necessary ( guard too many threads )
        hash_thread_->start();
    }
    return hash_cache_.value(fpath);
}

void FileInfoWidget::clearCache()
{
    hash_cache_.clear();
}

void FileInfoWidget::populateUi()
{
    QStringList fields = schema_->fields("file");
    LRline* line;
    for (int i = 0; i < fields.size(); ++i){
        line = new LRline(this);
        line->setName(fields.at(i));
        ui->layout->addWidget(line);
        field_widgets_.insert(fields.at(i), line);
    }
}
