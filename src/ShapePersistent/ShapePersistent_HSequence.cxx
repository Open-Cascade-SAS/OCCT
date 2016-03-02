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

#include <ShapePersistent_HSequence.hxx>
#include <StdObjMgt_ReadData.hxx>


template <class SequenceClass>
void ShapePersistent_HSequence::node<SequenceClass>::Read
  (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myPreviuos >> myItem >> myNext;
}

template <class SequenceClass>
void ShapePersistent_HSequence::instance<SequenceClass>::Read
  (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myFirst >> myLast >> mySize;
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
