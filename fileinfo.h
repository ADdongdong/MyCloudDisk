#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

/*
{
    "files":	[{
            "user":	"lisa",
            "md5":	"47c23eee9ce237d86036c10db0830f34",
            "create_time":	"2024-06-17 16:01:33",
            "file_name":	"猫咪.png",
            "share_status":	0,
            "pv":	0,
            "url":	"http://172.20.203.67:80/group1/M00/00/00/rBTLQ2Zv7V2AUMOUAAAjxg277Jc594.png",
            "size":	9158,
            "type":	"png"
        }]
}*/


struct FileInfo {
   QString user;        // 用户名
   QString md5;			// md5
   QString createTime;  // 上传时间
   QString fileName;    // 文件名称
   QString url;         // 文件url
   QString type;        // 文件类型
   int shareStatus;     // 共享状态是否共享
   int pv;              // 文件下载量，一次加1
   int size;            // 文件大小

};


struct FileDownload{
    QString fileName; // 文件名称
    int pv; // 文件下载量，下载一次加1
};

#endif // FILEINFO_H
