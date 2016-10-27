// Created on: 2002-11-19
// Created by: Edward AGAPOV (eap)
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


#include <BinMDataStd_NameDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Attribute.hxx>
#include <BinMDataStd.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_NameDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_NameDriver
//purpose  :
//=======================================================================
BinMDataStd_NameDriver::BinMDataStd_NameDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
     : BinMDF_ADriver (theMessageDriver, STANDARD_TYPE(TDataStd_Name)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_NameDriver::NewEmpty() const
{
  return new TDataStd_Name;
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_NameDriver::Paste
                         (const BinObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          BinObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(Target);
  TCollection_ExtendedString aStr;
  Standard_Boolean ok = Source >> aStr;
  if (ok)
    aName->Set( aStr );
  if(BinMDataStd::DocumentVersion() > 8) { // process user defined guid
	const Standard_Integer& aPos = Source.Position();
	Standard_GUID aGuid;
	ok = Source >> aGuid;	
	if (!ok) {
	  Source.SetPosition(aPos);	  
	  aName->SetID(TDataStd_Name::GetID());
	  ok = Standard_True;
	} else {	  
	  aName->SetID(aGuid);
	}
  } else
	aName->SetID(TDataStd_Name::GetID());
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_NameDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(Source);
  Target << aName->Get();
  // process user defined guid
  if(aName->ID() != TDataStd_Name::GetID()) 
	Target << aName->ID();
}
