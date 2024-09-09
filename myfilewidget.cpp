#include "myfilewidget.h"
#include "ui_myfilewidget.h"

MyFileWidget::MyFileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFileWidget)
{
    ui->setupUi(this);

    initListWidget();

    m_manager = Common::getInstance()->getQNetworkAccessManager();
    m_uploadTask = UploadTask::getInstance();
    m_downloadTask = DownloadTask::getInstance();

    addMenu();

    // 启动上传文件定时器
    checkTaskList();

}

MyFileWidget::~MyFileWidget()
{
    delete ui;
}

void MyFileWidget::checkTaskList(){
    QObject::connect(&m_uploadFileTimer, &QTimer::timeout, this, [=](){
        // 定时执行, 每500毫秒执行一次, 信号为定时器的timeout信号, 槽函数为执行上传文件任务
        // 上传文件处理， 取出上传任务列表的首任务，上传完成后，再取下一个任务
        uploadFileAction();
    });
    // 启动定时器
    m_uploadFileTimer.start(500);


    // 下载定时器
    QObject::connect(&m_downloadFileTimer, &QTimer::timeout, this, [=](){
         downloadFilesAction();
    });
    m_downloadFileTimer.start(500);
}

void MyFileWidget::downloadFilesAction(){
// 取出上传任务列表的首任务

    if (m_downloadTask->isEmpyt()){
        // qDebug() << "任务列表为空";
        return;
    }

    // 获取到下载队列的首任务
    DownloadFileInfo *downloadFileinfo= m_downloadTask->takeTask();

    QFile *file = downloadFileinfo->file;

    // 获取到下载文件的url，并构建http请求
    QUrl url = QUrl(downloadFileinfo->url);
    QNetworkRequest request;
    request.setUrl(url);

    // 去下载文件
    QNetworkReply *reply = m_manager->get(request); // 请求方法
    if (reply == NULL) {
        // 删除任务
        m_downloadTask->deleteDownloadTask();

        qDebug() << "下载文件失败";
        return;
    }


    // 读取数据并写入文件
    connect(reply, &QNetworkReply::readyRead, [=](){
        if (file != nullptr){
            // 读取返回的数据
            file->write(reply->readAll()); // 读取数据后，需要保存到文件
        }
    });

    // 读取并写入文件
    connect(reply, &QNetworkReply::finished, [=](){
        reply->deleteLater();

        m_common->writeRecord(m_loginInfo->user(),
                              downloadFileinfo->fileName,
                              "090"); // 下载成功

        // 删除下载任务
        m_downloadTask->deleteDownloadTask();
    });

    // 显示文件下载进度
    connect(reply, &QNetworkReply::downloadProgress, this ,[=](qint64 bytesSent, qint64 bytesTotal){
            downloadFileinfo->fdp->setProgress(bytesSent/1024, bytesTotal/1024);
    });

}
void MyFileWidget::uploadFileAction(){
    // 取出上传任务列表的首任务

    if (m_uploadTask->isEmpyt()){
        // qDebug() << "任务列表为空";
        return;
    }

    // 获取到下载队列的首任务
    UploadFileInfo *uploadFileInfo = m_uploadTask->takeTask();


    // 上传文件, 秒传: 发现数据库中，有这个文件，则不用上传了, 如果数据库中没有这个文件，则上传

    QNetworkRequest request;
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    // 请求用户的文件数量，如果用户在云盘上存放了文件，那就可以刷新出来，否则，刷新不出来
    QString url = QString("http://%1:%2/md5").arg(ip).arg(port);
    request.setUrl(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));


    // 3. 封装请求数据（json)
    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("filename", uploadFileInfo->fileName);
    paramsObj.insert("md5", uploadFileInfo->md5);
    paramsObj.insert("token", m_loginInfo->token());
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    // 请求方式为post
    QNetworkReply *reply = m_manager->post(request, data);


    // 5. 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();

        // 处理json数据
        QString code = NetworkData::getCode(data);
        qDebug() << "code:" << code;

        if (code == "005") {
            // 上传的文件已经存在
            uploadFileInfo->uploadStatus = UPLOAD_FILE_EXISTE;
            // 则删除加入到下载任务List中的任务，不用再下载了
            m_uploadTask->deleteUploadTask();
            m_common->writeRecord(m_loginInfo->user(), uploadFileInfo->fileName, code);
        } else if (code == "006") {
            // 秒传成功
            m_uploadTask->deleteUploadTask();
            m_common->writeRecord(m_loginInfo->user(), uploadFileInfo->fileName, code);
        } else if (code == "007") {
            // 秒传失败
            // 服务器没有此文件，需要真正上传此文件
            m_common->writeRecord(m_loginInfo->user(), uploadFileInfo->fileName, code);
            upLoadFile(uploadFileInfo);
        } else if (code == "111"){
            // token验证失败，重新等录
            QMessageBox::warning(this, "警告", "token验证错误，请重新登录");
            emit sigLoginAgain();
        }

        // 立即销毁
        // delete reply;
        // 延迟销毁
        // 调用QObject::deleteLater()这个函数并没有立即销毁，而是向主消息循环发送了一个event,
        // 下一次主循环收到这个event之后才会销毁对象。
        reply->deleteLater();
    });
}

void MyFileWidget::initListWidget(){
    m_common = Common::getInstance();
    m_loginInfo = LoginInfoInstance::getInstance();
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80, 80));
    ui->listWidget->setGridSize(QSize(100, 120));

    // 在listWidget中添加一个项目
    // ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/file.png"), "图标1"));

    // 根据mainwindow的大小进行缩放, 设置QListView大小，默认是固定的，可以改成自适应
    ui->listWidget->setResizeMode(QListView::Adjust); // 自适应布局

    // 设置图标无法拖动, QListView::Static表示不可拖动
    ui->listWidget->setMovement(QListView::Static);

    // listWidget右键菜单
    // the widget emits the QWidget::customContextMenuRequested() signal.
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MyFileWidget::onRightMenu);

    // 左键上传进行上传文件
    QObject::connect(ui->listWidget, &QListWidget::itemPressed, this, [=](QListWidgetItem *item){
        QString text = item->text();
        if (text == "上传文件"){
            // upLoadFile();
            // 添加文件到上传任务列表
            addUploadFiles();
        }
    });

}


// 添加下载任务到下载任务队列
void MyFileWidget::addDownloadFiles(){
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
                QString filePath = QFileDialog::getSaveFileName(this, "请萱蕚保存文件的路径", item->text());
                qDebug() << "filePath:" << filePath;
                if (filePath.isEmpty()) {
                    qDebug() << "filePathName.isEmpty()";
                    return;
                }

                // 将需要下载的文件添加到下载任务列表
                int res = m_downloadTask->appendDownloadTask(fileInfo, filePath);
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

// 添加上传任务到上传任务队列
void MyFileWidget::addUploadFiles(){
    // 开始传输以后就立刻切换到传输页面，查看文件的传输情况
    // 发送切换到传输页面的信号
    emit gotoTransform(TransformStatus::Upload);

    // 一次选中多个文件
    QStringList filePathList = QFileDialog::getOpenFileNames();
    for (QString filePath : filePathList) {
        qDebug() << "fileName:" << filePath;

        // 添加到上传任务列表
        int ret = m_uploadTask->appendUploadTask(filePath);
        if (ret == -1){
            QMessageBox::warning(this, QString("上传警告"), QString("上传文件大小不能超过30M"));
            return;
        }
    }
}

void MyFileWidget::upLoadFile(UploadFileInfo *uploadFileInfo){

    // 上传文件到服务器, 下面是上传文件的格式
    /*
    ------WebKitFormBoundaryPWCm6mQV9EznKDDy\r\n
    Content-Disposition: form-data; name="file"; filename="手机店.jpg"\r\n
    Content-Type: image/jpeg\r\n
    \r\n
    真正的文件内容\r\n
    ------WebKitFormBoundaryPWCm6mQV9EznKDDy
    */

    // 添加文件内容
    QFile file(uploadFileInfo->filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    //QString randText = "PWCm6mQV9EznKDDy"; // 随机生成的字符串 0-9a-zA-Z
    //QString boundary = "------WebKitFormBoundary" + randText;
    QString boundary = m_common->getBoundary();

    QByteArray data;
    data.append(boundary.toStdString());
    data.append("\r\n");
    data.append(QString("Content-Disposition: form-data; user=\"%1\" filename=\"%2\" md5=\"%3\" size=%4")
                .arg(m_loginInfo->user())
                .arg(uploadFileInfo->fileName)
                .arg(uploadFileInfo->md5)
                .arg(uploadFileInfo->size).toStdString());
    data.append("\r\n");
    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");

    // 上传文件
    data.append(file.readAll());
    data.append("\r\n");
    data.append(boundary.toStdString());

    file.close();

    // 发送http请求
    QString url = QString("http://%1:%2/upload").arg(m_loginInfo->ip()).arg(m_loginInfo->port());
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    // 发送http请求
    QNetworkReply *reply = m_manager->post(request, data);

    if (reply == NULL) {
        qDebug() << "请求失败";
        return;
    }


    // 显示上传进度条
    QObject::connect(reply, &QNetworkReply::uploadProgress, this, [=](qint64 bytessent, qint64 bytesTotal){
        // bytesSent 上传字节数
        // bytesTotal 文件需要上传的总字节数
        qDebug() << "bytesTotal:" << bytesTotal;
        if (bytesTotal != 0) {
            // 显示进度条
            uploadFileInfo->fdp->setProgress(bytessent/1024, bytesTotal/1024);
        }

    });

    connect(reply, &QNetworkReply::finished, this, [=](){
        // 文件上传完成后
        if (reply->error() != QNetworkReply::NoError) {
            qDebug()<< reply->errorString();
            reply->deleteLater();
        } else {
            QByteArray json = reply->readAll();
            // qDebug() << "json:" << QString(json);

            /*
            008: 上传成功
            009: 上传失败
             */
            QString code = NetworkData::getCode(json);
            if (code == "008") {
                qDebug() << "上传成功";
                m_common->writeRecord(m_loginInfo->user(), uploadFileInfo->fileName, code);
                uploadFileInfo->uploadStatus = UPLOAD_FINISHED;
                // 刷新页面
                getMyFileCount();
            } else if (code =="009"){
                qDebug() << "上传失败";
                m_common->writeRecord(m_loginInfo->user(), uploadFileInfo->fileName, code);
                uploadFileInfo->uploadStatus = UPLOAD_FAILD;
            }

            // 获取到上传任务列表
            UploadTask *uploadtask = UploadTask::getInstance();

            // 删除任务
            uploadtask->deleteUploadTask();
        }
        reply->deleteLater();
    });

}

// 显示右键菜单
void MyFileWidget::onRightMenu(const QPoint &pos){
    QListWidgetItem *item = ui->listWidget->itemAt(pos);
    // 是否点击到图标展示的内容是不一样的
    if (item == NULL) {
        // 没有点击到图标
        m_menuEmpty->exec(QCursor::pos());
    } else {
        // 点击了图标
        qDebug() << "clicked";
        if (item->text() ==  "上传文件"){
            // 如果是上传文件，就没有右击菜单
            return;
        }
        m_menuItem->exec(QCursor::pos()); // QCursor::pos()获取鼠标当前位置
    }
}

void MyFileWidget::addMenu(){
    // 新建菜单对象, 点击item所显示的菜单
    m_menuItem = new MyMenu(this);

    // 添加菜单中的每一项
    m_actionShare = new QAction("分享", this);
    m_actionDel = new QAction("删除", this);
    m_actionProperty = new QAction("属性", this);
    m_actionDownload = new QAction("下载", this);
    m_menuItem->addAction(m_actionDownload);
    m_menuItem->addAction(m_actionShare);
    m_menuItem->addAction(m_actionDel);
    m_menuItem->addAction(m_actionProperty);

    // 新建菜单对象, 点击空白所显示的菜单
    m_menuEmpty = new MyMenu(this);

    // 添加菜单中的每一项
    m_actionDownloadAsc = new QAction("按下载量升序", this);
    m_actionDownloadDesc = new QAction("按下载量降序", this);
    m_actionRefresh = new QAction("刷新", this);
    m_actionUpload = new QAction("上传", this);
    m_menuEmpty->addAction(m_actionUpload);
    m_menuEmpty->addAction(m_actionRefresh);
    m_menuEmpty->addAction(m_actionDownloadDesc);
    m_menuEmpty->addAction(m_actionDownloadAsc);

    meanActions();

}

void MyFileWidget::meanActions(){
    QObject::connect(m_actionDownload, &QAction::triggered, this, [=](){
        qDebug() << "下载";
        addDownloadFiles(); // 添加下载任务到下载任务队列
    });
    QObject::connect(m_actionDel, &QAction::triggered, this, [=](){
        qDebug() << "删除";
        dealFile("delete");
    });
    QObject::connect(m_actionShare, &QAction::triggered, this, [=](){
        qDebug() << "分享";
        // 分享文件
        dealFile("share");
    });
    QObject::connect(m_actionProperty, &QAction::triggered, this, [=](){
        qDebug() << "属性";
        // 展示文件属性
        dealFile("property");
    });
    QObject::connect(m_actionUpload, &QAction::triggered, this, [=](){
        qDebug() << "上传";
        addUploadFiles();
    });
    QObject::connect(m_actionRefresh, &QAction::triggered, this, [=](){
        getMyFileCount(Normal);
        qDebug() << "刷新";
    });
    QObject::connect(m_actionDownloadDesc, &QAction::triggered, this, [=](){
        getMyFileCount(Desc);
        qDebug() << "按下载量降序";
    });
    QObject::connect(m_actionDownloadAsc, &QAction::triggered, this, [=](){
        getMyFileCount(Asc);
        qDebug() << "按下载量升序";
    });
}



// 获取用户文件数量
void MyFileWidget::getMyFileCount(MyFileDisplay cmd){
    QNetworkRequest request; // 在栈中
    // QString url = QString("http://172.20.203.67/login");
    // 修改为从配置文件获取的信息

    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    // 请求用户的文件数量，如果用户在云盘上存放了文件，那就可以刷新出来，否则，刷新不出来
    QString url = QString("http://%1:%2/myfiles?cmd=count").arg(ip).arg(port);
    request.setUrl(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));


    // 3. 封装请求数据（json)
    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());// passward使用md5加密
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    // 5. 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        // 处理json数据
        QStringList list = NetworkData::getFileCount(data);

        if (!list.isEmpty()) {
            QString code = list.at(0);
            if (code == "110") {
                // 成功
                m_myFilesCount = list.at(1).toInt();
                qDebug() << "num:" << m_myFilesCount;
            } else if (code == "111") {
                // 失败
                QMessageBox::critical(this, "账号异常", "获取文件数量失败！");
                // 是因为token过期，所以，我们要重新向服务器获取token
                emit sigLoginAgain();
                return;
            }
        }

        if (m_myFilesCount > 0) {
            // 请求用户信息
            getMyFileList(cmd);
        } else {
            // 如果用户文件数量等于0，清空用户文件item
            refreshFileItems();
        }

        // 立即销毁
        // delete reply;
        // 延迟销毁
        // 调用QObject::deleteLater()这个函数并没有立即销毁，而是向主消息循环发送了一个event,
        // 下一次主循环收到这个event之后才会销毁对象。
        reply->deleteLater();
    });

}

void MyFileWidget::addListWidgetItem(FileInfo *fileInfo){
    QString fileTypeName = QString("%1.png").arg(fileInfo->type);
    QString fileName = m_common->getFileType(fileTypeName);
    QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(fileName);
    qDebug() << "fileName:" << fileName;

    //添加items(图片/文字)到listWidget
    ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), fileInfo->fileName));
}

//清空用户文件item
void MyFileWidget::refreshFileItems(){
    // 清空ui->listWidget中的item
    clearItems();

    // 如果文件列表不为空，显示文件列表
    if (!m_fileList.isEmpty()) {
        // 显示文件列表
        int n = m_fileList.size();
        for (int i = 0; i<n; ++i) {
            FileInfo *fileInfo = m_fileList.at(i);
            // 添加图标
            addListWidgetItem(fileInfo);
        }
    }
    addUploadItem();
}



void MyFileWidget::getMyFileList(MyFileDisplay cmd){
    QString strCmd;
    if (cmd == MyFileDisplay::Normal) {
        strCmd = "normal";
    } else if (cmd == MyFileDisplay::Asc) {
        strCmd = "pvasc";
    } else if (cmd == MyFileDisplay::Desc) {
        strCmd = "pvdesc";
    }

    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/myfiles?cmd=normal
    //http://192.168.52.139/myfiles?cmd=asc
    //http://192.168.52.139/myfiles?cmd=pvasc
    QString url = QString("http://%1:%2/myfiles?cmd=%3").arg(ip).arg(port).arg(strCmd);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

/*
{
    "count": 1,
    "start": 0,
    "token": "ecf3ac6f8863cd17ed1d3909c4386684",
    "user": "milo"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("start", 0);
    paramsObj.insert("count", m_myFilesCount);
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

        // 再添加最新文件列表加载到m_fileList中
        m_fileList = NetworkData::getFileInfo(data);
        qDebug() << "m_fileList.size()" << m_fileList.size();


        // 清空当前页面所展示的原来m_fileList中的内容
        clearItems();

        // 展示新的m_fileList中的内容
        showFileItems();

        // 立即销毁控制服务器返回信息的对象
        reply->deleteLater();
    });
}

void MyFileWidget::addUploadItem() {
    ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/upload.png"),"上传文件"));
}

void MyFileWidget::clearItems(){
    int count = ui->listWidget->count();
    for (int i = 0; i < count; i++) {
        /// 这个listWidget是一个大小动态变化的List，一个元素被删除了，后面的元素就会补上
        /// 所以，删除的时候，只删除index0就可以了
        QListWidgetItem *item = ui->listWidget->takeItem(0);
        delete item;
    }
}

// 清空m_fileList中的数据
void MyFileWidget::clearFileList(){
    int size = m_fileList.size();

    for (int i =0; i < size; i++) {
        FileInfo *temp = m_fileList.takeFirst();
        if (temp != nullptr) {
            delete temp;
        }
    }
}

void MyFileWidget::showFileItems(){

    for (int i =0; i < m_fileList.length(); i++) {
        FileInfo *fileInfo = m_fileList.at(i);

        QString fileTypeName = QString("%1.png").arg(fileInfo->type);
        fileTypeName = m_common->getFileType(fileTypeName);
        QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(fileTypeName);
        qDebug() << "fileTypeName:" << fileTypeName << " filePath:" << filePath;

        // 添加items(图片/文字)到listWidtget
        ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), fileInfo->fileName));
        /*
        qDebug() << m_fileList.at(i)->user;
        qDebug() <<m_fileList.at(i)->md5;
        qDebug() <<m_fileList.at(i)->createTime;
        qDebug() << m_fileList.at(i)->fileName;
        qDebug() << m_fileList.at(i)->shareStatus;
        qDebug() << m_fileList.at(i)->pv;
        qDebug() << m_fileList.at(i)->url;
        qDebug() << m_fileList.at(i)->size;
        qDebug() << m_fileList.at(i)->type;
        */
        }

    // 添加上传文件图标
    addUploadItem();
}

// 分享文件
void MyFileWidget::dealFile(QString cmd){
    // 获取到选中的item
    QListWidgetItem *curItem = ui->listWidget->currentItem();
    if(curItem == NULL) {
        qDebug() << "选中item NULL";
        return;
    }

    // 根据item拿到FileInfo*
    // m_fileList
    for (int i=0; i < m_fileList.length(); i++) {
        FileInfo *fileInfo = m_fileList.at(i);
        if(fileInfo->fileName == curItem->text()) {
            qDebug() << "fileInfo->fileName:" << fileInfo->fileName << "  item->text():" << curItem->text();
            // 请求接口
            if (cmd == "share") {
                getShareFile(fileInfo);
            } else if (cmd == "delete") {
                deleteFile(fileInfo);
            } else if (cmd == "property") {
                showFileProperty(fileInfo);
            } else if (cmd == "upload") {
                continue;
            }
            // 找到要分享的文件以后，就退出循环
            break;
        }
    }
}

// 展示文件信息文件
void MyFileWidget::showFileProperty(FileInfo *fileInfo){
    FilePropertyInfoDialog dig;
    dig.setFileInfo(fileInfo);
    dig.exec();
}


// 文件删除
void MyFileWidget::deleteFile(FileInfo *fileInfo){
    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealfiles?cmd=share
    QString url = QString("http://%1:%2/dealfile?cmd=del").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("filename", fileInfo->fileName);
    paramsObj.insert("md5", fileInfo->md5);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 解析返回的数据
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据：" << QString(data);

        QString code = NetworkData::getCode(data);

/*
013: 成功
014: 失败
111: Token 验证失败
*/

        if (code == "013"){ // 成功
            QMessageBox::information(this, "删除成功", QString("【%1】删除成功成功!").arg(fileInfo->fileName));
            //getMyFileCount(); // 刷新，重新展示
            // 在listWidget上删除对应的图标，在m_fileList上删除对应的文件信息

            for (int i = 0; i < m_fileList.size(); i++) {
                if (m_fileList.at(i)->fileName == fileInfo->fileName) {
                    // 获取对应的listWidget
                    for (int j = 0; j < ui->listWidget->count(); j++) {
                        QListWidgetItem *item = ui->listWidget->item(j);
                        if (item->text() == fileInfo->fileName) {
                            // 删除listWidget上对应的图标
                            ui->listWidget->removeItemWidget(item);
                            delete item;
                            break;
                        }
                    }
                    // 删除m_fileList上的数据
                    m_fileList.removeAt(i);
                    delete fileInfo;
                    break;
                }
            }

        } else if (code =="014") {// 失败
            QMessageBox::warning(this, "删除失败", QString("【%1】文件删除失败!").arg(fileInfo->fileName));
        } else if (code == "013"){ // token验证失败
            QMessageBox::warning(this, "删除失败", QString("账号异常，请重新登录!").arg(fileInfo->fileName));
            emit sigLoginAgain();
            return;
        }
    });

}

void MyFileWidget::getShareFile(FileInfo *fileInfo) {

    QNetworkRequest request; //栈
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    //http://192.168.52.139/dealfiles?cmd=share
    QString url = QString("http://%1:%2/dealfile?cmd=share").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("user", m_loginInfo->user());
    paramsObj.insert("token", m_loginInfo->token());
    paramsObj.insert("filename", fileInfo->fileName);
    paramsObj.insert("md5", fileInfo->md5);
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = m_manager->post(request, data);

    //读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 解析返回的数据
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据：" << QString(data);

        QString code = NetworkData::getCode(data);

/*
    010： 成功
    011： 失败
    012： 别人已经分享此文件
    013： token 验证失败
*/

        if (code == "010"){ // 成功
            fileInfo->shareStatus = 1;
            QMessageBox::information(this, "分享成功", QString("【%1】分享成功!").arg(fileInfo->fileName));
        } else if (code =="011") {// 失败
            QMessageBox::warning(this, "分享失败", QString("【%1】分享失败!").arg(fileInfo->fileName));
        } else if (code == "012") { //别人已经分享此文件
            QMessageBox::warning(this, "分享失败", QString("【%1】别人已经分享过!").arg(fileInfo->fileName));
        } else if (code == "013"){ // token验证失败
            QMessageBox::warning(this, "分享失败", QString("账号异常，请重新登录!").arg(fileInfo->fileName));
            emit sigLoginAgain();
            return;
        }
    });

}












