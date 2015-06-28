// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef TDF_IDList_HeaderFile
#define TDF_IDList_HeaderFile

#include <Standard_GUID.hxx>
#include <NCollection_List.hxx>

typedef NCollection_List<Standard_GUID> TDF_IDList;
typedef NCollection_List<Standard_GUID>::Iterator TDF_ListIteratorOfIDList;


#endif
