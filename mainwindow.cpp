#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 去掉标题栏
    this->setWindowFlag(Qt::FramelessWindowHint);

    // 哥buttongroup的对象的中的m_parent赋值为mainwindow, 多态调用，父类指针调用子类对象的函数
    ui->button_group->setParent(this);

    // 处理按钮信号
    buttonsSingals();

    // 处理关闭窗口信号
    windowSingal();

    // stackedWidget窗口切换
    QObject::connect(ui->myFile_page, &MyFileWidget::gotoTransform, this, [=](TransformStatus status){
        ui->button_group->onMapperButtonClicked("传输列表");
        ui->transform_page->showPage(status);
    });

}

void MainWindow::init(QString user){
    // 设置用户名称
    ui->button_group->setUser(user);
    // 初始化用户数据
    ui->myFile_page->getMyFileCount();
}

void MainWindow::buttonsSingals(){
    //ui->myFile_page->setStyleSheet("background-color: rgb(29, 212, 5);");
    //ui->download_page->setStyleSheet("background-color: rgb(100, 212, 100);");
    // ui->share_page->setStyleSheet("background-color: rgb(29,0, 5);");
    // ui->transform_page->setStyleSheet("background-color: rgb(29,100, 100);");

    // 设置默认页面为myfile页面
    ui->stackedWidget->setCurrentWidget(ui->myFile_page);

    // 进入文件页面，刷新文件页面
    QObject::connect(ui->button_group, &ButtonGroup::sigMyFile, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->myFile_page);
    });

    // 进入下载榜单页面，刷榜单页面
    QObject::connect(ui->button_group, &ButtonGroup::sigDownload, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->download_page);
        ui->download_page->refreshFiles();
    });
    QObject::connect(ui->button_group, &ButtonGroup::sigShare, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->share_page);
        // 初始化分享列表数据
        ui->share_page->getShareFilesCount();
    });
    QObject::connect(ui->button_group, &ButtonGroup::sigTransform, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->transform_page);
    });

    // 切换用户
    QObject::connect(ui->button_group, &ButtonGroup::sigSwitchUser, this, [=](){
        emit sigChangeUser();
    });

    // token过期，重新登录
    QObject::connect(ui->myFile_page, &MyFileWidget::sigLoginAgain, this, [=](){
        emit sigLoginAgain();
    });

    // 信号传递
    // 将share_page的信号传递给myFile_page。相当与myFile_page发送这个信号
    connect(ui->share_page, &ShareWidget::gotoTransform, ui->myFile_page, &MyFileWidget::gotoTransform);
}

void MainWindow::windowSingal() {
    QObject::connect(ui->button_group, &ButtonGroup::minWindow, this, [=](){
        this->setWindowState(Qt::WindowMinimized);
    });

    QObject::connect(ui->button_group, &ButtonGroup::maxWindow, this, [=](){
        static bool mainFlag = false;
        if (!mainFlag) {
            this->showMaximized();
            mainFlag = true;
        } else {
            this->showNormal();
            mainFlag = false;
        }
    });

    QObject::connect(ui->button_group, &ButtonGroup::closeWindow, this, [=](){
        this->close();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

