// Created on: 1999-06-17
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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



#include <MDataStd_TreeNodeStorageDriver.ixx>

#include <TDataStd_TreeNode.hxx>
#include <PDataStd_TreeNode.hxx>
#include <Standard_NoSuchObject.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_TreeNodeStorageDriver::MDataStd_TreeNodeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}

Standard_Integer MDataStd_TreeNodeStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_TreeNodeStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_TreeNode); }

Handle(PDF_Attribute) MDataStd_TreeNodeStorageDriver::NewEmpty() const
{ return new PDataStd_TreeNode(); }

void MDataStd_TreeNodeStorageDriver::Paste(const Handle(TDF_Attribute)&        Source,
					    const Handle(PDF_Attribute)&        Target,
					    const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_TreeNode) S = Handle(TDataStd_TreeNode)::DownCast (Source);
  Handle(PDataStd_TreeNode) T = Handle(PDataStd_TreeNode)::DownCast (Target);

  Handle (PDataStd_TreeNode) TF;
  // Only stores myFirst , myNext
  // When retrieving the function 
  // myFirst is appended to me and the next of myFirst and subsequently the next of the next
  // are appended to me.

  Handle (TDataStd_TreeNode) F  = S->First();
  if (!F.IsNull ()) {
    if (!RelocTable->HasRelocation(F, TF)) {
      Standard_NoSuchObject::Raise("MDataStd_TreeNodeStorageDriver::Paste");
    }
    T->SetFirst(TF);
  }

  F  = S->Next();
  if (!F.IsNull ()) {
    if (!RelocTable->HasRelocation(F, TF)) {
      Standard_NoSuchObject::Raise("MDataStd_TreeNodeStorageDriver::Paste");
    }
    T->SetNext(TF);
  }
  T->SetTreeID(S->ID());
}
