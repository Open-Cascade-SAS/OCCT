// File:      BinMDataStd_IntegerDriver.cxx
// Created:   30.10.02 14:56:34
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDataStd_IntegerDriver.ixx>
#include <TDataStd_Integer.hxx>

//=======================================================================
//function : BinMDataStd_IntegerDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_IntegerDriver::BinMDataStd_IntegerDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_Integer)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_IntegerDriver::NewEmpty() const
{
  return new TDataStd_Integer();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_IntegerDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_Integer) anAtt = Handle(TDataStd_Integer)::DownCast(theTarget);
  Standard_Integer aValue;
  Standard_Boolean ok = theSource >> aValue;
  if (ok)
    anAtt->Set(aValue);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_IntegerDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Integer) anAtt = Handle(TDataStd_Integer)::DownCast(theSource);
  theTarget << anAtt->Get();
}
