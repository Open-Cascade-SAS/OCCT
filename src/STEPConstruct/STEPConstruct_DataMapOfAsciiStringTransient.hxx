// Created on: 1999-11-17
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef STEPConstruct_DataMapOfAsciiStringTransient_HeaderFile
#define STEPConstruct_DataMapOfAsciiStringTransient_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>
  STEPConstruct_DataMapOfAsciiStringTransient;
typedef NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator
  STEPConstruct_DataMapIteratorOfDataMapOfAsciiStringTransient;

#endif
