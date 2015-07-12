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

#ifndef _IntPatch_SequenceOfIWLineOfTheIWalking_HeaderFile
#define _IntPatch_SequenceOfIWLineOfTheIWalking_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BaseSequence.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;
class IntPatch_TheIWLineOfTheIWalking;
class IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking;



class IntPatch_SequenceOfIWLineOfTheIWalking  : public TCollection_BaseSequence
{
public:

  DEFINE_STANDARD_ALLOC

  
    IntPatch_SequenceOfIWLineOfTheIWalking();
  
  Standard_EXPORT IntPatch_SequenceOfIWLineOfTheIWalking(const IntPatch_SequenceOfIWLineOfTheIWalking& Other);
  
  Standard_EXPORT void Clear();
~IntPatch_SequenceOfIWLineOfTheIWalking()
{
  Clear();
}
  
  Standard_EXPORT const IntPatch_SequenceOfIWLineOfTheIWalking& Assign (const IntPatch_SequenceOfIWLineOfTheIWalking& Other);
const IntPatch_SequenceOfIWLineOfTheIWalking& operator = (const IntPatch_SequenceOfIWLineOfTheIWalking& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT void Append (const Handle(IntPatch_TheIWLineOfTheIWalking)& T);
  
    void Append (IntPatch_SequenceOfIWLineOfTheIWalking& S);
  
  Standard_EXPORT void Prepend (const Handle(IntPatch_TheIWLineOfTheIWalking)& T);
  
    void Prepend (IntPatch_SequenceOfIWLineOfTheIWalking& S);
  
    void InsertBefore (const Standard_Integer Index, const Handle(IntPatch_TheIWLineOfTheIWalking)& T);
  
    void InsertBefore (const Standard_Integer Index, IntPatch_SequenceOfIWLineOfTheIWalking& S);
  
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Handle(IntPatch_TheIWLineOfTheIWalking)& T);
  
    void InsertAfter (const Standard_Integer Index, IntPatch_SequenceOfIWLineOfTheIWalking& S);
  
  Standard_EXPORT const Handle(IntPatch_TheIWLineOfTheIWalking)& First() const;
  
  Standard_EXPORT const Handle(IntPatch_TheIWLineOfTheIWalking)& Last() const;
  
    void Split (const Standard_Integer Index, IntPatch_SequenceOfIWLineOfTheIWalking& Sub);
  
  Standard_EXPORT const Handle(IntPatch_TheIWLineOfTheIWalking)& Value (const Standard_Integer Index) const;
const Handle(IntPatch_TheIWLineOfTheIWalking)& operator() (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Handle(IntPatch_TheIWLineOfTheIWalking)& I);
  
  Standard_EXPORT Handle(IntPatch_TheIWLineOfTheIWalking)& ChangeValue (const Standard_Integer Index);
Handle(IntPatch_TheIWLineOfTheIWalking)& operator() (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  
  Standard_EXPORT void Remove (const Standard_Integer Index);
  
  Standard_EXPORT void Remove (const Standard_Integer FromIndex, const Standard_Integer ToIndex);




protected:





private:





};

#define SeqItem Handle(IntPatch_TheIWLineOfTheIWalking)
#define SeqItem_hxx <IntPatch_TheIWLineOfTheIWalking.hxx>
#define TCollection_SequenceNode IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking
#define TCollection_SequenceNode_hxx <IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking.hxx>
#define Handle_TCollection_SequenceNode Handle(IntPatch_SequenceNodeOfSequenceOfIWLineOfTheIWalking)
#define TCollection_Sequence IntPatch_SequenceOfIWLineOfTheIWalking
#define TCollection_Sequence_hxx <IntPatch_SequenceOfIWLineOfTheIWalking.hxx>

#include <TCollection_Sequence.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _IntPatch_SequenceOfIWLineOfTheIWalking_HeaderFile
