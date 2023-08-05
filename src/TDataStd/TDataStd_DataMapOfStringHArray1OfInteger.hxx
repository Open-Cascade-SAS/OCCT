// Created on: 1995-05-10
// Created by: Denis PASCAL
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef TDataStd_DataMapOfStringHArray1OfInteger_HeaderFile
#define TDataStd_DataMapOfStringHArray1OfInteger_HeaderFile

#include <TCollection_ExtendedString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TCollection_ExtendedString,Handle(TColStd_HArray1OfInteger)> TDataStd_DataMapOfStringHArray1OfInteger;
typedef NCollection_DataMap<TCollection_ExtendedString,Handle(TColStd_HArray1OfInteger)>::Iterator TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger;


#endif
