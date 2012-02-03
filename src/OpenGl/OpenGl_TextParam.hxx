// File:      OpenGl_TextParam.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_TextParam_Header
#define _OpenGl_TextParam_Header

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>

struct OpenGl_TextParam
{
  int                               Height;
  Graphic3d_HorizontalTextAlignment HAlign;
  Graphic3d_VerticalTextAlignment   VAlign;
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //_OpenGl_TextParam_Header
