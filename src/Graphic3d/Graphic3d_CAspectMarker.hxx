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

#ifndef _Graphic3d_CAspectMarker_HeaderFile
#define _Graphic3d_CAspectMarker_HeaderFile

#include <Aspect_TypeOfMarker.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <Graphic3d_CTexture.hxx>
#include <Graphic3d_MarkerImage.hxx>
#include <Graphic3d_ShaderProgram.hxx>

class Graphic3d_CAspectMarker
{
public:

  Graphic3d_CAspectMarker()
  : IsDef (0),
    IsSet (0),
    MarkerType (Aspect_TOM_POINT),
    Scale (0),
    MarkerImage (NULL)
  {
    memset (&Color, 0, sizeof(Color));
  }

public:

  Standard_Integer                IsDef;
  Standard_Integer                IsSet;
  CALL_DEF_COLOR                  Color;
  Aspect_TypeOfMarker             MarkerType;
  Standard_ShortReal              Scale;
  Handle(Graphic3d_MarkerImage)   MarkerImage;
  Handle(Graphic3d_ShaderProgram) ShaderProgram;

};

typedef Graphic3d_CAspectMarker CALL_DEF_CONTEXTMARKER;

#endif // _Graphic3d_AspectMarker_HeaderFile
