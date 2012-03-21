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


#include <MXCAFDoc_MaterialRetrievalDriver.ixx>
#include <PXCAFDoc_Material.hxx>
#include <XCAFDoc_Material.hxx>

#include <TCollection_HAsciiString.hxx>
#include <PCollection_HAsciiString.hxx>


//=======================================================================
//function : MCAFDoc_MaterialRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_MaterialRetrievalDriver::MXCAFDoc_MaterialRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_MaterialRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_MaterialRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Material);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_MaterialRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Material();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_MaterialRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
                                             const Handle(TDF_Attribute)& Target,
                                             const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Material) S = Handle(PXCAFDoc_Material)::DownCast (Source);
  Handle(XCAFDoc_Material) T = Handle(XCAFDoc_Material)::DownCast (Target);
  
  Handle(TCollection_HAsciiString) aName, aDescr, aDensName, aDensValType;
  if ( !S->GetName().IsNull() )
    aName = new TCollection_HAsciiString( (S->GetName())->Convert() );
  if ( !S->GetDescription().IsNull() )
    aDescr = new TCollection_HAsciiString( (S->GetDescription())->Convert() );
  if ( !S->GetDensName().IsNull() )
    aDensName = new TCollection_HAsciiString( (S->GetDensName())->Convert() );
  if ( !S->GetDensValType().IsNull() )
    aDensValType = new TCollection_HAsciiString( (S->GetDensValType())->Convert() );

  T->Set(aName, aDescr, S->GetDensity(), aDensName, aDensValType);
}
