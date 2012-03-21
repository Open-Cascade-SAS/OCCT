// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <MXCAFDoc_DatumRetrievalDriver.ixx>
#include <PXCAFDoc_Datum.hxx>
#include <XCAFDoc_Datum.hxx>

#include <TCollection_HAsciiString.hxx>
#include <PCollection_HAsciiString.hxx>


//=======================================================================
//function : MCAFDoc_DatumRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_DatumRetrievalDriver::MXCAFDoc_DatumRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_DatumRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_DatumRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Datum);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_DatumRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Datum();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DatumRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
                                           const Handle(TDF_Attribute)& Target,
                                           const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Datum) S = Handle(PXCAFDoc_Datum)::DownCast (Source);
  Handle(XCAFDoc_Datum) T = Handle(XCAFDoc_Datum)::DownCast (Target);
  
  Handle(TCollection_HAsciiString) aName, aDescr, anId;
  if ( !S->GetName().IsNull() )
    aName = new TCollection_HAsciiString( (S->GetName())->Convert() );
  if ( !S->GetDescription().IsNull() )
    aDescr = new TCollection_HAsciiString( (S->GetDescription())->Convert() );
  if ( !S->GetIdentification().IsNull() )
    anId = new TCollection_HAsciiString( (S->GetIdentification())->Convert() );

  T->Set(aName, aDescr, anId);
}
