#include "fileexplorer.h"
#include "ui_fileexplorer.h"

FileExplorer::FileExplorer(QFileSystemModel *filesmodel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileExplorer), files_model_(filesmodel)
{
    ui->setupUi(this);

    // path Edit
    ui->pathEdit->setText( QDir::homePath() );
    QCompleter *completer = new QCompleter(this);
    QStringList fli("*");
    completer->setModel(new QDirModel(fli,
                                      QDir::NoDotAndDotDot | QDir::AllDirs,
                                      QDir::Name));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->pathEdit->setCompleter(completer);

    // short cut
    sc_editpath_ = new QShortcut(QKeySequence(tr("Ctrl+L", "Edit path")),this);
    connect(sc_editpath_, SIGNAL(activated()), ui->pathEdit, SLOT(setFocus()));

    // dir View
    dir_model_ = new QFileSystemModel(this);
    dir_proxy_model_ = new FSfilterProxyModel(this);
    dir_model_->setRootPath(QDir::rootPath());
    dir_model_->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dir_proxy_model_->setSourceModel(dir_model_);
    ui->dirView->setModel(dir_proxy_model_);
    ui->dirView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->dirView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onCurrentDirChanged(QModelIndex,QModelIndex)));

    // files View
    ui->filesView->setModel(files_model_);
    ui->filesView->setItemDelegate(new RankDelegate(this));

    ui->filesView->setEditTriggers(QAbstractItemView::DoubleClicked
                                | QAbstractItemView::SelectedClicked);
    ui->filesView->setSelectionBehavior(QAbstractItemView::SelectRows);
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

QString FileExplorer::currentPath() const
{
    return ui->pathEdit->text();
}

void FileExplorer::onCurrentDirChanged(QModelIndex current, QModelIndex previous)
{
    emit sigPath( dir_model_->filePath(dir_proxy_model_->mapToSource( current)) );
}

void FileExplorer::onCurrentFileChanged(QModelIndex current, QModelIndex previous)
{
    if (files_model_->filePath(current) != files_model_->filePath(previous))
        emit sigFilePath( files_model_->filePath(current));

    if (current.column() == 0){
        emit sigFileNameSelected(true);
    } else {
        emit sigFileNameSelected(false);
    }
}

void FileExplorer::setPath(QString path)
{
    ui->pathEdit->setText(path);

    QModelIndex din = dir_proxy_model_->mapFromSource(dir_model_->index(path));
    ui->dirView->scrollTo(din);
    ui->dirView->collapse(din);
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
