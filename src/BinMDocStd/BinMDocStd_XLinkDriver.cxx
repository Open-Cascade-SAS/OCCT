// File:	BinMDocStd_XLinkDriver.cxx
// Created:	Thu May 13 16:11:34 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004


#include <BinMDocStd_XLinkDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_XLink.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
//=======================================================================
//function : BinMDocStd_XLinkDriver
//purpose  : 
//=======================================================================

BinMDocStd_XLinkDriver::BinMDocStd_XLinkDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDocStd_XLink)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDocStd_XLinkDriver::NewEmpty() const
{
  return new TDocStd_XLink();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDocStd_XLinkDriver::Paste
                                (const BinObjMgt_Persistent& theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable& ) const
{
  TCollection_AsciiString aStr;
  Standard_Boolean ok = theSource >> aStr;
  if(ok) {
    Handle(TDocStd_XLink) anAtt = Handle(TDocStd_XLink)::DownCast(theTarget);
    anAtt->DocumentEntry(aStr);
    aStr.Clear();
    ok = theSource >> aStr;
    if(ok)
      anAtt->LabelEntry(aStr);
  }
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDocStd_XLinkDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent& theTarget,
                                       BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDocStd_XLink) anAtt = Handle(TDocStd_XLink)::DownCast(theSource);
  theTarget << anAtt->DocumentEntry() << anAtt->LabelEntry();
}


