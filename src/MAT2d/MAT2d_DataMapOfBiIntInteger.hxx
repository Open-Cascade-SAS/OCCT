// Created on: 1993-07-06
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef MAT2d_DataMapOfBiIntInteger_HeaderFile
#define MAT2d_DataMapOfBiIntInteger_HeaderFile

#include <Standard_Integer.hxx>
#include <MAT2d_BiInt.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<MAT2d_BiInt,Standard_Integer> MAT2d_DataMapOfBiIntInteger;
typedef NCollection_DataMap<MAT2d_BiInt,Standard_Integer>::Iterator MAT2d_DataMapIteratorOfDataMapOfBiIntInteger;


#endif
