#include "common.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QFile>
#include <QDebug>

Common* Common::m_instance = new Common;

Common::Common()
{
    m_manager = new QNetworkAccessManager(this);
    m_random = QRandomGenerator(2);

    getFileTypeList();
}

Common* Common::getInstance() {
    return m_instance;
}


// 从配置文件中得到对应的参数值
QString Common::getConfValue(QString title, QString key, QString path)
{
    QFile file(path);
    if (false == file.open(QIODevice::ReadOnly)){
        // 打开文件失败
        qDebug() << "打开cfg.json文件失败";
        return NULL;
    }

    QByteArray json = file.readAll();
    file.close(); // 关闭文件

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json, &err);
    if (err.error == QJsonParseError::NoError){
        // 没有出错
        if (doc.isNull() || doc.isEmpty()) {
            qDebug() << "doc is null or doc is empty";
            return NULL;
        }

        if (doc.isObject()) {
            QJsonObject rootObj = doc.object();
            QJsonValue titleValue = rootObj.value(title);
            if (titleValue.type() == QJsonValue::Object) {
                QJsonObject titleObj = titleValue.toObject();

                QStringList list = titleObj.keys();
                for (int i = 0; i < list.size(); i++){
                    QString keyTemp = list.at(i);
                    if (keyTemp == key) {
                        return titleObj.value(key).toString();
                    }
                }

            }
        }
    } else {
        qDebug() << "err = " << err.errorString();
    }
    return "";
}

void Common::writeLoginInfo(QString user, QString pwd, bool isRemeber, QString path)
{
    // <1>将web_server的ip和端口信息保存起来
    /*
    QString ip = getConfValue("web_server", "ip");
    QString port = getConfValue("web_server", "port");

    QMap<QString, QVariant> web_server;
    web_server.insert("ip", ip);
    web_server.insert("port", port);
    qDebug() << "ip:" << ip << ",port:" <<port;
    */



    // <2>将login登录信息保存起来
    // toLocal8bit 转换为本地字符集，如果为windows则为gbk编码，如果是linux则为utf-8
    // 做两次加密, 使用base64进行加密
    // <2.1>进行des加密（引入des.h des.c文件）
    // 登录用户名进行加密
    unsigned char encUsr[1024] = {0};
    int encUsrLen;
    int ret = DesEnc((unsigned char*)user.toLocal8Bit().data(),
           user.toLocal8Bit().length(),
           encUsr, &encUsrLen);
    if (ret != 0){
        // 加密失败
        qInfo()<<"debug 加密用户名失败";
        return;
    }

    // 密码进行加密
    unsigned char encPwd[1024] = {0};
    int encPwLen;
    ret = DesEnc((unsigned char*)pwd.toLocal8Bit().data(),
           pwd.toLocal8Bit().length(),
           encPwd, &encPwLen);
    if (ret != 0){
        // 加密失败
        qInfo()<<"debug 加密密码失败";
        return;
    }

    // <2.2>进行base64加密

    QString base64User = QByteArray((char*)encUsr, encUsrLen).toBase64();
    QString base64Pwd = QByteArray((char*)encPwd, encPwLen).toBase64();

    // 3. 将配置信息保存在配置文件中
    QMap<QString, QVariant> login;
    login.insert("pwd", base64Pwd);
    login.insert("user", base64User);

    if (isRemeber == true){
        login.insert("remember", CONF_REMEMBER_YES);
    } else {
        login.insert("remember", CONF_REMEMBER_NO);
    }


    QMap<QString, QVariant> json;
    json.insert("login", login);

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(json);

    // 保存到文件
    QFile file(path);
    if (false == file.open(QIODevice::WriteOnly)) {
        qDebug() <<"文件打开失败";
    }else {
        file.write(jsonDocument.toJson());
        file.close();// 关闭文件
    }
}

void Common::writeWebInfo(QString ip, QString port, QString path){
    // web_server信息
    QMap<QString, QVariant> server;
    server.insert("ip", ip);
    server.insert("port",port);

    // login信息
    QString user = getConfValue("login", "user");
    QString pwd = getConfValue("login", "pwd");
    QString remember = getConfValue("login", "remember");

    QMap<QString, QVariant> login;
    login.insert("user", user);
    login.insert("pwd", pwd);
    login.insert("remember", remember);


    QMap<QString, QVariant> json;
    json.insert("web_server", server);
    json.insert("login", login);


    QJsonDocument jsonDocument = QJsonDocument::fromVariant(json);

    // 保存到文件
    QFile file(path);
    if (false == file.open(QIODevice::WriteOnly)) {
        qDebug() <<"文件打开失败";
    }else {
        file.write(jsonDocument.toJson());
        file.close();// 关闭文件
    }
}

QString Common::getStrMd5(QString str){
    QByteArray arr;
    arr = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md5);

    return arr.toHex();
}

QString Common::getFileMd5(QString filePath){
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly)){
        // 文件打开错误
        qDebug() << "file open error.";
        return "";
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64	bytesToWrite = 0;
    quint64	loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while(1) {
        if (bytesToWrite >0) {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        } else {
            break;
        }

        if (bytesWritten == totalBytes) {
            break;
        }
    }

    if (localFile.isOpen()) {
        localFile.close();
    }

    QByteArray md5 = ch.result();
    return md5.toHex();
}


QNetworkAccessManager* Common::getQNetworkAccessManager(){
    return m_manager;
}

// 读取conf/fileType文件夹，得到文件夹中的所有文件名，保存在m_fileTypeList中
void Common::getFileTypeList(){
    QDir dir(FILE_TYPE_DIR);

    // 判断该文件夹是否存在如果不存在，这创建这个文件夹
    if (!dir.exists()) {
        dir.mkpath(FILE_TYPE_DIR);

        qDebug() << FILE_TYPE_DIR << "创建成功";
    }

    dir.setFilter(QDir::Files | QDir::NoDot | QDir::NoDotDot | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    // 对文件进行排序
    dir.setSorting(QDir::Size);


    // 遍历文件夹(conf/fileType)
    QFileInfoList fileInfoList = dir.entryInfoList();
    for(int i =0; i < fileInfoList.size(); i++) {
        QFileInfo fileInfo = fileInfoList.at(i);
        m_fileTypeList.append(fileInfo.fileName());
    }

}

// fileTypeName cpp.png/dll.png 如果能找到则返回，如果找不到则返回"other.png"
QString Common::getFileType(QString fileTypeName) {
    if(m_fileTypeList.contains(fileTypeName)){
        // 如果能找到
        return fileTypeName;
    } else {
        return "other.png";
    }
}

QString Common::getBoundary(){
    //
    char randoms[] = {
        '0', '1','2','3','4','5','6','7','8','9',
        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
    };

    // 随机数产生
    QString temp;
    temp.resize(16);// 设置temp 的长度为16
    int len = sizeof(randoms) - 1;
    for (int i = 0; i < 16; i++) {
        int rand = m_random.bounded(len);
        temp[i] = randoms[rand];
    }


    qDebug() << "temp" << temp;
    QString boundary = "------WebKitFormBoundary" + temp;

    qDebug() << "boundary:" << boundary;
    return	boundary;
}

void Common::writeRecord(QString user, QString fileName, QString code, QString path){

    // conf/record /lisa.txt
    QString userFilePath = QString("%1/%2.txt").arg(path).arg(user);
    qDebug() << "userFilePath:" << userFilePath;

    QDir dir(path);
    if (!dir.exists()) {
        // 如果目录不存在, 创建目录
        if (dir.mkpath(path)) {
            qDebug() << "目录创建成功";
        } else {
            qDebug() << "目录创建失败";
        }
    }

    QFile file(userFilePath);
    QByteArray array;
    if (file.exists()) {
    // 如果存在，先读取文件原来的内容
        if (!file.open(QIODevice::ReadOnly)){
            // 如果打开失败
            qDebug()<< "file.open(QIODevice::ReadOnly) err";
            if (file.isOpen()) {
                file.close();
            }
            return;
        }
        // 读取文件原来的全部内容
        array = file.readAll();
        if (file.isOpen()) {
            file.close();
        }
    }

    if (!file.open(QIODevice::WriteOnly)) {
        // 如果文件打开失败
        qDebug() << "file.open(QIODevice::WriteOnly) err";
        if (file.isOpen()) {
            file.close();
        }
        return;
    }

    // 记录写入到文件
    // xxx.jpg 2024/6/28 16:20:08 上传成功
    QDateTime time = QDateTime::currentDateTime(); // 获取系统当前时间
    QString timeStr = time.toString("yyyy/MM/dd hh:mm:ss"); // 时间格式化
    QString actionString = getActionString(code);

    // 记录到文件的内容
    QString str = QString("%1\t%2\t%3\n").arg(fileName).arg(timeStr).arg(actionString);
    qDebug() << "str:" << str;

    // toLocal8Bit转为本地字符串
    file.write(str.toLocal8Bit());
    if (!array.isEmpty()){
        // 读取到的文件内容不为空的时候, 将原来的文件中的内容加在后面
        file.write(array);
    }

    // 关闭文件
    if (file.isOpen()) {
        file.close();
    }
}

QString Common::getActionString(QString code){
    /*
     * 005: 上传的文件已经存在
     * 006: 秒传成功
     * 007: 秒传失败
     * 008: 上传成功
     * 009: 上传失败
     * 090: 下载成功
     * 091: 下载失败
     */
    QString str;
    if (code == "005") {
        str = "上传的文件已经存在";
    } else if (code == "006"){
        str = "秒传成功";
    } else if (code == "007"){
        str = "秒传失败";
    } else if (code == "008"){
        str = "上传成功";
    } else if (code == "009"){
        str = "上传失败";
    } else if (code == "090"){
        str = "下载成功";
    } else if (code == "091"){
        str = "下载失败";
    }

    return str;
}

void Common::sleep(unsigned int msc){
    QTime dieTime = QTime::currentTime().addMSecs(msc);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}



