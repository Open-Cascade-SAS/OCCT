// File:	MDataStd_UAttributeStorageDriver.cxx
// Created:	FriJun 11 16:29:04 1999
// Author:	Sergey RUIN


#include <MDataStd_UAttributeStorageDriver.ixx>
#include <PDataStd_UAttribute.hxx>
#include <TDataStd_UAttribute.hxx>
#include <MDataStd.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_UAttributeStorageDriver
//purpose  : 
//=======================================================================

MDataStd_UAttributeStorageDriver::MDataStd_UAttributeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_UAttributeStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_UAttributeStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_UAttribute);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_UAttributeStorageDriver::NewEmpty () const {

  return new PDataStd_UAttribute ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_UAttributeStorageDriver::Paste (
  const Handle(TDF_Attribute)&  Source,
  const Handle(PDF_Attribute)&        Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_UAttribute) S = Handle(TDataStd_UAttribute)::DownCast (Source);
  Handle(PDataStd_UAttribute) T = Handle(PDataStd_UAttribute)::DownCast (Target);
  TCollection_ExtendedString  extstr("00000000-0000-0000-0000-000000000000");  //covert GUID into ExtendedString
  Standard_PExtCharacter pStr=(Standard_PExtCharacter)extstr.ToExtString();
  S->ID().ToExtString( pStr );

  Handle(PCollection_HExtendedString) guid = new PCollection_HExtendedString ( extstr );

  T->SetID ( guid );  
}

