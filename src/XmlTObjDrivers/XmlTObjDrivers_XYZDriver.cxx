// Created on: 2004-11-24
// Created by: Edward AGAPOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A


#include <XmlTObjDrivers_XYZDriver.hxx>

#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>

#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>

#include <TObj_TXYZ.hxx>

IMPLEMENT_STANDARD_HANDLE(XmlTObjDrivers_XYZDriver,XmlMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_XYZDriver,XmlMDF_ADriver)

IMPLEMENT_DOMSTRING (CoordX,             "X")
IMPLEMENT_DOMSTRING (CoordY,             "Y")
IMPLEMENT_DOMSTRING (CoordZ,             "Z")

//=======================================================================
//function : XmlTObjDrivers_XYZDriver
//purpose  : constructor
//=======================================================================

XmlTObjDrivers_XYZDriver::XmlTObjDrivers_XYZDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: XmlMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) XmlTObjDrivers_XYZDriver::NewEmpty() const
{
  return new TObj_TXYZ;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//=======================================================================

Standard_Boolean XmlTObjDrivers_XYZDriver::Paste
                         (const XmlObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          XmlObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  const XmlObjMgt_Element& anElement = Source;
  
  // get co-ordinates
  TCollection_AsciiString CoordX = anElement.getAttribute(::CoordX());
  TCollection_AsciiString CoordY = anElement.getAttribute(::CoordY());
  TCollection_AsciiString CoordZ = anElement.getAttribute(::CoordZ());

  // creating gp_XYZ
  gp_XYZ aXYZ;
  Standard_CString aStr;
  Standard_Real aCoord;

  aStr = CoordX.ToCString();
  if(!XmlObjMgt::GetReal( aStr, aCoord )) return Standard_False;
  aXYZ.SetX(aCoord);

  aStr = CoordY.ToCString();
  if(!XmlObjMgt::GetReal( aStr, aCoord )) return Standard_False;
  aXYZ.SetY(aCoord);

  aStr = CoordZ.ToCString();
  if(!XmlObjMgt::GetReal( aStr, aCoord )) return Standard_False;
  aXYZ.SetZ(aCoord);

  // setting gp_XYZ
  Handle(TObj_TXYZ) aTarget = Handle(TObj_TXYZ)::DownCast (Target);
  aTarget->Set ( aXYZ );

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           Store master and referred labels as entry, the other model referred
//           as entry in model-container
//=======================================================================

void XmlTObjDrivers_XYZDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          XmlObjMgt_Persistent&        Target,
                          XmlObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TObj_TXYZ) aSource =
    Handle(TObj_TXYZ)::DownCast (Source);

  if(aSource.IsNull()) return;

  gp_XYZ aXYZ = aSource->Get();

  TCollection_AsciiString aCoord;

  // co-ordinate X
  aCoord = TCollection_AsciiString( aXYZ.X() );
  Target.Element().setAttribute(::CoordX(), aCoord.ToCString());

  // co-ordinate Y
  aCoord = TCollection_AsciiString( aXYZ.Y() );
  Target.Element().setAttribute(::CoordY(), aCoord.ToCString());

  // co-ordinate Z
  aCoord = TCollection_AsciiString( aXYZ.Z() );
  Target.Element().setAttribute(::CoordZ(), aCoord.ToCString());
}
