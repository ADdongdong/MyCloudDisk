#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H
/**
 * @brief 下载任务列表类，单例模式
 *        这个对象不需要创建多个，只需要一个就可以，所以使用单例
 */
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include "common.h"
#include "filedataprogress.h"
#include "fileinfo.h"
#include <QFile>
#include "downloadlayout.h"
#include <QFile>
// #include <QUrl>

struct DownloadFileInfo{
    QFile *file;      // 文件
    QString user;     // 下载用户
    QString md5;
    QString fileName;
    QString filePath; // 文件路径
    qint64 size;    // 文件大小
    QString url;
    FileDataProgress *fdp; //下载进度条
    int uploadStatus;   // 0:未上传 1：正在上传 2：上传完成
    bool isShareTask; // 标记当前下载文件的信息是不是从共享文件下载的
};

class DownloadTask
{
public:
    // 获取DownloadTask唯一实例对象
    static DownloadTask* getInstance();

    /**
     * @brief 添加文件到下载任务列表中
     * @param filePath
     * @param[in] isShareTask 标记是不是下载分享的文件
     * @return -1 表示文件大于30m，报错
     */
    int appendDownloadTask(FileInfo* fileInfo, QString filePath, bool isShareTask = false);

    DownloadFileInfo* takeTask();

    /**
     * @brief 判断上传任务队列是否为空
     */
    bool isEmpyt();

    /**
     * @brief 删除添加到下载任务队列中的任务，删除最上面的人物就行了，因为每次取的是最上面的任务
     * @return
     */
    void deleteDownloadTask();
private:
    DownloadTask();
    ~DownloadTask();

    // 静态数据成员，类中声明，类外定义
    static DownloadTask* m_instance;

private:
    QList<DownloadFileInfo*> m_fileList; // 上传的文件
};

#endif // DownloadTask

