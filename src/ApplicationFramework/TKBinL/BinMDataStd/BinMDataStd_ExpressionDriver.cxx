// Created on: 2001-09-12
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <BinMDataStd_ExpressionDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_List.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ExpressionDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_ExpressionDriver::BinMDataStd_ExpressionDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_ExpressionDriver::NewEmpty() const
{
  return (new TDataStd_Expression());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDataStd_ExpressionDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                         const occ::handle<TDF_Attribute>& theTarget,
                                         BinObjMgt_RRelocationTable&       theRelocTable) const
{
  occ::handle<TDataStd_Expression> aC = occ::down_cast<TDataStd_Expression>(theTarget);

  // variables
  int nbvar;
  if (!(theSource >> nbvar) || nbvar < 0)
    return false;
  NCollection_List<occ::handle<TDF_Attribute>>& aList = aC->GetVariables();
  for (; nbvar > 0; nbvar--)
  {
    occ::handle<TDF_Attribute> aV;
    int                        aNb;
    if (!(theSource >> aNb))
      return false;
    if (aNb > 0)
    {
      if (theRelocTable.IsBound(aNb))
        aV = occ::down_cast<TDataStd_Variable>(theRelocTable.Find(aNb));
      else
      {
        aV = new TDataStd_Variable;
        theRelocTable.Bind(aNb, aV);
      }
    }
    aList.Append(aV);
  }

  // expression
  TCollection_ExtendedString aString;
  if (!(theSource >> aString))
    return false;
  aC->SetExpression(aString);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_ExpressionDriver::Paste(
  const occ::handle<TDF_Attribute>&                        theSource,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const
{
  occ::handle<TDataStd_Expression> aC = occ::down_cast<TDataStd_Expression>(theSource);

  // variables
  const NCollection_List<occ::handle<TDF_Attribute>>& aList = aC->GetVariables();
  int                                                 nbvar = aList.Extent();
  theTarget << nbvar;
  NCollection_List<occ::handle<TDF_Attribute>>::Iterator it;
  for (it.Initialize(aList); it.More(); it.Next())
  {
    const occ::handle<TDF_Attribute>& TV = it.Value();
    int                               aNb;
    if (!TV.IsNull())
      aNb = theRelocTable.Add(TV);
    else
      aNb = -1;
    theTarget << aNb;
  }

  // expression
  TCollection_ExtendedString aName = aC->Name();
  theTarget << aName;
}
