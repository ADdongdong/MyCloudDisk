#ifndef FILEPROPERTYINFODIALOG_H
#define FILEPROPERTYINFODIALOG_H

#include <QDialog>
#include "networkdata.h"
#include <QString>

namespace Ui {
class FilePropertyInfoDialog;
}

class FilePropertyInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertyInfoDialog(QWidget *parent = nullptr);
    ~FilePropertyInfoDialog();
    void setFileInfo(FileInfo *fileInfo);



private:
    Ui::FilePropertyInfoDialog *ui;
};

#endif // FILEPROPERTYINFODIALOG_H
