/**
 * @brief 这个文件中封装了处理服务端发来的json回应
 */
#ifndef NETWORKDATA_H
#define NETWORKDATA_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include "fileinfo.h"


class NetworkData : public QObject
{
    Q_OBJECT
public:
    explicit NetworkData(QObject *parent = nullptr);

    static QString getCode(QByteArray json);
    static QStringList getFileCount(QByteArray json);
    static QList<FileInfo*> getFileInfo(QByteArray json);

    static int getCount(QByteArray json);
    static QList<FileDownload*> getFileDownload(QByteArray json);
};

#endif // NETWORKDATA_H
