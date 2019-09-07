#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QFileDialog"
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QtEvents>
#include <QVBoxLayout>

int flag_isOpen = 0; //判断是否打开或新建了一个文件
int flag_isNew = 0; //判断文件是否新建
QString Last_FileName;
QString Last_FileContent;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    this->setWindowTitle("IDE");
    this->setMinimumSize(1000,600); //运行后弹出界面大小，可放大

    editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE,QsciScintilla::SC_CP_UTF8);
    editor->setAutoIndent(true);
    QsciLexer *textLexer = new QsciLexerLua;
    editor->setLexer(textLexer);

    QsciAPIs *apis = new QsciAPIs(textLexer);
    //在这里可以添加自定义的自动完成函数
    //apis->add(QString("func_name(arg_1,arg_2) function info"));
    apis->prepare();
    //设置自动完成所有项
    editor->setAutoCompletionSource(QsciScintilla::AcsAll);
    //设置大小写敏感
    editor->setAutoCompletionCaseSensitivity(true);
    //每输入1个字符就出现自动完成的提示
    editor->setAutoCompletionThreshold(1);

    editor->setAutoIndent(true);
    editor->setMarginType(0,QsciScintilla::NumberMargin);
    editor->setMarginLineNumbers(0,true);
    editor->setMarginWidth(0,35);

    editor->setIndentationGuides(QsciScintilla::SC_IV_LOOKBOTH);
    editor->setCaretLineVisible(true);

    editor->setCaretLineBackgroundColor(Qt::white);
    editor->setMarginsBackgroundColor(Qt::lightGray);
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    QVBoxLayout *v=new QVBoxLayout();
    v->addWidget(editor);
    this->setLayout(v);
    setCentralWidget(editor);

    file=this->menuBar()->addMenu("文件");//在菜单栏中添加菜单项
    edit=this->menuBar()->addMenu("编辑");
    build=this->menuBar()->addMenu("构建");
    help=this->menuBar()->addMenu("帮助");
    file_open=new QAction("打开",this);//建立一个action
    file_open->setShortcut(tr("Ctrl+O"));//添加 快捷键
    file->addAction(file_open);//将file_open这个action加入到file菜单中
    file_save=new QAction("另存为",this);
    file_save->setShortcut(tr("Ctrl+P"));
    file->addAction(file_save);

    file_save1=new QAction("保存",this);
    file_save1->setShortcut(tr("Ctrl+S"));
    file->addAction(file_save1);

    file_exit=new QAction("退出",this);
    file->addSeparator();//添加分隔符
    file->addAction(file_exit);
    help_about=new QAction("关于",this);
    help_about->setShortcut(tr("Ctrl+H"));
    help->addAction(help_about);
    edit_copy=new QAction("拷贝",this);
    edit->addAction(edit_copy);
    edit_cut=new QAction("剪切",this);
    edit->addAction(edit_cut);
    edit_paste=new QAction("粘贴",this);
    edit->addAction(edit_paste);
    select_all=new QAction("select all",this);
    edit->addAction(select_all);
    build_compile=new QAction("编译",this);
    build->addAction(build_compile);
    build_run=new QAction("运行",this);
    build->addAction(build_run);
    //Qt的消息槽机制
    connect(file_open,SIGNAL(triggered()),this,SLOT(on_open()));
    //第一个参数是触发事件的控件，第二个参数是对于Action来说的固定写法
    //第三个参数固定写this，第四个参数指定点击Action后执行那个函数
    connect(help_about,SIGNAL(triggered()),this,SLOT(on_about()));
    connect(file_exit,SIGNAL(triggered()),this,SLOT(on_exit()));
    connect(edit_copy,SIGNAL(triggered()),this,SLOT(on_copy()));
    connect(edit_cut,SIGNAL(triggered()),this,SLOT(on_cut()));
    connect(edit_paste,SIGNAL(triggered()),this,SLOT(on_paste()));
    connect(select_all,SIGNAL(triggered()),this,SLOT(on_selectall()));
    connect(file_save,SIGNAL(triggered()),this,SLOT(on_save()));
    connect(build_compile,SIGNAL(triggered()),this,SLOT(on_compile()));
    connect(build_run,SIGNAL(triggered()),this,SLOT(on_run()));
    connect(file_save1,SIGNAL(triggered()),this,SLOT(Save_File()) );
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_open()
{
    filename=QFileDialog::getOpenFileName();
    //函数返回路径+文件名
    if(filename.isEmpty())
    {
        return;
    }
    QString content;//QString 是Qt封装的一个字符串类
    //QMessageBox::information(this,"提示","test");
    //filename.toStdString().data();//这个语句即可将QString类型转化为const char *类型
    FILE *p=fopen(filename.toStdString().data(),"r");//需要将QString转换为const char *
    if(p==NULL)
    {
        QMessageBox::information(this,"错误","打开文件失败");
    }
    else
    {
        while(!feof(p))
        {
            char buf[1024]={0};
            fgets(buf,sizeof(buf),p);
            content+=buf;//将读取到的内容追加到content后面
        }
        fclose(p);
        editor->setText(content);//将字符串的值放到text里面
        flag_isOpen = 1;
        Last_FileName = filename;
    }
}
void MainWindow::Save_File() //保存文件
{
    //新文件的保存
    if(flag_isNew == 1)
    {
        if(editor->text() == "") //文件内容为空，弹出提示框
        {
            QMessageBox::warning(this,tr("警告"),tr("内容不能为空!"),QMessageBox::Ok);
        }
        else
        {
            QFileDialog fileDialog;
            QString str = fileDialog.getSaveFileName(this,tr("打开文件"),"/home",tr("Text File(*.txt)"));
            if(str == "")
            {
                return;
            }
            QFile filename(str);
            if(!filename.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this,tr("错误"),tr("打开文件失败"),QMessageBox::Ok);
                return;
            }
            else
            {
                QTextStream textStream(&filename);
                QString str = editor->text();
                textStream<<str;
                Last_FileContent = str;
            }
            QMessageBox::information(this,"保存文件","保存文件成功",QMessageBox::Ok);
            filename.close();
            flag_isNew = 0;     //新文件标记位记为0
            Last_FileName = str;//保存文件内容
        }
    }
    //原有文件修改后的保存
    else
    {
        if(flag_isOpen == 1) //判断是否创建或打开了一个文件
        {
            QFile file(Last_FileName);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this,tr("警告"),tr("打开文件失败"));
                return;
            }
            else
            {
                QTextStream textStream(&file);
                QString str = editor->text();
                textStream<<str;
                Last_FileContent = str;
                file.close();
            }
        }
        else
        {
            QMessageBox::warning(this,tr("警告"),tr("请先创建或者打开文件"));
            return;
        }
    }

}
void MainWindow::on_about()
{
    QMessageBox::information(this,"帮助","Jumfens\n2010-2016");
}
void MainWindow::on_exit()
{
    exit(0);
}
void MainWindow::on_copy()
{
    editor->copy();
}
void MainWindow::on_cut()
{
    editor->cut();
}
void MainWindow::on_paste()
{
    editor->paste();
}
void MainWindow::on_selectall()
{
    editor->selectAll();
}
void MainWindow::on_save()
{
    savefilename=QFileDialog::getSaveFileName();
    if(savefilename==NULL)
    {
        return;
    }
    FILE *p=fopen(savefilename.toStdString().data(),"w");
    if(p==NULL)
    {
        QMessageBox::information(this,"错误","打开文件失败");
        return;
    }
    else
    {
        // text1->toPlainText().toStdString().data();//将用户在控件中输入的字符串转化为const char *
        fputs(editor->text().toStdString().data(),p);
        QString str = editor->text();
        fclose(p);
        Last_FileContent = str;
        Last_FileName = savefilename;
        flag_isNew = 0;
    }
}



void MainWindow::on_compile()
{
    QString content="#include<stdio.h>\n";
    FILE *p=fopen(filename.toStdString().data(),"r");
    if(p==NULL)
        return;
    while(!feof(p))
    {
        char buf[1024]={0};
        fgets(buf,sizeof(buf),p);
        content+=buf;
    }
    fclose(p);
    content.replace("111","int").replace("222","main").replace("333","{").replace("444","printf(\"hello jumfens\")").replace("8888","printf(\"\\n\");").replace("555","getchar();").replace("666","return").replace("777","}");
    QMessageBox::information(this,"显示",content);    //    return ;
    QString destfilename=filename;
    destfilename.replace(".e",".c");
    p=fopen(destfilename.toStdString().data(),"w");
    fputs(content.toStdString().data(),p);
    fclose(p);
    QString srcname=destfilename;
    srcname.replace(".c",".exe");
    QString command="gcc -o "+srcname +" "+destfilename;
    system(command.toStdString().data());
    // remove(destfilename.toStdString().data());
    //可以将中间生成的.c文件删除
}

void MainWindow::on_run()
{
    QString destfilename=filename;
    destfilename.replace(".e",".exe");
    system(destfilename.toStdString().data());
}

