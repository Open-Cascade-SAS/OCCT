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

#include <StdLPersistent_NamedData.hxx>

#include <TColStd_DataMapOfStringInteger.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>


static const TCollection_ExtendedString&
  String (Handle(StdObjMgt_Persistent) theValue)
{
  if (theValue)
    return theValue->ExtString()->String();

  static TCollection_ExtendedString anEmptyString;
  return anEmptyString;
}

template <class HArray>
static typename HArray::ArrayHandle
  Array (Handle(StdObjMgt_Persistent) theValue)
{
  Handle(HArray) anArray = Handle(HArray)::DownCast (theValue);
  return anArray ? anArray->Array() : NULL;
}

//=======================================================================
//function : Import
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
void StdLPersistent_NamedData::Import
  (const Handle(TDataStd_NamedData)& theAttribute) const
{
  if (myDimensions.IsNull())
    return;

  if (myInts)
  {
    TColStd_DataMapOfStringInteger aMap;
    for (Standard_Integer i = lower(0); i <= upper(0); i++)
      aMap.Bind (myInts.Key(i), myInts.Value(i));

    theAttribute->ChangeIntegers (aMap);
  }

  if (myReals)
  {
    TDataStd_DataMapOfStringReal aMap;
    for (Standard_Integer i = lower(1); i <= upper(1); i++)
      aMap.Bind (myReals.Key(i), myReals.Value(i));

    theAttribute->ChangeReals (aMap);
  }

  if (myStrings)
  {
    TDataStd_DataMapOfStringString aMap;
    for (Standard_Integer i = lower(2); i <= upper(2); i++)
      aMap.Bind (myStrings.Key(i), String (myStrings.Value(i)));

    theAttribute->ChangeStrings (aMap);
  }

  if (myBytes)
  {
    TDataStd_DataMapOfStringByte aMap;
    for (Standard_Integer i = lower(3); i <= upper(3); i++)
      aMap.Bind (myBytes.Key(i), myBytes.Value(i));

    theAttribute->ChangeBytes (aMap);
  }

  if (myIntArrays)
  {
    TDataStd_DataMapOfStringHArray1OfInteger aMap;
    for (Standard_Integer i = lower(4); i <= upper(4); i++)
      aMap.Bind (myIntArrays.Key(i),
                 Array<StdLPersistent_HArray1::Integer> (myIntArrays.Value(i)));

    theAttribute->ChangeArraysOfIntegers (aMap);
  }

  if (myRealArrays)
  {
    TDataStd_DataMapOfStringHArray1OfReal aMap;
    for (Standard_Integer i = lower(5); i <= upper(5); i++)
      aMap.Bind (myRealArrays.Key(i),
                 Array<StdLPersistent_HArray1::Real> (myRealArrays.Value(i)));

    theAttribute->ChangeArraysOfReals (aMap);
  }
}

Standard_Integer
  StdLPersistent_NamedData::lower (Standard_Integer theIndex) const
{
  const Handle(TColStd_HArray2OfInteger)& aDimensions = myDimensions->Array();
  return aDimensions->Value (aDimensions->LowerRow() + theIndex,
                             aDimensions->LowerCol());
}

Standard_Integer
  StdLPersistent_NamedData::upper (Standard_Integer theIndex) const
{
  const Handle(TColStd_HArray2OfInteger)& aDimensions = myDimensions->Array();
  return aDimensions->Value (aDimensions->LowerRow() + theIndex,
                             aDimensions->UpperCol());
}
