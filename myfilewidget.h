#ifndef MYFILEWIDGET_H
#define MYFILEWIDGET_H

#include <QWidget>
#include <QListView>
#include "mymenu.h"
#include <QAction>
#include <QNetworkAccessManager>
#include "common.h"
#include "logininfoinstance.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include "networkdata.h"
#include <QStringList>
#include <QDir>
#include <QPainter>
#include <QFileInfoList>
#include "filepropertyinfodialog.h"
#include <QFileDialog>
#include <QByteArray>
#include "uploadtask.h"
#include <QTimer>
#include "downloadtask.h"

namespace Ui {
class MyFileWidget;
}

class MyFileWidget : public QWidget
{
    Q_OBJECT

public:
    enum MyFileDisplay {Normal, Asc, Desc};
    explicit MyFileWidget(QWidget *parent = nullptr);
    ~MyFileWidget();

    /**
     * @brief 获取用户文件数量
     */
    void getMyFileCount(MyFileDisplay cmd=Normal);
private:
    /**
     * @brief 初始化listWidget
     */
    void initListWidget();

    /**
     * @brief 右键展示菜单，这个函数用来添加菜单
     *        菜单，通过重写QMenu类来实现
     */
    void addMenu();

    /**
     * @brief 关联菜单的信号与槽
     */
    void meanActions();


    void getMyFileList(MyFileDisplay cmd=Normal);

    /**
     * @brief 展示myfile页面的文件图标
     */
    void showFileItems();

    /**
     * @brief 添加上传按钮
     */
    void addUploadItem();

    /**
     * @brief 清楚myfiles页面所有图标, 这里只是针对ui页面item的删除
     */
    void clearItems();

    /**
     * @brief 清楚m_fileList数据中的信息
     */
    void clearFileList();

    /**
     * @brief 处理文件：删除，分享
     * @param[in] cmd
     */
    void dealFile(QString cmd);

    /**
     * @brief 分享文件
     * @param fileInfo 分享文件的信息对象
     */
    void getShareFile(FileInfo *fileInfo);
    /**
     * @brief 删除文件
     * @param fileInfo 删除文件的信息对象
     */
    void deleteFile(FileInfo *fileInfo);

    /**
     * @brief 展示文件的信息
     * @param fileInfo
     */
    void showFileProperty(FileInfo *fileInfo);

    /**
     * @brief 上传文件
     */
    void upLoadFile(UploadFileInfo *uploadFileInfo);

    /**
     * @brief 情况用户文件item
     */
    void refreshFileItems();

    void addListWidgetItem(FileInfo *fileInfo);

    /**
     * @brief 添加文件到上传任务列表
     */
    void addUploadFiles();

    /**
     * @brief 定时器
     */
    void checkTaskList();

    void uploadFileAction();
    void downloadFilesAction();

    /**
     * @brief 添加文件到下载任务列表
     */
    void addDownloadFiles();
signals:
    // 重新登录
    void sigLoginAgain();

    // 切换到传输transFrom页面
    void gotoTransform(TransformStatus status);
private slots:
    /**
     * @brief 定义槽函数，相应右键菜单
     */
    void onRightMenu(const QPoint &pos);
private:
    Ui::MyFileWidget *ui;
    MyMenu *m_menuItem;
    MyMenu *m_menuEmpty;


    QTimer m_uploadFileTimer;
    QTimer m_downloadFileTimer;

    QAction *m_actionDownload;
    QAction *m_actionShare;
    QAction *m_actionDel;
    QAction *m_actionProperty;
    QAction *m_actionDownloadAsc;
    QAction *m_actionDownloadDesc;
    QAction *m_actionRefresh;
    QAction *m_actionUpload;

    Common *m_common;
    UploadTask *m_uploadTask;
    DownloadTask *m_downloadTask;
    LoginInfoInstance *m_loginInfo;

    bool m_hasUploadItem = false;

    // 定义用户文件数量
    int m_myFilesCount;
    // 获取的文件的信息
    QList<FileInfo*> m_fileList;
    // 处理网络通信
    QNetworkAccessManager *m_manager;

    QStringList m_fileTypeList;
};

#endif // MYFILEWIDGET_H
