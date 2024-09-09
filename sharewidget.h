#ifndef SHAREWIDGET_H
#define SHAREWIDGET_H

#include <QWidget>
#include <QNetworkReply>
#include <QMessageBox>
#include <QString>
#include <QNetworkAccessManager>
#include <QListView>
#include <QAction>
#include <QTimer>
#include <QList>
#include "mymenu.h"
#include "common.h"
#include "filepropertyinfodialog.h"
#include "logininfoinstance.h"
#include <QFileDialog>
#include "fileinfo.h"
#include "downloadtask.h"

namespace Ui {
class ShareWidget;
}

class ShareWidget : public QWidget
{
    Q_OBJECT

private slots:
    void onRightMenu(const QPoint &pos);

signals:
    void gotoTransform(TransformStatus Download);
    void sigLoginAgain();
public:
    explicit ShareWidget(QWidget *parent = nullptr);
    ~ShareWidget();

    void getShareFilesCount();

private:
    /**
     * @brief 初始化共享界面
     */
    void initListWidget();

    /**
     * @brief 添加必要菜单栏
     */
    void addMenu();

    /**
     * @brief 关联菜单信号和槽
     */
    void menuActions();

    void addDownloadFiles();

    void getShareFilesList();


    void clearItems();
    void showFileItems();
    void clearFileList();
    void addListWidgetItem(FileInfo *fileInfo);

    void dealfile(QString cmd);
    void cancelShareFile(FileInfo *fileinfo);
    void checkTaskList(); // 添加下载任务

    void downloadFilesActions();
    void dealFilePV(QString md5, QString fileName);
    void showFileProperty(FileInfo *fileInfo);
    void saveFileToMyAccount(FileInfo *fileInfo);
private:
    Ui::ShareWidget *ui;
    QNetworkAccessManager *m_manager;
    Common *m_common;
    LoginInfoInstance *m_loginInfo;

    MyMenu *m_menuItem;
    MyMenu *m_menuEmpty;

    QAction *m_actionShareDownload; // 下载文件
    QAction	*m_actionShareProperty; // 文件信息
    QAction *m_actionShareCancel;   // 取消分享
    QAction *m_actionShareSave;     // 转存文件

    QAction *m_actionShareRefresh;  // 刷新

    QList<FileInfo*> m_fileList;

    DownloadTask *m_downloadTask;
    int m_shareFilesCount;

    // 定时器对象
    QTimer m_downloadFileTimer;


};

#endif // SHAREWIDGET_H
