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

    connect(ui->update2dbButton, SIGNAL(clicked()),
            this, SLOT(update2db()));
    connect(this, SIGNAL(sigGotHash(QString,QString,QString)),
            this, SLOT(updateLocalValue(QString,QString,QString)));
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

    this->clearValueAll();
    // local
    this->setValue("name", finfo.fileName());
    this->setValue("md5", this->getHash("md5", finfo.filePath()));
    this->setValue("size", QString::number(finfo.size()));
    this->setValue("timestamp", finfo.lastModified().toString(Qt::ISODate));
}

void FileInfoWidget::cacheFileHash(QString algo, QString hash, QString fpath)
{
    hash_cache_.insert(algo+fpath, hash);
    emit sigGotHash(algo, hash, fpath);
}

void FileInfoWidget::updateHashingProgress(QString algo, int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        this->setProgress(algo, percent);
    }
}

bool FileInfoWidget::isInDB()
{
    QString v = this->getValue("md5");
    QSqlRecord rec = queryDB("md5", v);
    return !rec.isEmpty();
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

void FileInfoWidget::clearValueAll()
{
    QHashIterator<QString, LRline*> i(field_widgets_);
    LRline *w;
    while (i.hasNext()){
        i.next();
        w = i.value();
        w->clear();
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

QSqlError FileInfoWidget::update2db(bool update)
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

    QString sql;
    if (update){
        sql = " UPDATE file ";
        sql += " SET ";
        QStringList assigns;
        for (int i =0; i< keys.size(); ++i){
            assigns.append(" " + keys.at(i) + " = " + values.at(i) + " ");
        }
        sql += assigns.join(",");
        sql += " WHERE md5 = '" + this->getValue("md5") + "'";
    } else { //insert
        sql = "INSERT INTO file (" + keys.join(",") + ") ";
        sql += "VALUES (" + values.join(",") + ")";
    }

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

QString FileInfoWidget::getHash(QString algo, QString fpath)
{
    if (!hash_cache_.contains(algo+fpath)) {
        hash_thread_ = new HashThread(QCryptographicHash::Md5,fpath,this);

        connect(hash_thread_, SIGNAL(finished()),
                hash_thread_, SLOT(deleteLater()));
        connect(hash_thread_, SIGNAL(sigHash(QString,QString,QString)),
                this, SLOT(cacheFileHash(QString,QString,QString)));
        connect(hash_thread_, SIGNAL(sigHashingPercent(QString, int,QString)),
                this, SLOT(updateHashingProgress(QString, int,QString)));

        hash_cache_.insert(algo+fpath, "");// Necessary ( guard too many threads obj )
        hash_thread_->start();
    }
    return hash_cache_.value(algo+fpath);
}

QSqlRecord FileInfoWidget::queryDB(QString fieldName, QString v)
{
    if ( fieldName.isEmpty() || v.isEmpty() )
        return QSqlRecord();

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
        return QSqlRecord();
    }
    QSqlRecord rec = q.record();
    if (rec.isEmpty())
        return QSqlRecord();

    for ( int i = 0; i< rec.count(); ++i){
        this->setValue(rec.fieldName(i), rec.value(i).toString(), false);
    }
    return rec;
}

void FileInfoWidget::updateLocalValue(QString fieldName, QString v, QString fpath)
{
    if (!(finfo.filePath() == fpath))
        return;
    else
        this->setValue(fieldName, v);
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
