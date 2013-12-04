// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//            Automatically created from NCollection_Stack.hxx by GAWK
// Purpose:      A stack is a structure where item can be added and
//               removed from the top. Like a stack of plates  in a
//               kitchen. The   last entered item  will be   be the
//               first  removed. This  is  called  a  LIFO (last In First Out).
//               Inherits BaseList, adding the data item to each node.


#ifndef NCollection_DefineStack_HeaderFile
#define NCollection_DefineStack_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Stack.hxx>

// **************************************** Template for  Stack  class ********

#define DEFINE_STACK(_ClassName_, _BaseCollection_, TheItemType)               \
typedef NCollection_Stack<TheItemType > _ClassName_;

#endif
