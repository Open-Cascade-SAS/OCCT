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


// modif du 14/09/95 mjm
// prise en compte de l'unite choisi par l'utilisateur
// pour l'ecriture du fichier IGES.

#include <GeomToIGES_GeomPoint.ixx>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

#include <Geom_Point.hxx>

#include <IGESGeom_Point.hxx>

#include <Interface_Macros.hxx>


//=============================================================================
// GeomToIGES_GeomPoint
//=============================================================================

GeomToIGES_GeomPoint::GeomToIGES_GeomPoint
(const GeomToIGES_GeomEntity& GE)
:GeomToIGES_GeomEntity(GE)
{
}


//=============================================================================
// GeomToIGES_GeomPoint
//=============================================================================

GeomToIGES_GeomPoint::GeomToIGES_GeomPoint()

:GeomToIGES_GeomEntity()
{
}


//=============================================================================
// Transfer de Point de Geom vers IGES
// TranferPoint
//=============================================================================

Handle(IGESGeom_Point) GeomToIGES_GeomPoint::TransferPoint
( const Handle(Geom_Point)& P)
{
  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y,Z;
  P->Coord (X,Y,Z);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()), voidsubdef);
  return Piges;
}

//=============================================================================
// Transfer de Point de Geom vers IGES
// TranferPoint
//=============================================================================

Handle(IGESGeom_Point) GeomToIGES_GeomPoint::TransferPoint
( const Handle(Geom_CartesianPoint)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y,Z;
  P->Coord (X,Y,Z);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()), voidsubdef);
  return Piges;
}
