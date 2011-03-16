#include <MDataStd_RelationStorageDriver.ixx>
#include <PDataStd_Relation.hxx>
#include <TDataStd_Relation.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx> 
#include <PDF_HAttributeArray1.hxx> 
#include <Standard_NoSuchObject.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_RelationStorageDriver
//purpose  : 
//=======================================================================

MDataStd_RelationStorageDriver::MDataStd_RelationStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RelationStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RelationStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Relation);}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_RelationStorageDriver::NewEmpty() const
{ return new PDataStd_Relation; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RelationStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_Relation) S = Handle(TDataStd_Relation)::DownCast (Source);
  Handle(PDataStd_Relation) T = Handle(PDataStd_Relation)::DownCast (Target);
  Handle(PCollection_HExtendedString) Relation = new PCollection_HExtendedString (S->Name());
  T->SetName (Relation);
  Handle(TDF_Attribute) TV;   
  Handle(PDF_Attribute) PV;

  Standard_Integer nbvar = S->GetVariables().Extent();
  if (nbvar <= 0) return;
  Handle(PDF_HAttributeArray1) PVARS = new PDF_HAttributeArray1 (1, nbvar);
  TDF_ListIteratorOfAttributeList it;
  Standard_Integer index = 0;
  for (it.Initialize(S->GetVariables());it.More();it.Next()) {
    index++;
    TV = it.Value(); 
    if(!RelocTable->HasRelocation (TV, PV)) {
      Standard_NoSuchObject::Raise("MDataStd_ExpressionStorageDriver::Paste");
    }
    PVARS->SetValue (index,PV);
  }
  T->SetVariables (PVARS);
}

