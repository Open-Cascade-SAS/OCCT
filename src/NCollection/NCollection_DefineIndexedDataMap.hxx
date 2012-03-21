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

// Purpose:     An indexed map is used  to store keys and to  bind
//              an index to them.  Each  new key stored in the map
//              gets an index.  Index are  incremented as keys are
//              stored in the map. A key can be found by the index
//              and an index by the key.  No  key but the last can
//              be  removed so the  indices   are in the range 1..
//              Extent.  An Item is stored with each key.
//              This   class is   similar  to  IndexedMap     from
//              NCollection  with the Item as  a new feature. Note
//              the important difference on  the operator  ().  In
//              the IndexedMap this operator returns  the Key.  In
//              the IndexedDataMap this operator returns the Item.
//              See  the  class   Map   from NCollection   for   a
//              discussion about the number of buckets.

#ifndef NCollection_DefineIndexedDataMap_HeaderFile
#define NCollection_DefineIndexedDataMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_IndexedDataMap.hxx>

// *********************************************** Class IndexedDataMap ******

#define DEFINE_INDEXEDDATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType) \
typedef NCollection_IndexedDataMap <TheKeyType, TheItemType > _ClassName_;

#endif
