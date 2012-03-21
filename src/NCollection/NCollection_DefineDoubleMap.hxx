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

// Purpose:     The DoubleMap  is used to  bind  pairs (Key1,Key2)
//              and retrieve them in linear time.
//              See Map from NCollection for a discussion about the number
//              of buckets

#ifndef NCollection_DefineDoubleMap_HeaderFile
#define NCollection_DefineDoubleMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DoubleMap.hxx>

// *********************************************** Class DoubleMap ************

#define DEFINE_DOUBLEMAP(_ClassName_, _BaseCollection_, TheKey1Type, TheKey2Type) \
typedef NCollection_DoubleMap <TheKey1Type, TheKey2Type > _ClassName_;

#endif
