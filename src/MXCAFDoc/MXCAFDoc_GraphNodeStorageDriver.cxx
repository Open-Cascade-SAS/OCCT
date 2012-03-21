// Created on: 2000-09-29
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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
