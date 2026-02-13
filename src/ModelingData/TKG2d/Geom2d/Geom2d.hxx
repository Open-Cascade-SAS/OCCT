// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _Geom2d_HeaderFile
#define _Geom2d_HeaderFile

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//! Result structure for 2D curve D1 evaluation (point and first derivative).
struct Geom2d_CurveD1
{
  gp_Pnt2d Point;
  gp_Vec2d D1;
};

//! Result structure for 2D curve D2 evaluation (point and first two derivatives).
struct Geom2d_CurveD2
{
  gp_Pnt2d Point;
  gp_Vec2d D1;
  gp_Vec2d D2;
};

//! Result structure for 2D curve D3 evaluation (point and first three derivatives).
struct Geom2d_CurveD3
{
  gp_Pnt2d Point;
  gp_Vec2d D1;
  gp_Vec2d D2;
  gp_Vec2d D3;
};

#endif // _Geom2d_HeaderFile
