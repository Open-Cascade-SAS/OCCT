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

#include <Bnd_Array1OfBox.hxx>
#include <Bnd_Box.hxx>
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

HLRBRep_ThePolyhedronOfInterCSurf::HLRBRep_ThePolyhedronOfInterCSurf(HLRBRep_Surface*       Surface,
                                                                     const Standard_Integer nbdU,
                                                                     const Standard_Integer nbdV,
                                                                     const Standard_Real    u1,
                                                                     const Standard_Real    v1,
                                                                     const Standard_Real    u2,
                                                                     const Standard_Real    v2)
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

HLRBRep_ThePolyhedronOfInterCSurf::HLRBRep_ThePolyhedronOfInterCSurf(
  HLRBRep_Surface*            Surface,
  const TColStd_Array1OfReal& Upars,
  const TColStd_Array1OfReal& Vpars)
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

void HLRBRep_ThePolyhedronOfInterCSurf::Destroy()
{
  PolyUtils::Destroy(C_MyPnts, C_MyU, C_MyV, C_MyIsOnBounds);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Init(HLRBRep_Surface*    Surface,
                                             const Standard_Real U0,
                                             const Standard_Real V0,
                                             const Standard_Real U1,
                                             const Standard_Real V1)
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
                         static_cast<Standard_Real*>(C_MyU),
                         static_cast<Standard_Real*>(C_MyV),
                         static_cast<Standard_Boolean*>(C_MyIsOnBounds),
                         TheBnd);

  Standard_Real tol = PolyUtils::ComputeMaxDeflection(Surface, *this, NbTriangles());
  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  TheBorderDeflection =
    PolyUtils::ComputeMaxBorderDeflection(anEval, U0, V0, U1, V1, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Init(HLRBRep_Surface*            Surface,
                                             const TColStd_Array1OfReal& Upars,
                                             const TColStd_Array1OfReal& Vpars)
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
                            static_cast<Standard_Real*>(C_MyU),
                            static_cast<Standard_Real*>(C_MyV),
                            static_cast<Standard_Boolean*>(C_MyIsOnBounds),
                            TheBnd);

  Standard_Real tol = PolyUtils::ComputeMaxDeflection(Surface, *this, NbTriangles());
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

Standard_Real HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOnTriangle(
  HLRBRep_Surface*       Surface,
  const Standard_Integer Triang) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  Standard_Real u1, v1, u2, v2, u3, v3;
  gp_Pnt        P1 = Point(i1, u1, v1), P2 = Point(i2, u2, v2), P3 = Point(i3, u3, v3);
  return PolyUtils::DeflectionOnTriangle(Surface, P1, P2, P3, u1, v1, u2, v2, u3, v3);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Parameters(const Standard_Integer Index,
                                                   Standard_Real&         U,
                                                   Standard_Real&         V) const
{
  PolyUtils::Parameters(Index, C_MyU, C_MyV, U, V);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOverEstimation(const Standard_Real flec)
{
  PolyUtils::SetDeflectionOverEstimation(flec, TheDeflection, TheBnd);
}

//==================================================================================================

Standard_Real HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOverEstimation() const
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
  TheComponentsBnd = new Bnd_HArray1OfBox(1, NbTriangles());
  PolyUtils::FillBounding(static_cast<gp_Pnt*>(C_MyPnts),
                          nbdeltaU,
                          nbdeltaV,
                          TheDeflection,
                          TheComponentsBnd);
}

//==================================================================================================

const Handle(Bnd_HArray1OfBox)& HLRBRep_ThePolyhedronOfInterCSurf::ComponentsBounding() const
{
  return TheComponentsBnd;
}

//==================================================================================================

Standard_Integer HLRBRep_ThePolyhedronOfInterCSurf::NbTriangles() const
{
  return PolyUtils::NbTriangles(nbdeltaU, nbdeltaV);
}

//==================================================================================================

Standard_Integer HLRBRep_ThePolyhedronOfInterCSurf::NbPoints() const
{
  return PolyUtils::NbPoints(nbdeltaU, nbdeltaV);
}

//==================================================================================================

Standard_Integer HLRBRep_ThePolyhedronOfInterCSurf::TriConnex(const Standard_Integer Triang,
                                                              const Standard_Integer Pivot,
                                                              const Standard_Integer Pedge,
                                                              Standard_Integer&      TriCon,
                                                              Standard_Integer&      OtherP) const
{
  return PolyUtils::TriConnex(Triang, Pivot, Pedge, TriCon, OtherP, nbdeltaU, nbdeltaV);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::PlaneEquation(const Standard_Integer Triang,
                                                      gp_XYZ&                NormalVector,
                                                      Standard_Real&         PolarDistance) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  PolyUtils::PlaneEquation(Point(i1), Point(i2), Point(i3), NormalVector, PolarDistance);
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::Contain(const Standard_Integer Triang,
                                                            const gp_Pnt&          ThePnt) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  return PolyUtils::Contain(Point(i1), Point(i2), Point(i3), ThePnt);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Dump() const {}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Size(Standard_Integer& nbdu, Standard_Integer& nbdv) const
{
  nbdu = nbdeltaU;
  nbdv = nbdeltaV;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Triangle(const Standard_Integer Index,
                                                 Standard_Integer&      P1,
                                                 Standard_Integer&      P2,
                                                 Standard_Integer&      P3) const
{
  PolyUtils::Triangle(Index, P1, P2, P3, nbdeltaV);
}

//==================================================================================================

const gp_Pnt& HLRBRep_ThePolyhedronOfInterCSurf::Point(const Standard_Integer Index,
                                                       Standard_Real&         U,
                                                       Standard_Real&         V) const
{
  return PolyUtils::Point(Index, C_MyPnts, C_MyU, C_MyV, U, V);
}

//==================================================================================================

const gp_Pnt& HLRBRep_ThePolyhedronOfInterCSurf::Point(const Standard_Integer Index) const
{
  return PolyUtils::Point(Index, C_MyPnts);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Point(const gp_Pnt&,
                                              const Standard_Integer,
                                              const Standard_Integer,
                                              const Standard_Real,
                                              const Standard_Real)
{
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Point(const Standard_Integer Index, gp_Pnt& P) const
{
  PolyUtils::Point(Index, C_MyPnts, P);
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::IsOnBound(const Standard_Integer Index1,
                                                              const Standard_Integer Index2) const
{
  return PolyUtils::IsOnBound(Index1,
                              Index2,
                              static_cast<Standard_Boolean*>(C_MyIsOnBounds),
                              nbdeltaU,
                              nbdeltaV);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::UMinSingularity(const Standard_Boolean Sing)
{
  UMinSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::UMaxSingularity(const Standard_Boolean Sing)
{
  UMaxSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::VMinSingularity(const Standard_Boolean Sing)
{
  VMinSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::VMaxSingularity(const Standard_Boolean Sing)
{
  VMaxSingular = Sing;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasUMinSingularity() const
{
  return UMinSingular;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasUMaxSingularity() const
{
  return UMaxSingular;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasVMinSingularity() const
{
  return VMinSingular;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasVMaxSingularity() const
{
  return VMaxSingular;
}
