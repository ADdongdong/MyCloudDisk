#ifndef LOGININFOINSTANCE_H
#define LOGININFOINSTANCE_H

#include <QString>


/**
 * @brief 【单例模式实现】
 */
class LoginInfoInstance
{
private:
    LoginInfoInstance();
    static LoginInfoInstance* m_instance;
public:
    static LoginInfoInstance* getInstance();

    void setUser(QString user);
    void setToken(QString token);
    void setIp(QString Ip);
    void setPort(QString port);

    QString user();
    QString token();
    QString ip();
    QString port();
private:
    QString _user;
    QString _token;
    QString _ip;
    QString _port;
};

#endif // LOGININFOINSTANCE_H
