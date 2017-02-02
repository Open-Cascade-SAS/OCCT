// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <DrawTrSurf.hxx>
#include <GeomTools.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Geom_Geometry.hxx>
#include <Geom2d_Curve.hxx>

// This file defines global functions not declared in any public header,
// intended for use from debugger prompt (Command Window in Visual Studio)

//! Save geometric object identified by pointer to handle
const char* DrawTrSurf_Set (const char* theNameStr, void* theHandlePtr)
{
  if (theNameStr == 0 || theHandlePtr == 0)
  {
    return "Error: argument is null";
  }
  try {
    const Handle(Standard_Transient)& aHandle = *(Handle(Standard_Transient)*)theHandlePtr;
    Handle(Geom_Geometry) aGeom3d = Handle(Geom_Geometry)::DownCast(aHandle);
    if (!aGeom3d.IsNull())
    {
      DrawTrSurf::Set (theNameStr, aGeom3d);
      return theNameStr;
    }
    Handle(Geom2d_Curve) aGeom2d = Handle(Geom2d_Curve)::DownCast(aHandle);
    if (!aGeom2d.IsNull())
    {
      DrawTrSurf::Set (theNameStr, aGeom2d);
      return theNameStr;
    }

    return "Error: Not a geometric object";
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//! Set point to DRAW variable
const char* DrawTrSurf_SetPnt (const char* theNameStr, void* thePntPtr)
{
  if (theNameStr == 0 || thePntPtr == 0)
  {
    return "Error: argument is null";
  }
  try {
    const gp_Pnt& aP = *(gp_Pnt*)thePntPtr;
    static char buff[256];
    sprintf (buff, "Point (%.16g, %.16g, %.16g) set to DRAW variable %.80s", aP.X(), aP.Y(), aP.Z(), theNameStr);
    DrawTrSurf::Set (theNameStr, aP);
    return buff;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//! Set 2d point to DRAW variable
const char* DrawTrSurf_SetPnt2d (const char* theNameStr, void* thePnt2dPtr)
{
  if (theNameStr == 0 || thePnt2dPtr == 0)
  {
    return "Error: argument is null";
  }
  try {
    const gp_Pnt2d& aP = *(gp_Pnt2d*)thePnt2dPtr;
    static char buff[256];
    sprintf (buff, "Point (%.16g, %.16g) set to DRAW variable %.80s", aP.X(), aP.Y(), theNameStr);
    DrawTrSurf::Set (theNameStr, aP);
    return buff;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

// MSVC debugger cannot deal correctly with functions whose argunments 
// have non-standard types. Here we define alternative to the above functions
// with good types with the hope that GDB on Linux or other debugger could
// work with them (DBX could, on SUN Solaris).
#ifndef _MSC_VER

const char* DrawTrSurf_Set (const char* name, const Handle(Standard_Transient)& G)
{
  return DrawTrSurf_Set (name, (void*)&G);
}

const char* DrawTrSurf_Set (const char* theName, const gp_Pnt& thePnt)
{
  return DrawTrSurf_SetPnt (theName, (void*)&thePnt);
}

const char* DrawTrSurf_Set (const char* theName, const gp_Pnt2d& thePnt2d)
{
  return DrawTrSurf_SetPnt2d (theName, (void*)&thePnt2d);
}

#endif /* _MSC_VER */

// old function, looks too dangerous to be used
/*
void DrawTrSurf_Get(const char* name, Handle(Standard_Transient)& G)
{
  Handle(Geom_Geometry) GG = DrawTrSurf::Get(name);
  cout << "Nom : " << name << endl;
  if (!GG.IsNull()) {
    G = GG;
    return;
  }

  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(name);
  if (!GC.IsNull()) {
    G = GC;
    return;
  }

  cout << "*** Not a geometric object ***" << endl;
}
*/
