// Created on: 2002-10-30
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

#include <BinMDataStd_RealDriver.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Attribute.hxx>
#include <BinMDataStd.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_RealDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_RealDriver::BinMDataStd_RealDriver(const Handle(Message_Messenger)& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_Real)->Name())
{
}

//=================================================================================================

Handle(TDF_Attribute) BinMDataStd_RealDriver::NewEmpty() const
{
  return new TDataStd_Real();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_RealDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                               const Handle(TDF_Attribute)& theTarget,
                                               BinObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataStd_Real) anAtt = Handle(TDataStd_Real)::DownCast(theTarget);
  Standard_Real         aValue;
  Standard_Boolean      ok = theSource >> aValue;
  if (ok)
    anAtt->Set(aValue);
  if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_9)
  { // process user defined guid
    const Standard_Integer& aPos = theSource.Position();
    Standard_GUID           aGuid;
    ok = theSource >> aGuid;
    if (!ok)
    {
      theSource.SetPosition(aPos);
      anAtt->SetID(TDataStd_Real::GetID());
      ok = Standard_True;
    }
    else
    {
      anAtt->SetID(aGuid);
    }
  }
  else
    anAtt->SetID(TDataStd_Real::GetID());
  return ok;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_RealDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                   BinObjMgt_Persistent&        theTarget,
                                   BinObjMgt_SRelocationTable&) const
{
  Handle(TDataStd_Real) anAtt = Handle(TDataStd_Real)::DownCast(theSource);
  theTarget << anAtt->Get();
  // process user defined guid
  if (anAtt->ID() != TDataStd_Real::GetID())
    theTarget << anAtt->ID();
}
