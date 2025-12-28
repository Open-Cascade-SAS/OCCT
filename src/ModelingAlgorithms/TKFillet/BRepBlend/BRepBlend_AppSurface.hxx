// Created on: 1996-11-25
// Created by: Philippe MANGIN
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

#ifndef _BRepBlend_AppSurface_HeaderFile
#define _BRepBlend_AppSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Approx_SweepApproximation.hxx>
#include <AppBlend_Approx.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OStream.hxx>
class Approx_SweepFunction;

//! Used to Approximate the blending surfaces.
class BRepBlend_AppSurface : public AppBlend_Approx
{
public:
  DEFINE_STANDARD_ALLOC

  //! Approximation of the new Surface (and
  //! eventually the 2d Curves on the support
  //! surfaces).
  //! Normally the 2d curve are
  //! approximated with a tolerance given by the
  //! resolution on support surfaces, but if this
  //! tolerance is too large Tol2d is used.
  Standard_EXPORT BRepBlend_AppSurface(const occ::handle<Approx_SweepFunction>& Funct,
                                       const double                             First,
                                       const double                             Last,
                                       const double                             Tol3d,
                                       const double                             Tol2d,
                                       const double                             TolAngular,
                                       const GeomAbs_Shape Continuity = GeomAbs_C0,
                                       const int           Degmax     = 11,
                                       const int           Segmax     = 50);

  bool IsDone() const;

  Standard_EXPORT void SurfShape(int& UDegree,
                                 int& VDegree,
                                 int& NbUPoles,
                                 int& NbVPoles,
                                 int& NbUKnots,
                                 int& NbVKnots) const;

  Standard_EXPORT void Surface(NCollection_Array2<gp_Pnt>& TPoles,
                               NCollection_Array2<double>& TWeights,
                               NCollection_Array1<double>& TUKnots,
                               NCollection_Array1<double>& TVKnots,
                               NCollection_Array1<int>&    TUMults,
                               NCollection_Array1<int>&    TVMults) const;

  int UDegree() const;

  int VDegree() const;

  const NCollection_Array2<gp_Pnt>& SurfPoles() const;

  const NCollection_Array2<double>& SurfWeights() const;

  const NCollection_Array1<double>& SurfUKnots() const;

  const NCollection_Array1<double>& SurfVKnots() const;

  const NCollection_Array1<int>& SurfUMults() const;

  const NCollection_Array1<int>& SurfVMults() const;

  //! returns the maximum error in the surface approximation.
  Standard_EXPORT double MaxErrorOnSurf() const;

  int NbCurves2d() const;

  Standard_EXPORT void Curves2dShape(int& Degree, int& NbPoles, int& NbKnots) const;

  Standard_EXPORT void Curve2d(const int                     Index,
                               NCollection_Array1<gp_Pnt2d>& TPoles,
                               NCollection_Array1<double>&   TKnots,
                               NCollection_Array1<int>&      TMults) const;

  int Curves2dDegree() const;

  const NCollection_Array1<gp_Pnt2d>& Curve2dPoles(const int Index) const;

  const NCollection_Array1<double>& Curves2dKnots() const;

  const NCollection_Array1<int>& Curves2dMults() const;

  Standard_EXPORT void TolReached(double& Tol3d, double& Tol2d) const;

  //! returns the maximum error in the <Index> 2d curve approximation.
  Standard_EXPORT double Max2dError(const int Index) const;

  Standard_EXPORT double TolCurveOnSurf(const int Index) const;

  //! display information on approximation.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  Approx_SweepApproximation approx;
};

#include <BRepBlend_AppSurface.lxx>

#endif // _BRepBlend_AppSurface_HeaderFile
