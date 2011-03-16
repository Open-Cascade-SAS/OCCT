// File:        MDataStd_TickRetrievalDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_TickRetrievalDriver.ixx>
#include <PDataStd_Tick.hxx>
#include <TDataStd_Tick.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_TickRetrievalDriver
//purpose  : 
//=======================================================================
MDataStd_TickRetrievalDriver::MDataStd_TickRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_TickRetrievalDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_TickRetrievalDriver::SourceType() const
{ 
  return STANDARD_TYPE(PDataStd_Tick);
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_TickRetrievalDriver::NewEmpty() const
{ 
  return new TDataStd_Tick; 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_TickRetrievalDriver::Paste (const Handle(PDF_Attribute)& ,
					  const Handle(TDF_Attribute)& ,
					  const Handle(MDF_RRelocationTable)& ) const
{
}
