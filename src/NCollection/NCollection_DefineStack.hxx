// File:         NCollection_DefineStack.hxx
// Created:      17.04.02 10:12:48
//               <a-kartomin@opencascade.com>
// Author:       Alexander Kartomin (akm)
//            Automatically created from NCollection_Stack.hxx by GAWK
// Copyright:    Open Cascade 2002
//
// Purpose:      A stack is a structure where item can be added and
//               removed from the top. Like a stack of plates  in a
//               kitchen. The   last entered item  will be   be the
//               first  removed. This  is  called  a  LIFO (last In First Out).
//               Inherits BaseList, adding the data item to each node.
//               


#ifndef NCollection_DefineStack_HeaderFile
#define NCollection_DefineStack_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Stack.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// **************************************** Template for  Stack  class ********

#define DEFINE_STACK(_ClassName_, _BaseCollection_, TheItemType)               \
        typedef NCollection_Stack<TheItemType > _ClassName_;

#endif
