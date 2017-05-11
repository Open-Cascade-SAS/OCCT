// Created on: 2004-08-24
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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
#include <BinMDataStd_ExtStringArrayDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfExtendedString.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ExtStringArrayDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_ExtStringArrayDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_ExtStringArrayDriver::BinMDataStd_ExtStringArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_ExtStringArray)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_ExtStringArrayDriver::NewEmpty() const
{
  return new TDataStd_ExtStringArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_ExtStringArrayDriver::Paste
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

  Handle(TDataStd_ExtStringArray) anAtt =
    Handle(TDataStd_ExtStringArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  TColStd_Array1OfExtendedString& aTargetArray = anAtt->Array()->ChangeArray1();
  Standard_Boolean ok = Standard_True;
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i ++)
  {
    TCollection_ExtendedString aStr;
    if ( !(theSource >> aStr) )
    {
      ok = Standard_False;
      break;
    }
    aTargetArray.SetValue( i, aStr );
  }

  if(ok) {
    Standard_Boolean aDelta(Standard_False);
    if(BinMDataStd::DocumentVersion() > 2) {
      Standard_Byte aDeltaValue;
      if (! (theSource >> aDeltaValue)) {
        return Standard_False;
      }
      else
        aDelta = (aDeltaValue != 0);
    }
    anAtt->SetDelta(aDelta);
  }

  BinMDataStd::SetAttributeID(theSource, anAtt);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_ExtStringArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 BinObjMgt_Persistent&        theTarget,
                                 BinObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_ExtStringArray) anAtt =
    Handle(TDataStd_ExtStringArray)::DownCast(theSource);
  const TColStd_Array1OfExtendedString& aSourceArray = anAtt->Array()->Array1();
  const Standard_Integer aFirstInd = aSourceArray.Lower();
  const Standard_Integer aLastInd  = aSourceArray.Upper();
  theTarget << aFirstInd << aLastInd;
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i ++)
    theTarget << anAtt->Value( i );

  theTarget << (Standard_Byte)(anAtt->GetDelta() ? 1 : 0);

  // process user defined guid
  if(anAtt->ID() != TDataStd_ExtStringArray::GetID()) 
    theTarget << anAtt->ID();
}
