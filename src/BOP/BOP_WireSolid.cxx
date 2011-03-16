// File:	BOP_WireSolid.cxx
// Created:	Mon Feb  4 11:48:19 2002
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_WireSolid.ixx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_StateOfShape.hxx>

#include <BOPTColStd_Dump.hxx>

#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_WireStateFiller.hxx>


#include <BOP_CorrectTolerances.hxx>
#include <BOP_BuilderTools.hxx>
#include <BOP_WireSolidHistoryCollector.hxx>

//=======================================================================
// function: BOP_WireSolid::BOP_WireSolid
// purpose: 
//=======================================================================
  BOP_WireSolid::BOP_WireSolid()
{
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void BOP_WireSolid::Destroy() 
{
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_WireSolid::Do() 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  // Filling the DS
  BOPTools_DSFiller aDSFiller;
  aDSFiller.SetShapes (myShape1, myShape2);
  //
  aDSFiller.Perform ();
  //
  DoWithFiller(aDSFiller);
}

//=======================================================================
// function: DoWithFiller
// purpose: 
//=======================================================================
  void BOP_WireSolid::DoWithFiller(const BOPTools_DSFiller& aDSFiller) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  myResultMap.Clear();
  myModifiedMap.Clear();
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
  //
  try {
    OCC_CATCH_SIGNALS

    if(!myDSFiller->IsDone()) {
      myErrorStatus = 1;
      BOPTColStd_Dump::PrintMessage("DSFiller is invalid: Can not build result\n");
      return;
    }

    Standard_Boolean bCheckTypes;
    //
    bCheckTypes=CheckArgTypes();
    if (!bCheckTypes) {
      myErrorStatus=10;
      return;
    }
    //
    Standard_Boolean bIsNewFiller;
    bIsNewFiller=aDSFiller.IsNewFiller();
    
    if (bIsNewFiller) {
      //
      // Preparing the States
      const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
      BOPTools_WireStateFiller aStateFiller(aPaveFiller);
      aStateFiller.Do();
      
      aDSFiller.SetNewFiller(!bIsNewFiller);
    }
    //
    BuildResult();
    //
    BOP_CorrectTolerances::CorrectTolerances(myResult, 0.01);
    //
    FillModified();

    if(!myHistory.IsNull()) {
      Handle(BOP_WireSolidHistoryCollector) aHistory = 
	Handle(BOP_WireSolidHistoryCollector)::DownCast(myHistory);
      aHistory->SetResult(myResult, myDSFiller);
    }
    myIsDone=Standard_True;
  }
  catch ( Standard_Failure ) {
    myErrorStatus = 1;
    BOPTColStd_Dump::PrintMessage("Can not build result\n");
  }
}

//=======================================================================
// function: BuildResult
// purpose: 
//=======================================================================
  void BOP_WireSolid::BuildResult()
{
  
  AddSplitParts();
  //
  MakeResult();
}

//=======================================================================
// function: CheckArgTypes
// purpose: 
//=======================================================================
Standard_Boolean BOP_WireSolid::CheckArgTypes(const TopAbs_ShapeEnum theType1,
					      const TopAbs_ShapeEnum theType2,
					      const BOP_Operation    theOperation) 
{
  Standard_Boolean bFlag=Standard_False;

  if (theType1==TopAbs_WIRE && theType2==TopAbs_SOLID) {
    if (theOperation==BOP_FUSE || theOperation==BOP_CUT21) {
      return bFlag;
    }
  }
  //
  if (theType1==TopAbs_SOLID && theType2==TopAbs_WIRE) {
    if (theOperation==BOP_FUSE || theOperation==BOP_CUT) {
      return bFlag;
    }
  }
  //
  return !bFlag;
}

//=======================================================================
// function: CheckArgTypes
// purpose: 
//=======================================================================
  Standard_Boolean BOP_WireSolid::CheckArgTypes() const
{
//   Standard_Boolean bFlag=Standard_False;
  
  TopAbs_ShapeEnum aT1, aT2;
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();

  aT1=aDS.Object().ShapeType();
  aT2=aDS.Tool().ShapeType();
  //
//   if (aT1==TopAbs_WIRE && aT2==TopAbs_SOLID) {
//     if (myOperation==BOP_FUSE || myOperation==BOP_CUT21) {
//       return bFlag;
//     }
//   }
//   //
//   if (aT1==TopAbs_SOLID && aT2==TopAbs_WIRE) {
//     if (myOperation==BOP_FUSE || myOperation==BOP_CUT) {
//       return bFlag;
//     }
//   }
//   //
//   return !bFlag;
  return CheckArgTypes(aT1, aT2, myOperation);
}

//=======================================================================
// function: AddSplitParts
// purpose: 
//=======================================================================
  void BOP_WireSolid::AddSplitParts() 
{
  
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
  const BOPTools_SplitShapesPool& aSplitShapesPool=aPaveFiller.SplitShapesPool();
  //
  Standard_Integer i, aNbPB, iRank, nSp, iBeg, iEnd;
  TopAbs_ShapeEnum aType;
  BooleanOperations_StateOfShape aState, aStateCmp;

  iBeg=1;
  iEnd=aDS.NumberOfShapesOfTheObject();
  if (aDS.Tool().ShapeType()==TopAbs_WIRE) {
    iBeg=iEnd+1;
    iEnd=aDS.NumberOfSourceShapes();
  }

  for (i=iBeg; i<=iEnd; ++i) {
    aType=aDS.GetShapeType(i);
    if (aType!=TopAbs_EDGE) {
      continue;
    }
    const TopoDS_Edge& aE=TopoDS::Edge(aDS.Shape(i));
    iRank=aDS.Rank(i);
  
    aStateCmp=BOP_BuilderTools::StateToCompare(iRank, myOperation);
  
    const BOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(aDS.RefEdge(i));
    aNbPB=aLPB.Extent();
    //
    if (!aNbPB) {
      aState=aDS.GetState(i);
      if (aState==aStateCmp) {
	myLS.Append(aE);
      }
      if (myOperation==BOP_COMMON && aState==BooleanOperations_ON) {
	myLS.Append(aE);
      }
    }
    //
    else {
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aLPB);
      for (; aPBIt.More(); aPBIt.Next()) {
	const BOPTools_PaveBlock& aPB=aPBIt.Value();
	nSp=aPB.Edge();
	const TopoDS_Edge& aSS=TopoDS::Edge(aDS.Shape(nSp));
	aState=aDS.GetState(nSp);
	if (aState==aStateCmp) {
	  myLS.Append(aSS);
	}
	if (myOperation==BOP_COMMON && aState==BooleanOperations_ON) {
	  myLS.Append(aSS);
	}
      }
    }
  }
}

//=======================================================================
// function: SetHistoryCollector
// purpose: 
//=======================================================================
void BOP_WireSolid::SetHistoryCollector(const Handle(BOP_HistoryCollector)& theHistory) 
{
  if(theHistory.IsNull() ||
     !theHistory->IsKind(STANDARD_TYPE(BOP_WireSolidHistoryCollector)))
    myHistory.Nullify();
  else
    myHistory = theHistory;
}
