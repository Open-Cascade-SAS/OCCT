// Created on: 2002-04-23
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

// Purpose:     Single hashed Map. This  Map is used  to store and
//              retrieve keys in linear time.
//              The ::Iterator class can be  used to explore  the
//              content of the map. It is not  wise to iterate and
//              modify a map in parallel.
//              To compute  the hashcode of  the key the  function
//              ::HashCode must be defined in the global namespace
//              To compare two keys the function ::IsEqual must be
//              defined in the global namespace.
//              The performance of  a Map is conditionned  by  its
//              number of buckets that  should be kept greater  to
//              the number   of keys.  This  map has  an automatic
//              management of the number of buckets. It is resized
//              when  the number of Keys  becomes greater than the
//              number of buckets.
//              If you have a fair  idea of the number of  objects
//              you  can save on automatic   resizing by giving  a
//              number of buckets  at creation or using the ReSize
//              method. This should be  consider only for  crucial
//              optimisation issues.

#ifndef NCollection_DefineMap_HeaderFile
#define NCollection_DefineMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Map.hxx>

// *********************************************** Class Map *****************

#define DEFINE_MAP(_ClassName_, _BaseCollection_, TheKeyType)                  \
typedef NCollection_Map <TheKeyType > _ClassName_;

#endif
