// Created on: 2000-09-07
// Created by: Vincent DELOS
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BooleanOperations_OnceExplorer.ixx>

//#define theStackSize (20)
const static Standard_Integer theStackSize=20;

#define BITFLAG(n) (1 << (n))                 // creation of 2 power n.
#define BITSET(word,mask) (word) |= (mask)    // to set a bit to 1 in word using mask.
#define BITCLEAR(word,mask) (word) &= ~(mask) // to set a bit to 0 in word using mask.
#define BITISSET(word,mask) ((word) & (mask)) // returns the value of the bit corresponding to mask.
#define LEMOT(id) ((id) >> 5)             // the number of the integer we will work on.
#define LEBIT(id) (BITFLAG((id) & 31))    // the number of the bit we will work on (2 power (id%32)).
#define CC0BIT(id,anArray) BITCLEAR(anArray[LEMOT(id)],LEBIT(id))  // sets to 0 the bit number id in anArray.
#define CC1BIT(id,anArray) BITSET(anArray[LEMOT(id)],LEBIT(id))    // sets to 1 the bit number id in anArray.
#define NNNBIT(id,anArray) (BITISSET(anArray[LEMOT(id)],LEBIT(id)) ? 1 : 0) // returns the bit number id in anArray.

//===========================================================================
//function : BooleanOperations_OnceExplorer
//purpose  : 
//===========================================================================
  BooleanOperations_OnceExplorer::BooleanOperations_OnceExplorer
    (const  BooleanOperations_ShapesDataStructure& SDS):
  BooleanOperations_Explorer(SDS)
{
  hasMore = Standard_False;
  // The size of the array of bits is the lower multiple of 
  //32 greater than the number of shapes in myShapesDataStructure.
  Standard_Integer MultipleOf32= (((*myShapesDataStructure).myLength+31) & (~31));

  mySizeOfArrayOfBits = MultipleOf32/32;
  myArrayOfBits = 0L;
}
//modified by NIZNHY-PKV Sun Dec 15 16:28:15 2002 f
//===========================================================================
//function : Delete
//purpose  : alias ~BooleanOperations_Explorer
//===========================================================================
  void BooleanOperations_OnceExplorer::Delete()
{
  if (myArrayOfBits) {
    free (myArrayOfBits);
  }
  BooleanOperations_Explorer::Delete();
}
//modified by NIZNHY-PKV Sun Dec 15 16:29:10 2002 t
//===========================================================================
//function : Init
//purpose  : 
//===========================================================================
  void BooleanOperations_OnceExplorer::Init(const Standard_Integer aShapeNumber,
					    const TopAbs_ShapeEnum TargetToFind,
					    const TopAbs_ShapeEnum  TargetToAvoid)
{
  Standard_Integer i,j,k,theNumberOfTheShapeOnTop,aSuccessorNumber;
  Standard_Integer* anArrayOfBits;
  Standard_Boolean shapeAlreadyProcessed;
  TopAbs_ShapeEnum theTypeOfShapeOnTop,successorType;

  myTargetToFind = TargetToFind;
  myTargetToAvoid = TargetToAvoid;

//  Modified by skv - Thu Apr  7 11:19:39 2005 Begin
  hasMore = Standard_False;
//  Modified by skv - Thu Apr  7 11:19:41 2005 End

  // We first test if myShapesDataStructure has changed.
  Standard_Integer MultipleOf32= (((*myShapesDataStructure).myLength+31) & (~31));
  Standard_Integer NewSize = MultipleOf32/32;
  if (myArrayOfBits!=0L)
    free(myArrayOfBits);
  myArrayOfBits = (Standard_Integer*)calloc(mySizeOfArrayOfBits,sizeof(Standard_Integer));
  mySizeOfArrayOfBits = NewSize;

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
//  Modified by skv - Thu Apr  7 11:19:39 2005 Begin
  if (theTypeOfShapeOnTop == TopAbs_VERTEX) {
    hasMore = Standard_False;

    return;
  }
//  Modified by skv - Thu Apr  7 11:19:41 2005 End

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
	  // We copy the old array in the new one.
	  for (j=0;j<myTopOfStack;j++)
	    ((Standard_Integer*)theNewStack)[j] = ((Standard_Integer*)myStack)[j];

	  Standard::Free((Standard_Address&)myStack);
	  myStack = theNewStack;
	  mySizeOfStack = mySizeOfStack+theStackSize+theNumberOfSuccessors;
	}
      // We remove the shape on top and replace it by its own successors.
      // We must skip the shape of type TargetToAvoid, k counts them.
      k = 0;
      anArrayOfBits = (Standard_Integer*)myArrayOfBits;
      for (i=0;i<theNumberOfSuccessors;i++)
	{
	  aSuccessorNumber = ((Standard_Integer*)theSuccessors)[i];
	  shapeAlreadyProcessed = NNNBIT(aSuccessorNumber,anArrayOfBits);
	  successorType = (*myShapesDataStructure).GetShapeType(((Standard_Integer*)theSuccessors)[i]);
//  Modified by skv - Thu Apr  7 11:19:39 2005 Begin
// 	  if ((successorType == myTargetToAvoid) || (shapeAlreadyProcessed==1))
	  if ((successorType == myTargetToAvoid) || (shapeAlreadyProcessed==1) ||
	      (successorType != myTargetToFind && successorType == TopAbs_VERTEX))
//  Modified by skv - Thu Apr  7 11:19:41 2005 End
	    k++;
	  else
	    {
	      // Insertion of the successor in the stack.
	      ((Standard_Integer*)myStack)[i+myTopOfStack-k] = ((Standard_Integer*)theSuccessors)[i];
	      // We need to set the corresponding bit to one to say that we processed this shape.
	      CC1BIT(aSuccessorNumber,anArrayOfBits);
	    }
	}
      if (theNumberOfSuccessors-k == 0)
	{
	  // No successor of a type different of myTargetToAvoid.
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
  Standard_Integer BooleanOperations_OnceExplorer::Current()
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
  void BooleanOperations_OnceExplorer::Next()
{
  TopAbs_ShapeEnum theTypeOfShapeOnTop,successorType;
  Standard_Integer j,k,theNumberOfTheShapeOnTop,successorNumber;
  Standard_Boolean shapeAlreadyProcessed;
  Standard_Integer* anArrayOfBits;
  
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
      anArrayOfBits = (Standard_Integer*)myArrayOfBits;
      for (j=0;j<theNumberOfSuccessors;j++)
	{
	  successorNumber = ((Standard_Integer*)theSuccessors)[j];
	  successorType = (*myShapesDataStructure).GetShapeType(successorNumber);
	  shapeAlreadyProcessed = NNNBIT(successorNumber,anArrayOfBits);
	  if ((successorType == myTargetToAvoid) || (shapeAlreadyProcessed==1))
	    k++;
	  else
	    {
	      ((Standard_Integer*)myStack)[j+myTopOfStack-k] = ((Standard_Integer*)theSuccessors)[j];
	      // We need to set the corresponding bit to one to say that we processed this shape.
	      CC1BIT(successorNumber,anArrayOfBits);
	    }
	}
      if (theNumberOfSuccessors-k == 0)
	{
	  // No valid successors...
	  myTopOfStack--;
	  if (myTopOfStack < 0)
	    {
	      // Empty stack...
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
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_OnceExplorer::Dump(Standard_OStream& S) const
{
  Standard_Integer u;
  Standard_Integer* anArrayOfBits;
  Standard_Integer* theSuccessors;

  theSuccessors = ((Standard_Integer*)myStack);
  S  << "\n" << "Dump of BooleanOperations_Explorer:" << "\n";
  S << "mySizeOfStack   = " << mySizeOfStack << "\n";
  S << "myTopOfStack    = " << myTopOfStack << "\n";
  S << "myTargetToFind  = " << myTargetToFind << "\n";
  S << "myTargetToAvoid = " << myTargetToAvoid << "\n";
  S << "hasMore         = " << hasMore << "\n";
  for (u=0;u<=myTopOfStack;u++)
    {
      S << " " << *theSuccessors;
      theSuccessors++;
    }

  anArrayOfBits = (Standard_Integer*)myArrayOfBits;

  S << "\n" ;
  for (u=1; u<=mySizeOfArrayOfBits*32; u++)
    {
      S << NNNBIT(u,anArrayOfBits);
      if (u%32==0)
	S << " ";
    }
  S << "\n" ;
}
