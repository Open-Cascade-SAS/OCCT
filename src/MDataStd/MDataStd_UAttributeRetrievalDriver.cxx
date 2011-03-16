// File:	MDataStd_UObjectRetrievalDriver.cxx
// Created:	Tue Jun 15 16:29:04 1999
// Author:	Sergey RUIN


#include <MDataStd_UAttributeRetrievalDriver.ixx>
#include <PDataStd_UAttribute.hxx>
#include <TDataStd_UAttribute.hxx>
#include <MDataStd.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_UAttributeRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_UAttributeRetrievalDriver::MDataStd_UAttributeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_UAttributeRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_UAttributeRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_UAttribute);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_UAttributeRetrievalDriver::NewEmpty () const {

  return new TDataStd_UAttribute ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_UAttributeRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
      const Handle(TDF_Attribute)& Target,
      const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataStd_UAttribute) S = Handle(PDataStd_UAttribute)::DownCast (Source);
  Handle(TDataStd_UAttribute) T = Handle(TDataStd_UAttribute)::DownCast (Target);

  TCollection_ExtendedString extstr = (S->GetID())->Convert();
  Standard_GUID guid( extstr.ToExtString() ); 

  T->SetID( guid );  
}

