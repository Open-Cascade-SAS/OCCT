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



#include <MFunction_FunctionRetrievalDriver.ixx>

#include <PFunction_Function.hxx>
#include <TFunction_Function.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_NoSuchObject.hxx>

MFunction_FunctionRetrievalDriver::MFunction_FunctionRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MFunction_FunctionRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MFunction_FunctionRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PFunction_Function);  }

Handle(TDF_Attribute) MFunction_FunctionRetrievalDriver::NewEmpty() const
{ return new TFunction_Function(); }

void MFunction_FunctionRetrievalDriver::Paste (const Handle(PDF_Attribute)&        Source,
					       const Handle(TDF_Attribute)&        Target,
//					       const Handle(MDF_RRelocationTable)& RelocTable) const
					       const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PFunction_Function) S = Handle(PFunction_Function)::DownCast(Source);
  Handle(TFunction_Function) T = Handle(TFunction_Function)::DownCast(Target);
  T->SetDriverGUID(S->GetDriverGUID());
  T->SetFailure(S->GetFailure()); 
}
