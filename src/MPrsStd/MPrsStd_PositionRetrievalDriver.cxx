// File:	MPrsStd_PositionRetrievalDriver.cxx
// Created:	Tue Apr  7 15:01:52 1998
// Author:	Jean-Pierre COMBE
//		<jpr>


#include <MPrsStd_PositionRetrievalDriver.ixx>

#include <PDataXtd_Position.hxx>
#include <TDataXtd_Position.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MPrsStd_PositionRetrievalDriver
//purpose  : 
//=======================================================================
MPrsStd_PositionRetrievalDriver::MPrsStd_PositionRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MPrsStd_PositionRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MPrsStd_PositionRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Position); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MPrsStd_PositionRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Position; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MPrsStd_PositionRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
							    const Handle(TDF_Attribute)& Target,
							    const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataXtd_Position) S = Handle(PDataXtd_Position)::DownCast (Source);
  Handle(TDataXtd_Position) T = Handle(TDataXtd_Position)::DownCast (Target);  
  T->SetPosition(S->GetPosition());
}

