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


#include <BinMDF_TagSourceDriver.ixx>
#include <TDF_TagSource.hxx>

//=======================================================================
//function : BinMDF_TagSourceDriver
//purpose  : Constructor
//=======================================================================

BinMDF_TagSourceDriver::BinMDF_TagSourceDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDF_TagSourceDriver::NewEmpty() const
{
  return (new TDF_TagSource());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDF_TagSourceDriver::Paste 
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDF_TagSource) aTag = Handle(TDF_TagSource)::DownCast(theTarget);
  Standard_Integer aValue;
  Standard_Boolean ok = theSource >> aValue;
  if (ok)
    aTag->Set(aValue);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDF_TagSourceDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    BinObjMgt_Persistent&        theTarget,
                                    BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDF_TagSource) aTag = Handle(TDF_TagSource)::DownCast(theSource);
  theTarget << aTag->Get();
}
