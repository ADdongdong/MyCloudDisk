#include "transformwidget.h"
#include "ui_transformwidget.h"

TransformWidget::TransformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransformWidget)
{
    ui->setupUi(this);

    // 垂直布局 (进度条UI)
    // 垂直布局，单独做成一个类
    // 动态添加，动态移除

    // 获取上传的布局类
    UploadLayout *uploadLayout = UploadLayout::getInstance();
    // 设置上传uploadScroll的布局，方便将进度条按照指定的布局添加到uploadScroll中
    uploadLayout->setUploadLayout(ui->uploadScroll);

    // 设置下载界面uploadScroll的布局
    DownloadLayout *downloadLayout = DownloadLayout::getInstance();
    downloadLayout->setDownloadLayout(ui->downScroll);

    // 传输页面默认显示上传
    ui->tabWidget->setCurrentWidget(ui->tabUpload);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int index){
        if (index == 0) {
            // 上传列表
        } else if (index == 1){
        } else if (index == 2) {
            // 传输记录, 显示数据，传输记录
            displayDataRecord();
        }
    });
}

TransformWidget::~TransformWidget()
{
    delete ui;
}

// 传输数据记录在UI显示
void TransformWidget::displayDataRecord(QString path){
    // conf/record /lisa.txt
    QString userFilePath = QString("%1/%2.txt").arg(path).arg(LoginInfoInstance::getInstance()->user());
    qDebug() << "userFilePath:" << userFilePath;

    // 读取记录文件中的内容
    QFile file(userFilePath);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "file.open(QIODevice::ReadOnly) err";
        return;
    }

    QByteArray array = file.readAll();

    // fromLocal8Bit utf-8转为本地字符集
    ui->record_mg->setText(QString::fromLocal8Bit(array));
    if (file.isOpen()){
        file.close();
    }
}

void TransformWidget::showPage(TransformStatus status){
    if (status == TransformStatus::Upload) {
        ui->tabWidget->setCurrentWidget(ui->tabUpload);
    } else if (status == TransformStatus::Download) {
        ui->tabWidget->setCurrentWidget(ui->tabDownload);
    } else if (status == TransformStatus::Record){
        ui->tabWidget->setCurrentWidget(ui->tabTransform);
    }
}



// 清空传输记录
void TransformWidget::on_toolButton_clicked()
{
    ui->record_mg->setHtml("");
}
