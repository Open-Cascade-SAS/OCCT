// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//            Automatically created from NCollection_Queue.hxx by GAWK
// Purpose:      A queue is  a structure where Items are  added  at
//               the end  and removed from   the  front. The  first
//               entered Item  will be the  first removed. This  is
//               called a FIFO (First In First Out).
//               Inherits BaseList, adds the data item to each node.


#ifndef NCollection_DefineQueue_HeaderFile
#define NCollection_DefineQueue_HeaderFile

#include <NCollection_Queue.hxx>

// **************************************** Template for  Queue  class ********

#define DEFINE_QUEUE(_ClassName_, _BaseCollection_, TheItemType)               \
typedef NCollection_Queue<TheItemType > _ClassName_;

#endif
