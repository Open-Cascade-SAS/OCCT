#include <MDF_TagSourceRetrievalDriver.ixx>
#include <PDF_TagSource.hxx>
#include <TDF_TagSource.hxx>
#include <CDM_MessageDriver.hxx>

MDF_TagSourceRetrievalDriver::MDF_TagSourceRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDF_TagSourceRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDF_TagSourceRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDF_TagSource); }

Handle(TDF_Attribute) MDF_TagSourceRetrievalDriver::NewEmpty() const
{ return new TDF_TagSource; }

//void MDF_TagSourceRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDF_TagSourceRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDF_TagSource) S = Handle(PDF_TagSource)::DownCast (Source);
  Handle(TDF_TagSource) T = Handle(TDF_TagSource)::DownCast (Target);
  T->Set (S->Get ());
}

