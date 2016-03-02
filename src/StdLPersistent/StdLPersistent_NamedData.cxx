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

#include <TCollection_HExtendedString.hxx>

#include <TColStd_DataMapOfStringInteger.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>


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
    {
      Standard_Integer aValue = 0;
      aMap.Bind (myInts.Get (i, aValue), aValue);
    }
    theAttribute->ChangeIntegers (aMap);
  }

  if (myReals)
  {
    TDataStd_DataMapOfStringReal aMap;
    for (Standard_Integer i = lower(1); i <= upper(1); i++)
    {
      Standard_Real aValue = 0.0;
      aMap.Bind (myReals.Get (i, aValue), aValue);
    }
    theAttribute->ChangeReals (aMap);
  }

  if (myStrings)
  {
    TDataStd_DataMapOfStringString aMap;
    for (Standard_Integer i = lower(2); i <= upper(2); i++)
    {
      Handle(StdObjMgt_Persistent) aValue;
      aMap.Bind (myStrings.Get (i, aValue),
                 aValue ? aValue->ExtString()->String()
                        : TCollection_ExtendedString());
    }
    theAttribute->ChangeStrings (aMap);
  }

  if (myBytes)
  {
    TDataStd_DataMapOfStringByte aMap;
    for (Standard_Integer i = lower(3); i <= upper(3); i++)
    {
      Standard_Byte aValue = 0;
      aMap.Bind (myBytes.Get (i, aValue), aValue);
    }
    theAttribute->ChangeBytes (aMap);
  }

  if (myIntArrays)
  {
    TDataStd_DataMapOfStringHArray1OfInteger aMap;
    for (Standard_Integer i = lower(4); i <= upper(4); i++)
    {
      Handle(StdObjMgt_Persistent)      aValue;
      const TCollection_ExtendedString& aKey = myIntArrays.Get (i, aValue);

      Handle(StdLPersistent_HArray1::Integer) aPArray =
        Handle(StdLPersistent_HArray1::Integer)::DownCast (aValue);

      Handle(TColStd_HArray1OfInteger) aTArray;
      if (aPArray)
        aTArray = aPArray->Array();

      aMap.Bind (aKey, aTArray);
    }
    theAttribute->ChangeArraysOfIntegers (aMap);
  }

  if (myRealArrays)
  {
    TDataStd_DataMapOfStringHArray1OfReal aMap;
    for (Standard_Integer i = lower(5); i <= upper(5); i++)
    {
      Handle(StdObjMgt_Persistent)      aValue;
      const TCollection_ExtendedString& aKey = myRealArrays.Get (i, aValue);

      Handle(StdLPersistent_HArray1::Real) aPArray =
        Handle(StdLPersistent_HArray1::Real)::DownCast (aValue);

      Handle(TColStd_HArray1OfReal) aTArray;
      if (aPArray)
        aTArray = aPArray->Array();

      aMap.Bind (aKey, aTArray);
    }
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
