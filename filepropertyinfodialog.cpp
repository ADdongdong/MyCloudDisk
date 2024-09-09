#include "filepropertyinfodialog.h"
#include "ui_filepropertyinfodialog.h"

FilePropertyInfoDialog::FilePropertyInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilePropertyInfoDialog)
{
    ui->setupUi(this);

    QString style1 = QString("color:rgb(11, 11, 11);");
    QString style2 = QString("color:rgb(255, 156, 16);");

    ui->lab_filename->setStyleSheet(style1);
    ui->lab_fileSize->setStyleSheet(style1);
    ui->lab_download->setStyleSheet(style1);
    ui->lab_pv->setStyleSheet(style1);
    ui->lab_uploadUser->setStyleSheet(style1);
    ui->lab_uploadtime->setStyleSheet(style1);
    ui->lab_shareStatus->setStyleSheet(style1);


    ui->val_filename->setStyleSheet(style2);
    ui->val_fileSize->setStyleSheet(style2);
    ui->val_download->setStyleSheet(style2);
    ui->val_pv->setStyleSheet(style2);
    ui->val_uploadUser->setStyleSheet(style2);
    ui->val_uploadtime->setStyleSheet(style2);
    ui->val_shareStatus->setStyleSheet(style2);

    ui->val_download->setOpenExternalLinks(true);// label内容作为超链接内容
}

FilePropertyInfoDialog::~FilePropertyInfoDialog()
{
    delete ui;
}

void FilePropertyInfoDialog::setFileInfo(FileInfo *fileInfo){
    ui->val_filename->setText(fileInfo->fileName);
    int size = fileInfo->size;
    // 展示给用户看的是KB 1KB = 1024Byte
    if (size >= 1024 && size < 1024*1024) {
        ui->val_fileSize->setText(QString("%1 KB").arg(size/1024.0));
    } else {
        ui->val_fileSize->setText(QString("%1 MB").arg(size/1024.0/1024.0));
    }

    if (fileInfo->shareStatus ==0){
        ui->val_shareStatus->setText("没有分享");
    } else if(fileInfo->shareStatus == 1){
        ui->val_shareStatus->setText("已经分享");
    }

    ui->val_uploadtime->setText(fileInfo->createTime);
    ui->val_uploadUser->setText(fileInfo->user);
    ui->val_pv->setText(QString("被下载%1次").arg(fileInfo->pv));

    // 传一个链接
    QString temp = QString("<a href=\"%1\">%2</a>").arg(fileInfo->url).arg(fileInfo->url);
    ui->val_download->setText(temp);
}



