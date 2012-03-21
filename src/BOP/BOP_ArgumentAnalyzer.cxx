// Created on: 2004-09-02
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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


#include <BOP_ArgumentAnalyzer.ixx>
#include <BOP_CheckResult.hxx>
#include <BOP_ShellSolid.hxx>
#include <BOP_WireSolid.hxx>
#include <BOP_WireShell.hxx>
#include <BOP_WireEdgeSet.hxx>
#include <BOP_FaceBuilder.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <TColStd_Array2OfBoolean.hxx>

#include <IntTools_Context.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_CommonPrt.hxx>

#include <BOPTools_DSFiller.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_Checker.hxx>
#include <BOPTools_CheckResult.hxx>
#include <BOPTools_ListOfCheckResults.hxx>
#include <BOPTools_ListIteratorOfListOfCheckResults.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

static Standard_Boolean TestShapeType(const TopoDS_Shape & TheShape);

static Standard_Boolean CheckEdge(const TopoDS_Edge& theEdge);

static Standard_Boolean TestSubShapeType(const TopAbs_ShapeEnum theT1,
                                         const TopAbs_ShapeEnum theT2,
                                         const BOP_Operation    theOP);

// ================================================================================
// function: Constructor
// purpose:
// ================================================================================
BOP_ArgumentAnalyzer::BOP_ArgumentAnalyzer() : 
myStopOnFirst(Standard_False),
myOperation(BOP_UNKNOWN),
myArgumentTypeMode(Standard_False),
mySelfInterMode(Standard_False),
mySmallEdgeMode(Standard_False),
myRebuildFaceMode(Standard_False),
myTangentMode(Standard_False),
myMergeVertexMode(Standard_False),
myMergeEdgeMode(Standard_False)
// myMergeFaceMode(Standard_False)
{
}

// ================================================================================
// function: SetShape1
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::SetShape1(const TopoDS_Shape & TheShape)
{
  myShape1 = TheShape;
}

// ================================================================================
// function: SetShape2
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::SetShape2(const TopoDS_Shape & TheShape)
{
  myShape2 = TheShape;
}

// ================================================================================
// function: GetShape1
// purpose:
// ================================================================================
const TopoDS_Shape & BOP_ArgumentAnalyzer::GetShape1() const
{
  return myShape1;
}

// ================================================================================
// function: GetShape2
// purpose:
// ================================================================================
const TopoDS_Shape & BOP_ArgumentAnalyzer::GetShape2() const
{
  return myShape2;
}

// ================================================================================
// function: OperationType
// purpose:
// ================================================================================
BOP_Operation& BOP_ArgumentAnalyzer::OperationType() 
{
  return myOperation;
}

// ================================================================================
// function: StopOnFirstFaulty
// purpose:
// ================================================================================
Standard_Boolean & BOP_ArgumentAnalyzer::StopOnFirstFaulty()
{
  return myStopOnFirst;
}

// ================================================================================
// function: Perform
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::Perform()
{
  try {
    OCC_CATCH_SIGNALS
    myResult.Clear();

    if(myArgumentTypeMode) {
      TestTypes();
    }

    if(mySelfInterMode) {
      TestSelfInterferences();
    }

    if(mySmallEdgeMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestSmallEdge();
    }

    if(myRebuildFaceMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestRebuildFace();
    }

    if(myTangentMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestTangent();
    }

    if(myMergeVertexMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestMergeVertex();
    }
    
    if(myMergeEdgeMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestMergeEdge();
    }
    
//     if(myMergeFaceMode) {
//       TestMergeFace();
//     }
  }
  catch(Standard_Failure) {
    BOP_CheckResult aResult;
    aResult.SetCheckStatus(BOP_CheckUnknown);
    myResult.Append(aResult);
  }
}

// ================================================================================
// function: HasFaulty
// purpose:
// ================================================================================
Standard_Boolean BOP_ArgumentAnalyzer::HasFaulty() const
{
  return ( !myResult.IsEmpty());
}

// ================================================================================
// function: GetCheckResult
// purpose:
// ================================================================================
const BOP_ListOfCheckResult& BOP_ArgumentAnalyzer::GetCheckResult() const
{
  return myResult;
}

// ================================================================================
// function: TestTypes
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestTypes()
{
  Standard_Boolean isS1 = myShape1.IsNull(), isS2 = myShape2.IsNull();

  if(isS1 && isS2) {
    BOP_CheckResult aResult;
    aResult.SetCheckStatus(BOP_BadType);
    myResult.Append(aResult);
    return;
  }

  Standard_Boolean testS1 = TestShapeType(myShape1);
  Standard_Boolean testS2 = TestShapeType(myShape2);

  // single shape check (begin)
  if((isS1 && !isS2) || (!isS1 && isS2)) {
//     Standard_Boolean testS = (isS1) ? testS1 : testS2;
    Standard_Boolean testS = (isS1) ? testS2 : testS1;

    if(!testS) {
      const TopoDS_Shape & aS = (isS1) ? myShape1 : myShape2;
      BOP_CheckResult aResult;
      aResult.SetShape1(aS);
      aResult.SetCheckStatus(BOP_BadType);
      myResult.Append(aResult);
      return;
    }
    
  } // single shape is set (end)
  // two shapes check (begin)
  else {
    // test compounds and compsolids
    if(!testS1 || !testS2) {
      BOP_CheckResult aResult;
      if(!testS1 && !testS2) {
	aResult.SetShape1(myShape1);
	aResult.SetShape2(myShape2);
      }
      else {
	const TopoDS_Shape & aS = (!testS1) ? myShape1 : myShape2;
        if(!testS1)
          aResult.SetShape1(aS);
        else
          aResult.SetShape2(aS);
      }
      aResult.SetCheckStatus(BOP_BadType);
      myResult.Append(aResult);
      return;
    }
    // test faces, wires, edges
    TopAbs_ShapeEnum aT1 = myShape1.ShapeType(), aT2 = myShape2.ShapeType();

    if(aT1 != TopAbs_COMPOUND && aT2 != TopAbs_COMPOUND) {
      Standard_Boolean aTestRes = TestSubShapeType(aT1,aT2,myOperation);
      if(!aTestRes) {
        BOP_CheckResult aResult;
	aResult.SetShape1(myShape1);
	aResult.SetShape2(myShape2);
	aResult.SetCheckStatus(BOP_BadType);
	myResult.Append(aResult);
	return;
      }
    }
    else {
      Standard_Boolean aTestRes = Standard_True;
      if(aT1 == TopAbs_COMPOUND && aT2 != TopAbs_COMPOUND) {
        TopoDS_Iterator itS1(myShape1);
        while(itS1.More()) {
          aT1 = itS1.Value().ShapeType();
          aTestRes = TestSubShapeType(aT1,aT2,myOperation);
          if(!aTestRes)
            break;
          itS1.Next();
        }
      }
      else if(aT1 != TopAbs_COMPOUND && aT2 == TopAbs_COMPOUND) {
        TopoDS_Iterator itS2(myShape2);
        while(itS2.More()) {
          aT2 = itS2.Value().ShapeType();
          aTestRes = TestSubShapeType(aT1,aT2,myOperation);
          if(!aTestRes)
            break;
          itS2.Next();
        }
      }
      else {
        TopoDS_Iterator itS1(myShape1);
        while(itS1.More()) {
          aT1 = itS1.Value().ShapeType();
          TopoDS_Iterator itS2(myShape2);
          while(itS2.More()) {
            aT2 = itS2.Value().ShapeType();
            aTestRes = TestSubShapeType(aT1,aT2,myOperation);
            if(!aTestRes)
              break;
            itS2.Next();
          }
          if(!aTestRes)
            break;
          itS1.Next();
        }
      }

      if(!aTestRes) {
        BOP_CheckResult aResult;
	aResult.SetShape1(myShape1);
	aResult.SetShape2(myShape2);
	aResult.SetCheckStatus(BOP_BadType);
	myResult.Append(aResult);
	return;
      }
    }
  } // both shapes are set (end)
}

// ================================================================================
// function: TestSelfInterferences
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestSelfInterferences()
{
  Standard_Integer i = 0;

  for(i = 0; i < 2; i++) {
    TopoDS_Shape aS = (i == 0) ? myShape1 : myShape2;

    if(aS.IsNull())
      continue;
    BOPTools_Checker aChecker(aS);
    aChecker.Perform();

    if (aChecker.HasFaulty()) {
      const BOPTools_ListOfCheckResults& aResultList = aChecker.GetCheckResult();
      BOPTools_ListIteratorOfListOfCheckResults anIt(aResultList);

      for(; anIt.More(); anIt.Next()) {
	const BOPTools_CheckResult& aCheckResult = anIt.Value();

	if((aCheckResult.GetCheckStatus() == BOPTools_CHKUNKNOWN) ||
	   (aCheckResult.GetCheckStatus() == BOPTools_BADSHRANKRANGE) ||
	   (aCheckResult.GetCheckStatus() == BOPTools_NULLSRANKRANGE)) {
	  continue;
	}
	BOP_CheckResult aResult;
	if(i == 0)
	  aResult.SetShape1(myShape1);
	else
	  aResult.SetShape2(myShape2);
	TopTools_ListIteratorOfListOfShape anIt2(aCheckResult.GetShapes());

	for(; anIt2.More(); anIt2.Next()) {
	  if(i == 0)
	    aResult.AddFaultyShape1(anIt2.Value());
	  else
	    aResult.AddFaultyShape2(anIt2.Value());
	}
	aResult.SetCheckStatus(BOP_SelfIntersect);
	myResult.Append(aResult);

	if(myStopOnFirst) {
	  return;
	}
      }
    }
  }
}

// ================================================================================
// function: TestSmallEdge
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestSmallEdge() 
{
  Standard_Integer i = 0;
  BRepExtrema_DistShapeShape aDist;

  for(i = 0; i < 2; i++) {
    TopoDS_Shape aS = (i == 0) ? myShape1 : myShape2;

    if(aS.IsNull())
      continue;

    TopExp_Explorer anExp(aS, TopAbs_EDGE);

    for(; anExp.More(); anExp.Next()) {
      TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());

      if(!CheckEdge(anEdge)) {
	Standard_Boolean bKeepResult = Standard_True;

	if(myOperation == BOP_SECTION) {
	  TopoDS_Shape anOtherS = (i == 0) ? myShape2 : myShape1;
	  
	  if(!anOtherS.IsNull()) {
//	    BRepExtrema_DistShapeShape aDist;
	    aDist.LoadS2(anOtherS);
	    
	    Standard_Boolean bVertexIsOnShape = Standard_False;
	    Standard_Integer ii = 0;
	    TopExp_Explorer anExpV(anEdge, TopAbs_VERTEX);
	    
	    for(; anExpV.More(); anExpV.Next()) {
	      TopoDS_Shape aV = anExpV.Current();

	      aDist.LoadS1(aV);
	      aDist.Perform();

	      if(aDist.IsDone()) {

		for(ii = 1; ii <= aDist.NbSolution(); ii++) {
		  Standard_Real aTolerance = BRep_Tool::Tolerance(TopoDS::Vertex(aV));
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
	      bKeepResult = Standard_False;
	    }
	  }
	}

	if(bKeepResult) {
	  BOP_CheckResult aResult;

	  if(i == 0) {
	    aResult.SetShape1(myShape1);
	    aResult.AddFaultyShape1(anEdge);
	  }
	  else {
	    aResult.SetShape2(myShape2);
	    aResult.AddFaultyShape2(anEdge);
	  }
	  
	  aResult.SetCheckStatus(BOP_TooSmallEdge);
	  myResult.Append(aResult);

	  if(myStopOnFirst) {
	    return;
	  }
	}
      }
    }
  }
}

// ================================================================================
// function: TestRebuildFace
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestRebuildFace() 
{
  if((myOperation == BOP_SECTION) ||
     (myOperation == BOP_UNKNOWN))
    return;
  Standard_Integer i = 0;

  for(i = 0; i < 2; i++) {
    TopoDS_Shape aS = (i == 0) ? myShape1 : myShape2;

    if(aS.IsNull())
      continue;

    TopExp_Explorer anExp(aS, TopAbs_FACE);

    for(; anExp.More(); anExp.Next()) {
      TopoDS_Face aFace = TopoDS::Face(anExp.Current());

      BOP_WireEdgeSet aWES (aFace);
      TopExp_Explorer anExpE(aFace, TopAbs_EDGE);
      Standard_Integer nbstartedges = 0;

      for(; anExpE.More(); anExpE.Next()) {
	aWES.AddStartElement(anExpE.Current());
	nbstartedges++;
      }
      BOP_FaceBuilder aFB;
      aFB.Do(aWES);
      const TopTools_ListOfShape& aLF = aFB.NewFaces();
      Standard_Boolean bBadFace = Standard_False;

      if(aLF.Extent() != 1) {
	bBadFace = Standard_True;
      }
      else {
	Standard_Integer nbedgeused = 0;
	anExpE.Init(aLF.First(), TopAbs_EDGE);

	for(; anExpE.More(); anExpE.Next(), nbedgeused++);

	if(nbstartedges != nbedgeused) {
	  bBadFace = Standard_True;
	}
      }

      if(bBadFace) {
	BOP_CheckResult aResult;

	if(i == 0) {
	  aResult.SetShape1(myShape1);
	  aResult.AddFaultyShape1(aFace);
	}
	else {
	  aResult.SetShape2(myShape2);
	  aResult.AddFaultyShape2(aFace);
	}
	  
	aResult.SetCheckStatus(BOP_NonRecoverableFace);
	myResult.Append(aResult);

	if(myStopOnFirst) {
	  return;
	}
      }
    }
  }
}

// ================================================================================
// function: TestTangent
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestTangent() 
{
 // not implemented
}

// ================================================================================
// function: TestMergeSubShapes
// purpose:
// ================================================================================
 void BOP_ArgumentAnalyzer::TestMergeSubShapes(const TopAbs_ShapeEnum theType) 
{
  if(myShape1.IsNull() || myShape2.IsNull())
    return;

  BOP_CheckStatus aStatus = BOP_CheckUnknown;

  switch(theType) {
  case TopAbs_VERTEX: {
    aStatus = BOP_IncompatibilityOfVertex;
    break;
  }
  case TopAbs_EDGE: {
    aStatus = BOP_IncompatibilityOfEdge;
    break;
  }
  case TopAbs_FACE: {
    aStatus = BOP_IncompatibilityOfFace;
    break;
  }
  default: 
    return;
  }
  TopExp_Explorer anExp1(myShape1, theType);
  TopExp_Explorer anExp2(myShape2, theType);
  TopTools_SequenceOfShape aSeq1, aSeq2;
  TopTools_MapOfShape aMap1, aMap2;

  for(; anExp1.More(); anExp1.Next()) {
    TopoDS_Shape aS1 = anExp1.Current();

    if(aMap1.Contains(aS1))
      continue;
    aSeq1.Append(aS1);
    aMap1.Add(aS1);
  }

  for(; anExp2.More(); anExp2.Next()) {
    TopoDS_Shape aS2 = anExp2.Current();
    if(aMap2.Contains(aS2))
      continue;
    aSeq2.Append(aS2);
    aMap2.Add(aS2);
  }

  TColStd_Array2OfBoolean anArrayOfFlag(1, aSeq1.Length(), 1, aSeq2.Length());
  Standard_Integer i = 0, j = 0;
  for(i = 1; i <= aSeq1.Length(); i++)
    for(j = 1; j <= aSeq2.Length(); j++)
      anArrayOfFlag.SetValue(i, j, Standard_False);

  for(i = 1; i <= aSeq1.Length(); i++) {
    TopoDS_Shape aS1 = aSeq1.Value(i);
    TopTools_ListOfShape aListOfS2;
    Standard_Integer nbs = 0;

    for(j = 1; j <= aSeq2.Length(); j++) {
      TopoDS_Shape aS2 = aSeq2.Value(j);
      Standard_Boolean bIsEqual = Standard_False;

      if(theType == TopAbs_VERTEX) {

        TopoDS_Vertex aV1 = TopoDS::Vertex(aS1);
	TopoDS_Vertex aV2 = TopoDS::Vertex(aS2);
        gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
        gp_Pnt aP2 = BRep_Tool::Pnt(aV2);
        Standard_Real aDist = aP1.Distance(aP2);

        if(aDist <= (BRep_Tool::Tolerance(aV1) + BRep_Tool::Tolerance(aV2))) {
          bIsEqual = Standard_True;
        }
      }
      else if(theType == TopAbs_EDGE) {
	Standard_Integer aDiscretize = 30;
	Standard_Real    aDeflection = 0.01;
	TopoDS_Edge aE1 = TopoDS::Edge(aS1);
	TopoDS_Edge aE2 = TopoDS::Edge(aS2);

	IntTools_EdgeEdge aEE;
	aEE.SetEdge1 (aE1);
	aEE.SetEdge2 (aE2);
	aEE.SetTolerance1 (BRep_Tool::Tolerance(aE1));
	aEE.SetTolerance2 (BRep_Tool::Tolerance(aE2));
	aEE.SetDiscretize (aDiscretize);
	aEE.SetDeflection (aDeflection);

	Standard_Real f = 0., l = 0.;
	BRep_Tool::Range(aE1, f, l);
	aEE.SetRange1(f, l);

	BRep_Tool::Range(aE2, f, l);
	aEE.SetRange2(f, l);

	aEE.Perform();

	if (aEE.IsDone()) {
	  const IntTools_SequenceOfCommonPrts& aCPrts = aEE.CommonParts();
	  Standard_Integer ii = 0;

	  for (ii = 1; ii <= aCPrts.Length(); ii++) {
	    const IntTools_CommonPrt& aCPart = aCPrts(ii);

	    if (aCPart.Type() == TopAbs_EDGE) {
	      bIsEqual = Standard_True;
	    }
	  }
	}
      }
      else if(theType == TopAbs_FACE) {
        // not yet implemented!
      }

      if(bIsEqual) {
	anArrayOfFlag.SetValue(i, j, Standard_True );
	aListOfS2.Append(aS2);
	nbs++;
      }
    }

    if(nbs > 1) {
      BOP_CheckResult aResult;

      aResult.SetShape1(myShape1);
      aResult.SetShape2(myShape2);
      aResult.AddFaultyShape1(aS1);
      TopTools_ListIteratorOfListOfShape anIt(aListOfS2);

      for(; anIt.More(); anIt.Next()) {
	aResult.AddFaultyShape2(anIt.Value());
      }

      aResult.SetCheckStatus(aStatus);
      myResult.Append(aResult);

      if(myStopOnFirst) {
	return;
      }
    }
  }

  for(i = 1; i <= aSeq2.Length(); i++) {
    TopoDS_Shape aS2 = aSeq2.Value(i);
    TopTools_ListOfShape aListOfS1;
    Standard_Integer nbs = 0;

    for(j = 1; j <= aSeq1.Length(); j++) {
      TopoDS_Shape aS1 = aSeq1.Value(j);

      if(anArrayOfFlag.Value(j, i)) {
	aListOfS1.Append(aS1);
	nbs++;
      }
    }

    if(nbs > 1) {
      BOP_CheckResult aResult;

      aResult.SetShape1(myShape1);
      aResult.SetShape2(myShape2);
      TopTools_ListIteratorOfListOfShape anIt(aListOfS1);

      for(; anIt.More(); anIt.Next()) {
	aResult.AddFaultyShape1(anIt.Value());
      }
      aResult.AddFaultyShape2(aS2);

      aResult.SetCheckStatus(aStatus);
      myResult.Append(aResult);

      if(myStopOnFirst) {
	return;
      }
    }
  }
}

// ================================================================================
// function: TestMergeVertex
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestMergeVertex() 
{
  TestMergeSubShapes(TopAbs_VERTEX); 
}

// ================================================================================
// function: TestMergeEdge
// purpose:
// ================================================================================
void BOP_ArgumentAnalyzer::TestMergeEdge() 
{
  TestMergeSubShapes(TopAbs_EDGE); 
}

// ================================================================================
// function: TestMergeFace
// purpose:
// ================================================================================
// void BOP_ArgumentAnalyzer::TestMergeFace() 
// {
  // not implemented
// }

// ----------------------------------------------------------------------
// static function: TestShapeType
// purpose:
// ----------------------------------------------------------------------
Standard_Boolean TestShapeType(const TopoDS_Shape & TheShape)
{
  if(TheShape.IsNull())
    return Standard_False;

  TopAbs_ShapeEnum aT = TheShape.ShapeType();

  if(aT == TopAbs_COMPOUND && BOPTools_Tools3D::IsEmptyShape(TheShape))
    return Standard_False;

  TopoDS_Iterator anIt;
  TopoDS_Shape aSTmp, aShape;
  Standard_Integer aNbShapes, TreatRes = 0;

  if(aT==TopAbs_COMPOUND || aT==TopAbs_COMPSOLID) {
    aNbShapes=0;
    anIt.Initialize(TheShape);
    for (; anIt.More(); anIt.Next()) {
      if(!aNbShapes) {
	aSTmp=anIt.Value();
      }
      aNbShapes++;
      if(aNbShapes>1) {
	break;
      }
    }
    if(aT == TopAbs_COMPOUND) {
      if (aNbShapes==1) {
	TreatRes = BOPTools_DSFiller::TreatCompound(TheShape, aSTmp);
	if(TreatRes != 0)
	  return Standard_False;
	aShape=aSTmp;
	aT = aShape.ShapeType();
      }
      else if (aNbShapes>1) {
	TreatRes = BOPTools_DSFiller::TreatCompound(TheShape, aSTmp);
	if(TreatRes != 0)
	  return Standard_False;
	aShape=aSTmp;
	aT=aShape.ShapeType();
      }
    }
  }

  if(aT==TopAbs_COMPOUND || aT==TopAbs_COMPSOLID) {
    return Standard_False;
  }

  return Standard_True;
}

// ----------------------------------------------------------------------
// static function: CheckEdge
// purpose:
// ----------------------------------------------------------------------
Standard_Boolean CheckEdge(const TopoDS_Edge& theEdge) 
{
  Handle(IntTools_Context) aContext;
  TopoDS_Vertex aV1, aV2;
  //
  //modified by NIZNHY-PKV Tue Jan 31 15:07:13 2012f
  aContext=new IntTools_Context;
  ////modified by NIZNHY-PKV Tue Jan 31 15:07:16 2012t
  TopExp::Vertices(theEdge, aV1, aV2);


  if(aV1.IsNull() || aV2.IsNull() || BRep_Tool::Degenerated(theEdge))
    return Standard_True;
  Standard_Real aFirst = 0., aLast = 0.;
  BRep_Tool::Range(theEdge, aFirst, aLast);
  IntTools_Range aRange(aFirst, aLast);
  IntTools_ShrunkRange aSR(theEdge, aV1, aV2, aRange, aContext);

  if (!aSR.IsDone() || aSR.ErrorStatus() == 6) {
    return Standard_False;
  }
  return Standard_True;
}

// ----------------------------------------------------------------------
// static function: TestSubShapeType
// purpose:
// ----------------------------------------------------------------------
Standard_Boolean TestSubShapeType(const TopAbs_ShapeEnum theT1,
                                  const TopAbs_ShapeEnum theT2,
                                  const BOP_Operation    theOP)
{
  TopAbs_ShapeEnum aT1 = theT1, aT2 = theT2;

  if(aT1==TopAbs_FACE) {
    if(aT2==TopAbs_SOLID || aT2==TopAbs_SHELL || aT2==TopAbs_FACE ||
       aT2==TopAbs_WIRE || aT2==TopAbs_EDGE) {
      aT1=TopAbs_SHELL;
    }
  }
  if(aT2==TopAbs_FACE) {
    if(aT1==TopAbs_SOLID || aT1==TopAbs_SHELL ||
       aT1==TopAbs_WIRE || aT1==TopAbs_EDGE) {
      aT2=TopAbs_SHELL;
    }
  }
  if(aT1==TopAbs_EDGE) {
    if(aT2==TopAbs_SOLID || aT2==TopAbs_SHELL ||
       aT2==TopAbs_WIRE || aT2==TopAbs_EDGE) {
      aT1=TopAbs_WIRE;
    }
  }
  if(aT2==TopAbs_EDGE) {
    if(aT1==TopAbs_SOLID || aT1==TopAbs_SHELL || aT1==TopAbs_WIRE) {
      aT2=TopAbs_WIRE;
    }
  }

  // test operations
  if(theOP!=BOP_UNKNOWN) {
    Standard_Boolean opOk;
    if(aT1==TopAbs_SHELL && aT2==TopAbs_SHELL)
      opOk = Standard_True;
    else if(aT1==TopAbs_SOLID && aT2==TopAbs_SOLID)
      opOk = Standard_True;
    else if((aT1==TopAbs_SOLID && aT2==TopAbs_SHELL) ||
            (aT2==TopAbs_SOLID && aT1==TopAbs_SHELL))
      opOk = Standard_True;
    else if(aT1==TopAbs_WIRE && aT2==TopAbs_WIRE)
      opOk = Standard_True;
    else if((aT1==TopAbs_WIRE  && aT2==TopAbs_SHELL) ||
            (aT2==TopAbs_WIRE  && aT1==TopAbs_SHELL))
      opOk = Standard_True;
    else if((aT1==TopAbs_WIRE  && aT2==TopAbs_SOLID) ||
            (aT2==TopAbs_WIRE  && aT1==TopAbs_SOLID))
      opOk = Standard_True;
    else 
      opOk = Standard_False;

    if(!opOk) {
      return Standard_False;
    }

    if((aT1==TopAbs_SHELL && aT2==TopAbs_SOLID) ||
       (aT2==TopAbs_SHELL && aT1==TopAbs_SOLID)) {
      if(!BOP_ShellSolid::CheckArgTypes(aT1,aT2,theOP)) {
        return Standard_False;
      }
    }
    if((aT1==TopAbs_WIRE && aT2==TopAbs_SOLID) ||
       (aT2==TopAbs_WIRE && aT1==TopAbs_SOLID)) {
      if(!BOP_WireSolid::CheckArgTypes(aT1,aT2,theOP)) {
        return Standard_False;
      }
    }
    if((aT1==TopAbs_WIRE && aT2==TopAbs_SHELL) ||
       (aT1==TopAbs_WIRE && aT2==TopAbs_SHELL)) {
      if(!BOP_WireShell::CheckArgTypes(aT1,aT2,theOP)) {
        return Standard_False;
      }
    }
  }
  
  return Standard_True;
}
