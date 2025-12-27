// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_HeaderFile
#define _GeomFill_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ParameterisationType.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
class Geom_Surface;
class Geom_Curve;
class gp_Vec;
class gp_Pnt;

//! Tools and Data to filling Surface and Sweep Surfaces
class GeomFill
{
public:
  DEFINE_STANDARD_ALLOC

  //! Builds a ruled surface between the two curves, Curve1 and Curve2.
  Standard_EXPORT static occ::handle<Geom_Surface> Surface(const occ::handle<Geom_Curve>& Curve1,
                                                      const occ::handle<Geom_Curve>& Curve2);

  Standard_EXPORT static void GetCircle(const Convert_ParameterisationType TConv,
                                        const gp_Vec&                      ns1,
                                        const gp_Vec&                      ns2,
                                        const gp_Vec&                      nplan,
                                        const gp_Pnt&                      pt1,
                                        const gp_Pnt&                      pt2,
                                        const double                Rayon,
                                        const gp_Pnt&                      Center,
                                        NCollection_Array1<gp_Pnt>&                Poles,
                                        NCollection_Array1<double>&              Weigths);

  Standard_EXPORT static bool GetCircle(const Convert_ParameterisationType TConv,
                                                    const gp_Vec&                      ns1,
                                                    const gp_Vec&                      ns2,
                                                    const gp_Vec&                      dn1w,
                                                    const gp_Vec&                      dn2w,
                                                    const gp_Vec&                      nplan,
                                                    const gp_Vec&                      dnplan,
                                                    const gp_Pnt&                      pts1,
                                                    const gp_Pnt&                      pts2,
                                                    const gp_Vec&                      tang1,
                                                    const gp_Vec&                      tang2,
                                                    const double                Rayon,
                                                    const double                DRayon,
                                                    const gp_Pnt&                      Center,
                                                    const gp_Vec&                      DCenter,
                                                    NCollection_Array1<gp_Pnt>&                Poles,
                                                    NCollection_Array1<gp_Vec>&                DPoles,
                                                    NCollection_Array1<double>&              Weigths,
                                                    NCollection_Array1<double>&              DWeigths);

  Standard_EXPORT static bool GetCircle(const Convert_ParameterisationType TConv,
                                                    const gp_Vec&                      ns1,
                                                    const gp_Vec&                      ns2,
                                                    const gp_Vec&                      dn1w,
                                                    const gp_Vec&                      dn2w,
                                                    const gp_Vec&                      d2n1w,
                                                    const gp_Vec&                      d2n2w,
                                                    const gp_Vec&                      nplan,
                                                    const gp_Vec&                      dnplan,
                                                    const gp_Vec&                      d2nplan,
                                                    const gp_Pnt&                      pts1,
                                                    const gp_Pnt&                      pts2,
                                                    const gp_Vec&                      tang1,
                                                    const gp_Vec&                      tang2,
                                                    const gp_Vec&                      Dtang1,
                                                    const gp_Vec&                      Dtang2,
                                                    const double                Rayon,
                                                    const double                DRayon,
                                                    const double                D2Rayon,
                                                    const gp_Pnt&                      Center,
                                                    const gp_Vec&                      DCenter,
                                                    const gp_Vec&                      D2Center,
                                                    NCollection_Array1<gp_Pnt>&                Poles,
                                                    NCollection_Array1<gp_Vec>&                DPoles,
                                                    NCollection_Array1<gp_Vec>&                D2Poles,
                                                    NCollection_Array1<double>&              Weigths,
                                                    NCollection_Array1<double>&              DWeigths,
                                                    NCollection_Array1<double>&              D2Weigths);

  Standard_EXPORT static void GetShape(const double           MaxAng,
                                       int&             NbPoles,
                                       int&             NbKnots,
                                       int&             Degree,
                                       Convert_ParameterisationType& TypeConv);

  Standard_EXPORT static void Knots(const Convert_ParameterisationType TypeConv,
                                    NCollection_Array1<double>&              TKnots);

  Standard_EXPORT static void Mults(const Convert_ParameterisationType TypeConv,
                                    NCollection_Array1<int>&           TMults);

  Standard_EXPORT static void GetMinimalWeights(const Convert_ParameterisationType TConv,
                                                const double                AngleMin,
                                                const double                AngleMax,
                                                NCollection_Array1<double>&              Weigths);

  //! Used by the generical classes to determine
  //! Tolerance for approximation
  Standard_EXPORT static double GetTolerance(const Convert_ParameterisationType TConv,
                                                    const double                AngleMin,
                                                    const double                Radius,
                                                    const double                AngularTol,
                                                    const double                SpatialTol);
};

#endif // _GeomFill_HeaderFile
