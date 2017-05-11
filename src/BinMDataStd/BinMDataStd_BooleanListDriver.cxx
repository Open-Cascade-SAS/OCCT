// Created on: 2007-05-29
// Created by: Vlad Romashko
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


#include <BinMDataStd_BooleanListDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfByte.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_BooleanListDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_BooleanListDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_BooleanListDriver::BinMDataStd_BooleanListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_BooleanList)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_BooleanListDriver::NewEmpty() const
{
  return new TDataStd_BooleanList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_BooleanListDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                      const Handle(TDF_Attribute)& theTarget,
                                                      BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aIndex, aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;

  const Handle(TDataStd_BooleanList) anAtt = Handle(TDataStd_BooleanList)::DownCast(theTarget);
  if(aLastInd > 0) {

    const Standard_Integer aLength = aLastInd - aFirstInd + 1;
    if (aLength > 0) {    
      TColStd_Array1OfByte aTargetArray(aFirstInd, aLastInd);
      theSource.GetByteArray (&aTargetArray(aFirstInd), aLength);
      for (aIndex = aFirstInd; aIndex <= aLastInd; aIndex++)
      {
        anAtt->Append(aTargetArray.Value(aIndex) ? Standard_True : Standard_False);
      }
    }
  }

  BinMDataStd::SetAttributeID(theSource, anAtt);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_BooleanListDriver::Paste(const Handle(TDF_Attribute)& theSource,
					  BinObjMgt_Persistent&        theTarget,
					  BinObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_BooleanList) anAtt = Handle(TDataStd_BooleanList)::DownCast(theSource);
  const Standard_Integer aFirstInd = (anAtt->Extent()> 0) ? 1 : 0;
  const Standard_Integer aLastInd(anAtt->Extent());  
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  if (aLength <= 0) return;
  theTarget << aFirstInd << aLastInd;
  if(aLastInd == 0) return;
  TColStd_Array1OfByte aSourceArray(aFirstInd, aLastInd);
  TDataStd_ListIteratorOfListOfByte itr(anAtt->List());
  for (Standard_Integer i = 1; itr.More(); itr.Next(), i++) {
    aSourceArray.SetValue(i, itr.Value());
  }
  Standard_Byte *aPtr = (Standard_Byte *) &aSourceArray(aFirstInd);
  theTarget.PutByteArray(aPtr, aLength);

  // process user defined guid
  if(anAtt->ID() != TDataStd_BooleanList::GetID()) 
    theTarget << anAtt->ID();
}