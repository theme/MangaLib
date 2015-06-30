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
    connect(sc_editpath_, SIGNAL(activated()),
            ui->pathEdit, SLOT(setFocus()));

    // dir View
    dir_model_ = new QFileSystemModel(this);
    dir_model_->setRootPath(QDir::rootPath());
    dir_model_->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->dirView->setModel(dir_model_);
    dir_selection_model_ = ui->dirView->selectionModel();
    connect(dir_selection_model_, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onDirSelectChanged(QModelIndex,QModelIndex)));

    // files View
    files_model_ = new QFileSystemModel(this);
    files_model_->setRootPath(QDir::rootPath());
    files_model_->setFilter(QDir::Files);
    files_model_->setNameFilters(QStringList()
                                 << "*.zip"
                                 << "*.cb?"
                                 << "*.rar");
    files_model_->setNameFilterDisables(false);
    ui->filesView->setModel(files_model_);

    // handle path change
    connect(this, SIGNAL(sigPath(QString)), this, SLOT(setPath(QString)));
}

FileExplorer::~FileExplorer()
{
    delete ui;
}

void FileExplorer::onDirSelectChanged(QModelIndex current, QModelIndex previous)
{
    emit sigPath( dir_model_->filePath(dir_selection_model_->currentIndex()) );
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
