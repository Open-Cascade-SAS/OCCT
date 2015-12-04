// Created on: 2007-07-02
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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


#include <BinMDataStd_NamedDataDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfStringInteger.hxx>
#include <TColStd_DataMapOfStringInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringByte.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringString.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_NamedDataDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_NamedDataDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_NamedDataDriver::BinMDataStd_NamedDataDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_NamedData)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_NamedDataDriver::NewEmpty() const
{
  return new TDataStd_NamedData();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_NamedDataDriver::Paste(const BinObjMgt_Persistent&  theSource,
						      const Handle(TDF_Attribute)& theTarget,
						      BinObjMgt_RRelocationTable&  ) const
{

  Handle(TDataStd_NamedData) T = Handle(TDataStd_NamedData)::DownCast(theTarget);
  if(T.IsNull()) return Standard_False;
  Standard_Integer aLower, anUpper,i;
  if (! (theSource >> aLower >> anUpper))
    return Standard_False;
//  const Standard_Integer aLength = anUpper - aLower + 1;
  if (anUpper < aLower)  return Standard_False;
  if(anUpper | aLower) {
    TColStd_DataMapOfStringInteger anIntegers;
    for (i=aLower; i<=anUpper; i++) {
      TCollection_ExtendedString aKey;
      Standard_Integer aValue;
      if (! (theSource >> aKey >> aValue))
        return Standard_False;
      anIntegers.Bind(aKey, aValue);
    }
    T->ChangeIntegers(anIntegers);
  }

  if (! (theSource >> aLower >> anUpper))
    return Standard_False;
  if (anUpper < aLower)  return Standard_False;
  if(anUpper | aLower) {
    TDataStd_DataMapOfStringReal aReals;
    for (i=aLower; i<=anUpper; i++) {
      TCollection_ExtendedString aKey;
      Standard_Real aValue;
      if (! (theSource >> aKey >> aValue))
        return Standard_False;
      aReals.Bind(aKey, aValue);
    }
    T->ChangeReals(aReals);
  }

// strings
  if (! (theSource >> aLower >> anUpper))
    return Standard_False;
  if (anUpper < aLower)  return Standard_False;
  if(anUpper | aLower) {
    TDataStd_DataMapOfStringString aStrings;
    for (i=aLower; i<=anUpper; i++) {
      TCollection_ExtendedString aKey;
      TCollection_ExtendedString aValue;
      if (! (theSource >> aKey >> aValue))
        return Standard_False;
      aStrings.Bind(aKey, aValue);
    }
    T->ChangeStrings(aStrings);
  }

//Bytes
  if (! (theSource >> aLower >> anUpper))
    return Standard_False;
  if (anUpper < aLower)  return Standard_False;
  if(anUpper | aLower) {
    TDataStd_DataMapOfStringByte aBytes;
    for (i=aLower; i<=anUpper; i++) {
      TCollection_ExtendedString aKey;
      Standard_Byte aValue;
      if (! (theSource >> aKey >> aValue))
        return Standard_False;
      aBytes.Bind(aKey, (Standard_Byte)aValue);
    }
    T->ChangeBytes(aBytes);
  }

// arrays of integers
  if (! (theSource >> aLower >> anUpper))
    return Standard_False;
  if (anUpper < aLower)  return Standard_False;
  Standard_Boolean aResult = Standard_False;
  if(anUpper | aLower) {
    TDataStd_DataMapOfStringHArray1OfInteger anIntArrays;
    for (i=aLower; i<=anUpper; i++) {
      TCollection_ExtendedString aKey;
      if (! (theSource >> aKey))
        return Standard_False;
      Standard_Integer low, up;
      if (! (theSource >> low >> up))
        return Standard_False;
      if(up < low)
        return Standard_False;
      if(up | low) {
	Handle(TColStd_HArray1OfInteger) aTargetArray = new TColStd_HArray1OfInteger (low, up);
	if(!theSource.GetIntArray (&(aTargetArray->ChangeArray1())(low), up-low+1))
	  return Standard_False;

	Standard_Boolean Ok = anIntArrays.Bind(aKey, aTargetArray);
	aResult |= Ok;
      }
    }
    if (aResult)
      T->ChangeArraysOfIntegers(anIntArrays);
  }

// arrays of reals
  if (! (theSource >> aLower >> anUpper))
    return Standard_False;
  if (anUpper < aLower)  return Standard_False;
  aResult = Standard_False;
  if(anUpper | aLower) {
    TDataStd_DataMapOfStringHArray1OfReal aRealArrays;
    for (i=aLower; i<=anUpper; i++) {
      TCollection_ExtendedString aKey;
      if (! (theSource >> aKey))
        return Standard_False;
      Standard_Integer low, up;
      if (! (theSource >> low >> up))
        return Standard_False;
      if (up < low)
        return Standard_False;
      if(low | up) {
        Handle(TColStd_HArray1OfReal) aTargetArray =
          new TColStd_HArray1OfReal(low, up);
        if(!theSource.GetRealArray (&(aTargetArray->ChangeArray1())(low), up-low+1))
          return Standard_False;
        Standard_Boolean Ok = aRealArrays.Bind(aKey, aTargetArray);
        aResult |= Ok;
      }
    }
    if(aResult)
      T->ChangeArraysOfReals(aRealArrays);
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_NamedDataDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_NamedData) S = Handle(TDataStd_NamedData)::DownCast (theSource);
  if(S.IsNull()) return;
//  Standard_Integer i=0;

  if(S->HasIntegers() && !S->GetIntegersContainer().IsEmpty()) {
    theTarget.PutInteger(1) << S->GetIntegersContainer().Extent(); //dim
    TColStd_DataMapIteratorOfDataMapOfStringInteger itr(S->GetIntegersContainer());
    for (; itr.More(); itr.Next()) {
      theTarget << itr.Key() << itr.Value(); // key - value;
    }
  } else {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if(S->HasReals() && !S->GetRealsContainer().IsEmpty()) {
    theTarget.PutInteger(1) << S->GetRealsContainer().Extent();
    TDataStd_DataMapIteratorOfDataMapOfStringReal itr(S->GetRealsContainer());
    for (; itr.More(); itr.Next()) {
      theTarget << itr.Key() << itr.Value();
    }
  } else {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if(S->HasStrings() && !S->GetStringsContainer().IsEmpty()) {
    theTarget.PutInteger(1) << S->GetStringsContainer().Extent();
    TDataStd_DataMapIteratorOfDataMapOfStringString itr(S->GetStringsContainer());
    for (; itr.More(); itr.Next()) {
      theTarget << itr.Key() << itr.Value();
    }
  } else {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if(S->HasBytes() && !S->GetBytesContainer().IsEmpty()) {
    theTarget.PutInteger(1) << S->GetBytesContainer().Extent();
    TDataStd_DataMapIteratorOfDataMapOfStringByte itr(S->GetBytesContainer());
    for (; itr.More(); itr.Next()) {
      theTarget << itr.Key() << (Standard_Byte) itr.Value();
    }
  } else {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if(S->HasArraysOfIntegers() && !S->GetArraysOfIntegersContainer().IsEmpty()) {
    theTarget.PutInteger(1) << S->GetArraysOfIntegersContainer().Extent();
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger
      itr(S->GetArraysOfIntegersContainer());
    for (; itr.More(); itr.Next()) {
      theTarget << itr.Key(); //key
      const TColStd_Array1OfInteger& anArr1 = itr.Value()->Array1();
      theTarget << anArr1.Lower() <<anArr1.Upper(); // value Arr1 dimensions
      Standard_Integer *aPtr = (Standard_Integer *) &anArr1(anArr1.Lower());
      theTarget.PutIntArray(aPtr, anArr1.Length());
    }
  } else {
    theTarget.PutInteger(0).PutInteger(0);
  }

  if(S->HasArraysOfReals() && !S->GetArraysOfRealsContainer().IsEmpty()) {
    theTarget.PutInteger(1) << S->GetArraysOfRealsContainer().Extent(); //dim
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal
      itr(S->GetArraysOfRealsContainer());
    for (; itr.More(); itr.Next()) {
      theTarget << itr.Key();//key
      const TColStd_Array1OfReal& anArr1 = itr.Value()->Array1();
      theTarget << anArr1.Lower() <<anArr1.Upper(); // value Arr1 dimensions
      Standard_Real *aPtr = (Standard_Real *) &anArr1(anArr1.Lower());
      theTarget.PutRealArray(aPtr, anArr1.Length());
    }
  } else {
    theTarget.PutInteger(0).PutInteger(0);
  }
}
