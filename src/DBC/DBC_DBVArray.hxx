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

#ifndef _DBC_DBVArray_HeaderFile
#define _DBC_DBVArray_HeaderFile

#ifdef OBJY
#include <PStandard_ArrayNode.hxx>
declare(ooVArray,PHandle_PStandard_ArrayNode)
typedef ooVArray(PHandle_PStandard_ArrayNode) DBC_DBVArray;
#endif

#ifdef CSFDB
#include <PStandard_ArrayNode.hxx>
//typedef Handle(PStandard_ArrayNode)* DBC_DBVArray;
typedef void* DBC_DBVArray;
#endif

#ifdef OBJS
#include <PStandard_ArrayNode.hxx>
typedef PStandard_ArrayNode* DBC_DBVArray;
#endif

#endif
