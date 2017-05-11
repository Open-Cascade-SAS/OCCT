// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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


#include <BinMDataStd_ReferenceArrayDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ReferenceArrayDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_ReferenceArrayDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_ReferenceArrayDriver::BinMDataStd_ReferenceArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_ReferenceArray)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_ReferenceArrayDriver::NewEmpty() const
{
  return new TDataStd_ReferenceArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_ReferenceArrayDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                         const Handle(TDF_Attribute)& theTarget,
                                                         BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;
  const Standard_Integer aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return Standard_False;

  const Handle(TDataStd_ReferenceArray) anAtt = Handle(TDataStd_ReferenceArray)::DownCast(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  for (Standard_Integer i = aFirstInd; i <= aLastInd; i++)
  {
    TCollection_AsciiString entry;
    if ( !(theSource >> entry) )
      return Standard_False;
    TDF_Label L;
    TDF_Tool::Label(anAtt->Label().Data(), entry, L, Standard_True);
    if (!L.IsNull())
      anAtt->SetValue(i, L);
  }

  BinMDataStd::SetAttributeID(theSource, anAtt);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_ReferenceArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                             BinObjMgt_Persistent&        theTarget,
                                             BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ReferenceArray) anAtt = Handle(TDataStd_ReferenceArray)::DownCast(theSource);
  Standard_Integer aFirstInd = anAtt->Lower(), aLastInd = anAtt->Upper(), i = aFirstInd;
  if (aFirstInd > aLastInd)
    return;
  theTarget << aFirstInd << aLastInd;
  for (; i <= aLastInd; i++)
  {
    TDF_Label L = anAtt->Value(i);
    if (!L.IsNull())
    {
      TCollection_AsciiString entry;
      TDF_Tool::Entry(L, entry);
      theTarget << entry;
    }
  }

  // process user defined guid
  if(anAtt->ID() != TDataStd_ReferenceArray::GetID()) 
    theTarget << anAtt->ID();
}
