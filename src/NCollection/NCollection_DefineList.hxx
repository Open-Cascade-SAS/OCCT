// File:         NCollection_DefineList.hxx
// Created:      17.04.02 10:12:48
// Author:       Alexander Kartomin (akm)
//               <a-kartomin@opencascade.com>
//            Automatically created from NCollection_List.hxx by GAWK
// Copyright:    Open Cascade 2002
//
// Purpose:      Simple list to link  items together keeping the first 
//               and the last one.
//               Inherits BaseList, adding the data item to each node.
//               


#ifndef NCollection_DefineList_HeaderFile
#define NCollection_DefineList_HeaderFile

#include <NCollection_List.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// **************************************** Template for  List   class ********

#define DEFINE_LIST(_ClassName_, _BaseCollection_, TheItemType)                \
        typedef NCollection_List <TheItemType > _ClassName_;

#endif
