#include "mainwindow.h"
#include "QTextEdit"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QWidget"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QListWidget>
#include <QDir>
#include <QInputDialog>

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
    fileList -> setMaximumHeight(220);
    mainLayout -> addWidget(fileList);

    //Right text edit

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QTextEdit *editor = new QTextEdit(central);
    editor -> setPlaceholderText(" ");
    editor -> setFont(QFont("Courier new", 12));
    rightLayout -> addWidget(editor);

    mainLayout->addLayout(rightLayout);

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
