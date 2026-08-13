#include "mainwindow.h"
#include "QTextEdit"
#include "QPushButton"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QWidget"
#include <QDir>
#include <QIcon>
#include <QFileDialog>
#include <QListView>
#include <QFileSystemModel>
#include <QDebug>


//#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{
    setWindowTitle("NotesRPG");
    resize(900, 600);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    central->setLayout(mainLayout);

    //Left list notes
    QListView *fileList = new QListView(central); //QListWidget
    mainLayout -> addWidget(fileList, 1);

    QFileSystemModel *model = new QFileSystemModel(fileList);
    QString path = QDir::homePath();
    model ->setRootPath(path);

    fileList->setModel(model);
    fileList->setRootIndex(model->index(path));



    //Right text edit

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QTextEdit *editor = new QTextEdit(central);
    editor -> setPlaceholderText(" ");
    editor -> setFont(QFont("Courier new", 12)); //Font
    rightLayout -> addWidget(editor);

    //BUTTON
    //new file
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnNew = new QPushButton(" ", this);
    btnNew->setFixedSize(45,45);
    //btnLayout->addWidget(btnNew);
    btnNew->setIcon(QIcon(":/img/img/folder.png"));
    btnNew->setIconSize(QSize(28,28));
    btnNew->setStyleSheet(
        "QPushButton {"
        "   padding: 0px;"
        "   border: none;"
        "   text-align: center;"
        "}"
        );

    connect(btnNew, &QPushButton::clicked, this, [this, model, fileList](){
        QString dirPath = QFileDialog::getExistingDirectory(this, "Select folder", QDir::homePath());
        if (!dirPath.isEmpty()) {
            model->setRootPath(dirPath);
            fileList->setRootIndex(model->index(dirPath));
            qDebug() << "Selected folder:" << dirPath;
        }
    });

    //save file
    QPushButton *btnSave = new QPushButton(" ", this);
    btnSave->setFixedSize(45,45);
    //btnLayout->addWidget(btnSave);
    btnSave->setIcon(QIcon(":/img/img/save.png"));
    btnSave->setIconSize(QSize(28,28));
    btnSave->setStyleSheet(
        "QPushButton {"
        "   padding: 0px;"
        "   border: none;"
        "   text-align: center;"
        "}"
        );

    connect(btnSave, &QPushButton::clicked, this,[this](){
        QString fileSave =QFileDialog::getSaveFileName(this, "Save", QDir::currentPath());
    });



    //setting buttons
    QPushButton *btnSettings = new QPushButton(" ", this);
    btnSettings->setFixedSize(45,45);
    //btnLayout->addWidget(btnSave);
    btnSettings->setIcon(QIcon(":/img/img/settings.png"));
    btnSettings->setIconSize(QSize(28,28));
    btnSettings->setStyleSheet(
        "QPushButton {"
        "   padding: 0px;"
        "   border: none;"
        "   text-align: center;"
        "}"
        );





    btnLayout->addWidget(btnNew);
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnSettings);





    rightLayout->addLayout(btnLayout);

    mainLayout->addLayout(rightLayout, 3);


}

MainWindow::~MainWindow()
{
}
