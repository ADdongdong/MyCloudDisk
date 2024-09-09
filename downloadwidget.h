#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
#include <QNetworkReply>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include "networkdata.h"
#include "common.h"
#include "QNetworkAccessManager"

namespace Ui {
class downloadWidget;
}

class downloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit downloadWidget(QWidget *parent = nullptr);
    ~downloadWidget();

    void refreshFiles();

private:
    void initTableWidget();
    void getShareFilesCount();
    void clearFileList();
    void clearShareFileDownload();
    void getFileDownload();
    void clearTableWidget();
    void refreshTableWidget();
private:
    Ui::downloadWidget *ui;

    Common *m_common;
    QNetworkAccessManager *m_manager;
    int m_shareFilesCount;

    QList<FileDownload*> m_fileDownload;

};

#endif // DOWNLOADWIDGET_H
