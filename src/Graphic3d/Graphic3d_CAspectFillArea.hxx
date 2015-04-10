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

#ifndef _Graphic3d_CAspectFillArea_HeaderFile
#define _Graphic3d_CAspectFillArea_HeaderFile

#include <InterfaceGraphic_Visual3d.hxx>
#include <Graphic3d_CTexture.hxx>
#include <Graphic3d_ShaderProgram.hxx>

class Graphic3d_CAspectFillArea
{

public:

  Graphic3d_CAspectFillArea()
  : IsDef (0),
    IsSet (0),
    Style (0),
    LineType (0),
    Width (0.0f),
    Hatch (0),
    Distinguish (0),
    BackFace (0),
    Edge (0),
    PolygonOffsetMode   (0),
    PolygonOffsetFactor (0.0f),
    PolygonOffsetUnits  (0.0f)
  {
    memset (&IntColor,     0, sizeof(IntColor));
    memset (&BackIntColor, 0, sizeof(BackIntColor));
    memset (&EdgeColor,    0, sizeof(EdgeColor));
    memset (&Front,        0, sizeof(Front));
    memset (&Back,         0, sizeof(Back));
  }

public:

  int IsDef;
  int IsSet;
  int Style;

  CALL_DEF_COLOR IntColor;
  CALL_DEF_COLOR BackIntColor;
  CALL_DEF_COLOR EdgeColor;

  int LineType;

  float Width;

  int Hatch;

  int Distinguish;
  int BackFace;

  int Edge;

  CALL_DEF_MATERIAL Front;
  CALL_DEF_MATERIAL Back;

  Graphic3d_CTexture Texture;

  int   PolygonOffsetMode;
  float PolygonOffsetFactor;
  float PolygonOffsetUnits;

  Handle(Graphic3d_ShaderProgram) ShaderProgram;

};

typedef Graphic3d_CAspectFillArea CALL_DEF_CONTEXTFILLAREA;

#endif // _Graphic3d_CAspectFillArea_HeaderFile
