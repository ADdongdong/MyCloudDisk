#include "networkdata.h"

NetworkData::NetworkData(QObject *parent) : QObject(parent)
{

}

QString NetworkData::getCode(QByteArray json){
    QString code;
    QJsonParseError err;
    // 解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json格式
        QJsonObject rootObj = rootDoc.object();
        QJsonValue codeValue = rootObj.value("code");
        code = codeValue.toString();
    }

    return code;
}

QStringList NetworkData::getFileCount(QByteArray data){
    QStringList list;
    // 解析返回的数据
    QJsonParseError err;
    QJsonDocument rootDoc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError){
        qDebug() << "服务器返回消息Json格式错误";
    } else {
        // 解析json
        /*
         * {
         *  "num":"1",
         *  "code":"110"
         * }
         */
        QJsonObject rootObj = rootDoc.object();
        QJsonValue codeVal = rootObj.value("code");
        if (codeVal.type() == QJsonValue::String) {
            // qDebug()<< "code:" << codeVal.toString();
            QString code = codeVal.toString();
            list.append(code);
        }
        QJsonValue numValue = rootObj.value("num");
        if (numValue.type() == QJsonValue::String) {
            QString num = numValue.toString();
            list.append(num);
        }
    }

    return list;
}

QList<FileInfo*> NetworkData::getFileInfo(QByteArray data){
    QList<FileInfo*> list;
    QJsonParseError err;
    // 解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError){
        qDebug() << "Json格式错误";
    } else {
        // 解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue filesValue = rootObj.value("files");
        /*
        {
        "files": [{
        "user": "milo",
        "md5": "602fdf30db2aacf517badf4565124f51",
        "create_time": "2020-07-03 20:37:18",
        "file_name": "Makefile",
        "share_status": 0,
        "pv": 0,
        "url": "http://172.16.0.15:80/group1/M00/00/00/rBAAD17_Jn6AM-iuAACBBC5AIsc2532509",
        "size": 33028,
        "type": "null"
        }]
        }
        */
        // Json解析数组
        if (filesValue.type() == QJsonValue::Array) {
            QJsonArray filesArr = filesValue.toArray();
            for (int i = 0; i <filesArr.count(); i++) {
                QJsonValue fileValue= filesArr.at(i);
                if (fileValue.type() == QJsonValue::Object) {

                    FileInfo *fileInfo = new FileInfo;
                    QJsonObject fileObject = fileValue.toObject();
                    QJsonValue userValue = fileObject.value("user");
                    // qDebug() << userValue.toString();
                    QJsonValue md5Value = fileObject.value("md5");
                    //qDebug() << md5Value.toString();
                    QJsonValue creatTimeValue = fileObject.value("create_time");
                    //qDebug() << creatTimeValue.toString();
                    QJsonValue fileNameValue = fileObject.value("file_name");
                    // qDebug() << fileNameValue.toString();
                    QJsonValue shareStatusValue = fileObject.value("share_status");
                    // qDebug() << shareStatusValue.toString().toInt();
                    QJsonValue pvValue= fileObject.value("pv");
                    // qDebug() <<pvValue.toString().toInt();
                    QJsonValue urlValue = fileObject.value("url");
                    // qDebug() << urlValue.toString();
                    QJsonValue sizeValue= fileObject.value("size");
                    // qDebug() << sizeValue.toString().toInt();
                    QJsonValue typeValue = fileObject.value("type");
                    // qDebug() << typeValue.toString();

                    fileInfo->user= userValue.toString();
                    fileInfo->md5 = md5Value.toString();
                    fileInfo->createTime=creatTimeValue.toString();
                    fileInfo->fileName = fileNameValue.toString();
                    fileInfo->shareStatus = shareStatusValue.toInt();
                    fileInfo->pv = pvValue.toString().toInt();
                    fileInfo->url = urlValue.toString();
                    fileInfo->size = sizeValue.toInt();
                    fileInfo->type = typeValue.toString();

                    list.append(fileInfo);
                }
            }
        }
    }
    return list;

}


int NetworkData::getCount(QByteArray json){
    int count = 0;
    QJsonParseError err;
    //解析返回的数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        // 解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue codeValue = rootObj.value("count");
        count = codeValue.toString().toInt();
    }

    return count;
}


// 处理json
QList<FileDownload*> NetworkData::getFileDownload(QByteArray json){
    QList<FileDownload*> list;
    QJsonParseError err;
    /*
    {
        "files": [{
            "filename":"ui_buttongroup.h",
            "pv": 0
         }]
    }
    */

    // 返回解析数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        // 解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue filesValue = rootObj.value("files");
        if (filesValue.type() == QJsonValue::Array) {
            QJsonArray filesArr = filesValue.toArray();

            for (int i = 0; i < filesArr.count(); i++) {
                QJsonValue fileValue = filesArr.at(i);
                if (fileValue.type() == QJsonValue::Object) {
                    FileDownload *fileDownload = new FileDownload;

                    QJsonObject fileObj = fileValue.toObject();
                    QJsonValue fileNameValue = fileObj.value("filename");
                    QJsonValue pvValue = fileObj.value("pv");

                    fileDownload->fileName = fileNameValue.toString();
                    fileDownload->pv = pvValue.toInt();

                    list.append(fileDownload);
                }
            }
        }
    }
    return list;

}









