#include "mymenu.h"

MyMenu::MyMenu(QWidget *parent) :QMenu(parent)
{
    // 设置样式
    this->setStyleSheet(
                "font: 75 14pt \"霞鹜文楷\";"
                "background-color: rgb(188, 193, 200);");
}
