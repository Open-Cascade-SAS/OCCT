// File:      VrmlData_ListOfNode.hxx
// Created:   06.11.06 11:39
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef VrmlData_ListOfNode_HeaderFile
#define VrmlData_ListOfNode_HeaderFile

#include <NCollection_List.hxx>
#include <VrmlData_Node.hxx>

/**
 * Container of List type, holding handles to VrmlData_Node objects
 */

typedef NCollection_List<Handle_VrmlData_Node> VrmlData_ListOfNode; 

#endif
