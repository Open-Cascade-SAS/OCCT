// Created on: 2007-08-01
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


#include <BinMDataStd_AsciiStringDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <BinMDataStd.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_AsciiStringDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_AsciiStringDriver
//purpose  :
//=======================================================================
BinMDataStd_AsciiStringDriver::BinMDataStd_AsciiStringDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
     : BinMDF_ADriver (theMessageDriver, STANDARD_TYPE(TDataStd_AsciiString)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_AsciiStringDriver::NewEmpty() const
{
  return new TDataStd_AsciiString;
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_AsciiStringDriver::Paste
                         (const BinObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          BinObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_AsciiString) aStrAtt = Handle(TDataStd_AsciiString)::DownCast(Target);
  TCollection_AsciiString aString;
  Standard_Boolean ok = Source >> aString;
  if (ok)
    aStrAtt->Set( aString );
  if(BinMDataStd::DocumentVersion() > 8) { // process user defined guid
	const Standard_Integer& aPos = Source.Position();
	Standard_GUID aGuid;
	ok = Source >> aGuid;	
	if (!ok) {
	  Source.SetPosition(aPos);	  
	  aStrAtt->SetID(TDataStd_AsciiString::GetID());
	  ok = Standard_True;
	} else {	  
	  aStrAtt->SetID(aGuid);
	}
  } else
	aStrAtt->SetID(TDataStd_AsciiString::GetID());
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_AsciiStringDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_AsciiString) anAtt = Handle(TDataStd_AsciiString)::DownCast(Source);
  Target << anAtt->Get();
 // process user defined guid
  if(anAtt->ID() != TDataStd_AsciiString::GetID()) 
	Target << anAtt->ID();

}
