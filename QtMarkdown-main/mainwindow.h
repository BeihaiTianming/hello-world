#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Document.h>
#include <QMainWindow>
#include <QWebEngineView>
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QTextDocument *document() const;
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *e) override;


private slots:/*Here we add functions that receives "connect signals"*/
    void file_New();
    void file_Open();
    bool file_Save();
    bool file_SaveAs();
    void onFileSave();
    void filePdfSave();


private:
    Ui::MainWindow *ui;
    Document m_content;

    void setupConnections();
    void setupHTMLrender();

    //File operations here
    void setup_File_Menu();
    void setup_Edit_Menu();
    void setup_Help_Menu();
    bool load_file(const QString &addr);
    void about();
    void tutorial();

    //Call this function to check whether the current file has to be saved before opening a new one
    bool wannaSave();
    void resetFileName(const QString &filename);

    //actions here
    QAction *open_file, *new_file,*save_file,*save_as_file,*quit_file,*undo,*redo;

    QString fileName;
    QTextEdit *text;//This points to ui->textEdit, for simplification
};
#endif // MAINWINDOW_H
