// Created on: 2002-04-24
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

// Purpose:     The DataMap is a Map to store keys with associated
//              Items. See Map  from NCollection for  a discussion
//              about the number of buckets.
//              The DataMap can be seen as an extended array where
//              the Keys  are the   indices.  For this reason  the
//              operator () is defined on DataMap to fetch an Item
//              from a Key. So the following syntax can be used :
//              anItem = aMap(aKey);
//              aMap(aKey) = anItem;
//              This analogy has its  limit.   aMap(aKey) = anItem
//              can  be done only  if aKey was previously bound to
//              an item in the map.

#ifndef NCollection_DefineDataMap_HeaderFile
#define NCollection_DefineDataMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DataMap.hxx>

// *********************************************** Class DataMap *************

#define DEFINE_DATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType) \
typedef NCollection_DataMap<TheKeyType, TheItemType > _ClassName_;

#endif
