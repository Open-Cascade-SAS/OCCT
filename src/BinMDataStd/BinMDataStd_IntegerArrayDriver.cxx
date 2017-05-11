// Created on: 2002-10-31
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BinMDataStd.hxx>
#include <BinMDataStd_IntegerArrayDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_IntegerArrayDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_IntegerArrayDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_IntegerArrayDriver::BinMDataStd_IntegerArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_IntegerArray)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_IntegerArrayDriver::NewEmpty() const
{
  return new TDataStd_IntegerArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_IntegerArrayDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  const Handle(TDataStd_IntegerArray) anAtt =
    Handle(TDataStd_IntegerArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  TColStd_Array1OfInteger& aTargetArray = anAtt->Array()->ChangeArray1();
  if(!theSource.GetIntArray (&aTargetArray(aFirstInd), aLength))
    return Standard_False;
  Standard_Boolean aDelta(Standard_False);
  if(BinMDataStd::DocumentVersion() > 2) {
    Standard_Byte aDeltaValue;
    if (! (theSource >> aDeltaValue))
      return Standard_False;
    else
      aDelta = (aDeltaValue != 0);
  }
#ifdef OCCT_DEBUG
  else if(BinMDataStd::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
  anAtt->SetDelta(aDelta);

  BinMDataStd::SetAttributeID(theSource, anAtt);
  return Standard_True; 
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_IntegerArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 BinObjMgt_Persistent&        theTarget,
                                 BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntegerArray) anAtt =
    Handle(TDataStd_IntegerArray)::DownCast(theSource);
  const TColStd_Array1OfInteger& aSourceArray = anAtt->Array()->Array1();
  const Standard_Integer aFirstInd = aSourceArray.Lower();
  const Standard_Integer aLastInd  = aSourceArray.Upper();
  const Standard_Integer aLength   = aLastInd - aFirstInd + 1;
  theTarget << aFirstInd << aLastInd;
  Standard_Integer *aPtr = (Standard_Integer *) &aSourceArray(aFirstInd);
  theTarget.PutIntArray (aPtr, aLength);
  theTarget << (Standard_Byte)(anAtt->GetDelta() ? 1 : 0);

  // process user defined guid
  if(anAtt->ID() != TDataStd_IntegerArray::GetID()) 
    theTarget << anAtt->ID();
}
