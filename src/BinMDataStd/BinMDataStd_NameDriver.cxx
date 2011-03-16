// File      : BinMDataStd_NameDriver.cxx
// Created   : Tue Nov 19 12:30:18 2002
// Author    : Edward AGAPOV (eap)
// Copyright : Open CASCADE


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
