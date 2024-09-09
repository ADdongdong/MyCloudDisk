#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include <QWidget>
#include "uploadlayout.h"
#include "downloadlayout.h"
#include "logininfoinstance.h"
#include "common.h"

namespace Ui {
class TransformWidget;
}


class TransformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformWidget(QWidget *parent = nullptr);
    ~TransformWidget();

    void showPage(TransformStatus status);

    void displayDataRecord(QString path=RECORD_DIR);

private slots:
    void on_toolButton_clicked();

private:
    Ui::TransformWidget *ui;
};

#endif // TRANSFORMWIDGET_H
