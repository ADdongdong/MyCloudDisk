#include "downloadlayout.h"

DownloadLayout::DownloadLayout()
{

}

DownloadLayout *DownloadLayout::m_instance = new DownloadLayout;


DownloadLayout::~DownloadLayout(){

}

DownloadLayout* DownloadLayout::getInstance(){
    return m_instance;
}

void DownloadLayout::setDownloadLayout(QWidget *parent){
    // parent: uploadScroll
    // 使用垂直布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    m_vLayout = vLayout;
    // 加一个弹簧，将下载的文件进度条，顶到最上面去
    m_vLayout->addStretch();

    parent->setLayout(vLayout);

}

QVBoxLayout* DownloadLayout::getDownloadLayout(){
    return m_vLayout;
}



