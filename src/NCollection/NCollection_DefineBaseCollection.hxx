// Created on: 2002-04-09
// Created by: Alexander KARTOMIN (akm)
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

// Purpose:     NCollection_BaseCollection  is the base  abstract  class for 
//              all collection templates of this package.
//              The set  of collections is similar  to that  of TCollection.
//              Also  the methods of classes  have mostly the same names for
//              easy switch from TCollection <-> NCollection containers.
//              NCollection is a nocdlpack, thus it is compiled without WOK.
//              BaseCollection allows assigning the collections of different
//              kinds  (the  items  type  being the same) with a few obvious
//              exclusions - one can not  assign any  collection to  the map 
//              having double data (two keys or a key  plus value). Only the 
//              maps of the very same type may be assigned through operator=
//              Said maps are: DoubleMap,
//                             DataMap,
//                             IndexedDataMap
//              For the  users  needing  control  over the memory  usage the
//              allocators were added (see NCollection_BaseAllocator header)
//              Others may forget it -  BaseAllocator is used by default and
//              then memory is managed through Standard::Allocate/::Free.

#ifndef NCollection_DefineBaseCollection_HeaderFile
#define NCollection_DefineBaseCollection_HeaderFile

#include <NCollection_BaseCollection.hxx>

// *************************************** Template for BaseCollection class

#define DEFINE_BASECOLLECTION(_ClassName_, TheItemType)                        \
typedef NCollection_BaseCollection<TheItemType > _ClassName_;

#endif
