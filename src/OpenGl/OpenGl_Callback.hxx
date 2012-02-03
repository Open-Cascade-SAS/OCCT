// File:      OpenGl_Callback.hxx
// Created:   25 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_Callback_HeaderFile
#define _OpenGl_Callback_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_Element.hxx>

typedef OpenGl_Element * (*OpenGl_UserDrawCallback)(const CALL_DEF_USERDRAW *);

Standard_EXPORT OpenGl_UserDrawCallback & UserDrawCallback ();

#endif //_OpenGl_Callback_HeaderFile
