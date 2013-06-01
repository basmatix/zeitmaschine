/// -*- coding: utf-8 -*-
///
/// file: main.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <QtGui/QApplication>
#include "zmQtUi.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    zmQtUi w;
#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
