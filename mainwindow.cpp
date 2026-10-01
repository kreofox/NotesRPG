#include "mainwindow.h"
#include "QTextEdit"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QWidget"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QListWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QDebug>
#include <memory>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("NotesRPG");
    resize(900, 600);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    central->setLayout(mainLayout);

    // Left side — file list
    QListWidget *fileList = new QListWidget(central);
    mainLayout->addWidget(fileList, 1);

    // Right side — text editor
    QVBoxLayout *rightLayout = new QVBoxLayout();
    QTextEdit *editor = new QTextEdit(central);
    editor->setPlaceholderText(" ");
    editor->setFont(QFont("Courier New", 12));
    rightLayout->addWidget(editor);
    mainLayout->addLayout(rightLayout, 3);

    // Notes folder — shared_ptr so every lambda sees the same, updatable path
    auto notesDir = std::make_shared<QString>(QDir::currentPath() + "/notes");
    QDir().mkpath(*notesDir);
    qDebug() << "Initial notesDir:" << *notesDir;

    // Currently opened file
    auto currentFile = std::make_shared<QString>("");

    // Function to refresh the file list — shows folders and all files
    auto refreshList = [=](){
        fileList->clear();
        QDir dir(*notesDir);

        QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
        for (const QFileInfo &info : entries) {
            QString displayName = info.fileName();
            if (info.isDir())
                displayName = "📁 " + displayName;
            fileList->addItem(displayName);
        }

        qDebug() << "Looking in:" << dir.absolutePath();
        qDebug() << "Found entries:" << entries.size();
    };
    refreshList(); // initial load

    // Click on a file or folder in the list
    connect(fileList, &QListWidget::itemClicked, this, [=](QListWidgetItem *item){
        QString name = item->text();
        bool isDir = name.startsWith("📁 ");
        if (isDir)
            name = name.mid(2).trimmed(); //Remove the prefix icon

        QString fullPath = *notesDir + "/" + name;

        if (isDir) {
            // Open the folder
            *notesDir = fullPath;
            refreshList();
        } else {
            // Open the file in an editor
            QFile file(fullPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                editor->setText(in.readAll());
                file.close();
                *currentFile = fullPath;
            } else {
                QMessageBox::warning(this, "Error", "Could not open file: " + name);
            }
        }
    });

    // === File menu ===
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QMenu *aboutMenu = menuBar()->addMenu("&About");

    QAction *newAction = fileMenu->addAction("New File");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, [=](){
        bool ok;
        QString name = QInputDialog::getText(this, "New File", "File name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            if (!name.endsWith(".txt") && !name.endsWith(".md"))
                name += ".txt";
            QString filePath = *notesDir + "/" + name;
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.close();
                *currentFile = filePath;
                editor->clear();
                refreshList();
            }
        }
    });


    QAction *openFolderAction = fileMenu->addAction("Open Folder...");
    connect(openFolderAction, &QAction::triggered, this, [=](){
        QString dirPath = QFileDialog::getExistingDirectory(this, "Select folder", QDir::homePath());
        if (!dirPath.isEmpty()) {
            *notesDir = dirPath;
            refreshList();
            qDebug() << "Selected folder:" << dirPath;
        }
    });

    QAction *saveAction = fileMenu->addAction("Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, [=](){
        if (currentFile->isEmpty()) {
            QString filePath = QFileDialog::getSaveFileName(this, "Save", *notesDir, "Text files (*.txt *.md)");
            if (filePath.isEmpty()) return;
            *currentFile = filePath;
        }
        QFile file(*currentFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << editor->toPlainText();
            file.close();
            refreshList();
        } else {
            QMessageBox::warning(this, "Error", "Could not save file.");
        }
    });
    QAction *aboutAction = aboutMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, [this](){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("About Kreofox");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(
            "<h3>MIT License</h3>"
            "<p>Copyright (c) 2026 Kreofox</p>"
            "<p>Permission is hereby granted, free of charge, to any person obtaining a copy "
            "of this software and associated documentation files (the \"Software\"), to deal "
            "in the Software without restriction, including without limitation the rights "
            "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
            "copies of the Software, and to permit persons to whom the Software is "
            "furnished to do so, subject to the following conditions:</p>"
            "<p>The above copyright notice and this permission notice shall be included in all "
            "copies or substantial portions of the Software.</p>"
            "<p>THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
            "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
            "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
            "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
            "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
            "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
            "SOFTWARE.</p>"
            "<p>Copyright © 2026. All rights reserved.</p>"
            "<p>GITHUB: <a href=\"https://github.com/kreofox\">github.com/kreofox</a></p>"
            );

        QLabel *label = msgBox.findChild<QLabel*>("qt_msgbox_label");
        if (label) {
            label->setOpenExternalLinks(true);
            label->setTextInteractionFlags(Qt::TextBrowserInteraction);
        }

        msgBox.exec();
    });

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("Exit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

MainWindow::~MainWindow()
{
}
