/// -*- coding: utf-8 -*-
///
/// file: zmQtMain.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "zmQtUi.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
//w.show();

    return a.exec();
}
