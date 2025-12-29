// Created on: 1993-03-24
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

#include <Geom2d_Transformation.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Transformation, Standard_Transient)

typedef Geom2d_Transformation Transformation;
typedef gp_Ax2d               Ax2d;
typedef gp_Pnt2d              Pnt2d;
typedef gp_TrsfForm           TrsfForm;
typedef gp_Vec2d              Vec2d;

occ::handle<Geom2d_Transformation> Geom2d_Transformation::Copy() const
{

  occ::handle<Geom2d_Transformation> T;
  T = new Transformation(gpTrsf2d);
  return T;
}

Geom2d_Transformation::Geom2d_Transformation() = default;

Geom2d_Transformation::Geom2d_Transformation(const gp_Trsf2d& T)
    : gpTrsf2d(T)
{
}

occ::handle<Geom2d_Transformation> Geom2d_Transformation::Inverted() const
{

  return new Transformation(gpTrsf2d.Inverted());
}

occ::handle<Geom2d_Transformation> Geom2d_Transformation::Multiplied(

  const occ::handle<Geom2d_Transformation>& Other) const
{

  return new Transformation(gpTrsf2d.Multiplied(Other->Trsf2d()));
}

occ::handle<Geom2d_Transformation> Geom2d_Transformation::Powered(const int N) const
{

  gp_Trsf2d Temp = gpTrsf2d;
  Temp.Power(N);
  return new Transformation(Temp);
}

void Geom2d_Transformation::SetMirror(const gp_Pnt2d& P)
{

  gpTrsf2d.SetMirror(P);
}

void Geom2d_Transformation::SetMirror(const gp_Ax2d& A)
{

  gpTrsf2d.SetMirror(A);
}

void Geom2d_Transformation::SetRotation(const gp_Pnt2d& P, const double Ang)
{

  gpTrsf2d.SetRotation(P, Ang);
}

void Geom2d_Transformation::SetScale(const gp_Pnt2d& P, const double S)
{

  gpTrsf2d.SetScale(P, S);
}

void Geom2d_Transformation::SetTransformation(const gp_Ax2d& ToAxis)
{

  gpTrsf2d.SetTransformation(ToAxis);
}

void Geom2d_Transformation::SetTransformation(const gp_Ax2d& FromAxis1, const gp_Ax2d& ToAxis2)
{

  gpTrsf2d.SetTransformation(FromAxis1, ToAxis2);
}

void Geom2d_Transformation::SetTranslation(const gp_Vec2d& V)
{

  gpTrsf2d.SetTranslation(V);
}

void Geom2d_Transformation::SetTranslation(const gp_Pnt2d& P1, const gp_Pnt2d& P2)
{

  gpTrsf2d.SetTranslation(P1, P2);
}

void Geom2d_Transformation::SetTrsf2d(const gp_Trsf2d& T)
{
  gpTrsf2d = T;
}

bool Geom2d_Transformation::IsNegative() const
{

  return gpTrsf2d.IsNegative();
}

TrsfForm Geom2d_Transformation::Form() const
{
  return gpTrsf2d.Form();
}

double Geom2d_Transformation::ScaleFactor() const
{

  return gpTrsf2d.ScaleFactor();
}

gp_Trsf2d Geom2d_Transformation::Trsf2d() const
{
  return gpTrsf2d;
}

double Geom2d_Transformation::Value(const int Row, const int Col) const
{

  return gpTrsf2d.Value(Row, Col);
}

void Geom2d_Transformation::Invert()
{
  gpTrsf2d.Invert();
}

void Geom2d_Transformation::Transforms(double& X, double& Y) const
{

  gpTrsf2d.Transforms(X, Y);
}

void Geom2d_Transformation::Multiply(const occ::handle<Geom2d_Transformation>& Other)
{

  gpTrsf2d.Multiply(Other->Trsf2d());
}

void Geom2d_Transformation::Power(const int N)
{
  gpTrsf2d.Power(N);
}

void Geom2d_Transformation::PreMultiply(const occ::handle<Geom2d_Transformation>& Other)
{

  gpTrsf2d.PreMultiply(Other->Trsf2d());
}
