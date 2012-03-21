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


#include <Geom2dToIGES_Geom2dVector.ixx>

#include <Geom2d_Vector.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <Geom2d_Direction.hxx>

#include <gp_XYZ.hxx>

#include <IGESGeom_Direction.hxx>

#include <Interface_Macros.hxx>

//=============================================================================
// Geom2dToIGES_Geom2dVector
//=============================================================================

Geom2dToIGES_Geom2dVector::Geom2dToIGES_Geom2dVector()
:Geom2dToIGES_Geom2dEntity()
{
}


//=============================================================================
// Geom2dToIGES_Geom2dVector
//=============================================================================

Geom2dToIGES_Geom2dVector::Geom2dToIGES_Geom2dVector
(const Geom2dToIGES_Geom2dEntity& G2dE)
:Geom2dToIGES_Geom2dEntity(G2dE)
{
}


//=============================================================================
// Transfer des Entites Vector de Geom2d vers IGES
// Transfer2dVector
//=============================================================================

Handle(IGESGeom_Direction) Geom2dToIGES_Geom2dVector::Transfer2dVector
(const Handle(Geom2d_Vector)& start)
{
  Handle(IGESGeom_Direction) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom2d_VectorWithMagnitude))) {
    DeclareAndCast(Geom2d_VectorWithMagnitude, VMagn, start);
    res = Transfer2dVector(VMagn);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom2d_Direction))) {
    DeclareAndCast(Geom2d_Direction, Direction, start);
    res = Transfer2dVector(Direction);
  }

  return res;
}
 

//=============================================================================
// Transfer des Entites VectorWithMagnitude de Geom2d vers IGES
// Transfer2dVector
//=============================================================================

Handle(IGESGeom_Direction) Geom2dToIGES_Geom2dVector::Transfer2dVector
(const Handle(Geom2d_VectorWithMagnitude)& start)
{
  Handle(IGESGeom_Direction) Dir = new IGESGeom_Direction;
  if (start.IsNull()) {
    return Dir;
  }

  Standard_Real X,Y;
  start->Coord(X,Y); 
  Standard_Real M = start->Magnitude();
  Dir->Init(gp_XYZ(X/M, Y/M, 0.));
  return Dir;
}
 

//=============================================================================
// Transfer des Entites Direction de Geom2d vers IGES
// Transfer2dVector
//=============================================================================

Handle(IGESGeom_Direction) Geom2dToIGES_Geom2dVector::Transfer2dVector
(const Handle(Geom2d_Direction)& start)
{
  Handle(IGESGeom_Direction) Dir = new IGESGeom_Direction;
  if (start.IsNull()) {
    return Dir;
  }

  Standard_Real X,Y;
  start->Coord(X,Y);
  Dir->Init(gp_XYZ(X, Y, 0.));
  return Dir;
}
