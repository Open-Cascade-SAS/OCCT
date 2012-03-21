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

#include <PBRep_PolygonOnClosedSurface.ixx>


//=======================================================================
//function : PBRep_PolygonOnClosedSurface
//purpose  : 
//=======================================================================

PBRep_PolygonOnClosedSurface::PBRep_PolygonOnClosedSurface
(const Handle(PPoly_Polygon2D)& aPPol2d1,
 const Handle(PPoly_Polygon2D)& aPPol2d2,
 const Handle(PGeom_Surface)&   aPSurf,
 const PTopLoc_Location&        aPLoc) :
 PBRep_PolygonOnSurface( aPPol2d1, aPSurf, aPLoc),
 myPolygon2(aPPol2d2)
{
}


//=======================================================================
//function : PBRep_PolygonOnClosedSurface::IsPolygonOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnClosedSurface::IsPolygonOnClosedSurface() const 
{
  return Standard_True;
}

//=======================================================================
//function : PBRep_PolygonOnSurface::Polygon2
//purpose  : 
//=======================================================================

Handle(PPoly_Polygon2D) PBRep_PolygonOnClosedSurface::Polygon2() const 
{
  return myPolygon2;
}
