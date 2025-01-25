// Created on: 1999-11-03
// Created by: Denis PASCAL
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

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Scope.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

TNaming_Scope::TNaming_Scope()
    : myWithValid(Standard_False)
{
}

//=================================================================================================

TNaming_Scope::TNaming_Scope(TDF_LabelMap& map)
{
  myWithValid = Standard_True;
  myValid     = map;
}

//=================================================================================================

TNaming_Scope::TNaming_Scope(const Standard_Boolean with)
    : myWithValid(with)
{
}

//=================================================================================================

Standard_Boolean TNaming_Scope::WithValid() const
{
  return myWithValid;
}

//=================================================================================================

void TNaming_Scope::WithValid(const Standard_Boolean mode)
{
  myWithValid = mode;
}

//=================================================================================================

void TNaming_Scope::ClearValid()
{
  myValid.Clear();
}

//=================================================================================================

void TNaming_Scope::Valid(const TDF_Label& L)
{
  myValid.Add(L);
}

//=================================================================================================

void TNaming_Scope::ValidChildren(const TDF_Label& L, const Standard_Boolean withroot)
{
  if (L.HasChild())
  {
    TDF_ChildIterator itc(L, Standard_True);
    for (; itc.More(); itc.Next())
      myValid.Add(itc.Value());
  }
  if (withroot)
    myValid.Add(L);
}

//=================================================================================================

void TNaming_Scope::Unvalid(const TDF_Label& L)
{
  myValid.Remove(L);
}

//=================================================================================================

void TNaming_Scope::UnvalidChildren(const TDF_Label& L, const Standard_Boolean withroot)
{
  if (L.HasChild())
  {
    TDF_ChildIterator itc(L, Standard_True);
    for (; itc.More(); itc.Next())
      myValid.Remove(itc.Value());
  }
  if (withroot)
    myValid.Remove(L);
}

//=================================================================================================

Standard_Boolean TNaming_Scope::IsValid(const TDF_Label& L) const
{
  if (myWithValid)
    return myValid.Contains(L);
  return Standard_True;
}

//=================================================================================================

const TDF_LabelMap& TNaming_Scope::GetValid() const
{
  return myValid;
}

//=================================================================================================

TDF_LabelMap& TNaming_Scope::ChangeValid()
{
  return myValid;
}

//=================================================================================================

TopoDS_Shape TNaming_Scope::CurrentShape(const Handle(TNaming_NamedShape)& NS) const
{
  if (myWithValid)
    return TNaming_Tool::CurrentShape(NS, myValid);
  return TNaming_Tool::CurrentShape(NS);
}
