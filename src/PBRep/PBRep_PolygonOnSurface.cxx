// Copyright (c) 1998-1999 Matra Datavision
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

#include <PBRep_PolygonOnSurface.ixx>


//=======================================================================
//function : PBRep_PolygonOnSurface
//purpose  : 
//=======================================================================

PBRep_PolygonOnSurface::PBRep_PolygonOnSurface
(const Handle(PPoly_Polygon2D)& aPPol2d,
 const Handle(PGeom_Surface)&   aPSurf,
 const PTopLoc_Location&        aPLoc) :
 PBRep_CurveRepresentation(aPLoc),
 myPolygon2D(aPPol2d),
 mySurface(aPSurf)
{
}


//=======================================================================
//function : PBRep_PolygonOnSurface::IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnSurface::IsPolygonOnSurface() const 
{
  return Standard_True;
}


//=======================================================================
//function : PBRep_PolygonOnSurface::Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface) PBRep_PolygonOnSurface::Surface() const 
{
  return mySurface;
}

//=======================================================================
//function : PBRep_PolygonOnSurface::Polygon
//purpose  : 
//=======================================================================

Handle(PPoly_Polygon2D) PBRep_PolygonOnSurface::Polygon() const 
{
  return myPolygon2D;
}
