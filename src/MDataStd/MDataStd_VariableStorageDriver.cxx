// File:	MDataStd_VariableStorageDriver.cxx
// Created:	Thu Apr 10 16:29:04 1997
// Author:	PASCAL Denis


#include <MDataStd_VariableStorageDriver.ixx>
#include <PDataStd_Variable.hxx>
#include <TDataStd_Variable.hxx>
#include <MDataStd.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataStd_VariableStorageDriver
//purpose  : 
//=======================================================================

MDataStd_VariableStorageDriver::MDataStd_VariableStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_VariableStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_VariableStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_Variable);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_VariableStorageDriver::NewEmpty () const {

  return new PDataStd_Variable ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_VariableStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&        Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Variable) S = Handle(TDataStd_Variable)::DownCast (Source);
  Handle(PDataStd_Variable) T = Handle(PDataStd_Variable)::DownCast (Target);
  T->Constant (S->IsConstant ());  
  T->Unit(new PCollection_HAsciiString(S->Unit()));
}

