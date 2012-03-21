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



#include <MDataStd_TreeNodeRetrievalDriver.ixx>
#include <PDataStd_TreeNode.hxx>
#include <TDataStd_TreeNode.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TDF_Label.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_TreeNodeRetrievalDriver::MDataStd_TreeNodeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MDataStd_TreeNodeRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_TreeNodeRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_TreeNode);  }

Handle(TDF_Attribute) MDataStd_TreeNodeRetrievalDriver::NewEmpty() const
{ return new TDataStd_TreeNode(); }

void MDataStd_TreeNodeRetrievalDriver::Paste (const Handle(PDF_Attribute)&        Source,
					      const Handle(TDF_Attribute)&        Target,
					      const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataStd_TreeNode) S = Handle(PDataStd_TreeNode)::DownCast(Source);
  Handle(TDataStd_TreeNode) T = Handle(TDataStd_TreeNode)::DownCast(Target);
  T->SetTreeID(S->GetTreeID());

   Handle(TDataStd_TreeNode) TFirst;
   Handle(PDataStd_TreeNode) PFirst = S->First(); 
   if (!PFirst.IsNull ()) {
     if (!RelocTable->HasRelocation(PFirst, TFirst)) { 
       Standard_NoSuchObject::Raise("MDataStd_TreeNodeRetrievalDriver::Paste");
     }
     if (!TFirst.IsNull()) {
       TFirst->SetTreeID(S->GetTreeID());
       T->Append(TFirst);
       Handle(PDataStd_TreeNode) PNext = PFirst; 
       Handle(TDataStd_TreeNode) TNext;
       while (!(PNext->Next()).IsNull() ) {
	 PNext = PNext->Next(); 
	 if (!RelocTable->HasRelocation(PNext, TNext)) { 
	   Standard_NoSuchObject::Raise("MDataStd_TreeNodeRetrievalDriver::Paste");
	 }
	 if (!TNext.IsNull()) {
	   TNext->SetTreeID(S->GetTreeID());
	   T->Append(TNext);
	 }
       }
     }
   }
}
