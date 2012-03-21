// Created on: 2002-11-19
// Created by: Edward AGAPOV (eap)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#include <BinMDataStd_NameDriver.ixx>
#include <TDataStd_Name.hxx>

//=======================================================================
//function : BinMDataStd_NameDriver
//purpose  :
//=======================================================================

BinMDataStd_NameDriver::BinMDataStd_NameDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
     : BinMDF_ADriver (theMessageDriver, STANDARD_TYPE(TDataStd_Name)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_NameDriver::NewEmpty() const
{
  return new TDataStd_Name;
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_NameDriver::Paste
                         (const BinObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          BinObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(Target);
  TCollection_ExtendedString aStr;
  Standard_Boolean ok = Source >> aStr;
  if (ok)
    aName->Set( aStr );
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_NameDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(Source);
  Target << aName->Get();
}
