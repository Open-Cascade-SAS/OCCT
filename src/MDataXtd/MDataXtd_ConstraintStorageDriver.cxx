#include <MDataXtd_ConstraintStorageDriver.ixx>
#include <PDataXtd_Constraint.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TNaming_NamedShape.hxx>
#include <PNaming_NamedShape.hxx>
#include <PDF_HAttributeArray1.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_DomainError.hxx>
#include <MDataXtd.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataXtd_ConstraintStorageDriver
//purpose  : 
//=======================================================================

MDataXtd_ConstraintStorageDriver::MDataXtd_ConstraintStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_ConstraintStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_ConstraintStorageDriver::SourceType() const
{ return STANDARD_TYPE (TDataXtd_Constraint); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataXtd_ConstraintStorageDriver::NewEmpty() const
{ return new PDataXtd_Constraint; }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_ConstraintStorageDriver::Paste (
  const Handle(TDF_Attribute)&        Source,
  const Handle(PDF_Attribute)&        Target,
  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataXtd_Constraint) S = 
     Handle(TDataXtd_Constraint)::DownCast (Source);
  Handle(PDataXtd_Constraint) T = 
    Handle(PDataXtd_Constraint)::DownCast (Target);

  Handle(PDataStd_Real) TValue;
  Handle(TDataStd_Real) Value = S->GetValue ();
  if (!Value.IsNull ()) {
    if (!RelocTable->HasRelocation (Value, TValue )) {
      Standard_NoSuchObject::Raise("MDataXtd_ConstraintStorageDriver::Paste");
    }
    T->Set (TValue);
  }

  Standard_Integer NbGeom = S->NbGeometries ();
  if (NbGeom >= 1) {
    Handle(PDF_HAttributeArray1) PGeometries = new PDF_HAttributeArray1 (1, NbGeom);
    for (Standard_Integer i = 1; i <= NbGeom; i++) {
      Handle(PDF_Attribute) PG;
      if (!S->GetGeometry (i).IsNull()) {
	if(!RelocTable->HasRelocation (S->GetGeometry (i), PG)) {
	  Standard_NoSuchObject::Raise("MDataXtd_ConstraintStorageDriver::Paste");
	}
      }
      PGeometries->SetValue (i,PG);
    }
    T->SetGeometries (PGeometries);
  }

  Handle(TNaming_NamedShape) TPlane = S->GetPlane();
  Handle(PNaming_NamedShape) PPlane;
  if (!TPlane.IsNull()) {
    if (!RelocTable->HasRelocation (TPlane, PPlane )) {
      Standard_NoSuchObject::Raise("MDataXtd_ConstraintStorageDriver::Paste");
    }
    T->SetPlane(PPlane);
  }
  
  T->SetType (MDataXtd::ConstraintTypeToInteger(S->GetType ()));
  T->Verified(S->Verified());
  T->Inverted(S->Inverted());
  T->Reversed(S->Reversed());
}





