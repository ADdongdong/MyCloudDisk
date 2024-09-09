#include "buttongroup.h"
#include "ui_buttongroup.h"
#include <QDebug>

ButtonGroup::ButtonGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonGroup)
{
    ui->setupUi(this);

    // qDebug() << "parent:" << parent->objectName();

    // 最小化
    QObject::connect(ui->btnMin, &QToolButton::clicked, this, [=](){
        emit minWindow();
    });

    // 最大化
    QObject::connect(ui->btnMax, &QToolButton::clicked, this, [=](){
        static bool flag = false;
        if (!flag) {
            ui->btnMax->setIcon(QIcon(":/images/minWindow2.png"));
        } else {
            ui->btnMax->setIcon(QIcon(":/images/maxWindow.png"));
        }
        flag = !flag;
        emit maxWindow();
    });

    // 关闭, 槽函数是发送信号，在mainwindow中接受信号，关闭mainwindow窗口
    QObject::connect(ui->btnClose, &QToolButton::clicked, this, [=](){
        emit closeWindow();
    });


    // 默认选中myfile按钮
    defaultPage();


    // 将一组由表示的发送者的signal链接在一起
    // QSignalMapper 通过setMapping来实现的
    // m_mapper = new QSignalMapper(this);

    m_map.insert(ui->btnMyFile->text(), ui->btnMyFile);
    m_map.insert(ui->btnDownload->text(), ui->btnDownload);
    m_map.insert(ui->btnShare->text(), ui->btnShare);
    m_map.insert(ui->btnTransform->text(), ui->btnTransform);
    //m_map.insert(ui->btnSwitchUser->text(), ui->btnSwitchUser);

    QMap<QString, QToolButton*>::iterator iter = m_map.begin();

    for (; iter != m_map.end(); ++iter) {
        auto button = iter.value();
        auto buttonText = iter.key();
        QObject::connect(button, &QToolButton::clicked, [this, buttonText](){
            onMapperButtonClicked(buttonText);
        });
    }

    /*
     * 在qt6中QSignalMapper已经被弃用
    for (; iter != m_map.end(); iter++) {
        m_mapper->setMapping(iter.value(), iter.key());
        QObject::connect(iter.value(), SIGNAL(clicked(bool)), m_mapper, SLOT(map()));
    }

    QObject::connect(m_mapper, SIGNAL(mapped(QString)), this, SLOT(onMapperButtonClicked(QString)));
    */

    /*
     * 对下面的代码进行优化
    // 关联信号与槽，这里槽函数是发送信号，这个信号在mainwindow中接受，并执行对应槽函数
    connect(ui->btnMyFile, &QToolButton::clicked, this, [=](){
        emit sigMyFile();
        ui->btnMyFile->setStyleSheet("color:blue");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:black");
    });

    connect(ui->btnShare, &QToolButton::clicked, this, [=](){
        emit sigShare();
        ui->btnMyFile->setStyleSheet("color:black");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:blue");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });

    connect(ui->btnDownload, &QToolButton::clicked, this, [=](){
        emit sigDownload();
        ui->btnMyFile->setStyleSheet("color:black");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:blue");
        ui->btnShare->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });

    connect(ui->btnTransform, &QToolButton::clicked, this, [=](){
        emit sigTransform();
        ui->btnMyFile->setStyleSheet("color:black");
        ui->btnTransform->setStyleSheet("color:blue");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });
    */

    // 切换用户按钮, 按下切换用户按钮，显示登录界面，当前界面隐藏
    // 所以，不需要切换用户按钮改变颜色，这里在用户切换完以后，新用户登录
    // 继续将myfile我的文件颜色改变为蓝色就行了
    connect(ui->btnSwitchUser, &QToolButton::clicked, this, [=](){
        emit sigSwitchUser(); // 1.发送信号，切换到登录界面
        m_curBtn = ui->btnMyFile; // 2.将当前按钮切换为btnMyfile
        ui->btnMyFile->setStyleSheet("color:blue");
        ui->btnTransform->setStyleSheet("color:black");
        ui->btnDownload->setStyleSheet("color:black");
        ui->btnShare->setStyleSheet("color:black");
        ui->btnSwitchUser->setStyleSheet("color:black");
    });
}

void ButtonGroup::onMapperButtonClicked(QString text) {
    QToolButton* btnTemp = nullptr; // 选中的按钮
    for (auto iter = m_map.begin(); iter!= m_map.end(); ++iter) {
        if (iter.key() == text) {
            btnTemp = (*iter);
        }
    }

    // 如果选中的按钮和m_curBtn想通，那就不执行操作，说明已经在期望的页面了
    if (btnTemp == m_curBtn) {
        return;
    }

    m_curBtn->setStyleSheet("color:black");

    // 1. 当前按钮设置选中颜色
    m_curBtn = btnTemp;
    btnTemp->setStyleSheet("color:blue");

    // 2.发送信号
    if (text == ui->btnMyFile->text() ) {
        emit sigMyFile();
    } else if (text == ui->btnShare->text()) {
        emit sigShare();
    } else if (text == ui->btnDownload->text()) {
        emit sigDownload();
    } else if (text == ui->btnTransform->text()) {
        emit sigTransform();
    }
    // qDebug() << "text:" << text;
}

void ButtonGroup::defaultPage(){
    // 默认的页面是btnMyfile
    m_curBtn = ui->btnMyFile;
    m_curBtn->setStyleSheet("color:blue");
    ui->btnSwitchUser->setStyleSheet("color:black");

}
// 方法(面相对象) == 函数
void ButtonGroup::setParent(QWidget *parent){
    // 在mainwindow.cpp的构造函数中调用
    // ui->button_group->setParent(this); // mainwindow的构造函数中调用
    m_parent = parent;
}

void ButtonGroup::setUser(QString user){
    ui->btnUser->setText(user);
}

/**
 * @brief 重写鼠标移动事件使得鼠标可以拖动登录窗口移动
 * @param event
 */
void ButtonGroup::mouseMoveEvent(QMouseEvent *event)  {
    // 只允许鼠标左键拖动
        // 窗口需要跟着移动
        // 窗口左上角的点 = 鼠标当前位置 - 差值
    if (event->buttons() == Qt::LeftButton) {
        m_parent->move(event->globalPosition().toPoint() - m_pt); // m_parent指loginDialog,
                                                                  // 最终实现loginDialog的移动
    }
}

/**
 * @brief mousePressEvent
 * @param event
 */
void ButtonGroup::mousePressEvent(QMouseEvent *event) {
    // 只允许鼠标左键按下
    if (event->buttons() == Qt::LeftButton) {
        // 求差值, 差值是当前控件左上角的位置到鼠标现在的位置
        // 差值 = 鼠标当前位置 - 窗口左上角位置
        m_pt = event->globalPosition().toPoint() - m_parent->geometry().topLeft();
    }
}


ButtonGroup::~ButtonGroup()
{
    delete ui;
}
