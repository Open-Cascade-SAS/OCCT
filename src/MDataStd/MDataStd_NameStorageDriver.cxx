#include <MDataStd_NameStorageDriver.ixx>
#include <PDataStd_Name.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>

#define BUGOCC425

MDataStd_NameStorageDriver::MDataStd_NameStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NameStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NameStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Name);}

Handle(PDF_Attribute) MDataStd_NameStorageDriver::NewEmpty() const
{ return new PDataStd_Name; }

void MDataStd_NameStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Name) S = Handle(TDataStd_Name)::DownCast (Source);
  Handle(PDataStd_Name) T = Handle(PDataStd_Name)::DownCast (Target);
#ifdef BUGOCC425
  if(S->Get().Length() != 0) {
#endif 
    Handle(PCollection_HExtendedString) Name = new PCollection_HExtendedString (S->Get());
    T->Set (Name);
#ifdef BUGOCC425
  }
#endif 
}

