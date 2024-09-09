#ifndef UPLOADLAYOUT_H
#define UPLOADLAYOUT_H

/**
 * @brief 上传进度布局类，单例模式
 *        设置的是uploadScroll的布局，进度条是要添加到这个容器里面的
 */
#include <QWidget>
#include <QVBoxLayout>
#include "filedataprogress.h"


class UploadLayout
{
public:
    static UploadLayout* getInstance();

    /**
     * @brief 设置上传的布局变量
     * @param parent
     */
    void setUploadLayout(QWidget *parent);

    /**
     * @brief 获取上传的布局变量
     * @return
     */
    QVBoxLayout* getUploadLayout();
private:
    UploadLayout();
    ~UploadLayout();

private:
    static UploadLayout *m_instance;

    QVBoxLayout *m_vLayout;
};

#endif // UPLOADLAYOUT_H
