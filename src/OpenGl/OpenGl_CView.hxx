// File:      OpenGl_CView.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_CView_Header
#define OpenGl_CView_Header

#include <InterfaceGraphic_telem.hxx>

#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>

struct OpenGl_CView
{
  Handle_OpenGl_Workspace WS;
  Handle_OpenGl_View      View;
  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_CView_Header
