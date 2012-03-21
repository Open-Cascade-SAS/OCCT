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


#include <BOPTools_IteratorOfCoupleOfShape.ixx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_RoughShapeIntersector.hxx>
#include <BOPTools_CoupleOfInteger.hxx>

// ================================================================================================
// function: Constructor
// ================================================================================================
BOPTools_IteratorOfCoupleOfShape::BOPTools_IteratorOfCoupleOfShape() 
:
  myCurrentIndex1(-1),
  myCurrentIndex2(-1),
  myType1(TopAbs_SHAPE),
  myType2(TopAbs_SHAPE),
  myFirstLowerIndex(1),
  myFirstUpperIndex(0),
  mySecondLowerIndex(1),
  mySecondUpperIndex(0)
{
  myPDS = NULL;
}

// ================================================================================================
// function: Constructor
// ================================================================================================
  BOPTools_IteratorOfCoupleOfShape::BOPTools_IteratorOfCoupleOfShape(const BooleanOperations_PShapesDataStructure& PDS,
								     const TopAbs_ShapeEnum Type1,
								     const TopAbs_ShapeEnum Type2)
 :
  myCurrentIndex1(-1),
  myCurrentIndex2(-1),
  myType1(TopAbs_SHAPE),
  myType2(TopAbs_SHAPE)
{
  SetDataStructure(PDS);

  Initialize(Type1, Type2);
}

// ================================================================================================
// function: virtual destructor
// ================================================================================================
void BOPTools_IteratorOfCoupleOfShape::Destroy() 
{
}

// ================================================================================================
// function: SetDataStructure
// ================================================================================================
  void BOPTools_IteratorOfCoupleOfShape::SetDataStructure(const BooleanOperations_PShapesDataStructure& PDS) 
{
  if(PDS==NULL) {
    Standard_NoSuchObject::Raise("BOPTools_IteratorOfCoupleOfShape::SetDataStructure: PDS==NULL");
  }

  myListOfCouple.Clear();

  myPDS = PDS;

  BOPTools_RoughShapeIntersector aRoughIntersector(myPDS);
  aRoughIntersector.Perform();
  if(aRoughIntersector.IsDone()) {
    myTableOfStatus = aRoughIntersector.TableOfStatus();
  } else {
    Handle(BOPTools_HArray2OfIntersectionStatus) anemptyhandle;
    myTableOfStatus = anemptyhandle;
  }
}


// ================================================================================================
// function: Initialize
// ================================================================================================
  void BOPTools_IteratorOfCoupleOfShape::Initialize(const TopAbs_ShapeEnum Type1,
						    const TopAbs_ShapeEnum Type2) 
{

  if(myPDS==NULL) {
    Standard_NoSuchObject::Raise("BOPTools_IteratorOfCoupleOfShape::Initialize: myPDS==NULL");
  }
  myType1 = Type1;
  myType2 = Type2;
  myCurrentIndex1 = -1;
  myCurrentIndex2 = -1;
  //
  myFirstLowerIndex=1;
  myFirstUpperIndex=myPDS->NumberOfShapesOfTheObject();
  mySecondLowerIndex=myFirstUpperIndex+1;
  mySecondUpperIndex=myFirstUpperIndex+myPDS->NumberOfShapesOfTheTool();
  
  NextP();

  Standard_Integer n1, n2;
  
  myListOfCouple.Clear();
  for (; MoreP(); NextP()) {
    CurrentP(n1, n2);
    BOPTools_CoupleOfInteger aCouple(n1, n2);
    myListOfCouple.Append(aCouple);
  }
  myIterator.Initialize(myListOfCouple);
}

//=======================================================================
// function: More
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_IteratorOfCoupleOfShape::More()const
{
  return myIterator.More();
} 
//=======================================================================
// function: Next
// purpose: 
//=======================================================================
  void BOPTools_IteratorOfCoupleOfShape::Next()
{
  myIterator.Next();
} 
//=======================================================================
// function: Current
// purpose: 
//=======================================================================
  void BOPTools_IteratorOfCoupleOfShape::Current(Standard_Integer& Index1,
						 Standard_Integer& Index2,
						 Standard_Boolean& WithSubShape) const
{
  WithSubShape = Standard_False;

  const BOPTools_CoupleOfInteger& aCouple=myIterator.Value();
  aCouple.Couple(Index1, Index2);

  BOPTools_IntersectionStatus aStatus = myTableOfStatus->Value(Index1, Index2);

  if(aStatus == BOPTools_BOUNDINGBOXOFSUBSHAPESINTERSECTED) {
    WithSubShape = Standard_True;
  }
} 

//=======================================================================
// function: ListOfCouple
// purpose: 
//=======================================================================
  const BOPTools_ListOfCoupleOfInteger& BOPTools_IteratorOfCoupleOfShape::ListOfCouple() const
{
  return myListOfCouple;
}

// ================================================================================================
// function: NextP
// ================================================================================================
  void BOPTools_IteratorOfCoupleOfShape::NextP() 
{
  if(myPDS==NULL) {
    myCurrentIndex1 = -1;
    myCurrentIndex2 = -1;
    return;
  } 
  
  Standard_Boolean couplefound, IsValidTableOfStatus = Standard_False;
  Standard_Integer stopedindex1, stopedindex2, starti, i, startj, j;
  TopAbs_ShapeEnum aTypei, aTypej;

  stopedindex1 = myCurrentIndex1;
  stopedindex2 = myCurrentIndex2;
  
  if(!myTableOfStatus.IsNull()) {
    IsValidTableOfStatus = Standard_True;
  }
  
  myCurrentIndex1 = -1;
  myCurrentIndex2 = -1;
  
  couplefound = Standard_False;
  starti = stopedindex1;
  if(starti < 0){
    starti = myFirstLowerIndex;
  }
  for(i = starti; !couplefound && i<=myFirstUpperIndex; i++) {
    startj = mySecondLowerIndex;
    if(i==stopedindex1 && (stopedindex2 >= 0)) {	
      startj = stopedindex2 + 1;
    }
    for(j = startj; !couplefound && j<=mySecondUpperIndex; j++) {
      aTypei=myPDS->GetShapeType(i);
      aTypej=myPDS->GetShapeType(j);
      
      if(((aTypei == myType1) && (aTypej == myType2)) ||
	 ((aTypei == myType2) && (aTypej == myType1))) {
	myCurrentIndex1 = i;
	myCurrentIndex2 = j;
	//
	couplefound = Standard_True;
	if(IsValidTableOfStatus) {
	  BOPTools_IntersectionStatus aStatus = myTableOfStatus->Value(i, j);
	  if(aStatus==BOPTools_INTERSECTED || aStatus==BOPTools_NONINTERSECTED) {
	    myCurrentIndex1 = -1;
	    myCurrentIndex2 = -1;
	    couplefound = Standard_False;
	  }
	}
      }
    }
  }
  //}    
}

// ================================================================================================
// function: More
// ================================================================================================
  Standard_Boolean BOPTools_IteratorOfCoupleOfShape::MoreP() const
{
  if((myCurrentIndex1 < 0) || (myCurrentIndex2 < 0))
    return Standard_False;
  return Standard_True;
}

// ================================================================================================
// function: Current
// ================================================================================================
  void BOPTools_IteratorOfCoupleOfShape::CurrentP(Standard_Integer& Index1,
						  Standard_Integer& Index2) const
{
  if((myCurrentIndex1 < 0) || (myCurrentIndex2 < 0)) {
    Standard_NoSuchObject::Raise("BOPTools_IteratorOfCoupleOfShape::Current");
  }
  Index1 = myCurrentIndex1;
  Index2 = myCurrentIndex2;
}


// ================================================================================================
// function: SetIntersectionStatus
// ================================================================================================
  void BOPTools_IteratorOfCoupleOfShape::SetIntersectionStatus(const Standard_Integer Index1,
							       const Standard_Integer Index2,
							       const BOPTools_IntersectionStatus theStatus) 
{
  if((Index1 >= myTableOfStatus->LowerRow()) &&
     (Index1 <= myTableOfStatus->UpperRow()) &&
     (Index2 >= myTableOfStatus->LowerCol()) &&
     (Index2 <= myTableOfStatus->UpperCol())) {
    myTableOfStatus->ChangeValue(Index1, Index2) = theStatus;
  }
}

// ================================================================================================
// function: GetTableOfIntersectionStatus
// ================================================================================================
  const Handle(BOPTools_HArray2OfIntersectionStatus)& 
    BOPTools_IteratorOfCoupleOfShape::GetTableOfIntersectionStatus() const
{
  return myTableOfStatus;
}

// ================================================================================================
// function: DumpTableOfIntersectionStatus
// ================================================================================================
  void BOPTools_IteratorOfCoupleOfShape::DumpTableOfIntersectionStatus() const
{
  cout << "*BOPTools_IteratorOfCoupleOfShape::DumpTableOfIntersectionStatus.BEGIN*"   << endl;
  cout << "myTableOfStatus.LowerRow="<< myTableOfStatus->LowerRow()   << endl;
  cout << "myTableOfStatus.UpperRow="<< myTableOfStatus->UpperRow()   << endl;
  cout << "myTableOfStatus.LowerCol()="<< myTableOfStatus->LowerCol()   << endl;
  cout << "myTableOfStatus.UpperCol()="<< myTableOfStatus->UpperCol()   << endl;
  for(Standard_Integer k=myTableOfStatus->LowerCol(); k<=myTableOfStatus->UpperCol(); k++) {
    cout << k << " ";
  }
  cout << endl;
  for(Standard_Integer i=myTableOfStatus->LowerRow(); i<=myTableOfStatus->UpperRow(); i++) {
    for(Standard_Integer j=myTableOfStatus->LowerCol(); j<=myTableOfStatus->UpperCol(); j++) {
      cout << myTableOfStatus->Value(i, j) << "  ";
    }
    cout << endl;
  }
  cout << "*BOPTools_IteratorOfCoupleOfShape::DumpTableOfIntersectionStatus.END*"   << endl;
}
