// Created on: 2002-04-09
// Created by: Alexander KARTOMIN (akm)
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
