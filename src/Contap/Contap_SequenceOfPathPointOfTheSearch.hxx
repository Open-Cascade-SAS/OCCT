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

#ifndef _Contap_SequenceOfPathPointOfTheSearch_HeaderFile
#define _Contap_SequenceOfPathPointOfTheSearch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BaseSequence.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class Standard_OutOfRange;
class Contap_ThePathPointOfTheSearch;
class Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch;



class Contap_SequenceOfPathPointOfTheSearch  : public TCollection_BaseSequence
{
public:

  DEFINE_STANDARD_ALLOC

  
    Contap_SequenceOfPathPointOfTheSearch();
  
  Standard_EXPORT Contap_SequenceOfPathPointOfTheSearch(const Contap_SequenceOfPathPointOfTheSearch& Other);
  
  Standard_EXPORT void Clear();
~Contap_SequenceOfPathPointOfTheSearch()
{
  Clear();
}
  
  Standard_EXPORT const Contap_SequenceOfPathPointOfTheSearch& Assign (const Contap_SequenceOfPathPointOfTheSearch& Other);
const Contap_SequenceOfPathPointOfTheSearch& operator = (const Contap_SequenceOfPathPointOfTheSearch& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT void Append (const Contap_ThePathPointOfTheSearch& T);
  
    void Append (Contap_SequenceOfPathPointOfTheSearch& S);
  
  Standard_EXPORT void Prepend (const Contap_ThePathPointOfTheSearch& T);
  
    void Prepend (Contap_SequenceOfPathPointOfTheSearch& S);
  
    void InsertBefore (const Standard_Integer Index, const Contap_ThePathPointOfTheSearch& T);
  
    void InsertBefore (const Standard_Integer Index, Contap_SequenceOfPathPointOfTheSearch& S);
  
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Contap_ThePathPointOfTheSearch& T);
  
    void InsertAfter (const Standard_Integer Index, Contap_SequenceOfPathPointOfTheSearch& S);
  
  Standard_EXPORT const Contap_ThePathPointOfTheSearch& First() const;
  
  Standard_EXPORT const Contap_ThePathPointOfTheSearch& Last() const;
  
    void Split (const Standard_Integer Index, Contap_SequenceOfPathPointOfTheSearch& Sub);
  
  Standard_EXPORT const Contap_ThePathPointOfTheSearch& Value (const Standard_Integer Index) const;
const Contap_ThePathPointOfTheSearch& operator() (const Standard_Integer Index) const
{
  return Value(Index);
}
  
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Contap_ThePathPointOfTheSearch& I);
  
  Standard_EXPORT Contap_ThePathPointOfTheSearch& ChangeValue (const Standard_Integer Index);
Contap_ThePathPointOfTheSearch& operator() (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  
  Standard_EXPORT void Remove (const Standard_Integer Index);
  
  Standard_EXPORT void Remove (const Standard_Integer FromIndex, const Standard_Integer ToIndex);




protected:





private:





};

#define SeqItem Contap_ThePathPointOfTheSearch
#define SeqItem_hxx <Contap_ThePathPointOfTheSearch.hxx>
#define TCollection_SequenceNode Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch
#define TCollection_SequenceNode_hxx <Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch.hxx>
#define Handle_TCollection_SequenceNode Handle(Contap_SequenceNodeOfSequenceOfPathPointOfTheSearch)
#define TCollection_Sequence Contap_SequenceOfPathPointOfTheSearch
#define TCollection_Sequence_hxx <Contap_SequenceOfPathPointOfTheSearch.hxx>

#include <TCollection_Sequence.lxx>

#undef SeqItem
#undef SeqItem_hxx
#undef TCollection_SequenceNode
#undef TCollection_SequenceNode_hxx
#undef Handle_TCollection_SequenceNode
#undef TCollection_Sequence
#undef TCollection_Sequence_hxx




#endif // _Contap_SequenceOfPathPointOfTheSearch_HeaderFile
