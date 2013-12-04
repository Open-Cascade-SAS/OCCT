// Created on: 1999-06-17
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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
