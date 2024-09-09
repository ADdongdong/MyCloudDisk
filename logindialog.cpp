#include "logindialog.h"
#include "ui_logindialog.h"
#include <QPainter>
#include <QDebug>
#include <QString>
#include <QRegularExpression>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // mainwindow不需要指定父窗口
    m_mainwindow = new MainWindow();
    // 设置mainwindow的图标
    // m_mainwindow->setWindowIcon(QIcon);
    m_common = Common::getInstance();

    // 处理信号，mainwindow发送过来的切换用户的信号，切换用户的时候要转到logindialog界面
    QObject::connect(m_mainwindow, &MainWindow::sigChangeUser, this, [=](){
        m_mainwindow->hide();// mainwindow隐藏
        this->show(); // logindialog展示
    });

    // 处理信号，mainwindow发送过来的重新登录信号，重新登录要转到logindialog界面
    QObject::connect(m_mainwindow, &MainWindow::sigLoginAgain, this, [=](){
        // 重新登录
        m_mainwindow->hide();
        this->show();
    });

    // 去掉标题栏
    this->setWindowFlag(Qt::FramelessWindowHint);

    // 设置登录界面提示字体加粗(如果设计了样式表，则不生效）
    // 使用this->setFont可以设计当前窗口的所有字体
    //this->setFont(QFont("楷体", 12, QFont::Bold, false));

    //ui->stackedWidget->currentIndex();
    // 将titlewidget发送来的展示设置界面的信号和槽函数关联
    QObject::connect(ui->title_widget, &TitleWidget::showSetPage, this, &LoginDialog::showSetPage);
    QObject::connect(ui->title_widget, &TitleWidget::closeButton, this, &LoginDialog::closeButton);

    // 设置初始化页面为登录页面
    ui->stackedWidget->setCurrentIndex(0);

    // 读取cfg.json文件
    readConf();

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::readConf() {
    // 加载login界面信息
    QString user = m_common->getConfValue("login", "user");
    QString pwd = m_common->getConfValue("login", "pwd");
    QString remember = m_common->getConfValue("login", "remember");

    // 加载webserver网络ip和port信息
    QString ip = m_common->getConfValue("web_server","ip");
    QString port = m_common->getConfValue("web_server","port");

    // 将login页面内容加载到login页面
    if (remember == CONF_REMEMBER_YES){
        // <1> base64解密
        // 记住密码
        QByteArray pwdTemp = QByteArray::fromBase64(pwd.toLocal8Bit());
        // <2> des解密
        unsigned char encPwd[512] = {0};
        int encPwdLen =0;
        int ret = DesDec((unsigned char*)pwdTemp.data(),
               pwdTemp.size(), encPwd, &encPwdLen);
        if(ret != 0) {
            qDebug() << "解密失败";
        }

        QString passward = QString::fromLocal8Bit((const char*)encPwd, encPwdLen);
        qDebug() << "pwd:" << passward;

        ui->savepassward->setChecked(true);
        ui->password->setText(passward);
    } else {
        // 没有选中记住密码
        ui->savepassward->setChecked(false);
        ui->password->setText("");

    }

    // 用户名解密(用户名无论什么有没有选中记住密码都要进行解密)
    // 1.base64解密（后加密的，先解密，第二次是进行的base64加密，所以，先解base64密码）
    // 2.des解密

    // <1> base64解密
    // toLocak8Bit，转为本地字符串
    QByteArray userTemp = QByteArray::fromBase64(user.toLocal8Bit());

    // <2> des解密
    unsigned char encUsr[512] = {0};
    int encUsrLen =0;
    int ret = DesDec((unsigned char*)userTemp.data(),
           userTemp.size(), encUsr, &encUsrLen);
    if(ret != 0) {
        qDebug() << "解密失败";
    }

    QString userName = QString::fromLocal8Bit((const char*)encUsr, encUsrLen);
    qDebug() << "userName:" << userName;
    ui->username->setText(userName);


    // 将设置webserver的信息从配置文件中加载、
    ui->server_ip->setText(ip);
    ui->server_port->setText(port);
}

/**
 * @brief 登录窗口添加背景图片,重写绘图事件
 * @todo 添加背景图片（目前觉的没图片好看）
 */
/*
void LoginDialog::painEvent(QPaintEvent *event){
    QPainter p(this);

    QPixmap bg(":path/to/logine_bc.jpg");
    // 图片绘制位置是从窗口的（0,0）点开始绘制
    p.drawPixmap(0,0, this->width(), this->height(), bg);
}
*/

/**
 * @brief 点击注册，跳转到注册页面
 */

void LoginDialog::on_notReg_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->reg);
}

/**
 * @brief 切换到设置页面
 */
void LoginDialog::showSetPage()
{
    ui->stackedWidget->setCurrentWidget(ui->set);
}

/**
 * @brief 关闭按按钮对应的槽函数, 信号在titlewidget中发送
 */
void LoginDialog::closeButton()
{
    if (ui->stackedWidget->currentIndex() == 0) {
        this->close();
    } else {
        ui->stackedWidget->setCurrentWidget(ui->login);
    }
}

/**
 * @brief 登录，正则表达是来检测输入框的内容
 */
void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->username->text();
    QString passward = ui->password->text();

    QRegularExpression regexp(USER_REG);
    QRegularExpressionMatch match = regexp.match(username);
    if(!match.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->username->clear();
        ui->username->setFocus();
        return;
    }

    QRegularExpression pwsreg(PASSWD_REG);
    QRegularExpressionMatch pwdmatch = pwsreg.match(passward);
    if(!pwdmatch.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "密码格式不正确");
        ui->password->clear();
        ui->password->setFocus();
        return;
    }

    // 正确就不提示了
    // QMessageBox::information(this, "提示", "用户名密码格式正确");

    // 处理json
    // 多个地方都要处理json,所以将json抽象出去，单独在一个文件中写
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request; // 在栈中
    // QString url = QString("http://172.20.203.67/login");
    // 修改为从配置文件获取的信息
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    QString url = QString("http://%1:%2/login").arg(ip).arg(port);
    request.setUrl(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    // 3. 封装请求数据（json)
    QJsonObject paramsObj;
    paramsObj.insert("user", username);
    paramsObj.insert("pwd", m_common->getStrMd5(passward));// passward使用md5加密
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply = manager->post(request, data);

    // 5. 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        // 解析返回的数据
        QJsonParseError err;
        QJsonDocument rootDoc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError){
            qDebug() << "服务器返回消息Json格式错误";
        } else {
            // 解析json
            QJsonObject rootObj = rootDoc.object();
            QJsonValue codeVal = rootObj.value("code");
            if (codeVal.type() == QJsonValue::String) {
                // qDebug()<< "code:" << codeVal.toString();
                QString code = codeVal.toString();
                if (code == "000") {
                    // QMessageBox::information(this, "提示", "登录成功");
                    // 如果登录成功，我们要把用户名和密码保存起来，方便下次自动登录
                    bool isremember = ui->savepassward->isChecked();

                    // 如果没有选择保存密码，则在登录后将密码栏设为空
                    if (isremember == false){
                        ui->password->setText("");
                    }

                    // 将登录信息写入到配置文件中
                    m_common->writeLoginInfo(username, passward, isremember);
                    // 将webserver信息写入到配置文件中
                    m_common->writeWebInfo(ip, port);

                    // 跳转到其他页面
                    // todo 其他操作
                    // 获取token
                    QJsonValue tokenValue= rootObj.value("token");
                    // 保存数据：token, user, ip, port
                    saveLoginInfoData(username, tokenValue, ip, port);

                    // 跳转到主界面, 隐藏当前界面
                    // 设置用户名
                    // m_mainwindow->setUser(username);
                    m_mainwindow->init(username);
                    m_mainwindow->show();
                    this->hide();

                } else if (code == "001") {
                    QMessageBox::critical(this, "错误", "登录失败，请检查密码是否正确！");
                }
            }
        }

        // 立即销毁
        // delete reply;
        // 延迟销毁
        // 调用QObject::deleteLater()这个函数并没有立即销毁，而是向主消息循环发送了一个event,
        // 下一次主循环收到这个event之后才会销毁对象。
        reply->deleteLater();
    });

}

/**
 * @brief 保存登录信息，包括token信息
 */
void LoginDialog::saveLoginInfoData(QString username, QJsonValue tokenValue, QString ip, QString port) {
// 创建一个对象来保存
    // 除了登录外：每一个请求，都需要校验token, 每一个请求都需要带token
    LoginInfoInstance* loginInfo = LoginInfoInstance::getInstance();

    qDebug() << "token:" << tokenValue.toString();

    // 保存用户数据
    loginInfo->setToken(tokenValue.toString());
    loginInfo->setUser(username);
    loginInfo->setIp(ip);
    loginInfo->setPort(port);

}

/**
 * @brief 实现用户注册
 */
void LoginDialog::on_regButton_clicked()
{
    // 用户注册
    QString username = ui->reg_username->text();
    QString nickname = ui->reg_nickname->text();
    QString password = ui->reg_passward->text();
    QString surepwd  = ui->reg_surepassward->text();
    QString phonenumber = ui->reg_phonenumber->text();
    QString email = ui->reg_email->text();

    // 对获取到的文本数据进行校验
    QRegularExpression regexp(USER_REG);
    QRegularExpressionMatch match = regexp.match(username);
    if (!match.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->reg_username->clear();
        ui->reg_username->setFocus();
        return;
    }

    // 昵称校验
    match = regexp.match(nickname);
    if (!match.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "昵称格式不正确");
        ui->reg_nickname->clear();
        ui->reg_nickname->setFocus();
        return;
    }


    // 密码校验
    regexp.setPattern(PASSWD_REG);
    match = regexp.match(password);
    if (!match.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "密码格式不正确");
        ui->reg_passward->clear();
        ui->reg_passward->setFocus();
        return;
    }

    // 判断确认密码是否正确
    if (password != surepwd) {
        // 如果两次输入的密码不一致
        QMessageBox::warning(this, "警告", "两次输入密码不一致");
        ui->reg_surepassward->clear();
        ui->reg_surepassward->setFocus();
        return;
    }

    // 电话号码校验
    regexp.setPattern(PHONE_REG);
    match = regexp.match(phonenumber);
    if (!match.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "电话格式不正确");
        ui->reg_phonenumber->clear();
        ui->reg_phonenumber->setFocus();
        return;
    }

    // 邮箱校验
    regexp.setPattern(EMAIL_REG);
    match = regexp.match(email);
    if (!match.hasMatch()) {
        // 如果校验失败
        QMessageBox::warning(this, "警告", "邮箱格式不正确");
        ui->reg_email->clear();
        ui->reg_email->setFocus();
        return;
    }

    // 发送http请求
    // 1. 获取输入框中的数据
    // 2. 校验数据
    // 3. 发送http请求
    //     1. 设置url
    //     2. 设置请求头
    //     3. 封装请求数据（json)
    //     4. 发送请求（get/post/put/delete
    //     5. 读取服务器返回的数据

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request; // 在栈中
    //QString url = QString("http://172.20.203.67/reg");
    // 修改为从设置页面获取
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");

    QString url = QString("http://%1:%2/reg").arg(ip).arg(port);
    request.setUrl(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    // 3. 封装请求数据（json)
    QJsonObject paramsObj;
    paramsObj.insert("email", email);
    paramsObj.insert("userName", username);
    paramsObj.insert("nickName", nickname);
    paramsObj.insert("phone", phonenumber);
    paramsObj.insert("firstPwd", m_common->getStrMd5(password)); // 只有再风爪给你请求的时候，才对密码加密
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager->post(request, data);

    //     5. 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // 读数据
        QByteArray data = reply->readAll();
        qDebug() << "服务器返回数据:" << QString(data);

        // 解析返回的数据
        QJsonParseError err;
        QJsonDocument rootDoc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError){
            qDebug() << "Json格式错误";
        } else {
            // 解析json
            // "{\n\t\"code\":\t\"002\"\n}"
            QJsonObject rootObj = rootDoc.object();
            QJsonValue codeVal = rootObj.value("code");
            if (codeVal.type() == QJsonValue::String) {
                // qDebug()<< "code:" << codeVal.toString();
                QString code = codeVal.toString();
                if (code == "002") {
                    QMessageBox::information(this, "提示", "注册成功");
                } else if (code == "003") {
                    QMessageBox::information(this, "提示","用户已经存在");
                } else {
                    QMessageBox::critical(this, "错误", "失败");
                }

            }
        }
        reply->deleteLater();
    });
}


void LoginDialog::on_set_2_clicked()
{
    QString ip = ui->server_ip->text();
    QString port = ui->server_port->text();


    // 数据正则校验
    // 服务器ip
    QRegularExpression regexp(IP_REG);
    QRegularExpressionMatch match = regexp.match(ip);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "警告", "请输入正确的IP格式！");
        return;
    }

    // 服务器端口号
    regexp.setPattern(PORT_REG);
    match = regexp.match(port);
    if (!match.hasMatch()){
        QMessageBox::warning(this, "警告", "请输入正确的端口号");
        return;
    }
    // 跳转到登录界面
    ui->stackedWidget->setCurrentWidget(ui->login);
    // 将端口和ip写入到配置文件中
    m_common->writeWebInfo(ip, port);

}
