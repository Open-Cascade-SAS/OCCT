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
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
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
  Standard_EXPORT static Handle(Geom_Surface) Surface(const Handle(Geom_Curve)& Curve1,
                                                      const Handle(Geom_Curve)& Curve2);

  Standard_EXPORT static void GetCircle(const Convert_ParameterisationType TConv,
                                        const gp_Vec&                      ns1,
                                        const gp_Vec&                      ns2,
                                        const gp_Vec&                      nplan,
                                        const gp_Pnt&                      pt1,
                                        const gp_Pnt&                      pt2,
                                        const Standard_Real                Rayon,
                                        const gp_Pnt&                      Center,
                                        TColgp_Array1OfPnt&                Poles,
                                        TColStd_Array1OfReal&              Weigths);

  Standard_EXPORT static Standard_Boolean GetCircle(const Convert_ParameterisationType TConv,
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
                                                    const Standard_Real                Rayon,
                                                    const Standard_Real                DRayon,
                                                    const gp_Pnt&                      Center,
                                                    const gp_Vec&                      DCenter,
                                                    TColgp_Array1OfPnt&                Poles,
                                                    TColgp_Array1OfVec&                DPoles,
                                                    TColStd_Array1OfReal&              Weigths,
                                                    TColStd_Array1OfReal&              DWeigths);

  Standard_EXPORT static Standard_Boolean GetCircle(const Convert_ParameterisationType TConv,
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
                                                    const Standard_Real                Rayon,
                                                    const Standard_Real                DRayon,
                                                    const Standard_Real                D2Rayon,
                                                    const gp_Pnt&                      Center,
                                                    const gp_Vec&                      DCenter,
                                                    const gp_Vec&                      D2Center,
                                                    TColgp_Array1OfPnt&                Poles,
                                                    TColgp_Array1OfVec&                DPoles,
                                                    TColgp_Array1OfVec&                D2Poles,
                                                    TColStd_Array1OfReal&              Weigths,
                                                    TColStd_Array1OfReal&              DWeigths,
                                                    TColStd_Array1OfReal&              D2Weigths);

  Standard_EXPORT static void GetShape(const Standard_Real           MaxAng,
                                       Standard_Integer&             NbPoles,
                                       Standard_Integer&             NbKnots,
                                       Standard_Integer&             Degree,
                                       Convert_ParameterisationType& TypeConv);

  Standard_EXPORT static void Knots(const Convert_ParameterisationType TypeConv,
                                    TColStd_Array1OfReal&              TKnots);

  Standard_EXPORT static void Mults(const Convert_ParameterisationType TypeConv,
                                    TColStd_Array1OfInteger&           TMults);

  Standard_EXPORT static void GetMinimalWeights(const Convert_ParameterisationType TConv,
                                                const Standard_Real                AngleMin,
                                                const Standard_Real                AngleMax,
                                                TColStd_Array1OfReal&              Weigths);

  //! Used  by  the  generical classes to determine
  //! Tolerance for approximation
  Standard_EXPORT static Standard_Real GetTolerance(const Convert_ParameterisationType TConv,
                                                    const Standard_Real                AngleMin,
                                                    const Standard_Real                Radius,
                                                    const Standard_Real                AngularTol,
                                                    const Standard_Real                SpatialTol);
};

#endif // _GeomFill_HeaderFile
