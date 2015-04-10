// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CAspectText_HeaderFile
#define _Graphic3d_CAspectText_HeaderFile

#include <InterfaceGraphic_Visual3d.hxx>
#include <Graphic3d_CTexture.hxx>
#include <Graphic3d_ShaderProgram.hxx>

class Graphic3d_CAspectText
{
public:

  int            IsDef;
  int            IsSet;
  const char*    Font;
  float          Space;
  float          Expan;
  CALL_DEF_COLOR Color;
  int            Style;
  int            DisplayType;
  CALL_DEF_COLOR ColorSubTitle;
  int            TextZoomable;
  float          TextAngle;
  int            TextFontAspect;
  Handle(Graphic3d_ShaderProgram) ShaderProgram;

};

typedef Graphic3d_CAspectText CALL_DEF_CONTEXTTEXT;

#endif // _Graphic3d_CAspectText_HeaderFile
