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


#include <BinMDataStd_ExtStringListDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfExtendedString.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ExtStringListDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataStd_ExtStringListDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_ExtStringListDriver::BinMDataStd_ExtStringListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataStd_ExtStringList)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_ExtStringListDriver::NewEmpty() const
{
  return new TDataStd_ExtStringList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataStd_ExtStringListDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  if (! (theSource >> aFirstInd >> aLastInd))
    return Standard_False;

  const Handle(TDataStd_ExtStringList) anAtt = Handle(TDataStd_ExtStringList)::DownCast(theTarget);
  if(aLastInd > 0) {
    const Standard_Integer aLength = aLastInd - aFirstInd + 1;
    if (aLength <= 0)
      return Standard_False;
    for (Standard_Integer i = aFirstInd; i <= aLastInd; i ++)
    {
      TCollection_ExtendedString aStr;
      if ( !(theSource >> aStr) )
      {
        return Standard_False;
      }
      anAtt->Append(aStr);
    }
  }

  BinMDataStd::SetAttributeID(theSource, anAtt);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_ExtStringListDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 BinObjMgt_Persistent&        theTarget,
                                 BinObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_ExtStringList) anAtt =
    Handle(TDataStd_ExtStringList)::DownCast(theSource);
  const Standard_Integer aFirstInd = (anAtt->Extent()> 0) ? 1 : 0;
  const Standard_Integer aLastInd(anAtt->Extent());
  theTarget << aFirstInd << aLastInd;
  TDataStd_ListIteratorOfListOfExtendedString itr(anAtt->List());
  for (; itr.More(); itr.Next())
  {
    theTarget << itr.Value();
  }

  // process user defined guid
  if(anAtt->ID() != TDataStd_ExtStringList::GetID()) 
    theTarget << anAtt->ID();
}
