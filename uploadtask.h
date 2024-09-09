#ifndef UPLOADTASK_H
#define UPLOADTASK_H

/**
 * @brief 上传任务列表类，单例模式
 *        这个对象不需要创建多个，只需要一个就可以，所以使用单例
 */
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include "common.h"
#include "filedataprogress.h"
#include "uploadlayout.h"

#define UPLOAD_NOT         0 // 还未上传
#define UPLODING 		   1
#define UPLOAD_FINISHED    2
#define UPLOAD_FAILD       3
#define UPLOAD_FILE_EXISTE 4 // 上传文件已经存在


struct UploadFileInfo{
    QString md5;
    QString fileName;
    QString filePath; // 文件路径
    qint64 size;    // 文件大小
    FileDataProgress *fdp; // 进度条
    int uploadStatus;   // 0:未上传 1：正在上传 2：上传完成
};


class UploadTask
{
public:
    // 获取UploadTask唯一实例对象
    static UploadTask* getInstance();

    /**
     * @brief 添加文件到上传任务列表中
     * @param filePath
     * @return -1 表示文件大于30m，报错
     */
    int appendUploadTask(QString filePath);


    /**
     * @brief 判断上传任务队列是否为空
     */
    bool isEmpyt();

    /**
     * @brief 取出任务
     * @return
     */
    UploadFileInfo* takeTask();

    /**
     * @brief 删除添加到下载任务队列中的任务，删除最上面的人物就行了，因为每次取的是最上面的任务
     * @return
     */
    void deleteUploadTask();
private:
    UploadTask();
    ~UploadTask();

    // 静态数据成员，类中声明，类外定义
    static UploadTask* m_instance;

private:
    QList<UploadFileInfo*> m_fileList; // 上传的文件
};

#endif // UPLOADTASK_H
