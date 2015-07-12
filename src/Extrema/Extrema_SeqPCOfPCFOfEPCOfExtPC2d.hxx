// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Extrema_SeqPCOfPCFOfEPCOfExtPC2d_HeaderFile
#define _Extrema_SeqPCOfPCFOfEPCOfExtPC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BaseSequence.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;
class Extrema_POnCurv2d;
class Extrema_SequenceNodeOfSeqPCOfPCFOfEPCOfExtPC2d;



class Extrema_SeqPCOfPCFOfEPCOfExtPC2d  : public TCollection_BaseSequence
{
public:

  DEFINE_STANDARD_ALLOC

  
    Extrema_SeqPCOfPCFOfEPCOfExtPC2d();
  
  Standard_EXPORT Extrema_SeqPCOfPCFOfEPCOfExtPC2d(const Extrema_SeqPCOfPCFOfEPCOfExtPC2d& Other);
  
  Standard_EXPORT void Clear();
~Extrema_SeqPCOfPCFOfEPCOfExtPC2d()
{
  Clear();
}
  
  Standard_EXPORT const Extrema_SeqPCOfPCFOfEPCOfExtPC2d& Assign (const Extrema_SeqPCOfPCFOfEPCOfExtPC2d& Other);
const Extrema_SeqPCOfPCFOfEPCOfExtPC2d& operator = (const Extrema_SeqPCOfPCFOfEPCOfExtPC2d& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT void Append (const Extrema_POnCurv2d& T);
  
    void Append (Extrema_SeqPCOfPCFOfEPCOfExtPC2d& S);
  
  Standard_EXPORT void Prepend (const Extrema_POnCurv2d& T);
  
    void Prepend (Extrema_SeqPCOfPCFOfEPCOfExtPC2d& S);
  
    void InsertBefore (const Standard_Integer Index, const Extrema_POnCurv2d& T);
  
    void InsertBefore (const Standard_Integer Index, Extrema_SeqPCOfPCFOfEPCOfExtPC2d& S);
  
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Extrema_POnCurv2d& T);
  
    void InsertAfter (const Standard_Integer Index, Extrema_SeqPCOfPCFOfEPCOfExtPC2d& S);
  
  Standard_EXPORT const Extrema_POnCurv2d& First() const;
  
  Standard_EXPORT const Extrema_POnCurv2d& Last() const;
  
    void Split (const Standard_Integer Index, Extrema_SeqPCOfPCFOfEPCOfExtPC2d& Sub);
  
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
#define TCollection_SequenceNode Extrema_SequenceNodeOfSeqPCOfPCFOfEPCOfExtPC2d
#define TCollection_SequenceNode_hxx <Extrema_SequenceNodeOfSeqPCOfPCFOfEPCOfExtPC2d.hxx>
#define Handle_TCollection_SequenceNode Handle(Extrema_SequenceNodeOfSeqPCOfPCFOfEPCOfExtPC2d)
#define TCollection_Sequence Extrema_SeqPCOfPCFOfEPCOfExtPC2d
#define TCollection_Sequence_hxx <Extrema_SeqPCOfPCFOfEPCOfExtPC2d.hxx>

#include <TCollection_Sequence.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Extrema_SeqPCOfPCFOfEPCOfExtPC2d_HeaderFile
