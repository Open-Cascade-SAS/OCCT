// Created on: 1996-11-21
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified:	Wed Mar  5 09:45:42 1997
//    by:	Joelle CHAUVET
//              G1134 : new methods RealBounds and Constraints
// Modified:	Mon Jun 16 15:22:41 1997
//    by:	Jerome LEMONIER
//              Correction de la methode D2 (faute de frappe dans le code)
//              Correction de la methode D1 (D0 inutile)

#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <GeomPlate_Surface.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XY.hxx>
#include <Plate_Plate.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomPlate_Surface, Geom_Surface)

//=================================================================================================

GeomPlate_Surface::GeomPlate_Surface(const Handle(Geom_Surface)& Surfinit,
                                     const Plate_Plate&          Surfinter)
    : mySurfinter(Surfinter),
      mySurfinit(Surfinit),
      myUmin(0.0),
      myUmax(0.0),
      myVmin(0.0),
      myVmax(0.0)
{
}

//=================================================================================================

void GeomPlate_Surface::UReverse()
{
  // throw Standard_Failure("UReverse");
}

//=================================================================================================

Standard_Real GeomPlate_Surface::UReversedParameter(const Standard_Real U) const
{ // throw Standard_Failure("UReversedParameter");
  return (-U);
}

//=================================================================================================

void GeomPlate_Surface::VReverse()
{ // throw Standard_Failure("VReverse");
}

//=================================================================================================

Standard_Real GeomPlate_Surface::VReversedParameter(const Standard_Real V) const
{ // throw Standard_Failure("VReversedParameter");
  return (-V);
}

//=================================================================================================

// void GeomPlate_Surface::TransformParameters(Standard_Real& U, Standard_Real& V, const gp_Trsf& T)
// const
void GeomPlate_Surface::TransformParameters(Standard_Real&, Standard_Real&, const gp_Trsf&) const
{ // throw Standard_Failure("TransformParameters");
}

//=================================================================================================

// gp_GTrsf2d GeomPlate_Surface::ParametricTransformation(const gp_Trsf& T) const
gp_GTrsf2d GeomPlate_Surface::ParametricTransformation(const gp_Trsf&) const
{ // throw Standard_Failure("ParametricTransformation");
  return gp_GTrsf2d();
}

//=================================================================================================

void GeomPlate_Surface::Bounds(Standard_Real& U1,
                               Standard_Real& U2,
                               Standard_Real& V1,
                               Standard_Real& V2) const
{
  if (mySurfinit->DynamicType() == STANDARD_TYPE(GeomPlate_Surface))
    mySurfinit->Bounds(U1, U2, V1, V2);
  else
  {
    U1 = myUmin;
    U2 = myUmax;
    V1 = myVmin;
    V2 = myVmax;
  }
}

//=================================================================================================

Standard_Boolean GeomPlate_Surface::IsUClosed() const
{ // throw Standard_Failure("IsUClosed(");
  // return 1;
  return 0;
}

//=================================================================================================

Standard_Boolean GeomPlate_Surface::IsVClosed() const
{ // throw Standard_Failure("IsVClosed(");
  // return 1;
  return 0;
}

//=================================================================================================

Standard_Boolean GeomPlate_Surface::IsUPeriodic() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Real GeomPlate_Surface::UPeriod() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean GeomPlate_Surface::IsVPeriodic() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Real GeomPlate_Surface::VPeriod() const
{
  return Standard_False;
}

//=================================================================================================

// Handle(Geom_Curve) GeomPlate_Surface::UIso(const Standard_Real U) const
Handle(Geom_Curve) GeomPlate_Surface::UIso(const Standard_Real) const
{ // throw Standard_Failure("UIso");
  return Handle(Geom_Curve)();
}

//=================================================================================================

// Handle(Geom_Curve) GeomPlate_Surface::VIso(const Standard_Real V) const
Handle(Geom_Curve) GeomPlate_Surface::VIso(const Standard_Real) const
{ // throw Standard_Failure("VIso");
  return Handle(Geom_Curve)();
}

//=================================================================================================

GeomAbs_Shape GeomPlate_Surface::Continuity() const
{ // throw Standard_Failure("Continuity()");
  return GeomAbs_Shape();
}

//=================================================================================================

// Standard_Boolean GeomPlate_Surface::IsCNu(const Standard_Integer N) const
Standard_Boolean GeomPlate_Surface::IsCNu(const Standard_Integer) const
{
  throw Standard_Failure("IsCNu");
}

//=================================================================================================

// Standard_Boolean GeomPlate_Surface::IsCNv(const Standard_Integer N) const
Standard_Boolean GeomPlate_Surface::IsCNv(const Standard_Integer) const
{
  throw Standard_Failure("IsCNv");
}

//=================================================================================================

void GeomPlate_Surface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const
{
  gp_XY  P1(U, V);
  gp_Pnt P2;
  mySurfinit->D0(U, V, P2);
  gp_XYZ P3; //=mySurfinter.Evaluate(P1);
  P3 = mySurfinter.Evaluate(P1);
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    P.SetCoord(i, P3.Coord(i) + P2.Coord(i));
  }
}

//=================================================================================================

void GeomPlate_Surface::D1(const Standard_Real U,
                           const Standard_Real V,
                           gp_Pnt&             P,
                           gp_Vec&             D1U,
                           gp_Vec&             D1V) const
{
  gp_XY  P1(U, V);
  gp_Pnt P2;
  D0(U, V, P);
  gp_Vec V1U, V1V;
  mySurfinit->D1(U, V, P2, V1U, V1V);
  gp_XYZ V2U = mySurfinter.EvaluateDerivative(P1, 1, 0);
  gp_XYZ V2V = mySurfinter.EvaluateDerivative(P1, 0, 1);
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    D1U.SetCoord(i, V1U.Coord(i) + V2U.Coord(i));
    D1V.SetCoord(i, V1V.Coord(i) + V2V.Coord(i));
  }
}

//=================================================================================================

void GeomPlate_Surface::D2(const Standard_Real U,
                           const Standard_Real V,
                           gp_Pnt&             P,
                           gp_Vec&             D1U,
                           gp_Vec&             D1V,
                           gp_Vec&             D2U,
                           gp_Vec&             D2V,
                           gp_Vec&             D2UV) const
{
  gp_XY  P1(U, V);
  gp_Pnt P2;

  gp_Vec V1U, V1V, V1UV, vv, v;
  D1(U, V, P, D1U, D1V);
  mySurfinit->D2(U, V, P2, vv, v, V1U, V1V, V1UV);
  gp_XYZ V2U  = mySurfinter.EvaluateDerivative(P1, 2, 0);
  gp_XYZ V2V  = mySurfinter.EvaluateDerivative(P1, 0, 2);
  gp_XYZ V2UV = mySurfinter.EvaluateDerivative(P1, 1, 1);
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    D2U.SetCoord(i, V1U.Coord(i) + V2U.Coord(i));
    D2V.SetCoord(i, V1V.Coord(i) + V2V.Coord(i));
    D2UV.SetCoord(i, V1UV.Coord(i) + V2UV.Coord(i));
  }
}

//=================================================================================================

// void GeomPlate_Surface::D3(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV,
// gp_Vec& D3UVV) const
void GeomPlate_Surface::D3(const Standard_Real,
                           const Standard_Real,
                           gp_Pnt&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&) const
{
  throw Standard_Failure("D3");
}

//=================================================================================================

// gp_Vec GeomPlate_Surface::DN(const Standard_Real U, const Standard_Real V, const Standard_Integer
// Nu, const Standard_Integer Nv) const
gp_Vec GeomPlate_Surface::DN(const Standard_Real,
                             const Standard_Real,
                             const Standard_Integer,
                             const Standard_Integer) const
{
  throw Standard_Failure("DN");
}

//=================================================================================================

Handle(Geom_Geometry) GeomPlate_Surface::Copy() const
{
  Handle(GeomPlate_Surface) GPS = new GeomPlate_Surface(mySurfinit, mySurfinter);
  return GPS;
}

//=================================================================================================

// void GeomPlate_Surface::Transform(const gp_Trsf& T)
void GeomPlate_Surface::Transform(const gp_Trsf&)
{ // throw Standard_Failure("Transform");
}

//=================================================================================================

Handle(Geom_Surface) GeomPlate_Surface::CallSurfinit() const

{
  return mySurfinit;
}

//=================================================================================================

void GeomPlate_Surface::SetBounds(const Standard_Real Umin,
                                  const Standard_Real Umax,
                                  const Standard_Real Vmin,
                                  const Standard_Real Vmax)
{
  if ((Umin > Umax) || (Vmin > Vmax))
    throw Standard_Failure("Bounds haven't the good sense");
  if ((Umin == Umax) || (Vmin == Vmax))
    throw Standard_Failure("Bounds are equal");
  myUmin = Umin;
  myUmax = Umax;
  myVmin = Vmin;
  myVmax = Vmax;
}

//=================================================================================================

void GeomPlate_Surface::RealBounds(Standard_Real& U1,
                                   Standard_Real& U2,
                                   Standard_Real& V1,
                                   Standard_Real& V2) const
{
  mySurfinter.UVBox(U1, U2, V1, V2);
}

//=================================================================================================

void GeomPlate_Surface::Constraints(TColgp_SequenceOfXY& Seq) const
{
  mySurfinter.UVConstraints(Seq);
}
