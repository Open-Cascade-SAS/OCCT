// Created on: 1993-01-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomTools.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_UndefinedTypeHandler.hxx>

static Handle(GeomTools_UndefinedTypeHandler) theActiveHandler = new GeomTools_UndefinedTypeHandler;

void  GeomTools::Dump(const Handle(Geom_Surface)& S, Standard_OStream& OS)
{
  GeomTools_SurfaceSet::PrintSurface(S,OS);
}

void  GeomTools::Write(const Handle(Geom_Surface)& S, Standard_OStream& OS)
{
  GeomTools_SurfaceSet::PrintSurface(S,OS,Standard_True);
}

void GeomTools::Read(Handle(Geom_Surface)& S, Standard_IStream& IS)
{
  S = GeomTools_SurfaceSet::ReadSurface(IS);
}

void  GeomTools::Dump(const Handle(Geom_Curve)& C, Standard_OStream& OS)
{
  GeomTools_CurveSet::PrintCurve(C,OS);
}

void  GeomTools::Write(const Handle(Geom_Curve)& C, Standard_OStream& OS)
{
  GeomTools_CurveSet::PrintCurve(C,OS,Standard_True);
}

void GeomTools::Read(Handle(Geom_Curve)& C, Standard_IStream& IS)
{
  C = GeomTools_CurveSet::ReadCurve(IS);
}

void  GeomTools::Dump(const Handle(Geom2d_Curve)& C, Standard_OStream& OS)
{
  GeomTools_Curve2dSet::PrintCurve2d(C,OS);
}

void  GeomTools::Write(const Handle(Geom2d_Curve)& C, Standard_OStream& OS)
{
  GeomTools_Curve2dSet::PrintCurve2d(C,OS,Standard_True);
}

void  GeomTools::Read(Handle(Geom2d_Curve)& C, Standard_IStream& IS)
{
  C = GeomTools_Curve2dSet::ReadCurve2d(IS);
}

//=======================================================================
//function : SetUndefinedTypeHandler
//purpose  : 
//=======================================================================

void GeomTools::SetUndefinedTypeHandler(const Handle(GeomTools_UndefinedTypeHandler)& aHandler)
{
  if(!aHandler.IsNull())
    theActiveHandler = aHandler;
}

//=======================================================================
//function : GetUndefinedTypeHandler
//purpose  : 
//=======================================================================

Handle(GeomTools_UndefinedTypeHandler) GeomTools::GetUndefinedTypeHandler()
{
  return theActiveHandler;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

void GeomTools::GetReal(Standard_IStream& IS,Standard_Real& theValue)
{
  theValue = 0.;
  if (IS.eof()) 
    return;

  char buffer[256];
  buffer[0] = '\0';
  std::streamsize anOldWide = IS.width(256);
  IS >> buffer;
  IS.width(anOldWide);
  theValue = Strtod(buffer, NULL);
}
