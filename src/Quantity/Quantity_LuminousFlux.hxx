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

#ifndef _Quantity_LuminousFlux_HeaderFile
#define _Quantity_LuminousFlux_HeaderFile

#include <Standard_Real.hxx>

//! Defined as the rate of flow of radiant energy as evaluated
//! by the luminous sensation it produces. This means it
//! depends on the sensitivity of the receptor or observer. It
//! is related to the radiant flux of the source by the
//! spectral luminous efficiency.
//! It is measured in lumens.
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_LuminousFlux;

#endif // _Quantity_LuminousFlux_HeaderFile
