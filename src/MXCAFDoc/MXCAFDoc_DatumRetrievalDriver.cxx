// File:      MXCAFDoc_DatumRetrievalDriver.cxx
// Created:   10.12.08 09:46:46
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <MXCAFDoc_DatumRetrievalDriver.ixx>
#include <PXCAFDoc_Datum.hxx>
#include <XCAFDoc_Datum.hxx>

#include <TCollection_HAsciiString.hxx>
#include <PCollection_HAsciiString.hxx>


//=======================================================================
//function : MCAFDoc_DatumRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_DatumRetrievalDriver::MXCAFDoc_DatumRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_DatumRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_DatumRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Datum);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_DatumRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Datum();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DatumRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
                                           const Handle(TDF_Attribute)& Target,
                                           const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Datum) S = Handle(PXCAFDoc_Datum)::DownCast (Source);
  Handle(XCAFDoc_Datum) T = Handle(XCAFDoc_Datum)::DownCast (Target);
  
#ifdef DEB
  PTColStd_PersistentTransientMap& PTMap = 
#endif
    RelocTable->OtherTable();
  Handle(TCollection_HAsciiString) aName, aDescr, anId;
  if ( !S->GetName().IsNull() )
    aName = new TCollection_HAsciiString( (S->GetName())->Convert() );
  if ( !S->GetDescription().IsNull() )
    aDescr = new TCollection_HAsciiString( (S->GetDescription())->Convert() );
  if ( !S->GetIdentification().IsNull() )
    anId = new TCollection_HAsciiString( (S->GetIdentification())->Convert() );

  T->Set(aName, aDescr, anId);
}
