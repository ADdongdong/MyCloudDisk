#ifndef COMMON_H
#define COMMON_H
#include <QTime>
#include <QString>
#include <QtDebug>
#include <QObject>
#include "des.h"
#include <QNetworkAccessManager>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QFile>
#include <QRandomGenerator>
#include <QDateTime>
#include <QCoreApplication>
#include <QEventLoop>

// 定义正则表达式，来约束输入的姓名，密码，电话，邮箱等内容
#define USER_REG     "^[a-zA-Z\\d_@#\\-\\*]{3,16}$"
#define PASSWD_REG   "^[a-zA-Z\\d_@#\\-\\*]{6,18}$"
#define PHONE_REG    "1\\d{10}"
#define EMAIL_REG  	 "^[a-zA-Z\\d._-]+@[a-zA-Z\\d._-]+\\.[a-zA-Z\\d._-]+$"
#define IP_REG       "((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)"
#define PORT_REG     "^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)"

// 配置文件路径
#define CONF_FILE  "conf/cfg.json"
// 文件图标路径
#define FILE_TYPE_DIR "conf/fileType"
// 文件上传下载记录的路径
#define RECORD_DIR    "conf/record"

#define CONF_REMEMBER_YES "yes"
#define CONF_REMEMBER_NO "no"

// 传输页面的三个子页面
enum TransformStatus{
    Upload, // 上传页面
    Download, // 下载页面
    Record // 传输记录页面
};

// 单例模式
class Common:public QObject
{
private:
    Common();

    static Common* m_instance;
    QNetworkAccessManager *m_manager;
    QStringList m_fileTypeList;
    QRandomGenerator m_random;

public:
    static Common* getInstance();

    /**
     * @brief 读取配置文件
     * @param title
     * @param key
     * @param path
     * @return
     */
    QString getConfValue(QString title, QString key, QString path=CONF_FILE);
    /**
     * @brief 将登录信息（用户，密码）写入数据到json文件
     * @param user 用户名
     * @param pwd 密码
     * @param isRemeber 是否记住用户名和密码
     * @param path 保存路径
     */
    void writeLoginInfo(QString user, QString pwd, bool isRemeber, QString path=CONF_FILE);

    /**
     * @brief 将服务器ip和端口信息写入到配置文件中
     * @param ip ip地址
     * @param port 端口号
     * @param path 配置文件路径
     */
    void writeWebInfo(QString ip, QString port, QString path=CONF_FILE);

    /**
     * @brief md5加密字符串
     * @return
     */
    QString getStrMd5(QString str);

    /**
     * @brief 获取某个文件的md5值
     * @param[in] filePath文件的路径
     * @return 返回文件的md5值
     */
    QString getFileMd5(QString filePath);
    // 读取json文件数据

    /**
     * @brief 获取QNetworkAccessManger对象
     * @return
     */
    QNetworkAccessManager* getQNetworkAccessManager();

    /**
     * @brief 定义函数获取文件类型列表/conf/fileType
     */
    void getFileTypeList();

    /**
     * @brief 从m_fileTypeList中找是否有fileTypeName,
     * 		  如果有，则返回fileTypeName,否则返回other.png
     * @param fileTypeName
     */
    QString getFileType(QString fileTypeName);


    /**
     * @brief 获取分隔符
     * @return
     */
    QString getBoundary();

    /**
     * @brief 传输数据记录到本地文件
     * @param user 用户操作
     * @param fileName 操作的文件
     * @param code 操作码
     * @param path 文件保存的路径
     */
    void writeRecord(QString user, QString fileName, QString code, QString path = RECORD_DIR);

    /**
     * @brief 返回操作名称
     * @param code
     * @return
     */
    QString getActionString(QString code);

    void sleep(unsigned int msc);
};

#endif // COMMON_H
