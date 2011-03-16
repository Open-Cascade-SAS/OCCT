// File:        NCollection_DefineSList.hxx
// Created:     17.04.02 10:12:48
// Author:      Alexander Kartomin (akm)
//            Automatically created from NCollection_SList.hxx by GAWK
// Copyright:   Open Cascade 2002
//           
// Purpose:     An SList is a LISP like list of Items.
//              An SList is :
//                . Empty.
//                . Or it has a Value and a  Tail  which is an other SList. 
//              
//              The Tail of an empty list is an empty list.
//              SList are  shared.  It  means   that they  can  be
//              modified through other lists.
//              SList may  be used  as Iterators. They  have Next,
//              More, and value methods. To iterate on the content
//              of the list S just do.
//              
//              SList Iterator;
//              for (Iterator = S; Iterator.More(); Iterator.Next())
//                X = Iterator.Value();
//            
//              Memory usage  is  automatically managed for  SLists
//              (using reference counts).
//             
// Example:
//              If S1 and S2 are SLists :
//              if S1.Value() is X.
//              
//              And the following is done :
//              S2 = S1;
//              S2.SetValue(Y);
//            
//              S1.Value() becomes also Y.   So SList must be used
//              with   care.  Mainly  the SetValue()    method  is
//              dangerous. 
//              


#ifndef NCollection_DefineSList_HeaderFile
#define NCollection_DefineSList_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_SList.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// **************************************** Template for  SList  class ********

#define DEFINE_SLIST(_ClassName_, _BaseCollection_, TheItemType)               \
        typedef NCollection_SList<TheItemType > _ClassName_;

#endif
