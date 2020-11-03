// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef DrawTrSurf_Params_HeaderFile
#define DrawTrSurf_Params_HeaderFile

#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>

//! DrawTrSurf parameters.
struct DrawTrSurf_Params
{
public:
  Draw_Color       PntColor;
  Draw_Color       CurvColor;
  Draw_Color       BoundsColor;
  Draw_Color       IsosColor;
  Draw_Color       PolesColor;
  Draw_Color       KnotsColor;

  Draw_MarkerShape PntMarker;
  Draw_MarkerShape KnotsMarker;
  Standard_Boolean IsShowPoles;
  Standard_Boolean IsShowKnots;
  Standard_Boolean NeedKnotsIsos;
  Standard_Real    Deflection;
  Standard_Integer KnotsSize;
  Standard_Integer Discret;
  Standard_Integer DrawMode;
  Standard_Integer NbUIsos;
  Standard_Integer NbVIsos;

  DrawTrSurf_Params()
  : PntColor   (Draw_rouge),
    CurvColor  (Draw_jaune),
    BoundsColor(Draw_vert),
    IsosColor  (Draw_bleu),
    PolesColor (Draw_rouge),
    KnotsColor (Draw_violet),
    PntMarker  (Draw_Plus),
    KnotsMarker(Draw_Losange),
    IsShowPoles (true),
    IsShowKnots (true),
    NeedKnotsIsos (true),
    Deflection (0.01),
    KnotsSize (5),
    Discret (30),
    DrawMode(0),
    NbUIsos (10),
    NbVIsos (10)
  {}

};

#endif
