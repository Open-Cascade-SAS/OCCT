// File:	BOP_Section.cxx
// Created:	Fri May 18 09:23:28 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_Section.ixx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>

#include <BRep_Builder.hxx>

#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
//
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_SSIntersectionAttribute.hxx>
#include <BOPTools_Tools2D.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>

#include <BOP_CorrectTolerances.hxx>

#include <BOPTColStd_Dump.hxx>

#include <BOP_SectionHistoryCollector.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

//
//
//=======================================================================
// function: BOP_Section::BOP_Section
// purpose: 
//=======================================================================
  BOP_Section::BOP_Section() 
{
  SetOperation (BOP_SECTION);
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void BOP_Section::Destroy() {}

//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_Section::Do()
{
  myErrorStatus=0;
  myIsDone=Standard_False;

  TopAbs_ShapeEnum aT1, aT2;

  aT1=myShape1.ShapeType();
  aT2=myShape2.ShapeType();

  BOP_Builder::SortTypes (aT1, aT2);
  //
  // Filling the DS
  BOPTools_DSFiller aDSFiller;
  aDSFiller.SetShapes (myShape1, myShape2);
  aDSFiller.Perform (); 
  DoWithFiller(aDSFiller);
}

//
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_Section::Do(const Standard_Boolean toApprox,
		       const Standard_Boolean toComputePCurve1,
		       const Standard_Boolean toComputePCurve2) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;

  TopAbs_ShapeEnum aT1, aT2;

  aT1=myShape1.ShapeType();
  aT2=myShape2.ShapeType();

  BOP_Builder::SortTypes (aT1, aT2);
  //
  // Filling the DS
  BOPTools_DSFiller aDSFiller;
  aDSFiller.SetShapes (myShape1, myShape2);

  BOPTools_SSIntersectionAttribute aSectionAttribute(toApprox, 
						     toComputePCurve1, 
						     toComputePCurve2);
  aDSFiller.Perform (aSectionAttribute); 
  DoWithFiller(aDSFiller);
}
//

//=======================================================================
// function: DoDoWithFiller
// purpose: 
//=======================================================================
  void BOP_Section::DoWithFiller(const BOPTools_DSFiller& aDSFiller) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;

  //
  myResultMap.Clear();
  myModifiedMap.Clear();
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
  //

  try {
    OCC_CATCH_SIGNALS
    Standard_Boolean addPCurve1 = aDSFiller.PaveFiller().SectionAttribute().PCurveOnS1();
    Standard_Boolean addPCurve2 = aDSFiller.PaveFiller().SectionAttribute().PCurveOnS2();
  
    Standard_Integer i, j, nF1, nF2,  aNbFFs, aNbS, aNbCurves, nSect;
    
    const BooleanOperations_ShapesDataStructure& aDS=aDSFiller.DS();
    const BOPTools_InterferencePool& anInterfPool=aDSFiller.InterfPool();
    BOPTools_InterferencePool* pInterfPool= 
      (BOPTools_InterferencePool*) &anInterfPool;
    BOPTools_CArray1OfSSInterference& aFFs=
      pInterfPool->SSInterferences();
    //
    TopTools_IndexedMapOfShape aMap;
    //
    aNbFFs=aFFs.Extent();
    for (i=1; i<=aNbFFs; ++i) {
      BOPTools_SSInterference& aFFi=aFFs(i);
      //
      nF1=aFFi.Index1();
      nF2=aFFi.Index2();
      
      TopoDS_Face aF1FWD = TopoDS::Face(aDSFiller.DS().Shape(nF1));
      aF1FWD.Orientation(TopAbs_FORWARD);
      TopoDS_Face aF2FWD = TopoDS::Face(aDSFiller.DS().Shape(nF2));
      aF2FWD.Orientation(TopAbs_FORWARD);
      //
      // Old Section Edges
      const BOPTools_ListOfPaveBlock& aSectList=aFFi.PaveBlocks();
      aNbS=aSectList.Extent();
      BOPTools_ListIteratorOfListOfPaveBlock anIt(aSectList);
      for (; anIt.More();anIt.Next()) {
	const BOPTools_PaveBlock& aPB=anIt.Value();
	nSect=aPB.Edge();
	const TopoDS_Shape& aS=aDS.GetShape(nSect);
	
	const TopoDS_Edge& aE = TopoDS::Edge(aS);
	
	if(addPCurve1) {
	  BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF1FWD);
	  }
	
	if(addPCurve2) {
	  BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF2FWD);
	  }
	aMap.Add(aS);
      }
      //
      // New Section Edges
      BOPTools_SequenceOfCurves& aBCurves=aFFi.Curves();
      aNbCurves=aBCurves.Length();
      for (j=1; j<=aNbCurves; j++) {
	BOPTools_Curve& aBC=aBCurves(j);
	const BOPTools_ListOfPaveBlock& aSectEdges=aBC.NewPaveBlocks();
	aNbS=aSectEdges.Extent();
	
	BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);
	for (; aPBIt.More(); aPBIt.Next()) {
	  BOPTools_PaveBlock& aPB=aPBIt.Value();
	  nSect=aPB.Edge();
	  const TopoDS_Shape& aS=aDS.GetShape(nSect);
	  
	  if(addPCurve1 || addPCurve2) {
	    const IntTools_Curve& aIC = aBC.Curve();
	    const TopoDS_Edge& aE = TopoDS::Edge(aS);
	    Standard_Real f, l;
	    const Handle(Geom_Curve)& aC3DE = BRep_Tool::Curve(aE, f, l);
	    Handle(Geom_TrimmedCurve) aC3DETrim;
	    
	    if(!aC3DE.IsNull()) {
	      aC3DETrim = new Geom_TrimmedCurve(aC3DE, f, l);
	    }
	    BRep_Builder aBB;
	    Standard_Real aTolEdge = BRep_Tool::Tolerance(aE);
	    Standard_Real aTolR2D  = aFFi.TolR2D();
	    Standard_Real aTolFact = Max(aTolEdge, aTolR2D);
	    
	    if(addPCurve1 && !BOPTools_Tools2D::HasCurveOnSurface(aE, aF1FWD)) {
	      Handle(Geom2d_Curve) aC2d = aIC.FirstCurve2d();
	      
	      if(!aC3DETrim.IsNull()) {
		Handle(Geom2d_Curve) aC2dNew;
		
		if(aC3DE->IsPeriodic()) {
		  BOPTools_Tools2D::AdjustPCurveOnFace(aF1FWD, f, l,  aC2d, aC2dNew);
		  }
		else {
		  BOPTools_Tools2D::AdjustPCurveOnFace(aF1FWD, aC3DETrim, aC2d, aC2dNew); 
		  }
		aC2d = aC2dNew;
	      }
	      aBB.UpdateEdge(aE, aC2d, aF1FWD, aTolFact);
	    }
	    
	    if(addPCurve2 && !BOPTools_Tools2D::HasCurveOnSurface(aE, aF2FWD)) {
	      Handle(Geom2d_Curve) aC2d = aIC.SecondCurve2d();
	      
	      if(!aC3DETrim.IsNull()) {
		Handle(Geom2d_Curve) aC2dNew;
		
		if(aC3DE->IsPeriodic()) {
		  BOPTools_Tools2D::AdjustPCurveOnFace(aF2FWD, f, l,  aC2d, aC2dNew);
		  }
		else {
		  BOPTools_Tools2D::AdjustPCurveOnFace(aF2FWD, aC3DETrim, aC2d, aC2dNew); 
		  }
		aC2d = aC2dNew;
	      }
	      aBB.UpdateEdge(aE, aC2d, aF2FWD, aTolFact);
	    }
	  }
	  aMap.Add(aS);
	}
      }
    }
    //
    BRep_Builder BB;
    TopoDS_Compound aCompound;
    BB.MakeCompound(aCompound);
    
    aNbS=aMap.Extent();
    
    for (i=1; i<=aNbS; i++) {
      const TopoDS_Shape& aS=aMap(i);
      BB.Add(aCompound, aS);
      mySectionEdges.Append(aS);
    }
    myResult=aCompound;
    BOP_CorrectTolerances::CorrectTolerances(myResult, 0.01);
    //
    if (!myErrorStatus) {
      FillModified();

      if(!myHistory.IsNull()) {
	Handle(BOP_SectionHistoryCollector) aHistory = 
	  Handle(BOP_SectionHistoryCollector)::DownCast(myHistory);
	aHistory->SetResult(myResult, myDSFiller);
      }
      myIsDone=Standard_True;
    }
  }
  catch ( Standard_Failure ) {
    myErrorStatus = 1;
    BOPTColStd_Dump::PrintMessage("Can not build result\n");
  }
}

//=======================================================================
// function: SetHistoryCollector
// purpose: 
//=======================================================================
void BOP_Section::SetHistoryCollector(const Handle(BOP_HistoryCollector)& theHistory) 
{
  if(theHistory.IsNull() ||
     !theHistory->IsKind(STANDARD_TYPE(BOP_SectionHistoryCollector)))
    myHistory.Nullify();
  else
    myHistory = theHistory;
}
