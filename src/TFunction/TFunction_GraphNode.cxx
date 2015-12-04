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
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_Scope.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TFunction_GraphNode,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : Static method to get an ID
//=======================================================================
const Standard_GUID&  TFunction_GraphNode::GetID() 
{  
  static Standard_GUID TFunction_GraphNodeID("DD51FA86-E171-41a4-A2C1-3A0FBF286798");
  return TFunction_GraphNodeID; 
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a graph node attribute
//=======================================================================

Handle(TFunction_GraphNode) TFunction_GraphNode::Set(const TDF_Label& L)
{
  Handle(TFunction_GraphNode) G;
  if (!L.FindAttribute(TFunction_GraphNode::GetID(), G)) 
  {
    G = new TFunction_GraphNode();
    L.AddAttribute(G);
  }
  return G;
}

//=======================================================================
//function : ID
//purpose  : Returns GUID of the function
//=======================================================================

const Standard_GUID& TFunction_GraphNode::ID() const
{ 
  return GetID(); 
}

//=======================================================================
//function : TFunction_GraphNode
//purpose  : Constructor
//=======================================================================

TFunction_GraphNode::TFunction_GraphNode():myStatus(TFunction_ES_WrongDefinition)
{

}

//=======================================================================
//function : AddPrevious
//purpose  : Adds a function to the previous functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::AddPrevious(const Standard_Integer funcID)
{
  if (myPrevious.Contains(funcID))
    return Standard_False;

  Backup();

  return myPrevious.Add(funcID);
}

//=======================================================================
//function : AddPrevious
//purpose  : Adds a function to the previous functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::AddPrevious(const TDF_Label& func)
{
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(func);
  if (!scope->GetFunctions().IsBound2(func))
    return Standard_False;
  Standard_Integer funcID = scope->GetFunctions().Find2(func);
  return AddPrevious(funcID);
}

//=======================================================================
//function : RemovePrevious
//purpose  : Removes a function to the previous functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::RemovePrevious(const Standard_Integer funcID)
{
  if (!myPrevious.Contains(funcID))
    return Standard_False;

  Backup();

  return myPrevious.Remove(funcID);
}

//=======================================================================
//function : RemovePrevious
//purpose  : Removes a function to the previous functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::RemovePrevious(const TDF_Label& func)
{
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(func);
  if (!scope->GetFunctions().IsBound2(func))
    return Standard_False;
  Standard_Integer funcID = scope->GetFunctions().Find2(func);
  return RemovePrevious(funcID);
}

//=======================================================================
//function : GetPrevious
//purpose  : Returns a map of previous functions.
//=======================================================================

const TColStd_MapOfInteger& TFunction_GraphNode::GetPrevious() const
{
  return myPrevious;
}

//=======================================================================
//function : RemoveAllPrevious
//purpose  : Clear the map of previous functions.
//=======================================================================

void TFunction_GraphNode::RemoveAllPrevious()
{
  if (myPrevious.IsEmpty())
    return;

  Backup();

  myPrevious.Clear();
}

//=======================================================================
//function : AddNext
//purpose  : Adds a function to the next functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::AddNext(const Standard_Integer funcID)
{
  if (myNext.Contains(funcID))
    return Standard_False;

  Backup();

  return myNext.Add(funcID);
}

//=======================================================================
//function : AddNext
//purpose  : Adds a function to the next functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::AddNext(const TDF_Label& func)
{
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(func);
  if (!scope->GetFunctions().IsBound2(func))
    return Standard_False;
  Standard_Integer funcID = scope->GetFunctions().Find2(func);
  return AddNext(funcID);
}

//=======================================================================
//function : RemoveNext
//purpose  : Removes a function to the next functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::RemoveNext(const Standard_Integer funcID)
{
  if (!myNext.Contains(funcID))
    return Standard_False;

  Backup();

  return myNext.Remove(funcID);
}

//=======================================================================
//function : RemoveNext
//purpose  : Remove a function to the next functions of this function.
//=======================================================================

Standard_Boolean TFunction_GraphNode::RemoveNext(const TDF_Label& func)
{
  Handle(TFunction_Scope) scope = TFunction_Scope::Set(func);
  if (!scope->GetFunctions().IsBound2(func))
    return Standard_False;
  Standard_Integer funcID = scope->GetFunctions().Find2(func);
  return RemoveNext(funcID);
}

//=======================================================================
//function : GetNext
//purpose  : Returns a map of next functions.
//=======================================================================

const TColStd_MapOfInteger& TFunction_GraphNode::GetNext() const
{
  return myNext;
}

//=======================================================================
//function : RemoveAllNext
//purpose  : Clear the map of next functions.
//=======================================================================

void TFunction_GraphNode::RemoveAllNext()
{
  if (myNext.IsEmpty())
    return;

  Backup();

  myNext.Clear();
}

//=======================================================================
//function : GetStatus
//purpose  : Returns the execution status of the function.
//=======================================================================

TFunction_ExecutionStatus TFunction_GraphNode::GetStatus() const
{
  return myStatus;
}

//=======================================================================
//function : SetStatus
//purpose  : Defines an execution status for a function.
//=======================================================================

void TFunction_GraphNode::SetStatus(const TFunction_ExecutionStatus status)
{
  if (myStatus == status)
    return;

  Backup();

  myStatus = status;
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TFunction_GraphNode::Restore(const Handle(TDF_Attribute)& other) 
{
  Handle(TFunction_GraphNode) G = Handle(TFunction_GraphNode)::DownCast(other);

  // Previous
  myPrevious = G->myPrevious;

  // Next
  myNext = G->myNext;
  
  // Status
  myStatus = G->myStatus;
}

//=======================================================================
//function : Paste
//purpose  : Method for Copy mechanism
//=======================================================================

void TFunction_GraphNode::Paste(const Handle(TDF_Attribute)& into,
				const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TFunction_GraphNode) G = Handle(TFunction_GraphNode)::DownCast(into);

  // Previous
  G->myPrevious = myPrevious;

  // Next
  G->myNext = myNext;

  // Status
  G->myStatus = myStatus;
}

//=======================================================================
//function : NewEmpty
//purpose  : Returns new empty graph node attribute
//=======================================================================

Handle(TDF_Attribute) TFunction_GraphNode::NewEmpty() const
{
  return new TFunction_GraphNode();
}

//=======================================================================
//function : References
//purpose  : Collects the references
//=======================================================================

void TFunction_GraphNode::References(const Handle(TDF_DataSet)& /*aDataSet*/) const
{

}

//=======================================================================
//function : Dump
//purpose  : Dump of the graph node
//=======================================================================

Standard_OStream& TFunction_GraphNode::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  return anOS;
}
