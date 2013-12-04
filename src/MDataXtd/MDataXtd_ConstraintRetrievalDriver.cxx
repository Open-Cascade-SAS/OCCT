// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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



