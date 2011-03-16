#include <MDataStd_NoteBookStorageDriver.ixx>
#include <PDataStd_NoteBook.hxx>
#include <TDataStd_NoteBook.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_NoteBookStorageDriver::MDataStd_NoteBookStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NoteBookStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NoteBookStorageDriver::SourceType() const
{ return STANDARD_TYPE (TDataStd_NoteBook); }

Handle(PDF_Attribute) MDataStd_NoteBookStorageDriver::NewEmpty() const
{ return new PDataStd_NoteBook(); }

//void MDataStd_NoteBookStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataStd_NoteBookStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

