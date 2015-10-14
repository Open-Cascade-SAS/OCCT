// Created on: 1992-10-14
// Created by: Christophe MARION
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

#ifndef _HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter_HeaderFile
#define _HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BaseSequence.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;
class Extrema_POnCurv2d;
class HLRBRep_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter;



class HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter  : public TCollection_BaseSequence
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty sequence.
  //! Use:
  //! -   the function Append or Prepend to add an item or
  //! a collection of items at the end, or at the beginning of the sequence,
  //! -   the function InsertAfter or InsertBefore to add an
  //! item or a collection of items at any position in the sequence,
  //! -   operator() or the function SetValue to assign a
  //! new value to an item of the sequence,
  //! -   operator() to read an item of the sequence,
  //! -   the function Remove to remove an item at any
  //! position in the sequence.
  //! Warning
  //! To copy a sequence, you must explicitly call the
  //! assignment operator (operator=).
    HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter();
  
  //! Creation by copy of existing Sequence.
  Standard_EXPORT HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter(const HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& Other);
  
  //! Removes all element(s) of the sequence <me>
  //! Example:
  //! before
  //! me = (A B C)
  //! after
  //! me = ()
  Standard_EXPORT void Clear();
~HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter()
{
  Clear();
}
  
  //! Copies the contents of the sequence Other into this sequence.
  //! If this sequence is not empty, it is automatically cleared before the copy.
  Standard_EXPORT const HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& Assign (const HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& Other);
const HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& operator = (const HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& Other)
{
  return Assign(Other);
}
  
  //! Appends <T> at the end of <me>.
  //! Example:
  //! before
  //! me = (A B C)
  //! after
  //! me = (A B C T)
  Standard_EXPORT void Append (const Extrema_POnCurv2d& T);
  
  //! Concatenates <S> at the end of <me>.
  //! <S> is cleared.
  //! Example:
  //! before
  //! me = (A B C)
  //! S  = (D E F)
  //! after
  //! me = (A B C D E F)
  //! S  = ()
    void Append (HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& S);
  
  //! Add <T> at the beginning of <me>.
  //! Example:
  //! before
  //! me = (A B C)
  //! after
  //! me = (T A B C )
  Standard_EXPORT void Prepend (const Extrema_POnCurv2d& T);
  
  //! Concatenates <S> at the beginning of <me>.
  //! <S> is cleared.
  //! Example:
  //! before
  //! me = (A B C) S =  (D E F)
  //! after me = (D E F A B C)
  //! S = ()
    void Prepend (HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& S);
  
  //! Inserts  <T> in  <me>  before the position <Index>.
  //! Raises an exception if the index is out of bounds.
  //! Example:
  //! before
  //! me = (A B D), Index = 3, T = C
  //! after
  //! me = (A B C D )
    void InsertBefore (const Standard_Integer Index, const Extrema_POnCurv2d& T);
  
  //! Inserts the  sequence <S>  in  <me> before
  //! the position <Index>. <S> is cleared.
  //! Raises an exception if the index is out of bounds
  //! Example:
  //! before
  //! me = (A B F), Index = 3, S = (C D E)
  //! after
  //! me = (A B C D E F)
  //! S  = ()
    void InsertBefore (const Standard_Integer Index, HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& S);
  
  //! Inserts  <T>  in  <me> after the  position <Index>.
  //! Raises an exception if the index is out of bound
  //! Example:
  //! before
  //! me = (A B C), Index = 3, T = D
  //! after
  //! me = (A B C D)
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Extrema_POnCurv2d& T);
  
  //! Inserts the sequence <S> in <me> after the
  //! position <Index>. <S> is cleared.
  //! Raises an exception if the index is out of bound.
  //! Example:
  //! before
  //! me = (A B C), Index = 3, S = (D E F)
  //! after
  //! me = (A B C D E F)
  //! S  = ()
    void InsertAfter (const Standard_Integer Index, HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& S);
  
  //! Returns the first element of the sequence <me>
  //! Raises an exception if the sequence is empty.
  //! Example:
  //! before
  //! me = (A B C)
  //! after
  //! me = (A B C)
  //! returns A
  Standard_EXPORT const Extrema_POnCurv2d& First() const;
  
  //! Returns the last element of the sequence <me>
  //! Raises an exception if the sequence is empty.
  //! Example:
  //! before
  //! me = (A B C)
  //! after
  //! me = (A B C)
  //! returns C
  Standard_EXPORT const Extrema_POnCurv2d& Last() const;
  
  //! Keeps in <me> the items 1 to <Index>-1 and
  //! puts  in  <Sub> the  items <Index>  to the end.
  //! Example:
  //! before
  //! me = (A B C D) ,Index = 3
  //! after
  //! me  = (A B)
  //! Sub = (C D)
    void Split (const Standard_Integer Index, HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter& Sub);
  
  //! Returns  the Item  at position <Index>  in <me>.
  //! Raises an exception if the index is out of bound
  //! Example:
  //! before
  //! me = (A B C), Index = 1
  //! after
  //! me = (A B C)
  //! returns
  //! A
  Standard_EXPORT const Extrema_POnCurv2d& Value (const Standard_Integer Index) const;
const Extrema_POnCurv2d& operator() (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  //! Changes the item at position <Index>
  //! Raises an exception if the index is out of bound
  //! Example:
  //! before
  //! me = (A B C), Index = 1, Item = D
  //! after
  //! me = (D B C)
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Extrema_POnCurv2d& I);
  
  //! Returns  the Item  at position <Index>  in
  //! <me>. This method  may  be  used to modify
  //! <me> : S.Value(Index) = Item.
  //! Raises an exception if the index is out of bound
  //! Example:
  //! before
  //! me = (A B C), Index = 1
  //! after
  //! me = (A B C)
  //! returns
  //! A
  Standard_EXPORT Extrema_POnCurv2d& ChangeValue (const Standard_Integer Index);
Extrema_POnCurv2d& operator() (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  
  //! Removes  from  <me> the  item at  position <Index>.
  //! Raises an exception if the index is out of bounds
  //! Example:
  //! before
  //! me = (A B C), Index = 3
  //! after
  //! me = (A B)
  Standard_EXPORT void Remove (const Standard_Integer Index);
  
  //! Removes  from  <me>    all  the  items  of
  //! positions between <FromIndex> and <ToIndex>.
  //! Raises an exception if the indices are out of bounds.
  //! Example:
  //! before
  //! me = (A B C D E F), FromIndex = 1 ToIndex = 3
  //! after
  //! me = (D E F)
  Standard_EXPORT void Remove (const Standard_Integer FromIndex, const Standard_Integer ToIndex);




protected:





private:





};

#define SeqItem Extrema_POnCurv2d
#define SeqItem_hxx <Extrema_POnCurv2d.hxx>
#define TCollection_SequenceNode HLRBRep_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter
#define TCollection_SequenceNode_hxx <HLRBRep_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter.hxx>
#define Handle_TCollection_SequenceNode Handle(HLRBRep_SequenceNodeOfSeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter)
#define TCollection_Sequence HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter
#define TCollection_Sequence_hxx <HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter.hxx>

#include <TCollection_Sequence.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter_HeaderFile
