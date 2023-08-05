// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_Container_HeaderFile
#define TObj_Container_HeaderFile

#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>

#include <TObj_SequenceOfObject.hxx>

typedef NCollection_DataMap<Handle(TCollection_HExtendedString), TDF_Label> TObj_DataMapOfNameLabel;

typedef NCollection_DataMap<Handle(TObj_Object), Handle(TObj_HSequenceOfObject)> TObj_DataMapOfObjectHSequenceOcafObjects;

typedef NCollection_DataMap<TCollection_AsciiString, Standard_Address> TObj_DataMapOfStringPointer;

#endif
