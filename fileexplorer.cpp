#include "fileexplorer.h"
#include "ui_fileexplorer.h"

FileExplorer::FileExplorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileExplorer)
{
    ui->setupUi(this);

    // path Edit
    ui->pathEdit->setText( QDir::homePath() );
    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->pathEdit->setCompleter(completer);

    // short cut
    sc_editpath_ = new QShortcut(QKeySequence(tr("Ctrl+L", "Edit path")),this);
    connect(sc_editpath_, SIGNAL(activated()), ui->pathEdit, SLOT(setFocus()));

    // dir View
    dir_model_ = new QFileSystemModel(this);
    dir_model_->setRootPath(QDir::rootPath());
    dir_model_->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->dirView->setModel(dir_model_);
    ui->dirView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->dirView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onCurrentDirChanged(QModelIndex,QModelIndex)));
    // files View
    files_model_ = new FSmixDBmodel(this);
    files_model_->setRootPath(QDir::rootPath());
    files_model_->setFilter(QDir::Files);
    files_model_->setNameFilters(QStringList()
                                 << "*.zip"
                                 << "*.cb?"
                                 << "*.rar");
    files_model_->setNameFilterDisables(false);
    files_model_->setReadOnly(false);
    ui->filesView->setModel(files_model_);
    ui->filesView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->filesView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onCurrentFileChanged(QModelIndex,QModelIndex)));

    // handle path change
    connect(this, SIGNAL(sigPath(QString)), this, SLOT(setPath(QString)));
}

FileExplorer::~FileExplorer()
{
    delete ui;
}

void FileExplorer::onCurrentDirChanged(QModelIndex current, QModelIndex previous)
{
    emit sigPath( dir_model_->filePath(current) );
}

void FileExplorer::onCurrentFileChanged(QModelIndex current, QModelIndex previous)
{
    emit sigFilePath( files_model_->filePath(current));
}

void FileExplorer::setPath(QString path)
{
    ui->pathEdit->setText(path);

    QModelIndex din = dir_model_->index(path);
    ui->dirView->setCurrentIndex(din);
    ui->dirView->resizeColumnToContents(din.column());

    ui->filesView->setRootIndex(files_model_->setRootPath(path));
}

void FileExplorer::on_pathEdit_editingFinished()
{
    finfo_.setFile(ui->pathEdit->text());
    if (finfo_.isDir()) {
        emit sigPath(finfo_.filePath());
    } else if ( finfo_.isFile()) {
        emit sigPath(finfo_.path());
        emit sigFilePath(finfo_.filePath());
    }
}
