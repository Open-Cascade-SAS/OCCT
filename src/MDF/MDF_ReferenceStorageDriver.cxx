#include <MDF_ReferenceStorageDriver.ixx>
#include <PCollection_HAsciiString.hxx>
#include <PDF_Reference.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <CDM_MessageDriver.hxx>

#define DeclareAndSpeedCast(V,T,Vdown) Handle(T) Vdown = *((Handle(T)*)& V)
#define DeclareConstAndSpeedCast(V,T,Vdown) const Handle(T)& Vdown = (Handle(T)&) V
#define SpeedCast(V,T,Vdown) Vdown = *((Handle(T)*)& V)

MDF_ReferenceStorageDriver::MDF_ReferenceStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDF_ReferenceStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDF_ReferenceStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDF_Reference); }

Handle(PDF_Attribute) MDF_ReferenceStorageDriver::NewEmpty() const
{ return new PDF_Reference; }

void MDF_ReferenceStorageDriver::Paste (
  const Handle(TDF_Attribute)&        Source,
  const Handle(PDF_Attribute)&        Target,
//  const Handle(MDF_SRelocationTable)& RelocTable) const
  const Handle(MDF_SRelocationTable)& ) const
{
  DeclareAndSpeedCast(Source,TDF_Reference,S);
  DeclareAndSpeedCast(Target,PDF_Reference,T);
  Handle(PCollection_HAsciiString) pEntry; // Null handle.
  if (!S.IsNull()) {
    const TDF_Label& lab = S->Label();
    const TDF_Label& refLab = S->Get();
    if (!lab.IsNull() && !refLab.IsNull()) {
      if (lab.IsDescendant(refLab.Root())) {
	// Ref. interne.
	TCollection_AsciiString entry;
	TDF_Tool::Entry(refLab,entry);
	pEntry = new PCollection_HAsciiString(entry);
      }
    }
  }
  T->ReferencedLabel(pEntry);
}

