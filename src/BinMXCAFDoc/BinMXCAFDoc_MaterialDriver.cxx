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


#include <BinMXCAFDoc_MaterialDriver.ixx>
#include <XCAFDoc_Material.hxx>

#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_MaterialDriver::BinMXCAFDoc_MaterialDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Material)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_MaterialDriver::NewEmpty() const
{
  return new XCAFDoc_Material();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_MaterialDriver::Paste(const BinObjMgt_Persistent& theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  Handle(XCAFDoc_Material) anAtt = Handle(XCAFDoc_Material)::DownCast(theTarget);
  Standard_Real aDensity;
  TCollection_AsciiString aName, aDescr, aDensName, aDensValType;
  if ( !(theSource >> aName >> aDescr >> aDensity >> aDensName >> aDensValType) )
    return Standard_False;

  anAtt->Set(new TCollection_HAsciiString( aName ),
             new TCollection_HAsciiString( aDescr ),
             aDensity,
             new TCollection_HAsciiString( aDensName ),
             new TCollection_HAsciiString( aDensValType ));
  return Standard_True;
}

static void pasteString( BinObjMgt_Persistent& theTarget,
                         Handle(TCollection_HAsciiString) theStr )
{
  if ( !theStr.IsNull() )
    theTarget << theStr->String();
  else
    theTarget << TCollection_AsciiString("");
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_MaterialDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent& theTarget,
                                     BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Material) anAtt = Handle(XCAFDoc_Material)::DownCast(theSource);
  pasteString( theTarget, anAtt->GetName() );
  pasteString( theTarget, anAtt->GetDescription() );
  theTarget << anAtt->GetDensity();
  pasteString( theTarget, anAtt->GetDensName() );
  pasteString( theTarget, anAtt->GetDensValType() );
}
