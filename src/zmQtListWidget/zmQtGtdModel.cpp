/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "zmQtGtdModel.h"
#include "zmWidgetItemMap.h"

zmQtGtdModel::zmQtGtdModel()
    : m_gtd_model()
    , m_wi_map( NULL )
{
    m_wi_map = new zmWidgetItemMap();
}

zmQtGtdModel::~zmQtGtdModel()
{
    delete m_wi_map;
}
