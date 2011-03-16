// File:        BinMDataStd_TickDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <BinMDataStd_TickDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDataStd_Tick.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>

//=======================================================================
//function : BinMDataStd_TickDriver
//purpose  : 
//=======================================================================
BinMDataStd_TickDriver::BinMDataStd_TickDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_Tick)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_TickDriver::NewEmpty() const
{
  return new TDataStd_Tick();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_TickDriver::Paste(const BinObjMgt_Persistent&,
						    const Handle(TDF_Attribute)&,
						    BinObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_TickDriver::Paste(const Handle(TDF_Attribute)&,
					BinObjMgt_Persistent&,
					BinObjMgt_SRelocationTable&  ) const
{
}
