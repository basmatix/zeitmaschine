/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <string>
#include "zmQtGtdModel.h"

int main(int argc, char *argv[])
{
    zmQtGtdModel        m_model;

    if( QDir( QDir::currentPath() + QDir::separator() + "zeitmaschine").exists() )
    {
        m_model.setLocalFolder( QDir::currentPath() + QDir::separator() + "zeitmaschine" );
    }
    else
    {
        m_model.setLocalFolder( QDir::homePath() + QDir::separator() + "zeitmaschine" );
    }

    m_model.initialize();
}
