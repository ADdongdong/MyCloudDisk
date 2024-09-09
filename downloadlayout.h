#ifndef DOWNLOADLAYOUT_H
#define DOWNLOADLAYOUT_H

/**
 * @details 下载布局类实现和上传类实现一样
 */

#include <QVBoxLayout>
#include <QWidget>

class DownloadLayout
{
public:
    static DownloadLayout* getInstance();

    void setDownloadLayout(QWidget *parent);

    QVBoxLayout* getDownloadLayout();
private:
    DownloadLayout();
    ~DownloadLayout();

private:
    static DownloadLayout* m_instance;
    QVBoxLayout* m_vLayout;
};

#endif // DOWNLOADLAYOUT_H
