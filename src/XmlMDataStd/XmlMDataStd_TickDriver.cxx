// File:        XmlMDataStd_TickDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <XmlMDataStd_TickDriver.ixx>
#include <TDataStd_Tick.hxx>

//=======================================================================
//function : XmlMDataStd_TickDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_TickDriver::XmlMDataStd_TickDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
 Handle(TDF_Attribute) XmlMDataStd_TickDriver::NewEmpty() const
{
  return (new TDataStd_Tick());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_TickDriver::Paste(const XmlObjMgt_Persistent&,
					       const Handle(TDF_Attribute)&,
					       XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_TickDriver::Paste(const Handle(TDF_Attribute)&,
				   XmlObjMgt_Persistent&,
				   XmlObjMgt_SRelocationTable&  ) const
{

}
