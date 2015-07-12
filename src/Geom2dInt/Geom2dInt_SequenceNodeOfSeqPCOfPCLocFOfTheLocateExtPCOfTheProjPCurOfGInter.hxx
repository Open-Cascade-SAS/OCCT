// Created on: 1992-06-04
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

#ifndef _Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Extrema_POnCurv2d.hxx>
#include <TCollection_SeqNode.hxx>
#include <TCollection_SeqNodePtr.hxx>
class Extrema_POnCurv2d;
class Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter;


class Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter;
DEFINE_STANDARD_HANDLE(Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter, TCollection_SeqNode)


class Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter : public TCollection_SeqNode
{

public:

  
    Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter(const Extrema_POnCurv2d& I, const TCollection_SeqNodePtr& n, const TCollection_SeqNodePtr& p);
  
    Extrema_POnCurv2d& Value() const;




  DEFINE_STANDARD_RTTI(Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter,TCollection_SeqNode)

protected:




private:


  Extrema_POnCurv2d myValue;


};

#define SeqItem Extrema_POnCurv2d
#define SeqItem_hxx <Extrema_POnCurv2d.hxx>
#define TCollection_SequenceNode Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter
#define TCollection_SequenceNode_hxx <Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define Handle_TCollection_SequenceNode Handle(Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter)
#define TCollection_Sequence Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter
#define TCollection_Sequence_hxx <Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>

#include <TCollection_SequenceNode.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
