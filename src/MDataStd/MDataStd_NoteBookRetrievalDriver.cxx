#include <MDataStd_NoteBookRetrievalDriver.ixx>
#include <PDataStd_NoteBook.hxx>
#include <TDataStd_NoteBook.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_NoteBookRetrievalDriver::MDataStd_NoteBookRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NoteBookRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NoteBookRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_NoteBook); }

Handle(TDF_Attribute) MDataStd_NoteBookRetrievalDriver::NewEmpty() const
{ return new TDataStd_NoteBook; }

//void MDataStd_NoteBookRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDataStd_NoteBookRetrievalDriver::Paste(const Handle(PDF_Attribute)& ,const Handle(TDF_Attribute)& ,const Handle(MDF_RRelocationTable)& ) const
{
}

