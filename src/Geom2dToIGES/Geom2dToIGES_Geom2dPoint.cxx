// Created on: 1995-02-01
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Geom2dToIGES_Geom2dPoint.ixx>

#include <gp_XYZ.hxx>

#include <Geom2d_Point.hxx>
#include <Geom2d_CartesianPoint.hxx>

#include <IGESGeom_Point.hxx>


//=============================================================================
// Geom2dToIGES_Geom2dPoint
//=============================================================================

Geom2dToIGES_Geom2dPoint::Geom2dToIGES_Geom2dPoint()
:Geom2dToIGES_Geom2dEntity()
{
}


//=============================================================================
// Geom2dToIGES_Geom2dPoint
//=============================================================================

Geom2dToIGES_Geom2dPoint::Geom2dToIGES_Geom2dPoint
(const Geom2dToIGES_Geom2dEntity& G2dE)
:Geom2dToIGES_Geom2dEntity(G2dE)
{
}


//=============================================================================
// Transfer de Point2d de Geom2d vers IGES
// Tranfer2dPoint
//=============================================================================

Handle(IGESGeom_Point) Geom2dToIGES_Geom2dPoint::Transfer2dPoint( const Handle(Geom2d_Point)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y;
  P->Coord (X,Y);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X,Y,0.), voidsubdef);
  return Piges;
}

//=============================================================================
// Transfer de CartesianPoint de Geom2d vers IGES
// Tranfer2dPoint
//=============================================================================

Handle(IGESGeom_Point) Geom2dToIGES_Geom2dPoint::Transfer2dPoint
( const Handle(Geom2d_CartesianPoint)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y;
  P->Coord (X,Y);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X,Y,0.), voidsubdef);
  return Piges;
}
