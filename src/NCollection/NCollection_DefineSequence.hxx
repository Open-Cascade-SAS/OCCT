// Created on: 2002-03-28
// Created by: Alexander GRIGORIEV
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

//            Automatically created from NCollection_Sequence.hxx by GAWK
// Purpose:     Definition of a sequence of elements indexed by
//              an Integer in range of 1..n


#ifndef NCollection_DefineSequence_HeaderFile
#define NCollection_DefineSequence_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Sequence.hxx>

// **************************************** Template for Sequence class ********

#define DEFINE_SEQUENCE(_ClassName_, _BaseCollection_, TheItemType)            \
typedef NCollection_Sequence<TheItemType > _ClassName_;

#endif
