// Created on: 2002-04-23
// Created by: Alexander KARTOMIN
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

// Purpose:     This Iterator class iterates on BaseList of TListNode and is 
//              instantiated in List/Set/Queue/Stack
// Remark:      TListIterator is internal class

#ifndef NCollection_DefineTListIterator_HeaderFile
#define NCollection_DefineTListIterator_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_TListIterator.hxx>

// ********************************** Implementation of the Iterator interface
#define DEFINE_TLISTITERATOR(_ClassName_, _BaseCollection_, TheItemType)       \
typedef NCollection_TListIterator<TheItemType > _ClassName_;

#endif
