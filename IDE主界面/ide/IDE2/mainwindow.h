#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qsciapis.h>

namespace Ui {
class MainWindow;
}

//add控件

#include<QTextEdit>//Qt自带的控件

#include<QMenu>

#include<QMenuBar>

#include<QAction>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QsciScintilla *editor=new QsciScintilla(this);//文本主体

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString filename;
    QString savefilename;

private :

    QMenu *file;

    QMenu *edit;

    QMenu *build;

    QMenu *help;

    QAction *file_open;
    QAction *file_exit;
    QAction *help_about;
    QAction *edit_copy;
    QAction *hide;
    QAction *showt;
    QAction *edit_cut;

    QAction *edit_paste;
    QAction *select_all;
    QAction *file_save;
    QAction *build_compile;
    QAction *build_run;
    QAction *build_debug;
    QAction *build_next;
    QAction *build_next_b;
    QAction *build_break;
    QAction *build_check;
    QAction *build_over;
    QAction *file_save1;

private slots:
    void on_open();
    void on_about();
    void on_exit();
    void on_copy();
    void on_cut();
    void on_paste();
    void on_selectall();
    void on_save();
    void on_compile();
    void on_run();
    void Save_File();
    void begin_Debug();
    void decline_tips();
    void show_tips();
};

#endif // MAINWINDOW_H
void get_name(char *pf,int l);//柯杭
void get_cmd(char *ch,char *cmd,int mode);
void fd_copy();
