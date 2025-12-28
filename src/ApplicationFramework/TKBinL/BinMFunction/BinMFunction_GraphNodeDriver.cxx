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
#include <BinMFunction_GraphNodeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TDF_Attribute.hxx>
#include <TFunction_GraphNode.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMFunction_GraphNodeDriver, BinMDF_ADriver)

//=================================================================================================

BinMFunction_GraphNodeDriver::BinMFunction_GraphNodeDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TFunction_GraphNode)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMFunction_GraphNodeDriver::NewEmpty() const
{
  return new TFunction_GraphNode();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMFunction_GraphNodeDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                         const occ::handle<TDF_Attribute>& theTarget,
                                         BinObjMgt_RRelocationTable&) const
{
  occ::handle<TFunction_GraphNode> GN = occ::down_cast<TFunction_GraphNode>(theTarget);

  int intStatus, nb_previous, nb_next;
  if (!(theSource >> intStatus >> nb_previous >> nb_next))
    return false;

  // Execution status
  GN->SetStatus((TFunction_ExecutionStatus)intStatus);

  // Previous functions
  if (nb_previous)
  {
    NCollection_Array1<int> aTargetArray(1, nb_previous);
    theSource.GetIntArray(&aTargetArray(1), nb_previous);

    for (int i = 1; i <= nb_previous; i++)
    {
      GN->AddPrevious(aTargetArray.Value(i));
    }
  }

  // Next functions
  if (nb_next)
  {
    NCollection_Array1<int> aTargetArray(1, nb_next);
    theSource.GetIntArray(&aTargetArray(1), nb_next);

    for (int i = 1; i <= nb_next; i++)
    {
      GN->AddNext(aTargetArray.Value(i));
    }
  }

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMFunction_GraphNodeDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TFunction_GraphNode> GN = occ::down_cast<TFunction_GraphNode>(theSource);

  // Execution status
  theTarget << (int)GN->GetStatus();
  // Number of previous functions
  theTarget << GN->GetPrevious().Extent();
  // Number of next functions
  theTarget << GN->GetNext().Extent();

  // Previous functions
  int nb = GN->GetPrevious().Extent();
  if (nb)
  {
    NCollection_Array1<int>        aSourceArray(1, nb);
    NCollection_Map<int>::Iterator itr(GN->GetPrevious());
    for (int i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Key());
    }
    int* aPtr = (int*)&aSourceArray(1);
    theTarget.PutIntArray(aPtr, nb);
  }

  // Next functions
  nb = GN->GetNext().Extent();
  if (nb)
  {
    NCollection_Array1<int>        aSourceArray(1, nb);
    NCollection_Map<int>::Iterator itr(GN->GetNext());
    for (int i = 1; itr.More(); itr.Next(), i++)
    {
      aSourceArray.SetValue(i, itr.Key());
    }
    int* aPtr = (int*)&aSourceArray(1);
    theTarget.PutIntArray(aPtr, nb);
  }
}
