#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "previewpage.h"
#include "texthighlighter.h"

#include <QWebEngineView>
#include <iostream>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QTextStream>
#include <QWebChannel>
#include <QCloseEvent>
#include <QTextCodec>
#include <QTextDocumentWriter>
#include <QSyntaxHighlighter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    this->setFixedSize(1280, 720);

    ui->setupUi(this);
    ui->textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->preview->setContextMenuPolicy(Qt::NoContextMenu);
    text = ui->textEdit;

    new TextHighlighter(text->document());//传一个QTextDocument

    // Initialize file name
    fileName="untitled.txt";

    setWindowTitle(QCoreApplication::applicationName());
    resetFileName(QString());


    setup_File_Menu();
    setup_Edit_Menu();
    setup_Help_Menu();

    // Build connections first
    setupConnections();

    // This function must follow  SetupConnections()
    setupHTMLrender();


}

void MainWindow::setupConnections(){

    // Setup connect functions here to send signals
    connect(ui->textEdit, &QTextEdit::textChanged, [this]() { m_content.setText(ui->textEdit->toPlainText());});

    // Set redo, undo connections
    connect(text->document(), &QTextDocument::undoAvailable, undo, &QAction::setEnabled);
    connect(text->document(), &QTextDocument::redoAvailable, redo, &QAction::setEnabled);

    // Set modification status
    connect(text->document(), &QTextDocument::modificationChanged, save_file,&QAction::setEnabled);
    connect(text->document(), &QTextDocument::modificationChanged, this,&QWidget::setWindowModified);

    save_file->setEnabled(text->document()->isModified());
    undo->setEnabled(text->document()->isUndoAvailable());
    redo->setEnabled(text->document()->isRedoAvailable());
}

void MainWindow::setup_File_Menu(){

    // Add a menubar "Files"
    QMenu *menu = menuBar()->addMenu(tr("&Files"));

    new_file = menu->addAction(tr("&New"),this,&MainWindow::file_New);
    open_file = menu->addAction(tr("&Open"),this,&MainWindow::file_Open);
    menu->addSeparator();

    save_file =  menu->addAction(tr("&Save"),this,&MainWindow::file_Save);
    save_as_file = menu->addAction(tr("&Save As..."),this,&MainWindow::file_SaveAs);
    menu->addSeparator();

    // This calls a function in QWidget::close
    quit_file = menu->addAction(tr("&Quit"),this,&QWidget::close);

    // Now let's bind keyboard shortcuts
    new_file->setShortcut(QKeySequence::New);
    open_file->setShortcut(QKeySequence::Open);
    save_file->setShortcut(QKeySequence::Save);
    save_as_file ->setShortcut(Qt::ALT + Qt:: CTRL + Qt:: Key_S);
    quit_file -> setShortcut(Qt::CTRL + Qt::Key_W);

    // The action save_file is a little bit complicated here, since it will be gray when theres no modification
    save_file->setEnabled(false);


}

void MainWindow::setup_Edit_Menu(){

    // Add a menubar "edit"
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    undo = menu->addAction(tr("&Undo"),text,&QTextEdit::undo);
    redo = menu->addAction(tr("&Redo"),text,&QTextEdit::redo);
    menu ->addSeparator();

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);
}

void MainWindow::setup_Help_Menu(){
   QMenu *help_menu = menuBar()->addMenu(tr("Help"));
    help_menu -> addAction(tr("About"),this,&MainWindow::about);
    help_menu -> addAction(tr("Tutorial"),this,&MainWindow::tutorial);
}

void MainWindow::setupHTMLrender(){

     // create a previewpage to render md files in html
    PreviewPage *previewWidget = new PreviewPage(ui->preview);

    ui->preview->setPage(previewWidget);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    previewWidget->setWebChannel(channel);

    ui->preview->setUrl(QUrl("qrc:/index.html"));

    ui->textEdit->setPlainText("# Qt Markdown Editor v1.0\n\n"
                                "This is a MarkDown Editor based on the open source C++ platform Qt.\nThis project is inspired by the official Qt MarkDown Editor.\n\n"
                                "## Developers\n\n"
                                "- Lin Yuanlin\n"
                                "- Zhen Huanhuan\n"
                                "- Cao Weijie\n"
                                "- Liu Jiahao\n"
                                "- Wang Yongjian\n\n"
                                "## What is Markdown\n\n "
                                "MarkDown is a light weight hyper text format. With markdown, you can easily create beatiful article.\n\n"
                                "## How to use\n\n"
                                "Left window is the text editor, you can input markdown text in it.\n"
                                "The right window is the preview area, you can see the markdown text are render in real-time.\n\n"
                                "# This is H1\n\n"
                                "## This is H2\n\n"
                                "### This is H3\n\n"
                                "#### This is H4\n\n"
                                "###### This is H5\n\n"
                                "```\nthis is code block\n```");
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::wannaSave(){

    // If the file is not modified, we don't have to save it again
    if(! text->document()->isModified()) return true;

    // Create a message box button
    const QMessageBox::StandardButton close_msg =
            QMessageBox::warning(this, QCoreApplication::applicationName(),
                                 tr("The file has been modified,\n"
                                    "Would you like to save tha changes?"),
                                 QMessageBox::Save | QMessageBox:: Discard | QMessageBox ::Cancel);

    switch(close_msg){
       case QMessageBox::Save : return file_Save();
       case QMessageBox::Discard : return true;
       case QMessageBox::Cancel : return false;
       default: return false;
    }
}

void MainWindow::resetFileName(const QString &filename){
    fileName = filename;
    QString tempname ;
    if(fileName.isEmpty()){
        //If enters an empty filename, we will not rename it
        tempname= "Untitled.md";
    }
    else tempname = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(tempname,QCoreApplication::applicationName()));
    setWindowModified(false);
}

void MainWindow::file_New(){
    if(wannaSave()){

        //Only if the previous file has been dealt with (saved/ discarded or just cancelled the operation)
        text->clear();

        //Default file name
        resetFileName(QString());
    }
}

//To be finished
void MainWindow::file_Open(){

    if(wannaSave()){
        QFileDialog file_diag(this,tr("Open File..."));

        //Open file configurations:
        file_diag.setAcceptMode(QFileDialog::AcceptOpen);
        file_diag.setFileMode(QFileDialog::ExistingFile);
        QString addr = QFileDialog::getOpenFileName(this,tr("Open Document"),
        QDir::currentPath(),tr("MarkDown files (*.md), txt(*.txt)"));

        if(load_file(addr)) {
            statusBar()->showMessage(tr("Opened file \"%1\"").arg(QDir::toNativeSeparators(addr)));
            return;
        }

        statusBar()->showMessage(tr("Cannot open file \"%1\"").arg(QDir::toNativeSeparators(addr)));
    }
}

bool MainWindow::load_file(const QString &addr){

    if(QFile::exists(addr)){

        //Now load a file
        QFile f(addr);

        if(!f.open(QFile::ReadOnly))
            return false;

        QByteArray textb = f.readAll();

        //QT read file routine
        QTextCodec *codec= Qt::codecForHtml(textb);
        QString str=codec->toUnicode(textb);

        if (Qt::mightBeRichText(str)) {
            text->setHtml(str);
        } else {
            str = QString::fromLocal8Bit(textb);
            text->setPlainText(str);
        }

        //Update file name
        resetFileName(addr);
        return true;
    }

    return false;
}

bool MainWindow::file_Save(){

    //We detect whether a file is a new file by checking for its name
    if(fileName.isEmpty())  return file_SaveAs();
    else if(fileName.startsWith(QStringLiteral(":/"))) return file_SaveAs();

    //Normal condition
    QTextDocumentWriter write(fileName);
    if(write.write(text->document())){
        text->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
        return true;
    }

    statusBar()->showMessage(tr("Failed to write \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    return false;
}

bool MainWindow::file_SaveAs(){

    //Open a new dialogue
    QFileDialog dialog(this,tr("Saving your file as..."));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QString filename = QFileDialog::getSaveFileName(this,tr("Open Document"),
    "","TXT(*.txt);;MarkDown(*.md);;PDF(*.pdf)");

    resetFileName(filename);

    if(!filename.isEmpty()) {
        int nIndex = fileName.lastIndexOf('.');
        nIndex++;
        int nLen = fileName.length() - nIndex;
        QString strSuffix = fileName.right(nLen);

        if(QString::compare("txt",strSuffix)==0)
            return file_Save();

        if(QString::compare("md",strSuffix)==0) {
            fileName = filename;
            onFileSave();
            return true;
            }
        if (QString::compare("pdf",strSuffix)==0){
            fileName =filename;
            filePdfSave();

            }

    }

     return false;
}

void MainWindow::filePdfSave(){

    QPrinter pic_printer(QPrinter::ScreenResolution);           //Set resolution
    pic_printer.setPageSize(QPrinter::A4);                  //Set paper size defaultA4
    pic_printer.setOutputFormat(QPrinter::PdfFormat);       //Set pdf form

    if(!fileName.isEmpty())
    {
        //Add suffix automatically
        if(QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        pic_printer.setOutputFileName(fileName);
        ui->preview->page()->printToPdf(fileName);

    }
    else
    QMessageBox::warning(this,tr("Path"),tr("You did not select any file."));
}

void MainWindow::onFileSave()
{
    if(fileName.isEmpty()){
        file_SaveAs();
        return;
    }

    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(fileName), f.errorString()));
        return;
    }

    QTextStream str(&f);
    str << ui->textEdit->toPlainText();

    ui->textEdit->document()->setModified(false);
}


void MainWindow::closeEvent(QCloseEvent *event){

    //If the file can be closed, then close it
    if (wannaSave()) {
        event->accept();
        return;
    }

    //If the file is yet to be saved, then ignore the event
    event->ignore();
}


void MainWindow::about(){
    QString about_docu = "This is a MarkDown Editor based on the open source C++ platform Qt.\n"
                         "This project is inspired by the official Qt MarkDown Editor. \n\n"
                         "Developers: \nLin Yuanlin, Zhen Huanhuan, Cao Weijie, Liu Jiahao, Wang Yongjian.";
    QMessageBox:: about(this, tr("About"), about_docu);
}

void MainWindow::tutorial(){
    QString tutorial_docu = "MarkDown is a light weight text editor.\n"
                            "\n"
                            "Left window is the text editor, you can input markdown text in it.\n"
                            "The right window is the preview area, you can see the markdown text are render in real-time.";
    QMessageBox::about(this, tr("Tutorial"), tutorial_docu);
}



