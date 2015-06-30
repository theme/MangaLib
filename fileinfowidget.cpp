#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"

FileInfoWidget::FileInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoWidget)
{
    ui->setupUi(this);
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

    ui->lname->setText(finfo.fileName());

    ui->lhash->setText(this->getHash(finfo.filePath()));
    ui->lsize->setText(QString::number(finfo.size()));
}

void FileInfoWidget::cacheFileHash(QString hash, QString fpath)
{
    hash_cache_.insert(fpath, hash);
}

void FileInfoWidget::updateUiFileHash(QString hash, QString fpath)
{
    if (finfo.filePath() == fpath){
        ui->lhash->setText(this->getHash(fpath));
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

        hash_cache_.insert(fpath, "Hashing...");// Necessary ( guard too many threads )
        hash_thread_->start();
    }
    return hash_cache_.value(fpath);
}

void FileInfoWidget::clearCache()
{
    hash_cache_.clear();
}
