#include "downloadwidget.h"
#include "ui_downloadwidget.h"

downloadWidget::downloadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::downloadWidget)
{
    ui->setupUi(this);

    m_common = Common::getInstance();
    m_manager = m_common->getQNetworkAccessManager();

    initTableWidget();
}


downloadWidget::~downloadWidget()
{
    delete ui;
}

void downloadWidget::initTableWidget(){
    // 标头相关设置
    // 设置表列数
    ui->tableWidget->setColumnCount(3);
    // 设置列的宽度
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(300);

    // 设置表头内容
    QStringList header;
    header.append("排名");
    header.append("文件名");
    header.append("下载量");
    ui->tableWidget->setHorizontalHeaderLabels(header);

    // 设置字体
    // 获取标头原来的字体
    QFont font = ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);// 标头字体加粗
    ui->tableWidget->horizontalHeader()->setFont(font);

    // 设置垂直方向高度
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(50);

    // 单行选择
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);// 单行选择
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);// 设置不可编辑

    // 设置标头不可编辑
    ui->tableWidget->horizontalHeader()->setSectionsClickable(false);
    // 设置垂直头不可见，不自动显示行号
    ui->tableWidget->verticalHeader()->setVisible(false);

    // 设置标头样式
    ui->tableWidget->horizontalHeader()->setStyleSheet(
                "QHeaderView::section{"
                "background: skyblue;"
                "font: 20pt \"霞鹜文楷\";"
                "height: 35px;"
                "border: 1px solid #FF00FF;"
                "}"
     );

    // 设置第0行100
    ui->tableWidget->setColumnWidth(0, 100);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}

// 获取用户文件数量
void downloadWidget::getShareFilesCount(){
    m_shareFilesCount = 0;

    QNetworkRequest request; // 在栈中

    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    // 请求用户的文件数量，如果用户在云盘上存放了文件，那就可以刷新出来，否则，刷新不出来
    QString url = QString("http://%1:%2/sharefiles?cmd=count").arg(ip).arg(port);
    request.setUrl(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QNetworkReply *reply = m_manager->get(request);


    // 5. 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        // 处理json数据
        // m_shareFilesCount = NetworkData::getCount(data);
        m_shareFilesCount = data.toInt();
        qDebug() << "m_shareFilesCount" << m_shareFilesCount;

        if (m_shareFilesCount > 0) {
            //将数据写入到tableWidget
            // 清空当前
            clearShareFileDownload();

            // 获取具体的文件信息
            getFileDownload();
        } else {
            // 如果用户文件数量等于0，清空用户文件item
            clearShareFileDownload();
            clearTableWidget();
        }

        reply->deleteLater();
    });
}

void downloadWidget::clearTableWidget(){
    int rowCount = ui->tableWidget->rowCount();// 获取表单行数
    for (int i = 0; i < rowCount; ++i) {
        ui->tableWidget->removeRow(0); // 删除之前的Item
    }
}

void downloadWidget::clearShareFileDownload(){
    while (m_fileDownload.size() > 0) {
        FileDownload *temp = m_fileDownload.takeFirst();
        delete temp;
    }
}


void downloadWidget::getFileDownload(){
    QNetworkRequest request; // 在栈中

    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    // 请求用户的文件数量，如果用户在云盘上存放了文件，那就可以刷新出来，否则，刷新不出来
    QString url = QString("http://%1:%2/sharefiles?cmd=pvdesc").arg(ip).arg(port);
    request.setUrl(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));


    QJsonObject paramObj;
    paramObj.insert("start", 0);
    paramObj.insert("count", m_shareFilesCount);
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    // 5. 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        m_fileDownload = NetworkData::getFileDownload(data);
        qDebug() << "m_fileDownload size()" << m_fileDownload.size();

        // 展示数据
        refreshTableWidget();

        // 清空m_fileDownload
        clearShareFileDownload();


        reply->deleteLater();
    });

}

void downloadWidget::refreshFiles(){
    getShareFilesCount();
}

void downloadWidget::refreshTableWidget(){
    clearTableWidget();

    // 元素个数
    int size = m_fileDownload.size();
    qDebug() << "m_fileDownload.size()" << size;
    int rowCount = 0;
    for (int i =0; i < size; i++) {
        FileDownload *temp = m_fileDownload.at(i);
        if (temp != NULL) {
            ui->tableWidget->insertRow(rowCount);// 插入新的一行
            QTableWidgetItem *item1 = new QTableWidgetItem;
            QTableWidgetItem *item2 = new QTableWidgetItem;
            QTableWidgetItem *item3 = new QTableWidgetItem;

            item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            QFont font;
            font.setPointSize(20);// 设置字体大小
            font.setFamily(QStringLiteral("霞鹜文楷"));
            item1->setFont(font);
            item1->setText(QString("%1").arg(i+1));

            item2->setFont(font);
            item2->setText(temp->fileName);

            item3->setFont(font);
            item3->setText(QString("%1").arg(temp->pv));

            ui->tableWidget->setItem(rowCount, 0, item1);
            ui->tableWidget->setItem(rowCount, 1, item2);
            ui->tableWidget->setItem(rowCount, 2, item3);

            rowCount++;
        }
    }
}
