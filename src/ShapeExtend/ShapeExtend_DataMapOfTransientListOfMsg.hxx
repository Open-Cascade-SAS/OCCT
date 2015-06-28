// Created on: 1998-07-21
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef ShapeExtend_DataMapOfTransientListOfMsg_HeaderFile
#define ShapeExtend_DataMapOfTransientListOfMsg_HeaderFile

#include <Standard_Transient.hxx>
#include <Message_ListOfMsg.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Handle(Standard_Transient),Message_ListOfMsg,TColStd_MapTransientHasher> ShapeExtend_DataMapOfTransientListOfMsg;
typedef NCollection_DataMap<Handle(Standard_Transient),Message_ListOfMsg,TColStd_MapTransientHasher>::Iterator ShapeExtend_DataMapIteratorOfDataMapOfTransientListOfMsg;


#endif
