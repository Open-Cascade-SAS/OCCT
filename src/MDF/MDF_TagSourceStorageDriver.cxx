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

#include <MDF_TagSourceStorageDriver.ixx>
#include <PDF_TagSource.hxx>
#include <TDF_TagSource.hxx>
#include <CDM_MessageDriver.hxx>

MDF_TagSourceStorageDriver::MDF_TagSourceStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDF_TagSourceStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDF_TagSourceStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDF_TagSource); }

Handle(PDF_Attribute) MDF_TagSourceStorageDriver::NewEmpty() const
{ return new PDF_TagSource (); }

//void MDF_TagSourceStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDF_TagSourceStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDF_TagSource) S = Handle(TDF_TagSource)::DownCast (Source);
  Handle(PDF_TagSource) T = Handle(PDF_TagSource)::DownCast (Target);
  T->Set (S->Get ());
}

