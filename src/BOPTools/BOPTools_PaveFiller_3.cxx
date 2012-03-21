// Created on: 2001-03-13
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOPTools_PaveFiller.ixx>

#include <Geom_Curve.hxx>

#include <gp_Pnt.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_OnceExplorer.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>

#include <IntTools_FaceFace.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_SequenceOfPntOn2Faces.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_PntOnFace.hxx>

#include <BOPTools_IteratorOfCoupleOfShape.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_Pave.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_ListOfPave.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>

#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_ListIteratorOfListOfInterference.hxx>
#include <BOPTools_InterferenceLine.hxx>
#include <BOPTools_ListOfInterference.hxx>
#include <BOPTools_Interference.hxx>
#include <BOPTools_CArray1OfInterferenceLine.hxx>

#include <Precision.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <TopExp_Explorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <BOPTools_ESInterference.hxx>
#include <BOPTools_VSInterference.hxx>
#include <BOPTools_VEInterference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BOPTools_EEInterference.hxx>
#include <IntTools_CommonPrt.hxx>
#include <BOPTools_ESInterference.hxx>
#include <BOPTools_ShapeShapeInterference.hxx>

#include <IntSurf_ListOfPntOn2S.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <TopTools_MapOfShape.hxx>


static
  Standard_Boolean IsPaveBlock(const Standard_Integer nV1,
			       const Standard_Integer nV2, 
			       const BOPTools_ListOfPaveBlock& aLPBExisting);
static
  Standard_Boolean IsFound(const TColStd_IndexedMapOfInteger& aMapWhat,
			   const TColStd_IndexedMapOfInteger& aMapWith);
static
  void FMapWith(const Standard_Integer nF,
		BOPTools_InterferencePool* myIntrPool,
		TColStd_IndexedMapOfInteger& aMapWith);
static
  void FMapWhat(const Standard_Integer nF,
		BOPTools_InterferencePool* myIntrPool,
		TColStd_IndexedMapOfInteger& aMapWhat);

//wkar OCC334 f
static
  void UnUsedMap(BOPTools_SequenceOfCurves& aSCvs,
		 const BOPTools_PaveSet& aPSF,
		 TColStd_IndexedMapOfInteger& aMapUnUsed);
static
  Standard_Boolean VertexRangeTolerance(const Standard_Integer nV,
					const Standard_Integer nF1,
					const Standard_Integer nF2,
					const BOPTools_InterferencePool& anIntrPool,
					Standard_Real& aTolV);
static 
  void FaceAndEdgeMap(const Standard_Integer nF,
		      const BOPTools_InterferencePool& anIntrPool,
		      TColStd_IndexedMapOfInteger& aMEF);
static
  void ProcessAloneStickVertices(const Standard_Integer nF1,
				 const Standard_Integer nF2,
				 const BOPTools_PaveSet& aPSF,
				 BOPTools_SequenceOfCurves& aSCvs,
				 const BOPTools_InterferencePool& anIntrPool,
				 BOPTools_PaveFiller& aPF,
				 TColStd_SequenceOfInteger& aSeqVx,
				 TColStd_SequenceOfReal& aSeqTolVx);
static
  void ProcessAloneStickVertices(const Standard_Integer nF1,
				 const Standard_Integer nF2,
				 const BOPTools_PaveSet& aPSF,
				 BOPTools_SequenceOfCurves& aSCvs,
				 BOPTools_PaveFiller& aPF,
				 TColStd_SequenceOfInteger& aSeqVx,
				 TColStd_SequenceOfReal& aSeqTolVx);
//wkar OCC334 t

static
  Standard_Boolean IsPairFound(const Standard_Integer nF1,
			       const Standard_Integer nF2,
			       BOPTools_InterferencePool* myIntrPool,
			       BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWhat,
			       BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWith);

static Standard_Boolean CheckNewVertexAndUpdateData(const TopoDS_Vertex&              theVertex,
						    const Standard_Real               theParamOnE,
						    const TopoDS_Edge&                theEdge,
						    const Standard_Real               theParamOnCurve,
						    const Standard_Integer            theIndexF1,
						    const Standard_Integer            theIndexF2,
						    const Standard_Real               theTolerance,
						    const BOPTools_PInterferencePool& theIntrPool,
						    const BooleanOperations_PShapesDataStructure& theDS,
						    const Handle(IntTools_Context)&   theContext,
						    const BOPTools_PaveSet&           theEdgePaveSet,
						    const Standard_Boolean            bAddNewVertex,
						    const Standard_Boolean            bAddOldVertex,
						    BOPTools_Curve&                   theBC,
						    BOPTools_Pave&                    thePaveToPut,
						    Standard_Boolean&                 bAddNewVertexOut,
						    Standard_Boolean&                 bAddOldVertexOut);

static void AddInterfForAdjacentFace(const Standard_Integer            theEdgeIndex,
				     const Standard_Integer            theIndexF1,
				     const Standard_Integer            theIndexF2,
				     BOPTools_PInterferencePool        theIntrPool,
				     const BooleanOperations_PShapesDataStructure& theDS);

static Standard_Boolean RejectPaveBlock(const IntTools_Curve& theC,
                                        const Standard_Real   theT1,
                                        const Standard_Real   theT2,
                                        const TopoDS_Vertex&  theV,
                                        Standard_Real&        theRT);

static Standard_Boolean ModifFFTol(const TopoDS_Face& theF1,
                                   const TopoDS_Face& theF2,
                                   Standard_Real&     theTF);

static Standard_Integer RejectBuildingEdge(const IntTools_Curve& theC,
                                           const TopoDS_Vertex&  theV1,
                                           const TopoDS_Vertex&  theV2,
                                           const Standard_Real   theT1,
                                           const Standard_Real   theT2,
                                           const TopTools_ListOfShape& theL,
                                           Standard_Real&        theTF);

static
  void CorrectTolR3D(BOPTools_PaveFiller& aPF,
		     const BOPTools_SSInterference& aFF,
		     const TColStd_MapOfInteger& aMVStick,
		     Standard_Real& aTolR3D);

//=======================================================================
// function: PerformFF
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PerformFF() 
{
  myIsDone=Standard_False;
  Standard_Boolean bIsFound, bToSplit;
  Standard_Integer n1, n2, anIndexIn=0, nF1, nF2, aNbFFs, aBlockLength;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aMapWhat, aMapWith;
  //
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  //
  //  F/F Interferences  [BooleanOperations_SurfaceSurface]
  myDSIt.Initialize(TopAbs_FACE, TopAbs_FACE);
  //
  // BlockLength correction
  aNbFFs=ExpectedPoolLength();
  aBlockLength=aFFs.BlockLength();
  if (aNbFFs > aBlockLength) {
    aFFs.SetBlockLength(aNbFFs);
  }
  // 
  bToSplit=Standard_False;

  for (; myDSIt.More(); myDSIt.Next()) {
    Standard_Boolean justaddinterference = Standard_True;
    myDSIt.Current(n1, n2, justaddinterference);

    if(justaddinterference) {
      if (!myIntrPool->IsComputed(n1, n2)) {

	if(n1 < n2) {
	  nF1 = n1;
	  nF2 = n2;
	}
	else {
	  nF1 = n2;
	  nF2 = n1;
	}
	//
	bIsFound=IsPairFound(nF1, nF2, myIntrPool, aMapWhat, aMapWith);
	//
	if (!bIsFound) {
	  myIntrPool->AddInterference (nF1, nF2, BooleanOperations_SurfaceSurface, anIndexIn); 
	}
	else {
	  IntTools_SequenceOfPntOn2Faces aPnts;
	  IntTools_SequenceOfCurves aCvs;

	  BOPTools_SSInterference anInterf (nF1, nF2, 1.e-07, 1.e-07, aCvs, aPnts);
	  anIndexIn=aFFs.Append(anInterf);
	  myIntrPool->AddInterference (nF1, nF2, BooleanOperations_SurfaceSurface, anIndexIn); 
	}
      }
      continue;
    }
    //
    if (myIntrPool->IsComputed(n1, n2)) {
      continue;
    }
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
    TopoDS_Face aF1=TopoDS::Face(myDS->GetShape(nF1));
    TopoDS_Face aF2=TopoDS::Face(myDS->GetShape(nF2));
    //
    IntSurf_ListOfPntOn2S aListOfPnts;
    GeomAPI_ProjectPointOnSurf& aProj1 = myContext->ProjPS(aF1);
    GeomAPI_ProjectPointOnSurf& aProj2 = myContext->ProjPS(aF2);

    BOPTools_CArray1OfESInterference& aEFs=myIntrPool->ESInterferences();
    TColStd_MapOfInteger aMapEdgeIndex1, aMapEdgeIndex2;
    for(Standard_Integer fIt = 0; fIt < 2; fIt++) {
      Standard_Integer nF = (fIt == 0) ? nF1 : nF2;
      for(Standard_Integer sIt1 = 1; sIt1 <= myDS->NumberOfSuccessors(nF); sIt1++) {
	Standard_Integer nIndexS1 = myDS->GetSuccessor(nF, sIt1);
	if(myDS->GetShapeType(nIndexS1) == TopAbs_EDGE) {
	  if(fIt == 0)
	    aMapEdgeIndex1.Add(nIndexS1);
	  else
	    aMapEdgeIndex2.Add(nIndexS1);
	}
	else {
	  for(Standard_Integer sIt2 = 1; sIt2 <= myDS->NumberOfSuccessors(nIndexS1); sIt2++) {
	    Standard_Integer nIndexS2 = myDS->GetSuccessor(nIndexS1, sIt2);
	    
	    if(myDS->GetShapeType(nIndexS2) == TopAbs_EDGE) {
	      if(fIt == 0)
		aMapEdgeIndex1.Add(nIndexS2);
	      else
		aMapEdgeIndex2.Add(nIndexS2);
	    }
	  }
	}
      }
      
      TColStd_MapIteratorOfMapOfInteger anIt;
      if(fIt == 0)
	anIt.Initialize(aMapEdgeIndex1);
      else
	anIt.Initialize(aMapEdgeIndex2);

      Standard_Integer nFOpposite = (fIt == 0) ? nF2 : nF1;      

      for(; anIt.More(); anIt.Next()) {
	Standard_Integer nIndexE = anIt.Key();

	for(Standard_Integer interIt = 1; interIt <= aEFs.Length(); interIt++) {
	  const BOPTools_ESInterference& aEF = aEFs(interIt);          

	  if((aEF.Index1() == nIndexE) && (nFOpposite == aEF.Index2())) {
	    IntTools_CommonPrt aCP = aEF.CommonPrt();

	    if(aCP.Type() == TopAbs_VERTEX) {
	      Standard_Real aPar = aCP.VertexParameter1();
	      // compute points and add to the list
	      Standard_Real f,l;
	      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(myDS->GetShape(nIndexE)), f,l);
	      Handle(Geom2d_Curve) aPCurve;
	      if(fIt == 0) {
		aPCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(myDS->GetShape(nIndexE)),
						    aF1, f, l);
	      }
	      else {
		aPCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(myDS->GetShape(nIndexE)),
						    aF2, f, l);
	      }		
								       
	      gp_Pnt aPoint;
	      aCurve->D0(aPar, aPoint);
	      Standard_Real U1,V1,U2,V2;
	      IntSurf_PntOn2S aPnt;
	      if(!aPCurve.IsNull()) {
		gp_Pnt2d aP2d = aPCurve->Value(aPar);
		if(fIt == 0) {
		  aProj2.Perform(aPoint);
		  if(aProj2.IsDone()) {
		    aProj2.LowerDistanceParameters(U2,V2);
		    aPnt.SetValue(aP2d.X(),aP2d.Y(),U2,V2);
		    aListOfPnts.Append(aPnt); 
		  }
		}
		else {
		  aProj1.Perform(aPoint);
		  if(aProj1.IsDone()) {
		    aProj1.LowerDistanceParameters(U1,V1);
		    aPnt.SetValue(U1,V1,aP2d.X(),aP2d.Y());
		    aListOfPnts.Append(aPnt); 
		  }
		}
	      }              
	      else {
		aProj1.Perform(aPoint);
		aProj2.Perform(aPoint);
		if(aProj1.IsDone() && aProj2.IsDone()){
		  aProj1.LowerDistanceParameters(U1,V1);
		  aProj2.LowerDistanceParameters(U2,V2);
		  aPnt.SetValue(U1,V1,U2,V2);
		  aListOfPnts.Append(aPnt);                
		}
	      }
	    }
	  }
	}
      }
    }
    //
    // FF
    Standard_Boolean bToApproxC3d, bToApproxC2dOnS1, bToApproxC2dOnS2, bIsDone;
    Standard_Real anApproxTol, aTolR3D, aTolR2D;
    //
    bToApproxC3d     = mySectionAttribute.Approximation();
    bToApproxC2dOnS1 = mySectionAttribute.PCurveOnS1();
    bToApproxC2dOnS2 = mySectionAttribute.PCurveOnS2();
    //
    anApproxTol=1.e-7;

    IntTools_FaceFace aFF;
    //
    aFF.SetContext(myContext);
    //
    aFF.SetParameters (bToApproxC3d, 
		       bToApproxC2dOnS1, 
		       bToApproxC2dOnS2,
		       anApproxTol);
    //
    if (!aListOfPnts.IsEmpty()) {
      aFF.SetList(aListOfPnts);
    }
    //
    aFF.Perform(aF1, aF2);
    //
    bIsDone=aFF.IsDone();
    if (bIsDone) {
      // Add Interference to the Pool
      aTolR3D=aFF.TolReached3d();
      aTolR2D=aFF.TolReached2d();
      if (aTolR3D < 1.e-7){
	aTolR3D=1.e-7;
      } 
      aFF.PrepareLines3D(bToSplit);
      //
      anIndexIn=0;
      Standard_Integer aNbCurves, aNbPoints;

      const IntTools_SequenceOfCurves& aCvs=aFF.Lines();
      aNbCurves=aCvs.Length();
      //
      const IntTools_SequenceOfPntOn2Faces& aPnts=aFF.Points();
      aNbPoints=aPnts.Length();
      
      if (!aNbCurves && !aNbPoints) {
	//
	bIsFound=IsPairFound(nF1, nF2, myIntrPool, aMapWhat, aMapWith);
	//
	if (!bIsFound) {
	  myIntrPool->AddInterference (nF1, nF2, BooleanOperations_SurfaceSurface, anIndexIn); 
	  continue;
	}
      }
      //
      BOPTools_SSInterference anInterf (nF1, nF2, aTolR3D, aTolR2D, aCvs, aPnts);
      anIndexIn=aFFs.Append(anInterf);
      
      myIntrPool->AddInterference (nF1, nF2, BooleanOperations_SurfaceSurface, anIndexIn);
    } //if (bIsDone)
    //
    else {
      anIndexIn=0;
      //
      bIsFound=IsPairFound(nF1, nF2, myIntrPool, aMapWhat, aMapWith);
      //
      if (!bIsFound) {
	myIntrPool->AddInterference (nF1, nF2, BooleanOperations_SurfaceSurface, anIndexIn); 
      }
      else {
	IntTools_SequenceOfPntOn2Faces aPnts;
	IntTools_SequenceOfCurves aCvs;
	
	BOPTools_SSInterference anInterf (nF1, nF2, 1.e-07, 1.e-07, aCvs, aPnts);
	anIndexIn=aFFs.Append(anInterf);
	myIntrPool->AddInterference (nF1, nF2, BooleanOperations_SurfaceSurface, anIndexIn);
      }
    }
  }// for (; myDSIt.More(); myDSIt.Next()) 
  myIsDone=Standard_True;
}
//=======================================================================
// function: MakeBlocks
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::MakeBlocks()
{
  Standard_Integer i, j, aNbCurves, aNbFFs, nF1, nF2, aBid=0, nV1, nV2;
  Standard_Real aTolR3D, aT1, aT2;
  Standard_Boolean bValid, bCoincide;

  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  
  aNbFFs=aFFs.Extent();

  for (i=1; i<=aNbFFs; i++) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    //  
    nF1=aFFi.Index1();
    nF2=aFFi.Index2();
    //
    // Curves' tolerance
    aTolR3D=aFFi.TolR3D();
    //
    // Faces
    const TopoDS_Face& aF1=TopoDS::Face(myDS->GetShape(nF1));
    const TopoDS_Face& aF2=TopoDS::Face(myDS->GetShape(nF2));

    TColStd_MapOfInteger aMap;
    BOPTools_ListOfPaveBlock aLPB;
    SplitsInFace (aBid, nF1, nF2, aLPB);
    SplitsInFace (aBid, nF2, nF1, aLPB);
    SplitsOnFace (aBid, nF1, nF2, aLPB);
    
    BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      aFFi.AppendBlock(aPB);
      nV1=aPB.Pave1().Index();
      nV2=aPB.Pave2().Index();
      aMap.Add(nV1);
      aMap.Add(nV2);
    }
    // Put existing paves on curves
    //     BOPTools_PaveSet aPSF;
    //     PrepareSetForFace (nF1, nF2, aPSF);

    BOPTools_SequenceOfCurves& aSCvs = aFFi.Curves();
    aNbCurves=aSCvs.Length();

    // 
    // Pave Blocks On Curves
    // 
    Standard_Boolean bIsPaveBlock;
    Standard_Integer iCheckIntermediatePoint;
    // 
    for (j=1; j<=aNbCurves; j++) {
      BOPTools_Curve& aBC=aSCvs(j);
      const IntTools_Curve& aC= aBC.Curve();
      
      BOPTools_PaveSet& aPaveSet=aBC.Set();
      BOPTools_PaveBlockIterator aPBIter(0, aPaveSet);
      for (; aPBIter.More(); aPBIter.Next()) {
	BOPTools_PaveBlock& aPBNew=aPBIter.Value();
	aPBNew.SetCurve(aC);
	aPBNew.SetFace1(nF1);
	aPBNew.SetFace2(nF2);
	//
	nV1=aPBNew.Pave1().Index();
	nV2=aPBNew.Pave2().Index();
	
	if (aMap.Contains(nV1) && aMap.Contains(nV2)) {
	  //
	  const BOPTools_ListOfPaveBlock& aLPBExisting=aFFi.PaveBlocks();
	  bIsPaveBlock=IsPaveBlock(nV1, nV2, aLPBExisting);
	  //
	  iCheckIntermediatePoint=1;
	  if (bIsPaveBlock) {
	    BOPTools_ListIteratorOfListOfPaveBlock anItLPB(aLPBExisting);
	    
	    for (; anItLPB.More(); anItLPB.Next()) {
	      const BOPTools_PaveBlock& aPBR=anItLPB.Value();
	      iCheckIntermediatePoint=
		CheckIntermediatePoint(aPBNew, aPBR, aTolR3D);
	      if (!iCheckIntermediatePoint) {
		break;
	      }
	    }
	    bIsPaveBlock=bIsPaveBlock && !iCheckIntermediatePoint;
	  }
	  //
	  if (bIsPaveBlock) {
	    continue;
	  }
	}
	//
	else {
	  iCheckIntermediatePoint=0;
	} 
	//
	aT1=aPBNew.Pave1().Param();
	aT2=aPBNew.Pave2().Param();
	// 
	if((nV1 == nV2) && (Abs(aT2 - aT1) < Precision::PConfusion())) {
	    continue;
	}
	// 
	// Checking of validity in 2D
	//
	Standard_Real aTolerance = (aTolR3D < 1.e-3) ? 1.e-3 : aTolR3D;
	bValid=myContext->IsValidBlockForFaces(aT1, aT2, aC, aF1, aF2, aTolerance);
	//
	if (!bValid) {
	  continue;
	}
	//
	// Checking the  paveblocks for coinsidence with aLPB
	bCoincide=CheckCoincidence (aPBNew, aFFi);
	//
	bCoincide=bCoincide && !iCheckIntermediatePoint;
	//
	if (bCoincide) {
	  continue;
	}
	//
        // reject pave block (FF) v1==v2 for too small sect. edge
        TopoDS_Vertex aV1=TopoDS::Vertex(myDS->GetShape(nV1));
        TopoDS_Vertex aV2=TopoDS::Vertex(myDS->GetShape(nV2));
        Standard_Boolean rejectPaveBlock = Standard_False;
        if(aV1.IsSame(aV2)) {
          Standard_Real aRT = 1.e-7;
          rejectPaveBlock = RejectPaveBlock(aC,aT1,aT2,aV1,aRT);
          if(rejectPaveBlock) {
            if(aRT > 1.e-7) {
              BRep_Builder BB;
              BB.UpdateVertex( aV1, 2*aRT );
            }
            continue;
          }
        }
        //
	aBC.AppendNewBlock(aPBNew);
      }
    } // end of for (j=1; j<=aNbCurves; j++)
  }// end of for (i=1; i<=aNbFFs; i++)
}
//=======================================================================
// function: MakeAloneVertices
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::MakeAloneVertices()
{
  Standard_Integer i, j, k, aNbFFs, aNbCurves, nF1, nF2, nV, aNbAlone, aNbV;
  Standard_Boolean bFlag=Standard_False;

  BOPTools_ListIteratorOfListOfPave anIt;
  TColStd_IndexedMapOfInteger aMap;
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; i++) {
    BOPTools_SSInterference& aFFi=aFFs(i);

    const IntTools_SequenceOfPntOn2Faces& aSeqAlonePnts=aFFi.AlonePnts();
    aNbAlone=aSeqAlonePnts.Length();
    
    if (!aNbAlone) {
      continue;
    }

    nF1=aFFi.Index1();
    nF2=aFFi.Index2();
    //
    TopoDS_Face aF1=TopoDS::Face(myDS->Shape(nF1));
    TopoDS_Face aF2=TopoDS::Face(myDS->Shape(nF2));
    // 
    // 1. fill aMap where all indices for (F/F) vertices are
    aMap.Clear();
    
    BOPTools_PaveSet aPSF;
    
    PrepareSetForFace (nF1, nF2, aPSF);
    const BOPTools_ListOfPave& aLPaves=aPSF.Set();
    anIt.Initialize(aLPaves);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_Pave& aPave=anIt.Value();
      nV=aPave.Index();
      aMap.Add(nV);
    }

    BOPTools_SequenceOfCurves& aSCvs=aFFi.Curves();
    aNbCurves=aSCvs.Length();
    for (j=1; j<=aNbCurves; j++) {
      BOPTools_Curve& aBC=aSCvs(j);
      const BOPTools_PaveSet& aCPSF=aBC.Set();
      const BOPTools_ListOfPave& aLPs=aCPSF.Set();
      anIt.Initialize(aLPs);
      for (; anIt.More(); anIt.Next()) {
	const BOPTools_Pave& aPv=anIt.Value();
	nV=aPv.Index();
	aMap.Add(nV);
      }
    }
    //
    // 2. check alone points on closure with aMap's vertices
    Standard_Integer iVV;
    Standard_Real aTolVAlone, aTolF1, aTolF2;
    TopoDS_Vertex aVAlone;
    BRep_Builder aBB;
    
    aTolF1=BRep_Tool::Tolerance(aF1);
    aTolF2=BRep_Tool::Tolerance(aF2);
    aTolVAlone=aTolF1+aTolF2;

    aNbV=aMap.Extent();
    for (j=1; j<=aNbAlone; ++j) {
      const IntTools_PntOn2Faces& aP2F=aSeqAlonePnts(j);
      const IntTools_PntOnFace& aPF1=aP2F.P1();
      const gp_Pnt& aPAlone=aPF1.Pnt();
      aBB.MakeVertex(aVAlone, aPAlone, aTolVAlone);
      //
      bFlag=Standard_True;
      //
      for (k=1; k<=aNbV; ++k) {
	nV=aMap(k);
	const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->Shape(nV));
	
	iVV= IntTools_Tools::ComputeVV (aVAlone, aV);
	if (!iVV) {
	  // It means that aVAlone and aV coinsided so 
	  // we do not need to insert aVAlone into the DS
	  bFlag=Standard_False;
	  break;
	}
	
      }
      if (bFlag) {
	Standard_Boolean bVF;
	Standard_Integer aNewShape;
	//
	bVF=myContext->IsValidPointForFaces (aPAlone, aF1, aF2, 1.e-3);
	//
	if (bVF) {
	  BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
	  myDS->InsertShapeAndAncestorsSuccessors(aVAlone, anASSeq);
	  aNewShape=myDS->NumberOfInsertedShapes();
	  myDS->SetState (aNewShape, BooleanOperations_ON);
	  //
	  TColStd_ListOfInteger& anAloneVertices=aFFi.AloneVertices();
	  anAloneVertices.Append(aNewShape);
	}
      }
    }
    
  }
}
//=======================================================================
// function: CheckCoincidence
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveFiller::CheckCoincidence(const BOPTools_PaveBlock& aPB,
							 const BOPTools_SSInterference& aFFi)
							 
{
  Standard_Real aTolC, aTE, aT11, aT12;
  Standard_Integer nV11, nV12, nV21, nV22, iVV, iVE, nE2, iCount=0, iCountExt=1;
  Standard_Integer iV;

  aTolC=aFFi.TolR3D();
  // V11
  const BOPTools_Pave& aPave11=aPB.Pave1();
  nV11=aPave11.Index();
  const TopoDS_Vertex& aV11=TopoDS::Vertex(myDS->GetShape(nV11));
  aT11=aPave11.Param();
  // V12
  const BOPTools_Pave& aPave12=aPB.Pave2();
  nV12=aPave12.Index();
  const TopoDS_Vertex& aV12=TopoDS::Vertex(myDS->GetShape(nV12));
  aT12=aPave12.Param();
  //
  const BOPTools_ListOfPaveBlock& aLPB=aFFi.PaveBlocks();
  BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
  for (; anIt.More(); anIt.Next()) {
    
    iCount=0;
    
    const BOPTools_PaveBlock& aPBR=anIt.Value();
    // V21
    const BOPTools_Pave& aPave21=aPBR.Pave1();
    nV21=aPave21.Index();
    const TopoDS_Vertex& aV21=TopoDS::Vertex(myDS->GetShape(nV21));
    // V22
    const BOPTools_Pave& aPave22=aPBR.Pave2();
    nV22=aPave22.Index();
    const TopoDS_Vertex& aV22=TopoDS::Vertex(myDS->GetShape(nV22));
    // E2
    nE2=aPBR.Edge();
    const TopoDS_Edge& aE2=TopoDS::Edge(myDS->GetShape(nE2));
    //
    // VV
    iV=0;
    iVV=IntTools_Tools::ComputeVV (aV11, aV21);
    if (!iVV) {
      iCount++;
      iV++;
      if (iCount>iCountExt) {
	break;
      }
    }
    
    iVV=IntTools_Tools::ComputeVV (aV11, aV22);
    if (!iVV) {
      iCount++;
      iV++;
      if (iCount>iCountExt) {
	break;
      }
    }

    // VE
    if (!iV) {
      iVE=myContext->ComputeVE (aV11, aE2, aTE);
      if (!iVE) {
	iCount++;
	if (iCount>iCountExt) {
	  break;
	}
      }
    }
    //
    // VV
    iV=0;
    iVV=IntTools_Tools::ComputeVV (aV12, aV21);
    if (!iVV) {
      iCount++;
      iV++;
      if (iCount>iCountExt) {
	break;
      }
    }

    iVV=IntTools_Tools::ComputeVV (aV12, aV22);
    if (!iVV) {
      iCount++;
      iV++;
      if (iCount>iCountExt) {
	break;
      }
    }
    // VE
    if (!iV) {
      //
      iVE=myContext->ComputeVE (aV12, aE2, aTE);
      //
      if (!iVE) {
	iCount++;
	if (iCount>iCountExt) {
	  break;
	}
      }
    }
    
  } // next aPBR
  return (iCount>iCountExt);
}
//=======================================================================
// function: CheckIntermediatePoint
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_PaveFiller::CheckIntermediatePoint(const BOPTools_PaveBlock& aPB,
							       const BOPTools_PaveBlock& aPBR,
							       const Standard_Real aTolC)
							 
{
  Standard_Real aT11, aT12, aTM, aTmp;
  Standard_Integer iVM, nE2;

  gp_Pnt aPM;
  BRep_Builder aBB;
  TopoDS_Vertex aVM;
  // 
  // Vertex
  const BOPTools_Pave& aPave11=aPB.Pave1();
  aT11=aPave11.Param();
  // 
  const BOPTools_Pave& aPave12=aPB.Pave2();
  aT12=aPave12.Param();
  //
  aTM=IntTools_Tools::IntermediatePoint (aT11, aT12);
  //
  const IntTools_Curve& aIC=aPB.Curve();
  aIC.D0(aTM, aPM);
  //
  aBB.MakeVertex (aVM, aPM, aTolC);
  //
  //Edge
  nE2=aPBR.Edge();
  const TopoDS_Edge& aE2=TopoDS::Edge(myDS->GetShape(nE2));
  // VE
  iVM=myContext->ComputeVE(aVM, aE2, aTmp); 
  //
  return iVM;
}
//=======================================================================
// function: PutBoundPaveOnCurve
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutBoundPaveOnCurve(BOPTools_Curve& aBC,
						BOPTools_SSInterference& aFFi)
{ 
  Standard_Boolean bHasBounds, bVF;
  

  const IntTools_Curve& aIC=aBC.Curve();
  bHasBounds=aIC.HasBounds ();
  
  if (!bHasBounds){
    return;
  }

  Standard_Integer nF1, nF2;
  Standard_Real aT1, aT2, aTolR3D;
  gp_Pnt aP1, aP2;
  //
  // Bounds
  aIC.Bounds (aT1, aT2, aP1, aP2);
  //
  // Faces
  nF1=aFFi.Index1();
  nF2=aFFi.Index2();
  //
  aTolR3D=aFFi.TolR3D();
  //
  TopoDS_Face aF1=TopoDS::Face(myDS->GetShape(nF1));
  TopoDS_Face aF2=TopoDS::Face(myDS->GetShape(nF2));
  //
  bVF=myContext->IsValidPointForFaces (aP1, aF1, aF2, aTolR3D);
  //
  if (bVF) {
    PutBoundPaveOnCurve (aP1, aT1, aBC, aFFi);
  }
  //
  bVF=myContext->IsValidPointForFaces (aP2, aF1, aF2, aTolR3D);
  //
  if (bVF) {
    PutBoundPaveOnCurve (aP2, aT2, aBC, aFFi);
  }
}
//=======================================================================
// function: PutBoundPaveOnCurve
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutBoundPaveOnCurve(const gp_Pnt& aP,
						const Standard_Real aT,
						BOPTools_Curve& aBC,
						BOPTools_SSInterference& aFFi)
{ 
  Standard_Boolean bFound1, bFound2;
  Standard_Integer nV;
  Standard_Real aTolV=aFFi.TolR3D();

  BOPTools_Pave aPave1, aPave2, aPave;
  BOPTools_PaveSet& aCPS=aBC.Set();
  BOPTools_PaveSet& aFFiPS=aFFi.NewPaveSet();
  const IntTools_Curve& aIC=aBC.Curve();

  bFound1=FindPave(aP, aTolV, aCPS  , aPave1);
  bFound2=FindPave(aP, aTolV, aFFiPS, aPave2);
  
  if (!bFound1 && !bFound2) {
    TopoDS_Vertex aNewVertex;
    BOPTools_Tools::MakeNewVertex(aP, aTolV, aNewVertex);
    //
    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
    myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
    nV=myDS->NumberOfInsertedShapes();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);

    aCPS.Append(aPave);
    aFFiPS.Append(aPave);
    //
    // Append Techno Vertex to the Curve
    TColStd_ListOfInteger& aTVs=aBC.TechnoVertices();
    aTVs.Append(nV);
  }
  
  if (bFound1 && !bFound2) {
    nV=aPave1.Index();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);
    aFFiPS.Append(aPave);
    //
    const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->Shape(nV));
    BOPTools_Tools::UpdateVertex (aIC, aT, aV);
  }
  
  if (!bFound1 && bFound2) {
    nV=aPave2.Index();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);
    aCPS.Append(aPave);
    //
    const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->Shape(nV));
    BOPTools_Tools::UpdateVertex (aIC, aT, aV);
  }
}

//=======================================================================
// function: PutBoundPaveOnCurveSpec
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutBoundPaveOnCurveSpec(BOPTools_Curve& aBC,
						    BOPTools_SSInterference& aFFi)
{ 
  Standard_Boolean bHasBounds, bVF;
  

  const IntTools_Curve& aIC=aBC.Curve();
  bHasBounds=aIC.HasBounds ();
  
  if (!bHasBounds){
    return;
  }

  Standard_Integer nF1, nF2;
  Standard_Real aT1, aT2, aTolR3D;
  gp_Pnt aP1, aP2;
  //
  // Bounds
  aIC.Bounds (aT1, aT2, aP1, aP2);
  //
  // Faces
  nF1=aFFi.Index1();
  nF2=aFFi.Index2();
  //
  aTolR3D=aFFi.TolR3D();
  //
  TopoDS_Face aF1=TopoDS::Face(myDS->GetShape(nF1));
  TopoDS_Face aF2=TopoDS::Face(myDS->GetShape(nF2));
  //
  bVF=myContext->IsValidPointForFaces (aP1, aF1, aF2, aTolR3D);
  //
  if (bVF) {
    PutBoundPaveOnCurveSpec (aP1, aT1, aBC, aFFi);
  }
  //
  bVF=myContext->IsValidPointForFaces (aP2, aF1, aF2, aTolR3D);
  //
  if (bVF) {
    PutBoundPaveOnCurveSpec (aP2, aT2, aBC, aFFi);
  }
}
//=======================================================================
// function: PutBoundPaveOnCurveSpec
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutBoundPaveOnCurveSpec(const gp_Pnt& aP,
						    const Standard_Real aT,
						    BOPTools_Curve& aBC,
						    BOPTools_SSInterference& aFFi)
{ 
  Standard_Boolean bFound1, bFound2;
  Standard_Integer nV;
  Standard_Real aTolV=aFFi.TolR3D();

  BOPTools_Pave aPave1, aPave2, aPave;
  BOPTools_PaveSet& aCPS=aBC.Set();
  BOPTools_PaveSet& aFFiPS=aFFi.NewPaveSet();
  const IntTools_Curve& aIC=aBC.Curve();

  bFound1=FindPave(aP, aTolV, aCPS  , aPave1);
  bFound2=FindPave(aP, aTolV, aFFiPS, aPave2);
  
  if (!bFound1 && !bFound2) {
    TopoDS_Vertex aNewVertex;
    BOPTools_Tools::MakeNewVertex(aP, aTolV, aNewVertex);
    //
    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
    myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
    nV=myDS->NumberOfInsertedShapes();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);

    aCPS.Append(aPave);
    aFFiPS.Append(aPave);
    //
    // Append Techno Vertex to the Curve
    TColStd_ListOfInteger& aTVs=aBC.TechnoVertices();
    aTVs.Append(nV);
    //
    //To check, if face boundary must be split by new vertex
    TopTools_MapOfShape aMap;
    Standard_Real aPar;
    Standard_Integer anErrStat;
    Standard_Integer aWhat, aWith, anIndexIn;
    BOPTools_CArray1OfVEInterference& aVEs=myIntrPool->VEInterferences();
    // Faces
    Standard_Integer nF1=aFFi.Index1();
    const TopoDS_Shape aF1 = myDS->GetShape(nF1);
    //
    Standard_Integer nF2=aFFi.Index2();
    const TopoDS_Shape aF2 = myDS->GetShape(nF2);
    //
    //
    Standard_Integer aRank = myDS->Rank(nF1);
    TopExp_Explorer anExp(aF1, TopAbs_EDGE);
    for(; anExp.More(); anExp.Next()) {

      const TopoDS_Shape& anE = anExp.Current();
      if (BRep_Tool::Degenerated(TopoDS::Edge(anE))){
	continue;
      }

      if(!aMap.Add(anE)) continue;

      anErrStat = 
	myContext->ComputeVE(aNewVertex, TopoDS::Edge(anE), aPar);
      if(anErrStat) continue;
      //
      Standard_Real aT1, aT2;
      gp_Pnt aP1, aP2;
      aIC.Bounds(aT1, aT2, aP1, aP2);
      //Check if any other point on curve belongs edge
      aT1 = 0.5*(aT1+aT2);
      aIC.D0(aT1, aP1);
      TopoDS_Vertex aNewVertex1;
      BOPTools_Tools::MakeNewVertex(aP1, aTolV, aNewVertex1);
      anErrStat = 
	myContext->ComputeVE(aNewVertex1, TopoDS::Edge(anE), aT1);
      if(!anErrStat) continue; //curve and edge seem to be coincide
      
      aWhat = nV;
      aWith = myDS->ShapeIndex(anE, aRank);
      BOPTools_VEInterference anInterf (aWhat, aWith, aPar);
      anIndexIn=aVEs.Append(anInterf);
      //
      // Add Pave to the Edge's myPavePool
      BOPTools_Pave aPave3(aWhat, aPar, BooleanOperations_VertexEdge);
      aPave3.SetInterference(anIndexIn);
      BOPTools_PaveSet& aPaveSet= myPavePool(myDS->RefEdge(aWith));
      aPaveSet.Append(aPave3);

      //
      // State for the Vertex in DS;
      myDS->SetState (aWhat, BooleanOperations_ON);
      // Insert Vertex in Interference Object
      BOPTools_VEInterference& aVE=aVEs(anIndexIn);
      aVE.SetNewShape(aWhat);

      PreparePaveBlocks(aWith);
      RecomputeCommonBlocks(aWith);
    }
    //
    aRank = myDS->Rank(nF2);
    anExp.Init(aF2, TopAbs_EDGE);
    for(; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& anE = anExp.Current();
      if (BRep_Tool::Degenerated(TopoDS::Edge(anE))){
	continue;
      }

      if(!aMap.Add(anE)) continue;

      anErrStat = 
	myContext->ComputeVE(aNewVertex, TopoDS::Edge(anE), aPar);
      if(anErrStat) continue;
      //
      Standard_Real aT1, aT2;
      gp_Pnt aP1, aP2;
      aIC.Bounds(aT1, aT2, aP1, aP2);
      //Check if any other point on curve belongs edge
      aT1 = 0.5*(aT1+aT2);
      aIC.D0(aT1, aP1);
      TopoDS_Vertex aNewVertex1;
      BOPTools_Tools::MakeNewVertex(aP1, aTolV, aNewVertex1);
      anErrStat = 
	myContext->ComputeVE(aNewVertex1, TopoDS::Edge(anE), aT1);
      if(!anErrStat) continue; //curve and edge seem to be coincide
      
      aWhat = nV;
      aWith = myDS->ShapeIndex(anE, aRank);
      BOPTools_VEInterference anInterf (aWhat, aWith, aPar);
      anIndexIn=aVEs.Append(anInterf);
      //
      // Add Pave to the Edge's myPavePool
      BOPTools_Pave aPave3(aWhat, aPar, BooleanOperations_VertexEdge);
      aPave3.SetInterference(anIndexIn);
      BOPTools_PaveSet& aPaveSet= myPavePool(myDS->RefEdge(aWith));
      aPaveSet.Append(aPave3);

      //
      // State for the Vertex in DS;
      myDS->SetState (aWhat, BooleanOperations_ON);
      // Insert Vertex in Interference Object
      BOPTools_VEInterference& aVE=aVEs(anIndexIn);
      aVE.SetNewShape(aWhat);

      PreparePaveBlocks(aWith);
      RecomputeCommonBlocks(aWith);
    }
    //
  }
  
  if (bFound1 && !bFound2) {
    nV=aPave1.Index();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);
    aFFiPS.Append(aPave);
    //
    const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->Shape(nV));
    BOPTools_Tools::UpdateVertex (aIC, aT, aV);
  }
  
  if (!bFound1 && bFound2) {
    nV=aPave2.Index();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);
    aCPS.Append(aPave);
    //
    const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->Shape(nV));
    BOPTools_Tools::UpdateVertex (aIC, aT, aV);
  }
}
//=======================================================================
// function: FindPave
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveFiller::FindPave(const gp_Pnt& aP,
						 const Standard_Real aTolPV, 
						 const BOPTools_PaveSet& aPS,
						 BOPTools_Pave& aPave)
{
  Standard_Integer nV;
  Standard_Boolean bIsVertex=Standard_False;
 
  const BOPTools_ListOfPave& aLP=aPS.Set();
  BOPTools_ListIteratorOfListOfPave anIt(aLP);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Pave& aPC=anIt.Value();
    nV=aPC.Index();
    const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->GetShape(nV));
    bIsVertex=IntTools_Tools::IsVertex (aP, aTolPV, aV);
    if (bIsVertex) {
      aPave=aPC;
      return bIsVertex;
    }
  }
  return bIsVertex;
}
//=======================================================================
// function: MakeSectionEdges
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::MakeSectionEdges()
{
  Standard_Integer i, j, aNbCurves, aNbFFs, nF1, nF2, nV1, nV2, aNbPaveBlocks,
                   aNewShapeIndex ;
  Standard_Real    t1, t2;
  TopoDS_Edge aESect;
  TopoDS_Vertex aV1, aV2;
  BRep_Builder BB;

  Standard_Integer pbi = 0;


  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; i++) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    nF1=aFFi.Index1();
    nF2=aFFi.Index2();
    //
    Standard_Real aTF = 1.e-7;
    TopoDS_Face aF1=TopoDS::Face(myDS->GetShape(nF1));
    TopoDS_Face aF2=TopoDS::Face(myDS->GetShape(nF2));
    Standard_Boolean isModT = ModifFFTol(aF1,aF2,aTF);
    Standard_Real aTolFF = (isModT) ? Max(aTF,aFFi.TolR3D()) : aFFi.TolR3D();
    BOPTools_ListOfPaveBlock aFFPBL;
    TopTools_ListOfShape aFFSEL;
    //
    BOPTools_SequenceOfCurves& aSCvs=aFFi.Curves();
    aNbCurves=aSCvs.Length();
    for (j=1; j<=aNbCurves; j++) {
      BOPTools_Curve& aBC=aSCvs(j);
      const IntTools_Curve& aIC=aBC.Curve();
      //
      const BOPTools_ListOfPaveBlock& aSectEdges=aBC.NewPaveBlocks();
      aNbPaveBlocks=aSectEdges.Extent();
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);
      pbi = 0;
      for (; aPBIt.More(); aPBIt.Next()) {
        pbi++;
	BOPTools_PaveBlock& aPB=aPBIt.Value();
	//
	// Pave1
	const BOPTools_Pave& aPave1=aPB.Pave1(); 
	nV1=aPave1.Index();
	t1=aPave1.Param();
	aV1=TopoDS::Vertex(myDS->GetShape(nV1));
	aV1.Orientation(TopAbs_FORWARD);
	//
	// Pave2
	const BOPTools_Pave& aPave2=aPB.Pave2();
	nV2=aPave2.Index();
	t2=aPave2.Param();
	aV2=TopoDS::Vertex(myDS->GetShape(nV2));
	aV2.Orientation(TopAbs_REVERSED);
	//
	// MakeSplitEdge
        //
        // reject building parallel sect. edge on the same pave block,
        //          if distance between created and this edges is too small
        if(IsPaveBlock(nV1,nV2,aFFPBL)) {
          Standard_Real diffTol = 1.e-7;
          Standard_Integer eI = RejectBuildingEdge(aIC,aV1,aV2,t1,t2,aFFSEL,diffTol);
          if(eI != 0) {
            Standard_Integer eIndex = 0;
            TopTools_ListIteratorOfListOfShape aSEIt(aFFSEL);
            for(; aSEIt.More(); aSEIt.Next()) {
              eIndex++;
              if(eIndex == eI) {
                const TopoDS_Edge & aE = TopoDS::Edge(aSEIt.Value());
                TopoDS_Edge& anEdge = (TopoDS_Edge &) aE;
                BOPTools_ListOfPaveBlock& aListPB = (BOPTools_ListOfPaveBlock&) aSectEdges;
                aListPB.Remove(aPBIt);
                BB.UpdateEdge( anEdge, 2*(aTolFF+diffTol) );
                BB.UpdateVertex( aV1, 2*(aTolFF+diffTol) );
                BB.UpdateVertex( aV2, 2*(aTolFF+diffTol) );
                break;
              }
            }
            if(aPBIt.More())
              continue;
            else
              break;
          }
        }
       
	if(fabs(t1-t2) <= 1.e-10) continue;
	BOPTools_Tools::MakeSectEdge (aIC, aV1, t1, aV2, t2, aESect);
	//
        BB.UpdateEdge( aESect, aTolFF );
	BB.UpdateVertex( aV1, aTolFF );
	BB.UpdateVertex( aV2, aTolFF );
	///////////////////////////////////
	// Add Sect Part to the DS
	BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
	
	anASSeq.SetNewSuccessor(nV1);
	anASSeq.SetNewOrientation(aV1.Orientation());
	
	anASSeq.SetNewSuccessor(nV2);
	anASSeq.SetNewOrientation(aV2.Orientation());
	//
	myDS->InsertShapeAndAncestorsSuccessors(aESect, anASSeq);
	aNewShapeIndex=myDS->NumberOfInsertedShapes();
	//
	aPB.SetEdge(aNewShapeIndex);
	//
        aFFSEL.Append(aESect);
        aFFPBL.Append(aPB);
      }
    }
  }
  
}
//=======================================================================
// function: PutPaveOnCurve
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutPaveOnCurve(const BOPTools_PaveSet& aPaveSet,
					   const Standard_Real aTolR3D,
					   BOPTools_Curve& aBC)
{ 
  const BOPTools_ListOfPave& aLP=aPaveSet.Set();
  BOPTools_ListIteratorOfListOfPave anIt(aLP);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Pave& aPave=anIt.Value();
    PutPaveOnCurve (aPave, aTolR3D, aBC);
  }
}
//=======================================================================
// function: PutPaveOnCurve
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutPaveOnCurve (const BOPTools_Pave& aPave,
					    const Standard_Real aTolR3D,
					    BOPTools_Curve& aBC)
{
  Standard_Integer nV;
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT, aTolVExt, aTolTresh;
  BRep_Builder aBB;
  //
  aTolTresh=0.01;
  nV=aPave.Index();
  const TopoDS_Vertex& aV=TopoDS::Vertex(myDS->Shape(nV));
  const IntTools_Curve& aC=aBC.Curve();
  Handle (Geom_Curve) aC3D= aC.Curve();
  //
  aTolVExt=BRep_Tool::Tolerance(aV);
  ExtendedTolerance(nV, aTolVExt);
  bIsVertexOnLine=myContext->IsVertexOnLine(aV, aTolVExt, aC, aTolR3D, aT);
  //
  if (bIsVertexOnLine) {
    BOPTools_Pave aPaveNew(nV, aT, BooleanOperations_SurfaceSurface);
    BOPTools_PaveSet& aPaveSet=aBC.Set();
    aPaveSet.Append(aPaveNew);
    //<-B
    BOPTools_Tools::UpdateVertex (aC, aT, aV);
    if(aTolR3D<aTolTresh) {
    aBB.UpdateVertex(aV, aTolR3D);
    }
  }
}
//
//=======================================================================
// function: ExtendedTolerance
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_PaveFiller::ExtendedTolerance(const Standard_Integer nV,
							  Standard_Real& aTolVExt)
{
  Standard_Boolean bFound, bIsNewShape;
  Standard_Integer k, i, aNbLines, aNewShape;
  Standard_Real aT11, aT12, aD1, aD2, aD;
  TopoDS_Vertex aV;
  gp_Pnt aPV, aP11, aP12;
  //
  bFound=Standard_False;
  //
  bIsNewShape=myDS->IsNewShape(nV);
  if (!bIsNewShape) {
    return bFound;
  }
  //
  aV=TopoDS::Vertex(myDS->Shape(nV));
  aPV=BRep_Tool::Pnt(aV);
  //
  const BOPTools_InterferencePool& anInterferencePool=*myIntrPool;
  const BOPTools_CArray1OfEEInterference&  aEEInterfs=anInterferencePool.EEInterfs();
  const BOPTools_CArray1OfESInterference&  aESInterfs=anInterferencePool.ESInterfs();
  //
  for (k=0; k<2; ++k) {
    aNbLines=(!k) ? aEEInterfs.Extent() : aESInterfs.Extent();
    for (i=1; i<=aNbLines; ++i) {
      BOPTools_ShapeShapeInterference *pI=(!k) ? 
	(BOPTools_ShapeShapeInterference *)&aEEInterfs(i):
	  (BOPTools_ShapeShapeInterference *)&aESInterfs(i);
      //
      aNewShape=pI->NewShape();
      if (aNewShape==nV) {
	const IntTools_CommonPrt& aCPart=(!k) ? 
	  aEEInterfs(i).CommonPrt() : 
	    aESInterfs(i).CommonPrt();
	//
	const TopoDS_Edge& aE1=aCPart.Edge1();
	aCPart.Range1(aT11, aT12);
	BOPTools_Tools::PointOnEdge(aE1, aT11, aP11);
	BOPTools_Tools::PointOnEdge(aE1, aT12, aP12);
	aD1=aPV.Distance(aP11);
	aD2=aPV.Distance(aP12);
	aD=(aD1>aD2)? aD1 : aD2;
	if (aD>aTolVExt) {
	  aTolVExt=aD;
	}
	bFound=!bFound;
	return bFound;
      }
    }
  }
  //
  return bFound;
}
//
//=======================================================================
// function: PutPavesOnCurves
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PutPavesOnCurves ()
{
  Standard_Integer i, j, aNbCurves, aNbFFs, nF1, nF2, nV;
  Standard_Real aTolR3D = 0.;

  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  
  aNbFFs=aFFs.Extent();

  for (i=1; i<=aNbFFs; i++) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    // 
    // Curves' tolerance
    aTolR3D=aFFi.TolR3D();
    //
    // Faces
    nF1=aFFi.Index1();
    nF2=aFFi.Index2();

    //
    // Put existing paves on curves
    BOPTools_PaveSet aPSF;
    PrepareSetForFace (nF1, nF2, aPSF);
    //
    //f
    {
      Standard_Integer nVX;
      BOPTools_ListIteratorOfListOfPave aItLP;
      TColStd_MapOfInteger aMVStick;
      //
      const BOPTools_ListOfPave& aLPX=aPSF.Set();
      aItLP.Initialize(aLPX);
      for (; aItLP.More(); aItLP.Next()) {
	const BOPTools_Pave& aPX=aItLP.Value();
	nVX=aPX.Index();
	aMVStick.Add(nVX);
      }
      //
      CorrectTolR3D(*this, aFFi, aMVStick, aTolR3D);
    }
    //t
    //
    BOPTools_SequenceOfCurves& aSCvs=aFFi.Curves();
    aNbCurves=aSCvs.Length();
    for (j=1; j<=aNbCurves; j++) {
      BOPTools_Curve& aBC=aSCvs(j);
      // DEBUG
      const IntTools_Curve& aC=aBC.Curve();
      Handle (Geom_Curve) aC3D= aC.Curve();
      //
      PutPaveOnCurve (aPSF, aTolR3D, aBC);
    }
    //f
    {
      Standard_Integer aNbVtx, jx;
      Standard_Real aTolVRange;
      TColStd_SequenceOfInteger aSeqVx;
      TColStd_SequenceOfReal    aSeqTolVx;
      //
      ProcessAloneStickVertices(nF1, 
				nF2, 
				aPSF, 
				aSCvs, 
				*this,
				aSeqVx, 
				aSeqTolVx);
      // 
      aNbVtx=aSeqVx.Length();
      for (jx=1; jx<=aNbVtx; ++jx) {
	BOPTools_PaveSet aPSFx;
	BOPTools_Pave aPVx;

	nV=aSeqVx(jx);
	aTolVRange=aSeqTolVx(jx);
	
	aPVx.SetIndex(nV);
	aPSFx.Append(aPVx);

	for (j=1; j<=aNbCurves; j++) {
	  BOPTools_Curve& aBC=aSCvs(j);
	  // DEBUG
	  const IntTools_Curve& aC=aBC.Curve();
	  Handle (Geom_Curve) aC3D= aC.Curve();
	  //
	  PutPaveOnCurve (aPSFx, aTolVRange, aBC);
	}
      }
    }
    //t
    //
    // Put bounding paves on curves
    //Check very specific case of cone-cone intersection (OCC13211)

    Standard_Boolean bIsSpecific = Standard_False;
    if(aNbCurves >= 4) {
      const TopoDS_Shape aF1 = myDS->GetShape(nF1);
      BRepAdaptor_Surface aS1(TopoDS::Face(aF1), Standard_False);
      GeomAbs_SurfaceType aSType = aS1.GetType();
      if(aSType == GeomAbs_Cone) {
	const TopoDS_Shape aF2 = myDS->GetShape(nF2);
	BRepAdaptor_Surface aS2(TopoDS::Face(aF2), Standard_False);
	aSType = aS2.GetType();
	if(aSType == GeomAbs_Cone) {
	  bIsSpecific = Standard_True;
	}
      }
    }
    //
    if(bIsSpecific) {
      for (j=1; j<=aNbCurves; j++) {
	BOPTools_Curve& aBC=aSCvs(j);
	PutBoundPaveOnCurveSpec (aBC, aFFi);
      }
    }
    else {
      for (j=1; j<=aNbCurves; j++) {
	BOPTools_Curve& aBC=aSCvs(j);
	PutBoundPaveOnCurve (aBC, aFFi);
      }
    }
    //
    // Put closing pave if needded
    for (j=1; j<=aNbCurves; ++j) {
      BOPTools_Curve& aBC=aSCvs(j);
      PutClosingPaveOnCurve (aBC, aFFi);
    }
    //
    // xxx
    for (j=1; j<=aNbCurves; j++) {
      BOPTools_Curve& aBC=aSCvs(j);
      BOPTools_ListOfPave anOldList;
      anOldList = aBC.Set().Set();

      if (aBC.NewPaveBlocks().IsEmpty()) {
	continue;
      }
      //
      
      BOPTools_CArray1OfESInterference& aESs = myIntrPool->ESInterferences();
      Standard_Integer k, fit;
      //
      // Among all aESs find those that are between nE1 from nF1(nE2 from nF2) and nF2(nF1)  
      for(k = 1; k <= aESs.Length(); k++) {
	BOPTools_ESInterference& aES = aESs(k);

	if(aES.Index1() == nF1 || aES.Index2() == nF2) {
	  Standard_Integer nE = (aES.Index1() == nF1) ? aES.Index2() : aES.Index1();

	  // check if it belongs to F1 or F2.begin
	  Standard_Boolean edgefound = Standard_False;

	  for(fit = 0; !edgefound && (fit < 2); fit++) {
	    Standard_Integer nF = (fit == 0) ? nF1 : nF2;
	    Standard_Integer sit1 = 0, sit2 = 0;

	    for(sit1 = 1; !edgefound && (sit1 <= myDS->NumberOfSuccessors(nF)); sit1++) {
	      Standard_Integer asuccessor = myDS->GetSuccessor(nF, sit1);

	      for(sit2 = 1; sit2 <= myDS->NumberOfSuccessors(asuccessor); sit2++) {
		if(nE == myDS->GetSuccessor(asuccessor, sit2)) {
		  edgefound = Standard_True;
		  break;
		}
	      }
	    }
	  }
	  // check if it belongs to F1 or F2.end

	  if(edgefound) {
	    RestrictCurveIn2d (nE, nF1, nF2, aTolR3D, aBC);// ->
	  }
	}
      }// for(k = 1; k <= aESs.Length(); k++) 

      for(fit = 1; fit <= 2; fit++) {
	Standard_Integer nF = (fit == 1) ? nF1 : nF2;
	Standard_Integer nFOpposite = (fit == 1) ? nF2 : nF1;
	TopExp_Explorer anExp(myDS->Shape(nF), TopAbs_EDGE);

	for(; anExp.More(); anExp.Next()) {
	  Standard_Integer nE = myDS->ShapeIndex(anExp.Current(), fit);

	  if(nE) {
	    const BOPTools_ListOfInterference& aList =
	      myIntrPool->InterferenceTable().Value(nE).GetOnType(BooleanOperations_EdgeSurface);
	    BOPTools_ListIteratorOfListOfInterference anIt2(aList);
	    Standard_Boolean bProcessed = Standard_False;

	    for(; anIt2.More(); anIt2.Next()) {
	      if(anIt2.Value().With() == nFOpposite) {
		if(!bProcessed) {
		  RestrictCurveIn2d (nE, nF1, nF2, aTolR3D, aBC);
		  bProcessed = Standard_True;
		  break;
		}
	      }
	    }

	    if(!myCommonBlockPool(myDS->RefEdge(nE)).IsEmpty() &&
	       !bProcessed) {
	      RestrictCurveIn2d (nE, nF1, nF2, aTolR3D, aBC);
	    }
	  }
	}
      }
      // end for(fit = 1...

      // put new paves on other curves.begin
      BOPTools_ListOfPave aListOfNewPave;
      BOPTools_ListIteratorOfListOfPave anIt1, anIt2;

      for(anIt1.Initialize(aBC.Set().Set()); anIt1.More(); anIt1.Next()) {
	Standard_Boolean bfound = Standard_False;
	for(anIt2.Initialize(anOldList); anIt2.More(); anIt2.Next()) {
	  if(anIt1.Value().IsEqual(anIt2.Value())) {
	    bfound = Standard_True;
	    break;
	  }
	}

	if(!bfound) {
	  aListOfNewPave.Append(anIt1.Value());
	}
      }

      Standard_Integer m = 0, n = 0;
      for (m=1; m<=aNbFFs; m++) {
	BOPTools_SSInterference& aFFm = aFFs(m);
	// 
	// Curves' tolerance
	Standard_Real aTolR3D2 = aFFm.TolR3D();

	BOPTools_SequenceOfCurves& aSCvs2 = aFFm.Curves();
	Standard_Integer aNbCurves2 = aSCvs2.Length();

	for(n = 1; n <= aNbCurves2; n++) {
	  if((n == j) && (m == i))
	    continue;
	  BOPTools_Curve& aBC2 = aSCvs2(n);

	  for(anIt1.Initialize(aListOfNewPave); anIt1.More(); anIt1.Next()) {
	    Standard_Boolean bfound = Standard_False;
	    for(anIt2.Initialize(aBC2.Set().Set()); anIt2.More(); anIt2.Next()) {
	      if(anIt1.Value().Index() == anIt2.Value().Index()) {
		bfound = Standard_True;
		break;
	      }	      
	    }

	    if(!bfound) {
	      PutPaveOnCurve (anIt1.Value(), aTolR3D2, aBC2);
	    }
	  }
	}
      }
      // put new paves on other curves.end
    } // xxx for (j=1; j<=aNbCurves; j++)

    //wkar OCC334 f
    {
      Standard_Integer aNbVtx, jx;
      Standard_Real aTolVRange;
      TColStd_SequenceOfInteger aSeqVx;
      TColStd_SequenceOfReal    aSeqTolVx;
      //
      ProcessAloneStickVertices(nF1, 
				nF2, 
				aPSF, 
				aSCvs, 
				*myIntrPool, 
				*this,
				aSeqVx, 
				aSeqTolVx);
      //
      aNbVtx=aSeqVx.Length();
      for (jx=1; jx<=aNbVtx; ++jx) {
	BOPTools_PaveSet aPSFx;
	BOPTools_Pave aPVx;

	nV=aSeqVx(jx);
	aTolVRange=aSeqTolVx(jx);
	
	aPVx.SetIndex(nV);
	aPSFx.Append(aPVx);

	for (j=1; j<=aNbCurves; j++) {
	  BOPTools_Curve& aBC=aSCvs(j);
	  // DEBUG
	  const IntTools_Curve& aC=aBC.Curve();
	  Handle (Geom_Curve) aC3D= aC.Curve();
	  //
	  PutPaveOnCurve (aPSFx, aTolVRange, aBC);
	}
      }
    }
    //wkar OCC334 t
  }
}
//=======================================================================
// function: PrepareSetForFace
// purpose: 
//=======================================================================
  void BOPTools_PaveFiller::PrepareSetForFace(const Standard_Integer nF1,
					      const Standard_Integer nF2,
					      BOPTools_PaveSet& aPaveSet)
{ 
  Standard_Integer i, aNbV, nV;
  TColStd_IndexedMapOfInteger aMV;
  //
  StickVertices(nF1, nF2, aMV);
  //
  aNbV=aMV.Extent();
  for (i=1; i<=aNbV; ++i) {
    nV=aMV(i);
    BOPTools_Pave aPV;
    aPV.SetIndex(nV);
    aPaveSet.Append(aPV);
  }
}
//=======================================================================
// function: IsPaveBlock
// purpose: 
//=======================================================================
Standard_Boolean IsPaveBlock(const Standard_Integer nV1,
			     const Standard_Integer nV2, 
			     const BOPTools_ListOfPaveBlock& aLPBExisting)
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Integer nVE1, nVE2;

  BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPBExisting);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPBR=anIt.Value();
    // 
    nVE1=aPBR.Pave1().Index();
    // 
    nVE2=aPBR.Pave2().Index();
    //
    if ((nVE1==nV1 && nVE2==nV2) || (nVE2==nV1 && nVE1==nV2)) {
      return bFlag;
    }
  }
   return !bFlag;
}

//=======================================================================
// function: FMapWhat
// purpose: 
//=======================================================================
  void FMapWhat(const Standard_Integer nF,
		    BOPTools_InterferencePool* myIntrPool,
		    TColStd_IndexedMapOfInteger& aMapWhat)
		    
{
  Standard_Integer nE, nV;
 
  

  BooleanOperations_ShapesDataStructure* myDS=myIntrPool->DS();
  BooleanOperations_OnceExplorer aExp(*myDS);
  //
  //  What
  aMapWhat.Add(nF);
  aExp.Init(nF, TopAbs_VERTEX);
  for (; aExp.More(); aExp.Next()) {
    nV=aExp.Current();
    aMapWhat.Add(nV);
  }
  //
  aExp.Init(nF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE=aExp.Current();
    aMapWhat.Add(nE);
  }
}

//=======================================================================
// function: FMapWith
// purpose: 
//=======================================================================
  void FMapWith(const Standard_Integer nF,
		BOPTools_InterferencePool* myIntrPool,
		TColStd_IndexedMapOfInteger& aMapWith)
{
  TColStd_IndexedMapOfInteger aMapWhat;
  
  FMapWhat(nF, myIntrPool, aMapWhat);
  //
  // With
  Standard_Integer i, aNb, anIndex, aWhat, aWith;
  BOPTools_ListIteratorOfListOfInterference anIt;
  
  const BOPTools_CArray1OfInterferenceLine& anArrIL= myIntrPool->InterferenceTable();

  aNb=aMapWhat.Extent();
  for (i=1; i<=aNb; i++) {
    aWhat=aMapWhat(i);
    
    const BOPTools_InterferenceLine& aWithLine=anArrIL(aWhat);
  
    const BOPTools_ListOfInterference& aLI=aWithLine.List();
    anIt.Initialize(aLI);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_Interference& anIntf=anIt.Value();
      anIndex=anIntf.Index();
      if (anIndex) {
	aWith=anIntf.With();
	aMapWith.Add(aWith);
      }
    }
  }
}

//=======================================================================
// function: IsFound
// purpose: 
//=======================================================================
Standard_Boolean IsFound(const TColStd_IndexedMapOfInteger& aMapWhat,
			 const TColStd_IndexedMapOfInteger& aMapWith)
{
  Standard_Boolean bFlag=Standard_False;
  Standard_Integer i, aNb, aWhat;

  aNb=aMapWhat.Extent();
  for (i=1; i<=aNb; i++) {
    aWhat=aMapWhat(i);
    if (aMapWith.Contains(aWhat)) {
      return !bFlag;
    }
  }
  return bFlag;
}


//wkar OCC334 f
#include <BOPTools_CArray1OfESInterference.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_ESInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <IntTools_CommonPrt.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BOPTools_Tools3D.hxx>

//=======================================================================
// function: ProcessAloneStickVertices
// purpose: 
//=======================================================================
void ProcessAloneStickVertices(const Standard_Integer nF1,
			       const Standard_Integer nF2,
			       const BOPTools_PaveSet& aPSF,
			       BOPTools_SequenceOfCurves& aSCvs,
			       const BOPTools_InterferencePool& anIntrPool, 
			       BOPTools_PaveFiller& aPF,
			       TColStd_SequenceOfInteger& aSeqVx,
			       TColStd_SequenceOfReal& aSeqTolVx)
{
  Standard_Boolean bFound, bIsClosed;
  Standard_Integer aNbVtx, jx, nV;
  Standard_Real aTolVRange;
  TColStd_IndexedMapOfInteger aMapUnUsed;
  GeomAbs_SurfaceType aType1, aType2;
  //
  BooleanOperations_PShapesDataStructure pDS=anIntrPool.DS();
  //
  const TopoDS_Face& aF1= TopoDS::Face(pDS->Shape(nF1));
  const TopoDS_Face& aF2= TopoDS::Face(pDS->Shape(nF2));
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2);
  GeomAdaptor_Surface aGAS1(aS1);
  GeomAdaptor_Surface aGAS2(aS2);
  
  aType1=aGAS1.GetType();
  aType2=aGAS2.GetType();

  if((aType1==GeomAbs_Cylinder && aType2==GeomAbs_Cylinder) 
     ||
     (aType1==GeomAbs_Plane && aType2==GeomAbs_Plane)) {
    //
    UnUsedMap(aSCvs, aPSF, aMapUnUsed);
    // 
    aNbVtx=aMapUnUsed.Extent();
    for (jx=1; jx<=aNbVtx; ++jx) {
      nV=aMapUnUsed(jx);
      if (pDS->IsNewShape(nV)) {
	bFound=VertexRangeTolerance(nV, nF1, nF2, anIntrPool, aTolVRange);
	if (bFound) {
	  aSeqVx.Append(nV);
	  aSeqTolVx.Append(aTolVRange);
	}
      }
    }
  }
  //
  //wkar  pkv/904/F7
  if((aType1==GeomAbs_Torus && aType2==GeomAbs_Plane) ||
     (aType1==GeomAbs_Plane && aType2==GeomAbs_Torus)) {
    Standard_Integer aNbSCvs, aNbPoints;
    Standard_Real aDist, aTolV;
    gp_Pnt aPV;
    //
    UnUsedMap(aSCvs, aPSF, aMapUnUsed);
    aNbVtx=aMapUnUsed.Extent();
    if (aNbVtx) {
      const Handle(IntTools_Context)& aCtx=aPF.Context();
      //
      aNbSCvs=aSCvs.Length();
      if (aNbSCvs==1) {
	BOPTools_Curve& aBC=aSCvs(1);
	const IntTools_Curve& aIC=aBC.Curve();
	Handle (Geom_Curve) aC3D= aIC.Curve();
	//
	bIsClosed=IntTools_Tools::IsClosed(aC3D);
	if (bIsClosed) {
	  return;
	}
	GeomAPI_ProjectPointOnCurve& aProjPT=aCtx->ProjPT(aC3D);
	//
	for (jx=1; jx<=aNbVtx; ++jx) {
	  nV=aMapUnUsed(jx);
	  if (pDS->IsNewShape(nV)) {
	    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&pDS->Shape(nV));
	    aTolV=BRep_Tool::Tolerance(aV);
	    aPV=BRep_Tool::Pnt(aV);
	    //
	    aProjPT.Perform(aPV);
	    aNbPoints=aProjPT.NbPoints();
	    if (aNbPoints) {
	      aDist=aProjPT.LowerDistance();
	      aDist=(aDist>aTolV)?  aDist : aTolV;
	      aSeqVx.Append(nV);
	      aSeqTolVx.Append(aDist);
	    }
	  }
	}
      }
    }
  }
}
//=======================================================================
// function: ProcessAloneStickVertices
// purpose: 
//=======================================================================
void ProcessAloneStickVertices(const Standard_Integer nF1,
			       const Standard_Integer nF2,
			       const BOPTools_PaveSet& aPSF,
			       BOPTools_SequenceOfCurves& aSCvs,
			       BOPTools_PaveFiller& aPF,
			       TColStd_SequenceOfInteger& aSeqVx,
			       TColStd_SequenceOfReal& aSeqTolVx)
{
  GeomAbs_SurfaceType aType1, aType2;
  //
  BooleanOperations_PShapesDataStructure pDS=aPF.DS();
  //
  const TopoDS_Face& aF1= TopoDS::Face(pDS->Shape(nF1));
  const TopoDS_Face& aF2= TopoDS::Face(pDS->Shape(nF2));
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  Handle(Geom_Surface) aS2=BRep_Tool::Surface(aF2);
  GeomAdaptor_Surface aGAS1(aS1);
  GeomAdaptor_Surface aGAS2(aS2);
  //
  aType1=aGAS1.GetType();
  aType2=aGAS2.GetType();
  //
  if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
    Standard_Integer aNbSCvs, jVU, aNbVU, nVU, k, m, n;
    Standard_Real aTC[2], aD, aD2, aDT2, aU, aV, aScPr, aDScPr;
    TColStd_IndexedMapOfInteger aMVU;
    GeomAbs_CurveType aTypeC;
    gp_Pnt aPC[2], aPVU;
    gp_Dir aDN[2];
    gp_Pnt2d aP2D;
    
    Handle(Geom2d_Curve) aC2D[2];
    //
    aDT2=2e-7;     // the rich criteria
    aDScPr=5.e-9;  // the creasing criteria
    //
    UnUsedMap(aSCvs, aPSF, aMVU);
    //
    aNbVU=aMVU.Extent();
    for (jVU=1; jVU<=aNbVU; ++jVU) {
      nVU=aMVU(jVU);
      const TopoDS_Vertex& aVU=*((TopoDS_Vertex*)&pDS->Shape(nVU));
      aPVU=BRep_Tool::Pnt(aVU);
      //
      aNbSCvs=aSCvs.Length();
      for (k=1; k<=aNbSCvs; ++k) {
	BOPTools_Curve& aBC=aSCvs(k);
	const IntTools_Curve& aIC=aBC.Curve();
	//Handle(Geom_Curve) aC3D=aIC.Curve(); //DEB
	aTypeC=aIC.Type();
	if (!(aTypeC==GeomAbs_BezierCurve || GeomAbs_BSplineCurve)) {
	  continue;
	}
	//
	aIC.Bounds(aTC[0], aTC[1], aPC[0], aPC[1]);
	aC2D[0]=aIC.FirstCurve2d();
	aC2D[1]=aIC.SecondCurve2d();
	if (aC2D[0].IsNull() || aC2D[1].IsNull()) {
	   continue;
	}
	//
	for (m=0; m<2; ++m) {
	  aD2=aPC[m].SquareDistance(aPVU);
	  if (aD2>aDT2) {// no rich
	    continue; 
	  }
	  //
	  for (n=0; n<2; ++n) {
	    Handle(Geom_Surface)& aS=(!n)? aS1 : aS2;
	    aC2D[n]->D0(aTC[m], aP2D);
	    aP2D.Coord(aU, aV);
	    BOPTools_Tools3D::GetNormalToSurface(aS, aU, aV, aDN[n]);
	  }
	  // 
	  aScPr=aDN[0]*aDN[1];
	  if (aScPr<0.) {
	    aScPr=-aScPr;
	  }
	  aScPr=1.-aScPr;
	  //
	  if (aScPr>aDScPr) {
	    continue;
	  }
	  //
	  // The intersection curve aIC is vanishing curve (the crease)
	  aD=sqrt(aD2);
	  //
	  aSeqVx.Append(nVU);
	  aSeqTolVx.Append(aD);
	}
      }//for (k=1; k<=aNbSCvs; ++k) {
    }//for (jVU=1; jVU=aNbVU; ++jVU) {
  }//if(aType1==GeomAbs_Torus  || aType2==GeomAbs_Torus) {
}
//=======================================================================
// function: UnUsedMap
// purpose: 
//=======================================================================
void UnUsedMap(BOPTools_SequenceOfCurves& aSCvs,
	       const BOPTools_PaveSet& aPSF,
	       TColStd_IndexedMapOfInteger& aMapUnUsed)
{
  //
  // What stick/non-stick vertices we used 
  TColStd_IndexedMapOfInteger aMapUsed, aMapMustBeUsed;
  Standard_Integer j, aNbCurves, aNbVtx, nV1;//, nV2;
  BOPTools_ListIteratorOfListOfPave anLPIt;

  aNbCurves=aSCvs.Length();
  for (j=1; j<=aNbCurves; ++j) {
    BOPTools_Curve& aBC=aSCvs(j);
    //const IntTools_Curve& aC= aBC.Curve();// Wng in Gcc 3.0
	
    const BOPTools_PaveSet& aPaveSet=aBC.Set();
    const BOPTools_ListOfPave& aLPAlreadyUsed=aPaveSet.Set();
    anLPIt.Initialize(aLPAlreadyUsed);
    for (; anLPIt.More(); anLPIt.Next()) {
      const BOPTools_Pave& aPave=anLPIt.Value();
      nV1=aPave.Index();
      aMapUsed.Add(nV1);
    }
  }
  // 
  // 2. Stick vertices that must be used
  const BOPTools_ListOfPave& aLPMustUsed=aPSF.Set();
  anLPIt.Initialize(aLPMustUsed);
  for (; anLPIt.More(); anLPIt.Next()) {
    const BOPTools_Pave& aPave=anLPIt.Value();
    nV1=aPave.Index();
    aMapMustBeUsed.Add(nV1);
  }
  //
  // 3.Unused Stick vertices .
  aNbVtx=aMapMustBeUsed.Extent();
  for (j=1; j<=aNbVtx; ++j) {
    nV1=aMapMustBeUsed(j);
    if (!aMapUsed.Contains(nV1)) {
      aMapUnUsed.Add(nV1);
    }
  }
  // 
}
//=======================================================================
// function: VertexRangeTolerance
// purpose: 
//=======================================================================
Standard_Boolean VertexRangeTolerance(const Standard_Integer nV,
				      const Standard_Integer nF1,
				      const Standard_Integer nF2,
				      const BOPTools_InterferencePool& anIntrPool,
				      Standard_Real& aTolV)
{
  Standard_Boolean bFound=Standard_False;
  Standard_Integer i, aNbEFs, iWhat, iWith, iNewShape ;
  TColStd_IndexedMapOfInteger aMEF;
  //
  BooleanOperations_PShapesDataStructure pDS=anIntrPool.DS();
  //
  const TopoDS_Vertex& aV=TopoDS::Vertex(pDS->Shape(nV));
  //
  FaceAndEdgeMap(nF1, anIntrPool, aMEF);
  FaceAndEdgeMap(nF2, anIntrPool, aMEF);
  //
  // EF Interferences
  const BOPTools_CArray1OfESInterference& aEFs=anIntrPool.ESInterfs();
  
  aNbEFs=aEFs.Extent();
  for (i=1; i<=aNbEFs; ++i) {
    const BOPTools_ESInterference& aEF=aEFs(i);
    iNewShape=aEF.NewShape();
    if (iNewShape==nV) {
      aEF.Indices(iWhat, iWith);
      if (aMEF.Contains(iWhat) && aMEF.Contains(iWith)) {
	//...
	Standard_Real aTolVWas, aD1, aD2, aDMax;
	gp_Pnt aP3DV, aP3DV1, aP3DV2;
	//
	const IntTools_CommonPrt& aCommonPrt=aEF.CommonPrt();
	//const TopoDS_Edge& aE1= aCommonPrt.Edge1();// Wng in Gcc 3.0
	
	aP3DV=BRep_Tool::Pnt(aV);
	aTolVWas=BRep_Tool::Tolerance(aV);
	
	aCommonPrt.BoundingPoints(aP3DV1, aP3DV2);
	aD1=aP3DV.Distance(aP3DV1);
	aD2=aP3DV.Distance(aP3DV2);
	
	aDMax=Max(aD1, aD2);

	aTolV=aTolVWas;
	if (aDMax>aTolVWas) {
	  aTolV=aDMax;
	}
	return !bFound;
      }
    }
  }
  //
  // EE Interferences
  /*
  const BOPTools_CArray1OfEEInterference& aEEs=anIntrPool.EEInterfs();
  aNbEEs=aEEs.Extent();
  for (i=1; i<=aNbEEs; ++i) {
    const BOPTools_EEInterference& aEE=aEEs(i);
    iNewShape=aEE.NewShape();
    if (iNewShape==nV) {
      aEE.Indices(iWhat, iWith);
      if (aMEF.Contains(iWhat) && aMEF.Contains(iWith)) {
	//...
	aTolV=0.;
	return !bFound;
      }
    }
  }
  */
  //
  return bFound;
}

//=======================================================================
// function: FaceAndEdgeMap
// purpose: 
//=======================================================================
void FaceAndEdgeMap(const Standard_Integer nF,
		    const BOPTools_InterferencePool& anIntrPool,
		    TColStd_IndexedMapOfInteger& aMEF)
{
  Standard_Integer nEF;
  aMEF.Add(nF);
  BooleanOperations_PShapesDataStructure myDS=anIntrPool.DS();
  BooleanOperations_OnceExplorer aExp(*myDS);
  aExp.Init(nF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nEF=aExp.Current();
    aMEF.Add(nEF);
  }
}
//wkar OCC334 t

//=======================================================================
// function: IsPairFound
// purpose: 
//=======================================================================
Standard_Boolean IsPairFound(const Standard_Integer nF1,
			     const Standard_Integer nF2,
			     BOPTools_InterferencePool* myIntrPool,
			     BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWhat,
			     BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWith)
{
  Standard_Boolean bIsFound;
  //
  if (!aMapWhat.Contains(nF1)) {
    TColStd_IndexedMapOfInteger aMWhat;
    FMapWhat(nF1, myIntrPool, aMWhat);
    aMapWhat.Add(nF1, aMWhat);
  }
  //
  if (!aMapWith.Contains(nF2)) {
    TColStd_IndexedMapOfInteger aMWith;
    FMapWith(nF2, myIntrPool, aMWith);
    aMapWith.Add(nF2, aMWith);
  }
  //
  const TColStd_IndexedMapOfInteger& aMWht=aMapWhat.FindFromKey(nF1);
  const TColStd_IndexedMapOfInteger& aMWit=aMapWith.FindFromKey(nF2);
  //
  bIsFound=IsFound(aMWht, aMWit);
  //
  return bIsFound;
}

//=======================================================================
// function: RestrictCurveIn2d
// purpose:  Intersects 2d curve of edge nE and 2d curve of theBC.
//           Splits theBC by new vertex, also splits nE by new vertex.
//           If nE has same domain with another edge, the same domain 
//           edge is splitted too.
//=======================================================================
  void BOPTools_PaveFiller::RestrictCurveIn2d(const Standard_Integer nE,
					      const Standard_Integer nF1,
					      const Standard_Integer nF2,
					      const Standard_Real    theTolerance,
					      BOPTools_Curve&        theBC) 
{
  myPavePoolNew.Resize (myNbEdges);
  TopoDS_Shape atmpShape;
 
  BOPTools_CArray1OfESInterference& aESs = myIntrPool->ESInterferences();

  BOPTools_PaveSet& aPS = myPavePool(myDS->RefEdge(nE));
  Standard_Boolean bSearchInter = Standard_True;

  // 6841
  Standard_Integer pvVrtIndex = 0;
  //

  BOPTools_ListIteratorOfListOfPave anIt1(aPS.Set());
  BOPTools_ListIteratorOfListOfPave anIt2;

  for(; bSearchInter && anIt1.More(); anIt1.Next()) {

    for(anIt2.Initialize(theBC.Set().Set()); anIt2.More(); anIt2.Next()) {

      if(anIt1.Value().Index() == anIt2.Value().Index()) { // too hard condition for the algorithm
	// 6841
        pvVrtIndex = anIt1.Value().Index();
        //
	bSearchInter = Standard_False;
	break;
      }
    }
  }
  // -- 6841: test 2d intersection any way, but update vertex only
  if((!bSearchInter && pvVrtIndex != 0)) {
    Standard_Boolean OnFirst = (myDS->Rank(nE) == 1);
    TopoDS_Edge aE = TopoDS::Edge(myDS->Shape(nE));
    TopoDS_Face aF  = (OnFirst) ? TopoDS::Face(myDS->Shape(nF1)) : TopoDS::Face(myDS->Shape(nF2));
    Standard_Real pf2 = 0., pl2 = 0.;
    Handle(Geom_Curve)   aC2D3 = BRep_Tool::Curve(aE, pf2, pl2);
    Handle(Geom2d_Curve) aC2D2 = BRep_Tool::CurveOnSurface(aE, aF, pf2, pl2);
    Handle(Geom2d_Curve) aC1D2 = (OnFirst) ? theBC.Curve().FirstCurve2d() : theBC.Curve().SecondCurve2d();
    Handle(Geom_Curve)   aC1D3 = theBC.Curve().Curve();
    if((!aC2D3.IsNull() && !aC2D2.IsNull()) && (!aC1D2.IsNull() && !aC1D3.IsNull())) {
      Standard_Real pf1 = aC1D2->FirstParameter();
      Standard_Real pl1 = aC1D2->LastParameter();
      Geom2dAPI_InterCurveCurve aInt(aC1D2, aC2D2, Precision::PConfusion());
      if(aInt.NbPoints() > 0) {
        Standard_Integer jj = 1;
        for(; jj <= aInt.NbPoints(); jj++) {
          Standard_Real t1 = aInt.Intersector().Point(jj).ParamOnFirst();
          Standard_Real t2 = aInt.Intersector().Point(jj).ParamOnSecond();
          if((t1 >= pf1) && (t1 <= pl1) && (t2 >= pf2) && (t2 <= pl2)) {
//             gp_Pnt2d aP2d = aInt.Point(jj);
            gp_Pnt   aP3d = aC2D3->Value(t2);
            gp_Pnt   aP3d2 = aC1D3->Value(t1);
            TopoDS_Vertex & aVrt = (TopoDS_Vertex &) myDS->Shape(pvVrtIndex);
            gp_Pnt aVP = BRep_Tool::Pnt(aVrt);
            Standard_Real aVTol  = BRep_Tool::Tolerance(aVrt);
            Standard_Real aD1 = aP3d.Distance(aVP);
            Standard_Real aD2 = aP3d2.Distance(aVP);
            Standard_Real aFD1 = fabs(aVTol-aD1);
            Standard_Real aFD2 = fabs(aVTol-aD2);
            if(aD1 > aVTol || aD2 > aVTol) {
              if(Max(aFD1,aFD2) <= 1.e-2) {
                Standard_Real nTol = aVTol + 2. * Max(aFD1,aFD2) + 1.e-7;
                BRep_Builder bb;
                bb.UpdateVertex(aVrt, nTol);
              }
            }
          }
        }
      }
    }
  }
  //-- 6841

  if(bSearchInter) {
    // search intersection in 2d. begin
    BOPTools_ListOfPave aPavesOnCurve, aPavesOnEdge;
    Standard_Boolean bIsOnFirst = (myDS->Rank(nE) == 1);

    Handle(Geom2d_Curve) aC1 = (bIsOnFirst) ? theBC.Curve().FirstCurve2d() : theBC.Curve().SecondCurve2d();

    if(aC1.IsNull())
      return;
    Standard_Real f1 = aC1->FirstParameter();
    Standard_Real l1 = aC1->LastParameter();
    
    // f1 and l1 may not correspond 
    // to boundary parameters of 3d curve
    if(theBC.Curve().HasBounds()) {
      gp_Pnt tmpp1, tmpp2;
      theBC.Curve().Bounds(f1, l1, tmpp1, tmpp2);
    }

    atmpShape           = myDS->Shape(nE);
    TopoDS_Edge  anEdge = TopoDS::Edge(atmpShape);
    TopoDS_Shape aFace  = (bIsOnFirst) ? myDS->Shape(nF1) : myDS->Shape(nF2);
    Standard_Real f2=0., l2=0.;
    Handle(Geom2d_Curve) aC2 = BRep_Tool::CurveOnSurface(anEdge, TopoDS::Face(aFace), f2, l2);
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, f2, l2);

    if(aC2.IsNull() || aCurve.IsNull())
      return;

    Geom2dAPI_InterCurveCurve anIntersector(aC1, aC2, Precision::PConfusion());

    if(anIntersector.NbPoints() > 0) {
      Standard_Integer j = 0;

      for(j = 1; j <= anIntersector.NbPoints(); j++) {
	Standard_Real t1 = anIntersector.Intersector().Point(j).ParamOnFirst();
	Standard_Real t2 = anIntersector.Intersector().Point(j).ParamOnSecond();

	if((t1 >= f1) && (t1 <= l1) &&
	   (t2 >= f2) && (t2 <= l2)) {
	  gp_Pnt2d aP2dOnFace = anIntersector.Point(j);
	  atmpShape  = (!bIsOnFirst) ? myDS->Shape(nF1) : myDS->Shape(nF2);
	  TopoDS_Face anOtherFace = TopoDS::Face(atmpShape);
	  gp_Pnt aP3d = aCurve->Value(t2);

	  if(myContext->IsPointInOnFace(TopoDS::Face(aFace), aP2dOnFace) &&
	     myContext->IsValidPointForFace(aP3d, anOtherFace, BRep_Tool::Tolerance(anEdge))) {
	    BOPTools_Pave aPave1;
	    aPave1.SetParam(t1);
	    aPave1.SetIndex(-1);
	    aPavesOnCurve.Append(aPave1);
	    BOPTools_Pave aPave2;
	    aPave2.SetParam(t2);
	    aPave2.SetIndex(-1);
	    aPavesOnEdge.Append(aPave2);
	  }
	}
      }
    } // (anIntersector.NbPoints())
    // search intersection in 2d. end


    BOPTools_ListIteratorOfListOfPave aPaveIt1(aPavesOnCurve);
    BOPTools_ListIteratorOfListOfPave aPaveIt2(aPavesOnEdge);

    // test common blocks. begin
    Standard_Boolean bFaceCBFound = Standard_False;
    Standard_Boolean bEdgeCBFound = Standard_False;
    const BOPTools_ListOfCommonBlock& aLCBTest = myCommonBlockPool(myDS->RefEdge(nE));
    BOPTools_ListIteratorOfListOfCommonBlock aCBListIt(aLCBTest);
    Standard_Boolean bHasCBOnFace = Standard_False;
    Standard_Boolean bHasCBOnEdge = Standard_False;

    for(; aCBListIt.More(); aCBListIt.Next()) {
      if((aCBListIt.Value().Face() == nF1) ||
	 (aCBListIt.Value().Face() == nF2)) {
	bHasCBOnFace = Standard_True;
      }

      if(aCBListIt.Value().Face() == 0) {
	bHasCBOnEdge = Standard_True;
      }
    }
    
    if(!bHasCBOnFace || !bHasCBOnEdge) {
      BOPTools_PaveSet aTestPaveSet;
      aTestPaveSet.ChangeSet() = aPS.Set();

      for(; aPaveIt2.More(); aPaveIt2.Next()) {
	aTestPaveSet.Append(aPaveIt2.Value());
      }
      BOPTools_PaveBlockIterator aPBIter(0, aTestPaveSet);

      for (; aPBIter.More(); aPBIter.Next()) {
	const BOPTools_PaveBlock& aPB = aPBIter.Value();
	Standard_Real aT1=aPB.Pave1().Param();
	Standard_Real aT2=aPB.Pave2().Param();
	gp_Pnt aPMid = aCurve->Value((aT1 + aT2) * 0.5);
	Standard_Integer nFOpposite = (bIsOnFirst) ? nF2 : nF1;
	TopoDS_Shape aOppFace = myDS->Shape(nFOpposite);

	if(!bHasCBOnFace && !bFaceCBFound &&
	   myContext->IsValidPointForFace(aPMid, TopoDS::Face(aOppFace), 
					 BRep_Tool::Tolerance(anEdge) +
					 BRep_Tool::Tolerance(TopoDS::Face(aOppFace)))) {
	  bFaceCBFound = Standard_True;
	}

	if(!bHasCBOnEdge && !bEdgeCBFound) {
	  TopoDS_Vertex aVMid;
	  BRep_Builder aBB;
	  aBB.MakeVertex(aVMid, aPMid, BRep_Tool::Tolerance(anEdge));
	  TopExp_Explorer anExpE(aOppFace, TopAbs_EDGE);

	  for(; anExpE.More(); anExpE.Next()) {
	    TopoDS_Shape aTmpEdge = anExpE.Current();
	    Standard_Real aParameter = 0.;

	    if(myContext->ComputeVE(aVMid, TopoDS::Edge(aTmpEdge), aParameter) == 0) {
	      bEdgeCBFound = Standard_True;
	      break;
	    }
	  }
	}
      }
      aPaveIt2.Initialize(aPavesOnEdge);
    }
    // test common blocks. end

    Standard_Boolean bChecknAddPaves = Standard_True;

    if(bEdgeCBFound) {
      bChecknAddPaves = Standard_False;
    }
    else {
      if(!bHasCBOnEdge) {
	bChecknAddPaves = !bFaceCBFound;
      }
    }

    if(bChecknAddPaves) {
      // add paves chaking if new pave is equal to existent. begin
      for(; aPaveIt1.More() && aPaveIt2.More(); aPaveIt1.Next(), aPaveIt2.Next()) {
	BOPTools_Pave& aPaveOnCurve = aPaveIt1.Value();
	BOPTools_Pave& aPaveOnEdge  = aPaveIt2.Value();

	gp_Pnt aP1 = theBC.Curve().Curve()->Value(aPaveOnCurve.Param());
	gp_Pnt aP2 = aCurve->Value(aPaveOnEdge.Param());

	Standard_Boolean bAddNewVertex = Standard_True;
	Standard_Boolean bAddOldVertex = Standard_False;
	Standard_Integer oldvertexindex = 0;

	for(anIt2.Initialize(theBC.Set().Set()); anIt2.More(); anIt2.Next()) {
	  atmpShape = myDS->Shape(anIt2.Value().Index());
	  TopoDS_Vertex aVertex = TopoDS::Vertex(atmpShape);
	  gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
	  Standard_Real aTolerance = theTolerance + BRep_Tool::Tolerance(aVertex);
	  if((aPoint.Distance(aP1) < aTolerance) ||
	     (aPoint.Distance(aP2) < aTolerance)) {
	    bAddNewVertex = Standard_False;
	    bAddOldVertex = Standard_True;
	    oldvertexindex = anIt2.Value().Index(); 
	    break;
	  }
	}

	// treat equality with other vertices.begin
	if(bAddNewVertex || bAddOldVertex) {
	  TopoDS_Vertex aNewVertex;
	  BOPTools_Tools::MakeNewVertex(anEdge, aPaveOnEdge.Param(), 
					TopoDS::Face(aFace),  aNewVertex);
	  
	  BOPTools_Pave aPaveToPut;
	  Standard_Boolean bAddNewVertextmp = bAddNewVertex, bAddOldVertextmp = bAddOldVertex;

	  if(!CheckNewVertexAndUpdateData(aNewVertex, aPaveOnEdge.Param(), anEdge, aPaveOnCurve.Param(), 
					  nF1, nF2, theTolerance, myIntrPool, myDS, myContext, aPS, 
					  bAddNewVertextmp, bAddOldVertextmp, theBC, aPaveToPut, 
					  bAddNewVertex, bAddOldVertex)) {
	    bAddNewVertex = Standard_False;
	    bAddOldVertex = Standard_False;
	  }
	  else {
	    if((aPaveToPut.Index() != 0) && (aPaveToPut.Param() != 0.) &&
	       aPaveToPut.Interference() != 0) {
	      PutPaveOnCurve(aPaveToPut, theTolerance, theBC);
	    }
	  }
	  // end for(anIt1.Initialize...
	}
	// treat equality with other vertices.end
	
	if(bAddNewVertex || (bAddOldVertex && oldvertexindex)) {
	  TopoDS_Vertex aNewVertex;

	  Standard_Integer aNewShapeIndex = 0;
          
          // reject creation new vertex if there is a created one at a too small distance
          BOPTools_Tools::MakeNewVertex(anEdge,aPaveOnEdge.Param(),TopoDS::Face(aFace),aNewVertex);
          Standard_Real nvTol = BRep_Tool::Tolerance(aNewVertex);
          Standard_Integer chShNb = myDS->NumberOfInsertedShapes(), chShInd = 0;
          Standard_Boolean completeSearching = Standard_False;
          for(chShInd = 1; chShInd <= chShNb; chShInd++) {
            if(myDS->GetShapeType(chShInd) != TopAbs_VERTEX)
              continue;
            TopoDS_Vertex chV = TopoDS::Vertex(myDS->Shape(chShInd));
            Standard_Real distVV = BRep_Tool::Pnt(chV).Distance(BRep_Tool::Pnt(aNewVertex));
            if(distVV <= 1.e-5) {
              for(anIt1.Initialize(aPS.Set()); anIt1.More(); anIt1.Next()) {
                if(anIt1.Value().Index() == chShInd) {
                  Standard_Real dParam = fabs(aPaveOnEdge.Param()-anIt1.Value().Param());
                  if(dParam <= 1.e-7) {
                    bAddNewVertex = Standard_False;
                    bAddOldVertex = Standard_True;
                    oldvertexindex = anIt1.Value().Index();
                    nvTol += BRep_Tool::Tolerance(chV) + distVV;
                    completeSearching = Standard_True;
                    break;
                  }
                }
              }
            }
            if(completeSearching)
              break;
          }
          // -&&


	  if(!bAddOldVertex) {
	    BOPTools_Tools::MakeNewVertex(anEdge, aPaveOnEdge.Param(), 
					  TopoDS::Face(aFace),  aNewVertex);
	    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq; 
	    myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
	    aNewShapeIndex = myDS->NumberOfInsertedShapes();
	  }
	  else {
	    aNewShapeIndex = oldvertexindex;
	    aNewVertex = TopoDS::Vertex(myDS->Shape(aNewShapeIndex));
	    BRep_Builder aBB;
	    //
            aBB.UpdateVertex(aNewVertex, aPaveOnEdge.Param(), anEdge, nvTol);
	  }

          BOPTools_ListIteratorOfListOfPave anItAll;
          Standard_Boolean samePFound = Standard_False;
          for(anItAll.Initialize(aPS.Set()); anItAll.More(); anItAll.Next()) {
            if(anItAll.Value().Index() == aNewShapeIndex) {
              BOPTools_Pave& aPV = anItAll.Value();
              aPV.SetParam(aPaveOnEdge.Param());
              samePFound = Standard_True; 
              break;
            }
          }
          if(samePFound)
            continue;


	  myDS->SetState (aNewShapeIndex, BooleanOperations_ON);

	  IntTools_CommonPrt aCPart;
	  aCPart.SetEdge1(anEdge);
	  aCPart.SetType(TopAbs_VERTEX);
	  aCPart.SetRange1(IntTools_Range(aPaveOnEdge.Param(), aPaveOnEdge.Param()));
	  aCPart.SetVertexParameter1(aPaveOnEdge.Param());

	  Standard_Integer nFOpposite = (bIsOnFirst) ? nF2 : nF1;
	  BOPTools_ESInterference anInterf (nE, nFOpposite, aCPart);
	  Standard_Integer anIndexIn = aESs.Append(anInterf);
	  myIntrPool->AddInterference (nE, nFOpposite, BooleanOperations_EdgeSurface, anIndexIn);

	  BOPTools_ESInterference& aESInterf = aESs(anIndexIn);
	  aESInterf.SetNewShape(aNewShapeIndex);

	  // put pave on edge. begin 
	  aPaveOnEdge.SetInterference(anIndexIn);
	  aPaveOnEdge.SetType (BooleanOperations_EdgeSurface);
	  aPaveOnEdge.SetIndex(aNewShapeIndex);

	  BOPTools_PaveSet& aPaveSet1 = myPavePoolNew(myDS->RefEdge(nE));
	  aPaveSet1.Append(aPaveOnEdge);
	  // put pave on edge. end

	  // put pave on curve. begin 
	  aPaveOnCurve.SetIndex(aNewShapeIndex);
	  aPaveOnCurve.SetType(BooleanOperations_SurfaceSurface);
	  BOPTools_PaveSet& aPaveSet = theBC.Set();
	  aPaveSet.Append(aPaveOnCurve);

	  BOPTools_Tools::UpdateVertex (theBC.Curve(), aPaveOnCurve.Param(), aNewVertex);

	  // put pave on curve. end

	  BOPTools_ListOfCommonBlock& aLCB1 = myCommonBlockPool(myDS->RefEdge(nE));

	  BOPTools_ListIteratorOfListOfCommonBlock  anIt(aLCB1);

	  for(; anIt.More(); anIt.Next()) {
	    BOPTools_CommonBlock& aCB = anIt.Value();

	    if(aCB.Face()) {
	      continue;
	    }

	    Standard_Integer anOppIndex = aCB.PaveBlock2().OriginalEdge();
	    IntTools_Range aRange = aCB.PaveBlock2().Range();

	    if(anOppIndex == nE) {
	      anOppIndex = aCB.PaveBlock1().OriginalEdge();
	      aRange = aCB.PaveBlock1().Range();
	    }
	    TopoDS_Edge anOppEdge = TopoDS::Edge(myDS->Shape(anOppIndex));
	    Standard_Real aOppParameter = 0.;

	    if(myContext->ComputeVE(aNewVertex, anOppEdge, aOppParameter) == 0) {
	    
	      if((aOppParameter > aRange.First()) && (aOppParameter < aRange.Last())) {
		// put pave on same domain edge. begin
		BRep_Builder aBB;
		aBB.UpdateVertex(aNewVertex, aOppParameter, anOppEdge, BRep_Tool::Tolerance(aNewVertex));
		BOPTools_Pave aPaveOpp;
		aPaveOpp.SetParam(aOppParameter);
		aPaveOpp.SetIndex(aNewShapeIndex);
		BOPTools_PaveSet& aPaveSetOpp = myPavePoolNew(myDS->RefEdge(anOppIndex));
		aPaveSetOpp.Append(aPaveOpp);
		// put pave on same domain edge. end
	      }
	    }
	  }

          // add SS interference for adjacent face.begin
	  if(aLCB1.IsEmpty()) {
	    AddInterfForAdjacentFace(nE, nF1, nF2, myIntrPool, myDS);
	  }
	  // add SS interference for adjacent face.end

	  RefinePavePool();
	  myPavePoolNew.Resize(myNbEdges);
	  //

	  RecomputeCommonBlocks(nE);

	}
      } // end for(; aPaveIt1.More() && aPaveIt2.More()...
      // add paves chaking if new pave is equal to existent. end
    }
  } //(bSearchInter)
  myPavePoolNew.Destroy();
}
//=======================================================================
//function : RecomputeCommonBlocks
//purpose  : 
//=======================================================================
  void BOPTools_PaveFiller::RecomputeCommonBlocks(const Standard_Integer nE) 
{
  TopoDS_Shape atmpShape = myDS->Shape(nE);
  TopoDS_Edge  anEdge   = TopoDS::Edge(atmpShape);
  BOPTools_ListOfCommonBlock& aLCB1 = myCommonBlockPool(myDS->RefEdge(nE));

  BOPTools_ListOfCommonBlock anOldLCB;
  anOldLCB = aLCB1;
  aLCB1.Clear();
  BOPTools_ListOfCommonBlock aNewLCB1;
  Standard_Boolean bReverse = Standard_False;
  BOPTools_ListIteratorOfListOfCommonBlock  anIt(anOldLCB);

  for(; anIt.More(); anIt.Next()) {
    BOPTools_CommonBlock& anOldCB = anIt.Value();

    Standard_Integer anIndex = anOldCB.PaveBlock1().OriginalEdge();
    Standard_Integer anIndexOpp = anOldCB.PaveBlock2().OriginalEdge();
    IntTools_Range aCBRange = anOldCB.PaveBlock1().Range();

    if(anIndex != nE) {
      aCBRange = anOldCB.PaveBlock2().Range();
      anIndex = anOldCB.PaveBlock2().OriginalEdge();
      anIndexOpp = anOldCB.PaveBlock1().OriginalEdge();
      bReverse = Standard_True;
    }

    BOPTools_ListOfPaveBlock& aSplitEdges1 = mySplitShapesPool(myDS->RefEdge(nE));
    BOPTools_ListIteratorOfListOfPaveBlock aLPBIt1(aSplitEdges1);
    Standard_Boolean found = Standard_False;
    BOPTools_ListOfCommonBlock aNewListOfCommonBlock;

    for(; aLPBIt1.More(); aLPBIt1.Next()) {
      BOPTools_PaveBlock& aPBCurrent1 = aLPBIt1.Value();
      IntTools_Range aCurRange1 = aPBCurrent1.Range();

      if((aCurRange1.First() > aCBRange.First() && aCurRange1.First() < aCBRange.Last()) ||
	 (aCurRange1.Last() > aCBRange.First() && aCurRange1.Last() < aCBRange.Last())) {
	BOPTools_CommonBlock aNewCB;

	if(!bReverse)
	  aNewCB.SetPaveBlock1(aPBCurrent1);
	else
	  aNewCB.SetPaveBlock2(aPBCurrent1);

	Standard_Boolean foundpaveblock2 = Standard_False;

	if(anOldCB.Face()) {
	  foundpaveblock2 = Standard_True;
	  aNewCB.SetFace(anOldCB.Face());
	}
	else {
	  if(anIndexOpp <= 0)
	    continue;
	  BOPTools_ListIteratorOfListOfPaveBlock aLPBIt2(mySplitShapesPool(myDS->RefEdge(anIndexOpp)));

	  for(; aLPBIt2.More(); aLPBIt2.Next()) {
	    BOPTools_PaveBlock& aPBCurrent2 = aLPBIt2.Value();
	    IntTools_Range aCurRange2 = aPBCurrent2.Range();

	    if(((aPBCurrent1.Pave1().Index() == aPBCurrent2.Pave1().Index()) && 
		(aPBCurrent1.Pave2().Index() == aPBCurrent2.Pave2().Index())) ||
	       ((aPBCurrent1.Pave1().Index() == aPBCurrent2.Pave2().Index()) && 
		(aPBCurrent1.Pave2().Index() == aPBCurrent2.Pave1().Index()))) {
	      Standard_Real f = 0., l = 0.;
	      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, f, l);
	      Standard_Real aMidPar = (aCurRange1.First() + aCurRange1.Last()) * 0.5;
	      gp_Pnt aMidPnt = aCurve->Value(aMidPar);
	      Standard_Real aProjPar = 0.;
	      TopoDS_Vertex aTestpVertex;
	      TopoDS_Edge aOppEdge = TopoDS::Edge(myDS->Shape(anIndexOpp));
	      BRep_Builder aBB;
	      aBB.MakeVertex(aTestpVertex, aMidPnt, BRep_Tool::Tolerance(anEdge));

	      if(myContext->ComputeVE(aTestpVertex, aOppEdge, aProjPar) == 0) {
		if(aProjPar > aCurRange2.First() && aProjPar < aCurRange2.Last()) {
		  if(!bReverse)
		    aNewCB.SetPaveBlock2(aPBCurrent2);
		  else
		    aNewCB.SetPaveBlock1(aPBCurrent2);
		  foundpaveblock2 = Standard_True;
		  break;
		}
	      }
	    }
	  }
	}

	if(foundpaveblock2) {
	  found = Standard_True;
	  aNewListOfCommonBlock.Append(aNewCB);
	}
      }
    }
    // end for(; aLPBIt1.More()...

    if(!found) {
      aNewLCB1.Append(anOldCB);
    }
    else {
      BOPTools_ListOfCommonBlock tmplst;
      tmplst = aNewListOfCommonBlock;
      aNewLCB1.Append(tmplst);

      if((anOldCB.Face() == 0) && (anIndexOpp > 0)) {
	tmplst = aNewListOfCommonBlock;
	BOPTools_ListOfCommonBlock& aLCB2 = myCommonBlockPool(myDS->RefEdge(anIndexOpp));

	BOPTools_ListIteratorOfListOfCommonBlock anItLCB2(aLCB2);

	for(; anItLCB2.More(); anItLCB2.Next()) {
	  BOPTools_CommonBlock& anOldCB2 = anItLCB2.Value();

	  if(anOldCB2.Face())
	    continue;

	  if(anOldCB.PaveBlock1().IsEqual(anOldCB2.PaveBlock1()) &&
	     anOldCB.PaveBlock2().IsEqual(anOldCB2.PaveBlock2())) {
	    aLCB2.Remove(anItLCB2);
	    aLCB2.Append(tmplst);
	    break;
	  }
	}
      }
    }
  }
  // end for(; anIt.More()...
  aLCB1 = aNewLCB1;
}
//=======================================================================
//function : CheckNewVertexAndUpdateData
//purpose  : 
//=======================================================================
Standard_Boolean CheckNewVertexAndUpdateData(const TopoDS_Vertex&              theVertex,
					     const Standard_Real               theParamOnE,
					     const TopoDS_Edge&                theEdge,
					     const Standard_Real               theParamOnCurve,
					     const Standard_Integer            theIndexF1,
					     const Standard_Integer            theIndexF2,
					     const Standard_Real               theTolerance,
					     const BOPTools_PInterferencePool& theIntrPool,
					     const BooleanOperations_PShapesDataStructure& theDS,
					     const Handle(IntTools_Context)&   theContext,
					     const BOPTools_PaveSet&           theEdgePaveSet,
					     const Standard_Boolean            bAddNewVertex,
					     const Standard_Boolean            bAddOldVertex,
					     BOPTools_Curve&                   theBC,
					     BOPTools_Pave&                    thePaveToPut,
					     Standard_Boolean&                 bAddNewVertexOut,
					     Standard_Boolean&                 bAddOldVertexOut) {

  thePaveToPut.SetParam(0.);
  thePaveToPut.SetIndex(0);
  thePaveToPut.SetInterference(0);

  bAddNewVertexOut = bAddNewVertex;
  bAddOldVertexOut = bAddOldVertex;
  TopoDS_Shape atmpShape;
  gp_Pnt aP1 = theBC.Curve().Curve()->Value(theParamOnCurve);
  Standard_Real f = 0., l = 0.;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, f, l);

  if(aCurve.IsNull())
    return Standard_False;
  gp_Pnt aP2 = aCurve->Value(theParamOnE);

  BOPTools_ListIteratorOfListOfPave anIt1, anIt2;

  for(anIt1.Initialize(theEdgePaveSet.Set()); anIt1.More(); anIt1.Next()) {
    atmpShape = theDS->Shape(anIt1.Value().Index());
    TopoDS_Vertex aVertex = TopoDS::Vertex(atmpShape);
    gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
    Standard_Real aTolerance = theTolerance + BRep_Tool::Tolerance(aVertex);
    aTolerance *= 4.;

    if((aPoint.Distance(aP1) < aTolerance) ||
       (aPoint.Distance(aP2) < aTolerance)) {
      IntTools_Range aRange(anIt1.Value().Param(), theParamOnE);
      TopoDS_Vertex aV1 = aVertex;
      TopoDS_Vertex aV2 = theVertex;

      if(anIt1.Value().Param() > theParamOnE) {
	aRange.SetFirst(theParamOnE);
	aRange.SetLast(anIt1.Value().Param());
	aV1 = theVertex;
	aV2 = aVertex;
      }
      gp_Pnt ptest1 = aCurve->Value(aRange.First());
      gp_Pnt ptest2 = aCurve->Value(aRange.Last());
      Standard_Boolean bUpdateVertex = Standard_True;

      if(ptest1.Distance(ptest2) > (BRep_Tool::Tolerance(aVertex) + BRep_Tool::Tolerance(theEdge))) {
	IntTools_ShrunkRange aSR (theEdge, aV1, aV2, aRange, theContext);
	bUpdateVertex = !aSR.IsDone() || (aSR.ErrorStatus() != 0);
      }

      if(bUpdateVertex) {
	bAddNewVertexOut = Standard_False;
	      
	if(bAddOldVertexOut) {
	  bAddOldVertexOut = Standard_False;
	  break;
	}
	BOPTools_Tools::UpdateVertex (theBC.Curve(), theParamOnCurve, aVertex);

	Standard_Boolean bPutPave = Standard_True;

	for(anIt2.Initialize(theBC.Set().Set()); anIt2.More(); anIt2.Next()) {
	  if(anIt1.Value().Index() == anIt2.Value().Index()) {
	    bPutPave = Standard_False;
	    break;
	  }
	}

	Standard_Integer nbbefore = theBC.Set().Set().Extent();

	if(bPutPave) {
	  thePaveToPut = anIt1.Value();
	}

	if(anIt1.Value().Index() > theDS->NumberOfSourceShapes())
	  break;

	Standard_Integer nbafter = theBC.Set().Set().Extent();

	if(nbbefore < nbafter) {
	  // update interferences.begin
	  Standard_Integer nF = theIndexF1;

	  if(theDS->Rank(anIt1.Value().Index()) != theDS->Rank(theIndexF1)) {
	    nF = theIndexF2;
	  }
	  atmpShape = theDS->Shape(nF);
	  TopoDS_Face   aF = TopoDS::Face  (atmpShape);
	  BOPTools_CArray1OfVSInterference& aVSs = theIntrPool->VSInterferences();
	  Standard_Integer vsit = 0;
	  Standard_Boolean interffound = Standard_False;

	  for(vsit = 1; vsit <= aVSs.Length(); vsit++) {
	    if((aVSs.Value(vsit).Index1() == anIt1.Value().Index()) &&
	       (aVSs.Value(vsit).Index2() == nF)) {
	      interffound = Standard_True;
	      break;
	    }
	  }

	  if(!interffound) {
	    BOPTools_CArray1OfVEInterference& aVEs = theIntrPool->VEInterferences();

	    for(vsit = 1; vsit <= aVEs.Length(); vsit++) {
	      if((aVEs.Value(vsit).Index1() == anIt1.Value().Index())) {
		interffound = Standard_True;
		break;
	      }
	    }
	  }

	  if(!interffound) {
	    BOPTools_CArray1OfVVInterference& aVVs = theIntrPool->VVInterferences();

	    for(vsit = 1; vsit <= aVVs.Length(); vsit++) {
	      if((aVVs.Value(vsit).Index1() == anIt1.Value().Index())) {
		interffound = Standard_True;
		break;
	      }
	    }
	  }

	  if(!interffound) {
	    Standard_Real aU = 0., aV = 0.;
	    Standard_Integer aFlag = theContext->ComputeVS (aVertex, aF, aU, aV);
	    Standard_Integer anIndexIn = 0;

	    if (!aFlag) {
	      //
	      // Add Interference to the Pool
	      BOPTools_VSInterference anInterf (anIt1.Value().Index(), nF, aU, aV);
	      anIndexIn=aVSs.Append(anInterf);
	      //
	      // SetState for Vertex in DS;
	      theDS->SetState (anIt1.Value().Index(), BooleanOperations_ON);
	      // Insert Vertex in Interference Object
	      BOPTools_VSInterference& aVS = aVSs(anIndexIn);
	      aVS.SetNewShape(anIt1.Value().Index());

	      interffound = Standard_False;
	      const BOPTools_ListOfInterference& aList1 =
		theIntrPool->InterferenceTable().Value(anIt1.Value().Index()).GetOnType(BooleanOperations_EdgeSurface);
	      BOPTools_ListOfInterference& amodifList1 = *((BOPTools_ListOfInterference*)&aList1); // not very good approach
	      BOPTools_ListIteratorOfListOfInterference anInterfIt(amodifList1);

	      for(; anInterfIt.More(); anInterfIt.Next()) {
		if(anInterfIt.Value().With() == nF) {
		  anInterfIt.Value().SetIndex(anIndexIn);
		  interffound = Standard_True;
		}
	      }
	      const BOPTools_ListOfInterference& aList2 =
		theIntrPool->InterferenceTable().Value(nF).GetOnType(BooleanOperations_EdgeSurface);
	      BOPTools_ListOfInterference& amodifList2 = *((BOPTools_ListOfInterference*)&aList2); // not very good approach
	      anInterfIt.Initialize(amodifList2);

	      for(; anInterfIt.More(); anInterfIt.Next()) {
		if(anInterfIt.Value().With() == anIt1.Value().Index()) {
		  anInterfIt.Value().SetIndex(anIndexIn);
		  interffound = Standard_True;
		}
	      }
		      
	      if(!interffound)
		theIntrPool->AddInterference(anIt1.Value().Index(), nF, BooleanOperations_VertexSurface, anIndexIn);
	    }
	  }
	  // update interferences.end
	}
	break;
      }
    }
  }
  // end for(anIt1.Initialize...
  return Standard_True;
}
//=======================================================================
//function : AddInterfForAdjacentFace
//purpose  : 
//=======================================================================
void AddInterfForAdjacentFace(const Standard_Integer            theEdgeIndex,
			      const Standard_Integer            theIndexF1,
			      const Standard_Integer            theIndexF2,
			      BOPTools_PInterferencePool        theIntrPool,
			      const BooleanOperations_PShapesDataStructure& theDS) {
  Standard_Boolean bIsOnFirst = (theDS->Rank(theEdgeIndex) == 1);

  IntTools_SequenceOfPntOn2Faces aPnts;
  IntTools_SequenceOfCurves aCvs;
  Standard_Integer index1 = (bIsOnFirst) ? theIndexF1 : theIndexF2;
  Standard_Integer index2 = (bIsOnFirst) ? theIndexF2 : theIndexF1;
  Standard_Integer nbw = theDS->NumberOfAncestors(theEdgeIndex);
  Standard_Integer ancwit = 0, ancfit = 0;
  Standard_Boolean badjacentfound = Standard_False;

  for(ancwit = 1; (!badjacentfound) && (ancwit <= nbw); ancwit++) {
    Standard_Integer ancestor1 = theDS->GetAncestor(theEdgeIndex, ancwit);

    if(ancestor1 == index1)
      continue;

    if(theDS->GetShapeType(ancestor1) == TopAbs_WIRE) {
      Standard_Integer nbf = theDS->NumberOfAncestors(ancestor1);

      for(ancfit = 1; ancfit <= nbf; ancfit++) {
	Standard_Integer ancestor2 = theDS->GetAncestor(ancestor1, ancfit);

	if(ancestor2 != index1) {
	  index1 = ancestor2;
	  badjacentfound = Standard_True;
	  break;
	}
      }
    }
  }

  if(index1 > index2) {
    Standard_Integer tmp = index1;
    index1 = index2;
    index2 = tmp;
  }
  Standard_Boolean bAddInterference = Standard_True;
  Standard_Integer ffit = 0;
  BOPTools_CArray1OfSSInterference& aFFs = theIntrPool->SSInterferences();

  for(ffit = 1; ffit <= aFFs.Extent(); ffit++) {
    BOPTools_SSInterference& aFFi3 = aFFs(ffit);

    if((index1 == aFFi3.Index1()) && (index2 == aFFi3.Index2())) {
      bAddInterference = Standard_False;
    }
  }

  if(bAddInterference) {
    BOPTools_SSInterference anInterfSS (index1, index2, 1.e-07, 1.e-07, aCvs, aPnts);
    Standard_Integer anIndexInSS = aFFs.Append(anInterfSS);
    theIntrPool->AddInterference (index1, index2, BooleanOperations_SurfaceSurface, anIndexInSS); 
  }
}

//=======================================================================
//function : RejectPaveBlock
//purpose  : 
//=======================================================================
Standard_Boolean RejectPaveBlock(const IntTools_Curve& theC,
                                 const Standard_Real   theT1,
                                 const Standard_Real   theT2,
                                 const TopoDS_Vertex&  theV,
                                 Standard_Real&        theRT)
{
  Standard_Boolean bIsPeriodic, bClosed, isp, c3d;
  Standard_Real aPeriod, dt, pf, pl, dp, aTol;
  Handle(Geom_Curve) aC;
  //
  theRT = BRep_Tool::Tolerance(theV);
  aC = theC.Curve();
  //
  pf = aC->FirstParameter();
  pl = aC->LastParameter();
  dp = fabs(pl-pf);
  aTol=1.e-9;
  bIsPeriodic=aC->IsPeriodic();
  bClosed=IntTools_Tools::IsClosed(aC);
  //
  isp=Standard_False;
  if (bIsPeriodic) {
    aPeriod=aC->Period();
    isp=(fabs(aPeriod-dp) <= aTol);
    dt = fabs(theT2-theT1);
    isp = (isp ||  fabs(aPeriod-dt) <= aTol);
  }
  c3d=(bClosed || isp);
  //
  /*
  Standard_Boolean isp = (aC->IsPeriodic() && fabs(aC->Period()-dp) <= 1.e-9);
  Standard_Real dt = fabs(theT2-theT1);
  isp = (isp || (aC->IsPeriodic() && fabs(aC->Period()-dt) <= 1.e-9));
  Standard_Boolean c3d = (aC->IsClosed() || isp);
  */
  if(c3d) {
    return !c3d;
  }
  //
  Standard_Real p1, p2, tp, d3d2, aRT2;;
  //
  p1 = Max(pf,theT1);
  p2 = Min(pl,theT2);
  if(p2 != p1) {
    if(p2 < p1) {
      tp = p1; 
      p1 = p2; 
      p2 = tp;
    }
    gp_Pnt pntf, pntl;
    aC->D0(p1,pntf);
    aC->D0(p2,pntl);
    //
    aRT2=theRT*theRT;
    d3d2 = pntf.SquareDistance(pntl);
    if(d3d2 > aRT2) {
      theRT=sqrt(d3d2);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : ModifFFTol
//purpose  : 
//=======================================================================
Standard_Boolean ModifFFTol(const TopoDS_Face& theF1,
                            const TopoDS_Face& theF2,
                            Standard_Real&     theTF)
{
  Standard_Real t1 = BRep_Tool::Tolerance(theF1), t2 = BRep_Tool::Tolerance(theF2);
  theTF = 2* (t1 + t2);
  BRepAdaptor_Surface BAS1(theF1);
  BRepAdaptor_Surface BAS2(theF2);
  
  Standard_Boolean isAna1 = (BAS1.GetType() == GeomAbs_Plane ||
                             BAS1.GetType() == GeomAbs_Cylinder ||
                             BAS1.GetType() == GeomAbs_Cone ||
                             BAS1.GetType() == GeomAbs_Sphere);
  Standard_Boolean isAna2 = (BAS2.GetType() == GeomAbs_Plane ||
                             BAS2.GetType() == GeomAbs_Cylinder ||
                             BAS2.GetType() == GeomAbs_Cone ||
                             BAS2.GetType() == GeomAbs_Sphere);

  isAna1=isAna1||(BAS1.GetType() == GeomAbs_Torus);
  isAna2=isAna2||(BAS2.GetType() == GeomAbs_Torus);

  if(isAna1 && isAna2)
    return Standard_False;

  theTF = Max(theTF, 5.e-6);
  return Standard_True;
}


//=======================================================================
//function : RejectBuildingEdge
//purpose  : 
//=======================================================================
Standard_Integer RejectBuildingEdge(const IntTools_Curve& theC,
                                    const TopoDS_Vertex&  theV1,
                                    const TopoDS_Vertex&  theV2,
                                    const Standard_Real   theT1,
                                    const Standard_Real   theT2,
                                    const TopTools_ListOfShape& theL,
                                    Standard_Real&        theTF)
{
  theTF = 1.e-7;
  if(theL.Extent() == 0)
    return 0;

  Handle(Geom_Curve) aTCurve;
  Standard_Real aTT;
  
  Standard_Integer eIndex = 0;
  Standard_Boolean edgeFound = Standard_False;
  Handle(Geom_Curve) aCurve = theC.Curve();
  TopTools_ListIteratorOfListOfShape anIt(theL);
  for(; anIt.More(); anIt.Next()) {
    eIndex++;
    const TopoDS_Edge & aE = TopoDS::Edge(anIt.Value());
    if(aE.IsNull()) continue;
    TopExp_Explorer ee(aE,TopAbs_VERTEX);
    Standard_Boolean v1Found = Standard_False;
    Standard_Boolean v2Found = Standard_False;
    Standard_Real v1P = 0., v2P = 0;
    for(; ee.More(); ee.Next()) {
      const TopoDS_Vertex aV = TopoDS::Vertex(ee.Current());
      if(aV.IsNull()) continue;
      if(aV.IsEqual(theV1)) {
        v1Found = Standard_True;
        v1P = BRep_Tool::Parameter(aV,aE);
      }
      if(aV.IsEqual(theV2)) {
        v2Found = Standard_True;
        v2P = BRep_Tool::Parameter(aV,aE);
      }
    }
    Standard_Boolean sameParam = Standard_False;
    if(v1Found && v2Found) {
      if(fabs(theT1-v1P) <= 1.e-8 && fabs(theT2-v2P) <= 1.e-8)
        sameParam = Standard_True;
    }
    if(sameParam) {
      Standard_Real f,l;
      aTCurve = BRep_Tool::Curve(aE,f,l);
      aTT = BRep_Tool::Tolerance(aE);
      edgeFound = Standard_True;
    }
    if(edgeFound)
      break;
  }

  if(!edgeFound)
    return 0;

  gp_Pnt p1 = aTCurve->Value(theT1);
  gp_Pnt p2 = aCurve->Value(theT1);
  Standard_Real dpf = p1.Distance(p2);
  p1 = aTCurve->Value(theT2);
  p2 = aCurve->Value(theT2);
  Standard_Real dpl = p1.Distance(p2);
  Standard_Real dplf = fabs(dpl-dpf);
  Standard_Real dpp = Max(dpl,dpf);

  if(dplf > 1.e-7)
    return 0;

  Standard_Real maxD = Max(dpl,dpf);
  Standard_Boolean inTol = Standard_True;
  Standard_Real dp = fabs(theT2-theT1)/23.;
  Standard_Integer di = 0;
  for(di = 1; di <= 21; di++) {
    Standard_Real cp = theT1 + dp*((Standard_Real)di);
    p1 = aTCurve->Value(cp);
    p2 = aCurve->Value(cp);
    Standard_Real d12 = p1.Distance(p2);
    maxD = Max(maxD,d12);
    if(fabs(d12-dpp) > 1.e-7) {
      inTol = Standard_False;
      break;
    }
  }

  if(!inTol)
    return 0;

  theTF = maxD;
  return eIndex;
}
//=======================================================================
//function : CorrectTolR3D
//purpose  : 
//=======================================================================
void CorrectTolR3D(BOPTools_PaveFiller& aPF,
		   const BOPTools_SSInterference& aFF,
		   const TColStd_MapOfInteger& aMVStick,
		   Standard_Real& aTolR3D)
{
  Standard_Boolean bHasBounds;
  Standard_Integer i, nF[2], nV, aNbCurves;
  Standard_Real aT1, aT2, aU, aV, aT, aA, aTolV, aTolVmax;
  Standard_Real aTolR, aTolTresh, aAmin, aAmax;
  TColStd_MapIteratorOfMapOfInteger aIt;
  gp_Pnt aP, aP1, aP2;
  gp_Dir aDN[2];
  gp_Vec aVT;
  Handle(Geom_Surface) aS[2];
  Handle(Geom_Curve) aC3D;
  GeomAdaptor_Surface aGAS;
  GeomAbs_SurfaceType aType;
  TopoDS_Face aF[2];
  //
  BooleanOperations_PShapesDataStructure myDS=aPF.DS();
  const Handle(IntTools_Context)& myContext=aPF.Context();
  //
  aTolTresh=0.0005;
  aAmin=0.012;// 0.7-7 deg
  aAmax=0.12;
  //
  if (!aMVStick.Extent()) {
    return;
  }
  //
  BOPTools_SSInterference& aFFi=*((BOPTools_SSInterference*)&aFF);
  BOPTools_SequenceOfCurves& aSCvs=aFFi.Curves();
  aNbCurves=aSCvs.Length();
  if (aNbCurves!=1){
    return;
  }
  //
  aFFi.Indices(nF[0], nF[1]);
  for (i=0; i<2; ++i) {
    aF[i]=*((TopoDS_Face*)(&myDS->Shape(nF[i])));
    aS[i]=BRep_Tool::Surface(aF[i]);
    aGAS.Load(aS[i]);
    aType=aGAS.GetType();
    if (aType!=GeomAbs_BSplineSurface) {
      return;
    }
  }
  //
  BOPTools_Curve& aBC=aSCvs(1);
  const IntTools_Curve& aIC=aBC.Curve();
  bHasBounds=aIC.HasBounds();
  if (!bHasBounds){
    return;
  }
  //
  aIC.Bounds (aT1, aT2, aP1, aP2);
  aT=IntTools_Tools::IntermediatePoint(aT1, aT2);
  aC3D=aIC.Curve();
  aC3D->D0(aT, aP);
  //
  for (i=0; i<2; ++i) {
    GeomAPI_ProjectPointOnSurf& aPPS=myContext->ProjPS(aF[i]);
    aPPS.Perform(aP);
    aPPS.LowerDistanceParameters(aU, aV);
    BOPTools_Tools3D::GetNormalToSurface(aS[i], aU, aV, aDN[i]);
  }
  //
  aA=aDN[0].Angle(aDN[1]);
  aA=fabs(aA);
  if (aA>0.5*M_PI) {
    aA=M_PI-aA;
  }
  //
  if (aA<aAmin || aA>aAmax) {
    return;
  }
  //
  aTolVmax=-1.;
  aIt.Initialize(aMVStick);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Key();
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)(&myDS->Shape(nV)));
    aTolV=BRep_Tool::Tolerance(aV);
    if (aTolV>aTolVmax) {
      aTolVmax=aTolV;
    }
  }
  //
  aTolR=aTolVmax/aA;
  if (aTolR<aTolTresh) {
    aTolR3D=aTolR;
  }
}
//=======================================================================
// function: PutClosingPaveOnCurve
// purpose:
//=======================================================================
void BOPTools_PaveFiller::PutClosingPaveOnCurve(BOPTools_Curve& aBC,
						BOPTools_SSInterference& aFFi)
{
  Standard_Boolean bIsClosed, bHasBounds, bAdded;
  Standard_Integer nVC, j;
  Standard_Real aT[2], aTolR3D, aTC, dT, aTx;
  gp_Pnt aP[2] ; 
  BOPTools_Pave aPVx;
  BOPTools_ListIteratorOfListOfPave aItLP;
  //
  const IntTools_Curve& aIC=aBC.Curve();
  const Handle (Geom_Curve)& aC3D=aIC.Curve();
  if(aC3D.IsNull()) {
    return;
  }
  //
  bIsClosed=IntTools_Tools::IsClosed(aC3D);
  if (!bIsClosed) {
    return;
  }
  //
  bHasBounds=aIC.HasBounds ();
  if (!bHasBounds){
    return;
  }
  // 
  bAdded=Standard_False;
  dT=Precision::PConfusion();
  aTolR3D=aFFi.TolR3D();
  aIC.Bounds (aT[0], aT[1], aP[0], aP[1]);
  //
  BOPTools_PaveSet& aFFiPS=aFFi.NewPaveSet();
  BOPTools_PaveSet& aCPS=aBC.Set();
  //
  const BOPTools_ListOfPave& aLP=aCPS.Set();
  aItLP.Initialize(aLP);
  for (; aItLP.More() && !bAdded; aItLP.Next()) {
    const BOPTools_Pave& aPC=aItLP.Value();
    nVC=aPC.Index();
    const TopoDS_Vertex aVC=TopoDS::Vertex(myDS->Shape(nVC));
    aTC=aPC.Param();
    //
    for (j=0; j<2; ++j) {
      if (fabs(aTC-aT[j]) < dT) {
	aTx=(!j) ? aT[1] : aT[0];
	aPVx.SetIndex(nVC);
	aPVx.SetParam(aTx);
	//
	aCPS.Append(aPVx);
	aFFiPS.Append(aPVx);
	//
	bAdded=Standard_True;
	break;
      }
    }
  }
}
