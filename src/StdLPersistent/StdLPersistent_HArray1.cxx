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

#include <StdLPersistent_HArray1.hxx>
#include <StdObjMgt_ReadData.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_HArray1::commonBase::Read (StdObjMgt_ReadData& theReadData)
{
  Value<Standard_Integer> aLowerBound, anUpperBound;
  theReadData >> aLowerBound >> anUpperBound;
  createArray (aLowerBound, anUpperBound);

  theReadData.Driver().BeginReadObjectData();

  Standard_Integer aSize;
  theReadData.ReadValue (aSize);

  for (Standard_Integer i = aLowerBound; i <= anUpperBound; i++)
    readValue (theReadData, i);

  theReadData.Driver().EndReadObjectData();
}

template <class ArrayClass, class ValueClass>
void StdLPersistent_HArray1::instance<ArrayClass, ValueClass>::readValue (
  StdObjMgt_ReadData& theReadData,
  const Standard_Integer theIndex)
{
  ValueClass aValue;
  theReadData >> aValue;
  this->myArray->SetValue (theIndex, static_cast<typename ArrayClass::value_type> (aValue));
}


template class StdLPersistent_HArray1::instance<TColStd_HArray1OfInteger>;
template class StdLPersistent_HArray1::instance<TColStd_HArray1OfReal>;
template class StdLPersistent_HArray1::instance<TColStd_HArray1OfByte,
  StdObjMgt_ContentTypes::Value<Standard_Character> >;
template class StdLPersistent_HArray1::instance<StdLPersistent_HArray1OfPersistent,
  StdObjMgt_ContentTypes::Reference<> >;
