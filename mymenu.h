#ifndef MYMENU_H
#define MYMENU_H

#include <QMenu>
#include <QObject>

class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QWidget *parent = nullptr);

signals:

};

#endif // MYMENU_H
