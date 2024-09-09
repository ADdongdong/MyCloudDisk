#include "filedataprogress.h"
#include "ui_filedataprogress.h"

FileDataProgress::FileDataProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileDataProgress)
{
    ui->setupUi(this);
}

FileDataProgress::~FileDataProgress()
{
    delete ui;
}

// 设置文件名称, 并初始化进度条
void FileDataProgress::setFileName(QString fileName){
    ui->label->setText(fileName);
    ui->progressBar->setValue(0); // 开始进度为0
    ui->progressBar->setMinimum(0); // 进度的最小值为0
}

void FileDataProgress::setProgress(int val, int max){
    ui->progressBar->setValue(val);
    ui->progressBar->setMaximum(max);
}
