#include "downloadtask.h"

DownloadTask* DownloadTask::m_instance = new DownloadTask;

DownloadTask::DownloadTask()
{

}

DownloadTask::~DownloadTask(){

}

DownloadTask* DownloadTask::getInstance(){
    return m_instance;
}

bool DownloadTask::isEmpyt(){
    return m_fileList.isEmpty();
}

DownloadFileInfo* DownloadTask::takeTask(){
    DownloadFileInfo *temp = nullptr;
    if (m_fileList.size()>0){
       temp = m_fileList.at(0);
    }
    return temp;
}

void DownloadTask::deleteDownloadTask(){

    // 上传完成后需要处理的：
    // 1.FileDataProgress(fdp)对象从vLayout中移除
    // 2.删除FileDataProgress(fdp)对象
    // 3.删除UploadFileInfo对象

    for (int i = 0; i < m_fileList.size(); i++) {
        DownloadFileInfo* downFileInfo = m_fileList.takeAt(0); // takeAt函数会取出对应的元素

        // 1. FileDataProgress(fdp)对象从vLayout中移除

        QVBoxLayout *vLayout = DownloadLayout::getInstance()->getDownloadLayout();
        vLayout->removeWidget(downFileInfo->fdp);

        delete downFileInfo->fdp;
        QFile *file = downFileInfo->file;
        file->close();// 关闭文件
        delete file;
        // 3. 删除uploaafileinfo对象
        delete downFileInfo;
    }
    /*
    UploadFileInfo *uploadFileInfo = m_fileList.takeAt(0);
    // 删除掉拿出来的堆区对象
    if (uploadFileInfo != nullptr){
        delete uploadFileInfo;
    }
    */
}

int DownloadTask::appendDownloadTask(FileInfo *fileInfo, QString filePath, bool isShareTask){
    QFile *file = new QFile(filePath);
    // 打开要下载到的目录
    if (!file->open(QIODevice::WriteOnly)){
        qDebug() << "文件打开失败";
        delete file;
        file = nullptr;
        return -2;

    } else {
        // 以只写的方式打开文件

        // 添加到下载的任务列表中
        // 对象有那些属性
        DownloadFileInfo *dfi = new DownloadFileInfo;
        dfi->user = fileInfo->user;
        dfi->fileName = fileInfo->fileName;
        dfi->filePath = filePath;
        dfi->md5 = fileInfo->md5;
        dfi->url = fileInfo->url;
        dfi->file = file;
        dfi->isShareTask = isShareTask;


        // 上传进度条显示, 将进度条UI显示vLayout上
        FileDataProgress *fdp = new FileDataProgress();
        fdp->setFileName(dfi->fileName);
        dfi->fdp = fdp;

        QVBoxLayout* vlayout = DownloadLayout::getInstance()->getDownloadLayout();
        // 这里完成添加展示进度条的操作，这里vlayout是一个uploadScroll的布局，将fdp添加到这个布局中
        // 就完成了进度条的展示
        vlayout->insertWidget(0, fdp);

        // 将这个文件添加到m_fileList中
        m_fileList.append(dfi);
    }
    return 0;
}
