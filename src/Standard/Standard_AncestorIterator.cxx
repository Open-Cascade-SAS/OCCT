// Copyright (c) 1998-1999 Matra Datavision
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


#include <Standard_AncestorIterator.ixx>
#include <Standard_NoMoreObject.hxx>

//============================================================================
Standard_AncestorIterator::Standard_AncestorIterator
  (const Standard_AncestorIterator& anOther)
{
  myType   = anOther.myType;
  myNbIter = anOther.myNbIter;
}

//============================================================================
Standard_AncestorIterator::Standard_AncestorIterator
  (const Handle(Standard_Type)& aType)
{
  myType = aType;
  myNbIter = 0;
}

//============================================================================
void Standard_AncestorIterator::Assign
  (const Standard_AncestorIterator& anOther)
{
  myType   = anOther.myType;
  myNbIter = anOther.myNbIter;
}

//============================================================================
Standard_Boolean Standard_AncestorIterator::More() const
{
  return (myNbIter < myType->NumberOfAncestor());
}

//============================================================================
void Standard_AncestorIterator::Next() 
{
  Standard_NoMoreObject_Raise_if(myNbIter == myType->NumberOfAncestor(),
				 "Standard_AncestorIterator::Next()");
  myNbIter++;
}

//============================================================================
Standard_AncestorIterator Standard_AncestorIterator::Iterator() const
{
  Handle(Standard_Type) aType, *allAncestors;

  Standard_NoMoreObject_Raise_if(myNbIter == myType->NumberOfAncestor(),
				 "Standard_AncestorIterator::Next()");
  
  allAncestors = (Handle(Standard_Type) *) myType->Ancestors();
  aType = allAncestors[myNbIter];

  return Standard_AncestorIterator(aType); 
}

//============================================================================
Handle(Standard_Type) Standard_AncestorIterator::Value() const
{
  Handle(Standard_Type) aType, *allAncestors;

  Standard_NoMoreObject_Raise_if(myNbIter == myType->NumberOfAncestor(),
				 "Standard_AncestorIterator::Next()");
  
  allAncestors = (Handle(Standard_Type) *) myType->Ancestors();
  aType = allAncestors[myNbIter];

  return aType; 
}

