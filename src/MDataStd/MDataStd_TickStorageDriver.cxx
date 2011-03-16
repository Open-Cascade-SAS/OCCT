// File:        MDataStd_TickStorageDriver.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <MDataStd_TickStorageDriver.ixx>
#include <PDataStd_Tick.hxx>
#include <TDataStd_Tick.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_TickStorageDriver
//purpose  : 
//=======================================================================
MDataStd_TickStorageDriver::MDataStd_TickStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_TickStorageDriver::VersionNumber() const
{ 
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_TickStorageDriver::SourceType() const
{ 
  return STANDARD_TYPE(TDataStd_Tick); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_TickStorageDriver::NewEmpty() const
{ 
  return new PDataStd_Tick; 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_TickStorageDriver::Paste (const Handle(TDF_Attribute)& ,
					const Handle(PDF_Attribute)& ,
					const Handle(MDF_SRelocationTable)& ) const
{
}
