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

#ifndef _Quantity_Work_HeaderFile
#define _Quantity_Work_HeaderFile

#include <Standard_Real.hxx>

//! In mechanics, the product of a force and the distance
//! moved. In a rotating system, the product of the torque
//! and the angular displacement. In a pressure system,
//! the product of the pressure and the change in volume.
//! In electrical terms, the product of a charge and the
//! potential difference it traverses.
//! It is measured in joules (newton-metres).
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_Work;

#endif // _Quantity_Work_HeaderFile
