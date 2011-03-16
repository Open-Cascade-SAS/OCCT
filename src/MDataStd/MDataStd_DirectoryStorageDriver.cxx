#include <MDataStd_DirectoryStorageDriver.ixx>
#include <PDataStd_Directory.hxx>
#include <TDataStd_Directory.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_DirectoryStorageDriver::MDataStd_DirectoryStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_DirectoryStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_DirectoryStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Directory); }

Handle(PDF_Attribute) MDataStd_DirectoryStorageDriver::NewEmpty() const
{ return new PDataStd_Directory; }

//void MDataStd_DirectoryStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataStd_DirectoryStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

