// Created on: 2014-12-08
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_NMapOfTransient_HeaderFile
#define _Graphic3d_NMapOfTransient_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Shared.hxx>

typedef NCollection_Shared< NCollection_Map<const Standard_Transient* > > Graphic3d_NMapOfTransient;

#endif // _Graphic3d_NMapOfTransient_HeaderFile
