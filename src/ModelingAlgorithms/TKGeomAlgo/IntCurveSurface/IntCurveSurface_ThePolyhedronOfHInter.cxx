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

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <GeomGridEval_Surface.hxx>
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
  const occ::handle<Adaptor3d_Surface>& Surface,
  const int                             nbdU,
  const int                             nbdV,
  const double                          u1,
  const double                          v1,
  const double                          u2,
  const double                          v2)
    : nbdeltaU((nbdU < 3) ? 3 : nbdU),
      nbdeltaV((nbdV < 3) ? 3 : nbdV),
      TheDeflection(Epsilon(100.)),
      C_MyPnts(nullptr),
      C_MyU(nullptr),
      C_MyV(nullptr),
      C_MyIsOnBounds(nullptr)
{
  PolyUtils::AllocateArrays(nbdeltaU, nbdeltaV, C_MyPnts, C_MyU, C_MyV, C_MyIsOnBounds);
  Init(Surface, u1, v1, u2, v2);
}

//==================================================================================================

IntCurveSurface_ThePolyhedronOfHInter::IntCurveSurface_ThePolyhedronOfHInter(
  const occ::handle<Adaptor3d_Surface>& Surface,
  const NCollection_Array1<double>&     Upars,
  const NCollection_Array1<double>&     Vpars)
    : nbdeltaU(Upars.Length() - 1),
      nbdeltaV(Vpars.Length() - 1),
      TheDeflection(Epsilon(100.)),
      C_MyPnts(nullptr),
      C_MyU(nullptr),
      C_MyV(nullptr),
      C_MyIsOnBounds(nullptr)
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

void IntCurveSurface_ThePolyhedronOfHInter::Init(const occ::handle<Adaptor3d_Surface>& Surface,
                                                 const double                          U0,
                                                 const double                          V0,
                                                 const double                          U1,
                                                 const double                          V1)
{
  // Initialize grid evaluator with surface
  GeomGridEval_Surface anEval;
  anEval.Initialize(*Surface);

  PolyUtils::InitUniform(anEval,
                         U0,
                         V0,
                         U1,
                         V1,
                         nbdeltaU,
                         nbdeltaV,
                         static_cast<gp_Pnt*>(C_MyPnts),
                         static_cast<double*>(C_MyU),
                         static_cast<double*>(C_MyV),
                         static_cast<bool*>(C_MyIsOnBounds),
                         TheBnd);

  double tol = PolyUtils::ComputeMaxDeflection(*Surface, *this, NbTriangles());
  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  TheBorderDeflection =
    PolyUtils::ComputeMaxBorderDeflection(anEval, U0, V0, U1, V1, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Init(const occ::handle<Adaptor3d_Surface>& Surface,
                                                 const NCollection_Array1<double>&     Upars,
                                                 const NCollection_Array1<double>&     Vpars)
{
  // Initialize grid evaluator with surface
  GeomGridEval_Surface anEval;
  anEval.Initialize(*Surface);

  PolyUtils::InitWithParams(anEval,
                            Upars,
                            Vpars,
                            nbdeltaU,
                            nbdeltaV,
                            static_cast<gp_Pnt*>(C_MyPnts),
                            static_cast<double*>(C_MyU),
                            static_cast<double*>(C_MyV),
                            static_cast<bool*>(C_MyIsOnBounds),
                            TheBnd);

  double tol = PolyUtils::ComputeMaxDeflection(*Surface, *this, NbTriangles());
  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  TheBorderDeflection = PolyUtils::ComputeMaxBorderDeflection(anEval,
                                                              Upars(Upars.Lower()),
                                                              Vpars(Vpars.Lower()),
                                                              Upars(Upars.Upper()),
                                                              Vpars(Vpars.Upper()),
                                                              nbdeltaU,
                                                              nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Parameters(const int Index, double& U, double& V) const
{
  PolyUtils::Parameters(Index, C_MyU, C_MyV, U, V);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::DeflectionOverEstimation(const double flec)
{
  PolyUtils::SetDeflectionOverEstimation(flec, TheDeflection, TheBnd);
}

//==================================================================================================

double IntCurveSurface_ThePolyhedronOfHInter::DeflectionOverEstimation() const
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
  TheComponentsBnd = new NCollection_HArray1<Bnd_Box>(1, NbTriangles());
  PolyUtils::FillBounding(static_cast<gp_Pnt*>(C_MyPnts),
                          nbdeltaU,
                          nbdeltaV,
                          TheDeflection,
                          TheComponentsBnd);
}

//==================================================================================================

const occ::handle<NCollection_HArray1<Bnd_Box>>& IntCurveSurface_ThePolyhedronOfHInter::
  ComponentsBounding() const
{
  return TheComponentsBnd;
}

//==================================================================================================

int IntCurveSurface_ThePolyhedronOfHInter::NbTriangles() const
{
  return PolyUtils::NbTriangles(nbdeltaU, nbdeltaV);
}

//==================================================================================================

int IntCurveSurface_ThePolyhedronOfHInter::NbPoints() const
{
  return PolyUtils::NbPoints(nbdeltaU, nbdeltaV);
}

//==================================================================================================

int IntCurveSurface_ThePolyhedronOfHInter::TriConnex(const int Triang,
                                                     const int Pivot,
                                                     const int Pedge,
                                                     int&      TriCon,
                                                     int&      OtherP) const
{
  return PolyUtils::TriConnex(Triang, Pivot, Pedge, TriCon, OtherP, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::PlaneEquation(const int Triang,
                                                          gp_XYZ&   NormalVector,
                                                          double&   PolarDistance) const
{
  int i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  PolyUtils::PlaneEquation(Point(i1), Point(i2), Point(i3), NormalVector, PolarDistance);
}

//==================================================================================================

bool IntCurveSurface_ThePolyhedronOfHInter::Contain(const int Triang, const gp_Pnt& ThePnt) const
{
  int i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  return PolyUtils::Contain(Point(i1), Point(i2), Point(i3), ThePnt);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Dump() const {}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Size(int& nbdu, int& nbdv) const
{
  nbdu = nbdeltaU;
  nbdv = nbdeltaV;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Triangle(const int Index,
                                                     int&      P1,
                                                     int&      P2,
                                                     int&      P3) const
{
  PolyUtils::Triangle(Index, P1, P2, P3, nbdeltaV);
}

//==================================================================================================

const gp_Pnt& IntCurveSurface_ThePolyhedronOfHInter::Point(const int Index,
                                                           double&   U,
                                                           double&   V) const
{
  return PolyUtils::Point(Index, C_MyPnts, C_MyU, C_MyV, U, V);
}

//==================================================================================================

const gp_Pnt& IntCurveSurface_ThePolyhedronOfHInter::Point(const int Index) const
{
  return PolyUtils::Point(Index, C_MyPnts);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Point(const gp_Pnt&,
                                                  const int,
                                                  const int,
                                                  const double,
                                                  const double)
{
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::Point(const int Index, gp_Pnt& P) const
{
  PolyUtils::Point(Index, C_MyPnts, P);
}

//==================================================================================================

bool IntCurveSurface_ThePolyhedronOfHInter::IsOnBound(const int Index1, const int Index2) const
{
  return PolyUtils::IsOnBound(Index1,
                              Index2,
                              static_cast<bool*>(C_MyIsOnBounds),
                              nbdeltaU,
                              nbdeltaV);
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::UMinSingularity(const bool Sing)
{
  UMinSingular = Sing;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::UMaxSingularity(const bool Sing)
{
  UMaxSingular = Sing;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::VMinSingularity(const bool Sing)
{
  VMinSingular = Sing;
}

//==================================================================================================

void IntCurveSurface_ThePolyhedronOfHInter::VMaxSingularity(const bool Sing)
{
  VMaxSingular = Sing;
}

//==================================================================================================

bool IntCurveSurface_ThePolyhedronOfHInter::HasUMinSingularity() const
{
  return UMinSingular;
}

//==================================================================================================

bool IntCurveSurface_ThePolyhedronOfHInter::HasUMaxSingularity() const
{
  return UMaxSingular;
}

//==================================================================================================

bool IntCurveSurface_ThePolyhedronOfHInter::HasVMinSingularity() const
{
  return VMinSingular;
}

//==================================================================================================

bool IntCurveSurface_ThePolyhedronOfHInter::HasVMaxSingularity() const
{
  return VMaxSingular;
}
