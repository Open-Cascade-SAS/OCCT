// File:	BinMFunction_FunctionDriver.cxx
// Created:	Thu May 13 14:46:12 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004


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

