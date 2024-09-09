#include "titlewidget.h"
#include "ui_titlewidget.h"
TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWidget)
{
    ui->setupUi(this);

    // 设置窗口logo
    ui->logo->setPixmap(QPixmap(":/images/logo.png").scaled(30, 30));

    m_parent = parent;
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

/**
 * @brief 重写鼠标移动事件使得鼠标可以拖动登录窗口移动
 * @param event
 */
void TitleWidget::mouseMoveEvent(QMouseEvent *event)  {
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
void TitleWidget::mousePressEvent(QMouseEvent *event) {
    // 只允许鼠标左键按下
    if (event->buttons() == Qt::LeftButton) {
        // 求差值, 差值是当前控件左上角的位置到鼠标现在的位置
        // 差值 = 鼠标当前位置 - 窗口左上角位置
        m_pt = event->globalPosition().toPoint() - m_parent->geometry().topLeft();
    }
}

void TitleWidget::on_min_clicked()
{
    m_parent->setWindowState(Qt::WindowMinimized);
}

void TitleWidget::on_close_clicked()
{
    // m_parent->close();
    emit closeButton();
}

/**
 * @brief 点击进入设置页面
 */
void TitleWidget::on_set_clicked()
{
    // 发送信号

    emit showSetPage();
}
