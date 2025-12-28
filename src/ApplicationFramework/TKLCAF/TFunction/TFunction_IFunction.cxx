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

#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_Scope.hxx>

//=======================================================================
// function : NewFunction
// purpose  : Static method to create a new function.
//=======================================================================
bool TFunction_IFunction::NewFunction(const TDF_Label& L, const Standard_GUID& ID)
{
  // Set Function (ID, code of failure)
  TFunction_Function::Set(L, ID)->SetFailure(0);

  // Set graph node (dependencies, status)
  occ::handle<TFunction_GraphNode> graphNode = TFunction_GraphNode::Set(L);
  graphNode->RemoveAllPrevious();
  graphNode->RemoveAllNext();
  graphNode->SetStatus(TFunction_ES_WrongDefinition);

  // Check presence of the function in the current scope
  TFunction_Scope::Set(L)->AddFunction(L);

  return TFunction_DriverTable::Get()->HasDriver(ID);
}

//=======================================================================
// function : DeleteFunction
// purpose  : Static method to delete a function.
//=======================================================================

bool TFunction_IFunction::DeleteFunction(const TDF_Label& L)
{
  // Delete Function
  occ::handle<TFunction_Function> func;
  if (L.FindAttribute(TFunction_Function::GetID(), func))
    L.ForgetAttribute(func);

  // Take the scope of functions
  occ::handle<TFunction_Scope> scope  = TFunction_Scope::Set(L);
  const int  funcID = scope->GetFunctions().Find2(L);

  // Delete graph node
  occ::handle<TFunction_GraphNode> graphNode;
  if (L.FindAttribute(TFunction_GraphNode::GetID(), graphNode))
  {
    const NCollection_Map<int>& prev = graphNode->GetPrevious();
    const NCollection_Map<int>& next = graphNode->GetNext();
    // Disconnect previous functions
    NCollection_Map<int>::Iterator itrm(prev);
    for (; itrm.More(); itrm.Next())
    {
      const int      ID = itrm.Key();
      const TDF_Label&            La = scope->GetFunctions().Find1(ID);
      occ::handle<TFunction_GraphNode> G;
      if (La.FindAttribute(TFunction_GraphNode::GetID(), G))
      {
        G->RemoveNext(funcID);
      }
    }
    // Disconnect next functions
    for (itrm.Initialize(next); itrm.More(); itrm.Next())
    {
      const int      ID = itrm.Key();
      const TDF_Label&            La = scope->GetFunctions().Find1(ID);
      occ::handle<TFunction_GraphNode> G;
      if (La.FindAttribute(TFunction_GraphNode::GetID(), G))
      {
        G->RemovePrevious(funcID);
      }
    }

    L.ForgetAttribute(graphNode);
  }

  // Delete the function from the current scope of functions.
  scope->RemoveFunction(L);

  return true;
}

//=======================================================================
// function : UpdateDependencies
// purpose  : Updates the dependencies of all functions.
//=======================================================================

bool TFunction_IFunction::UpdateDependencies(const TDF_Label& Access)
{
  // Take the scope of functions.
  occ::handle<TFunction_Scope> scope = TFunction_Scope::Set(Access);

  // Make a data map of function - results.
  NCollection_DataMap<TDF_Label, NCollection_List<TDF_Label>>                  table;
  NCollection_DoubleMap<int, TDF_Label>::Iterator itrm(scope->GetFunctions());
  for (; itrm.More(); itrm.Next())
  {
    // Label of the function
    const TDF_Label&    L = itrm.Key2();
    TFunction_IFunction iFunction(L);

    // Take the driver.
    occ::handle<TFunction_Driver> driver = iFunction.GetDriver();

    // Take the results.
    NCollection_List<TDF_Label> res;
    driver->Results(res);

    // Fill-in the table
    table.Bind(L, res);

    // Clean the graph node
    occ::handle<TFunction_GraphNode> graphNode = iFunction.GetGraphNode();
    graphNode->RemoveAllPrevious();
    graphNode->RemoveAllNext();
  }

  // Update previous and next functions for each function of the scope
  NCollection_DataMap<TDF_Label, NCollection_List<TDF_Label>>::Iterator itrd;
  for (itrm.Initialize(scope->GetFunctions()); itrm.More(); itrm.Next())
  {
    // Label of the functions
    const TDF_Label&    L = itrm.Key2();
    TFunction_IFunction iFunction(L);

    // Take the driver.
    occ::handle<TFunction_Driver> driver = iFunction.GetDriver();

    // Take the arguments.
    NCollection_List<TDF_Label> args;
    driver->Arguments(args);

    // Make a map of arguments
    NCollection_Map<TDF_Label>                argsMap;
    NCollection_List<TDF_Label>::Iterator itrl(args);
    for (; itrl.More(); itrl.Next())
      argsMap.Add(itrl.Value());

    // ID of the function
    const int funcID = itrm.Key1();

    // Find the functions, which produce the arguments of this function.
    for (itrd.Initialize(table); itrd.More(); itrd.Next())
    {
      const TDF_Label& anotherL = itrd.Key();
      if (L == anotherL)
        continue;
      const NCollection_List<TDF_Label>& anotherRes = itrd.Value();

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

  return true;
}

//=================================================================================================

TFunction_IFunction::TFunction_IFunction() {}

//=================================================================================================

TFunction_IFunction::TFunction_IFunction(const TDF_Label& L)
{
  Init(L);
}

//=======================================================================
// function : Init
// purpose  : Initializes the interface.
//=======================================================================

void TFunction_IFunction::Init(const TDF_Label& L)
{
  myLabel = L;
}

//=======================================================================
// function : Label
// purpose  : Returns the label of the interface.
//=======================================================================

const TDF_Label& TFunction_IFunction::Label() const
{
  return myLabel;
}

//=======================================================================
// function : UpdateDependencies
// purpose  : Updates the dependencies of this function only.
//=======================================================================

bool TFunction_IFunction::UpdateDependencies() const
{
  // Take the arguments & results of the functions
  NCollection_List<TDF_Label>            args, res;
  occ::handle<TFunction_Driver> D = GetDriver();
  D->Arguments(args);
  D->Results(res);

  // Insert the arguments and results into maps for fast searching.
  NCollection_Map<TDF_Label>                argsMap, resMap;
  NCollection_List<TDF_Label>::Iterator itrl(args);
  for (; itrl.More(); itrl.Next())
  {
    argsMap.Add(itrl.Value());
  }
  for (itrl.Initialize(res); itrl.More(); itrl.Next())
  {
    resMap.Add(itrl.Value());
  }

  // Consider all other functions checking their attitude to this function.
  occ::handle<TFunction_Scope>                              scope = TFunction_Scope::Set(myLabel);
  NCollection_DoubleMap<int, TDF_Label>::Iterator itrm(scope->GetFunctions());
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

  return true;
}

//=======================================================================
// function : Arguments
// purpose  : The method fills-in the list by labels,
//           where the arguments of the function are located.
//=======================================================================

void TFunction_IFunction::Arguments(NCollection_List<TDF_Label>& args) const
{
  occ::handle<TFunction_Driver> driver = GetDriver();
  driver->Arguments(args);
}

//=======================================================================
// function : Results
// purpose  : The method fills-in the list by labels,
//           where the results of the function are located.
//=======================================================================

void TFunction_IFunction::Results(NCollection_List<TDF_Label>& res) const
{
  occ::handle<TFunction_Driver> driver = GetDriver();
  driver->Results(res);
}

//=======================================================================
// function : GetPrevious
// purpose  : Returns a list of previous functions.
//=======================================================================

void TFunction_IFunction::GetPrevious(NCollection_List<TDF_Label>& prev) const
{
  occ::handle<TFunction_GraphNode> graph = GetGraphNode();
  const NCollection_Map<int>& map   = graph->GetPrevious();
  occ::handle<TFunction_Scope>     scope = TFunction_Scope::Set(myLabel);

  NCollection_Map<int>::Iterator itrm(map);
  for (; itrm.More(); itrm.Next())
  {
    const int funcID = itrm.Key();
    if (scope->GetFunctions().IsBound1(funcID))
    {
      prev.Append(scope->GetFunctions().Find1(funcID));
    }
  }
}

//=======================================================================
// function : GetNext
// purpose  : Returns a list of next functions.
//=======================================================================

void TFunction_IFunction::GetNext(NCollection_List<TDF_Label>& next) const
{
  occ::handle<TFunction_GraphNode> graph = GetGraphNode();
  const NCollection_Map<int>& map   = graph->GetNext();
  occ::handle<TFunction_Scope>     scope = TFunction_Scope::Set(myLabel);

  NCollection_Map<int>::Iterator itrm(map);
  for (; itrm.More(); itrm.Next())
  {
    const int funcID = itrm.Key();
    if (scope->GetFunctions().IsBound1(funcID))
    {
      next.Append(scope->GetFunctions().Find1(funcID));
    }
  }
}

//=======================================================================
// function : GetStatus
// purpose  : Returns the execution status of the function.
//=======================================================================

TFunction_ExecutionStatus TFunction_IFunction::GetStatus() const
{
  occ::handle<TFunction_GraphNode> graph = GetGraphNode();
  return graph->GetStatus();
}

//=======================================================================
// function : SetStatus
// purpose  : Defines an execution status for a function.
//=======================================================================

void TFunction_IFunction::SetStatus(const TFunction_ExecutionStatus status) const
{
  occ::handle<TFunction_GraphNode> graph = GetGraphNode();
  graph->SetStatus(status);
}

//=======================================================================
// function : GetFunctions
// purpose  : Returns the scope of functions.
//=======================================================================

const NCollection_DoubleMap<int, TDF_Label>& TFunction_IFunction::GetAllFunctions() const
{
  return TFunction_Scope::Set(myLabel)->GetFunctions();
}

//=================================================================================================

occ::handle<TFunction_Logbook> TFunction_IFunction::GetLogbook() const
{
  return TFunction_Scope::Set(myLabel)->GetLogbook();
}

//=======================================================================
// function : GetDriver
// purpose  : Returns the function driver.
//=======================================================================

occ::handle<TFunction_Driver> TFunction_IFunction::GetDriver(const int thread) const
{
  occ::handle<TFunction_Driver>   driver;
  occ::handle<TFunction_Function> func;
  if (!myLabel.FindAttribute(TFunction_Function::GetID(), func))
    throw Standard_NoSuchObject(
      "TFunction_IFunction::GetDriver(): A Function is not found attached to this label");
  if (!TFunction_DriverTable::Get()->FindDriver(func->GetDriverGUID(), driver, thread))
    throw Standard_NoSuchObject(
      "TFunction_IFunction::GetDriver(): A driver is not found for this ID");
  driver->Init(myLabel);
  return driver;
}

//=======================================================================
// function : GetGraphNode
// purpose  : Returns a graph node of the function.
//=======================================================================

occ::handle<TFunction_GraphNode> TFunction_IFunction::GetGraphNode() const
{
  occ::handle<TFunction_GraphNode> graphNode;
  if (!myLabel.FindAttribute(TFunction_GraphNode::GetID(), graphNode))
    throw Standard_NoSuchObject(
      "TFunction_IFunction::GetStatus(): A graph node is not found attached to this label");
  return graphNode;
}
