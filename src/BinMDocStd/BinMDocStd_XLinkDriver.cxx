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


