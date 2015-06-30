#include "fileexplorer.h"
#include "ui_fileexplorer.h"

FileExplorer::FileExplorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileExplorer)
{
    ui->setupUi(this);

    // path Edit
    ui->pathEdit->setText( QDir::currentPath() );
    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QDirModel(completer));
    ui->pathEdit->setCompleter(completer);
    connect(ui->pathEdit, SIGNAL(editingFinished()),
            this, SLOT(onPathEdited()));

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
    files_model_->setRootPath(ui->pathEdit->text());
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

void FileExplorer::onPathEdited()
{
    if(dir_model_->index(ui->pathEdit->text()).isValid()){
        emit sigPath(ui->pathEdit->text());
    }
}

void FileExplorer::onDirSelectChanged(QModelIndex current, QModelIndex previous)
{
    setPath( dir_model_->filePath(dir_selection_model_->currentIndex()) );
    ui->dirView->resizeColumnToContents(current.column());
}

void FileExplorer::setPath(QString path)
{
    ui->dirView->setCurrentIndex(dir_model_->index(path));
    ui->filesView->setRootIndex(files_model_->setRootPath(path));
}
