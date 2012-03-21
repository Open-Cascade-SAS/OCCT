// Created on: 2008-06-21
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



#include <TFunction_GraphNode.ixx>
#include <TFunction_Scope.hxx>

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
