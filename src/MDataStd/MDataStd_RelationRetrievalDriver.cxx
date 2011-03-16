#include <MDataStd_RelationRetrievalDriver.ixx>
#include <PDataStd_Relation.hxx>
#include <TDataStd_Relation.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx> 
#include <PDF_HAttributeArray1.hxx> 
#include <Standard_NoSuchObject.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataStd_RelationRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_RelationRetrievalDriver::MDataStd_RelationRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RelationRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RelationRetrievalDriver::SourceType() const
{
  return STANDARD_TYPE(PDataStd_Relation); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_RelationRetrievalDriver::NewEmpty() const
{
  return new TDataStd_Relation (); 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RelationRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
					     const Handle(TDF_Attribute)& Target,
					     const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataStd_Relation) S = Handle(PDataStd_Relation)::DownCast (Source);
  Handle(TDataStd_Relation) T = Handle(TDataStd_Relation)::DownCast (Target);
  TCollection_ExtendedString Relation = (S->GetName())->Convert ();
  T->SetRelation (Relation);
  
  Handle(PDF_Attribute) PV; 
  Handle(TDF_Attribute) TV;

  Handle(PDF_HAttributeArray1) PVARS = S->GetVariables ();
  if (PVARS.IsNull()) return;
  for (Standard_Integer i = 1; i <= PVARS->Length (); i++) {
    PV = PVARS->Value (i);
    if (!PV.IsNull ()) {
      if (!RelocTable->HasRelocation (PV, TV)) {
	Standard_NoSuchObject::Raise("MDataStd_ConstraintRetrievalDriver::Paste");
      }
      T->GetVariables().Append(TV);
    }
  }
}

