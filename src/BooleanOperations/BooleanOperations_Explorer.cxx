// File:	BooleanOperations_Explorer.cxx
// Created:	Mon Sep  4 11:58:18 2000
// Author:	Vincent DELOS
//		<vds@bulox.paris1.matra-dtv.fr>


#include <BooleanOperations_Explorer.ixx>

const static Standard_Integer theStackSize=20;
//
//===========================================================================
//function : BooleanOperations_Explorer
//purpose  : 
//===========================================================================
  BooleanOperations_Explorer::BooleanOperations_Explorer
    (const BooleanOperations_ShapesDataStructure& SDS):
  myStack(0L),
  myTopOfStack(-1),
  mySizeOfStack(-1),
  myTargetToFind(TopAbs_SHAPE),
  myTargetToAvoid(TopAbs_SHAPE)
{
  myShapesDataStructure = (BooleanOperations_PShapesDataStructure)&SDS;
}
//===========================================================================
//function : Delete
//purpose  : ~BooleanOperations_Explorer
//===========================================================================
  void BooleanOperations_Explorer::Delete()
{
  if(myStack != 0L) {
    
    Standard::Free((Standard_Address&)myStack);
  }
  myStack = 0;
}
//===========================================================================
//function : Init
//purpose  : 
//===========================================================================
  void BooleanOperations_Explorer::Init(const Standard_Integer aShapeNumber,
					const TopAbs_ShapeEnum TargetToFind,
					const TopAbs_ShapeEnum  TargetToAvoid)
{
  Standard_Integer i,j,k,theNumberOfTheShapeOnTop;
  TopAbs_ShapeEnum theTypeOfShapeOnTop,successorType;

  myTargetToFind = TargetToFind;
  myTargetToAvoid = TargetToAvoid;

  if (myStack != 0L) {
    Standard::Free((Standard_Address&)myStack);
  }

  mySizeOfStack = theStackSize;
  myStack = (Standard_Integer*)Standard::Allocate(theStackSize*sizeof(Standard_Integer));

  ((Standard_Integer*)myStack)[0] = aShapeNumber;
  myTopOfStack = 0;

  theNumberOfTheShapeOnTop = ((Standard_Integer*)myStack)[myTopOfStack];
  theTypeOfShapeOnTop = (*myShapesDataStructure).GetShapeType(theNumberOfTheShapeOnTop);
  if (theTypeOfShapeOnTop == myTargetToFind)
    {
      hasMore = Standard_True;
      return;
    }
  while (theTypeOfShapeOnTop != myTargetToFind)
    {
      Standard_Address theSuccessors;
      Standard_Integer theNumberOfSuccessors;
      // We get the successors of the shape on top of the stack.
      (*myShapesDataStructure).GetSuccessors(theNumberOfTheShapeOnTop,theSuccessors,theNumberOfSuccessors);
      // Do we have enough place to store our new successors ?
      if ((myTopOfStack+theNumberOfSuccessors > mySizeOfStack) && (theSuccessors != 0L))
	{
	  // We don't have enough place so we reallocate.
	  Standard_Address theNewStack = (Standard_Integer*)Standard::Allocate
	    ((mySizeOfStack+theStackSize+theNumberOfSuccessors)*sizeof(Standard_Integer));
	  // We copy the old array in the one.
	  for (j=0;j<myTopOfStack;j++)
	    ((Standard_Integer*)theNewStack)[j] = ((Standard_Integer*)myStack)[j];

	  Standard::Free((Standard_Address&)myStack);
	  myStack = theNewStack;
	  mySizeOfStack = mySizeOfStack+theStackSize+theNumberOfSuccessors;
	}
      // We remove the shape on top and replace it by its own successors.
      // We must skip the shape of type TargetToAvoid.
      k = 0;
      for (i=0;i<theNumberOfSuccessors;i++)
	{
	  successorType = (*myShapesDataStructure).GetShapeType(((Standard_Integer*)theSuccessors)[i]);
	  if (successorType == myTargetToAvoid)
	    k++;
	  else
	    ((Standard_Integer*)myStack)[i+myTopOfStack-k] = ((Standard_Integer*)theSuccessors)[i];
	}
      if (theNumberOfSuccessors-k == 0)
	{
	  // No valid successor...
	  myTopOfStack--;
	  if (myTopOfStack < 0)
	    {
	      // Empty stack.
	      hasMore = Standard_False;
	      return;
	    }
	}
      else
	myTopOfStack = myTopOfStack+theNumberOfSuccessors-k-1;
      theNumberOfTheShapeOnTop = ((Standard_Integer*)myStack)[myTopOfStack];
      theTypeOfShapeOnTop = (*myShapesDataStructure).GetShapeType(theNumberOfTheShapeOnTop);
      if (theTypeOfShapeOnTop == myTargetToFind)
	{
	  hasMore = Standard_True;
	  return;
	}
    }
}

//===========================================================================
//function : Current
//purpose  : 
//===========================================================================
  Standard_Integer BooleanOperations_Explorer::Current()
{
  myTopOfStack--;
  if (myTopOfStack < 0)
    hasMore = Standard_False;
  return ((Standard_Integer*)myStack)[myTopOfStack+1];
}

//===========================================================================
//function : Next
//purpose  : 
//===========================================================================
  void BooleanOperations_Explorer::Next()
{
  TopAbs_ShapeEnum theTypeOfShapeOnTop,successorType;
  Standard_Integer j,k,theNumberOfTheShapeOnTop,successorNumber;
  
  if (myTopOfStack < 0)
    {
      hasMore = Standard_False;
      return;
    }
  theNumberOfTheShapeOnTop = ((Standard_Integer*)myStack)[myTopOfStack];
  theTypeOfShapeOnTop = (*myShapesDataStructure).GetShapeType(theNumberOfTheShapeOnTop);
  if (theTypeOfShapeOnTop == myTargetToFind)
    {
      hasMore = Standard_True;
      return;
    }
  while (theTypeOfShapeOnTop != myTargetToFind)
    {
      Standard_Address theSuccessors = 0L;
      Standard_Integer theNumberOfSuccessors;
      (*myShapesDataStructure).GetSuccessors(theNumberOfTheShapeOnTop,(Standard_Address&)theSuccessors,theNumberOfSuccessors);
      // Do we have enough place to store our new successors ?
      if ((myTopOfStack+theNumberOfSuccessors > mySizeOfStack) && (theSuccessors != 0L))
	{
	  Standard_Address theNewStack;
	  theNewStack = (Standard_Integer*)Standard::Allocate
	    ((mySizeOfStack+theNumberOfSuccessors+theStackSize)*sizeof(Standard_Integer));
	  for (j=0;j<myTopOfStack;j++)
	    ((Standard_Integer*)theNewStack)[j] = ((Standard_Integer*)myStack)[j];
	  
	  Standard::Free((Standard_Address&)myStack);
	  myStack = theNewStack;
	  mySizeOfStack = mySizeOfStack+theNumberOfSuccessors+theStackSize;
	}
      // We remove the shape on top and replace it by its own successors.
      // We must skip the shape of type TargetToAvoid.
      k = 0;
      for (j=0;j<theNumberOfSuccessors;j++)
	{
	  successorNumber = ((Standard_Integer*)theSuccessors)[j];
	  successorType = (*myShapesDataStructure).GetShapeType(successorNumber);
	  if (successorType == myTargetToAvoid)
	    k++;
	  else
	    ((Standard_Integer*)myStack)[j+myTopOfStack-k] = ((Standard_Integer*)theSuccessors)[j];
	}
      if (theNumberOfSuccessors-k == 0)
	{
	  myTopOfStack--;
	  if (myTopOfStack < 0)
	    {
	      hasMore = Standard_False;
	      return;
	    }
	}
      else
	myTopOfStack = myTopOfStack+theNumberOfSuccessors-k-1;
      theNumberOfTheShapeOnTop = ((Standard_Integer*)myStack)[myTopOfStack];
      theTypeOfShapeOnTop = (*myShapesDataStructure).GetShapeType(theNumberOfTheShapeOnTop);
      if (theTypeOfShapeOnTop == myTargetToFind)
	{
	  hasMore = Standard_True;
	  return;
	}
    }
}

//===========================================================================
//function : More
//purpose  : 
//===========================================================================
  Standard_Boolean BooleanOperations_Explorer::More() const
{
  return hasMore;
}

//===========================================================================
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_Explorer::Dump(Standard_OStream& S) const
{
  Standard_Integer i;
  Standard_Integer* theSuccessors;

  theSuccessors = ((Standard_Integer*)myStack);
  S  << "\n" << "Dump of BooleanOperations_Explorer:" << "\n";
  S << "mySizeOfStack   = " << mySizeOfStack << "\n";
  S << "myTopOfStack    = " << myTopOfStack << "\n";
  S << "myTargetToFind  = " << myTargetToFind << "\n";
  S << "myTargetToAvoid = " << myTargetToAvoid << "\n";
  S << "hasMore         = " << hasMore << "\n";
  for (i=0;i<=myTopOfStack;i++)
    {
      S << " " << *theSuccessors;
      theSuccessors++;
    }
  S  << "\n";
}
