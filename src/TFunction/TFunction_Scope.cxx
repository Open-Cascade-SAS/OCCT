// Created on: 2008-06-22
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
#include <TDF_Label.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_RelocationTable.hxx>
#include <TFunction_Scope.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TFunction_Scope,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : Static method to get an ID
//=======================================================================
const Standard_GUID&  TFunction_Scope::GetID() 
{  
  static Standard_GUID TFunction_ScopeID("F2DE4EFF-7FE8-40a3-AAD5-5B6DDEA83469");
  return TFunction_ScopeID; 
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a Scope attribute
//=======================================================================

Handle(TFunction_Scope) TFunction_Scope::Set(const TDF_Label& Access)
{
  Handle(TFunction_Scope) S;
  if (!Access.Root().FindAttribute(TFunction_Scope::GetID(), S)) 
  {
    S = new TFunction_Scope();
    Access.Root().AddAttribute(S);
  }
  return S;
}

//=======================================================================
//function : ID
//purpose  : Returns GUID of the function
//=======================================================================

const Standard_GUID& TFunction_Scope::ID() const
{ 
  return GetID(); 
}

//=======================================================================
//function : TFunction_Scope
//purpose  : Constructor
//=======================================================================

TFunction_Scope::TFunction_Scope():myFreeID(1)
{

}

//=======================================================================
//function : AddFunction
//purpose  : Adds a function to the scope.
//=======================================================================

Standard_Boolean TFunction_Scope::AddFunction(const TDF_Label& L)
{
  if (myFunctions.IsBound2(L))
    return Standard_False;

  Backup();

  myFunctions.Bind(myFreeID++, L);
  return Standard_True;
}

//=======================================================================
//function : RemoveFunction
//purpose  : Removes a function from the scope.
//=======================================================================

Standard_Boolean TFunction_Scope::RemoveFunction(const TDF_Label& L)
{
  if (!myFunctions.IsBound2(L))
    return Standard_False;

  Backup();

  return myFunctions.UnBind2(L);
}

//=======================================================================
//function : RemoveFunction
//purpose  : Removes a function from the scope.
//=======================================================================

Standard_Boolean TFunction_Scope::RemoveFunction(const Standard_Integer ID)
{
  if (!myFunctions.IsBound1(ID))
    return Standard_False;

  Backup();

  return myFunctions.UnBind1(ID);
}

//=======================================================================
//function : RemoveAllFunctions
//purpose  : Removes a function from the scope.
//=======================================================================

void TFunction_Scope::RemoveAllFunctions()
{
  if (myFunctions.IsEmpty())
    return;

  Backup();

  myFunctions.Clear();
}

//=======================================================================
//function : HasFunction
//purpose  : Checks presence of a function.
//=======================================================================

Standard_Boolean TFunction_Scope::HasFunction(const Standard_Integer ID) const
{
  return myFunctions.IsBound1(ID);
}

//=======================================================================
//function : HasFunction
//purpose  : Checks presence of a function.
//=======================================================================

Standard_Boolean TFunction_Scope::HasFunction(const TDF_Label& L) const
{
  return myFunctions.IsBound2(L);
}

//=======================================================================
//function : GetFunction
//purpose  : Returns a function.
//=======================================================================

Standard_Integer TFunction_Scope::GetFunction(const TDF_Label& L) const
{
  return myFunctions.Find2(L);
}

//=======================================================================
//function : GetFunction
//purpose  : Returns a function.
//=======================================================================

const TDF_Label& TFunction_Scope::GetFunction(const Standard_Integer ID) const
{
  return myFunctions.Find1(ID);
}

//=======================================================================
//function : GetLogbook
//purpose  : Returns the Logbook.
//=======================================================================

Handle(TFunction_Logbook) TFunction_Scope::GetLogbook() const 
{
  Handle(TFunction_Logbook) logbook;
  FindAttribute(TFunction_Logbook::GetID(), logbook);
  return logbook;
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TFunction_Scope::Restore(const Handle(TDF_Attribute)& other) 
{
  Handle(TFunction_Scope) S = Handle(TFunction_Scope)::DownCast(other);

  // Functions
  myFunctions = S->myFunctions; // copying...
  myFreeID = S->myFreeID;
}

//=======================================================================
//function : Paste
//purpose  : Method for Copy mechanism
//=======================================================================

void TFunction_Scope::Paste(const Handle(TDF_Attribute)& /*into*/,
			    const Handle(TDF_RelocationTable)& /*RT*/) const
{
  // Do we need to copy a Scope attribute somewhere?
}

//=======================================================================
//function : NewEmpty
//purpose  : Returns new empty graph node attribute
//=======================================================================

Handle(TDF_Attribute) TFunction_Scope::NewEmpty() const
{
  return new TFunction_Scope();
}

//=======================================================================
//function : Dump
//purpose  : Dump of the scope of functions
//=======================================================================

Standard_OStream& TFunction_Scope::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  return anOS;
}

//=======================================================================
//function : GetFunctions
//purpose  : Returns the scope of functions.
//=======================================================================

const TFunction_DoubleMapOfIntegerLabel& TFunction_Scope::GetFunctions() const
{
  return myFunctions;
}

//=======================================================================
//function : ChangeFunctions
//purpose  : Returns the scope of functions.
//=======================================================================

TFunction_DoubleMapOfIntegerLabel& TFunction_Scope::ChangeFunctions()
{
  return myFunctions;
}

//=======================================================================
//function : SetFreeID
//purpose  : Defines a free function ID
//=======================================================================

void TFunction_Scope::SetFreeID (const Standard_Integer ID)
{
  if (myFreeID == ID)
    return;

  Backup();

  myFreeID = ID;
}

//=======================================================================
//function : GetFreeID
//purpose  : Returns a free function ID
//=======================================================================

Standard_Integer TFunction_Scope::GetFreeID () const
{
  return myFreeID;
}
