// Created on: 2004-09-02
// Copyright (c) 2004-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_Range.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_Array2OfBoolean.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

// ================================================================================
// function: Constructor
// purpose:
// ================================================================================
BOPAlgo_ArgumentAnalyzer::BOPAlgo_ArgumentAnalyzer() : 
BOPAlgo_Algo(),
myStopOnFirst(Standard_False),
myOperation(BOPAlgo_UNKNOWN),
myArgumentTypeMode(Standard_False),
mySelfInterMode(Standard_False),
mySmallEdgeMode(Standard_False),
myRebuildFaceMode(Standard_False),
myTangentMode(Standard_False),
myMergeVertexMode(Standard_False),
myMergeEdgeMode(Standard_False),
myContinuityMode(Standard_False),
myCurveOnSurfaceMode(Standard_False),
myEmpty1(Standard_False),
myEmpty2(Standard_False)
{
}
//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BOPAlgo_ArgumentAnalyzer::~BOPAlgo_ArgumentAnalyzer()
{
  myResult.Clear();
}

// ================================================================================
// function: SetShape1
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::SetShape1(const TopoDS_Shape & TheShape)
{
  myShape1 = TheShape;
}

// ================================================================================
// function: SetShape2
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::SetShape2(const TopoDS_Shape & TheShape)
{
  myShape2 = TheShape;
}

// ================================================================================
// function: GetShape1
// purpose:
// ================================================================================
const TopoDS_Shape & BOPAlgo_ArgumentAnalyzer::GetShape1() const
{
  return myShape1;
}

// ================================================================================
// function: GetShape2
// purpose:
// ================================================================================
const TopoDS_Shape & BOPAlgo_ArgumentAnalyzer::GetShape2() const
{
  return myShape2;
}

// ================================================================================
// function: OperationType
// purpose:
// ================================================================================
BOPAlgo_Operation& BOPAlgo_ArgumentAnalyzer::OperationType() 
{
  return myOperation;
}

// ================================================================================
// function: StopOnFirstFaulty
// purpose:
// ================================================================================
Standard_Boolean & BOPAlgo_ArgumentAnalyzer::StopOnFirstFaulty()
{
  return myStopOnFirst;
}

// ================================================================================
// function: Prepare
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::Prepare()
{
  Standard_Boolean isS1 = myShape1.IsNull(), isS2 = myShape2.IsNull();
  if (!isS1) {
    myEmpty1 = BOPTools_AlgoTools3D::IsEmptyShape(myShape1);
  }
  if (!isS2) {
    myEmpty2 = BOPTools_AlgoTools3D::IsEmptyShape(myShape2);
  }
}
// ================================================================================
// function: Perform
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::Perform()
{
  try {
    OCC_CATCH_SIGNALS
    myResult.Clear();
    //
    UserBreak();
    //
    // 1. Prepare
    Prepare();
    //
    UserBreak();
    //
    // 2. Test types
    if(myArgumentTypeMode) {
      TestTypes();
    }
    //
    UserBreak();
    //
    // 3. Test self-interference
    if(mySelfInterMode) {
      TestSelfInterferences();
    }
    //
    UserBreak();
    //
    // 4. Test small edges
    if(mySmallEdgeMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestSmallEdge();
    }
    //
    UserBreak();
    //
    // 5. Test possibility to rebuild faces
    if(myRebuildFaceMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestRebuildFace();
    }
    //
    UserBreak();
    //
    // 6. Test tangent
    if(myTangentMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestTangent();
    }
    //
    UserBreak();
    //
    // 7. Test merge vertices
    if(myMergeVertexMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestMergeVertex();
    }
    //
    UserBreak();
    //
    // 8. Test merge edges
    if(myMergeEdgeMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestMergeEdge();
    }
    //
    UserBreak();
    //
    // 9. Test shapes continuity
    if(myContinuityMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestContinuity();
    }
    //
    UserBreak();
    //
    // 10. Test validity of the curves on the surfaces
    if(myCurveOnSurfaceMode) {
      if(!(!myResult.IsEmpty() && myStopOnFirst))
        TestCurveOnSurface();
    }
  }
  catch(Standard_Failure) {
    BOPAlgo_CheckResult aResult;
    aResult.SetCheckStatus(BOPAlgo_CheckUnknown);
    myResult.Append(aResult);
  }
}

// ================================================================================
// function: HasFaulty
// purpose:
// ================================================================================
Standard_Boolean BOPAlgo_ArgumentAnalyzer::HasFaulty() const
{
  return ( !myResult.IsEmpty());
}

// ================================================================================
// function: GetCheckResult
// purpose:
// ================================================================================
const BOPAlgo_ListOfCheckResult& BOPAlgo_ArgumentAnalyzer::GetCheckResult() const
{
  return myResult;
}

// ================================================================================
// function: TestTypes
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::TestTypes()
{
  Standard_Boolean isS1 = myShape1.IsNull(), isS2 = myShape2.IsNull();

  if(isS1 && isS2) {
    BOPAlgo_CheckResult aResult;
    aResult.SetCheckStatus(BOPAlgo_BadType);
    myResult.Append(aResult);
    return;
  }

  //single shape check
  if((isS1 && !isS2) || (!isS1 && isS2)) {
    Standard_Boolean bIsEmpty = (isS1) ? myEmpty2 : myEmpty1;

    if(bIsEmpty || myOperation!=BOPAlgo_UNKNOWN) {
      const TopoDS_Shape & aS = (isS1) ? myShape2 : myShape1;
      BOPAlgo_CheckResult aResult;
      aResult.SetShape1(aS);
      aResult.SetCheckStatus(BOPAlgo_BadType);
      myResult.Append(aResult);
      return;
    }
  }
  // two shapes check (begin)
  else {
    if(myEmpty1 || myEmpty2) {
      BOPAlgo_CheckResult aResult;
      if(myEmpty1 && myEmpty2) {
        aResult.SetShape1(myShape1);
        aResult.SetShape2(myShape2);
      }
      else {
        const TopoDS_Shape & aS = myEmpty1 ? myShape1 : myShape2;
        if(myEmpty1)
          aResult.SetShape1(aS);
        else
          aResult.SetShape2(aS);
      }
      aResult.SetCheckStatus(BOPAlgo_BadType);
      myResult.Append(aResult);
      return;
    }
    //
    Standard_Integer aDim1, aDim2;
    Standard_Boolean bBadTypes = Standard_False;
    //
    aDim1 = BOPTools_AlgoTools::Dimension(myShape1);
    aDim2 = BOPTools_AlgoTools::Dimension(myShape2);
    if (aDim1 < aDim2) {
      if (myOperation == BOPAlgo_FUSE ||
          myOperation == BOPAlgo_CUT21) {
        bBadTypes = Standard_True;
      }
    }
    else if (aDim1 > aDim2) {
      if (myOperation == BOPAlgo_FUSE ||
          myOperation == BOPAlgo_CUT) {
        bBadTypes = Standard_True;
      }
    }
    if (bBadTypes) {
      BOPAlgo_CheckResult aResult;
      aResult.SetShape1(myShape1);
      aResult.SetShape2(myShape2);
      aResult.SetCheckStatus(BOPAlgo_BadType);
      myResult.Append(aResult);
    }
  }
}
//=======================================================================
//function : TestSelfInterferences
//purpose  : 
//=======================================================================
void BOPAlgo_ArgumentAnalyzer::TestSelfInterferences()
{
  Standard_Integer ii;
  //
  for(ii = 0; ii < 2; ii++) {
    const TopoDS_Shape& aS = (ii == 0) ? myShape1 : myShape2;
    if(aS.IsNull()) {
      continue;
    }
    //
    Standard_Boolean bIsEmpty = (ii == 0) ? myEmpty1 : myEmpty2;
    if (bIsEmpty) {
      continue;
    }
    //
    Standard_Integer n1, n2;
    BOPDS_MapIteratorOfMapOfPair aItMPK;
    TopTools_ListOfShape anArgs;
    BOPAlgo_CheckerSI aChecker;
    //
    anArgs.Append(aS);
    aChecker.SetArguments(anArgs);
    aChecker.SetNonDestructive(Standard_True);
    aChecker.SetRunParallel(myRunParallel);
    aChecker.SetFuzzyValue(myFuzzyValue);
    aChecker.SetProgressIndicator(myProgressIndicator);
    //
    aChecker.Perform();
    Standard_Boolean hasError = aChecker.HasErrors();
    //
    const BOPDS_DS& aDS=*(aChecker.PDS());
    const BOPDS_MapOfPair& aMPK=aDS.Interferences();
    //
    aItMPK.Initialize(aMPK);
    for (; aItMPK.More(); aItMPK.Next()) {
      const BOPDS_Pair& aPK=aItMPK.Value();
      aPK.Indices(n1, n2);
      if(aDS.IsNewShape(n1) || aDS.IsNewShape(n2)) {
        continue;
      }
      //
      const TopoDS_Shape& aS1=aDS.Shape(n1);
      const TopoDS_Shape& aS2=aDS.Shape(n2);
      //
      BOPAlgo_CheckResult aResult;
      if(ii == 0) {
        aResult.SetShape1(myShape1);
        aResult.AddFaultyShape1(aS1);
        if (!aS1.IsSame(aS2))
          aResult.AddFaultyShape1(aS2);
      }
      else {
        aResult.SetShape2(myShape2);
        aResult.AddFaultyShape2(aS1);
        if (!aS1.IsSame(aS2))
          aResult.AddFaultyShape2(aS2);
      }
      aResult.SetCheckStatus(BOPAlgo_SelfIntersect);
      myResult.Append(aResult);
    }
    //
    if (hasError) {
      BOPAlgo_CheckResult aResult;
      if(ii == 0) {
        aResult.SetShape1(myShape1);
        aResult.AddFaultyShape1(myShape1);
      }
      else {
        aResult.SetShape2(myShape2);
        aResult.AddFaultyShape2(myShape2);
      }
      aResult.SetCheckStatus(BOPAlgo_OperationAborted);
      myResult.Append(aResult);
    }
  }// for(ii = 0; ii < 2; ii++) {
}
// ================================================================================
// function: TestSmallEdge
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::TestSmallEdge() 
{
  Standard_Integer i = 0;
  BRepExtrema_DistShapeShape aDist;
  Handle(IntTools_Context) aCtx;
  //
  aCtx = new IntTools_Context;
  
  for(i = 0; i < 2; i++) {
    const TopoDS_Shape& aS = (i == 0) ? myShape1 : myShape2;

    if(aS.IsNull())
      continue;

    TopExp_Explorer anExp(aS, TopAbs_EDGE);

    for(; anExp.More(); anExp.Next()) {
      const TopoDS_Edge& anEdge = *(TopoDS_Edge*)&anExp.Current();
      if (BRep_Tool::Degenerated(anEdge)) {
        continue;
      }

      if(BOPTools_AlgoTools::IsMicroEdge(anEdge, aCtx)) {
        Standard_Boolean bKeepResult = Standard_True;
        
        if(myOperation == BOPAlgo_SECTION) {
          const TopoDS_Shape& anOtherS = (i == 0) ? myShape2 : myShape1;
          
          if(!anOtherS.IsNull()) {
            aDist.LoadS2(anOtherS);
            
            Standard_Boolean bVertexIsOnShape = Standard_False;
            Standard_Integer ii = 0;
            TopExp_Explorer anExpV(anEdge, TopAbs_VERTEX);
            
            for(; anExpV.More(); anExpV.Next()) {
              const TopoDS_Shape& aV = anExpV.Current();
              
              aDist.LoadS1(aV);
              aDist.Perform();
              
              if(aDist.IsDone()) {
                
                for(ii = 1; ii <= aDist.NbSolution(); ii++) {
                  Standard_Real aTolerance = BRep_Tool::Tolerance(*(TopoDS_Vertex*)&aV);
                  const TopoDS_Shape& aSupportShape = aDist.SupportOnShape2(ii);
                  
                  switch(aSupportShape.ShapeType()) {
                  case TopAbs_VERTEX: {
                    aTolerance += BRep_Tool::Tolerance(*(TopoDS_Vertex*)&(aSupportShape));
                    break;
                  }
                  case TopAbs_EDGE: {
                    aTolerance += BRep_Tool::Tolerance(*(TopoDS_Edge*)&(aSupportShape));
                    break;
                  }
                  case TopAbs_FACE: {
                    aTolerance += BRep_Tool::Tolerance(*(TopoDS_Face*)&(aSupportShape));
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
          BOPAlgo_CheckResult aResult;
          
          if(i == 0) {
            aResult.SetShape1(myShape1);
            aResult.AddFaultyShape1(anEdge);
          }
          else {
            aResult.SetShape2(myShape2);
            aResult.AddFaultyShape2(anEdge);
          }
          
          aResult.SetCheckStatus(BOPAlgo_TooSmallEdge);
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
void BOPAlgo_ArgumentAnalyzer::TestRebuildFace() 
{
  if((myOperation == BOPAlgo_SECTION) ||
     (myOperation == BOPAlgo_UNKNOWN))
    return;
  Standard_Integer i = 0;

  for(i = 0; i < 2; i++) {
    const TopoDS_Shape& aS = (i == 0) ? myShape1 : myShape2;

    if(aS.IsNull())
      continue;

    TopExp_Explorer anExp(aS, TopAbs_FACE);
    TopTools_ListOfShape aLS;

    for(; anExp.More(); anExp.Next()) {
      const TopoDS_Face& aFace = *(TopoDS_Face*)&(anExp.Current());

      TopoDS_Face aFF = aFace;
      aFF.Orientation(TopAbs_FORWARD);
      TopExp_Explorer anExpE(aFF, TopAbs_EDGE);
      Standard_Integer nbstartedges = 0;
      aLS.Clear();
      //
      for(; anExpE.More(); anExpE.Next()) {
        const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&anExpE.Current()));
        TopAbs_Orientation anOriE=aE.Orientation();
        //
        if (anOriE==TopAbs_INTERNAL) {
          TopoDS_Edge aEE=aE;
          aEE.Orientation(TopAbs_FORWARD);
          aLS.Append(aEE);
          aEE.Orientation(TopAbs_REVERSED);
          aLS.Append(aEE);
        }
        else {
          aLS.Append(aE);
        }
        nbstartedges++;
      }
      BOPAlgo_BuilderFace aBF;
      aBF.SetFace(aFace);
      aBF.SetShapes(aLS);
      aBF.Perform();
      const TopTools_ListOfShape& aLF = aBF.Areas();
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
        BOPAlgo_CheckResult aResult;
        
        if(i == 0) {
          aResult.SetShape1(myShape1);
          aResult.AddFaultyShape1(aFace);
        }
        else {
          aResult.SetShape2(myShape2);
          aResult.AddFaultyShape2(aFace);
        }
        
        aResult.SetCheckStatus(BOPAlgo_NonRecoverableFace);
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
void BOPAlgo_ArgumentAnalyzer::TestTangent() 
{
 // not implemented
}

// ================================================================================
// function: TestMergeSubShapes
// purpose:
// ================================================================================
 void BOPAlgo_ArgumentAnalyzer::TestMergeSubShapes(const TopAbs_ShapeEnum theType) 
{
  if(myShape1.IsNull() || myShape2.IsNull())
    return;

  if (myEmpty1 || myEmpty2) 
    return;

  BOPAlgo_CheckStatus aStatus = BOPAlgo_CheckUnknown;

  switch(theType) {
  case TopAbs_VERTEX: {
    aStatus = BOPAlgo_IncompatibilityOfVertex;
    break;
  }
  case TopAbs_EDGE: {
    aStatus = BOPAlgo_IncompatibilityOfEdge;
    break;
  }
  case TopAbs_FACE: {
    aStatus = BOPAlgo_IncompatibilityOfFace;
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
    const TopoDS_Shape& aS1 = anExp1.Current();

    if(aMap1.Contains(aS1))
      continue;
    aSeq1.Append(aS1);
    aMap1.Add(aS1);
  }

  for(; anExp2.More(); anExp2.Next()) {
    const TopoDS_Shape& aS2 = anExp2.Current();
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
    const TopoDS_Shape& aS1 = aSeq1.Value(i);
    TopTools_ListOfShape aListOfS2;
    Standard_Integer nbs = 0;

    for(j = 1; j <= aSeq2.Length(); j++) {
      const TopoDS_Shape& aS2 = aSeq2.Value(j);
      Standard_Boolean bIsEqual = Standard_False;

      if(theType == TopAbs_VERTEX) {

        const TopoDS_Vertex& aV1 = *(TopoDS_Vertex*)&(aS1);
        const TopoDS_Vertex& aV2 = *(TopoDS_Vertex*)&(aS2);
        gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
        gp_Pnt aP2 = BRep_Tool::Pnt(aV2);
        Standard_Real aDist = aP1.Distance(aP2);

        if(aDist <= (BRep_Tool::Tolerance(aV1) + BRep_Tool::Tolerance(aV2))) {
          bIsEqual = Standard_True;
        }
      }
      else if(theType == TopAbs_EDGE) {
        const TopoDS_Edge& aE1 = *(TopoDS_Edge*)&(aS1);
        const TopoDS_Edge& aE2 = *(TopoDS_Edge*)&(aS2);
        //
        IntTools_EdgeEdge aEE(aE1, aE2);
        //
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
      BOPAlgo_CheckResult aResult;

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
    const TopoDS_Shape& aS2 = aSeq2.Value(i);
    TopTools_ListOfShape aListOfS1;
    Standard_Integer nbs = 0;

    for(j = 1; j <= aSeq1.Length(); j++) {
      const TopoDS_Shape& aS1 = aSeq1.Value(j);

      if(anArrayOfFlag.Value(j, i)) {
        aListOfS1.Append(aS1);
        nbs++;
      }
    }
    
    if(nbs > 1) {
      BOPAlgo_CheckResult aResult;

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
void BOPAlgo_ArgumentAnalyzer::TestMergeVertex() 
{
  TestMergeSubShapes(TopAbs_VERTEX); 
}

// ================================================================================
// function: TestMergeEdge
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::TestMergeEdge() 
{
  TestMergeSubShapes(TopAbs_EDGE); 
}

// ================================================================================
// function: TestContinuity
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::TestContinuity() 
{
  Standard_Integer i, j, aNbS;
  Standard_Real f, l;
  TopExp_Explorer aExp;
  //
  for (i = 0; i < 2; ++i) {
    const TopoDS_Shape& aS = !i ? myShape1 : myShape2;
    if(aS.IsNull()) {
      continue;
    }
    //
    TopTools_IndexedMapOfShape aMS;
    //Edges
    aExp.Init(aS, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExp.Current();
      if (BRep_Tool::Degenerated(aE)) {
        continue;
      }
      const Handle(Geom_Curve)& aC = BRep_Tool::Curve(aE, f, l);
      if (aC->Continuity() == GeomAbs_C0) {
        aMS.Add(aE);
      }
    }
    //Faces
    aExp.Init(aS, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Face& aF = *(TopoDS_Face*)&aExp.Current();
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
      if (aSurf->Continuity() == GeomAbs_C0) {
        aMS.Add(aF);
      }
    }
    //
    //add shapes with continuity C0 to result
    aNbS = aMS.Extent();
    for (j = 1; j <= aNbS; ++j) {
      const TopoDS_Shape& aFS = aMS(j);
      BOPAlgo_CheckResult aResult;
      if(i == 0) {
        aResult.SetShape1(myShape1);
        aResult.AddFaultyShape1(aFS);
      } else {
        aResult.SetShape2(myShape2);
        aResult.AddFaultyShape2(aFS);
      }
      aResult.SetCheckStatus(BOPAlgo_GeomAbs_C0);
      myResult.Append(aResult);
    }
  }
}

// ================================================================================
// function: TestCurveOnSurface
// purpose:
// ================================================================================
void BOPAlgo_ArgumentAnalyzer::TestCurveOnSurface()
{
  Standard_Integer i;
  Standard_Real aT, aD, aTolE;
  TopExp_Explorer aExpF, aExpE;
  //
  for(i = 0; i < 2; i++) {
    const TopoDS_Shape& aS = (i == 0) ? myShape1 : myShape2;
    if(aS.IsNull()) {
      continue;
    }
    //
    aExpF.Init(aS, TopAbs_FACE);
    for (; aExpF.More(); aExpF.Next()) {
      const TopoDS_Face& aF = *(TopoDS_Face*)&aExpF.Current();
      //
      aExpE.Init(aF, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExpE.Current();
        //
        if (BOPTools_AlgoTools::ComputeTolerance(aF, aE, aD, aT)) {
          aTolE = BRep_Tool::Tolerance(aE);
          if (aD > aTolE) {
            BOPAlgo_CheckResult aResult;
            aResult.SetCheckStatus(BOPAlgo_InvalidCurveOnSurface);
            if(i == 0) {
              aResult.SetShape1(myShape1);
              aResult.AddFaultyShape1(aE);
              aResult.AddFaultyShape1(aF);
              aResult.SetMaxDistance1(aD);
              aResult.SetMaxParameter1(aT);
            }
            else {
              aResult.SetShape2(myShape2);
              aResult.AddFaultyShape2(aE);
              aResult.AddFaultyShape2(aF);
              aResult.SetMaxDistance2(aD);
              aResult.SetMaxParameter2(aT);
            }
            myResult.Append(aResult);
          }
        }
      }
    }
  }
}
