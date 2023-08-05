// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef XmlMDF_TypeADriverMap_HeaderFile
#define XmlMDF_TypeADriverMap_HeaderFile

#include <Standard_Type.hxx>
#include <XmlMDF_ADriver.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Handle(Standard_Type),Handle(XmlMDF_ADriver)> XmlMDF_TypeADriverMap;
typedef NCollection_DataMap<Handle(Standard_Type),Handle(XmlMDF_ADriver)>::Iterator XmlMDF_DataMapIteratorOfTypeADriverMap;


#endif
