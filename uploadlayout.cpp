#include "uploadlayout.h"

UploadLayout *UploadLayout::m_instance = new UploadLayout;

UploadLayout::UploadLayout()
{

}

UploadLayout::~UploadLayout(){

}

UploadLayout* UploadLayout::getInstance(){
    return m_instance;
}

void UploadLayout::setUploadLayout(QWidget *parent){
    // parent: uploadScroll
    // 使用垂直布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    m_vLayout = vLayout;
    // 加一个弹簧，将下载的文件进度条，顶到最上面去
    m_vLayout->addStretch();

    parent->setLayout(vLayout);

    /* testDemo
    for (int i = 0; i < 10; i++) {
        FileDataProgress *f1 = new FileDataProgress();
        f1->setFileName(QString("文件%1").arg(i+1));
        // 每一次都添加到最上面
        // 第一个参数为0，表示每一次添加到最上面, index=0在最上面
        vLayout->insertWidget(0, f1);
    }
    */
}

QVBoxLayout* UploadLayout::getUploadLayout(){
    return m_vLayout;
}



