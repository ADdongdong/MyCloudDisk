#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init(QString user);

signals:
    // 点击切换用户，发送信号给logindialog对象
    void sigChangeUser();

    // token过期，发送重新登录的信号，等待logindialog来接受处理
    void sigLoginAgain();

private:
    // 定义函数，处理主界面按钮信号
    void buttonsSingals();

    // 定义函数，处理mainwindow开关，放大窗口信号
    void windowSingal();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
