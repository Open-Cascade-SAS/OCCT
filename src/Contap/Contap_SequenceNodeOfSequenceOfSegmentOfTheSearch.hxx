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

#ifndef _Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch_HeaderFile
#define _Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Contap_TheSegmentOfTheSearch.hxx>
#include <TCollection_SeqNode.hxx>
#include <TCollection_SeqNodePtr.hxx>
class Contap_TheSegmentOfTheSearch;
class Contap_SequenceOfSegmentOfTheSearch;


class Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch;
DEFINE_STANDARD_HANDLE(Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch, TCollection_SeqNode)


class Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch : public TCollection_SeqNode
{

public:

  
    Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch(const Contap_TheSegmentOfTheSearch& I, const TCollection_SeqNodePtr& n, const TCollection_SeqNodePtr& p);
  
    Contap_TheSegmentOfTheSearch& Value() const;




  DEFINE_STANDARD_RTTI(Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch,TCollection_SeqNode)

protected:




private:


  Contap_TheSegmentOfTheSearch myValue;


};

#define SeqItem Contap_TheSegmentOfTheSearch
#define SeqItem_hxx <Contap_TheSegmentOfTheSearch.hxx>
#define TCollection_SequenceNode Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch
#define TCollection_SequenceNode_hxx <Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch.hxx>
#define Handle_TCollection_SequenceNode Handle(Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch)
#define TCollection_Sequence Contap_SequenceOfSegmentOfTheSearch
#define TCollection_Sequence_hxx <Contap_SequenceOfSegmentOfTheSearch.hxx>

#include <TCollection_SequenceNode.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch_HeaderFile
