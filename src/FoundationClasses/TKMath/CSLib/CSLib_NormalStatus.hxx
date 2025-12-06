// Created on: 1991-09-09
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _CSLib_NormalStatus_HeaderFile
#define _CSLib_NormalStatus_HeaderFile

//! Status of surface normal computation.
//!
//! Describes the result of attempting to compute the normal N to a surface,
//! including cases involving derivatives of the normal (DN/du, DN/dv).
enum CSLib_NormalStatus
{
  CSLib_Singular,            //!< Surface is singular at the point (normal undefined).
  CSLib_Defined,             //!< Normal is well-defined and computed successfully.
  CSLib_InfinityOfSolutions, //!< Infinite normals possible.
  CSLib_D1NuIsNull,          //!< DN/du has null length: ||DN/du|| <= Resolution.
  CSLib_D1NvIsNull,          //!< DN/dv has null length: ||DN/dv|| <= Resolution.
  CSLib_D1NIsNull,           //!< Both normal derivatives are null.
  CSLib_D1NuNvRatioIsNull,   //!< DN/du is negligible compared to DN/dv.
  CSLib_D1NvNuRatioIsNull,   //!< DN/dv is negligible compared to DN/du.
  CSLib_D1NuIsParallelD1Nv   //!< DN/du and DN/dv are parallel.
};

#endif // _CSLib_NormalStatus_HeaderFile
