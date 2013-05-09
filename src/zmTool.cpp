/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <string>
#include "zmQtGtdModel.h"

int main(int argc, char *argv[])
{
    zmQtGtdModel        m_model;

    if( ! QDir( QDir::currentPath() + QDir::separator() + "zeitmaschine").exists() )
    {
        return -1;
    }
    m_model.setLocalFolder( QDir::currentPath() + QDir::separator() + "zeitmaschine" );

    m_model.setConfigPersistance( false );

    m_model.setUsedUsername("frans");
    m_model.setUsedHostname("x121e");

    if( !m_model.hasUsedUsername() )
    {
        return -1;
    }
    if( !m_model.hasUsedHostname() )
    {
        return -1;
    }

    m_model.initialize();

    m_model.merge( "zm-frans-blackbox-local.yaml" );
}
