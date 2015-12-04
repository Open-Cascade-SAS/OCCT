// Created on: 2004-05-13
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BinMDF_ADriver.hxx>
#include <BinMDocStd_XLinkDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_XLink.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDocStd_XLinkDriver,BinMDF_ADriver)

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


