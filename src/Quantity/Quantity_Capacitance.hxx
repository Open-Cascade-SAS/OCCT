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

#ifndef _Quantity_Capacitance_HeaderFile
#define _Quantity_Capacitance_HeaderFile

#include <Standard_Real.hxx>

//! Defined as a measure of the increase in voltage on
//! an isolated conductor by the addition of a charge.
//! for two isolated conductors, it is the ratio of the charge
//! on either conductor to the potential difference between
//! them.
//! It is measured in farads (coulomb per volt).
//! In practice micro-, nano-, and pico-farads are used.
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_Capacitance;

#endif // _Quantity_Capacitance_HeaderFile
