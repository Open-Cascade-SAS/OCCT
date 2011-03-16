#include <MDataStd_NameRetrievalDriver.ixx>
#include <PDataStd_Name.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_NameRetrievalDriver::MDataStd_NameRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NameRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NameRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Name); }

Handle(TDF_Attribute) MDataStd_NameRetrievalDriver::NewEmpty() const
{ return new TDataStd_Name (); }


#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>

#define BUGOCC425

void MDataStd_NameRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_Name) S = Handle(PDataStd_Name)::DownCast (Source);
  Handle(TDataStd_Name) T = Handle(TDataStd_Name)::DownCast (Target);
#ifdef BUGOCC425
  if(!S->Get().IsNull()) {
#endif 
    TCollection_ExtendedString Name = (S->Get())->Convert ();
    T->Set (Name);
#ifdef BUGOCC425
  }
#endif 
}

