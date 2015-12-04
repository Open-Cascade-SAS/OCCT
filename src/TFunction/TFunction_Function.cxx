// Created on: 1999-06-10
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_Scope.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TFunction_Function,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : Static method to get an ID
//=======================================================================
const Standard_GUID&  TFunction_Function::GetID() 
{  
  static Standard_GUID TFunction_FunctionID("5b35ca00-5b78-11d1-8940-080009dc3333");
  return TFunction_FunctionID; 
}


//=======================================================================
//function : Set
//purpose  : Finds or creates a function attribute
//=======================================================================

Handle(TFunction_Function) TFunction_Function::Set(const TDF_Label& L)
{
  Handle(TFunction_Function) F;
  if (!L.FindAttribute(TFunction_Function::GetID(), F)) {
    F = new TFunction_Function();
    L.AddAttribute(F);
  }
  return F;
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a function attribute and initializes 
//         : a driver for it
//=======================================================================

Handle(TFunction_Function) TFunction_Function::Set(const TDF_Label& L, 
						   const Standard_GUID& DriverID)
{
  Handle(TFunction_Function) F;
  if (!L.FindAttribute(TFunction_Function::GetID(), F)) {
    F = new TFunction_Function();  
    L.AddAttribute(F);
  }
  F->SetDriverGUID(DriverID);
  return F;
}

//=======================================================================
//function : ID
//purpose  : Returns GUID of the function
//=======================================================================

const Standard_GUID& TFunction_Function::ID() const
{ return GetID(); }


//=======================================================================
//function : Find
//purpose  : Finds a function if it is on the label
//=======================================================================

// Standard_Boolean TFunction_Function::Find(const TDF_Label& L,
// 					  Handle(TFunction_Function)& F)
// {
//   if (!L.FindAttribute(TFunction_Function::GetID(), F))
//     return Standard_False;
//   return Standard_True;
// }

//=======================================================================
//function : TFunction_Function
//purpose  : Constructor
//=======================================================================

TFunction_Function::TFunction_Function () : myFailure(0)
{}  

//=======================================================================
//function : SetDriverGUID
//purpose  : 
//=======================================================================

void TFunction_Function::SetDriverGUID(const Standard_GUID& guid)
{
  // OCC2932 correction
  if(myDriverGUID == guid) return;

  Backup();
  myDriverGUID = guid;
}

//=======================================================================
//function : SetFailure
//purpose  : Sets the failed status of the function
//=======================================================================

void TFunction_Function::SetFailure (const Standard_Integer mode)
{
  // OCC2932 correction
  if(myFailure == mode) return;

  Backup();
  myFailure = mode;
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TFunction_Function::Restore(const Handle(TDF_Attribute)& other) 
{
  Handle(TFunction_Function) F =  Handle(TFunction_Function)::DownCast(other);
  myFailure    = F->myFailure;
  myDriverGUID = F->myDriverGUID;
}

//=======================================================================
//function : Paste
//purpose  : Method for Copy mechanism
//=======================================================================

void TFunction_Function::Paste(const Handle(TDF_Attribute)& into,
			       const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TFunction_Function) intof = Handle(TFunction_Function)::DownCast(into);
  intof->SetFailure(myFailure);
  intof->SetDriverGUID(myDriverGUID);
}

//=======================================================================
//function : NewEmpty
//purpose  : Returns new empty function attribute
//=======================================================================

Handle(TDF_Attribute) TFunction_Function::NewEmpty() const
{
  return new TFunction_Function();
}

//=======================================================================
//function : References
//purpose  : Collects the references
//=======================================================================

void TFunction_Function::References(const Handle(TDF_DataSet)& /*aDataSet*/) const
{
}

//=======================================================================
//function : Dump
//purpose  : Dump of the function
//=======================================================================

Standard_OStream& TFunction_Function::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  return anOS;
}
