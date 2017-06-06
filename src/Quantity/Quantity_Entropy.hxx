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

#ifndef _Quantity_Entropy_HeaderFile
#define _Quantity_Entropy_HeaderFile

#include <Standard_Real.hxx>

//! Defined as a property which changes as a system undergoes
//! reversible change. The change in entropy is quantified as
//! the change in energy per degree of temperature. All real
//! changes are at least partially irreversible so entropy
//! is increased by change. Entropy can be viewed as a
//! measure of the molecular disorder of a system, or as the
//! unavailability of its internal energy to do work in a
//! cyclical process.
//! A change in entropy is measured in joules per degree kelvin.
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_Entropy;

#endif // _Quantity_Entropy_HeaderFile
