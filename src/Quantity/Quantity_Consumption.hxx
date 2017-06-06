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

#ifndef _Quantity_Consumption_HeaderFile
#define _Quantity_Consumption_HeaderFile

#include <Standard_Real.hxx>

//! Defined as a measure of fuel used per unit distance
//! travelled, or distance travelled per unit of fuel.
//! It is measured in litres per 100 kilometres or in
//! miles per gallon (UK or US).
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_Consumption;

#endif // _Quantity_Consumption_HeaderFile
