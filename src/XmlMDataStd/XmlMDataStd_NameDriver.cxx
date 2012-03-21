// Created on: 2001-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlMDataStd_NameDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <TDataStd_Name.hxx>

//=======================================================================
//function : XmlMDataStd_NameDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_NameDriver::XmlMDataStd_NameDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_NameDriver::NewEmpty () const
{
  return (new TDataStd_Name());
}

//=======================================================================
//function : Paste()
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_NameDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString (theSource, aString))
  {
    Handle(TDataStd_Name)::DownCast(theTarget) -> Set (aString);
    return Standard_True;
  }
  WriteMessage("error retrieving ExtendedString for type TDataStd_Name");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : 
//=======================================================================
void XmlMDataStd_NameDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(theSource);
  XmlObjMgt::SetExtendedString (theTarget, aName -> Get());
}
