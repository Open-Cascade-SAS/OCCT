// File:	MDocStd_XLinkRetrievalDriver.cxx
//      	-----------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Sep 17 1997	Creation



#include <MDocStd_XLinkRetrievalDriver.ixx>
#include <TDocStd_XLink.hxx>
#include <PDocStd_XLink.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

#define DeclareAndSpeedCast(V,T,Vdown) Handle(T) Vdown = *((Handle(T)*)& V)
#define DeclareConstAndSpeedCast(V,T,Vdown) const Handle(T)& Vdown = (Handle(T)&) V
#define SpeedCast(V,T,Vdown) Vdown = *((Handle(T)*)& V)

//=======================================================================
//function : MDocStd_XLinkRetrievalDriver
//purpose  : 
//=======================================================================

MDocStd_XLinkRetrievalDriver::MDocStd_XLinkRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDocStd_XLinkRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDocStd_XLinkRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDocStd_XLink); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDocStd_XLinkRetrievalDriver::NewEmpty() const
{ return new TDocStd_XLink(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDocStd_XLinkRetrievalDriver::Paste
(const Handle(PDF_Attribute)& aSource,
 const Handle(TDF_Attribute)& aTarget,
// const Handle(MDF_RRelocationTable)& aRelocTable) const
 const Handle(MDF_RRelocationTable)& ) const
{
  DeclareConstAndSpeedCast(aSource,PDocStd_XLink,pxref);
  DeclareConstAndSpeedCast(aTarget,TDocStd_XLink,txref);
  txref->DocumentEntry(pxref->DocumentEntry()->Convert());
  txref->LabelEntry(pxref->LabelEntry()->Convert());
}

