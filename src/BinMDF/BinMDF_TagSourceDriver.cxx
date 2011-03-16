// File      : BinMDF_TagSourceDriver.cxx
// Created   : Tue Nov 19 12:05:04 2002
// Author    : Edward AGAPOV (eap)
// Copyright : Open CASCADE

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
