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
    ui->layout->setContentsMargins(1,1,1,1);
    ui->layout->setSpacing(1);
    ui->layout->setMargin(1);
    this->populateUi();
    connect(ui->save2dbButton, SIGNAL(clicked()),
            this, SLOT(save2db()));
    connect(this, SIGNAL(sigGotHash(QString,QString,QString)),
            this, SLOT(queryDB(QString,QString)));
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

    // local
    this->setValue("name", finfo.fileName());

    this->setValue("md5", this->getHash(finfo.filePath()));

    this->setValue("size", QString::number(finfo.size()));

    this->setValue("timestamp", finfo.lastModified().toString(Qt::ISODate));
}

void FileInfoWidget::cacheFileHash(QString hash, QString fpath)
{
    hash_cache_.insert(fpath, hash);
    this->setValue("md5", this->getHash(finfo.filePath()));
    emit sigGotHash("md5", hash, fpath);
}

void FileInfoWidget::updateUiFileHashingPercent(int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        this->setProgress("md5", percent);
    }
}

void FileInfoWidget::setValue(QString fieldName, QString v, bool local )
{
    QStringList sf = dbschema_->fields("file");
    if (!sf.contains(fieldName))
        return;

    LRline* w = field_widgets_.value(fieldName);
    if ( local ){
        w->setLocalValue(v);
    } else {
        w->setRemoveValue(v);
    }
}

void FileInfoWidget::setProgress(QString fieldName, int p, bool local)
{
    QStringList sf = dbschema_->fields("file");
    if (!sf.contains(fieldName))
        return;

    LRline* w = field_widgets_.value(fieldName);
    if ( local ){
        w->setLocalProgress(p);
    } else {
        w->setRemoteProgress(p);
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

    emit sigSaved2DB();
    q.finish();
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
        return QString();   // empty
    } else {
        return hash_cache_.value(fpath);
    }
}

void FileInfoWidget::clearCache()
{
    hash_cache_.clear();
}

void FileInfoWidget::queryDB(QString fieldName, QString v)
{
    if ( fieldName.isEmpty() || v.isEmpty() )
        return;

    QSqlQuery q(db_);
    QString sql = "select * from file where " + fieldName + " = " + "'" + v + "'";
    if (!q.exec(sql)){
        QString msg = "FileInfoWidget::queryFileInfo() error: "+ q.lastError().text();
        qDebug()  << msg;
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }
    if(!q.first()){
        qDebug() << "! no result. (queryFileInfo)";
        return;
    }
    QSqlRecord rec = q.record();
    if (rec.isEmpty())
        return;

    for ( int i = 0; i< rec.count(); ++i){
        this->setValue(rec.fieldName(i), rec.value(i).toString(), false);
    }
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
