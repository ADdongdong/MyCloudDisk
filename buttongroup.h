#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QString>
#include <QToolButton>
#include <QSignalMapper> // QT6被弃用
#include <QMap>

namespace Ui {
class ButtonGroup;
}

class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonGroup(QWidget *parent = nullptr);
    ~ButtonGroup();

    // 设置父类，当前buttonGroup的父类，不是mainwindow
    // 是QWidget，QWidget的父类才是QMainwindwo,我们要控制的是QMainwindow,所以，要设置buttonGroup的父类
    // 设置为QMainwindow
    void setParent(QWidget* parent);

    /**
     * @brief 显示登录用户的名称
     * @param user
     */
    void setUser(QString user);
    void defaultPage();

protected:
    // 重写鼠标移动事件，使得鼠标可以拖动窗口移动
    void mouseMoveEvent(QMouseEvent *event) override;
    // 重写鼠标移动事件
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void minWindow();
    void maxWindow();
    void closeWindow();

    void sigMyFile();
    void sigShare();
    void sigDownload();
    void sigTransform();
    void sigSwitchUser();

public slots:
    void onMapperButtonClicked(QString text);

private:
    Ui::ButtonGroup *ui;
    QWidget* m_parent;
    QPoint m_pt;
    QToolButton *m_curBtn; // 当前选中的按钮

    QMap<QString, QToolButton*> m_map;
};

#endif // BUTTONGROUP_H
