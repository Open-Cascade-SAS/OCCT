// Created on: 1996-01-22
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

#ifndef _FairCurve_AnalysisCode_HeaderFile
#define _FairCurve_AnalysisCode_HeaderFile

//! To deal with different results in the computation of curvatures.
//! -   FairCurve_OK describes the case where computation is successfully
//! completed
//! -   FairCurve_NotConverged describes
//! the case where the algorithm does not
//! converge. In this case, you can not be
//! certain of the result quality and should
//! resume computation if you want to make use of the curve.
//! -   FairCurve_InfiniteSliding describes the case where sliding is infinite, and,
//! consequently, computation stops. The solution is to use an imposed sliding value.
//! -   FairCurve_NullHeight describes the case where no matter is left at one of the
//! ends of the curve, and as a result, computation stops. The solution is to
//! change (increase or reduce) the slope value by increasing or decreasing it.
enum FairCurve_AnalysisCode
{
  FairCurve_OK,
  FairCurve_NotConverged,
  FairCurve_InfiniteSliding,
  FairCurve_NullHeight
};

#endif // _FairCurve_AnalysisCode_HeaderFile
