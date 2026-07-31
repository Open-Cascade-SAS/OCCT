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

#ifndef _CSLib_DerivativeStatus_HeaderFile
#define _CSLib_DerivativeStatus_HeaderFile

//! Status of surface derivatives computation for normal calculation.
//!
//! Describes the result of attempting to compute a surface normal
//! from the first derivatives D1U and D1V at a point on a surface.
enum CSLib_DerivativeStatus
{
  CSLib_Done,              //!< Normal computed successfully.
  CSLib_D1uIsNull,         //!< D1U has null length: ||D1U|| <= Resolution.
  CSLib_D1vIsNull,         //!< D1V has null length: ||D1V|| <= Resolution.
  CSLib_D1IsNull,          //!< Both derivatives are null.
  CSLib_D1uD1vRatioIsNull, //!< D1U is negligible: ||D1U|| / ||D1V|| <= RealEpsilon.
  CSLib_D1vD1uRatioIsNull, //!< D1V is negligible: ||D1V|| / ||D1U|| <= RealEpsilon.
  CSLib_D1uIsParallelD1v   //!< D1U and D1V are parallel.
};

#endif // _CSLib_DerivativeStatus_HeaderFile
