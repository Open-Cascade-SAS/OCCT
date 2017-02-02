// Created on: 2008-06-21
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


#include <Standard_GUID.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TFunction_DataMapIteratorOfDataMapOfLabelListOfLabel.hxx>
#include <TFunction_DataMapOfLabelListOfLabel.hxx>
#include <TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_Logbook.hxx>
#include <TFunction_Scope.hxx>

//=======================================================================
//function : NewFunction
//purpose  : Static method to create a new function.
//=======================================================================
Standard_Boolean TFunction_IFunction::NewFunction(const TDF_Label& L, const Standard_GUID& ID) 
{ 
  // Set Function (ID, code of failure)
  TFunction_Function::Set(L, ID)->SetFailure(0);

  // Set graph node (dependencies, status)
  Handle(TFunction_GraphNode) graphNode = TFunction_GraphNode::Set(L);
  graphNode->RemoveAllPrevious();
  graphNode->RemoveAllNext();
  graphNode->SetStatus(TFunction_ES_WrongDefinition);

  // Check presence of the function in the current scope
  TFunction_Scope::Set(L)->AddFunction(L);

  return TFunction_DriverTable::Get()->HasDriver(ID);
}

//=======================================================================
//function : DeleteFunction
//purpose  : Static method to delete a function.
//=======================================================================

Standard_Boolean TFunction_IFunction::DeleteFunction(const TDF_Label& L) 
{ 
  // Delete Function
  Handle(TFunction_Function) func;
  if (L.FindAttribute(TFunction_Function::GetID(), func))
    L.ForgetAttribute(func);

  // Take the scope of functions
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(L);
  const Standard_Integer funcID = scope->GetFunctions().Find2(L);

  // Delete graph node
  Handle(TFunction_GraphNode) graphNode;
  if (L.FindAttribute(TFunction_GraphNode::GetID(), graphNode))
  {
    const TColStd_MapOfInteger& prev = graphNode->GetPrevious();
    const TColStd_MapOfInteger& next = graphNode->GetNext();
    // Disconnect previous functions
    TColStd_MapIteratorOfMapOfInteger itrm(prev);
    for (; itrm.More(); itrm.Next())
    {
      const Standard_Integer ID = itrm.Key();
      const TDF_Label& La = scope->GetFunctions().Find1(ID);
      Handle(TFunction_GraphNode) G;
      if (La.FindAttribute(TFunction_GraphNode::GetID(), G))
      {
	G->RemoveNext(funcID);
      }
    }
    // Disconnect next functions
    for (itrm.Initialize(next); itrm.More(); itrm.Next())
    {
      const Standard_Integer ID = itrm.Key();
      const TDF_Label& La = scope->GetFunctions().Find1(ID);
      Handle(TFunction_GraphNode) G;
      if (La.FindAttribute(TFunction_GraphNode::GetID(), G))
      {
	G->RemovePrevious(funcID);
      }
    }

    L.ForgetAttribute(graphNode);
  }

  // Delete the function from the current scope of functions.
  scope->RemoveFunction(L);

  return Standard_True;
}

//=======================================================================
//function : UpdateDependencies
//purpose  : Updates the dependencies of all functions.
//=======================================================================

Standard_Boolean TFunction_IFunction::UpdateDependencies(const TDF_Label& Access)
{
  // Take the scope of functions.
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(Access);

  // Make a data map of function - results.
  TFunction_DataMapOfLabelListOfLabel table;
  TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrm(scope->GetFunctions());
  for (; itrm.More(); itrm.Next())
  {
    // Label of the function
    const TDF_Label& L = itrm.Key2();
    TFunction_IFunction iFunction(L);

    // Take the driver.
    Handle(TFunction_Driver) driver = iFunction.GetDriver();

    // Take the results.
    TDF_LabelList res;
    driver->Results(res);

    // Fill-in the table
    table.Bind(L, res);
    
    // Clean the graph node
    Handle(TFunction_GraphNode) graphNode = iFunction.GetGraphNode();
    graphNode->RemoveAllPrevious();
    graphNode->RemoveAllNext();
  }

  // Update previous and next functions for each function of the scope
  TFunction_DataMapIteratorOfDataMapOfLabelListOfLabel itrd;
  for (itrm.Initialize(scope->GetFunctions()); itrm.More(); itrm.Next())
  {
    // Label of the functions
    const TDF_Label& L = itrm.Key2();
    TFunction_IFunction iFunction(L);

    // Take the driver.
    Handle(TFunction_Driver) driver = iFunction.GetDriver();

    // Take the arguments.
    TDF_LabelList args;
    driver->Arguments(args);
    
    // Make a map of arguments
    TDF_LabelMap argsMap;
    TDF_ListIteratorOfLabelList itrl(args);
    for (; itrl.More(); itrl.Next())
      argsMap.Add(itrl.Value());

    // ID of the function
    const Standard_Integer funcID = itrm.Key1();

    // Find the functions, which produce the arguments of this function.
    for (itrd.Initialize(table); itrd.More(); itrd.Next())
    {
      const TDF_Label& anotherL = itrd.Key();
      if (L == anotherL)
	continue;
      const TDF_LabelList& anotherRes = itrd.Value();

      for (itrl.Initialize(anotherRes); itrl.More(); itrl.Next())
      {
	if (argsMap.Contains(itrl.Value()))
	{
	  iFunction.GetGraphNode()->AddPrevious(anotherL);

	  TFunction_IFunction iAnotherFunction(anotherL);
	  iAnotherFunction.GetGraphNode()->AddNext(funcID);
	}
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Create
//purpose  : Constructor
//=======================================================================

TFunction_IFunction::TFunction_IFunction() 
{  

}

//=======================================================================
//function : Create
//purpose  : Constructor
//=======================================================================

TFunction_IFunction::TFunction_IFunction(const TDF_Label& L)
{
  Init(L);
}

//=======================================================================
//function : Init
//purpose  : Initializes the interface.
//=======================================================================

void TFunction_IFunction::Init(const TDF_Label& L)
{
  myLabel = L;
}

//=======================================================================
//function : Label
//purpose  : Returns the label of the interface.
//=======================================================================

const TDF_Label& TFunction_IFunction::Label() const
{
  return myLabel;
}

//=======================================================================
//function : UpdateDependencies
//purpose  : Updates the dependencies of this function only.
//=======================================================================

Standard_Boolean TFunction_IFunction::UpdateDependencies() const
{
  // Take the arguments & results of the functions
  TDF_LabelList args, res;
  Handle(TFunction_Driver) D = GetDriver();
  D->Arguments(args);
  D->Results(res);

  // Insert the arguments and results into maps for fast searching.
  TDF_LabelMap argsMap, resMap;
  TDF_ListIteratorOfLabelList itrl(args);
  for (; itrl.More(); itrl.Next())
  {
    argsMap.Add(itrl.Value());
  }
  for (itrl.Initialize(res); itrl.More(); itrl.Next())
  {
    resMap.Add(itrl.Value());
  }

  // Consider all other functions checking their attitude to this function.
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(myLabel);
  TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel itrm(scope->GetFunctions());
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label& L = itrm.Key2();
    if (L == myLabel)
      continue;
    TFunction_IFunction iFunc(L);
    D = iFunc.GetDriver();

    // Arguments of another function
    args.Clear();
    D->Arguments(args);

    // Check presence of the arguments in results of our function
    for (itrl.Initialize(args); itrl.More(); itrl.Next())
    {
      if (resMap.Contains(itrl.Value()))
      {
	// Our function is a previous one for this function.
	GetGraphNode()->AddNext(scope->GetFunctions().Find2(L));
	iFunc.GetGraphNode()->AddPrevious(scope->GetFunctions().Find2(myLabel));
      }
    }

    // Results of another function
    res.Clear();
    D->Results(res);

    // Check presence of the results in arguments of our function
    for (itrl.Initialize(res); itrl.More(); itrl.Next())
    {
      if (argsMap.Contains(itrl.Value()))
      {
	// Our function is a next one for this function.
	GetGraphNode()->AddPrevious(scope->GetFunctions().Find2(L));
	iFunc.GetGraphNode()->AddNext(scope->GetFunctions().Find2(myLabel));
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Arguments
//purpose  : The method fills-in the list by labels, 
//           where the arguments of the function are located.
//=======================================================================

void TFunction_IFunction::Arguments(TDF_LabelList& args) const
{
  Handle(TFunction_Driver) driver = GetDriver();
  driver->Arguments(args);
}

//=======================================================================
//function : Results
//purpose  : The method fills-in the list by labels,
//           where the results of the function are located.
//=======================================================================

void TFunction_IFunction::Results(TDF_LabelList& res) const
{
  Handle(TFunction_Driver) driver = GetDriver();
  driver->Results(res);
}

//=======================================================================
//function : GetPrevious
//purpose  : Returns a list of previous functions.
//=======================================================================

void TFunction_IFunction::GetPrevious(TDF_LabelList& prev) const
{
  Handle(TFunction_GraphNode) graph = GetGraphNode();
  const TColStd_MapOfInteger& map = graph->GetPrevious();
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(myLabel);

  TColStd_MapIteratorOfMapOfInteger itrm(map);
  for (; itrm.More(); itrm.Next())
  {
    const Standard_Integer funcID = itrm.Key();
    if (scope->GetFunctions().IsBound1(funcID))
    {
      prev.Append(scope->GetFunctions().Find1(funcID));
    }
  }
}

//=======================================================================
//function : GetNext
//purpose  : Returns a list of next functions.
//=======================================================================

void TFunction_IFunction::GetNext(TDF_LabelList& next) const
{
  Handle(TFunction_GraphNode) graph = GetGraphNode();
  const TColStd_MapOfInteger& map = graph->GetNext();
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(myLabel);

  TColStd_MapIteratorOfMapOfInteger itrm(map);
  for (; itrm.More(); itrm.Next())
  {
    const Standard_Integer funcID = itrm.Key();
    if (scope->GetFunctions().IsBound1(funcID))
    {
      next.Append(scope->GetFunctions().Find1(funcID));
    }
  }
}

//=======================================================================
//function : GetStatus
//purpose  : Returns the execution status of the function.
//=======================================================================

TFunction_ExecutionStatus TFunction_IFunction::GetStatus() const
{
  Handle(TFunction_GraphNode) graph = GetGraphNode();
  return graph->GetStatus();
}

//=======================================================================
//function : SetStatus
//purpose  : Defines an execution status for a function.
//=======================================================================

void TFunction_IFunction::SetStatus(const TFunction_ExecutionStatus status) const
{
  Handle(TFunction_GraphNode) graph = GetGraphNode();
  graph->SetStatus(status);
}

//=======================================================================
//function : GetFunctions
//purpose  : Returns the scope of functions.
//=======================================================================

const TFunction_DoubleMapOfIntegerLabel& TFunction_IFunction::GetAllFunctions() const
{
  return TFunction_Scope::Set(myLabel)->GetFunctions();
}

//=======================================================================
//function : GetLogbook
//purpose  : Returns the Logbook.
//=======================================================================

Handle(TFunction_Logbook) TFunction_IFunction::GetLogbook() const
{
  return TFunction_Scope::Set(myLabel)->GetLogbook();
}

//=======================================================================
//function : GetDriver
//purpose  : Returns the function driver.
//=======================================================================

Handle(TFunction_Driver) TFunction_IFunction::GetDriver(const Standard_Integer thread) const
{
  Handle(TFunction_Driver) driver;
  Handle(TFunction_Function) func;
  if (!myLabel.FindAttribute(TFunction_Function::GetID(), func))
    throw Standard_NoSuchObject("TFunction_IFunction::GetDriver(): A Function is not found attached to this label");
  if (!TFunction_DriverTable::Get()->FindDriver(func->GetDriverGUID(), driver, thread))
    throw Standard_NoSuchObject("TFunction_IFunction::GetDriver(): A driver is not found for this ID");
  driver->Init(myLabel);
  return driver;
}

//=======================================================================
//function : GetGraphNode
//purpose  : Returns a graph node of the function.
//=======================================================================

Handle(TFunction_GraphNode) TFunction_IFunction::GetGraphNode() const
{
  Handle(TFunction_GraphNode) graphNode;
  if (!myLabel.FindAttribute(TFunction_GraphNode::GetID(), graphNode))
    throw Standard_NoSuchObject("TFunction_IFunction::GetStatus(): A graph node is not found attached to this label");
  return graphNode;
}
