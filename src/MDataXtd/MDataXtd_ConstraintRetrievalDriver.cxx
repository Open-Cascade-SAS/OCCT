// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <MDataXtd_ConstraintRetrievalDriver.ixx>
#include <MDataXtd.hxx>
#include <PDataXtd_Constraint.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <TNaming_NamedShape.hxx>
#include <PNaming_NamedShape.hxx>
#include <PDF_HAttributeArray1.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_DomainError.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataXtd_ConstraintRetrievalDriver
//purpose  : 
//=======================================================================

MDataXtd_ConstraintRetrievalDriver::MDataXtd_ConstraintRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_ConstraintRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_ConstraintRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Constraint);  }




//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataXtd_ConstraintRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Constraint (); }



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_ConstraintRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
  const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataXtd_Constraint) S = 
     Handle(PDataXtd_Constraint)::DownCast (Source);
  Handle(TDataXtd_Constraint) T = 
    Handle(TDataXtd_Constraint)::DownCast (Target);

  Handle(TDataStd_Real) TValue;
  Handle(PDataStd_Real) Value = S->GetValue ();
  if (!Value.IsNull ()) {
    if (!RelocTable->HasRelocation(Value, TValue )) {
      Standard_NoSuchObject::Raise("MDataXtd_ConstraintRetrievalDriver::Paste");
    }
    T->SetValue(TValue);
  }
  
  Handle(PDF_HAttributeArray1) PGeometries = S->GetGeometries ();
  if (!PGeometries.IsNull()) {
    for (Standard_Integer i = 1; i <= PGeometries->Length (); i++) {
      Handle(PDF_Attribute) PG = PGeometries->Value (i);
      if (!PG.IsNull ()) {
	Handle(TNaming_NamedShape) G;
	if (!RelocTable->HasRelocation (PG, G)) {
	  Standard_NoSuchObject::Raise("MDataXtd_ConstraintRetrievalDriver::Paste");
	}
	T->SetGeometry (i, G);
      }
    }
  }


  Handle(PNaming_NamedShape) PPlane = S->GetPlane();
  Handle(TNaming_NamedShape) TPlane;
  if (!PPlane.IsNull()) {
    if (!RelocTable->HasRelocation (PPlane, TPlane )) {
      Standard_NoSuchObject::Raise("MDataXtd_ConstraintStorageDriver::Paste");
    }
    T->SetPlane(TPlane);
  }
  T->SetType (MDataXtd::IntegerToConstraintType (S->GetType ()));
  T->Verified(S->Verified());
  T->Inverted(S->Inverted());
  T->Reversed(S->Reversed());
}



