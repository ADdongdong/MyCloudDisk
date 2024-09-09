#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>

namespace Ui {
class TitleWidget;
}

class TitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleWidget(QWidget *parent = nullptr);
    ~TitleWidget();

protected:
    // 重写鼠标移动事件，使得鼠标可以拖动窗口移动
    void mouseMoveEvent(QMouseEvent *event) override;
    // 重写鼠标移动事件
    void mousePressEvent(QMouseEvent *event) override;

signals:
    // 定义信号，发送信号给父类窗口，让父类窗口去切换到设置界面
    void showSetPage();

    // 发送信号给父窗口，让父窗口去切换到登录界面，如果不是登录界面，那按下close就切换到登录界面
    void closeButton();
private slots:
    void on_min_clicked();

    void on_close_clicked();

    void on_set_clicked();

private:
    Ui::TitleWidget *ui;
    QPoint m_pt;
    QWidget *m_parent;
};

#endif // TITLEWIDGET_H
