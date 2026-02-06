// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _Convert_BSplineCurveData_HeaderFile
#define _Convert_BSplineCurveData_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

//! Data structure holding complete BSpline curve data in 2D.
//! Provides a single-shot way to retrieve all BSpline curve parameters
//! instead of element-by-element access.
struct Convert_BSplineCurveData2d
{
  NCollection_Array1<gp_Pnt2d> Poles;
  NCollection_Array1<double>   Weights;
  NCollection_Array1<double>   Knots;
  NCollection_Array1<int>      Mults;
  int                          Degree = 0;
  bool                         IsPeriodic = false;
};

//! Data structure holding complete BSpline curve data in 3D.
//! Provides a single-shot way to retrieve all BSpline curve parameters
//! instead of element-by-element access.
struct Convert_BSplineCurveData3d
{
  NCollection_Array1<gp_Pnt> Poles;
  NCollection_Array1<double> Weights;
  NCollection_Array1<double> Knots;
  NCollection_Array1<int>    Mults;
  int                        Degree = 0;
  bool                       IsPeriodic = false;
};

#endif // _Convert_BSplineCurveData_HeaderFile
