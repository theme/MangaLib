#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"

FileInfoWidget::FileInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoWidget)
{
    ui->setupUi(this);
    ui->lhashProgress->hide();
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
    ui->lhashProgress->setMinimum(0);
    ui->lhashProgress->setMaximum(100);

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
        ui->lhashProgress->hide();
    }
}

void FileInfoWidget::updateUiFileHashingPercent(int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        ui->lhashProgress->setValue(percent);
        ui->lhashProgress->show();
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
