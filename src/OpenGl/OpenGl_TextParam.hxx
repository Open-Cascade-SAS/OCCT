// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_TextParam_Header
