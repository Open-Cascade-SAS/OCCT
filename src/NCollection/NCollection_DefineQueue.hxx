// File:         NCollection_DefineQueue.hxx
// Created:      17.04.02 10:12:48
// Author:       Alexander Kartomin (akm)
//               <a-kartomin@opencascade.com>
//            Automatically created from NCollection_Queue.hxx by GAWK
// Copyright:    Open Cascade 2002
//               
// Purpose:      A queue is  a structure where Items are  added  at
//               the end  and removed from   the  front. The  first
//               entered Item  will be the  first removed. This  is
//               called a FIFO (First In First Out).
//               Inherits BaseList, adds the data item to each node.
//               


#ifndef NCollection_DefineQueue_HeaderFile
#define NCollection_DefineQueue_HeaderFile

#include <NCollection_Queue.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// **************************************** Template for  Queue  class ********

#define DEFINE_QUEUE(_ClassName_, _BaseCollection_, TheItemType)               \
        typedef NCollection_Queue<TheItemType > _ClassName_;

#endif
