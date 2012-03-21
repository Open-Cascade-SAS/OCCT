// Created on: 2000-09-29
// Created by: Pavel TELKOV
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



#include <MXCAFDoc_GraphNodeRetrievalDriver.ixx>
#include <PXCAFDoc_GraphNode.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

MXCAFDoc_GraphNodeRetrievalDriver::MXCAFDoc_GraphNodeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_GraphNodeRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_GraphNodeRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PXCAFDoc_GraphNode);  }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_GraphNodeRetrievalDriver::NewEmpty() const
{ return new XCAFDoc_GraphNode(); }


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_GraphNodeRetrievalDriver::Paste (const Handle(PDF_Attribute)&        Source,
					      const Handle(TDF_Attribute)&        Target,
					      const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_GraphNode) S = Handle(PXCAFDoc_GraphNode)::DownCast(Source);
  Handle(XCAFDoc_GraphNode) T = Handle(XCAFDoc_GraphNode)::DownCast(Target);
  T->SetGraphID(S->GetGraphID());

  Handle(XCAFDoc_GraphNode) TFirst;
  Handle(PXCAFDoc_GraphNode) PFirst;
  Standard_Integer i;
  for ( i = 1; i <= S->NbFathers(); i++) {
    PFirst = S->GetFather(i); 
    if ( !PFirst.IsNull() ) {
      if (!RelocTable->HasRelocation(PFirst, TFirst)) { 
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeRetrievalDriver::Paste");
	}
      if (!TFirst.IsNull()) {
	TFirst->SetGraphID(S->GetGraphID());
	T->SetFather(TFirst);
      }
    }
  }

  for ( i = 1; i <= S->NbChildren(); i++) {
    PFirst = S->GetChild(i);
    if ( !PFirst.IsNull() ) {
      if (!RelocTable->HasRelocation(PFirst, TFirst)) { 
	Standard_NoSuchObject::Raise("MXCAFDoc_GraphNodeRetrievalDriver::Paste");
	}
      if (!TFirst.IsNull()) {
	TFirst->SetGraphID(S->GetGraphID());
	T->SetChild(TFirst);
      }
    }
  }
  
  
}
