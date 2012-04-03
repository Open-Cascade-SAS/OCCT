// Created on: 2001-03-07
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

#include <stdio.h>

#include <Standard_Failure.hxx>
#include <Precision.hxx>

#include <Geom_Curve.hxx>

#include <TColStd_MapOfInteger.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_SequenceOfRanges.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BooleanOperations_OnceExplorer.hxx>

#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_IteratorOfCoupleOfShape.hxx>
#include <BOPTools_ListIteratorOfListOfInterference.hxx>
#include <BOPTools_InterferenceLine.hxx>
#include <BOPTools_Interference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_VEInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <BOPTools_VSInterference.hxx>
#include <BOPTools_CArray1OfVEInterference.hxx>
#include <BOPTools_CArray1OfVVInterference.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_CArray1OfVSInterference.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_Pave.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
#include <BOPTools_ListOfPave.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_CommonBlockAPI.hxx>
#include <BOPTools_ListOfCoupleOfInteger.hxx>

#include <BRepExtrema_DistShapeShape.hxx>
#include <BOPTools_IntersectionStatus.hxx>
#include <BOPTools_HArray2OfIntersectionStatus.hxx>

#include <BOPTColStd_Failure.hxx>
#include <BOPTColStd_Dump.hxx>

//=======================================================================
// function: BOPTools_PavePoolFiller::BOPTools_PavePoolFiller
// purpose: 
//=======================================================================
BOPTools_PaveFiller::BOPTools_PaveFiller()
{
  myIsDone=Standard_False;
  myIntrPool=NULL;
  myDS=NULL;
  myNbSources=0;
  myNbEdges=0;
}

//=======================================================================
// function: BOPTools_PavePoolFiller::BOPTools_PavePoolFiller
// purpose: 
//=======================================================================
BOPTools_PaveFiller::BOPTools_PaveFiller(const BOPTools_InterferencePool& aPool)
{
  myIsDone=Standard_False;
  void* p=(void*) &aPool;
  myIntrPool=(BOPTools_InterferencePool*) p;
  myDS=myIntrPool->DS();
  myNbSources=myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myNbEdges=myDS->NbEdges();
}

//=======================================================================
// function: Constructor
// purpose: 
//=======================================================================
BOPTools_PaveFiller::BOPTools_PaveFiller
  (const BOPTools_InterferencePool&        theIP,
   const BOPTools_SSIntersectionAttribute& theSectionAttribute)
{
  myIsDone=Standard_False;
  myIntrPool = (BOPTools_PInterferencePool) &theIP;
  myDS = myIntrPool->DS();
  myNbSources=myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myNbEdges=myDS->NbEdges();
  mySectionAttribute = theSectionAttribute;
}

//=======================================================================
// function: SetInterferencePool
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::SetInterferencePool(const BOPTools_InterferencePool& aPool)
{
  myIsDone=Standard_False;
  void* p=(void*) &aPool;
  myIntrPool=(BOPTools_InterferencePool*) p;
  myDS=myIntrPool->DS();
  myNbSources=myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myNbEdges=myDS->NbEdges();
}

//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::Destroy()
{
}

//=======================================================================
// function: SetSectionAttribute
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::SetSectionAttribute
    (const BOPTools_SSIntersectionAttribute& anAtt) 
{
  mySectionAttribute=anAtt;
}
//=======================================================================
// function: SectionAttribute
// purpose: 
//=======================================================================
const BOPTools_SSIntersectionAttribute& 
  BOPTools_PaveFiller::SectionAttribute() const
{
  return mySectionAttribute;
}
//=======================================================================
// function: SetContext
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::SetContext(const Handle(IntTools_Context)& aContext) 
{
  myContext=aContext;
}
//=======================================================================
// function: Context
// purpose: 
//=======================================================================
const Handle(IntTools_Context)& BOPTools_PaveFiller::Context() const
{
  return myContext;
}
//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::Perform()
{
  try {
    //
    if (myContext.IsNull()) {
      myContext=new IntTools_Context;
    }
    //
    // 0. Prepare the IteratorOfCoupleOfShape
    myDSIt.SetDataStructure(myDS);
    //
    // 1.VV
    PerformVV();
    PerformNewVertices();
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
    
    RefinePavePool ();
    myPavePoolNew.Destroy();
    myPavePoolNew    .Resize (myNbEdges);
    //
    // 5.EF
    PreparePaveBlocks(TopAbs_EDGE, TopAbs_FACE);
    
    PerformEF();
    RefinePavePool();
    // 
    myPavePoolNew.Destroy();

    //     MakeSplitEdges ();
    //     DoSDEdges();
    //
    // 6. FF
    PerformFF ();

    PutPavesOnCurves();

    MakeSplitEdges ();
    DoSDEdges();
    
    MakeBlocks();

    MakeSectionEdges();
    //
    MakeAloneVertices();
    //
  } // end of try block

  catch (BOPTColStd_Failure& x) {
    cout << x.Message() << endl << flush;
  }

}

//=======================================================================
// function: PartialPerform
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PartialPerform
  (const TColStd_SetOfInteger& anObjSubSet,
   const TColStd_SetOfInteger& aToolSubSet)
{
  try {
    //
    if (myContext.IsNull()) {
      myContext=new IntTools_Context;
    }
    //
    //
    // 0. Prepare the IteratorOfCoupleOfShape
    myDSIt.SetDataStructure(myDS);
    //Fill TableOfIntersectionStatus
    Standard_Integer i, j;
    Standard_Integer iObjF, iObjL, iToolF, iToolL;
    myDS->ObjectRange(iObjF, iObjL);
    myDS->ToolRange(iToolF, iToolL);
    for(i = iObjF; i <= iObjL; ++i) {
      for(j = iToolF; j <= iToolL; ++j) {
	if(!anObjSubSet.Contains(i) || !aToolSubSet.Contains(j)) {
	  myDSIt.SetIntersectionStatus(i, j, BOPTools_NONINTERSECTED);
	}
      }
    }
    //
    // 1.VV
    PerformVV();
    PerformNewVertices();
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
    
    RefinePavePool ();
    myPavePoolNew.Destroy();
    myPavePoolNew    .Resize (myNbEdges);
    //
    // 5.EF
    PreparePaveBlocks(TopAbs_EDGE, TopAbs_FACE);
    
    PerformEF();
    RefinePavePool();
    // 
    myPavePoolNew.Destroy();

    //     MakeSplitEdges ();
    //     DoSDEdges();
    //
    // 6. FF
    PerformFF ();

    //
  } // end of try block

  catch (BOPTColStd_Failure& x) {
    cout << x.Message() << endl << flush;
  }

}

//=======================================================================
// function: ToCompletePerform
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::ToCompletePerform()
{
  try {
    //
    if (myContext.IsNull()) {
      myContext=new IntTools_Context;
    }
    //
    PutPavesOnCurves();

    MakeSplitEdges ();
    DoSDEdges();

    MakeBlocks();

    MakeSectionEdges();
    //
    MakeAloneVertices();
  } // end of try block

  catch (BOPTColStd_Failure& x) {
    cout << x.Message() << endl << flush;
  }
}

//=======================================================================
// function: PerformVE
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PerformVE() 
{
  myIsDone=Standard_False;
  
  Standard_Integer n1, n2, anIndexIn, aFlag, aWhat, aWith, aNbVEs, aBlockLength;
  Standard_Real aT;
  //
  BOPTools_CArray1OfVEInterference& aVEs=myIntrPool->VEInterferences();
  //
  // V/E Interferences  [BooleanOperations_VertexEdge]
  myDSIt.Initialize (TopAbs_VERTEX, TopAbs_EDGE);
  //
  //
  // BlockLength correction
  aNbVEs=ExpectedPoolLength();
  aBlockLength=aVEs.BlockLength();
  if (aNbVEs > aBlockLength) {
    aVEs.SetBlockLength(aNbVEs);
  }
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    Standard_Boolean justaddinterference = Standard_False;
    myDSIt.Current(n1, n2, justaddinterference);
    
    if(justaddinterference) {
      if (!myIntrPool->IsComputed(n1, n2) && !IsSuccesstorsComputed(n1, n2)) {
	anIndexIn = 0;
	aWhat = n1; // Vertex
	aWith = n2; // Edge
	SortTypes(aWhat, aWith);
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexEdge, anIndexIn);
      }
      continue;
    }
    //
    if (! myIntrPool->IsComputed(n1, n2)) {
      if (! IsSuccesstorsComputed(n1, n2)) {
	anIndexIn=0;
	aWhat=n1; // Vertex
	aWith=n2; // Edge
	SortTypes(aWhat, aWith);

	const TopoDS_Shape& aS1=myDS->GetShape(aWhat);
	const TopoDS_Shape& aS2=myDS->GetShape(aWith);

	const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
	const TopoDS_Edge&   aE2=TopoDS::Edge  (aS2);
	
	if (BRep_Tool::Degenerated(aE2)){
	  continue;
	}
	//
	aFlag=myContext->ComputeVE (aV1, aE2, aT);
	//
	if (!aFlag) {
	  //
	  // Add Interference to the Pool
	  BOPTools_VEInterference anInterf (aWhat, aWith, aT);
	  anIndexIn=aVEs.Append(anInterf);
	  //
	  // Add Pave to the Edge's myPavePool
	  BOPTools_Pave aPave(aWhat, aT, BooleanOperations_VertexEdge);
	  aPave.SetInterference(anIndexIn);
	  BOPTools_PaveSet& aPaveSet= myPavePool(myDS->RefEdge(aWith));
	  aPaveSet.Append(aPave);

	  //
	  // State for the Vertex in DS;
	  myDS->SetState (aWhat, BooleanOperations_ON);
	  // Insert Vertex in Interference Object
	  BOPTools_VEInterference& aVE=aVEs(anIndexIn);
	  aVE.SetNewShape(aWhat);
	}
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexEdge, anIndexIn);
	//myIntrPool->ComputeResult(n1, n2);
      }
    }
  }
  myIsDone=Standard_True;
}
//=======================================================================
// function: PerformVF
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PerformVF() 
{
  myIsDone=Standard_False;
  
  Standard_Integer n1, n2, anIndexIn, aFlag, aWhat, aWith, aNbVSs, aBlockLength;
  Standard_Real aU, aV;
  //
  BOPTools_CArray1OfVSInterference& aVSs=myIntrPool->VSInterferences();
  //
  // V/E Interferences  [BooleanOperations_VertexEdge]
  myDSIt.Initialize(TopAbs_VERTEX, TopAbs_FACE);
  //
  // BlockLength correction
  aNbVSs=ExpectedPoolLength();
  aBlockLength=aVSs.BlockLength();
  if (aNbVSs > aBlockLength) {
    aVSs.SetBlockLength(aNbVSs);
  }
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    Standard_Boolean justaddinterference = Standard_False;
    myDSIt.Current(n1, n2, justaddinterference);

    if(justaddinterference) {
      if (!myIntrPool->IsComputed(n1, n2) && !IsSuccesstorsComputed(n1, n2)) {
	anIndexIn = 0;
	aWhat = n1; // Vertex
	aWith = n2; // Face
	SortTypes(aWhat, aWith);
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexSurface, anIndexIn);
      }
      continue;
    }
    //
    if (! myIntrPool->IsComputed(n1, n2)) {
      if (! IsSuccesstorsComputed(n1, n2)) {
	anIndexIn=0;
	aWhat=n1; // Vertex
	aWith=n2; // Face
	SortTypes(aWhat, aWith);

	const TopoDS_Shape& aS1=myDS->GetShape(aWhat);
	const TopoDS_Shape& aS2=myDS->GetShape(aWith);

	const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
	const TopoDS_Face&   aF2=TopoDS::Face  (aS2);
	//
	aFlag=myContext->ComputeVS (aV1, aF2, aU, aV);
	//
	if (!aFlag) {
	  //
	  // Add Interference to the Pool
	  BOPTools_VSInterference anInterf (aWhat, aWith, aU, aV);
	  anIndexIn=aVSs.Append(anInterf);
	  //
	  // SetState for Vertex in DS;
	  myDS->SetState (aWhat, BooleanOperations_ON);
	  // Insert Vertex in Interference Object
	  BOPTools_VSInterference& aVS=aVSs(anIndexIn);
	  aVS.SetNewShape(aWhat);
	}
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexSurface, anIndexIn);
	//myIntrPool->ComputeResult(n1, n2);
      }
    }
  }
  myIsDone=Standard_True;
}

//=======================================================================
// function: PerformEE
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PerformEE() 
{
  myIsDone=Standard_False;
  
  Standard_Integer n1, n2, anIndexIn=0, nE1, nE2, aNbVEs, aBlockLength;
  Standard_Integer aTmp, aWhat, aWith;
  //
  BOPTools_CArray1OfEEInterference& aEEs=myIntrPool->EEInterferences();
  //
  // E/E Interferences  [BooleanOperations_EdgeEdge]
  myDSIt.Initialize(TopAbs_EDGE, TopAbs_EDGE);
  //
  //
  // BlockLength correction
  aNbVEs=ExpectedPoolLength();
  aBlockLength=aEEs.BlockLength();
  if (aNbVEs > aBlockLength) {
    aEEs.SetBlockLength(aNbVEs);
  }
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    Standard_Boolean justaddinterference = Standard_False;
    myDSIt.Current(n1, n2, justaddinterference);
    
    if(justaddinterference) {
      if (!myIntrPool->IsComputed(n1, n2)) {
	anIndexIn = 0;
	nE1 = n1; 
	nE2 = n2; 
	SortTypes(nE1, nE2);
	myIntrPool->AddInterference (nE1, nE2, BooleanOperations_EdgeEdge, anIndexIn);
      }
      continue;
    }
    //
    if (myIntrPool->IsComputed(n1, n2)) {
      continue;
    }
    //
    nE1=n1; 
    nE2=n2; 
    SortTypes(nE1, nE2);
    //
    Standard_Real aTolE1, aTolE2, aDeflection=0.01;
    Standard_Integer aDiscretize=30;
    TopoDS_Edge aE1, aE2;
    //
    aE1=TopoDS::Edge(myDS->GetShape(nE1));
    aE2=TopoDS::Edge(myDS->GetShape(nE2));
    //
    if (BRep_Tool::Degenerated(aE1)){
      continue;
    }
    if (BRep_Tool::Degenerated(aE2)){
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
	  Standard_Integer i, aNbCPrts;
	  aNbCPrts=aCPrts.Length();
	  //
	  if(aNbCPrts != 0) {
	    char buf[512];

	    if(!aShrunkRange1.IsDone()) {
	      sprintf (buf, "Can not obtain ShrunkRange for Edge %d", nE1);
	      throw BOPTColStd_Failure(buf) ;
	    }

	    if(!aShrunkRange2.IsDone()) {
	      sprintf (buf, "Can not obtain ShrunkRange for Edge %d", nE2);
	      throw BOPTColStd_Failure(buf) ;
	    }
	  }
	  //
	  for (i=1; i<=aNbCPrts; i++) {
	    const IntTools_CommonPrt& aCPart=aCPrts(i);
	    //
	    anIndexIn=0;
	    //
	    TopAbs_ShapeEnum aType=aCPart.Type();
	    switch (aType) {
	      
  	      case TopAbs_VERTEX:  {
		
		Standard_Real aT1, aT2; 
		Standard_Integer aNewShape;
		
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

		TopoDS_Vertex aNewVertex;
		BOPTools_Tools::MakeNewVertex(aEWhat, aT1, aEWith, aT2, aNewVertex);
		// 
		//decide to add pave or not
		Standard_Real aTolerance = Precision::PConfusion();
		IntTools_Range aRange = (aEE.Order()) ? anewSR2 : anewSR1;
		Standard_Boolean firstisonpave1  = (Abs(aRange.First() - aT1) < aTolerance);
		if(!firstisonpave1) firstisonpave1 = (Abs(aRange.First() - aR1.First()) < aTolerance);

		Standard_Boolean firstisonpave2  = (Abs(aRange.Last()  - aT1) < aTolerance);
		if(!firstisonpave2) firstisonpave2 = (Abs(aRange.Last()  - aR1.Last()) < aTolerance);

		aRange = (aEE.Order()) ? anewSR1 : anewSR2;
		Standard_Boolean secondisonpave1 = (Abs(aRange.First() - aT2) < aTolerance);
		if(!secondisonpave1) secondisonpave1 = (Abs(aRange.First() - aR2.First()) < aTolerance);

		Standard_Boolean secondisonpave2 = (Abs(aRange.Last()  - aT2) < aTolerance);
		if(!secondisonpave2) secondisonpave2 = (Abs(aRange.Last()  - aR2.Last()) < aTolerance);
		
		if(firstisonpave1 ||
		   firstisonpave2 ||
		   secondisonpave1 ||
		   secondisonpave2) {
		  //
		  myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		  continue;
		}
		// 
		else {
		  Standard_Integer eit = 0;
		  Standard_Boolean bisoldvertex = Standard_False;
		  BOPTools_PaveBlock* aPPB1 = (aEE.Order()) ? (BOPTools_PaveBlock*)&aPB2 : (BOPTools_PaveBlock*)&aPB1;
		  BOPTools_PaveBlock* aPPB2 = (aEE.Order()) ? (BOPTools_PaveBlock*)&aPB1 : (BOPTools_PaveBlock*)&aPB2;

		  for(eit = 0; eit < 2; eit++) {
		    if(aEE.Order())
		      aRange = (eit == 0) ? anewSR2 : anewSR1;
		    else
		      aRange = (eit == 0) ? anewSR1 : anewSR2;
		    const TopoDS_Edge& aE = (eit == 0) ? aEWhat : aEWith;
		    
		    BOPTools_PaveBlock* aPB = (eit == 0) ? aPPB1 : aPPB2;
		    TopoDS_Vertex aV1;
		    TopoDS_Vertex aV2;
		    Standard_Boolean bisfirst = Standard_False;
		    Standard_Real aT = (eit == 0) ? aT1 : aT2;
		    Standard_Real adist1 = fabs(aRange.First() - aT);
		    Standard_Real adist2 = fabs(aRange.Last() - aT);
		    bisfirst = (adist1 < adist2);
		    IntTools_Range aRangeCur;

		    if(bisfirst) {
		      aV2 = aNewVertex;
		      aV1 = TopoDS::Vertex(myDS->Shape(aPB->Pave1().Index()));
		      aRangeCur = IntTools_Range(aPB->Pave1().Param(), aT);
		    }
		    else {
		      aV1 = aNewVertex;
		      aV2 = TopoDS::Vertex(myDS->Shape(aPB->Pave2().Index()));
		      aRangeCur = IntTools_Range(aT, aPB->Pave2().Param());
		    }
		    Standard_Real aroughtoler = BRep_Tool::Tolerance(aV1) + BRep_Tool::Tolerance(aV2);
		    aroughtoler *=10.;

		    if((adist1 > aroughtoler) && (adist2 > aroughtoler))
		       continue;
		    IntTools_ShrunkRange aSR (aE, aV1, aV2, aRangeCur, myContext);

		    if (!aSR.IsDone()) {
		      bisoldvertex = Standard_True;
		      break;
		    }
		  }

		  if(bisoldvertex) {
		    myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		    continue;
		  }
		}
		//
		// Add Interference to the Pool
		BOPTools_EEInterference anInterf (aWhat, aWith, aCPart);
		anIndexIn=aEEs.Append(anInterf);
		myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		//
		// Insert New Vertex in DS;
		// aNewShape is # of DS-line, where aNewVertex is kept
		BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq; 
		myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
		aNewShape=myDS->NumberOfInsertedShapes();
		// State of the new Vertex is ON
		myDS->SetState (aNewShape, BooleanOperations_ON);
		//
		// Insert New Vertex in EE Interference
		BOPTools_EEInterference& aEEInterf= aEEs(anIndexIn);
		aEEInterf.SetNewShape(aNewShape);
		//
		// Add Paves to the myPavePoolNew
		BOPTools_Pave aPave;
		aPave.SetInterference(anIndexIn);
		aPave.SetType (BooleanOperations_EdgeEdge);
		aPave.SetIndex(aNewShape);
		
		aPave.SetParam(aT1);
		BOPTools_PaveSet& aPaveSet1=myPavePoolNew(myDS->RefEdge(aWhat));
		aPaveSet1.Append(aPave);
		
		aPave.SetParam(aT2);
		BOPTools_PaveSet& aPaveSet2=myPavePoolNew(myDS->RefEdge(aWith));
		aPaveSet2.Append(aPave);
	      }
	      break;

	      case TopAbs_EDGE: {
	      
		const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
		Standard_Integer aNbComPrt2=aRanges2.Length();
		
		if (aNbComPrt2>1) {
		  //
		  myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		  break;
		}

		Standard_Boolean aCoinsideFlag;
		//
		aCoinsideFlag=IsBlocksCoinside(aPB1, aPB2);
		//
		if (!aCoinsideFlag) {
		  //
		  myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		  break;
		}
		//
		// Add Interference to the Pool
		BOPTools_EEInterference anInterf (aWhat, aWith, aCPart);
		anIndexIn=aEEs.Append(anInterf);
		myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		//
		BOPTools_CommonBlock aCB;
		if (aTolE1>=aTolE2) {
		  aCB.SetPaveBlock1(aPB1);
		  aCB.SetPaveBlock2(aPB2);
		}
		else {
		  aCB.SetPaveBlock1(aPB2);
		  aCB.SetPaveBlock2(aPB1);
		}
		BOPTools_ListOfCommonBlock& aLCB1=myCommonBlockPool(myDS->RefEdge(aWhat));
		aLCB1.Append(aCB);
		BOPTools_ListOfCommonBlock& aLCB2=myCommonBlockPool(myDS->RefEdge(aWith));
		aLCB2.Append(aCB);
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
// function: MakeSplitEdges
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::MakeSplitEdges()
{
  myIsDone=Standard_False;

  Standard_Integer i, nV1, nV2, aNbPaveBlocks, aNewShapeIndex;;
  Standard_Real    t1, t2;
  TopoDS_Edge aE, aESplit;
  TopoDS_Vertex aV1, aV2;

  for (i=1; i<=myNbSources; i++) {

    if (myDS->GetShapeType(i) != TopAbs_EDGE)
      continue;
    //
    // Original Edge
    aE=TopoDS::Edge(myDS->GetShape(i));
    //
    TopoDS_Edge anEdgeOriginal=aE;
    TopAbs_Orientation anOrientationOriginal=anEdgeOriginal.Orientation(); 
    //
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    //
    aE.Orientation(TopAbs_FORWARD);
    //
    // Making Split Edges
    //
    // Split Set for the Original Edge i
    BOPTools_ListOfPaveBlock& aSplitEdges=mySplitShapesPool(myDS->RefEdge(i));
    BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);

    aNbPaveBlocks=aSplitEdges.Extent();
    
    if (aNbPaveBlocks==1) {
      Standard_Boolean IsNewVertex1, IsNewVertex2;
      // the split is equal to the original Edge
      if (aPBIt.More()) {
	BOPTools_PaveBlock& aPB1=aPBIt.Value();
	
	// 1
	const BOPTools_Pave& aPave1=aPB1.Pave1();
	nV1=aPave1.Index();
	t1=aPave1.Param();
	aV1=TopoDS::Vertex(myDS->GetShape(nV1));
	aV1.Orientation(TopAbs_FORWARD);
	// 2
	const BOPTools_Pave& aPave2=aPB1.Pave2();
	nV2=aPave2.Index();
	t2=aPave2.Param();
	aV2=TopoDS::Vertex(myDS->GetShape(nV2));
	aV2.Orientation(TopAbs_REVERSED);
	// 3
	IsNewVertex1=myDS->IsNewShape (nV1);
	IsNewVertex2=myDS->IsNewShape (nV2);
	
	if (IsNewVertex1 || IsNewVertex2) {
	  
	  BOPTools_Tools::MakeSplitEdge(aE, aV1, t1, aV2, t2, aESplit);  
	  BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
	    
	  anASSeq.SetNewSuccessor(nV1);
	  anASSeq.SetNewOrientation(aV1.Orientation());
	    
	  anASSeq.SetNewSuccessor(nV2);
	  anASSeq.SetNewOrientation(aV2.Orientation());
	  //
	  if (anOrientationOriginal==TopAbs_INTERNAL) {
	    anASSeq.SetNewAncestor(i);
	    aESplit.Orientation(anOrientationOriginal);
	  }
	  //
	  myDS->InsertShapeAndAncestorsSuccessors(aESplit, anASSeq);
	  aNewShapeIndex=myDS->NumberOfInsertedShapes();
	  myDS->SetState(aNewShapeIndex, BooleanOperations_UNKNOWN);
	  //
	  // Fill Split Set for the Original Edge
	  aPB1.SetEdge(aNewShapeIndex); 
	}
	
	else {
	  aPB1.SetEdge(i);
	}
	//
	continue;
      }
    } // if (aNbPaveBlocks==1) 

    for (; aPBIt.More(); aPBIt.Next()) {
      BOPTools_PaveBlock& aPB=aPBIt.Value();
      
      const BOPTools_Pave& aPave1=aPB.Pave1();
      nV1=aPave1.Index();
      t1=aPave1.Param();
      aV1=TopoDS::Vertex(myDS->GetShape(nV1));
      aV1.Orientation(TopAbs_FORWARD);

      const BOPTools_Pave& aPave2=aPB.Pave2();
      nV2=aPave2.Index();
      t2=aPave2.Param();
      aV2=TopoDS::Vertex(myDS->GetShape(nV2));
      aV2.Orientation(TopAbs_REVERSED);
      
      BOPTools_Tools::MakeSplitEdge(aE, aV1, t1, aV2, t2, aESplit);  
      //
      // Add Split Part of the Original Edge to the DS
      BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;

      anASSeq.SetNewSuccessor(nV1);
      anASSeq.SetNewOrientation(aV1.Orientation());

      anASSeq.SetNewSuccessor(nV2);
      anASSeq.SetNewOrientation(aV2.Orientation());
      //
      if (anOrientationOriginal==TopAbs_INTERNAL) {
	anASSeq.SetNewAncestor(i);
	aESplit.Orientation(anOrientationOriginal);
      }
      //
      myDS->InsertShapeAndAncestorsSuccessors(aESplit, anASSeq);
      aNewShapeIndex=myDS->NumberOfInsertedShapes();
      myDS->SetState(aNewShapeIndex, BooleanOperations_UNKNOWN);
      //
      // Fill Split Set for the Original Edge
      aPB.SetEdge(aNewShapeIndex);
      //
    }

  } //for (i=1; i<=myNbSources; i++) {
  myIsDone=Standard_True;
}
//=======================================================================
// function: PreparePaveBlocks
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PreparePaveBlocks(const TopAbs_ShapeEnum aType1, 
					    const TopAbs_ShapeEnum aType2)
{
  myIsDone=Standard_False;

  Standard_Boolean Ok1, Ok2, Ok3;
  Ok1= (aType1==TopAbs_VERTEX) &&  (aType2==TopAbs_EDGE) ;
  Ok2= (aType1==TopAbs_EDGE)   &&  (aType2==TopAbs_EDGE) ;
  Ok3= (aType1==TopAbs_EDGE)   &&  (aType2==TopAbs_FACE) ;
  if (!Ok1 && !Ok2 && !Ok3) {
    // error: Type mismatch
    return;
  }

  Standard_Integer n1, n2, nE1, nE2, aNbSplits;
  TColStd_MapOfInteger aMap;
  
  myDSIt.Initialize(aType1, aType2);

  for (; myDSIt.More(); myDSIt.Next()) {
    Standard_Boolean aFlag = Standard_False;
    myDSIt.Current(n1, n2, aFlag);
    nE1=n1; 
    nE2=n2; 
    SortTypes(nE1, nE2);

    if (aType1==TopAbs_EDGE) {
      BOPTools_ListOfPaveBlock& aLPB1=mySplitShapesPool(myDS->RefEdge(nE1));
      aNbSplits=aLPB1.Extent();
      if (!aNbSplits) {
	if (!aMap.Contains(nE1)) { 
	  aMap.Add(nE1);
	  PreparePaveBlocks(nE1);
	  
	  if (!myIsDone) {
	    return;
	  }
	}
      }
    }
    
    if (aType2==TopAbs_EDGE) {
      BOPTools_ListOfPaveBlock& aLPB2=mySplitShapesPool(myDS->RefEdge(nE2));
      aNbSplits=aLPB2.Extent();
      if (!aNbSplits) {
	if (!aMap.Contains(nE2)) { 
	  aMap.Add(nE2);
	  PreparePaveBlocks(nE2);
	
	  if (!myIsDone) {
	    return;
	  }
	}
      }
    }// if (aType2==TopAbs_EDGE)
  }// for (; myDSIt.More(); myDSIt.Next()) 

  myIsDone=Standard_True;
}

//=======================================================================
// function: PreparePaveBlocks
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PreparePaveBlocks(const Standard_Integer nE)
{
  myIsDone=Standard_False;
  
  Standard_Integer nV1, nV2;

  TopoDS_Edge aE;
  TopoDS_Vertex aV1, aV2;
    
  // SplitShapesPool
  BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE));
  aLPB.Clear();
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
	Standard_Boolean bThrow = Standard_True;
	Standard_Integer iRank = myDS->Rank(nE);
	TopoDS_Shape aRef = (iRank == 1) ? myDS->Tool() : myDS->Object();
	Standard_Integer aRefIndex = (iRank == 1) ? myDS->ShapeIndex(aRef, 2) : myDS->ShapeIndex(aRef, 1);

	Standard_Boolean bCheckDistance = Standard_True;

	if(myDS->IsNewShape(nV1) || myDS->IsNewShape(nV2)) {
	  bCheckDistance = Standard_False;
	}
	else {
	  const BOPTools_CArray1OfInterferenceLine& aTable = myIntrPool->InterferenceTable();
	  Standard_Integer tmpIt = 0;

	  for(tmpIt = 0; tmpIt < 3; tmpIt++) {
	    Standard_Integer acurindex = (tmpIt == 0) ? nE : ((tmpIt == 1) ? nV1 : nV2);
	    const BOPTools_InterferenceLine& anInterfLine =  aTable(acurindex);

	    if(!anInterfLine.RealList().IsEmpty())
	      bCheckDistance = Standard_False;
	  }
	}

	if(bCheckDistance) {
	  BRepExtrema_DistShapeShape aDist;
	  Standard_Integer bRefLoaded = Standard_False;

	  Standard_Boolean bVertexIsOnShape = Standard_False;
	  Standard_Integer ii = 0, jj = 0;

	  for(jj = 0; !bVertexIsOnShape && (jj < 2); jj++) {
	    Standard_Integer currentNV = (jj == 0) ? nV1 : nV2;

	    Standard_Integer aVertexRank = myDS->Rank(currentNV);

	    if(aVertexRank != iRank) {
	      bVertexIsOnShape = Standard_True;
	      break;
	    }
	    BOPTools_IntersectionStatus aStatus = BOPTools_UNKNOWN;

	    if(aVertexRank == 1)
	      aStatus = myDSIt.GetTableOfIntersectionStatus()->Value(currentNV, aRefIndex);
	    else
	      aStatus = myDSIt.GetTableOfIntersectionStatus()->Value(aRefIndex, currentNV);
	    
	    if(aStatus == BOPTools_NONINTERSECTED) {
	      continue;
	    }

	    if(jj == 0) {
	      aDist.LoadS1(aV1);

	      if(!bRefLoaded)
		aDist.LoadS2(aRef);
	      bRefLoaded = Standard_True;
	    }
	    else {
	      aDist.LoadS1(aV2);

	      if(!bRefLoaded)
		aDist.LoadS2(aRef);
	      bRefLoaded = Standard_True;
	    }
	    aDist.Perform();

	    if(aDist.IsDone()) {

	      for(ii = 1; ii <= aDist.NbSolution(); ii++) {
		Standard_Real aTolerance = (jj == 0) ? BRep_Tool::Tolerance(aV1) : BRep_Tool::Tolerance(aV2);
		TopoDS_Shape aSupportShape = aDist.SupportOnShape2(ii);

		switch(aSupportShape.ShapeType()) {
		case TopAbs_VERTEX: {
		  aTolerance += BRep_Tool::Tolerance(TopoDS::Vertex(aSupportShape));
		  break;
		}
		case TopAbs_EDGE: {
		  aTolerance += BRep_Tool::Tolerance(TopoDS::Edge(aSupportShape));
		  break;
		}
		case TopAbs_FACE: {
		  aTolerance += BRep_Tool::Tolerance(TopoDS::Face(aSupportShape));
		  break;
		}
		default:
		  break;
		}

		if(aDist.Value() < aTolerance) {
		  bVertexIsOnShape = Standard_True;
		  break;
		}
	      }
	    }
	  }

	  if(!bVertexIsOnShape) {
	    aSR.SetShrunkRange(aRange);
	    bThrow = Standard_False;
	  }
	}

	if(bThrow) {

	  sprintf (buf, "Can not obtain ShrunkRange for Edge %d\n", nE);
	  BOPTColStd_Dump::PrintMessage(buf);
	  sprintf (buf, "Can not obtain ShrunkRange for Edge %d", nE);
	  throw 
	    BOPTColStd_Failure(buf) ;
	}
      }
      //
      if (anErrorStatus==6) {
	sprintf(buf,
		"Warning: [PreparePaveBlocks()] Max.Dummy Shrunk Range for Edge %d\n", nE);
	BOPTColStd_Dump::PrintMessage(buf);
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
// function: CorrectShrunkRanges
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::CorrectShrunkRanges(const Standard_Integer aSide,
					      const BOPTools_Pave& aPave,
					      IntTools_ShrunkRange& aShrunkRange)
{
  BooleanOperations_KindOfInterference aType;
  
  aType=aPave.Type();
  if (aType!=BooleanOperations_EdgeEdge) {
    return;
  }

  Standard_Integer anIndexInterf ;
  anIndexInterf=aPave.Interference();
  BOPTools_CArray1OfEEInterference& aEEs=myIntrPool->EEInterferences();
  const BOPTools_EEInterference& aEE=aEEs(anIndexInterf);
  const IntTools_CommonPrt& aCP=aEE.CommonPrt();
  const TopoDS_Edge& aE1=aCP.Edge1();
  const TopoDS_Edge& aE2=aCP.Edge2();

  const IntTools_Range& aSR=aShrunkRange.ShrunkRange();
  const TopoDS_Edge& aE=aShrunkRange.Edge();
 
  IntTools_Range aNewRange;
  IntTools_Range aCPRange;

  if (aE1.IsSame(aE)) {
    const IntTools_Range& aR1=aCP.Range1();
    aCPRange=aR1;
  }
  if (aE2.IsSame(aE)) {
    const IntTools_SequenceOfRanges& aSeqR=aCP.Ranges2();
    const IntTools_Range& aR2=aSeqR(1);
     aCPRange=aR2;
  }
  

  Standard_Real aCoeff=1.05, tV, tNV, t2, t1;
  tV=aPave.Param();
  if (aSide==0) { // Left
    if (aCPRange.Last() > aSR.First()) {
      tNV=aCPRange.Last();
      tNV=tV+aCoeff*(tNV-tV);
      aNewRange.SetFirst(tNV);
      t2=aSR.Last();
      aNewRange.SetLast (t2);
      if(tNV < t2) {
      //if(aNewRange.First() > aNewRange.Last()) {
	aShrunkRange.SetShrunkRange(aNewRange);
      }
    }
  }
  else { // Right
    if (aCPRange.First() < aSR.Last()) {
      tNV=aCPRange.First();
      tNV=tV-aCoeff*(tV-tNV);
      t1=aSR.First();
      aNewRange.SetFirst(t1);
      aNewRange.SetLast (tNV);
      if(tNV > t1) {
      //if(aNewRange.First() < aNewRange.Last()) {
	aShrunkRange.SetShrunkRange(aNewRange);
      }
    }
  }
}

//=======================================================================
// function: RefinePavePool
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::RefinePavePool()
{
  Standard_Integer  i, aNbNew;

  for (i=1; i<=myNbSources; i++) {

    if ((myDS->GetShape(i)).ShapeType()==TopAbs_EDGE) {
      BOPTools_PaveSet& aPS= myPavePool(myDS->RefEdge(i));
      //ZZ BOPTools_ListOfPave& aLP=aPS.ChangeSet();
      
      BOPTools_PaveSet& aNewPS= myPavePoolNew(myDS->RefEdge(i));
      BOPTools_ListOfPave& aNewLP=aNewPS.ChangeSet();

      aNbNew=aNewLP.Extent();
      if (aNbNew) {
	BOPTools_ListIteratorOfListOfPave anIt(aNewLP);
	for (; anIt.More(); anIt.Next()) {
	  const BOPTools_Pave& aPave=anIt.Value();
	  aPS.Append(aPave);
	}
	// Clear the ListOfPaveBlock
	BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(i));
	aLPB.Clear();
	// Prepare the paveBlocks for that egde again
	PreparePaveBlocks(i);
      }
      aNewLP.Clear();
    }
  }
}

//=======================================================================
// function: PrepareEdges
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PrepareEdges() 
{
  Standard_Integer  i, nV, ii, aNBSuc, ip;
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
	ip=FindSDVertex(nV);
	if (ip) {
	  aV=TopoDS::Vertex(myDS->GetShape(ip));
	  aV.Orientation(anOr);
	  nV=ip;
	}
	//
	BOPTools_Pave aPave(nV, aT); 
	aPaveSet.Append (aPave);
      }
    }
  }
}
//=======================================================================
// function: PerformVV
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PerformVV() 
{
  myIsDone=Standard_False;
 
  Standard_Integer n1, n2,anIndexIn, aFlag, aWhat, aWith, aNbVVs, aBlockLength;
  //
  BOPTools_CArray1OfVVInterference& aVVs=myIntrPool->VVInterferences();
  //
  // V/V  BooleanOperations_VertexVertex
  myDSIt.Initialize(TopAbs_VERTEX, TopAbs_VERTEX);
  //
  //
  // BlockLength correction
  aNbVVs=ExpectedPoolLength();
  aBlockLength=aVVs.BlockLength();
  if (aNbVVs > aBlockLength) {
    aVVs.SetBlockLength(aNbVVs);
  }
  //
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    Standard_Boolean justaddinterference = Standard_False;
    myDSIt.Current(n1, n2, justaddinterference);

    if(justaddinterference) {
      if (! myIntrPool->IsComputed(n1, n2)) {
	anIndexIn=0;
	aWhat=n1;
	aWith=n2;
	SortTypes(aWhat, aWith);
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexVertex, anIndexIn);
      }
      continue;
    }
    //
    if (! myIntrPool->IsComputed(n1, n2)) {
      anIndexIn=0;
      aWhat=n1;
      aWith=n2;
      SortTypes(aWhat, aWith);
      const TopoDS_Shape& aS1=myDS->GetShape(aWhat);
      const TopoDS_Shape& aS2=myDS->GetShape(aWith);
      
      const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
      const TopoDS_Vertex& aV2=TopoDS::Vertex(aS2);
      aFlag=IntTools_Tools::ComputeVV (aV1, aV2);
    
      if (!aFlag) {
	BOPTools_VVInterference anInterf (aWhat, aWith);
	anIndexIn=aVVs.Append(anInterf);
      }
      myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexVertex, anIndexIn);
      //myIntrPool->ComputeResult(n1, n2);
    }
  }

  myIsDone=Standard_True;
}
//=======================================================================
// function: PerformNewVertices
// purpose: 
//=======================================================================
void BOPTools_PaveFiller::PerformNewVertices() 
{
  myIsDone=Standard_False;

  Standard_Integer i, aNb, anIndex1, anIndex2, aNewShape;
  TopoDS_Vertex aV1, aV2, aNewVertex;
  BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
  //
  // 1. VV Interferences
  BOPTools_CArray1OfVVInterference& VVs=myIntrPool->VVInterferences();
  aNb=VVs.Extent();
  for (i=1; i<=aNb; i++) {
    BOPTools_VVInterference& VV=VVs(i);
    anIndex1=VV.Index1();
    anIndex2=VV.Index2();
    //
    // Make New Vertex
    aV1=TopoDS::Vertex(myDS->GetShape(anIndex1));
    aV2=TopoDS::Vertex(myDS->GetShape(anIndex2));
    BOPTools_Tools::MakeNewVertex(aV1, aV2, aNewVertex);
    //
    // Insert New Vertex in DS;
    // aNewShape is # of DS-line, where aNewVertex is kept
    myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
    aNewShape=myDS->NumberOfInsertedShapes();
    // State of New Vertex is ON
    myDS->SetState (aNewShape, BooleanOperations_ON);
    // Insert New Vertex in Interference
    VV.SetNewShape(aNewShape);
  }

  myIsDone=Standard_True;
}
//=======================================================================
// function: FindSDVertex
// purpose: 
//=======================================================================
Standard_Integer BOPTools_PaveFiller::FindSDVertex
  (const Standard_Integer nV)const
{
  Standard_Integer i, aNb, anIndex1, anIndex2, aNewShape=0;

  BOPTools_CArray1OfVVInterference& VVs=myIntrPool->VVInterferences();
  aNb=VVs.Extent();
  
  for (i=1; i<=aNb; i++) {
    const BOPTools_VVInterference& VV=VVs(i);
    anIndex1=VV.Index1();
    anIndex2=VV.Index2();
    if (nV==anIndex1 || nV==anIndex2) {
      aNewShape=VV.NewShape();
      return aNewShape;
    }
  }
  return aNewShape;
}

//=======================================================================
// function:IsSuccesstorsComputed
// purpose: 
//=======================================================================
Standard_Boolean BOPTools_PaveFiller::IsSuccesstorsComputed
  (const Standard_Integer aN1,
   const Standard_Integer aN2)const
{
  Standard_Integer nSuc, n1, n2;

  BooleanOperations_OnceExplorer aExp(*myDS);
  TopAbs_ShapeEnum aType=myDS->GetShapeType(aN1);

  n1=aN1;
  n2=aN2;

  if (aType!=TopAbs_VERTEX) {
    Standard_Integer ntmp=n1;
    n1=n2;
    n2=ntmp;
  }

  aType=myDS->GetShapeType(n2);
  if (aType==TopAbs_EDGE) {
    aExp.Init(n2, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }
  return Standard_False;
  }

  else if (aType==TopAbs_FACE) {
    aExp.Init(n2, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }

    aExp.Init(n2, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }
    return Standard_False;
  }

  return Standard_False;
}

//=======================================================================
//function : SortTypes
//purpose  : 
//=======================================================================
void BOPTools_PaveFiller::SortTypes(Standard_Integer& theWhat,
				    Standard_Integer& theWith)const 
{ 
  Standard_Boolean aReverseFlag=Standard_True;

  TopAbs_ShapeEnum aType1= myDS->GetShapeType(theWhat),
                   aType2= myDS->GetShapeType(theWith);
  
  if (aType1==aType2)
    return;
  
  if (aType1==TopAbs_EDGE && aType2==TopAbs_FACE){
    aReverseFlag=Standard_False;
  }

  if (aType1==TopAbs_VERTEX && 
      (aType2==TopAbs_FACE || aType2==TopAbs_EDGE)) {
    aReverseFlag=Standard_False;
  }
  
  Standard_Integer aWhat, aWith;
  aWhat=(aReverseFlag) ? theWith : theWhat;
  aWith=(aReverseFlag) ? theWhat : theWith;
  
  theWhat=aWhat;
  theWith=aWith;
}

//=======================================================================
// function:IsDone
// purpose: 
//=======================================================================
Standard_Boolean BOPTools_PaveFiller::IsDone() const
{
  return myIsDone;
}

//=======================================================================
// function: PavePool
// purpose: 
//=======================================================================
const BOPTools_PavePool& BOPTools_PaveFiller::PavePool() const
{
  return myPavePool;
}
//=======================================================================
// function: ChangePavePool
// purpose: 
//=======================================================================
BOPTools_PavePool& BOPTools_PaveFiller::ChangePavePool()
{
  return myPavePool;
}

//=======================================================================
// function:  CommonBlockPool
// purpose: 
//=======================================================================
const BOPTools_CommonBlockPool& BOPTools_PaveFiller::CommonBlockPool() const
{
  return myCommonBlockPool;
}
//=======================================================================
// function:  ChangeCommonBlockPool
// purpose: 
//=======================================================================
BOPTools_CommonBlockPool& BOPTools_PaveFiller::ChangeCommonBlockPool()
{
  return myCommonBlockPool;
}
//=======================================================================
// function:  SplitShapesPool
// purpose: 
//=======================================================================
const BOPTools_SplitShapesPool& BOPTools_PaveFiller::SplitShapesPool() const
{
  return mySplitShapesPool;
}

//=======================================================================
// function:  ChangeSplitShapesPool
// purpose: 
//=======================================================================
BOPTools_SplitShapesPool& BOPTools_PaveFiller::ChangeSplitShapesPool()
{
  return mySplitShapesPool;
}
//=======================================================================
// function:  DS
// purpose: 
//=======================================================================
BooleanOperations_PShapesDataStructure BOPTools_PaveFiller::DS()
{
  return myDS;
}
//=======================================================================
// function:  InterfPool
// purpose: 
//=======================================================================
BOPTools_PInterferencePool BOPTools_PaveFiller::InterfPool()
{
  return myIntrPool;
}

//
//=======================================================================
// function:  IteratorOfCoupleOfShape
// purpose: 
//=======================================================================
const BOPTools_IteratorOfCoupleOfShape& 
  BOPTools_PaveFiller::IteratorOfCoupleOfShape() const
{
  return myDSIt;
}
//
//=======================================================================
// function:  ExpectedPoolLength
// purpose: 
//=======================================================================
Standard_Integer BOPTools_PaveFiller::ExpectedPoolLength()const
{
  Standard_Integer aNbIIs;
  Standard_Real aCfPredict=.5;

  const BOPTools_ListOfCoupleOfInteger& aLC=myDSIt.ListOfCouple();
  aNbIIs=aLC.Extent();
  //
  if (aNbIIs==1) {
    return aNbIIs;
  }
  //
  aNbIIs=(Standard_Integer) (aCfPredict*(Standard_Real)aNbIIs);
  
  return aNbIIs;
}
//
//=======================================================================
// function:  IsBlocksCoinside
// purpose: 
//=======================================================================
Standard_Boolean 
  BOPTools_PaveFiller::IsBlocksCoinside(const BOPTools_PaveBlock& aPB1,
					const BOPTools_PaveBlock& aPB2) const
{
  Standard_Boolean bRetFlag=Standard_True;
  Standard_Real aTolV11, aTolV12, aTolV21, aTolV22;
  Standard_Real d1121, d1122, d1222, d1221, aTolSum, aCoeff=1.05;
  gp_Pnt aP11, aP12, aP21, aP22;

  const TopoDS_Vertex& aV11=TopoDS::Vertex(myDS->Shape(aPB1.Pave1().Index()));
  const TopoDS_Vertex& aV12=TopoDS::Vertex(myDS->Shape(aPB1.Pave2().Index()));
  const TopoDS_Vertex& aV21=TopoDS::Vertex(myDS->Shape(aPB2.Pave1().Index()));
  const TopoDS_Vertex& aV22=TopoDS::Vertex(myDS->Shape(aPB2.Pave2().Index()));

  aTolV11=BRep_Tool::Tolerance(aV11);
  aTolV12=BRep_Tool::Tolerance(aV12);
  aTolV21=BRep_Tool::Tolerance(aV21);
  aTolV22=BRep_Tool::Tolerance(aV22);
  
  aP11=BRep_Tool::Pnt(aV11);
  aP12=BRep_Tool::Pnt(aV12);
  aP21=BRep_Tool::Pnt(aV21);
  aP22=BRep_Tool::Pnt(aV22);

  d1121=aP11.Distance(aP21);
  aTolSum=aCoeff*(aTolV11+aTolV21);
  if (d1121<aTolSum) {
    d1222=aP12.Distance(aP22);
    aTolSum=aCoeff*(aTolV12+aTolV22);
    if (d1222<aTolSum) {
      return bRetFlag;
    }
  }
  //
  d1122=aP11.Distance(aP22);
  aTolSum=aCoeff*(aTolV11+aTolV22);
  if (d1122<aTolSum) {
    d1221=aP12.Distance(aP21);
    aTolSum=aCoeff*(aTolV12+aTolV21);
    if (d1221<aTolSum) {
      return bRetFlag;
    }
  }
  return !bRetFlag;
}
