// File:	BinMDataStd_AsciiStringDriver.cxx
// Created:	Wed Aug  1 12:09:29 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <BinMDataStd_AsciiStringDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMDataStd_AsciiStringDriver
//purpose  :
//=======================================================================

BinMDataStd_AsciiStringDriver::BinMDataStd_AsciiStringDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
     : BinMDF_ADriver (theMessageDriver, STANDARD_TYPE(TDataStd_AsciiString)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================

Handle(TDF_Attribute) BinMDataStd_AsciiStringDriver::NewEmpty() const
{
  return new TDataStd_AsciiString;
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataStd_AsciiStringDriver::Paste
                         (const BinObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          BinObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_AsciiString) aStrAtt = Handle(TDataStd_AsciiString)::DownCast(Target);
  TCollection_AsciiString aString;
  Standard_Boolean ok = Source >> aString;
  if (ok)
    aStrAtt->Set( aString );
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_AsciiStringDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          BinObjMgt_Persistent&        Target,
                          BinObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TDataStd_AsciiString) anAtt = Handle(TDataStd_AsciiString)::DownCast(Source);
  Target << anAtt->Get();
}
