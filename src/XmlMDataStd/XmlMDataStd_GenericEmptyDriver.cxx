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


#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_GenericEmptyDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_GenericEmptyDriver,XmlMDF_ADriver)

//=======================================================================
//function : XmlMDataStd_GenericEmptyDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_GenericEmptyDriver::XmlMDataStd_GenericEmptyDriver(const Handle(Message_Messenger)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_GenericEmptyDriver::NewEmpty() const
{
  return Handle(TDF_Attribute)(); // this attribute can not be created
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) XmlMDataStd_GenericEmptyDriver::SourceType() const
{
  return Standard_Type::Instance<TDataStd_GenericEmpty>();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_GenericEmptyDriver::Paste(const XmlObjMgt_Persistent&,
					       const Handle(TDF_Attribute)&,
					       XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_GenericEmptyDriver::Paste(const Handle(TDF_Attribute)&,
				   XmlObjMgt_Persistent&,
				   XmlObjMgt_SRelocationTable&  ) const
{

}
