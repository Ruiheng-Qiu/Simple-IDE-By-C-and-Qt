#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("IDE"); //设置项目名称
    connect(ui->actionopen,SIGNAL(triggered()),this,SLOT(on_open())); //连接信号槽，点击open按键实现接口函数on_open()的各项功能
    connect(ui->actionsave,SIGNAL(triggered()),this,SLOT(on_save())); //连接信号槽，点击save按键实现接口函数on_save()的各项功能
}


void MainWindow::on_open() //打开文件
{
    filename=QFileDialog::getOpenFileName(); //打开文件的默认路径
    if(filename.isEmpty()) //取消的情况
        return;
    FILE *p=fopen(filename.toStdString().data(),"r");
    if(p==NULL)
    {
        //QMessageBox::information(this,"notify","open file failure."); //弹出提示框
        return;
    }
    QString str;
    char buf[1024]={0};
    while(!feof(p)) //判断文件指针是否在文件尾，没到文件尾继续向下执行
    {
        str+=fgets(buf,sizeof(buf),p);
    }
    fclose(p);
    text->setText(str);

}

void MainWindow::on_save() //保存文件
{
    if(filename.isEmpty())
    {
        filename=QFileDialog::getSaveFileName();
    }
    if(!filename.isEmpty())
    {
        FILE *p=fopen(filename.toStdString().data(),"w");
        if(p==NULL)
        return ;
        fputs(text->toPlainText().toStdString().data(),p);
        fclose(p);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
