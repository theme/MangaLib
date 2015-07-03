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

    // init cache
    QStringList fields = dbschema_->fields("file");
    for (int i=0; i<fields.size(); ++i){
        caches_.insert(fields.at(i), QHash<QString, QString>());
    }

    //
    connect(ui->save2dbButton, SIGNAL(clicked()),
            this, SLOT(save2db()));
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

    // local
    this->setValue("name", finfo.fileName());
    this->setValue("md5", this->getHash("md5", finfo.filePath()));
    this->setValue("size", QString::number(finfo.size()));
    this->setValue("timestamp", finfo.lastModified().toString(Qt::ISODate));
}

void FileInfoWidget::cacheFileHash(QString algo, QString hash, QString fpath)
{
    QHash<QString, QString> h = caches_.value(algo);
    h.insert(fpath,hash);
    emit sigGotHash(algo, hash, fpath);
}

void FileInfoWidget::updateHashingProgress(QString algo, int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        this->setProgress(algo, percent);
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

QString FileInfoWidget::getHash(QString algo, QString fpath)
{
    QHash<QString, QString> h = caches_.value(algo);
    if (!h.contains(fpath)) {
        hash_thread_ = new HashThread(QCryptographicHash::Md5,fpath,this);

        connect(hash_thread_, SIGNAL(finished()),
                hash_thread_, SLOT(deleteLater()));
        connect(hash_thread_, SIGNAL(sigHash(QString,QString,QString)),
                this, SLOT(cacheFileHash(QString,QString,QString)));
        connect(hash_thread_, SIGNAL(sigHashingPercent(QString, int,QString)),
                this, SLOT(updateHashingProgress(QString, int,QString)));

        h.insert(fpath, "Hashing...");// Necessary ( guard too many threads obj )
        hash_thread_->start();
        return QString();   // empty
    } else {
        return h.value(fpath);
    }
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
