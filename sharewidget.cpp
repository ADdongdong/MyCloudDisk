#include "sharewidget.h"
#include "ui_sharewidget.h"

ShareWidget::ShareWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareWidget)
{
    ui->setupUi(this);

    initListWidget();

    addMenu(); // 添加菜单

    // 初始化下载任务队列
    m_downloadTask = DownloadTask::getInstance();
    // 初始化下载任务
    checkTaskList();

}

ShareWidget::~ShareWidget()
{
    delete ui;
}

void ShareWidget::initListWidget()
{
    m_common = Common::getInstance();

    m_loginInfo = LoginInfoInstance::getInstance();
    m_manager = Common::getInstance()->getQNetworkAccessManager();

    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80, 80));
    ui->listWidget->setGridSize(QSize(100, 120)); // 这是设置每个item所占用的大小

    // ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/file.png"), "图标1"));

    // 设置QListView大小改变时，图标的调整模式默认是固定的，可以改成自动调整
    ui->listWidget->setResizeMode(QListView::Adjust);
    // 设置图标拖动状态，QListView::Static表示不可拖动
    ui->listWidget->setMovement(QListView::Static);

    // listWidget右键菜单
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &ShareWidget::onRightMenu);
}

void ShareWidget::addMenu()
{
    //右键击item显示的菜单
    m_menuItem = new MyMenu(this);

    m_actionShareDownload = new QAction("下载", this);
    m_actionShareProperty = new QAction("属性", this);
    m_actionShareCancel = new QAction("取消分享", this);
    m_actionShareSave = new QAction("转存文件", this);

    m_menuItem->addAction(m_actionShareDownload);
    m_menuItem->addAction(m_actionShareProperty);
    m_menuItem->addAction(m_actionShareCancel);
    m_menuItem->addAction(m_actionShareSave);

    // 右键点击空白位置显示的菜单
    m_menuEmpty = new MyMenu(this);
    m_actionShareRefresh = new QAction("刷新", this);
    m_menuEmpty->addAction(m_actionShareRefresh);

    // 信号槽关联
    menuActions();
}


void ShareWidget::menuActions()
{
    /*
    QObject::connect(m_actionDownload, &QAction::triggered, this, [=](){
        qDebug() << "下载";
        addDownloadFiles(); // 添加下载任务到下载任务队列
    });
    */

    QObject::connect(m_actionShareDownload, &QAction::triggered, this, [=](){
        qDebug() << "下载";
        addDownloadFiles();
    });


    QObject::connect(m_actionShareProperty, &QAction::triggered, this, [=](){
        qDebug() << "属性";
        dealfile("property");
        // 查看文件属性实现
    });


    QObject::connect(m_actionShareSave, &QAction::triggered, this, [=](){
        qDebug() <<"转存";
        dealfile("save");
        // 转存文件实现
    });

    QObject::connect(m_actionShareCancel, &QAction::triggered, this, [=](){
        qDebug() << "取消分享";
        dealfile("cancel");
    });

    // 空白处右键
    QObject::connect(m_actionShareRefresh, &QAction::triggered, this, [=](){
        getShareFilesCount();
        qDebug() << "刷新";
    });
}


void ShareWidget::onRightMenu(const QPoint &pos){
    QListWidgetItem *item = ui->listWidget->itemAt(pos); // 获取当前鼠标所在的对象
    if (item == NULL) { // 如果没有点击图标
        qDebug() << "NULL";
         m_menuEmpty->exec(QCursor::pos()); // QCursor::pos() 鼠标当前位置
    } else {
        qDebug() << "click";
        /*
        if (item->text() == "上传文件") {
            // 如果是上传问价， 没有右键菜单
            return;
        }
        */
        m_menuItem->exec(QCursor::pos());
    }
}


void ShareWidget::checkTaskList(){
    QObject::connect(&m_downloadFileTimer, &QTimer::timeout, this, [=](){
        // 定时执行上传任务
        downloadFilesActions();
    });

    // q启动定时器
    m_downloadFileTimer.start(500);
}

void ShareWidget::downloadFilesActions(){
    // 取出任务队列首任务
    if (m_downloadTask->isEmpyt()){
        return;
    }

    DownloadFileInfo* downloadFileInfo = m_downloadTask->takeTask();
    if (downloadFileInfo == NULL) {
        qDebug() << "任务列表为空";
        return;
    }

    if (downloadFileInfo->isShareTask == false) {
        // 不是共享文件任务
        qDebug() << QString("【%1】不是共享文件任务").arg(downloadFileInfo->fileName);
        return;
    }

    QFile *file = downloadFileInfo->file;
    QUrl url = QUrl(downloadFileInfo->url);
    QNetworkRequest request;
    request.setUrl(url);
    qDebug() << "url"<< url.toString();

    // 向服务器发送请求，执行具体的下载任务
    QNetworkReply *reply = m_manager->get(request);// 请求方法
    if (reply == nullptr) {
        // 删除任务
        m_downloadTask->deleteDownloadTask();

        qDebug() << "下载文件失败";
        return;
    }

    // 读取数据并写入文件
    connect(reply, &QNetworkReply::readyRead, [=](){
        // 如果file不是空指针，那就将服务器数据写入到file 中
        qDebug() << "开始写入共享文件";
        if (file != nullptr) {
            file->write(reply->readAll()); // 读取数据后， 需要保存到文件
        }
    });

    // 如果，数据向file中写入完毕，记录文件的下载信息
    connect(reply, &QNetworkReply::finished, [=](){
        qDebug() << "共享文件下载完毕";
        reply->deleteLater();

        m_common->writeRecord(m_loginInfo->user(),
                              downloadFileInfo->fileName,
                              "090");
        qDebug() <<"共享文件记录写入成功";

        // 等待
        m_common->sleep(3000);


        // 调用共享文件下载标志处理接口
        dealFilePV(downloadFileInfo->md5, downloadFileInfo->fileName);

        // 删除下载任务
        // 不能先删除了downloadFileInfo在处理pv
        // 因为处理pv要用到downloadFileInfo
        m_downloadTask->deleteDownloadTask();

    });

    // 显示文件下载进度
    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesSent, qint64 byteTotal){
        downloadFileInfo->fdp->setProgress(bytesSent/1024, byteTotal/1024);
    });
}

void ShareWidget::dealFilePV(QString md5, QString fileName){
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealsharefile?cmd=pv
    QString url = QString("http://%1:%2/dealsharefile?cmd=pv").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "filename": "ui_buttongroup.h",
    "md5": "a89390d867d5da18c8b1a95908d7c653",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", LoginInfoInstance::getInstance()->user());
    paramsObj.insert("filename", fileName);
    paramsObj.insert("md5", md5);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);


    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        //读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        QString code = NetworkData::getCode(data);
        if (code == "017") {
            //成功
            qDebug() <<"PV处理成功";

            //需要去改m_fileList中对应的文件pv值
            for (int i=0;i<m_fileList.size();++i) {
                FileInfo *info = m_fileList.at(i);
                if (info->md5 == md5 && info->fileName == fileName) {
                    int pv = info->pv;
                    info->pv = pv + 1;
                    break;
                }
            }
        } else {
            //失败
            qDebug() <<"PV处理失败";
        }

        //立即销毁
        reply->deleteLater();

    });
}

// 添加下载任务到下载任务队列
void ShareWidget::addDownloadFiles(){
    emit gotoTransform(TransformStatus::Download);

    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item == NULL){
        qDebug() << "item: null";
        return;
    }

    FileInfo *fileInfo = nullptr;
    int size =  m_fileList.size();
    for (int i = 0; i < size; i++) {
        fileInfo = m_fileList.at(i);
        if (fileInfo != nullptr) {
            if (fileInfo->fileName == item->text()){
                // 找到fileInfo对象了
                // 打开保存文件的对话框
                QString filePath = QFileDialog::getSaveFileName(this, "请选择保存文件的路径", item->text());
                qDebug() << "filePath:" << filePath;
                if (filePath.isEmpty()) {
                    qDebug() << "filePathName.isEmpty()";
                    return;
                }

                // 将需要下载的文件添加到下载任务列表
                int res = m_downloadTask->appendDownloadTask(fileInfo, filePath, true);
                // 如果下载失败
                if (res == -2){
                    m_common->writeRecord(m_loginInfo->user(),
                                          fileInfo->fileName,
                                          "091");// 091 下载失败
                }
            }
        }
    }
}

// 获取用户文件数量
void ShareWidget::getShareFilesCount(){
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
        if (m_shareFilesCount> 0) {
            // 请求用户信息
            getShareFilesList();
        } else {
            // 如果用户文件数量等于0，清空用户文件item
        }

        reply->deleteLater();
    });
}


void ShareWidget::getShareFilesList(){
    QString strCmd;

    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/myfiles?cmd=normal
    //http://192.168.52.139/myfiles?cmd=asc
    //http://192.168.52.139/myfiles?cmd=pvasc
    QString url = QString("http://%1:%2/sharefiles?cmd=normal").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("start", 0);
    paramsObj.insert("count", m_shareFilesCount);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);


    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 解析返回的数据
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据：" << QString(data);

        // 先清空原来的m_fileList中的内容
        clearFileList();

        m_fileList = NetworkData::getFileInfo(data);
        qDebug() << "m_fileList size()" << m_fileList.size();

        // 立即销毁控制服务器返回信息的对象
        reply->deleteLater();

        // 清空当前页面所展示的原来m_fileList中的内容
        clearItems();

        // 展示新的m_fileList中的内容
        showFileItems();

    });
}

void ShareWidget::clearItems(){
    int count = ui->listWidget->count();
    for (int i = 0; i < count; i++) {
        /// 这个listWidget是一个大小动态变化的List，一个元素被删除了，后面的元素就会补上
        /// 所以，删除的时候，只删除index0就可以了
        QListWidgetItem *item = ui->listWidget->takeItem(0);
        delete item;
    }
}


void ShareWidget::showFileItems(){

    for (int i =0; i < m_fileList.length(); i++) {
        FileInfo *fileInfo = m_fileList.at(i);
        addListWidgetItem(fileInfo);
    }
}

void ShareWidget::addListWidgetItem(FileInfo *fileInfo){
        QString fileTypeName = QString("%1.png").arg(fileInfo->type);
        fileTypeName = m_common->getFileType(fileTypeName);
        QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(fileTypeName);
        qDebug() << "fileTypeName:" << fileTypeName << " filePath:" << filePath;

        // 添加items(图片/文字)到listWidtget
        ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), fileInfo->fileName));
}

// 清空m_fileList中的数据
void ShareWidget::clearFileList(){
    int size = m_fileList.size();

    for (int i =0; i < size; i++) {
        FileInfo *temp = m_fileList.takeFirst();
        if (temp != nullptr) {
            delete temp;
        }
    }
}


void ShareWidget::dealfile(QString cmd){

    // 获取当前选中的item
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item == NULL) {
        qDebug() << "选中item null";
        return;
    }

    // 根据item拿到FileInfo*
    for (int i = 0; i < m_fileList.length(); i++){
        FileInfo *fileInfo = m_fileList.at(i);
        if (fileInfo->fileName == item->text()){
            if (cmd == "property") {
                showFileProperty(fileInfo);
            } else if (cmd == "cancel") {
                // 取消分享
                cancelShareFile(fileInfo);
            } else if (cmd == "save") {
                // 转存文件
                saveFileToMyAccount(fileInfo);
            }
            break;
        }
    }
}

void ShareWidget::saveFileToMyAccount(FileInfo *fileinfo){
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    QString url = QString("http://%1:%2/dealsharefile?cmd=save").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    // 设置请求头的json
    QJsonObject paramObj;
    paramObj.insert("user", m_loginInfo->user());
    paramObj.insert("filename", fileinfo->fileName);
    paramObj.insert("md5", fileinfo->md5);
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();// 将json转为网络字节序
    QNetworkReply *reply = m_manager->post(request, data);

    // 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据：" << QString(data);

        QString code = NetworkData::getCode(data);
        if (code == "020"){
            QMessageBox::information(this, "转存成功", "文件已经保存!");
            //
        } else if (code == "021") {
            // 取消分享失败
            QMessageBox::warning(this, "提醒", "文件已经存在!");
        } else if (code == "022"){
            QMessageBox::warning(this, "错误", "转存失败!");
        }
        reply->deleteLater();
    });
}


void ShareWidget::showFileProperty(FileInfo *fileInfo){
    FilePropertyInfoDialog dig;
    dig.setFileInfo(fileInfo);
    dig.exec();
}


void ShareWidget::cancelShareFile(FileInfo *fileinfo) {
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    QString url = QString("http://%1:%2/dealsharefile?cmd=cancel").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    // 设置请求头的json
    QJsonObject paramObj;
    paramObj.insert("user", m_loginInfo->user());
    paramObj.insert("filename", fileinfo->fileName);
    paramObj.insert("md5", fileinfo->md5);
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();// 将json转为网络字节序
    QNetworkReply *reply = m_manager->post(request, data);

    // 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据：" << QString(data);

        QString code = NetworkData::getCode(data);
        if (code == "018"){
            FileInfo *temp = NULL;
            // 取消成功
            for (int i = 0; i < m_fileList.size(); i++){
                temp = m_fileList.at(i);
                if (temp == fileinfo) {
                    QListWidgetItem *item = NULL; // 要删除的图标
                    for (int k = 0; k < ui->listWidget->count(); k++){
                        item = ui->listWidget->item(k);
                        if (item->text() == fileinfo->fileName){
                            break;
                        }
                    }

                    if (item != NULL) {
                        /// 删除图标
                        ui->listWidget->removeItemWidget(item);
                        delete item;

                        m_fileList.removeAt(i);
                        delete fileinfo;
                    }
                    break;
                }
            }
            QMessageBox::information(this, "操作成功", "此文件已经取消分享!");
        } else if (code == "019") {
            // 取消分享失败
            QMessageBox::warning(this, "操作失败", "此文件取消分享失败!");
        }

        reply->deleteLater();
    });
}
