// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>

#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Array1OfBox.hxx>
#include <Bnd_Box.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_OutOfRange.hxx>

#include "IntCurveSurface_PolyhedronUtils.pxx"

// Namespace alias for brevity
namespace PolyUtils = IntCurveSurface_PolyhedronUtils;

//==================================================================================================

IntCurveSurface_ThePolyhedronOfHInter::IntCurveSurface_ThePolyhedronOfHInter(
  const Handle(Adaptor3d_Surface)& Surface,
  const Standard_Integer           nbdU,
  const Standard_Integer           nbdV,
  const Standard_Real              u1,
  const Standard_Real              v1,
  const Standard_Real              u2,
  const Standard_Real              v2)
    : nbdeltaU((nbdU < 3) ? 3 : nbdU),
      nbdeltaV((nbdV < 3) ? 3 : nbdV),
      TheDeflection(Epsilon(100.)),
      C_MyPnts(NULL),
      C_MyU(NULL),
      C_MyV(NULL),
      C_MyIsOnBounds(NULL)
{
  PolyUtils::AllocateArrays(nbdeltaU, nbdeltaV, C_MyPnts, C_MyU, C_MyV, C_MyIsOnBounds);
  Init(Surface, u1, v1, u2, v2);
}

//==================================================================================================

IntCurveSurface_ThePolyhedronOfHInter::IntCurveSurface_ThePolyhedronOfHInter(
  const Handle(Adaptor3d_Surface)& Surface,
  const TColStd_Array1OfReal&      Upars,
  const TColStd_Array1OfReal&      Vpars)
    : nbdeltaU(Upars.Length() - 1),
      nbdeltaV(Vpars.Length() - 1),
      TheDeflection(Epsilon(100.)),
      C_MyPnts(NULL),
      C_MyU(NULL),
      C_MyV(NULL),
      C_MyIsOnBounds(NULL)
{
  PolyUtils::AllocateArrays(nbdeltaU, nbdeltaV, C_MyPnts, C_MyU, C_MyV, C_MyIsOnBounds);
  Init(Surface, Upars, Vpars);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Destroy()
{
  PolyUtils::Destroy(C_MyPnts, C_MyU, C_MyV, C_MyIsOnBounds);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Init(const Handle(Adaptor3d_Surface)& Surface,
                                                 const Standard_Real              U0,
                                                 const Standard_Real              V0,
                                                 const Standard_Real              U1,
                                                 const Standard_Real              V1)
{
  PolyUtils::InitUniform<Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool>(
    Surface, U0, V0, U1, V1, nbdeltaU, nbdeltaV,
    static_cast<gp_Pnt*>(C_MyPnts), static_cast<Standard_Real*>(C_MyU),
    static_cast<Standard_Real*>(C_MyV), static_cast<Standard_Boolean*>(C_MyIsOnBounds), TheBnd);

  Standard_Real tol = PolyUtils::ComputeMaxDeflection<
    Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool, IntCurveSurface_ThePolyhedronOfHInter>(
      Surface, *this, NbTriangles());
  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  TheBorderDeflection = PolyUtils::ComputeMaxBorderDeflection<
    Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool>(
      Surface, U0, V0, U1, V1, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Init(const Handle(Adaptor3d_Surface)& Surface,
                                                 const TColStd_Array1OfReal&      Upars,
                                                 const TColStd_Array1OfReal&      Vpars)
{
  PolyUtils::InitWithParams<Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool>(
    Surface, Upars, Vpars, nbdeltaU, nbdeltaV,
    static_cast<gp_Pnt*>(C_MyPnts), static_cast<Standard_Real*>(C_MyU),
    static_cast<Standard_Real*>(C_MyV), static_cast<Standard_Boolean*>(C_MyIsOnBounds), TheBnd);

  Standard_Real tol = PolyUtils::ComputeMaxDeflection<
    Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool, IntCurveSurface_ThePolyhedronOfHInter>(
      Surface, *this, NbTriangles());
  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  TheBorderDeflection = PolyUtils::ComputeMaxBorderDeflection<
    Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool>(
      Surface, Upars(Upars.Lower()), Vpars(Vpars.Lower()),
      Upars(Upars.Upper()), Vpars(Vpars.Upper()), nbdeltaU, nbdeltaV);
}

//==================================================================================================

Standard_Real IntCurveSurface_ThePolyhedronOfHInter::DeflectionOnTriangle(
  const Handle(Adaptor3d_Surface)& Surface,
  const Standard_Integer           Triang) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  Standard_Real u1, v1, u2, v2, u3, v3;
  gp_Pnt        P1 = Point(i1, u1, v1), P2 = Point(i2, u2, v2), P3 = Point(i3, u3, v3);
  return PolyUtils::DeflectionOnTriangle<Handle(Adaptor3d_Surface), Adaptor3d_HSurfaceTool>(
    Surface, P1, P2, P3, u1, v1, u2, v2, u3, v3);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Parameters(const Standard_Integer Index,
                                                       Standard_Real&         U,
                                                       Standard_Real&         V) const
{
  PolyUtils::Parameters(Index, C_MyU, C_MyV, U, V);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::DeflectionOverEstimation(const Standard_Real flec)
{
  PolyUtils::SetDeflectionOverEstimation(flec, TheDeflection, TheBnd);
}

//==================================================================================================

Standard_Real IntCurveSurface_ThePolyhedronOfHInter::DeflectionOverEstimation() const
{
  return TheDeflection;
}

//==================================================================================================

const Bnd_Box& IntCurveSurface_ThePolyhedronOfHInter::Bounding() const
{
  return TheBnd;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::FillBounding()
{
  TheComponentsBnd = new Bnd_HArray1OfBox(1, NbTriangles());
  PolyUtils::FillBounding(static_cast<gp_Pnt*>(C_MyPnts), nbdeltaU, nbdeltaV, TheDeflection, TheComponentsBnd);
}

//==================================================================================================

const Handle(Bnd_HArray1OfBox)& IntCurveSurface_ThePolyhedronOfHInter::ComponentsBounding() const
{
  return TheComponentsBnd;
}

//==================================================================================================

Standard_Integer IntCurveSurface_ThePolyhedronOfHInter::NbTriangles() const
{
  return PolyUtils::NbTriangles(nbdeltaU, nbdeltaV);
}

//==================================================================================================

Standard_Integer IntCurveSurface_ThePolyhedronOfHInter::NbPoints() const
{
  return PolyUtils::NbPoints(nbdeltaU, nbdeltaV);
}

//==================================================================================================

Standard_Integer IntCurveSurface_ThePolyhedronOfHInter::TriConnex(const Standard_Integer Triang,
                                                                  const Standard_Integer Pivot,
                                                                  const Standard_Integer Pedge,
                                                                  Standard_Integer&      TriCon,
                                                                  Standard_Integer& OtherP) const
{
  return PolyUtils::TriConnex(Triang, Pivot, Pedge, TriCon, OtherP, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::PlaneEquation(const Standard_Integer Triang,
                                                          gp_XYZ&                NormalVector,
                                                          Standard_Real&         PolarDistance) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  PolyUtils::PlaneEquation(Point(i1), Point(i2), Point(i3), NormalVector, PolarDistance);
}

//==================================================================================================

Standard_Boolean IntCurveSurface_ThePolyhedronOfHInter::Contain(const Standard_Integer Triang,
                                                                const gp_Pnt&          ThePnt) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  return PolyUtils::Contain(Point(i1), Point(i2), Point(i3), ThePnt);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Dump() const {}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Size(Standard_Integer& nbdu,
                                                 Standard_Integer& nbdv) const
{
  nbdu = nbdeltaU;
  nbdv = nbdeltaV;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Triangle(const Standard_Integer Index,
                                                     Standard_Integer&      P1,
                                                     Standard_Integer&      P2,
                                                     Standard_Integer&      P3) const
{
  PolyUtils::Triangle(Index, P1, P2, P3, nbdeltaV);
}

//==================================================================================================

const gp_Pnt& IntCurveSurface_ThePolyhedronOfHInter::Point(const Standard_Integer Index,
                                                           Standard_Real&         U,
                                                           Standard_Real&         V) const
{
  return PolyUtils::Point(Index, C_MyPnts, C_MyU, C_MyV, U, V);
}

//==================================================================================================

const gp_Pnt& IntCurveSurface_ThePolyhedronOfHInter::Point(const Standard_Integer Index) const
{
  return PolyUtils::Point(Index, C_MyPnts);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Point(const gp_Pnt&,
                                                  const Standard_Integer,
                                                  const Standard_Integer,
                                                  const Standard_Real,
                                                  const Standard_Real)
{
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Point(const Standard_Integer Index, gp_Pnt& P) const
{
  PolyUtils::Point(Index, C_MyPnts, P);
}

//==================================================================================================

Standard_Boolean IntCurveSurface_ThePolyhedronOfHInter::IsOnBound(
  const Standard_Integer Index1,
  const Standard_Integer Index2) const
{
  return PolyUtils::IsOnBound(
    Index1, Index2, static_cast<Standard_Boolean*>(C_MyIsOnBounds), nbdeltaU, nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::UMinSingularity(const Standard_Boolean Sing)
{
  UMinSingular = Sing;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::UMaxSingularity(const Standard_Boolean Sing)
{
  UMaxSingular = Sing;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::VMinSingularity(const Standard_Boolean Sing)
{
  VMinSingular = Sing;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::VMaxSingularity(const Standard_Boolean Sing)
{
  VMaxSingular = Sing;
}

//==================================================================================================

Standard_Boolean IntCurveSurface_ThePolyhedronOfHInter::HasUMinSingularity() const
{
  return UMinSingular;
}

//==================================================================================================

Standard_Boolean IntCurveSurface_ThePolyhedronOfHInter::HasUMaxSingularity() const
{
  return UMaxSingular;
}

//==================================================================================================

Standard_Boolean IntCurveSurface_ThePolyhedronOfHInter::HasVMinSingularity() const
{
  return VMinSingular;
}

//==================================================================================================

Standard_Boolean IntCurveSurface_ThePolyhedronOfHInter::HasVMaxSingularity() const
{
  return VMaxSingular;
}
