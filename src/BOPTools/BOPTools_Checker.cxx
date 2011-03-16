// File:	BOPTools_Checker.cxx
// Created:	Mon Aug  5 16:06:12 2002
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_Checker.ixx>

#include <stdio.h>  
#include <stdlib.h> 

#include <Precision.hxx>

#include <gp_Pnt.hxx>

#include <Geom_CartesianPoint.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

#include <TopExp.hxx>

#include <Bnd_Box.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BOPTools_Pave.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
// modified by NIZHNY-MKK  Fri Sep  3 16:00:15 2004.BEGIN
#include <BOPTools_CheckResult.hxx>
// modified by NIZHNY-MKK  Fri Sep  3 16:00:18 2004.END

#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <IntTools_EdgeFace.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_PntOnFace.hxx>
#include <IntTools_Tools.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>

#include <BOPTColStd_Failure.hxx>

//=======================================================================
// function:  BOPTools_Checker::BOPTools_Checker
// purpose: 
//=======================================================================
BOPTools_Checker::BOPTools_Checker() : BOPTools_PaveFiller()
{
  myEntryType=1;
  myStopOnFirst = Standard_False;
}
//=======================================================================
// function:  BOPTools_Checker::BOPTools_Checker
// purpose: 
//=======================================================================
BOPTools_Checker::BOPTools_Checker(const TopoDS_Shape& aS) : BOPTools_PaveFiller()
{
  myEntryType=1;
  myStopOnFirst = Standard_False;
  SetShape(aS);
} 
//=======================================================================
// function:  BOPTools_Checker::BOPTools_Checker
// purpose: 
//=======================================================================
BOPTools_Checker::BOPTools_Checker(const BOPTools_InterferencePool& aPool) : BOPTools_PaveFiller(aPool)
{
  myStopOnFirst = Standard_False;
  myEntryType=0;
  myIsDone=Standard_False;
  void* p=(void*) &aPool;
  myIntrPool=(BOPTools_InterferencePool*) p;
  myDS=myIntrPool->DS();
  myNbSources=myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myNbEdges=myDS->NbEdges();
}

//=======================================================================
// function: SetShape
// purpose: 
//=======================================================================
void  BOPTools_Checker::SetShape(const TopoDS_Shape& aS)
{
  myShape=aS;

  Destroy();
  myDS = new BooleanOperations_ShapesDataStructure (aS, aS);
  
  myIntrPool = new BOPTools_InterferencePool (*myDS);

  myNbSources=myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myNbEdges=myDS->NbEdges();
}

//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
void BOPTools_Checker::Destroy()
{
  if (myEntryType) {
    //
    if (myIntrPool!=NULL) {
      delete myIntrPool; myIntrPool = NULL;
    }
    if (myDS!=NULL) {
      delete myDS; myDS = NULL;
    }
  }
  myCheckResults.Clear();
}

//=======================================================================
// function: SetPerformType
// purpose: 
//=======================================================================

void BOPTools_Checker::SetPerformType(const Standard_Boolean StopOnFirstFaulty)
{
  myStopOnFirst = StopOnFirstFaulty;
}

//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
void BOPTools_Checker::Perform()
{
  myCheckResults.Clear();
  try {
    //
    // 0. Prepare the IteratorOfCoupleOfShape
    myDSIt.SetDataStructure(myDS);
    //
    // 1.VV
    PerformVV();
    //
    // 2.VE
    myPavePool.Resize (myNbEdges);
    PrepareEdges();
    PerformVE();
    //
    // 3.VF
    PerformVF();
    //
    // 4.EE
    myCommonBlockPool.Resize (myNbEdges);
    mySplitShapesPool.Resize (myNbEdges);
    myPavePoolNew    .Resize (myNbEdges);
    
    PreparePaveBlocks(TopAbs_VERTEX, TopAbs_EDGE);
    PreparePaveBlocks(TopAbs_EDGE, TopAbs_EDGE);
    
    PerformEE();
    //
    // 5.EF
    PreparePaveBlocks(TopAbs_EDGE, TopAbs_FACE);
    
    PerformEF();
    //
    // 6. FF
    PerformFF ();
  }// end of try block
  //
  catch (BOPTColStd_Failure& x) {
    cout << x.Message() << endl << flush;
  }
}
//=======================================================================
// function: PerformVV
// purpose: 
//=======================================================================
void BOPTools_Checker::PerformVV()
{
  myIsDone=Standard_False;
  Standard_Boolean bJustAddInterference;
  Standard_Integer n1, n2, aFlag;
  //
  // V/V  BooleanOperations_VertexVertex
  myDSIt.Initialize(TopAbs_VERTEX, TopAbs_VERTEX);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    bJustAddInterference = Standard_False;
    myDSIt.Current(n1, n2, bJustAddInterference);
    //
    const TopoDS_Shape& aS1=myDS->Shape(n1);
    const TopoDS_Shape& aS2=myDS->Shape(n2);
    //
    if (aS1.IsSame(aS2)){
      continue;
    }
    //
    if(bJustAddInterference) {
      continue;
    }
    //
    const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
    const TopoDS_Vertex& aV2=TopoDS::Vertex(aS2);
    
    aFlag=IntTools_Tools::ComputeVV (aV1, aV2);
    
    if (!aFlag) {
      char buf[512];
      sprintf (buf, "VV: (%d, %d)", n1, n2);

      BOPTools_CheckResult aChRes;
      aChRes.AddShape(aV1);
      aChRes.AddShape(aV2);
      aChRes.SetCheckStatus(BOPTools_VERTEXVERTEX);
      myCheckResults.Append(aChRes);

      if(myStopOnFirst)
        throw BOPTColStd_Failure(buf) ;
    }
  }
  myIsDone=Standard_True;
}

//=======================================================================
// function: PerformVE
// purpose: 
//=======================================================================
void BOPTools_Checker::PerformVE()
{
  myIsDone=Standard_False;
  Standard_Boolean bSameFlag, bJustAddInterference;
  Standard_Integer n1, n2, aFlag, aWhat, aWith;
  Standard_Real aT;
  //
  // V/E Interferences  [BooleanOperations_VertexEdge]
  myDSIt.Initialize (TopAbs_VERTEX, TopAbs_EDGE);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    bJustAddInterference = Standard_False;
    myDSIt.Current(n1, n2, bJustAddInterference);
    //
    aWhat=n1; // Vertex
    aWith=n2; // Edge

    SortTypes(aWhat, aWith);
    
    const TopoDS_Shape& aS1=myDS->Shape(aWhat);
    const TopoDS_Shape& aS2=myDS->Shape(aWith);
    
    const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
    const TopoDS_Edge&   aE2=TopoDS::Edge  (aS2);
    
    if (BRep_Tool::Degenerated(aE2)){
      continue;
    }
    //
    TopTools_IndexedMapOfShape aM2;
    //
    bSameFlag=Standard_False;
    //
    BOPTools_Tools::MapShapes(aE2, aM2);
    //
    if (aM2.Contains(aV1)) {
      bSameFlag=Standard_True;
    }
    //
    if (bSameFlag){
      continue;
    }
    //
    aFlag=myContext.ComputeVE (aV1, aE2, aT);
    //
    if (!aFlag) {
      char buf[512];
      sprintf (buf, "VE: (%d, %d)", aWhat, aWith);

      BOPTools_CheckResult aChRes;
      aChRes.AddShape(aV1);
      aChRes.AddShape(aE2);
      aChRes.SetCheckStatus(BOPTools_VERTEXEDGE);
      myCheckResults.Append(aChRes);
      //
      if(myStopOnFirst)
        throw BOPTColStd_Failure(buf) ;
    }
  }
  myIsDone=Standard_True;
}

//=======================================================================
// function: PerformVF
// purpose: 
//=======================================================================
void BOPTools_Checker::PerformVF()
{
  myIsDone=Standard_False;
  Standard_Boolean justaddinterference, bSameFlag;
  Standard_Integer n1, n2, aFlag, aWhat, aWith;
  Standard_Real aU, aV;
  //
  // V/V  BooleanOperations_VertexFace
  myDSIt.Initialize(TopAbs_VERTEX, TopAbs_FACE);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    justaddinterference = Standard_False;
    myDSIt.Current(n1, n2, justaddinterference);
    //
    aWhat=n1; // Vertex
    aWith=n2; // Face
    SortTypes(aWhat, aWith);
    
    const TopoDS_Shape& aS1=myDS->Shape(aWhat);
    const TopoDS_Shape& aS2=myDS->Shape(aWith);
   
    const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
    const TopoDS_Face&   aF2=TopoDS::Face  (aS2);
    //
    TopTools_IndexedMapOfShape aM2;
    //
    bSameFlag=Standard_False;
    //
    BOPTools_Tools::MapShapes(aF2, aM2);
    //
    if (aM2.Contains(aV1)) {
      bSameFlag=Standard_True;
    }
    //
    if (bSameFlag){
      continue;
    }
    //
    aFlag=myContext.ComputeVS (aV1, aF2, aU, aV);
    //
    if (!aFlag) {
      char buf[512];
      sprintf (buf, "VF: (%d, %d)", aWhat, aWith);

      BOPTools_CheckResult aChRes;
      aChRes.AddShape(aV1);
      aChRes.AddShape(aF2);
      aChRes.SetCheckStatus(BOPTools_VERTEXFACE);
      myCheckResults.Append(aChRes);

      if(myStopOnFirst)
        throw BOPTColStd_Failure(buf) ;
    }
  }
  myIsDone=Standard_True;
}

//=======================================================================
// function: PerformEE
// purpose: 
//=======================================================================
void BOPTools_Checker::PerformEE()
{
  myIsDone=Standard_False;

  Standard_Boolean justaddinterference;
  Standard_Integer n1, n2, anIndexIn=0, nE1, nE2;
  Standard_Integer aTmp, aWhat, aWith;
  Standard_Integer i, aNbCPrts;
  //
  // E/E Interferences  [BooleanOperations_EdgeEdge]
  myDSIt.Initialize(TopAbs_EDGE, TopAbs_EDGE);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    justaddinterference = Standard_False;
    myDSIt.Current(n1, n2, justaddinterference);
    //
    nE1=n1; 
    nE2=n2; 
    SortTypes(nE1, nE2);
    //
    Standard_Real aTolE1, aTolE2, aDeflection=0.01;
    Standard_Integer aDiscretize=30;

    const TopoDS_Edge& aE1=TopoDS::Edge(myDS->GetShape(nE1));
    const TopoDS_Edge& aE2=TopoDS::Edge(myDS->GetShape(nE2));
    //
    if (BRep_Tool::Degenerated(aE1)){
      continue;
    }
    if (BRep_Tool::Degenerated(aE2)){
      continue;
    }
    //
    // 
    Standard_Boolean bSameFlag;
    TopTools_IndexedMapOfShape aM1, aM2;
    //
    bSameFlag=aE1.IsSame(aE2);
    //
    if (bSameFlag){
      continue;
    }
    //
    aTolE1=BRep_Tool::Tolerance(aE1);
    aTolE2=BRep_Tool::Tolerance(aE2);
    //
    BOPTools_ListOfPaveBlock& aLPB1=mySplitShapesPool(myDS->RefEdge(nE1));
    BOPTools_ListIteratorOfListOfPaveBlock anIt1(aLPB1);

    for (; anIt1.More(); anIt1.Next()) {
      BOPTools_PaveBlock& aPB1=anIt1.Value();
      const IntTools_ShrunkRange& aShrunkRange1=aPB1.ShrunkRange();
    
      const IntTools_Range& aSR1=aShrunkRange1.ShrunkRange();
      const Bnd_Box&        aBB1=aShrunkRange1.BndBox();

      BOPTools_ListOfPaveBlock& aLPB2=mySplitShapesPool(myDS->RefEdge(nE2));
      BOPTools_ListIteratorOfListOfPaveBlock anIt2(aLPB2);
      
      for (; anIt2.More(); anIt2.Next()) {
	BOPTools_PaveBlock& aPB2=anIt2.Value();
	const IntTools_ShrunkRange& aShrunkRange2=aPB2.ShrunkRange();
      
	const IntTools_Range& aSR2=aShrunkRange2.ShrunkRange();
	const Bnd_Box&        aBB2=aShrunkRange2.BndBox();
	
	//////////////////////////////////////////////
	if (aBB1.IsOut (aBB2)) {
	  continue;
	}
	// 
	// EE
	IntTools_EdgeEdge aEE;
	aEE.SetEdge1 (aE1);
	aEE.SetEdge2 (aE2);
	aEE.SetTolerance1 (aTolE1);
	aEE.SetTolerance2 (aTolE2);
	aEE.SetDiscretize (aDiscretize);
	aEE.SetDeflection (aDeflection);
	//
	IntTools_Range anewSR1 = aSR1;
	IntTools_Range anewSR2 = aSR2;
	//
	BOPTools_Tools::CorrectRange (aE1, aE2, aSR1, anewSR1);
	BOPTools_Tools::CorrectRange (aE2, aE1, aSR2, anewSR2);
	//
	aEE.SetRange1(anewSR1);
	aEE.SetRange2(anewSR2);
	  
	aEE.Perform();
	//
	anIndexIn=0;
	//
	if (aEE.IsDone()) {
	  //
	  // reverse order if it is necessary
	  TopoDS_Edge aEWhat, aEWith;
	  aEWhat=aE1;
	  aEWith=aE2;
	  aWhat=nE1;
	  aWith=nE2;
	  if (aEE.Order()) {
	    aTmp=aWhat;
	    aWhat=aWith;
	    aWith=aTmp;
	    aEWhat=aE2;
	    aEWith=aE1;
	  }
	  //
	  const IntTools_SequenceOfCommonPrts& aCPrts=aEE.CommonParts();
	  
	  aNbCPrts=aCPrts.Length();
	  for (i=1; i<=aNbCPrts; i++) {
	    const IntTools_CommonPrt& aCPart=aCPrts(i);
	    //
	    anIndexIn=0;
	    //
	    TopAbs_ShapeEnum aType=aCPart.Type();
	    switch (aType) {
	      
  	      case TopAbs_VERTEX:  {
		
		Standard_Real aT1, aT2; 
		
		const IntTools_Range& aR1=aCPart.Range1();
		aT1=0.5*(aR1.First()+aR1.Last());

		if((aCPart.VertexParameter1() >= aR1.First()) &&
		   (aCPart.VertexParameter1() <= aR1.Last())) {
		  aT1 = aCPart.VertexParameter1();
		}

		const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
		const IntTools_Range& aR2=aRanges2(1);
		aT2=0.5*(aR2.First()+aR2.Last());

		if((aCPart.VertexParameter2() >= aR2.First()) &&
		   (aCPart.VertexParameter2() <= aR2.Last())) {
		  aT2 = aCPart.VertexParameter2();
		}
		//
		char buf[512];
		sprintf (buf, "EE: (%d, %d), vertex at t1=%f, t2=%f", aWhat, aWith, aT1, aT2);
		//
		gp_Pnt aPnt;
		BOPTools_Tools::PointOnEdge(aEWhat, aT1, aPnt);
		Handle (Geom_CartesianPoint) aCPnt= new Geom_CartesianPoint(aPnt);
// 		myInerference=aCPnt;

                BOPTools_CheckResult aChRes;
                aChRes.AddShape(aE1);
                aChRes.AddShape(aE2);
                aChRes.SetCheckStatus(BOPTools_EDGEEDGE);
// modified by NIZHNY-MKK  Fri Sep  3 16:01:52 2004
//                 aChRes.SetInterferenceGeometry(myInerference);
		aChRes.SetInterferenceGeometry(aCPnt);
                myCheckResults.Append(aChRes);

                if(myStopOnFirst)
                  throw BOPTColStd_Failure(buf) ;
		//
	      }
	      break;

	      case TopAbs_EDGE: {
	      
		const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
		Standard_Integer aNbComPrt2=aRanges2.Length();
		
		if (aNbComPrt2>1) {
		  break;
		}

		Standard_Boolean aCoinsideFlag;
		
		aCoinsideFlag=IsBlocksCoinside(aPB1, aPB2);
		//
		if (!aCoinsideFlag) {
		  break;
		}
		//
		char buf[512];
		sprintf (buf, "EE: (%d, %d), common block ", aWhat, aWith);
                
                BOPTools_CheckResult aChRes;
                aChRes.AddShape(aE1);
                aChRes.AddShape(aE2);
                aChRes.SetCheckStatus(BOPTools_EDGEEDGECOMBLK);
                myCheckResults.Append(aChRes);

                if(myStopOnFirst)
                  throw BOPTColStd_Failure(buf) ;
		//
	      }
	      break;

	    default:
	      break;
	    } // switch (aType) 
	  } // for (i=1; i<=aNbCPrts; i++) 
	}// if (aEE.IsDone())
	
	//////////////////////////////////////////////
      } // for (; anIt2.More(); anIt2.Next()) 
    } // for (; anIt1.More(); anIt1.Next()) 
  }// for (; myDSIt.More(); myDSIt.Next()) 
  myIsDone=Standard_True;
}

//=======================================================================
// function: PerformEF
// purpose: 
//=======================================================================
void BOPTools_Checker::PerformEF()
{
  myIsDone=Standard_False;
  //
  Standard_Boolean justaddinterference, bSameFlag;
  Standard_Integer n1, n2,  nE, nF, i, aNbCPrts;
  //
  // E/F Interferences  [BooleanOperations_EdgeFace]
  myDSIt.Initialize(TopAbs_EDGE, TopAbs_FACE);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    justaddinterference = Standard_True;
    myDSIt.Current(n1, n2, justaddinterference);
    //
    nE=n1; 
    nF=n2; 
    SortTypes(nE, nF);
    //
    Standard_Real aTolE, aTolF, aDeflection=0.01;
    Standard_Integer aDiscretize=35;

    const TopoDS_Edge& aE=TopoDS::Edge(myDS->GetShape(nE));
    const TopoDS_Face& aF=TopoDS::Face(myDS->GetShape(nF));
    //
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    // 
    TopTools_IndexedMapOfShape aMF;
    //
    bSameFlag=Standard_False;
    //
    TopExp::MapShapes(aF, TopAbs_EDGE, aMF);
    if (aMF.Contains(aE)) { 
      bSameFlag=Standard_True;
    }
    //
    if (bSameFlag){
      continue;
    }
    //
    aTolE=BRep_Tool::Tolerance(aE);
    aTolF=BRep_Tool::Tolerance(aF);
    //
    const Bnd_Box& aBBF=myDS->GetBoundingBox(nF); 
    //
    BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE));
    BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);

    for (; anIt.More(); anIt.Next()) {
      BOPTools_PaveBlock& aPB=anIt.Value();
      const IntTools_ShrunkRange& aShrunkRange=aPB.ShrunkRange();
      const IntTools_Range& aSR =aShrunkRange.ShrunkRange();
      const Bnd_Box&        aBBE=aShrunkRange.BndBox();
      //
      if (aBBF.IsOut (aBBE)) {
	continue;
      }
      // 
      // EF
      IntTools_EdgeFace aEF;
      aEF.SetEdge (aE);
      aEF.SetFace (aF);
      aEF.SetTolE (aTolE);
      aEF.SetTolF (aTolF);
      aEF.SetDiscretize (aDiscretize);
      aEF.SetDeflection (aDeflection);

      IntTools_Range anewSR = aSR;
      //
      BOPTools_Tools::CorrectRange(aE, aF, aSR, anewSR);
      //
      aEF.SetRange (anewSR);
      //
      aEF.Perform();
      //
      if (aEF.IsDone()) {
	//
	const IntTools_SequenceOfCommonPrts& aCPrts=aEF.CommonParts();
	aNbCPrts=aCPrts.Length();
	for (i=1; i<=aNbCPrts; i++) {
	  const IntTools_CommonPrt& aCPart=aCPrts(i);
	  //
	  TopAbs_ShapeEnum aType=aCPart.Type();
	  switch (aType) {
	      
	    case TopAbs_VERTEX:  {
		
	      Standard_Real aT; 
	      
	      const IntTools_Range& aR=aCPart.Range1();

	      Standard_Real aRFirst, aRLast;

	      aR.Range(aRFirst, aRLast);
	      aT=0.5*(aRFirst+aRLast);

	      if((aCPart.VertexParameter1() >= aRFirst) &&
		 (aCPart.VertexParameter1() <= aRLast)) {
		aT = aCPart.VertexParameter1();
	      }
	      //
	      char buf[512];
	      sprintf (buf, "EF: (%d, %d), vertex at t=%f", nE, nF, aT);
	      //
	      gp_Pnt aPnt;
	      BOPTools_Tools::PointOnEdge(aE, aT, aPnt);
	      Handle (Geom_CartesianPoint) aCPnt= new Geom_CartesianPoint(aPnt);
// 	      myInerference=aCPnt;

              BOPTools_CheckResult aChRes;
              aChRes.AddShape(aE);
              aChRes.AddShape(aF);
              aChRes.SetCheckStatus(BOPTools_EDGEFACE);
// modified by NIZHNY-MKK  Fri Sep  3 16:02:10 2004
//               aChRes.SetInterferenceGeometry(myInerference);
              aChRes.SetInterferenceGeometry(aCPnt);
              myCheckResults.Append(aChRes);

              if(myStopOnFirst)
                throw BOPTColStd_Failure(buf) ;
	    }// case TopAbs_VERTEX:
	      break;

	    case TopAbs_EDGE: {
	      
	      Standard_Boolean aCoinsideFlag;
	      aCoinsideFlag=BOPTools_Tools::IsBlockInOnFace(aPB, aF, myContext);
	      if (!aCoinsideFlag) {
		break;
	      }
	      //
	      char buf[512];
	      sprintf (buf, "EF: (%d, %d), common block ", nE, nF);

              BOPTools_CheckResult aChRes;
              aChRes.AddShape(aE);
              aChRes.AddShape(aF);
              aChRes.SetCheckStatus(BOPTools_EDGEFACECOMBLK);
              myCheckResults.Append(aChRes);

              if(myStopOnFirst)
                throw BOPTColStd_Failure(buf) ;
	    }// case TopAbs_EDGE:
	      break;

	    default:
	      break;
	  } // switch (aType) 
	} // for (i=1; i<=aNbCPrts; i++) 
      } //if (aEF.IsDone())
    } // for (; anIt.More(); anIt.Next()) 
  }// for (; myDSIt.More(); myDSIt.Next()) 
  myIsDone=Standard_True;
}

//=======================================================================
// function: PerformFF
// purpose: 
//=======================================================================
  void BOPTools_Checker::PerformFF()
{
  myIsDone=Standard_False;
  //
  Standard_Boolean justaddinterference, bSameFlag;
  Standard_Integer n1, n2, nF1, nF2, i, aNbS1;
  //
  //  F/F Interferences  [BooleanOperations_SurfaceSurface]
  myDSIt.Initialize(TopAbs_FACE, TopAbs_FACE);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    justaddinterference = Standard_True;
    myDSIt.Current(n1, n2, justaddinterference);
    //
    nF1=n1; 
    nF2=n2; 
    if (nF1 > nF2) {
      Standard_Integer iTmp;
      iTmp=nF1;
      nF1=nF2;
      nF2=iTmp;
    }
    //
    const TopoDS_Face& aF1=TopoDS::Face(myDS->Shape(nF1));
    const TopoDS_Face& aF2=TopoDS::Face(myDS->Shape(nF2));
    //
    TopTools_IndexedMapOfShape aM1, aM2;
    //
    bSameFlag=Standard_False;
    //
    TopExp::MapShapes(aF1, TopAbs_EDGE, aM1);
    TopExp::MapShapes(aF2, TopAbs_EDGE, aM2);
    //
    aNbS1=aM1.Extent();

    for (i=1; i<=aNbS1; ++i) {
      const TopoDS_Shape& aS1=aM1(i);
      if (aM2.Contains(aS1)) {
	bSameFlag=Standard_True;
	break;
      }
    }
    //
    if (bSameFlag){
      continue;
    }
    //
    // FF
    Standard_Boolean bToApproxC3d, bToApproxC2dOnS1, bToApproxC2dOnS2;
    Standard_Real anApproxTol, aTolR3D, aTolR2D;
    //
    bToApproxC3d     = mySectionAttribute.Approximation();
    bToApproxC2dOnS1 = mySectionAttribute.PCurveOnS1();
    bToApproxC2dOnS2 = mySectionAttribute.PCurveOnS2();
    //
    anApproxTol=1.e-7;

    IntTools_FaceFace aFF;
    aFF.SetParameters (bToApproxC3d, 
		       bToApproxC2dOnS1, 
		       bToApproxC2dOnS2,
		       anApproxTol);
	  
    aFF.Perform(aF1, aF2);

    if (aFF.IsDone()) {
      // Add Interference to the Pool
      aTolR3D=aFF.TolReached3d();
      aTolR2D=aFF.TolReached2d();
      if (aTolR3D < 1.e-7){
	aTolR3D=1.e-7;
      } 
      aFF.PrepareLines3D();
      //
      //
      Standard_Integer j, aNbCurves, aNbPoints;
      //
      const IntTools_SequenceOfCurves& aCvs=aFF.Lines();
      aNbCurves=aCvs.Length();
      //
      const IntTools_SequenceOfPntOn2Faces& aPnts=aFF.Points();
      aNbPoints=aPnts.Length();
      
      if (aNbPoints) {
	char buf[512];
	sprintf (buf, "FF: (%d, %d) ", nF1, nF2);
	//
	const IntTools_PntOn2Faces& aPntOn2Faces=aPnts(1);
	const IntTools_PntOnFace& aPntOnFace=aPntOn2Faces.P1();
	const gp_Pnt& aPnt=aPntOnFace.Pnt();
	Handle (Geom_CartesianPoint) aCPnt= new Geom_CartesianPoint(aPnt);
// 	myInerference=aCPnt;

        BOPTools_CheckResult aChRes;
        aChRes.AddShape(aF1);
        aChRes.AddShape(aF2);
        aChRes.SetCheckStatus(BOPTools_FACEFACE);
// modified by NIZHNY-MKK  Fri Sep  3 16:02:25 2004
//         aChRes.SetInterferenceGeometry(myInerference);
        aChRes.SetInterferenceGeometry(aCPnt);
        myCheckResults.Append(aChRes);

        if(myStopOnFirst)
          throw BOPTColStd_Failure(buf) ;
      }
      
      if (aNbCurves) {
	for (j=1; j<=aNbCurves; j++) {
	  const IntTools_Curve& aC=aCvs(j);
	  if (aC.HasBounds()) {
	    Standard_Real aT1, aT2;
	    Standard_Boolean bValid;
	    gp_Pnt aP1, aP2;
	    
	    aC.Bounds(aT1, aT2, aP1, aP2);
	    //
	    bValid=myContext.IsValidBlockForFaces(aT1, aT2, aC, aF1, aF2, 1.e-3);
	    //
	    if (bValid) {
	      char buf[512];
	      sprintf (buf, "FF: (%d, %d) ", nF1, nF2);
	      //
	      Handle (Geom_Curve) aC3D=aC.Curve();
	      Handle (Geom_TrimmedCurve) aTC3D=Handle (Geom_TrimmedCurve)::DownCast(aC3D);
// 	      myInerference=aTC3D;

              BOPTools_CheckResult aChRes;
              aChRes.AddShape(aF1);
              aChRes.AddShape(aF2);
              aChRes.SetCheckStatus(BOPTools_FACEFACE);
// modified by NIZHNY-MKK  Fri Sep  3 16:02:40 2004
//               aChRes.SetInterferenceGeometry(myInerference);
              aChRes.SetInterferenceGeometry(aTC3D);
              myCheckResults.Append(aChRes);

              if(myStopOnFirst)
                throw BOPTColStd_Failure(buf) ;
	    }
	  }
	}
      }// if (aNbCurves)
      
    }// if (aFF.IsDone())
  }// for (; myDSIt.More(); myDSIt.Next()) 
  myIsDone=Standard_True;
}
//=======================================================================
// function: PrepareEdges
// purpose: 
//=======================================================================
  void BOPTools_Checker::PrepareEdges()
{
  Standard_Integer  i, nV, ii, aNBSuc;
  Standard_Real aT;
  TopAbs_Orientation anOr;
  TopoDS_Edge   aE;
  TopoDS_Vertex aV;

  for (i=1; i<=myNbSources; i++) {
    if (myDS->GetShapeType(i)==TopAbs_EDGE) {
      aE=TopoDS::Edge(myDS->GetShape(i));
      //
      if (BRep_Tool::Degenerated(aE)){
	continue;
      }
      //
      BOPTools_PaveSet& aPaveSet= myPavePool(myDS->RefEdge(i));
      //
      //                                                   cto900/M2
      // Some of Edges can be [Semi] Infinite.  Such  Edges have no 
      // vertices on correspondant INF ends.   So we  must  provide 
      // these vertices formally (to obtain  Shrunk  Ranges for e.g). 
      // In reality this vertex(-es) does not belong to the INF Edge.
      // It just has reference in the DS.
      //                            PKV Tue Apr 23 10:21:45 2002                 
      {
	Standard_Real aT1, aT2, aTolE;
	Standard_Boolean bInf1, bInf2;
	gp_Pnt aPx;
	TopoDS_Vertex aVx; 
	BRep_Builder aBB;
	BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq; 
	//
	aTolE=BRep_Tool::Tolerance(aE);
	Handle(Geom_Curve) aC3D=BRep_Tool::Curve (aE, aT1, aT2);
	bInf1=Precision::IsNegativeInfinite(aT1);
	bInf2=Precision::IsPositiveInfinite(aT2);

	if (bInf1) {
	  aC3D->D0(aT1, aPx);
	  aBB.MakeVertex(aVx, aPx, aTolE);
	  myDS->InsertShapeAndAncestorsSuccessors(aVx, anASSeq);
	  nV=myDS->NumberOfInsertedShapes();
	  BOPTools_Pave aPave(nV, aT1); 
	  aPaveSet.Append (aPave);
	}

	if (bInf2) {
	  aC3D->D0(aT2, aPx);
	  aBB.MakeVertex(aVx, aPx, aTolE);
	  myDS->InsertShapeAndAncestorsSuccessors(aVx, anASSeq);
	  nV=myDS->NumberOfInsertedShapes();
	  BOPTools_Pave aPave(nV, aT2);
	  aPaveSet.Append (aPave); 
	}
      }
      //
      aNBSuc=myDS->NumberOfSuccessors(i);
      for (ii=1; ii <= aNBSuc; ii++) {
	nV=myDS->GetSuccessor(i, ii);
	anOr=myDS->GetOrientation(i, ii);

	aV=TopoDS::Vertex(myDS->GetShape(nV));
	aV.Orientation(anOr);
	aT=BRep_Tool::Parameter(aV, aE);
	//
	BOPTools_Pave aPave(nV, aT); 
	aPaveSet.Append (aPave);
      }
    }
  }
}
//=======================================================================
// function: PreparePaveBlocks
// purpose: 
//=======================================================================
  void BOPTools_Checker::PreparePaveBlocks(const TopAbs_ShapeEnum aType1, 
					   const TopAbs_ShapeEnum aType2)
{
  BOPTools_PaveFiller::PreparePaveBlocks(aType1, aType2);
}
//=======================================================================
// function: PreparePaveBlocks
// purpose: 
//=======================================================================
  void BOPTools_Checker::PreparePaveBlocks(const Standard_Integer nE)
{
  myIsDone=Standard_False;
  
  Standard_Integer nV1, nV2;

  TopoDS_Edge aE;
  TopoDS_Vertex aV1, aV2;
    
  // SplitShapesPool
  BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE));
  // Edge 
  aE=TopoDS::Edge(myDS->GetShape(nE));
  //
  if (!BRep_Tool::Degenerated(aE)){
    //
    BOPTools_PaveSet& aPS=myPavePool(myDS->RefEdge(nE));
    
    BOPTools_PaveBlockIterator aPBIt(nE, aPS);
    for (; aPBIt.More(); aPBIt.Next()) {
      BOPTools_PaveBlock& aPB=aPBIt.Value();
      
      const IntTools_Range& aRange=aPB.Range();
      
      const BOPTools_Pave& aPave1=aPB.Pave1();
      nV1=aPave1.Index();
      aV1=TopoDS::Vertex(myDS->GetShape(nV1));
      
      const BOPTools_Pave& aPave2=aPB.Pave2();
      nV2=aPave2.Index();
      aV2=TopoDS::Vertex(myDS->GetShape(nV2));
      //
      // ShrunkRange
      IntTools_ShrunkRange aSR (aE, aV1, aV2, aRange, myContext);
      //
      Standard_Integer anErrorStatus;
      anErrorStatus=aSR.ErrorStatus();

      char buf[512];
      if (!aSR.IsDone()) {
	sprintf (buf, "Can not obtain ShrunkRange for Edge %d", nE);
        
        BOPTools_CheckResult aChRes;
        aChRes.AddShape(aE);
        aChRes.SetCheckStatus(BOPTools_BADSHRANKRANGE);
        myCheckResults.Append(aChRes);

        if(myStopOnFirst)
          throw BOPTColStd_Failure(buf) ;
      }
      //
      if (anErrorStatus==6) {
	sprintf(buf,
		"Warning: [PreparePaveBlocks()] Max.Dummy Shrunk Range for Edge %d\n", nE);

        BOPTools_CheckResult aChRes;
        aChRes.AddShape(aE);
        aChRes.SetCheckStatus(BOPTools_NULLSRANKRANGE);
        myCheckResults.Append(aChRes);

        if(myStopOnFirst)
          throw BOPTColStd_Failure(buf);
      }
      else {
	// Check left paves and correct ShrunkRange if it is necessary
	CorrectShrunkRanges (0, aPave1, aSR);
	CorrectShrunkRanges (1, aPave2, aSR);
      }
      //
      aPB.SetShrunkRange(aSR);
      aLPB.Append(aPB);
    } //for (; aPBIt1.More(); aPBIt1.Next()) 
  }
  myIsDone=Standard_True;
}

//=======================================================================
// function: GetCheckResult
// purpose: 
//=======================================================================
const BOPTools_ListOfCheckResults& BOPTools_Checker::GetCheckResult() const
{
  return myCheckResults;
}

//=======================================================================
// function: HasFaulty
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_Checker::HasFaulty()const 
{
  return (!myIsDone || !myCheckResults.IsEmpty());
}

//=======================================================================
// function: Shape
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOPTools_Checker::Shape()const 
{
  return myShape;
}



