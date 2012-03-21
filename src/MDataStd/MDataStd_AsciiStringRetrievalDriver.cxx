// Created on: 2007-08-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <MDataStd_AsciiStringRetrievalDriver.ixx>
#include <PDataStd_AsciiString.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <PCollection_HAsciiString.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_AsciiStringRetrievalDriver
//purpose  : Constructor
//=======================================================================
MDataStd_AsciiStringRetrievalDriver::MDataStd_AsciiStringRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}
//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_AsciiStringRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_AsciiStringRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_AsciiString); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) MDataStd_AsciiStringRetrievalDriver::NewEmpty() const
{ return new TDataStd_AsciiString (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_AsciiStringRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_AsciiString) S = Handle(PDataStd_AsciiString)::DownCast (Source);
  Handle(TDataStd_AsciiString) T = Handle(TDataStd_AsciiString)::DownCast (Target);
  if(!S.IsNull() && !T.IsNull()) {
    if(!S->Get().IsNull()) {
      TCollection_AsciiString aString((S->Get())->Convert ());
      T->Set (aString);
    }
  }
}

