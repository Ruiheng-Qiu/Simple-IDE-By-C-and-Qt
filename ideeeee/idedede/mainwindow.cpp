#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QtEvents>

int flag_isOpen = 0; //判断是否打开或新建了一个文件
int flag_isNew = 0; //判断文件是否新建
QString Last_FileName;
QString Last_FileContent;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("IDE");
    this->setMinimumSize(1000,600); //运行后弹出界面大小，可放大

    file_menu = new QMenu(this);
    edit_menu = new QMenu(this);
    build_menu = new QMenu(this);
    help_menu = new QMenu(this);

    textEdit = new QTextEdit(this);

    textEdit->setGeometry(65,60,1800,800); //文本框大小，前两个参数为坐标
    textEdit->setHidden(true); //隐藏文本框，点击新建出现

    //创建菜单项
    //文件栏
    file_newAct = new QAction(tr("新建"),this);
    file_openAct = new QAction(tr("打开"),this);
    file_saveAct = new QAction(tr("保存"),this);
    file_saveasAct = new QAction(tr("另存为"),this);

    //编辑栏
    edit_undoAct = new QAction(tr("恢复"),this);
    edit_copyAct = new QAction(tr("拷贝"),this);
    edit_pasteAct = new QAction(tr("粘贴"),this);
    edit_cutAct = new QAction(tr("剪切"),this);

    //构建栏
    build_compileAct = new QAction(tr("编译"),this);
    build_runAct = new QAction(tr("运行"),this);

    //帮助栏
    help_aboutAct = new QAction(tr("关于"),this);

    //添加菜单
    file_menu = menuBar()->addMenu(tr("文件"));
    edit_menu = menuBar()->addMenu(tr("编辑"));
    build_menu = menuBar()->addMenu(tr("构建"));
    help_menu = menuBar()->addMenu(tr("帮助"));

    //添加菜单项
    file_menu->addSeparator(); //分隔符
    file_menu->addAction(file_newAct);
    file_menu->addSeparator();
    file_menu->addAction(file_openAct);
    file_menu->addSeparator();
    file_menu->addAction(file_saveAct);
    file_menu->addSeparator();
    file_menu->addAction(file_saveasAct);

    edit_menu->addSeparator();
    edit_menu->addAction(edit_undoAct);
    edit_menu->addSeparator();
    edit_menu->addAction(edit_copyAct);
    edit_menu->addSeparator();
    edit_menu->addAction(edit_pasteAct);
    edit_menu->addSeparator();
    edit_menu->addAction(edit_cutAct);

    build_menu->addSeparator();
    build_menu->addAction(build_compileAct);
    build_menu->addSeparator();
    build_menu->addAction(build_runAct);

    help_menu->addSeparator();
    help_menu->addAction(help_aboutAct);

    /*
    QToolButton *pActionOpenBar = new QToolButton(this);
    pActionOpenBar->setIcon(QIcon(":/image/dakai.png"));
    pActionOpenBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pActionOpenBar->setText(QStringLiteral("打开"));
    pToolBar->addWidget(pActionOpenBar);
    */

    //添加触发函数
    connect(file_openAct,SIGNAL(triggered()),this,SLOT(Open_File()) );
    connect(file_newAct,SIGNAL(triggered()),this,SLOT(Create_NewFile()) );
    connect(file_saveAct,SIGNAL(triggered()),this,SLOT(Save_File()) );
    connect(file_saveasAct,SIGNAL(triggered()),this,SLOT(Save_FileAs()) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Create_NewFile() //创建新文件
{
    textEdit->clear(); //清除原先文件内容
    textEdit->setHidden(false);
    flag_isNew = 1;                 //新文件标记位设为1
    flag_isOpen = 1;                //新文件创建 标记位设为1
}

void MainWindow::Open_File() //打开文件
{

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("打开文件"),tr(""),tr("Text File (*.docx)")); //打开文件的默认路径，第四个参数为对话框的后缀名过滤器
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
                   textEdit->setPlainText(textStream.readAll());
               }
               textEdit->show();
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
        if(textEdit->toPlainText() == "") //文件内容为空，弹出提示框
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
                QString str = textEdit->toPlainText();
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
                QString str = textEdit->toPlainText();
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

void MainWindow::Save_FileAs() //另存为
{
    if(flag_isOpen == 1)
    {
        QFileDialog fileDialog;
        QString fileName = fileDialog.getSaveFileName(this,tr("打开文件"),"/home",tr("Text File(*.txt)"));
        if(fileName == "")
        return;
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,tr("错误"),tr("打开文件失败"));
            return;
        }
        else
        {
            QTextStream textStream(&file);
            QString str = textEdit->toPlainText();
            textStream<<str;
            QMessageBox::warning(this,tr("提示"),tr("保存文件成功"));
            Last_FileContent = str;
            Last_FileName = fileName;
            flag_isNew = 0;
            file.close();
        }
    }
    else
    {
        QMessageBox::warning(this,tr("警告"),tr("请先创建或者打开文件"));
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) //点击关闭触发事件
{
    if(textEdit->toPlainText() == Last_FileContent) //如果文本框的内容与上次保存内容相同，接受关闭信号
    event->accept();
    else //弹出警告，有修改的内容未保存
    {
        if(QMessageBox::warning(this,tr("警告"),tr("文件还未保存,确定退出？"),QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) //选择yes则接受关闭信号
        event->accept();
        else
        event->ignore();
    }
}

