// File:	MDocStd_XLinkStorageDriver.cxx
//      	---------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Sep 17 1997	Creation



#include <MDocStd_XLinkStorageDriver.ixx>

#include <TDF_Label.hxx>
#include <TDocStd_XLink.hxx>
#include <PDocStd_XLink.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

#define DeclareAndSpeedCast(V,T,Vdown) Handle(T) Vdown = *((Handle(T)*)& V)
#define DeclareConstAndSpeedCast(V,T,Vdown) const Handle(T)& Vdown = (Handle(T)&) V
#define SpeedCast(V,T,Vdown) Vdown = *((Handle(T)*)& V)

//=======================================================================
//function : MDocStd_XLinkStorageDriver
//purpose  : 
//=======================================================================

MDocStd_XLinkStorageDriver::MDocStd_XLinkStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDocStd_XLinkStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDocStd_XLinkStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDocStd_XLink); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDocStd_XLinkStorageDriver::NewEmpty() const
{ return new PDocStd_XLink(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDocStd_XLinkStorageDriver::Paste
(const Handle(TDF_Attribute)& aSource,
 const Handle(PDF_Attribute)& aTarget,
// const Handle(MDF_SRelocationTable)& aRelocTable) const
 const Handle(MDF_SRelocationTable)& ) const
{
  DeclareConstAndSpeedCast(aSource,TDocStd_XLink,txref);
  DeclareConstAndSpeedCast(aTarget,PDocStd_XLink,pxref);
  pxref->DocumentEntry(new PCollection_HAsciiString(txref->DocumentEntry()));
  pxref->LabelEntry(new PCollection_HAsciiString(txref->LabelEntry()));
}

