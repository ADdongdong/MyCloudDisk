#ifndef FILEDATAPROGRESS_H
#define FILEDATAPROGRESS_H

#include <QWidget>

namespace Ui {
class FileDataProgress;
}

class FileDataProgress : public QWidget
{
    Q_OBJECT

public:
    explicit FileDataProgress(QWidget *parent = nullptr);
    ~FileDataProgress();

    /**
     * @brief 设置当前进度条的名称
     * @param 进度条的名称
     */
    void setFileName(QString fileName);

    /**
     * @brief 设置当前进度
     * @param[in] val 当期的进度
     * @param[in] max 最大值
     */
    void setProgress(int val, int max);

private:
    Ui::FileDataProgress *ui;
};

#endif // FILEDATAPROGRESS_H
