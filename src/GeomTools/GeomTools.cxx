// Created on: 1993-01-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GeomTools.ixx>

#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_Curve2dSet.hxx>
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
  GeomTools_SurfaceSet::ReadSurface(IS,S);
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
  GeomTools_CurveSet::ReadCurve(IS,C);
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
  GeomTools_Curve2dSet::ReadCurve2d(IS,C);
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
