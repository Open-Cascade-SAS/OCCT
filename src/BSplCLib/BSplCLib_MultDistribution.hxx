// Created on: 1991-08-09
// Created by: Jean Claude VAUTHIER
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

#ifndef _BSplCLib_MultDistribution_HeaderFile
#define _BSplCLib_MultDistribution_HeaderFile

//! This   enumeration describes the   form  of  the
//! sequence of mutiplicities.  MultDistribution is :
//!
//! Constant if all the multiplicities have the same
//! value.
//!
//! QuasiConstant if all the internal knots have the
//! same multiplicity and if the first and last knot
//! have  a different  multiplicity.
//!
//! NonConstant in other cases.
enum BSplCLib_MultDistribution
{
BSplCLib_NonConstant,
BSplCLib_Constant,
BSplCLib_QuasiConstant
};

#endif // _BSplCLib_MultDistribution_HeaderFile
