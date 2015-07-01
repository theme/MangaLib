#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"

FileInfoWidget::FileInfoWidget(const DBSchema *schema, QSqlDatabase &db,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoWidget),
    dbschema_(schema),
    db_(db)
{
    ui->setupUi(this);
    this->populateUi();
    connect(ui->save2dbButton, SIGNAL(clicked()),
            this, SLOT(save2db()));
}

FileInfoWidget::~FileInfoWidget()
{
    delete ui;
}

QString FileInfoWidget::getValue(QString field, bool local)
{
    if (! dbschema_->fields("file").contains(field) )
        return QString();

    LRline *line = field_widgets_.value(field);
    return line->value(local);
}

void FileInfoWidget::setFile(QString f)
{
    finfo.setFile(f);
    if (!finfo.isFile())
        return;

    this->setValue("name", finfo.fileName());

    this->setValue("md5", this->getHash(finfo.filePath()));

    this->setValue("size", QString::number(finfo.size()));

    this->setValue("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
}

void FileInfoWidget::cacheFileHash(QString hash, QString fpath)
{
    hash_cache_.insert(fpath, hash);
    this->setValue("md5", this->getHash(finfo.filePath()));
}

void FileInfoWidget::updateUiFileHashingPercent(int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        hash_cache_.insert(fpath, "Hashing..." + QString::number(percent) + "%");
        this->setValue("md5", this->getHash(finfo.filePath()));
    }
}

void FileInfoWidget::setValue(QString field, QString value, bool local )
{
    QStringList sf = dbschema_->fields("file");
    if (!sf.contains(field))
        return;

    LRline* w = field_widgets_.value(field);
    if ( local ){
        w->setLocalValue(value);
    } else {
        w->setRemoveValue(value);
    }
}

QSqlError FileInfoWidget::save2db()
{
    // gen sql
    QStringList fields = dbschema_->fields("file");
    QString k,v;
    QStringList keys, values;
    for (int i = 0; i < fields.size(); ++i){
        k = fields.at(i);
        v = this->getValue(k);
        if (!v.isEmpty()){
            keys.append("'" + k + "'");
            values.append("'" + v + "'");
        }
    }
    QString sql = "INSERT INTO file (" + keys.join(",") + ") ";
    sql += "VALUES (" + values.join(",") + ")";
    QSqlQuery q(db_);

    if (!q.exec(sql)){
        QString msg = "insert to |file| error: "+ q.lastError().text();
        qDebug()  << msg;
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
        return q.lastError();
    }

    emit sigSaved();

    return QSqlError();
}

QString FileInfoWidget::getHash(QString fpath)
{
    if (!hash_cache_.contains(fpath)) {
        hash_thread_ = new HashThread(fpath,QCryptographicHash::Md5,this);

        connect(hash_thread_, SIGNAL(finished()),
                hash_thread_, SLOT(deleteLater()));
        connect(hash_thread_, SIGNAL(sigHash(QString,QString)),
                this, SLOT(cacheFileHash(QString,QString)));
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
    QStringList fields = dbschema_->fields("file");
    LRline* line;
    for (int i = 0; i < fields.size(); ++i){
        line = new LRline(this);
        line->setName(fields.at(i));
        ui->layout->addWidget(line);
        field_widgets_.insert(fields.at(i), line);
    }
}
