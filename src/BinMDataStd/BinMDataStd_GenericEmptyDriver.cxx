// Copyright (c) 2020 OPEN CASCADE SAS
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


#include <BinMDataStd_GenericEmptyDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_GenericEmptyDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_GenericEmptyDriver
//purpose  : 
//=======================================================================
BinMDataStd_GenericEmptyDriver::BinMDataStd_GenericEmptyDriver(const Handle(Message_Messenger)& theMsgDriver)
: BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_GenericEmpty)->Name())
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_GenericEmptyDriver::NewEmpty() const
{
  return Handle(TDF_Attribute)(); // this attribute can not be created
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
const Handle(Standard_Type)& BinMDataStd_GenericEmptyDriver::SourceType() const
{
  return Standard_Type::Instance<TDataStd_GenericEmpty>();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_GenericEmptyDriver::Paste(const BinObjMgt_Persistent&,
						    const Handle(TDF_Attribute)&,
						    BinObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_GenericEmptyDriver::Paste(const Handle(TDF_Attribute)&,
					BinObjMgt_Persistent&,
					BinObjMgt_SRelocationTable&  ) const
{
}
