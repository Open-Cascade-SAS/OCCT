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


#include <BinMXCAFDoc_DatumDriver.ixx>
#include <XCAFDoc_Datum.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_DatumDriver::BinMXCAFDoc_DatumDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Datum)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_DatumDriver::NewEmpty() const
{
  return new XCAFDoc_Datum();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_DatumDriver::Paste(const BinObjMgt_Persistent& theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable& /*theRelocTable*/) const 
{
  Handle(XCAFDoc_Datum) anAtt = Handle(XCAFDoc_Datum)::DownCast(theTarget);
  TCollection_AsciiString aName, aDescr, anId;
  if ( !(theSource >> aName >> aDescr >> anId) )
    return Standard_False;

  anAtt->Set(new TCollection_HAsciiString( aName ),
             new TCollection_HAsciiString( aDescr ),
             new TCollection_HAsciiString( anId ));
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_DatumDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent& theTarget,
                                     BinObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  Handle(XCAFDoc_Datum) anAtt = Handle(XCAFDoc_Datum)::DownCast(theSource);
  if ( !anAtt->GetName().IsNull() )
    theTarget << anAtt->GetName()->String();
  else
    theTarget << TCollection_AsciiString("");

  if ( !anAtt->GetDescription().IsNull() )
    theTarget << anAtt->GetDescription()->String();
  else
    theTarget << TCollection_AsciiString("");

  if ( !anAtt->GetIdentification().IsNull() )
    theTarget << anAtt->GetIdentification()->String();
  else
    theTarget << TCollection_AsciiString("");
}
