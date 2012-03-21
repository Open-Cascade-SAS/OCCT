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

#include <GeomToIGES_GeomVector.ixx>

#include <Geom_Vector.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <Geom_Direction.hxx>

#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>

#include <IGESData_IGESEntity.hxx>
#include <IGESData_ToolLocation.hxx>

#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_Point.hxx>

#include <Interface_Macros.hxx>



//=============================================================================
// GeomToIGES_GeomVector
//=============================================================================

GeomToIGES_GeomVector::GeomToIGES_GeomVector()
:GeomToIGES_GeomEntity()
{
}


//=============================================================================
// GeomToIGES_GeomVector
//=============================================================================

GeomToIGES_GeomVector::GeomToIGES_GeomVector
(const GeomToIGES_GeomEntity& GE)
:GeomToIGES_GeomEntity(GE)
{
}


//=============================================================================
// Transfer des Entites Vector de Geom vers IGES
// TransferVector
//=============================================================================

Handle(IGESGeom_Direction) GeomToIGES_GeomVector::TransferVector
(const Handle(Geom_Vector)& start)
{
  Handle(IGESGeom_Direction) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_VectorWithMagnitude))) {
    DeclareAndCast(Geom_VectorWithMagnitude, VMagn, start);
    res = TransferVector(VMagn);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_Direction))) {
    DeclareAndCast(Geom_Direction, Direction, start);
    res = TransferVector(Direction);
  }

  return res;
}
 

//=============================================================================
// Transfer des Entites VectorWithMagnitude de Geom vers IGES
// TransferVector
//=============================================================================

Handle(IGESGeom_Direction) GeomToIGES_GeomVector::TransferVector
(const Handle(Geom_VectorWithMagnitude)& start)
{
  Handle(IGESGeom_Direction) Dir = new IGESGeom_Direction;
  if (start.IsNull()) {
    return Dir;
  }

  Standard_Real X,Y,Z;
  start->Coord(X,Y,Z); 
  Standard_Real M = start->Magnitude();
  Dir->Init(gp_XYZ(X/(M*GetUnit()),Y/(M*GetUnit()),Z/(M*GetUnit())));
  return Dir;
}
 

//=============================================================================
// Transfer des Entites Direction de Geom vers IGES
// TransferVector
//=============================================================================

Handle(IGESGeom_Direction) GeomToIGES_GeomVector::TransferVector
(const Handle(Geom_Direction)& start)
{
  Handle(IGESGeom_Direction) Dir = new IGESGeom_Direction;
  if (start.IsNull()) {
    return Dir;
  }

  Standard_Real X,Y,Z;
  start->Coord(X,Y,Z);
  Dir->Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()));
  return Dir;
}
