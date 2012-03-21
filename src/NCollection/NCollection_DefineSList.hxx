// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
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

//            Automatically created from NCollection_SList.hxx by GAWK
// Purpose:     An SList is a LISP like list of Items.
//              An SList is :
//                . Empty.
//                . Or it has a Value and a  Tail  which is an other SList. 
//              The Tail of an empty list is an empty list.
//              SList are  shared.  It  means   that they  can  be
//              modified through other lists.
//              SList may  be used  as Iterators. They  have Next,
//              More, and value methods. To iterate on the content
//              of the list S just do.
//              SList Iterator;
//              for (Iterator = S; Iterator.More(); Iterator.Next())
//                X = Iterator.Value();
//              Memory usage  is  automatically managed for  SLists
//              (using reference counts).
// Example:
//              If S1 and S2 are SLists :
//              if S1.Value() is X.
//              And the following is done :
//              S2 = S1;
//              S2.SetValue(Y);
//              S1.Value() becomes also Y.   So SList must be used
//              with   care.  Mainly  the SetValue()    method  is
//              dangerous. 


#ifndef NCollection_DefineSList_HeaderFile
#define NCollection_DefineSList_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_SList.hxx>

// **************************************** Template for  SList  class ********

#define DEFINE_SLIST(_ClassName_, _BaseCollection_, TheItemType)               \
typedef NCollection_SList<TheItemType > _ClassName_;

#endif
