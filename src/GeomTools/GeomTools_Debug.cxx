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

#include <GeomTools.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

// This file defines global functions not declared in any public header,
// intended for use from debugger prompt (Command Window in Visual Studio)

//! Dump content of the geometric object to cout
const char* GeomTools_Dump (void* theHandlePtr)
{
  if (theHandlePtr == 0)
  {
    return "Error: argument is null";
  }
  try {
    OCC_CATCH_SIGNALS
    const Handle(Standard_Transient)& aHandle = *(Handle(Standard_Transient)*)theHandlePtr;

    Handle(Geom_Surface) GS = Handle(Geom_Surface)::DownCast(aHandle);
    if (!GS.IsNull()) {
      cout << "\n\n";
      GeomTools_SurfaceSet::PrintSurface (GS,cout);
      cout << endl;
      return "Found Geom_Surface, see dump in cout";
    }

    Handle(Geom_Curve) GC = Handle(Geom_Curve)::DownCast(aHandle);
    if (!GC.IsNull()) {
      cout << "\n\n";
      GeomTools_CurveSet::PrintCurve(GC,cout);
      cout << endl;
      return "Found Geom_Curve, see dump in cout";
    }

    Handle(Geom2d_Curve) GC2d = Handle(Geom2d_Curve)::DownCast(aHandle);
    if (!GC2d.IsNull()) {
      cout << "\n\n";
      GeomTools_Curve2dSet::PrintCurve2d(GC2d,cout);
      cout << endl;
      return "Found Geom2d_Curve, see dump in cout";
    }

    return "Error: Not a geometric object";
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

const char* GeomTools_Dump (const Handle(Standard_Transient)& theGeom)
{
  return GeomTools_Dump ((void*)&theGeom);
}

#endif
