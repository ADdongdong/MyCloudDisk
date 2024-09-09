#include "logininfoinstance.h"

LoginInfoInstance* LoginInfoInstance::m_instance = new LoginInfoInstance;

LoginInfoInstance::LoginInfoInstance()
{

}

void LoginInfoInstance::setUser(QString user){
    _user = user;
}
void LoginInfoInstance::setToken(QString token){
    _token = token;
}
void LoginInfoInstance::setIp(QString Ip){
    _ip = Ip;
}
void LoginInfoInstance::setPort(QString port){
    _port = port;
}

QString LoginInfoInstance::user() {
    return _user;
}
QString LoginInfoInstance::token(){
    return _token;
}
QString LoginInfoInstance::ip() {
    return _ip;
}
QString LoginInfoInstance::port() {
    return _port;
}

LoginInfoInstance* LoginInfoInstance::getInstance(){
    return m_instance;
}
