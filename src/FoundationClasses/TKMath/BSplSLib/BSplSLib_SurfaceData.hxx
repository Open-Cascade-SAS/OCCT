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

#ifndef BSplSLib_SurfaceData_HeaderFile
#define BSplSLib_SurfaceData_HeaderFile

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//! Data structure holding complete BSpline surface definition.
struct BSplSLib_SurfaceData
{
  NCollection_Array2<gp_Pnt>  Poles;
  NCollection_Array2<double>  Weights;
  NCollection_Array1<double>  UKnots;
  NCollection_Array1<double>  VKnots;
  NCollection_Array1<double>  UFlatKnots;
  NCollection_Array1<double>  VFlatKnots;
  NCollection_Array1<int>     UMults;
  NCollection_Array1<int>     VMults;
  int                         UDegree = 0;
  int                         VDegree = 0;
  bool                        IsUPeriodic = false;
  bool                        IsVPeriodic = false;
};

#endif // BSplSLib_SurfaceData_HeaderFile
