#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "common.h"
#include "logininfoinstance.h"
#include "mainwindow.h"


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

protected:
    // 登录窗口添加背景图片,重写绘图事件
    //void painEvent(QPaintEvent *event);

private slots:
    void on_notReg_clicked();
    void showSetPage(); // 定义槽函数，切换到设置页面, 用来和titlewidget发送来的信号关联
    void closeButton();// 设置关闭按钮，如果在登录页面直接关闭，如果不在登录页面，切换到登录页面


    void on_loginButton_clicked();

    void on_regButton_clicked();

    /**
     * @brief 设置ip地址和端口号确定按钮
     */
    void on_set_2_clicked();

private:
    /**
     * @brief 从conf/cfg.json文件中读取配置信息
     */
    void readConf();

    /**
     * @brief 登录时候保存用户信息，包括token信息
     * @param username 用户名
     * @param rootObj
     * @param ip
     * @param port
     */
    void saveLoginInfoData(QString username, QJsonValue rootObj, QString ip, QString port);

private:
    Ui::LoginDialog *ui;
    Common *m_common;
    MainWindow *m_mainwindow;
};

#endif // LOGINDIALOG_H
