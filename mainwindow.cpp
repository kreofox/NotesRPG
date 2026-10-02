#include "mainwindow.h"
#include "QTextEdit"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QWidget"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QDebug>
#include <memory>
#include <QLabel>
#include <QTreeView>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QStackedWidget>
#include <QPixmap>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("NotesRPG");
    resize(900, 600);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    central->setLayout(mainLayout);

    // Left side — file tree (VS Code style, with expand/collapse animation)
    QTreeView *fileTree = new QTreeView(central);
    mainLayout->addWidget(fileTree, 1);

    // Right side — stacked: text editor OR image preview
    QVBoxLayout *rightLayout = new QVBoxLayout();

    QStackedWidget *rightStack = new QStackedWidget(central);

    QTextEdit *editor = new QTextEdit(central);
    editor->setPlaceholderText(" ");
    editor->setFont(QFont("Courier New", 12));

    QLabel *imageLabel = new QLabel(central);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(false);

    rightStack->addWidget(editor);      // index 0 — текст
    rightStack->addWidget(imageLabel);  // index 1 — картинка

    rightLayout->addWidget(rightStack);
    mainLayout->addLayout(rightLayout, 3);

    // Notes folder — shared_ptr so every lambda sees the same, updatable path
    auto notesDir = std::make_shared<QString>(QDir::currentPath() + "/notes");
    QDir().mkpath(*notesDir);
    qDebug() << "Initial notesDir:" << *notesDir;

    // Currently opened file
    auto currentFile = std::make_shared<QString>("");

    // File system model — shows all folders and files, updates itself automatically
    QFileSystemModel *model = new QFileSystemModel(fileTree);
    model->setRootPath(*notesDir);
    model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    fileTree->setModel(model);
    fileTree->setRootIndex(model->index(*notesDir));

    fileTree->hideColumn(1);
    fileTree->hideColumn(2);
    fileTree->hideColumn(3);
    fileTree->header()->hide();

    fileTree->setAnimated(true);

    // Helper to point the tree at a (possibly new) folder
    auto setRootFolder = [=](const QString &dirPath){
        *notesDir = dirPath;
        model->setRootPath(dirPath);
        fileTree->setRootIndex(model->index(dirPath));
    };

    // Click on a file — show it (text or image). Click on a folder — expand/collapse with animation.
    connect(fileTree, &QTreeView::clicked, this, [=](const QModelIndex &index){
        QString path = model->filePath(index);
        QFileInfo info(path);

        if (info.isFile()) {
            QString ext = info.suffix().toLower();
            static const QStringList imageExts = {"png", "jpg", "jpeg", "bmp", "webp", "gif"};

            if (imageExts.contains(ext)) {
                // Освобождаем предыдущий QMovie, если был
                if (imageLabel->movie()) {
                    imageLabel->movie()->deleteLater();
                    imageLabel->setMovie(nullptr);
                }

                if (ext == "gif") {
                    QMovie *movie = new QMovie(path, QByteArray(), imageLabel);
                    imageLabel->setMovie(movie);
                    movie->start();
                } else {
                    QPixmap pix(path);
                    if (!pix.isNull()) {
                        imageLabel->setPixmap(pix.scaled(
                            imageLabel->size(),
                            Qt::KeepAspectRatio,
                            Qt::SmoothTransformation
                            ));
                    } else {
                        imageLabel->setText("Cannot preview: " + info.fileName());
                    }
                }
                rightStack->setCurrentWidget(imageLabel);
            } else {
                QFile file(path);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    editor->setText(in.readAll());
                    file.close();
                    *currentFile = path;
                } else {
                    QMessageBox::warning(this, "Error", "Could not open file: " + info.fileName());
                }
                rightStack->setCurrentWidget(editor);
            }
        } else {
            if (fileTree->isExpanded(index))
                fileTree->collapse(index);
            else
                fileTree->expand(index);
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

            QModelIndex current = fileTree->currentIndex();
            QString targetDir = *notesDir;
            if (current.isValid()) {
                QFileInfo info(model->filePath(current));
                targetDir = info.isDir() ? info.filePath() : info.absolutePath();
            }

            QString filePath = targetDir + "/" + name;
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.close();
                *currentFile = filePath;
                editor->clear();
                rightStack->setCurrentWidget(editor);
            }
        }
    });

    QAction *openFolderAction = fileMenu->addAction("Open Folder...");
    connect(openFolderAction, &QAction::triggered, this, [=](){
        QString dirPath = QFileDialog::getExistingDirectory(this, "Select folder", QDir::homePath());
        if (!dirPath.isEmpty()) {
            setRootFolder(dirPath);
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