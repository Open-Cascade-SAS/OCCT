// Created on: 1999-11-03
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <TNaming_Scope.ixx>

#include <TDF_Label.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_Tool.hxx>

//=======================================================================
//function : TNaming_Scope
//purpose  : 
//=======================================================================

TNaming_Scope::TNaming_Scope () : myWithValid(Standard_False)
{
}

//=======================================================================
//function : TNaming_Scope
//purpose  : 
//=======================================================================

TNaming_Scope::TNaming_Scope (TDF_LabelMap& map)
{ 
  myWithValid = Standard_True;
  myValid = map;
}

//=======================================================================
//function : TNaming_Scope
//purpose  : 
//=======================================================================

TNaming_Scope::TNaming_Scope (const Standard_Boolean with) : myWithValid(with)
{
}


//=======================================================================
//function : WithValid
//purpose  : 
//=======================================================================
Standard_Boolean TNaming_Scope::WithValid() const
{
  return myWithValid;
}

//=======================================================================
//function : WithValid
//purpose  : 
//=======================================================================
void TNaming_Scope::WithValid(const Standard_Boolean mode) 
{
  myWithValid = mode;
}

//=======================================================================
//function : ClearValid
//purpose  : 
//=======================================================================
void TNaming_Scope::ClearValid() 
{
  myValid.Clear(); 
}

//=======================================================================
//function : Valid
//purpose  : 
//=======================================================================
void TNaming_Scope::Valid(const TDF_Label& L) 
{
  myValid.Add(L);
}

//=======================================================================
//function : ValidChildren
//purpose  : 
//=======================================================================

void TNaming_Scope::ValidChildren(const TDF_Label& L,
					  const Standard_Boolean withroot) 
{  
  if (L.HasChild()) {
    TDF_ChildIterator itc (L,Standard_True);
    for (;itc.More();itc.Next()) myValid.Add(itc.Value());
  }
  if (withroot) myValid.Add(L);
}

//=======================================================================
//function : Unvalid
//purpose  : 
//=======================================================================
void TNaming_Scope::Unvalid(const TDF_Label& L) 
{
  myValid.Remove(L);
}

//=======================================================================
//function : UnvalidChildren
//purpose  : 
//=======================================================================

void TNaming_Scope::UnvalidChildren(const TDF_Label& L,
					  const Standard_Boolean withroot) 
{  
  if (L.HasChild()) {
    TDF_ChildIterator itc (L,Standard_True);
    for (;itc.More();itc.Next()) myValid.Remove(itc.Value());
  }
  if (withroot) myValid.Remove(L);
}

//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================
Standard_Boolean TNaming_Scope::IsValid(const TDF_Label& L) const
{
  if (myWithValid) return myValid.Contains (L);
  return Standard_True;
}

//=======================================================================
//function : GetValid
//purpose  : 
//=======================================================================
const TDF_LabelMap& TNaming_Scope::GetValid() const
{
  return myValid;
}

//=======================================================================
//function : ChangeValid
//purpose  : 
//=======================================================================
TDF_LabelMap& TNaming_Scope::ChangeValid()
{
  return myValid;
}

//=======================================================================
//function : CurrentShape
//purpose  : 
//=======================================================================
TopoDS_Shape TNaming_Scope::CurrentShape(const Handle(TNaming_NamedShape)& NS) const     
{
  if (myWithValid) return TNaming_Tool::CurrentShape(NS,myValid);
  return TNaming_Tool::CurrentShape(NS);
}




