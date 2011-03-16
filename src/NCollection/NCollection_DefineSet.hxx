// File:         NCollection_DefineSet.hxx
// Created:      17.04.02 10:12:48
// Author:       Alexander Kartomin (akm)
//               <a-kartomin@opencascade.com>
//            Automatically created from NCollection_Set.hxx by GAWK
// Copyright:    Open Cascade 2002
//               
// Purpose:      A set is an  unordered  collection  of items without
//               duplications. To test for duplications the operators == and !=
//               are used on the items.
//               Inherits BaseList, adding the data item to each node.
//


#ifndef NCollection_DefineSet_HeaderFile
#define NCollection_DefineSet_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Set.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// **************************************** Template for   Set   class ********

#define DEFINE_SET(_ClassName_, _BaseCollection_, TheItemType)                 \
        typedef NCollection_Set<TheItemType > _ClassName_;

#endif
