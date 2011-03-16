
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

