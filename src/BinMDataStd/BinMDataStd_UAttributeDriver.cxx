// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
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


#include <BinMDataStd_UAttributeDriver.ixx>
#include <TDataStd_UAttribute.hxx>

//=======================================================================
//function : BinMDataStd_UAttributeDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_UAttributeDriver::BinMDataStd_UAttributeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_UAttributeDriver::NewEmpty() const
{
  return (new TDataStd_UAttribute());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMDataStd_UAttributeDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_UAttribute) anUAttr =
    Handle(TDataStd_UAttribute)::DownCast (theTarget);
  Standard_GUID aGUID;
  Standard_Boolean ok = theSource >> aGUID;
  if (ok)
    anUAttr -> SetID (aGUID);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMDataStd_UAttributeDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                         BinObjMgt_Persistent&        theTarget,
                                         BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_UAttribute) anUAttr =
    Handle(TDataStd_UAttribute)::DownCast(theSource);
  theTarget << anUAttr->ID();
}
