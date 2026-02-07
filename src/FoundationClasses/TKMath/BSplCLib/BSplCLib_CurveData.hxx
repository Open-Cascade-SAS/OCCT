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

#ifndef BSplCLib_CurveData_HeaderFile
#define BSplCLib_CurveData_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

//! Data structure holding complete BSpline curve definition.
//! @tparam PointType the point type (gp_Pnt2d for 2D curves, gp_Pnt for 3D curves)
template <typename PointType>
struct BSplCLib_CurveData
{
  NCollection_Array1<PointType> Poles;
  NCollection_Array1<double>    Weights;
  NCollection_Array1<double>    Knots;
  NCollection_Array1<double>    FlatKnots;
  NCollection_Array1<int>       Mults;
  int                           Degree = 0;
  bool                          IsPeriodic = false;
};

using BSplCLib_CurveData2d = BSplCLib_CurveData<gp_Pnt2d>;
using BSplCLib_CurveData3d = BSplCLib_CurveData<gp_Pnt>;

#endif // BSplCLib_CurveData_HeaderFile
