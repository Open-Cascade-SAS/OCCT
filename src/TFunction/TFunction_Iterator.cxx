// Created on: 2008-06-23
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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



#include <TFunction_Iterator.ixx>
#include <TFunction_IFunction.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>

#include <TDF_LabelMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_LabelIntegerMap.hxx>
#include <TDF_DataMapIteratorOfLabelIntegerMap.hxx>

#include <TDataStd_Name.hxx>

#include <TColStd_MapIteratorOfMapOfInteger.hxx>

//=======================================================================
//function : Create
//purpose  : Constructor
//=======================================================================

TFunction_Iterator::TFunction_Iterator():myUsageOfExecutionStatus(Standard_False)
{  

}

//=======================================================================
//function : Create
//purpose  : Constructor
//=======================================================================

TFunction_Iterator::TFunction_Iterator(const TDF_Label& Access):myUsageOfExecutionStatus(Standard_False)
{
  Init(Access);
}

//=======================================================================
//function : Init
//purpose  : Initializes the Iterator.
//=======================================================================

void TFunction_Iterator::Init(const TDF_Label& Access)
{
  myCurrent.Clear();
  myPassedFunctions.Clear();

  // Get the scope of functions
  myScope = TFunction_Scope::Set(Access);

  // Find the roots
  TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrm(myScope->GetFunctions());
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label& L = itrm.Key2();

    TFunction_IFunction iFunction(L);
    Handle(TFunction_GraphNode) graphNode = iFunction.GetGraphNode();
    TFunction_ExecutionStatus status = graphNode->GetStatus();

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
//function : SetUsageOfExecutionStatus
//purpose  : Defines usage of execution status
//=======================================================================

void TFunction_Iterator::SetUsageOfExecutionStatus(const Standard_Boolean usage)
{
  myUsageOfExecutionStatus = usage;
}

//=======================================================================
//function : GetUsageOfExecutionStatus
//purpose  : Returns usage of execution status
//=======================================================================

Standard_Boolean TFunction_Iterator::GetUsageOfExecutionStatus() const
{
  return myUsageOfExecutionStatus;
}

//=======================================================================
//function : GetMaxNbThreads
//purpose  : Defines the maximum number of threads
//=======================================================================

Standard_Integer TFunction_Iterator::GetMaxNbThreads() const
{
  int nb_threads = 0;
  TFunction_Iterator fIterator;
  fIterator.myUsageOfExecutionStatus = Standard_False;

  // Start iteration from current functions
  TDF_ListIteratorOfLabelList itrl(myCurrent);
  for (; itrl.More(); itrl.Next())
  {
    fIterator.myCurrent.Append(itrl.Value());
  }

  // Check number of semultenious current functions
  while (!fIterator.Current().IsEmpty())
  {
    const TDF_LabelList& current = fIterator.Current();
    if (nb_threads < current.Extent())
      nb_threads = current.Extent();
    fIterator.Next();
  }

  return nb_threads;
}

//=======================================================================
//function : Current
//purpose  : Returns the current list of functions
//=======================================================================

const TDF_LabelList& TFunction_Iterator::Current() const
{
  return myCurrent;
}

//=======================================================================
//function : More
//purpose  : Returns true if the iteration is ended
//=======================================================================

Standard_Boolean TFunction_Iterator::More() const
{
  if (myUsageOfExecutionStatus)
  {
    TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrm(myScope->GetFunctions());
    for (; itrm.More(); itrm.Next())
    {
      const TDF_Label& L = itrm.Key2();
      if (GetStatus(L) == TFunction_ES_NotExecuted)
	return Standard_True;
    }
    return Standard_False;
  }
  return myPassedFunctions.Extent() < myScope->GetFunctions().Extent();
}

//=======================================================================
//function : Next
//purpose  : Switches the iterator to the next functions
//=======================================================================

void TFunction_Iterator::Next()
{
  TDF_LabelMap next_current;
  TDF_ListIteratorOfLabelList itrl(myCurrent);
  for (; itrl.More(); itrl.Next())
  {
    const TDF_Label& L = itrl.Value();
    TFunction_IFunction iFunction(L);

    Handle(TFunction_GraphNode) graphNode = iFunction.GetGraphNode();
    const TColStd_MapOfInteger& next      = graphNode->GetNext();
    TFunction_ExecutionStatus   status    = graphNode->GetStatus();

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
    TColStd_MapIteratorOfMapOfInteger itrm(next);
    for (; itrm.More(); itrm.Next())
    {
      const Standard_Integer IDnext = itrm.Key();
      const TDF_Label& Lnext = myScope->GetFunctions().Find1(IDnext);

      if (myUsageOfExecutionStatus)
      {
	// A previous function is "succeeded", check status of next functions and 
	// all other previous functions of the next functions.

	// Check status, it should be "not executed"
	TFunction_IFunction iNextFunction(Lnext);
	Handle(TFunction_GraphNode) nextGraphNode = iNextFunction.GetGraphNode();
	TFunction_ExecutionStatus next_status = nextGraphNode->GetStatus();
	if (next_status != TFunction_ES_NotExecuted && next_status != TFunction_ES_Executing)
	{
	  continue;
	}

	// Check all previous functions: all of them should be "succeeded"
	Standard_Boolean is_prev_succeeded = Standard_True;
	const TColStd_MapOfInteger& prevOfNext = nextGraphNode->GetPrevious();
	TColStd_MapIteratorOfMapOfInteger itrp(prevOfNext);
	for (; itrp.More(); itrp.Next())
	{
	  const Standard_Integer IDprevOfNext = itrp.Key();
	  const TDF_Label& LprevOfNext = myScope->GetFunctions().Find1(IDprevOfNext);
	  Handle(TFunction_GraphNode) GprevOfNext;
	  LprevOfNext.FindAttribute(TFunction_GraphNode::GetID(), GprevOfNext);
	  TFunction_ExecutionStatus prev_status = GprevOfNext->GetStatus();
	  if (prev_status != TFunction_ES_Succeeded)
	  {
	    is_prev_succeeded = Standard_False;
	    break;
	  }
	}
	if (!is_prev_succeeded)
	{
	  continue;
	}
      }

      // Ignore already passed fucntions (for the mode of ignoring the execution status).
      if (!myUsageOfExecutionStatus && myPassedFunctions.Contains(Lnext))
	continue;

      next_current.Add(Lnext);

      // Register already passed functions
      if (!myUsageOfExecutionStatus)
	myPassedFunctions.Add(Lnext);
    }
  }

  myCurrent.Clear();
  TDF_MapIteratorOfLabelMap itrm(next_current);
  for (; itrm.More(); itrm.Next())
  {
    myCurrent.Append(itrm.Key());
  }
}

//=======================================================================
//function : GetStatus
//purpose  : Returns the execution status of the function
//=======================================================================

TFunction_ExecutionStatus TFunction_Iterator::GetStatus(const TDF_Label& func) const
{
  TFunction_IFunction iFunction(func);
  return iFunction.GetGraphNode()->GetStatus();
}

//=======================================================================
//function : SetStatus
//purpose  : Defines an execution status for a function
//=======================================================================

void TFunction_Iterator::SetStatus(const TDF_Label& func,
				   const TFunction_ExecutionStatus status) const
{
  TFunction_IFunction iFunction(func);
  iFunction.GetGraphNode()->SetStatus(status);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TFunction_Iterator::Dump (Standard_OStream& anOS) const
{  
  anOS << "Functions:" << endl ;

  if (myCurrent.IsEmpty())
    return anOS;

  // Memorize the status of each function
  // in order to recover it after iteration.
  TDF_LabelIntegerMap saved_status;
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(myCurrent.First());
  TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrd(scope->GetFunctions());
  for (; itrd.More(); itrd.Next())
  {
    const TDF_Label& L = itrd.Key2();
    Handle(TFunction_GraphNode) G;
    if (L.FindAttribute(TFunction_GraphNode::GetID(), G))
    {
      saved_status.Bind(L, (Standard_Integer) G->GetStatus());
      G->SetStatus(TFunction_ES_NotExecuted);
    }
  }

  TFunction_Iterator fIterator(myCurrent.First());
  fIterator.myUsageOfExecutionStatus = Standard_True;

  while (fIterator.More())
  {
    const TDF_LabelList& current = fIterator.Current();

    TDF_ListIteratorOfLabelList itrl(current);
    for (; itrl.More(); itrl.Next())
    {

      const TDF_Label& L = itrl.Value();

      Handle(TDataStd_Name) N;
      if (L.FindAttribute(TDataStd_Name::GetID(), N))
      {
	anOS << TCollection_AsciiString(N->Get()).ToCString() ;
      }

      Handle(TFunction_GraphNode) G;
      if (L.FindAttribute(TFunction_GraphNode::GetID(), G))
      {
	G->SetStatus(TFunction_ES_Succeeded);
      }

      anOS << "\t" ;
    }

    fIterator.Next();
    
    anOS << endl;
  }

  // Recover the status of functions
  TDF_DataMapIteratorOfLabelIntegerMap itrm(saved_status);
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label& L = itrm.Key();
    TFunction_ExecutionStatus status = (TFunction_ExecutionStatus) itrm.Value();
    
    Handle(TFunction_GraphNode) G;
    if (L.FindAttribute(TFunction_GraphNode::GetID(), G))
    {
      G->SetStatus(status);
    }
  }

  return anOS;
}
