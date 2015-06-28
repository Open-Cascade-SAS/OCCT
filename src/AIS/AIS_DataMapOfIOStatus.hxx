// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef AIS_DataMapOfIOStatus_HeaderFile
#define AIS_DataMapOfIOStatus_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_GlobalStatus.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Handle(AIS_InteractiveObject),Handle(AIS_GlobalStatus),TColStd_MapTransientHasher> AIS_DataMapOfIOStatus;
typedef NCollection_DataMap<Handle(AIS_InteractiveObject),Handle(AIS_GlobalStatus),TColStd_MapTransientHasher>::Iterator AIS_DataMapIteratorOfDataMapOfIOStatus;


#endif
