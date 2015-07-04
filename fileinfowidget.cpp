#include "fileinfowidget.h"
#include "ui_fileinfowidget.h"

FileInfoWidget::FileInfoWidget(SQLiteDB *db, HashPool *hp,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileInfoWidget),
    db_(db), hp_(hp)
{
    ui->setupUi(this);
    ui->layout->setContentsMargins(1,1,1,1);
    ui->layout->setSpacing(1);
    ui->layout->setMargin(1);
    this->populateUi();

    connect(ui->update2dbButton, SIGNAL(clicked()),
            this, SLOT(update2db()));

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
    finfo.setFile(f);
    if (!finfo.isFile())
        return;

    this->clearValueAll();
    // local
    this->setValue("name", finfo.fileName());
    this->setValue("md5", hp_->getFileHash(QCryptographicHash::Md5, finfo.filePath()));
    this->setValue("size", QString::number(finfo.size()));
    this->setValue("timestamp", finfo.lastModified().toString(Qt::ISODate));
    // db
    if( !this->getValue("md5").isEmpty()){
        this->updateFromDB("md5",this->getValue("md5"));
    }
    if( !this->getValue("size").isEmpty()){
        this->updateFromDB("size",this->getValue("size"));
    }
}

void FileInfoWidget::handleGotHash(int algo, QString hash, QString fpath)
{
    updateLocalValue(hp_->algoName(static_cast<QCryptographicHash::Algorithm>(algo)), hash, fpath);
    updateFromDB(hp_->algoName(static_cast<QCryptographicHash::Algorithm>(algo)),hash);
}

void FileInfoWidget::updateHashingProgress(int algo, int percent, QString fpath)
{
    if( fpath == finfo.filePath()){
        this->setProgress(hp_->algoName(static_cast<QCryptographicHash::Algorithm>(algo)), percent);
    }
}

bool FileInfoWidget::isInDB()
{
    QSqlRecord rec = db_->query1record("file", "md5", this->getValue("md5"));
    return !rec.isEmpty();
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

bool FileInfoWidget::update2db(bool update)
{
    QStringList fields = db_->schema()->fields("file");
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

    if (update && this->isInDB()){
        return db_->update("file", keys, values, "md5", this->getValue("md5"));
    } else {
        return db_->insert("file", keys, values);
    }
}

void FileInfoWidget::updateFromDB(QString fieldName, QString v)
{
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
    if (!(finfo.filePath() == fpath))
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
        field_widgets_.insert(fields.at(i), line);
    }
}
