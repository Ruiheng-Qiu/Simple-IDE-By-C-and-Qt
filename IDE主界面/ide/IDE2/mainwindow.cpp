#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QFileDialog"
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QtEvents>
#include <QVBoxLayout>
#include <Qsci/qscilexercpp.h>


int flag_isOpen = 0; //判断是否打开或新建了一个文件
int flag_isNew = 0; //判断文件是否新建
QString Last_FileName;
QString Last_FileContent;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    this->setWindowTitle("Simple C! :)");
    this->setMinimumSize(1000,600); //运行后弹出界面大小，可放大

    editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE,QsciScintilla::SC_CP_UTF8);//设置UTF-8编码
    QsciLexer *textLexer=new QsciLexerCPP;
    editor->setLexer(textLexer);//设置词法器检测单词

    QsciAPIs *apis = new QsciAPIs(textLexer);
    apis->prepare();
    editor->setAutoCompletionSource(QsciScintilla::AcsAll);//设置自动完成所有项
    editor->setAutoCompletionCaseSensitivity(true);//大小写敏感
    editor->setAutoCompletionThreshold(1);//每输入1个字符就出现自动完成的提示

    editor->setAutoIndent(true);//自动缩进

    editor->setMarginType(0,QsciScintilla::NumberMargin);
    editor->setMarginLineNumbers(0,true);
    editor->setMarginWidth(0,35);//设置行号计数

    editor->setIndentationGuides(QsciScintilla::SC_IV_LOOKBOTH);
    editor->setCaretLineVisible(true);//选中行颜色可见

    editor->setCaretLineBackgroundColor(QColor(187,255,255));//选中所在行颜色
    editor->setMarginsBackgroundColor(Qt::lightGray);//行号颜色
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);

    QVBoxLayout *v=new QVBoxLayout();
    v->addWidget(editor);
    this->setLayout(v);
    setCentralWidget(editor);//将editor置于窗口中心

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
/*
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
*/
void MainWindow::on_open() //打开文件
{

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("打开文件"),tr(""),tr("Text File (*.txt)")); //打开文件的默认路径，第四个参数为对话框的后缀名过滤器
    if(fileName == "") //考虑用户取消的情况
    return;
    else
    {
       QFile file(fileName);
       if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           QMessageBox::warning(this,tr("错误"),tr("打开文件失败"));
           return;
       }
       else
       {
           if(!file.isReadable())
           {
               QMessageBox::warning(this,tr("错误"),tr("该文件不可读"));
           }
           else
           {
               QTextStream textStream(&file); //读取文件，使用QTextStream
               while(!textStream.atEnd())
               {
                   editor->setText(textStream.readAll());
               }
               editor->show();
               file.close();
               flag_isOpen = 1;
               Last_FileName = fileName;
           }
       }
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
        QMessageBox::information(this,"错误","保存文件失败");
        return;
    }
    else
    {
        fputs(editor->text().toStdString().data(),p);
        QString str = editor->text();
        fclose(p);
        Last_FileContent = str;
        Last_FileName = savefilename;
        flag_isNew = 0;
    }
}

void MainWindow::precomp()//预编译
{
    FILE *p = fopen(filename.toStdString().data(),"r");
    if(p == NULL) return ;
    QString cmd = filename +".c";//准备写入cmd的命令
    FILE *p1 = fopen(cmd.toStdString().data(),"w");//写入命令
    if(p1 == NULL) return ;
    QString str;
    while(!feof(p))
    {
        char buf[1024] = {0};
        fgets(buf,sizeof(buf),p);
        str += buf;//拼接文本内容
    }

    fputs(str.toStdString().data(),p1);
    fclose(p);
    fclose(p1);//记得关闭文件！
}


void MainWindow::on_compile()
{
    if (flag_isNew == true)//如果是新文件，编译就另存为
        {
            on_save();
        }
    else {//不是则先保存再编译
     flag_isOpen = 1;
     Save_File();
    }
        precomp();//预编译
        QString cmd;
        const char *s = filename.toStdString().data();//获取代码内容
        cmd.sprintf("gcc -o %s.exe %s.c",s,s);//编译命令
        system(cmd.toStdString().data());//先编译

        cmd = filename.replace("/","\\") + ".c";
        remove(cmd.toStdString().data());//清楚数据


        cmd = filename + ".exe";//生成可运行我呢见
        system(cmd.toStdString().data());//再运行

}

void MainWindow::on_run()
{
    QString cmd;
    cmd = filename + ".exe";
    system(cmd.toStdString().data());//运行exe文件
}

