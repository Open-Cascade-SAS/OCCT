// Created on: 2008-06-23
// Created by: Vladislav ROMASHKO
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

#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_Iterator.hxx>
#include <TFunction_Scope.hxx>

//=================================================================================================

TFunction_Iterator::TFunction_Iterator()
    : myUsageOfExecutionStatus(false)
{
}

//=================================================================================================

TFunction_Iterator::TFunction_Iterator(const TDF_Label& Access)
    : myUsageOfExecutionStatus(false)
{
  Init(Access);
}

//=======================================================================
// function : Init
// purpose  : Initializes the Iterator.
//=======================================================================

void TFunction_Iterator::Init(const TDF_Label& Access)
{
  myCurrent.Clear();
  myPassedFunctions.Clear();

  // Get the scope of functions
  myScope = TFunction_Scope::Set(Access);

  // Find the roots
  NCollection_DoubleMap<int, TDF_Label>::Iterator itrm(myScope->GetFunctions());
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label& L = itrm.Key2();

    TFunction_IFunction              iFunction(L);
    occ::handle<TFunction_GraphNode> graphNode = iFunction.GetGraphNode();
    TFunction_ExecutionStatus        status    = graphNode->GetStatus();

    // Check whether the function is a root function
    if (!graphNode->GetPrevious().IsEmpty())
      continue;

    // In execution mode we consider only "not executed" functions.
    if (myUsageOfExecutionStatus && status != TFunction_ES_NotExecuted)
      continue;

    myCurrent.Append(L);

    // Register already passed functions
    if (!myUsageOfExecutionStatus)
      myPassedFunctions.Add(L);
  }
}

//=======================================================================
// function : SetUsageOfExecutionStatus
// purpose  : Defines usage of execution status
//=======================================================================

void TFunction_Iterator::SetUsageOfExecutionStatus(const bool usage)
{
  myUsageOfExecutionStatus = usage;
}

//=======================================================================
// function : GetUsageOfExecutionStatus
// purpose  : Returns usage of execution status
//=======================================================================

bool TFunction_Iterator::GetUsageOfExecutionStatus() const
{
  return myUsageOfExecutionStatus;
}

//=======================================================================
// function : GetMaxNbThreads
// purpose  : Defines the maximum number of threads
//=======================================================================

int TFunction_Iterator::GetMaxNbThreads() const
{
  int                nb_threads = 0;
  TFunction_Iterator fIterator;
  fIterator.myUsageOfExecutionStatus = false;

  // Start iteration from current functions
  NCollection_List<TDF_Label>::Iterator itrl(myCurrent);
  for (; itrl.More(); itrl.Next())
  {
    fIterator.myCurrent.Append(itrl.Value());
  }

  // Check number of semultenious current functions
  while (!fIterator.Current().IsEmpty())
  {
    const NCollection_List<TDF_Label>& current = fIterator.Current();
    if (nb_threads < current.Extent())
      nb_threads = current.Extent();
    fIterator.Next();
  }

  return nb_threads;
}

//=======================================================================
// function : Current
// purpose  : Returns the current list of functions
//=======================================================================

const NCollection_List<TDF_Label>& TFunction_Iterator::Current() const
{
  return myCurrent;
}

//=======================================================================
// function : More
// purpose  : Returns true if the iteration is ended
//=======================================================================

bool TFunction_Iterator::More() const
{
  if (myUsageOfExecutionStatus)
  {
    NCollection_DoubleMap<int, TDF_Label>::Iterator itrm(myScope->GetFunctions());
    for (; itrm.More(); itrm.Next())
    {
      const TDF_Label& L = itrm.Key2();
      if (GetStatus(L) == TFunction_ES_NotExecuted)
        return true;
    }
    return false;
  }
  return !myCurrent.IsEmpty();
}

//=======================================================================
// function : Next
// purpose  : Switches the iterator to the next functions
//=======================================================================

void TFunction_Iterator::Next()
{
  NCollection_Map<TDF_Label>            next_current;
  NCollection_List<TDF_Label>::Iterator itrl(myCurrent);
  for (; itrl.More(); itrl.Next())
  {
    const TDF_Label&    L = itrl.Value();
    TFunction_IFunction iFunction(L);

    occ::handle<TFunction_GraphNode> graphNode = iFunction.GetGraphNode();
    const NCollection_Map<int>&      next      = graphNode->GetNext();
    TFunction_ExecutionStatus        status    = graphNode->GetStatus();

    // Consider the execution status
    if (myUsageOfExecutionStatus)
    {
      if (status == TFunction_ES_NotExecuted || status == TFunction_ES_Executing)
      {
        next_current.Add(L);
        continue;
      }
      else if (status == TFunction_ES_WrongDefinition || status == TFunction_ES_Failed)
      {
        continue;
      }

      // if "succeeded", we consider the next functions... see below.
    }

    // Add next functions
    NCollection_Map<int>::Iterator itrm(next);
    for (; itrm.More(); itrm.Next())
    {
      const int        IDnext = itrm.Key();
      const TDF_Label& Lnext  = myScope->GetFunctions().Find1(IDnext);

      if (myUsageOfExecutionStatus)
      {
        // A previous function is "succeeded", check status of next functions and
        // all other previous functions of the next functions.

        // Check status, it should be "not executed"
        TFunction_IFunction              iNextFunction(Lnext);
        occ::handle<TFunction_GraphNode> nextGraphNode = iNextFunction.GetGraphNode();
        TFunction_ExecutionStatus        next_status   = nextGraphNode->GetStatus();
        if (next_status != TFunction_ES_NotExecuted && next_status != TFunction_ES_Executing)
        {
          continue;
        }

        // Check all previous functions: all of them should be "succeeded"
        bool                           is_prev_succeeded = true;
        const NCollection_Map<int>&    prevOfNext        = nextGraphNode->GetPrevious();
        NCollection_Map<int>::Iterator itrp(prevOfNext);
        for (; itrp.More(); itrp.Next())
        {
          const int        IDprevOfNext = itrp.Key();
          const TDF_Label& LprevOfNext  = myScope->GetFunctions().Find1(IDprevOfNext);
          occ::handle<TFunction_GraphNode> GprevOfNext;
          LprevOfNext.FindAttribute(TFunction_GraphNode::GetID(), GprevOfNext);
          TFunction_ExecutionStatus prev_status = GprevOfNext->GetStatus();
          if (prev_status != TFunction_ES_Succeeded)
          {
            is_prev_succeeded = false;
            break;
          }
        }
        if (!is_prev_succeeded)
        {
          continue;
        }
      }

      // Ignore already passed functions (for the mode of ignoring the execution status).
      if (!myUsageOfExecutionStatus && myPassedFunctions.Contains(Lnext))
        continue;

      next_current.Add(Lnext);

      // Register already passed functions
      if (!myUsageOfExecutionStatus)
        myPassedFunctions.Add(Lnext);
    }
  }

  myCurrent.Clear();
  NCollection_Map<TDF_Label>::Iterator itrm(next_current);
  for (; itrm.More(); itrm.Next())
  {
    myCurrent.Append(itrm.Key());
  }
}

//=======================================================================
// function : GetStatus
// purpose  : Returns the execution status of the function
//=======================================================================

TFunction_ExecutionStatus TFunction_Iterator::GetStatus(const TDF_Label& func) const
{
  TFunction_IFunction iFunction(func);
  return iFunction.GetGraphNode()->GetStatus();
}

//=======================================================================
// function : SetStatus
// purpose  : Defines an execution status for a function
//=======================================================================

void TFunction_Iterator::SetStatus(const TDF_Label&                func,
                                   const TFunction_ExecutionStatus status) const
{
  TFunction_IFunction iFunction(func);
  iFunction.GetGraphNode()->SetStatus(status);
}

//=================================================================================================

Standard_OStream& TFunction_Iterator::Dump(Standard_OStream& anOS) const
{
  anOS << "Functions:" << std::endl;

  if (myCurrent.IsEmpty())
    return anOS;

  // Memorize the status of each function
  // in order to recover it after iteration.
  NCollection_DataMap<TDF_Label, int>             saved_status;
  occ::handle<TFunction_Scope>                    scope = TFunction_Scope::Set(myCurrent.First());
  NCollection_DoubleMap<int, TDF_Label>::Iterator itrd(scope->GetFunctions());
  for (; itrd.More(); itrd.Next())
  {
    const TDF_Label&                 L = itrd.Key2();
    occ::handle<TFunction_GraphNode> G;
    if (L.FindAttribute(TFunction_GraphNode::GetID(), G))
    {
      saved_status.Bind(L, (int)G->GetStatus());
      G->SetStatus(TFunction_ES_NotExecuted);
    }
  }

  TFunction_Iterator fIterator(myCurrent.First());
  fIterator.myUsageOfExecutionStatus = true;

  while (fIterator.More())
  {
    const NCollection_List<TDF_Label>& current = fIterator.Current();

    NCollection_List<TDF_Label>::Iterator itrl(current);
    for (; itrl.More(); itrl.Next())
    {

      const TDF_Label& L = itrl.Value();

      occ::handle<TDataStd_Name> N;
      if (L.FindAttribute(TDataStd_Name::GetID(), N))
      {
        anOS << TCollection_AsciiString(N->Get()).ToCString();
      }

      occ::handle<TFunction_GraphNode> G;
      if (L.FindAttribute(TFunction_GraphNode::GetID(), G))
      {
        G->SetStatus(TFunction_ES_Succeeded);
      }

      anOS << "\t";
    }

    fIterator.Next();

    anOS << std::endl;
  }

  // Recover the status of functions
  NCollection_DataMap<TDF_Label, int>::Iterator itrm(saved_status);
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label&          L      = itrm.Key();
    TFunction_ExecutionStatus status = (TFunction_ExecutionStatus)itrm.Value();

    occ::handle<TFunction_GraphNode> G;
    if (L.FindAttribute(TFunction_GraphNode::GetID(), G))
    {
      G->SetStatus(status);
    }
  }

  return anOS;
}
