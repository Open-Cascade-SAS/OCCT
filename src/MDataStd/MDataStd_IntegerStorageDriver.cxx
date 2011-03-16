// File:	MDataStd_IntegerStorageDriver.cxx
// Created:	Thu Apr 10 16:31:25 1997
// Author:	VAUTHIER Jean-Claude


#include <MDataStd_IntegerStorageDriver.ixx>
#include <TDataStd_Integer.hxx>
#include <PDataStd_Integer.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataStd_IntegerStorageDriver
//purpose  : 
//=======================================================================

MDataStd_IntegerStorageDriver::MDataStd_IntegerStorageDriver (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_IntegerStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_IntegerStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_Integer);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_IntegerStorageDriver::NewEmpty () const {

  return new PDataStd_Integer ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_IntegerStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&  Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Integer) S = Handle(TDataStd_Integer)::DownCast (Source);
  Handle(PDataStd_Integer) T = Handle(PDataStd_Integer)::DownCast (Target);
  T->Set (S->Get ());
}




