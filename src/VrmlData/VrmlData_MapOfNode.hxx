// Created on: 2007-07-22
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_MapOfNode_HeaderFile
#define VrmlData_MapOfNode_HeaderFile

#include <NCollection_Map.hxx>
#include <VrmlData_Node.hxx>

/**
 * Container of Map type, holding handles to VrmlData_Node objects
 */

typedef NCollection_Map<Handle(VrmlData_Node)> VrmlData_MapOfNode; 

#endif
