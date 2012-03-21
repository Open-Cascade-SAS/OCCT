// Copyright (c) 1997-1999 Matra Datavision
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

#include <MDataStd_NameRetrievalDriver.ixx>
#include <PDataStd_Name.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MessageDriver.hxx>

MDataStd_NameRetrievalDriver::MDataStd_NameRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_NameRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_NameRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Name); }

Handle(TDF_Attribute) MDataStd_NameRetrievalDriver::NewEmpty() const
{ return new TDataStd_Name (); }


#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>

#define BUGOCC425

void MDataStd_NameRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_Name) S = Handle(PDataStd_Name)::DownCast (Source);
  Handle(TDataStd_Name) T = Handle(TDataStd_Name)::DownCast (Target);
#ifdef BUGOCC425
  if(!S->Get().IsNull()) {
#endif 
    TCollection_ExtendedString Name = (S->Get())->Convert ();
    T->Set (Name);
#ifdef BUGOCC425
  }
#endif 
}

