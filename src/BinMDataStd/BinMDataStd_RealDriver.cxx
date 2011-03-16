// File:      BinMDataStd_RealDriver.cxx
// Created:   30.10.02 14:56:34
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDataStd_RealDriver.ixx>
#include <TDataStd_Real.hxx>

//=======================================================================
//function : BinMDataStd_RealDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_RealDriver::BinMDataStd_RealDriver 
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_Real)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_RealDriver::NewEmpty() const
{
  return new TDataStd_Real();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_RealDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_Real) anAtt= Handle(TDataStd_Real)::DownCast(theTarget);
  Standard_Real aValue;
  Standard_Boolean ok = theSource >> aValue;
  if (ok)
    anAtt->Set(aValue);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_RealDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    BinObjMgt_Persistent&        theTarget,
                                    BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Real) anAtt= Handle(TDataStd_Real)::DownCast(theSource);
  theTarget << anAtt->Get();
}
