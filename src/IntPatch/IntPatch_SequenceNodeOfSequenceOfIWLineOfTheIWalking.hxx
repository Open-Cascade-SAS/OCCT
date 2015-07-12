// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking_HeaderFile
#define _IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_SeqNode.hxx>
#include <TCollection_SeqNodePtr.hxx>
class IntPatch_TheIWLineOfTheIWalking;
class IntPatch_SequenceOfIWLineOfTheIWalking;


class IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking;
DEFINE_STANDARD_HANDLE(IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking, TCollection_SeqNode)


class IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking : public TCollection_SeqNode
{

public:

  
    IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking(const Handle(IntPatch_TheIWLineOfTheIWalking)& I, const TCollection_SeqNodePtr& n, const TCollection_SeqNodePtr& p);
  
    Handle(IntPatch_TheIWLineOfTheIWalking)& Value() const;




  DEFINE_STANDARD_RTTI(IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking,TCollection_SeqNode)

protected:




private:


  Handle(IntPatch_TheIWLineOfTheIWalking) myValue;


};

#define SeqItem Handle(IntPatch_TheIWLineOfTheIWalking)
#define SeqItem_hxx <IntPatch_TheIWLineOfTheIWalking.hxx>
#define TCollection_SequenceNode IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking
#define TCollection_SequenceNode_hxx <IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking.hxx>
#define Handle_TCollection_SequenceNode Handle(IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking)
#define TCollection_Sequence IntPatch_SequenceOfIWLineOfTheIWalking
#define TCollection_Sequence_hxx <IntPatch_SequenceOfIWLineOfTheIWalking.hxx>

#include <TCollection_SequenceNode.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking_HeaderFile
