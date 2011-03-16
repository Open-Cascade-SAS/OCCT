// File:      VrmlData_MapOfNode.hxx
// Created:   22.07.07 09:12
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef VrmlData_MapOfNode_HeaderFile
#define VrmlData_MapOfNode_HeaderFile

#include <NCollection_Map.hxx>
#include <VrmlData_Node.hxx>

/**
 * Container of Map type, holding handles to VrmlData_Node objects
 */

typedef NCollection_Map<Handle_VrmlData_Node> VrmlData_MapOfNode; 

#endif
