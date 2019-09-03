#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMenu *file_menu;
    QMenu *edit_menu;
    QMenu *build_menu;
    QMenu *help_menu;

    QAction *file_newAct;
    QAction *file_openAct;
    QAction *file_saveAct;
    QAction *file_saveasAct;
    QAction *edit_undoAct;
    QAction *edit_copyAct;
    QAction *edit_pasteAct;
    QAction *edit_cutAct;
    QAction *build_compileAct;
    QAction *build_runAct;
    QAction *help_aboutAct;

    QTextEdit *textEdit;
private slots:
    void Open_File();
    void Create_NewFile();
    void Save_File();
    void Save_FileAs();
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H

