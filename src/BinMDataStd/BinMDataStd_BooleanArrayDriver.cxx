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


#include <BinMDataStd_BooleanArrayDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_BooleanArrayDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_BooleanArrayDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_BooleanArrayDriver::BinMDataStd_BooleanArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_BooleanArray)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_BooleanArrayDriver::NewEmpty() const
{
  return new TDataStd_BooleanArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_BooleanArrayDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                       const Handle(TDF_Attribute)& theTarget,
                                                       BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  if (aLastInd < aFirstInd)
    return Standard_False;

  TColStd_Array1OfByte aTargetArray(0, (aLastInd - aFirstInd + 1) >> 3);
  theSource.GetByteArray (&aTargetArray(0), aTargetArray.Length());

  const Handle(TDataStd_BooleanArray) anAtt = Handle(TDataStd_BooleanArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  Handle(TColStd_HArray1OfByte) bytes = new TColStd_HArray1OfByte(aTargetArray.Lower(), aTargetArray.Upper());
  Standard_Integer lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  for (; i <= upper; i++)
  {
    bytes->SetValue(i, aTargetArray.Value(i));
  }
  anAtt->SetInternalArray(bytes);
  BinMDataStd::SetAttributeID(theSource, anAtt);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_BooleanArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                           BinObjMgt_Persistent&        theTarget,
                                           BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_BooleanArray) anAtt = Handle(TDataStd_BooleanArray)::DownCast(theSource);
  const Standard_Integer aFirstInd = anAtt->Lower();
  const Standard_Integer aLastInd  = anAtt->Upper();
  if (aLastInd < aFirstInd)
    return;
  theTarget << aFirstInd << aLastInd;

  const Handle(TColStd_HArray1OfByte)& bytes = anAtt->InternalArray();
  TColStd_Array1OfByte aSourceArray(bytes->Lower(), bytes->Upper());
  Standard_Integer lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  for (; i <= upper; i++)
  {
    aSourceArray.SetValue(i, bytes->Value(i));
  }
  Standard_Byte *aPtr = (Standard_Byte *) &aSourceArray(lower);
  theTarget.PutByteArray(aPtr, upper - lower + 1);

  // process user defined guid
  if(anAtt->ID() != TDataStd_BooleanArray::GetID()) 
    theTarget << anAtt->ID();
}
