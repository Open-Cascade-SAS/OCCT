// Created on: 2000-09-29
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <MXCAFDoc_GraphNodeStorageDriver.ixx>

#include <XCAFDoc_GraphNode.hxx>
#include <PXCAFDoc_GraphNode.hxx>
#include <Standard_NoSuchObject.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_GraphNodeStorageDriver::MXCAFDoc_GraphNodeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_GraphNodeStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_GraphNodeStorageDriver::SourceType() const
{ return STANDARD_TYPE(XCAFDoc_GraphNode); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MXCAFDoc_GraphNodeStorageDriver::NewEmpty() const
{ return new PXCAFDoc_GraphNode(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_GraphNodeStorageDriver::Paste(const Handle(TDF_Attribute)&        Source,
					    const Handle(PDF_Attribute)&        Target,
					    const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_GraphNode) S = Handle(XCAFDoc_GraphNode)::DownCast (Source);
  Handle(PXCAFDoc_GraphNode) T = Handle(PXCAFDoc_GraphNode)::DownCast (Target);

  Handle (PXCAFDoc_GraphNode) TF;
  Handle (XCAFDoc_GraphNode) F;
  Standard_Integer i;

  for ( i = 1; i <= S->NbFathers(); i++) {
    F = S->GetFather(i);
    if (!F.IsNull ()) {
      if (!RelocTable->HasRelocation(F, TF)) {
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeStorageDriver::Paste");
	}
      T->SetFather(TF);
    }
  }
  
  for ( i = 1; i <= S->NbChildren(); i++) {
    F = S->GetChild(i);
    if (!F.IsNull()) {
      if (!RelocTable->HasRelocation(F, TF)) {
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeStorageDriver::Paste");
	}
      T->SetChild(TF);
    }
  }
  
  T->SetGraphID(S->ID());
}
