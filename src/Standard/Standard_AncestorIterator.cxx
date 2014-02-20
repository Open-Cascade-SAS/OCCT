// Copyright (c) 1998-1999 Matra Datavision
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

