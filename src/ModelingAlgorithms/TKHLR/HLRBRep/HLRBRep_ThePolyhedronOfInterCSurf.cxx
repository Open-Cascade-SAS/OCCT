// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>

#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <GeomGridEval_Surface.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <HLRBRep_Surface.hxx>
#include <Standard_OutOfRange.hxx>

#include "../../TKGeomAlgo/IntCurveSurface/IntCurveSurface_PolyhedronUtils.pxx"

// Namespace alias for brevity
namespace PolyUtils = IntCurveSurface_PolyhedronUtils;

//==================================================================================================

HLRBRep_ThePolyhedronOfInterCSurf::HLRBRep_ThePolyhedronOfInterCSurf(HLRBRep_Surface* Surface,
                                                                     const int        nbdU,
                                                                     const int        nbdV,
                                                                     const double     u1,
                                                                     const double     v1,
                                                                     const double     u2,
                                                                     const double     v2)
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

HLRBRep_ThePolyhedronOfInterCSurf::HLRBRep_ThePolyhedronOfInterCSurf(
  HLRBRep_Surface*                  Surface,
  const NCollection_Array1<double>& Upars,
  const NCollection_Array1<double>& Vpars)
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

void HLRBRep_ThePolyhedronOfInterCSurf::Destroy()
{
  PolyUtils::Destroy(C_MyPnts, C_MyU, C_MyV, C_MyIsOnBounds);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Init(HLRBRep_Surface* Surface,
                                             const double     U0,
                                             const double     V0,
                                             const double     U1,
                                             const double     V1)
{
  // Initialize grid evaluator with the underlying BRepAdaptor_Surface
  GeomGridEval_Surface anEval;
  anEval.Initialize(Surface->Surface());

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

  double tol = PolyUtils::ComputeMaxDeflection(Surface->Surface(), *this, NbTriangles());
  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  TheBorderDeflection =
    PolyUtils::ComputeMaxBorderDeflection(anEval, U0, V0, U1, V1, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Init(HLRBRep_Surface*                  Surface,
                                             const NCollection_Array1<double>& Upars,
                                             const NCollection_Array1<double>& Vpars)
{
  // Initialize grid evaluator with the underlying BRepAdaptor_Surface
  GeomGridEval_Surface anEval;
  anEval.Initialize(Surface->Surface());

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

  double tol = PolyUtils::ComputeMaxDeflection(Surface->Surface(), *this, NbTriangles());
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

void HLRBRep_ThePolyhedronOfInterCSurf::Parameters(const int Index, double& U, double& V) const
{
  PolyUtils::Parameters(Index, C_MyU, C_MyV, U, V);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOverEstimation(const double flec)
{
  PolyUtils::SetDeflectionOverEstimation(flec, TheDeflection, TheBnd);
}

//==================================================================================================

double HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOverEstimation() const
{
  return TheDeflection;
}

//==================================================================================================

const Bnd_Box& HLRBRep_ThePolyhedronOfInterCSurf::Bounding() const
{
  return TheBnd;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::FillBounding()
{
  TheComponentsBnd = new NCollection_HArray1<Bnd_Box>(1, NbTriangles());
  PolyUtils::FillBounding(static_cast<gp_Pnt*>(C_MyPnts),
                          nbdeltaU,
                          nbdeltaV,
                          TheDeflection,
                          TheComponentsBnd);
}

//==================================================================================================

const occ::handle<NCollection_HArray1<Bnd_Box>>& HLRBRep_ThePolyhedronOfInterCSurf::
  ComponentsBounding() const
{
  return TheComponentsBnd;
}

//==================================================================================================

int HLRBRep_ThePolyhedronOfInterCSurf::NbTriangles() const
{
  return PolyUtils::NbTriangles(nbdeltaU, nbdeltaV);
}

//==================================================================================================

int HLRBRep_ThePolyhedronOfInterCSurf::NbPoints() const
{
  return PolyUtils::NbPoints(nbdeltaU, nbdeltaV);
}

//==================================================================================================

int HLRBRep_ThePolyhedronOfInterCSurf::TriConnex(const int Triang,
                                                 const int Pivot,
                                                 const int Pedge,
                                                 int&      TriCon,
                                                 int&      OtherP) const
{
  return PolyUtils::TriConnex(Triang, Pivot, Pedge, TriCon, OtherP, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::PlaneEquation(const int Triang,
                                                      gp_XYZ&   NormalVector,
                                                      double&   PolarDistance) const
{
  int i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  PolyUtils::PlaneEquation(Point(i1), Point(i2), Point(i3), NormalVector, PolarDistance);
}

//==================================================================================================

bool HLRBRep_ThePolyhedronOfInterCSurf::Contain(const int Triang, const gp_Pnt& ThePnt) const
{
  int i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  return PolyUtils::Contain(Point(i1), Point(i2), Point(i3), ThePnt);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Dump() const {}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Size(int& nbdu, int& nbdv) const
{
  nbdu = nbdeltaU;
  nbdv = nbdeltaV;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Triangle(const int Index, int& P1, int& P2, int& P3) const
{
  PolyUtils::Triangle(Index, P1, P2, P3, nbdeltaV);
}

//==================================================================================================

const gp_Pnt& HLRBRep_ThePolyhedronOfInterCSurf::Point(const int Index, double& U, double& V) const
{
  return PolyUtils::Point(Index, C_MyPnts, C_MyU, C_MyV, U, V);
}

//==================================================================================================

const gp_Pnt& HLRBRep_ThePolyhedronOfInterCSurf::Point(const int Index) const
{
  return PolyUtils::Point(Index, C_MyPnts);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Point(const gp_Pnt&,
                                              const int,
                                              const int,
                                              const double,
                                              const double)
{
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Point(const int Index, gp_Pnt& P) const
{
  PolyUtils::Point(Index, C_MyPnts, P);
}

//==================================================================================================

bool HLRBRep_ThePolyhedronOfInterCSurf::IsOnBound(const int Index1, const int Index2) const
{
  return PolyUtils::IsOnBound(Index1,
                              Index2,
                              static_cast<bool*>(C_MyIsOnBounds),
                              nbdeltaU,
                              nbdeltaV);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::UMinSingularity(const bool Sing)
{
  UMinSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::UMaxSingularity(const bool Sing)
{
  UMaxSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::VMinSingularity(const bool Sing)
{
  VMinSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::VMaxSingularity(const bool Sing)
{
  VMaxSingular = Sing;
}

//==================================================================================================

bool HLRBRep_ThePolyhedronOfInterCSurf::HasUMinSingularity() const
{
  return UMinSingular;
}

//==================================================================================================

bool HLRBRep_ThePolyhedronOfInterCSurf::HasUMaxSingularity() const
{
  return UMaxSingular;
}

//==================================================================================================

bool HLRBRep_ThePolyhedronOfInterCSurf::HasVMinSingularity() const
{
  return VMinSingular;
}

//==================================================================================================

bool HLRBRep_ThePolyhedronOfInterCSurf::HasVMaxSingularity() const
{
  return VMaxSingular;
}
