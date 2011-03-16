#include <BOPTools_RoughShapeIntersector.ixx>

#include <TopExp_Explorer.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <Bnd_Box.hxx>

static Standard_Integer TypeToIndex(const TopAbs_ShapeEnum& theType);

static Bnd_Box GetBoxEnlargedBySubShape(const Standard_Integer                        theIndex,
					const BooleanOperations_PShapesDataStructure& PDS);


// =============================================================================================
// function: Constructor
// =============================================================================================
  BOPTools_RoughShapeIntersector::BOPTools_RoughShapeIntersector(const BooleanOperations_PShapesDataStructure& PDS)
:
  myIsDone(Standard_False)
{
  myPDS = PDS;
}

// =============================================================================================
// function: Perform
// =============================================================================================
  void BOPTools_RoughShapeIntersector::Perform() 
{
  myIsDone = Standard_True;

  Prepare();

  // check flag myIsDone after Prepare() function
  if(!myIsDone)
    return;
  TColStd_ListOfInteger thelist;
  TColStd_Array1OfListOfInteger aSortedByTypeShapes1(TypeToIndex(TopAbs_COMPOUND), TypeToIndex(TopAbs_VERTEX));
  aSortedByTypeShapes1.Init(thelist);

  TColStd_ListOfInteger thelist1;
  TColStd_Array1OfListOfInteger aSortedByTypeShapes2(TypeToIndex(TopAbs_COMPOUND), TypeToIndex(TopAbs_VERTEX));
  aSortedByTypeShapes2.Init(thelist1);

  Standard_Integer k = 0;

  for(k = myTableOfStatus->LowerRow(); k <= myTableOfStatus->UpperRow(); k++) {
    aSortedByTypeShapes1(TypeToIndex(myPDS->GetShapeType(k))).Append(k);
  }

  for(k = myTableOfStatus->LowerCol(); k <= myTableOfStatus->UpperCol(); k++) {
    aSortedByTypeShapes2(TypeToIndex(myPDS->GetShapeType(k))).Append(k);
  }

  // 
  Standard_Integer aStartIndex = TypeToIndex(TopAbs_COMPOUND);
  Standard_Integer anEndIndex = TypeToIndex(TopAbs_VERTEX);

  for(Standard_Integer index1 = aStartIndex; index1 <= anEndIndex; index1++) {
  // 
    if(aSortedByTypeShapes1(index1).IsEmpty())
	continue;

    for(Standard_Integer index2 = aStartIndex; index2 <= anEndIndex; index2++) {
      if(aSortedByTypeShapes2(index2).IsEmpty())
	continue;

      TColStd_ListIteratorOfListOfInteger anIt1;
      TColStd_ListIteratorOfListOfInteger anIt2;
	
      for(anIt1.Initialize(aSortedByTypeShapes1.Value(index1)); anIt1.More(); anIt1.Next()) {
	for(anIt2.Initialize(aSortedByTypeShapes2.Value(index2)); anIt2.More(); anIt2.Next()) {
    
	  // 
	  BOPTools_IntersectionStatus aStatus = myTableOfStatus->Value(anIt1.Value(), anIt2.Value());

	  if(aStatus != BOPTools_UNKNOWN && aStatus != BOPTools_BOUNDINGBOXOFSUBSHAPESINTERSECTED)
	    continue;

	  const Bnd_Box& B1 = myBoundingBoxes->Value(anIt1.Value());
	  const Bnd_Box& B2 = myBoundingBoxes->Value(anIt2.Value());
	  // 
	  BOPTools_IntersectionStatus aNewValue = BOPTools_BOUNDINGBOXINTERSECTED;

	  if(B1.IsOut(B2)) {
	    aNewValue = BOPTools_NONINTERSECTED;
	    // 
	    PropagateForSuccessors1(anIt1.Value(), anIt2.Value(), aNewValue);
	    //
	  }
	  else {
	    aNewValue = BOPTools_BOUNDINGBOXINTERSECTED;
	    const Bnd_Box& BB1 = myPDS->GetBoundingBox(anIt1.Value());
	    const Bnd_Box& BB2 = myPDS->GetBoundingBox(anIt2.Value());

	    if(BB1.IsOut(BB2)) {
	      aNewValue = BOPTools_BOUNDINGBOXOFSUBSHAPESINTERSECTED;
	    }
	  }
	  // It is important to place this line after Propagation
	  myTableOfStatus->ChangeValue(anIt1.Value(), anIt2.Value()) = aNewValue;
	}
      }
    }
  }
}

// =============================================================================================
// function: TableOfStatus
// =============================================================================================
const Handle(BOPTools_HArray2OfIntersectionStatus)& BOPTools_RoughShapeIntersector::TableOfStatus() const
{
  return myTableOfStatus;
}

// =============================================================================================
// function: Prepare
// =============================================================================================
void BOPTools_RoughShapeIntersector::Prepare() 
{
  myIsDone = Standard_False;

  if(!myTableOfStatus.IsNull() &&
     (myPDS != NULL) &&
     (myTableOfStatus->LowerRow() == 1) &&
     (myTableOfStatus->UpperRow() == myPDS->NumberOfShapesOfTheObject()) &&
     (myTableOfStatus->LowerCol() == myPDS->NumberOfShapesOfTheObject() + 1) &&
     (myTableOfStatus->UpperCol() == myPDS->NumberOfShapesOfTheObject() + myPDS->NumberOfShapesOfTheTool())) {
    myIsDone = Standard_True;
  }
  else { 
    if(myPDS != NULL) {
      Standard_Integer firstlowerindex = 1;
      Standard_Integer firstupperindex = myPDS->NumberOfShapesOfTheObject();
      Standard_Integer secondlowerindex = myPDS->NumberOfShapesOfTheObject() + 1;
      Standard_Integer secondupperindex = myPDS->NumberOfShapesOfTheTool() + myPDS->NumberOfShapesOfTheObject();
      myTableOfStatus = new BOPTools_HArray2OfIntersectionStatus(firstlowerindex, firstupperindex, 
								 secondlowerindex, secondupperindex);
      myIsDone = Standard_True;
    }
  }
  if(!myIsDone)
    return;

  myTableOfStatus->Init(BOPTools_UNKNOWN);  
  // 
  myBoundingBoxes = new Bnd_HArray1OfBox(1, myPDS->NumberOfSourceShapes());

  for(Standard_Integer i = 1; i <= myPDS->NumberOfSourceShapes(); i++) {    
    const Bnd_Box& aBox = GetBoxEnlargedBySubShape(i, myPDS);
    myBoundingBoxes->SetValue(i, aBox);
  }
}

// =============================================================================================
// function: IsDone
// =============================================================================================
  Standard_Boolean BOPTools_RoughShapeIntersector::IsDone() const
{
  return myIsDone;
}

// =============================================================================================
// function: PropagateForSuccessors1
// =============================================================================================
void BOPTools_RoughShapeIntersector::PropagateForSuccessors1(const Standard_Integer            AncestorsIndex1,
							     const Standard_Integer            AncestorsIndex2,
							     const BOPTools_IntersectionStatus theStatus) 
{
  if(myTableOfStatus->Value(AncestorsIndex1, AncestorsIndex2) != BOPTools_UNKNOWN)
    return;

  myTableOfStatus->ChangeValue(AncestorsIndex1, AncestorsIndex2) = theStatus;
  
  PropagateForSuccessors2(AncestorsIndex1, AncestorsIndex2, theStatus);
  Standard_Integer i = 1;

  for(i = 1; i <= myPDS->NumberOfSuccessors(AncestorsIndex1); i++) {
    Standard_Integer asuccessor1 = myPDS->GetSuccessor(AncestorsIndex1, i);
    PropagateForSuccessors1(asuccessor1, AncestorsIndex2, theStatus);
  }
}

// =============================================================================================
// function: PropagateForSuccessors2
// =============================================================================================
void BOPTools_RoughShapeIntersector::PropagateForSuccessors2(const Standard_Integer            AncestorsIndex1,
							     const Standard_Integer            AncestorsIndex2,
							     const BOPTools_IntersectionStatus theStatus) 
{
  for(Standard_Integer j = 1; j <= myPDS->NumberOfSuccessors(AncestorsIndex2); j++) {
    Standard_Integer asuccessor2 = myPDS->GetSuccessor(AncestorsIndex2, j);

    if(myTableOfStatus->Value(AncestorsIndex1, asuccessor2) == BOPTools_UNKNOWN) {
      myTableOfStatus->ChangeValue(AncestorsIndex1, asuccessor2) = theStatus;

      PropagateForSuccessors2(AncestorsIndex1, asuccessor2, theStatus);
    }
  }
}

// =============================================================================================
// function: TypeToIndex
// =============================================================================================
 Standard_Integer TypeToIndex(const TopAbs_ShapeEnum& theType) {
  return (Standard_Integer) theType;
}

// =============================================================================================
// function: GetBoxEnlargedBySubShape
// =============================================================================================
Bnd_Box GetBoxEnlargedBySubShape(const Standard_Integer                        theIndex,
				 const BooleanOperations_PShapesDataStructure& PDS) {
  Bnd_Box aBox = PDS->GetBoundingBox(theIndex);
  
  for(Standard_Integer i=1; i<= PDS->NumberOfSuccessors(theIndex); i++) {
    Standard_Integer asuccessorindex = PDS->GetSuccessor(theIndex, i);
    aBox.Add(GetBoxEnlargedBySubShape(asuccessorindex, PDS));
  }
  return aBox;
}
