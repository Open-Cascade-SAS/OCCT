// Copyright: 	Matra-Datavision 1995
// File:	Geom2dToIGES_Geom2dVector.cxx
// Created:	Wed Feb  1 15:58:37 1995
// Author:	Marie Jose MARTZ
//		<mjm>

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
