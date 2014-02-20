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
