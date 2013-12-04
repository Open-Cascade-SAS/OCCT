// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BinMDataStd_IntegerListDriver.ixx>
#include <TDataStd_IntegerList.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

//=======================================================================
//function : BinMDataStd_IntegerListDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_IntegerListDriver::BinMDataStd_IntegerListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_IntegerList)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_IntegerListDriver::NewEmpty() const
{
  return new TDataStd_IntegerList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_IntegerListDriver::Paste(const BinObjMgt_Persistent&  theSource,
						      const Handle(TDF_Attribute)& theTarget,
						      BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aIndex, aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  TColStd_Array1OfInteger aTargetArray(aFirstInd, aLastInd);
  theSource.GetIntArray (&aTargetArray(aFirstInd), aLength);

  Handle(TDataStd_IntegerList) anAtt = Handle(TDataStd_IntegerList)::DownCast(theTarget);
  for (aIndex = aFirstInd; aIndex <= aLastInd; aIndex++)
  {
    anAtt->Append(aTargetArray.Value(aIndex));
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_IntegerListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					  BinObjMgt_Persistent&        theTarget,
					  BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntegerList) anAtt = Handle(TDataStd_IntegerList)::DownCast(theSource);
  const Standard_Integer aFirstInd = 1;
  const Standard_Integer aLastInd  = anAtt->Extent();
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return;
  theTarget << aFirstInd << aLastInd;
  TColStd_Array1OfInteger aSourceArray(aFirstInd, aLastInd);
  if (aLastInd >= 1)
  {
    TColStd_ListIteratorOfListOfInteger itr(anAtt->List());
    for (Standard_Integer i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Value());
    }
    Standard_Integer *aPtr = (Standard_Integer *) &aSourceArray(aFirstInd);
    theTarget.PutIntArray(aPtr, aLength);
  }
}
