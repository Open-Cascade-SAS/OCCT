#include <MDataStd_DirectoryRetrievalDriver.ixx>
#include <PDataStd_Directory.hxx>
#include <TDataStd_Directory.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_DirectoryRetrievalDriver::MDataStd_DirectoryRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MDataStd_DirectoryRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_DirectoryRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Directory); }

Handle(TDF_Attribute) MDataStd_DirectoryRetrievalDriver::NewEmpty() const
{ return new TDataStd_Directory; }

void MDataStd_DirectoryRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
{
}

