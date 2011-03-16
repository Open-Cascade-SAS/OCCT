// File:	MDataStd_RealStorageDriver.cxx
// Created:	Thu Apr 10 16:29:04 1997
// Author:	VAUTHIER Jean-Claude


#include <MDataStd_RealStorageDriver.ixx>
#include <PDataStd_Real.hxx>
#include <TDataStd_Real.hxx>
#include <MDataStd.hxx>
#include <TDataStd_RealEnum.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataStd_RealStorageDriver
//purpose  : 
//=======================================================================

MDataStd_RealStorageDriver::MDataStd_RealStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RealStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RealStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_Real);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_RealStorageDriver::NewEmpty () const {

  return new PDataStd_Real ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RealStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&        Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Real) S = Handle(TDataStd_Real)::DownCast (Source);
  Handle(PDataStd_Real) T = Handle(PDataStd_Real)::DownCast (Target);
  T->Set (S->Get ());  
  T->SetDimension (MDataStd::RealDimensionToInteger(S->GetDimension()));
}

