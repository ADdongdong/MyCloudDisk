#include "uploadtask.h"

UploadTask* UploadTask::m_instance = new UploadTask;

UploadTask::UploadTask()
{

}

UploadTask* UploadTask::getInstance(){
    return m_instance;
}

bool UploadTask::isEmpyt(){
    return m_fileList.isEmpty();
}


UploadFileInfo* UploadTask::takeTask(){
    UploadFileInfo *temp = nullptr;
    if (m_fileList.size()>0){
       temp = m_fileList.at(0);
       temp->uploadStatus = UPLODING; // 状态更改为正在上传
    }
    return temp;
}

void UploadTask::deleteUploadTask(){

    // 上传完成后需要处理的：
    // 1.FileDataProgress(fdp)对象从vLayout中移除
    // 2.删除FileDataProgress(fdp)对象
    // 3.删除UploadFileInfo对象

    for (int i = 0; i < m_fileList.size(); i++) {
        UploadFileInfo *tmp = m_fileList.at(i);
        if (tmp->uploadStatus == UPLOAD_FAILD
                || tmp->uploadStatus == UPLOAD_FINISHED
                || tmp->uploadStatus == UPLOAD_FILE_EXISTE) {
            UploadFileInfo *uploaadfileinfo = m_fileList.takeAt(0); // takeAt函数会取出对应的元素

            // 1. FileDataProgress(fdp)对象从vLayout中移除
            QVBoxLayout *vLayout = UploadLayout::getInstance()->getUploadLayout();
            vLayout->removeWidget(uploaadfileinfo->fdp);

            // 2.
            delete uploaadfileinfo->fdp;
            // 3. 删除uploaafileinfo对象
            delete uploaadfileinfo;
        }
    }
    /*
    UploadFileInfo *uploadFileInfo = m_fileList.takeAt(0);
    // 删除掉拿出来的堆区对象
    if (uploadFileInfo != nullptr){
        delete uploadFileInfo;
    }
    */
}

int UploadTask::appendUploadTask(QString filePath){

    //form-data; user=\"%1\" filename=\"%2\" md5=\"%3\" size=%4
    UploadFileInfo *uploadFile = new UploadFileInfo;

    qint64 filesize = QFileInfo(filePath).size();
    // 判断文件的大小
    if (filesize > 30 *1024*1024) {
        qDebug() << "文件太大，最大允许上传30M";
        return -1;
    }

    // 获取到filename
    int pos = filePath.lastIndexOf("/", -1) + 1;
    uploadFile->filePath = filePath;
    uploadFile->fileName = filePath.mid(pos);
    uploadFile->size = filesize;
    uploadFile->md5 = Common::getInstance()->getFileMd5(filePath);
    uploadFile->uploadStatus = UPLOAD_NOT;

    // 上传进度条显示, 将进度条UI显示vLayout上
    FileDataProgress *fdp = new FileDataProgress();
    fdp->setFileName(uploadFile->fileName);
    uploadFile->fdp = fdp;

    QVBoxLayout* vlayout = UploadLayout::getInstance()->getUploadLayout();
    // 这里完成添加展示进度条的操作，这里vlayout是一个uploadScroll的布局，将fdp添加到这个布局中
    // 就完成了进度条的展示
    vlayout->insertWidget(0, fdp);

    // FileDataProgress *f1 = new FileDataProgress();
    // f1->setFileName(QString("文件%1").arg());

    // 第一个参数为0，表示每一次添加到最上面, index=0在最上面

    // 将这个文件添加到m_fileList中
    m_fileList.append(uploadFile);
    return 0;
}
