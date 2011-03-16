// File:        NCollection_DefineTListIterator.hxx
// Created:     Tue Apr 23 17:33:02 2002
// Author:      Alexander KARTOMIN
//              <a-kartomin@opencascade.com>
//
// Purpose:     This Iterator class iterates on BaseList of TListNode and is 
//              instantiated in List/Set/Queue/Stack
// Remark:      TListIterator is internal class
//

#ifndef NCollection_DefineTListIterator_HeaderFile
#define NCollection_DefineTListIterator_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_TListIterator.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// ********************************** Implementation of the Iterator interface
#define DEFINE_TLISTITERATOR(_ClassName_, _BaseCollection_, TheItemType)       \
        typedef NCollection_TListIterator<TheItemType > _ClassName_;

#endif
