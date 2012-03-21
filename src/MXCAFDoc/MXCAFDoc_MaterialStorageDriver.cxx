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


#include <MXCAFDoc_MaterialStorageDriver.ixx>
#include <XCAFDoc_Material.hxx>
#include <PXCAFDoc_Material.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <PCollection_HAsciiString.hxx>
#include <PColStd_HArray1OfReal.hxx>

//=======================================================================
//function : MXCAFDoc_MaterialStorageDriver
//purpose  : 
//=======================================================================

MXCAFDoc_MaterialStorageDriver::MXCAFDoc_MaterialStorageDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ASDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_MaterialStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_MaterialStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(XCAFDoc_Material);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MXCAFDoc_MaterialStorageDriver::NewEmpty() const
{
  return new PXCAFDoc_Material();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_MaterialStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
                                         const Handle(PDF_Attribute)& Target,
                                         const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(XCAFDoc_Material) S = Handle(XCAFDoc_Material)::DownCast (Source);
  Handle(PXCAFDoc_Material) T = Handle(PXCAFDoc_Material)::DownCast (Target);
  Handle(TCollection_HAsciiString) aNameStr = S->GetName();
  Handle(TCollection_HAsciiString) aDescrStr = S->GetDescription();
  Handle(TCollection_HAsciiString) aDensNameStr = S->GetDensName();
  Handle(TCollection_HAsciiString) aDensValTypeStr = S->GetDensValType();
  
  Handle(PCollection_HAsciiString) aName, aDescr, aDensName, aDensValType;
  if ( !aNameStr.IsNull() )
    aName = new PCollection_HAsciiString (aNameStr->String());
  if ( !aDescrStr.IsNull() )
    aDescr = new PCollection_HAsciiString (aDescrStr->String());
  if ( !aDensNameStr.IsNull() )
    aDensName = new PCollection_HAsciiString (aDensNameStr->String());
  if ( !aDensValTypeStr.IsNull() )
    aDensValType = new PCollection_HAsciiString (aDensValTypeStr->String());

  T->Set(aName, aDescr, S->GetDensity(), aDensName, aDensValType);
}
