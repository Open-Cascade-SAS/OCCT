// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch_HeaderFile
#define _Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Contap_ThePathPointOfTheSearch.hxx>
#include <TCollection_SeqNode.hxx>
#include <TCollection_SeqNodePtr.hxx>
class Contap_ThePathPointOfTheSearch;
class Contap_SequenceOfPathPointOfTheSearch;


class Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch;
DEFINE_STANDARD_HANDLE(Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch, TCollection_SeqNode)


class Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch : public TCollection_SeqNode
{

public:

  
    Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch(const Contap_ThePathPointOfTheSearch& I, const TCollection_SeqNodePtr& n, const TCollection_SeqNodePtr& p);
  
    Contap_ThePathPointOfTheSearch& Value() const;




  DEFINE_STANDARD_RTTI(Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch,TCollection_SeqNode)

protected:




private:


  Contap_ThePathPointOfTheSearch myValue;


};

#define SeqItem Contap_ThePathPointOfTheSearch
#define SeqItem_hxx <Contap_ThePathPointOfTheSearch.hxx>
#define TCollection_SequenceNode Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch
#define TCollection_SequenceNode_hxx <Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch.hxx>
#define Handle_TCollection_SequenceNode Handle(Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch)
#define TCollection_Sequence Contap_SequenceOfPathPointOfTheSearch
#define TCollection_Sequence_hxx <Contap_SequenceOfPathPointOfTheSearch.hxx>

#include <TCollection_SequenceNode.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch_HeaderFile
