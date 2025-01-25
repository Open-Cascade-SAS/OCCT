// Created on: 2002-05-15
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef QANCollection_Common_HeaderFile
#define QANCollection_Common_HeaderFile

#include <gp_Pnt.hxx>

// ===================== Methods for accessing items/keys =====================

// To print other type of items define PrintItem for it

Standard_EXPORT void PrintItem(const gp_Pnt& thePnt);
Standard_EXPORT void PrintItem(const Standard_Real theDbl);

// So do for the pseudo-random generation

Standard_EXPORT void Random(Standard_Real& theValue);
Standard_EXPORT void Random(Standard_Integer& theValue, const Standard_Integer theMax = RAND_MAX);
Standard_EXPORT void Random(gp_Pnt& thePnt);

#endif
