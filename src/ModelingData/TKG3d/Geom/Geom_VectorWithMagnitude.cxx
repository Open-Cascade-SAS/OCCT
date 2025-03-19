// Created on: 1993-03-10
// Created by: JCV
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

#include <Geom_Geometry.hxx>
#include <Geom_Vector.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_VectorWithMagnitude, Geom_Vector)

typedef Geom_VectorWithMagnitude VectorWithMagnitude;
typedef Geom_Vector              Vector;
typedef gp_Ax1                   Ax1;
typedef gp_Ax2                   Ax2;
typedef gp_Pnt                   Pnt;
typedef gp_Trsf                  Trsf;

//=================================================================================================

Geom_VectorWithMagnitude::Geom_VectorWithMagnitude(const gp_Vec& V)
{
  gpVec = V;
}

//=================================================================================================

Geom_VectorWithMagnitude::Geom_VectorWithMagnitude(const Standard_Real X,
                                                   const Standard_Real Y,
                                                   const Standard_Real Z)
{
  gpVec = gp_Vec(X, Y, Z);
}

//=================================================================================================

Geom_VectorWithMagnitude::Geom_VectorWithMagnitude(const Pnt& P1, const Pnt& P2)
{
  gpVec = gp_Vec(P1, P2);
}

//=================================================================================================

Handle(Geom_Geometry) Geom_VectorWithMagnitude::Copy() const
{

  Handle(Geom_VectorWithMagnitude) V;
  V = new VectorWithMagnitude(gpVec);
  return V;
}

//=================================================================================================

void Geom_VectorWithMagnitude::SetCoord(const Standard_Real X,
                                        const Standard_Real Y,
                                        const Standard_Real Z)
{
  gpVec = gp_Vec(X, Y, Z);
}

//=================================================================================================

void Geom_VectorWithMagnitude::SetVec(const gp_Vec& V)
{
  gpVec = V;
}

//=================================================================================================

void Geom_VectorWithMagnitude::SetX(const Standard_Real X)
{
  gpVec.SetX(X);
}

//=================================================================================================

void Geom_VectorWithMagnitude::SetY(const Standard_Real Y)
{
  gpVec.SetY(Y);
}

//=================================================================================================

void Geom_VectorWithMagnitude::SetZ(const Standard_Real Z)
{
  gpVec.SetZ(Z);
}

//=================================================================================================

Standard_Real Geom_VectorWithMagnitude::Magnitude() const
{
  return gpVec.Magnitude();
}

//=================================================================================================

Standard_Real Geom_VectorWithMagnitude::SquareMagnitude() const
{

  return gpVec.SquareMagnitude();
}

//=================================================================================================

void Geom_VectorWithMagnitude::Add(const Handle(Geom_Vector)& Other)
{

  gpVec.Add(Other->Vec());
}

//=================================================================================================

Handle(Geom_VectorWithMagnitude) Geom_VectorWithMagnitude::Added(
  const Handle(Geom_Vector)& Other) const
{

  gp_Vec V1 = gpVec;
  V1.Add(Other->Vec());
  return new VectorWithMagnitude(V1);
}

//=================================================================================================

void Geom_VectorWithMagnitude::Cross(const Handle(Geom_Vector)& Other)
{

  gpVec.Cross(Other->Vec());
}

//=================================================================================================

Handle(Geom_Vector) Geom_VectorWithMagnitude::Crossed(const Handle(Geom_Vector)& Other) const
{

  gp_Vec V(gpVec);
  V.Cross(Other->Vec());
  return new VectorWithMagnitude(V);
}

//=================================================================================================

void Geom_VectorWithMagnitude::CrossCross(const Handle(Geom_Vector)& V1,
                                          const Handle(Geom_Vector)& V2)
{

  gpVec.CrossCross(V1->Vec(), V2->Vec());
}

//=================================================================================================

Handle(Geom_Vector) Geom_VectorWithMagnitude::CrossCrossed(const Handle(Geom_Vector)& V1,
                                                           const Handle(Geom_Vector)& V2) const
{

  gp_Vec V(gpVec);
  V.CrossCross(V1->Vec(), V2->Vec());
  return new VectorWithMagnitude(V);
}

//=================================================================================================

void Geom_VectorWithMagnitude::Divide(const Standard_Real Scalar)
{

  gpVec.Divide(Scalar);
}

//=================================================================================================

Handle(Geom_VectorWithMagnitude) Geom_VectorWithMagnitude::Divided(const Standard_Real Scalar) const
{

  gp_Vec V(gpVec);
  V.Divide(Scalar);
  return new VectorWithMagnitude(V);
}

//=================================================================================================

Handle(Geom_VectorWithMagnitude) Geom_VectorWithMagnitude::Multiplied(
  const Standard_Real Scalar) const
{

  gp_Vec V(gpVec);
  V.Multiply(Scalar);
  return new VectorWithMagnitude(V);
}

//=================================================================================================

void Geom_VectorWithMagnitude::Multiply(const Standard_Real Scalar)
{

  gpVec.Multiply(Scalar);
}

//=================================================================================================

void Geom_VectorWithMagnitude::Normalize()
{
  gpVec.Normalize();
}

//=================================================================================================

Handle(Geom_VectorWithMagnitude) Geom_VectorWithMagnitude::Normalized() const
{

  gp_Vec V(gpVec);
  V.Normalize();
  return new VectorWithMagnitude(V);
}

//=================================================================================================

void Geom_VectorWithMagnitude::Subtract(const Handle(Geom_Vector)& Other)
{

  gpVec.Subtract(Other->Vec());
}

//=================================================================================================

Handle(Geom_VectorWithMagnitude) Geom_VectorWithMagnitude::Subtracted(
  const Handle(Geom_Vector)& Other) const
{

  gp_Vec V(gpVec);
  V.Subtract(Other->Vec());
  return new VectorWithMagnitude(V);
}

//=================================================================================================

void Geom_VectorWithMagnitude::Transform(const Trsf& T)
{

  gpVec.Transform(T);
}
