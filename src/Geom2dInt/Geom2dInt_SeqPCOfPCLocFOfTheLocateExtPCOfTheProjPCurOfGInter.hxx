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

#ifndef _Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BaseSequence.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;
class Extrema_POnCurv2d;
class Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter;



class Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter  : public TCollection_BaseSequence
{
public:

  DEFINE_STANDARD_ALLOC

  
    Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter();
  
  Standard_EXPORT Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter(const Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& Other);
  
  Standard_EXPORT void Clear();
~Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter()
{
  Clear();
}
  
  Standard_EXPORT const Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& Assign (const Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& Other);
const Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& operator = (const Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT void Append (const Extrema_POnCurv2d& T);
  
    void Append (Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& S);
  
  Standard_EXPORT void Prepend (const Extrema_POnCurv2d& T);
  
    void Prepend (Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& S);
  
    void InsertBefore (const Standard_Integer Index, const Extrema_POnCurv2d& T);
  
    void InsertBefore (const Standard_Integer Index, Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& S);
  
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Extrema_POnCurv2d& T);
  
    void InsertAfter (const Standard_Integer Index, Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& S);
  
  Standard_EXPORT const Extrema_POnCurv2d& First() const;
  
  Standard_EXPORT const Extrema_POnCurv2d& Last() const;
  
    void Split (const Standard_Integer Index, Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter& Sub);
  
  Standard_EXPORT const Extrema_POnCurv2d& Value (const Standard_Integer Index) const;
const Extrema_POnCurv2d& operator() (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Extrema_POnCurv2d& I);
  
  Standard_EXPORT Extrema_POnCurv2d& ChangeValue (const Standard_Integer Index);
Extrema_POnCurv2d& operator() (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  
  Standard_EXPORT void Remove (const Standard_Integer Index);
  
  Standard_EXPORT void Remove (const Standard_Integer FromIndex, const Standard_Integer ToIndex);




protected:





private:





};

#define SeqItem Extrema_POnCurv2d
#define SeqItem_hxx <Extrema_POnCurv2d.hxx>
#define TCollection_SequenceNode Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter
#define TCollection_SequenceNode_hxx <Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define Handle_TCollection_SequenceNode Handle(Geom2dInt_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter)
#define TCollection_Sequence Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter
#define TCollection_Sequence_hxx <Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>

#include <TCollection_Sequence.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
