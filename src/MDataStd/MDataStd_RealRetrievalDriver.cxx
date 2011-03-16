// File:	MDataStd_RealRetrievalDriver.cxx
// Created:	Thu Apr 10 16:30:18 1997
// Author:	VAUTHIER Jean-Claude


#include <MDataStd_RealRetrievalDriver.ixx>
#include <TDataStd_Real.hxx>
#include <PDataStd_Real.hxx>
#include <MDataStd.hxx>
#include <TDataStd_RealEnum.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataStd_RealRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_RealRetrievalDriver::MDataStd_RealRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RealRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RealRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_Real);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_RealRetrievalDriver::NewEmpty () const {

  return new TDataStd_Real ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RealRetrievalDriver::Paste 
(const Handle(PDF_Attribute)&  Source,
 const Handle(TDF_Attribute)&  Target,
// const Handle(MDF_RRelocationTable)& RelocTable) const
 const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_Real) S = Handle(PDataStd_Real)::DownCast (Source);
  Handle(TDataStd_Real) T = Handle(TDataStd_Real)::DownCast (Target);
  T->Set (S->Get ());
  T->SetDimension(MDataStd::IntegerToRealDimension(S->GetDimension()));
}

