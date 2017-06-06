// Created on: 1994-02-08
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Quantity_AcousticIntensity_HeaderFile
#define _Quantity_AcousticIntensity_HeaderFile

#include <Standard_Real.hxx>

//! Defined as a dimensionless comparison of sound pressure
//! levels. The conventional unit, the bel, is the base ten
//! logarithm of the ratio of the two pressures. In practice
//! the decibel (one tenth of a bel) is used.
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_AcousticIntensity;

#endif // _Quantity_AcousticIntensity_HeaderFile
