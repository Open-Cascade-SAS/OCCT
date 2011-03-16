// File:        NCollection_DefineTListNode.hxx
// Created:     Tue Apr 23 17:30:38 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@opencascade.com>
//
// Purpose:     Abstract list node class. Used by BaseList
// Remark:      Internal class
//              

#ifndef NCollection_DefineTListNode_HeaderFile
#define NCollection_DefineTListNode_HeaderFile

#include <NCollection_TListNode.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// ******************************* Class defining list node - for internal use
#define DEFINE_TLISTNODE(_ClassName_, _BaseCollection_, TheItemType)           \
        typedef NCollection_TListNode<TheItemType > _ClassName_;

#endif
