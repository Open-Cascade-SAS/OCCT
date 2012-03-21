// Created on: 2004-05-13
// Created by: Sergey ZARITCHNY
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



#include <BinMFunction_FunctionDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TFunction_Function.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMFunction_FunctionDriver
//purpose  : 
//=======================================================================

BinMFunction_FunctionDriver::BinMFunction_FunctionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TFunction_Function)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMFunction_FunctionDriver::NewEmpty() const
{
  return new TFunction_Function();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMFunction_FunctionDriver::Paste
                                (const BinObjMgt_Persistent& theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable& ) const
{

  Handle(TFunction_Function) anAtt = Handle(TFunction_Function)::DownCast(theTarget);
  Standard_GUID aGUID("00000000-0000-0000-0000-000000000000");
  Standard_Boolean ok = theSource >> aGUID;
  if (ok) {
    anAtt->SetDriverGUID(aGUID);  
    Standard_Integer aValue;
    ok = theSource >> aValue;
    if(ok)
      anAtt->SetFailure(aValue); 
  }
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMFunction_FunctionDriver::Paste (const Handle(TDF_Attribute)& theSource,
					 BinObjMgt_Persistent& theTarget,
					 BinObjMgt_SRelocationTable&  ) const
{
  Handle(TFunction_Function) aS = Handle(TFunction_Function)::DownCast (theSource);
  theTarget << aS->GetDriverGUID();
  theTarget << aS->GetFailure();
}

