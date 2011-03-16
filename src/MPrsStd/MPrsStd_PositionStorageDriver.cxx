// File:	MPrsStd_PositionStorageDriver.cxx
// Created:	Tue Apr  7 14:59:53 1998
// Author:	Jean-Pierre COMBE
//		<jpr>


#include <MPrsStd_PositionStorageDriver.ixx>

#include <PDataXtd_Position.hxx>
#include <TDataXtd_Position.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MPrsStd_PositionStorageDriver
//purpose  : 
//=======================================================================
MPrsStd_PositionStorageDriver::MPrsStd_PositionStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MPrsStd_PositionStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MPrsStd_PositionStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Position); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MPrsStd_PositionStorageDriver::NewEmpty() const
{ return new PDataXtd_Position; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MPrsStd_PositionStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
							  const Handle(PDF_Attribute)& Target,
							  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataXtd_Position) S = Handle(TDataXtd_Position)::DownCast (Source);
  Handle(PDataXtd_Position) T = Handle(PDataXtd_Position)::DownCast (Target);  
  T->SetPosition(S->GetPosition());
}


