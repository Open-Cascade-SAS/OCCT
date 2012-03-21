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

#include <MDF_TagSourceRetrievalDriver.ixx>
#include <PDF_TagSource.hxx>
#include <TDF_TagSource.hxx>
#include <CDM_MessageDriver.hxx>

MDF_TagSourceRetrievalDriver::MDF_TagSourceRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDF_TagSourceRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDF_TagSourceRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDF_TagSource); }

Handle(TDF_Attribute) MDF_TagSourceRetrievalDriver::NewEmpty() const
{ return new TDF_TagSource; }

//void MDF_TagSourceRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
void MDF_TagSourceRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDF_TagSource) S = Handle(PDF_TagSource)::DownCast (Source);
  Handle(TDF_TagSource) T = Handle(TDF_TagSource)::DownCast (Target);
  T->Set (S->Get ());
}

