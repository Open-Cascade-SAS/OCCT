// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>
#include <ShapePersistent_HSequence.hxx>


template <class SequenceClass>
void ShapePersistent_HSequence::node<SequenceClass>::Read
  (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myPreviuos >> myItem >> myNext;
}

template <class SequenceClass>
void ShapePersistent_HSequence::node<SequenceClass>::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myPreviuos << myItem << myNext;
}

template <class SequenceClass>
void ShapePersistent_HSequence::instance<SequenceClass>::Read
  (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myFirst >> myLast >> mySize;
}

template <class SequenceClass>
void ShapePersistent_HSequence::instance<SequenceClass>::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myFirst << myLast << mySize;
}

template <class SequenceClass>
Handle(SequenceClass)
  ShapePersistent_HSequence::instance<SequenceClass>::Import() const
{
  Handle(SequenceClass) aSequence = new SequenceClass;
  
  for (Handle(Node) aNode = myFirst; aNode; aNode = aNode->Next())
    aSequence->Append (aNode->Item());

  return aSequence;
}

template class ShapePersistent_HSequence::node<TColgp_HSequenceOfXYZ>;
template class ShapePersistent_HSequence::node<TColgp_HSequenceOfPnt>;
template class ShapePersistent_HSequence::node<TColgp_HSequenceOfDir>;
template class ShapePersistent_HSequence::node<TColgp_HSequenceOfVec>;

template class ShapePersistent_HSequence::instance<TColgp_HSequenceOfXYZ>;
template class ShapePersistent_HSequence::instance<TColgp_HSequenceOfPnt>;
template class ShapePersistent_HSequence::instance<TColgp_HSequenceOfDir>;
template class ShapePersistent_HSequence::instance<TColgp_HSequenceOfVec>;


//=======================================================================
// XYZ 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfXYZ>
  ::PName() const { return "PColgp_HSequenceOfXYZ"; }

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfXYZ>
  ::PName() const { return "PColgp_SeqNodeOfHSequenceOfXYZ"; }

//=======================================================================
// Pnt 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfPnt>
  ::PName() const { return "PColgp_HSequenceOfPnt"; }

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfPnt>
  ::PName() const { return "PColgp_SeqNodeOfHSequenceOfPnt"; }

//=======================================================================
// Dir 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfDir>
  ::PName() const { return "PColgp_HSequenceOfDir"; }

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfDir>
  ::PName() const { return "PColgp_SeqNodeOfHSequenceOffDir"; }

//=======================================================================
// Vec 
//=======================================================================
template<>
Standard_CString ShapePersistent_HSequence::instance<TColgp_HSequenceOfVec>
  ::PName() const { return "PColgp_HSequenceOfVec"; }

template<>
Standard_CString ShapePersistent_HSequence::node<TColgp_HSequenceOfVec>
  ::PName() const { return "PColgp_SeqNodeOfHSequenceOfVec"; }
