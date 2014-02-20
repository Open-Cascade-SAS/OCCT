// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

//            Automatically created from NCollection_Set.hxx by GAWK
// Purpose:      A set is an  unordered  collection  of items without
//               duplications. To test for duplications the operators == and !=
//               are used on the items.
//               Inherits BaseList, adding the data item to each node.


#ifndef NCollection_DefineSet_HeaderFile
#define NCollection_DefineSet_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Set.hxx>

// **************************************** Template for   Set   class ********

#define DEFINE_SET(_ClassName_, _BaseCollection_, TheItemType)                 \
typedef NCollection_Set<TheItemType > _ClassName_;

#endif
