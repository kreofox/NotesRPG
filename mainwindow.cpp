#include "mainwindow.h"
#include "QTextEdit"
#include "QPushButton"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QWidget"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QListWidget>
#include <QDir>
#include <QInputDialog>
#include <QIcon>
#include <QFileDialog>

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
    QListWidget *fileList = new QListWidget(central);
    //fileList -> setMaximumHeight(280);
    mainLayout -> addWidget(fileList, 1);


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

    connect(btnNew, &QPushButton::clicked, this, [this](){
        QString filePath = QFileDialog::getOpenFileName(this, "File seelection", QDir::currentPath());
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

    //File save
    QString notesDir = QDir::currentPath() + "/notes";
    QDir().mkpath(notesDir);

    //open file
    QString currentFile = "";

    //Fun update list files left
    auto refreshList = [=](){
        fileList->clear();
        QDir dir(notesDir);
        QStringList files = dir.entryList(QStringList() << "*.txt" << "*.md", QDir::Files);
        fileList ->addItems(files);
    };

    refreshList();
}

MainWindow::~MainWindow()
{
}
