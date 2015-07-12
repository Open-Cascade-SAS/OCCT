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

#ifndef _Contap_SequenceOfSegmentOfTheSearch_HeaderFile
#define _Contap_SequenceOfSegmentOfTheSearch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BaseSequence.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;
class Contap_TheSegmentOfTheSearch;
class Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch;



class Contap_SequenceOfSegmentOfTheSearch  : public TCollection_BaseSequence
{
public:

  DEFINE_STANDARD_ALLOC

  
    Contap_SequenceOfSegmentOfTheSearch();
  
  Standard_EXPORT Contap_SequenceOfSegmentOfTheSearch(const Contap_SequenceOfSegmentOfTheSearch& Other);
  
  Standard_EXPORT void Clear();
~Contap_SequenceOfSegmentOfTheSearch()
{
  Clear();
}
  
  Standard_EXPORT const Contap_SequenceOfSegmentOfTheSearch& Assign (const Contap_SequenceOfSegmentOfTheSearch& Other);
const Contap_SequenceOfSegmentOfTheSearch& operator = (const Contap_SequenceOfSegmentOfTheSearch& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT void Append (const Contap_TheSegmentOfTheSearch& T);
  
    void Append (Contap_SequenceOfSegmentOfTheSearch& S);
  
  Standard_EXPORT void Prepend (const Contap_TheSegmentOfTheSearch& T);
  
    void Prepend (Contap_SequenceOfSegmentOfTheSearch& S);
  
    void InsertBefore (const Standard_Integer Index, const Contap_TheSegmentOfTheSearch& T);
  
    void InsertBefore (const Standard_Integer Index, Contap_SequenceOfSegmentOfTheSearch& S);
  
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Contap_TheSegmentOfTheSearch& T);
  
    void InsertAfter (const Standard_Integer Index, Contap_SequenceOfSegmentOfTheSearch& S);
  
  Standard_EXPORT const Contap_TheSegmentOfTheSearch& First() const;
  
  Standard_EXPORT const Contap_TheSegmentOfTheSearch& Last() const;
  
    void Split (const Standard_Integer Index, Contap_SequenceOfSegmentOfTheSearch& Sub);
  
  Standard_EXPORT const Contap_TheSegmentOfTheSearch& Value (const Standard_Integer Index) const;
const Contap_TheSegmentOfTheSearch& operator() (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Contap_TheSegmentOfTheSearch& I);
  
  Standard_EXPORT Contap_TheSegmentOfTheSearch& ChangeValue (const Standard_Integer Index);
Contap_TheSegmentOfTheSearch& operator() (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  
  Standard_EXPORT void Remove (const Standard_Integer Index);
  
  Standard_EXPORT void Remove (const Standard_Integer FromIndex, const Standard_Integer ToIndex);




protected:





private:





};

#define SeqItem Contap_TheSegmentOfTheSearch
#define SeqItem_hxx <Contap_TheSegmentOfTheSearch.hxx>
#define TCollection_SequenceNode Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch
#define TCollection_SequenceNode_hxx <Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch.hxx>
#define Handle_TCollection_SequenceNode Handle(Contap_SequenceNodeOfSequenceOfSegmentOfTheSearch)
#define TCollection_Sequence Contap_SequenceOfSegmentOfTheSearch
#define TCollection_Sequence_hxx <Contap_SequenceOfSegmentOfTheSearch.hxx>

#include <TCollection_Sequence.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Contap_SequenceOfSegmentOfTheSearch_HeaderFile
