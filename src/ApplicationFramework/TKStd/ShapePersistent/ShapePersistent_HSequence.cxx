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
#include <ShapePersistent_HSequence.hxx>

template <class SequenceClass>
void ShapePersistent_HSequence::node<SequenceClass>::Read(StdObjMgt_ReadData& theReadData)
{
  theReadData >> myPreviuos >> myItem >> myNext;
}

template <class SequenceClass>
void ShapePersistent_HSequence::node<SequenceClass>::Write(StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myPreviuos << myItem << myNext;
}

template <class SequenceClass>
void ShapePersistent_HSequence::instance<SequenceClass>::Read(StdObjMgt_ReadData& theReadData)
{
  theReadData >> myFirst >> myLast >> mySize;
}

template <class SequenceClass>
void ShapePersistent_HSequence::instance<SequenceClass>::Write(
  StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myFirst << myLast << mySize;
}

template <class SequenceClass>
occ::handle<SequenceClass> ShapePersistent_HSequence::instance<SequenceClass>::Import() const
{
  occ::handle<SequenceClass> aSequence = new SequenceClass;

  for (occ::handle<Node> aNode = myFirst; aNode; aNode = aNode->Next())
    aSequence->Append(aNode->Item());

  return aSequence;
}

template class ShapePersistent_HSequence::node<NCollection_HSequence<gp_XYZ>>;
template class ShapePersistent_HSequence::node<NCollection_HSequence<gp_Pnt>>;
template class ShapePersistent_HSequence::node<NCollection_HSequence<gp_Dir>>;
template class ShapePersistent_HSequence::node<NCollection_HSequence<gp_Vec>>;

template class ShapePersistent_HSequence::instance<NCollection_HSequence<gp_XYZ>>;
template class ShapePersistent_HSequence::instance<NCollection_HSequence<gp_Pnt>>;
template class ShapePersistent_HSequence::instance<NCollection_HSequence<gp_Dir>>;
template class ShapePersistent_HSequence::instance<NCollection_HSequence<gp_Vec>>;

//=======================================================================
// XYZ
//=======================================================================
template <>
const char* ShapePersistent_HSequence::instance<NCollection_HSequence<gp_XYZ>>::PName() const
{
  return "PColgp_HSequenceOfXYZ";
}

template <>
const char* ShapePersistent_HSequence::node<NCollection_HSequence<gp_XYZ>>::PName() const
{
  return "PColgp_SeqNodeOfHSequenceOfXYZ";
}

//=======================================================================
// Pnt
//=======================================================================
template <>
const char* ShapePersistent_HSequence::instance<NCollection_HSequence<gp_Pnt>>::PName() const
{
  return "PColgp_HSequenceOfPnt";
}

template <>
const char* ShapePersistent_HSequence::node<NCollection_HSequence<gp_Pnt>>::PName() const
{
  return "PColgp_SeqNodeOfHSequenceOfPnt";
}

//=======================================================================
// Dir
//=======================================================================
template <>
const char* ShapePersistent_HSequence::instance<NCollection_HSequence<gp_Dir>>::PName() const
{
  return "PColgp_HSequenceOfDir";
}

template <>
const char* ShapePersistent_HSequence::node<NCollection_HSequence<gp_Dir>>::PName() const
{
  return "PColgp_SeqNodeOfHSequenceOffDir";
}

//=======================================================================
// Vec
//=======================================================================
template <>
const char* ShapePersistent_HSequence::instance<NCollection_HSequence<gp_Vec>>::PName() const
{
  return "PColgp_HSequenceOfVec";
}

template <>
const char* ShapePersistent_HSequence::node<NCollection_HSequence<gp_Vec>>::PName() const
{
  return "PColgp_SeqNodeOfHSequenceOfVec";
}
