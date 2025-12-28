// Created on: 2008-05-11
// Created by: Vlad Romashko
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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
#include <BinMFunction_ScopeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_Scope.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMFunction_ScopeDriver, BinMDF_ADriver)

//=================================================================================================

BinMFunction_ScopeDriver::BinMFunction_ScopeDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TFunction_Scope)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMFunction_ScopeDriver::NewEmpty() const
{
  return new TFunction_Scope();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMFunction_ScopeDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                     const occ::handle<TDF_Attribute>& theTarget,
                                     BinObjMgt_RRelocationTable&) const
{
  occ::handle<TFunction_Scope> S = occ::down_cast<TFunction_Scope>(theTarget);

  int nb;
  if (!(theSource >> nb))
    return false;
  if (!nb)
    return true;

  NCollection_DoubleMap<int, TDF_Label>& map = S->ChangeFunctions();

  // IDs
  NCollection_Array1<int> IDs(1, nb);
  theSource.GetIntArray(&IDs(1), nb);

  // Labels
  int freeID = 0;
  for (int i = 1; i <= nb; i++)
  {
    TCollection_AsciiString entry;
    if (!(theSource >> entry))
      return false;
    TDF_Label L;
    TDF_Tool::Label(S->Label().Data(), entry, L, true);
    if (!L.IsNull())
    {
      map.Bind(IDs.Value(i), L);
      if (IDs.Value(i) > freeID)
        freeID = IDs.Value(i);
    }
  }

  // Free ID
  freeID++;
  S->SetFreeID(freeID);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMFunction_ScopeDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                     BinObjMgt_Persistent&             theTarget,
                                     NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TFunction_Scope>                 S   = occ::down_cast<TFunction_Scope>(theSource);
  const NCollection_DoubleMap<int, TDF_Label>& map = S->GetFunctions();
  const int                                    nb  = map.Extent();

  // Number of functions
  theTarget << nb;
  if (!nb)
    return;

  // IDs
  {
    NCollection_Array1<int>                         aSourceArray(1, nb);
    NCollection_DoubleMap<int, TDF_Label>::Iterator itr(map);
    for (int i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Key1());
    }
    int* aPtr = (int*)&aSourceArray(1);
    theTarget.PutIntArray(aPtr, nb);
  }

  // Labels
  {
    NCollection_DoubleMap<int, TDF_Label>::Iterator itr(map);
    for (; itr.More(); itr.Next())
    {
      TDF_Label L = itr.Key2();
      if (!L.IsNull())
      {
        TCollection_AsciiString entry;
        TDF_Tool::Entry(L, entry);
        theTarget << entry;
      }
    }
  }
}
