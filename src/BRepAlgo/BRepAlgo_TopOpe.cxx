// Created on: 2004-03-02
// Created by: Igor FEOKTISTOV
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



#include <BRepAlgo_TopOpe.ixx>
#include <BRepAlgo_EdgeConnector.hxx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_SetIteratorOfSetOfInteger.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BOP_Builder.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOP_SolidSolid.hxx>
#include <BOP_SolidSolidHistoryCollector.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_SetOfInteger.hxx>
#include <BOP_ShellSolid.hxx>
#include <BOP_ShellSolidHistoryCollector.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>

#ifdef DRAW
//#include <TestTopOpe.hxx>
#endif

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================

BRepAlgo_TopOpe::BRepAlgo_TopOpe() :
       myDSFiller(NULL), myBuilder(NULL)
{
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::Init()
{
  if (myBuilder != NULL) {
    delete myBuilder;
  }
  if (myDSFiller != NULL) {
    delete myDSFiller;
  }

  myDSFiller = NULL;
  myBuilder = NULL;

  myDSFiller = new BOPTools_DSFiller;


  myRecomputeBuilderIsDone = Standard_False;
  myGetSectionIsDone = Standard_False;
  myListOfCompoundOfEdgeConnected.Clear();
  myEC = new BRepAlgo_EdgeConnector();
  

  myState1 = TopAbs_UNKNOWN;
  myState2 = TopAbs_UNKNOWN;

}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
  void BRepAlgo_TopOpe::Destroy()
{
  if (myBuilder!=NULL) {
    delete myBuilder;
    myBuilder=NULL;
  }
  if (myDSFiller!=NULL) {
    delete myDSFiller;
    myDSFiller=NULL;
  }
}
// Filling the DS

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::Load(const TopoDS_Shape& )
{
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::Load(TopoDS_Shape& S1, 
			   TopoDS_Shape& S2)
{

//   if ( S1.Orientation() == TopAbs_REVERSED ) {
//     S1.Orientation(TopAbs_FORWARD);
//   }
//   if ( S2.Orientation() == TopAbs_REVERSED ) {
//     S2.Orientation(TopAbs_FORWARD);
//   }
  

  myDSFiller->SetShapes(S1, S2);
  myS1 = myDSFiller->Shape1();
  myS2 = myDSFiller->Shape2();

  myDSFiller->InitFillersAndPools();

}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::Intersect()
{
//  myRecomputeBuilderIsDone = Standard_False;
 
  if(!myS1.IsNull() && !myS2.IsNull()) {
    myDSFiller->Perform();
  }

  myRecomputeBuilderIsDone = Standard_True;

}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::Intersect
(const TopoDS_Shape& S1,
 const TopoDS_Shape& S2)
{
  myRecomputeBuilderIsDone = Standard_False;
  
  if(S1.IsNull() || S2.IsNull()) {
    return;
  }
  
//  Standard_Boolean orientFORWARD = Standard_False;
  TopExp_Explorer exp;
  if(S1.ShapeType() != TopAbs_FACE) {
    exp.Init(S1, TopAbs_FACE);
    if(!exp.More())
      return;
  }
  if(S2.ShapeType() != TopAbs_FACE) {
    exp.Init(S2, TopAbs_FACE);
    if(!exp.More())
      return;
  }

  const BooleanOperations_ShapesDataStructure& aDS = myDSFiller->DS();
  TColStd_SetOfInteger anObjSubSet, aToolSubSet;

  TopTools_IndexedMapOfShape aMap;
  TopExp::MapShapes(S1, aMap);

  Standard_Integer i, index, nbs = aMap.Extent();
  for(i = 1; i <= nbs; ++i) {
    const TopoDS_Shape& aS = aMap(i);
    index = aDS.ShapeIndex(aS, 1);
    if(index > 0) {
      anObjSubSet.Add(index);
    }
  }
    
  aMap.Clear();
  TopExp::MapShapes(S2, aMap);
  nbs = aMap.Extent();
  for(i = 1; i <= nbs; ++i) {
    const TopoDS_Shape& aS = aMap(i);
    index = aDS.ShapeIndex(aS, 2);
    if(index > 0) {
      aToolSubSet.Add(index);
    }
  }
    
  myDSFiller->PartialPerform(anObjSubSet, aToolSubSet);

}

//=======================================================================
//function : ToCompleteIntersection
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::ToCompleteIntersection()
{
  if(!myRecomputeBuilderIsDone) {
    myDSFiller->ToCompletePerform();
    myRecomputeBuilderIsDone = Standard_True;
  }
}

// Construction des Sections

//=======================================================================
//function : GetSectionEdgeSet
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_TopOpe::GetSectionEdgeSet()
{
  if(!myRecomputeBuilderIsDone) {
    ToCompleteIntersection();   
    myRecomputeBuilderIsDone = Standard_True;
    myGetSectionIsDone = Standard_False;
  } 

  if(myGetSectionIsDone)
    return myListOfCompoundOfEdgeConnected;

  myGetSectionIsDone = Standard_True;
  
  myListOfCompoundOfEdgeConnected.Clear();
  
  // EdgeConnector
  Handle(BRepAlgo_EdgeConnector) EC = myEC;
  EC->ClearStartElement();
  TopTools_MapOfShape ME;
  ME.Clear();

  Standard_Integer i, j,  aNbFFs, aNbS, aNbCurves, nSect;
    
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_InterferencePool& anInterfPool=myDSFiller->InterfPool();
  BOPTools_InterferencePool* pInterfPool= 
    (BOPTools_InterferencePool*) &anInterfPool;
  BOPTools_CArray1OfSSInterference& aFFs=
    pInterfPool->SSInterferences();
  //
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; ++i) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    //
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
      
      if(ME.Contains(aE)) continue;
      
      ME.Add(aE);
      EC->AddStart(aE);
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
	const TopoDS_Edge& aE = TopoDS::Edge(aS);
	
	if(ME.Contains(aE)) continue;
	
	ME.Add(aE);
	EC->AddStart(aE);
      }
    }
  }
  

  TopTools_ListOfShape& LW = EC->MakeBlock();
  
  // the wires are transformed into compounds.
  myCompoundWireMap.Clear();
  BRep_Builder BB;
  TopTools_ListIteratorOfListOfShape ILW(LW);
  TopExp_Explorer Explor;
  for(;ILW.More();ILW.Next()) {
    TopoDS_Compound Compound;
    //POP
    BB.MakeCompound(Compound);
    //      BB.MakeCompound(TopoDS::Compound(Compound));
    Explor.Init(ILW.Value(), TopAbs_EDGE);
    for(;Explor.More(); Explor.Next()) {
      BB.Add(Compound, Explor.Current());
    }
    myListOfCompoundOfEdgeConnected.Append(Compound);
    myCompoundWireMap.Bind(Compound,ILW.Value());
  }
  
  return myListOfCompoundOfEdgeConnected;

}


//=======================================================================
//function : SuppressEdgeSet
//purpose  : 
//=======================================================================

void BRepAlgo_TopOpe::SuppressEdgeSet()
{
  myGetSectionIsDone = Standard_False;
}


// Reconstruction of Shapes

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_TopOpe::Merge
(const TopAbs_State state1,
 const TopAbs_State state2)
{

  if(!myDSFiller->IsDone()) return myEmptyShape;

  if((state1 != TopAbs_IN) &&
     (state1 != TopAbs_OUT))
    return myEmptyShape;
  if((state2 != TopAbs_IN) &&
     (state2 != TopAbs_OUT))
    return myEmptyShape;
  // if GetSectionEdgeSet has already been called, nothing is done 
  // in GetSectionEdgeSet.
  if(myState1 != TopAbs_UNKNOWN)
    if(myState1 != state1 || myState2 != state2)
      myGetSectionIsDone = Standard_False;
  myState1 = state1;
  myState2 = state2;
  
  BOP_Operation anOp = BOP_UNKNOWN;
  if(myState1 == TopAbs_IN && myState2 == TopAbs_IN) anOp = BOP_COMMON;
  if(myState1 == TopAbs_OUT && myState2 == TopAbs_IN) anOp = BOP_CUT;
  if(myState1 == TopAbs_IN && myState2 == TopAbs_OUT) anOp = BOP_CUT21;
  if(myState1 == TopAbs_OUT && myState2 == TopAbs_OUT) anOp = BOP_FUSE;

  if (myBuilder != NULL) {
    delete myBuilder;
  }

  myBuilder = new BOP_SolidSolid;
  myBuilder->SetShapes(myS1, myS2);
  myBuilder->SetOperation (anOp);

  myHistory = new BOP_SolidSolidHistoryCollector(myS1, myS2, anOp);
  myBuilder->SetHistoryCollector(myHistory);

  myBuilder->DoWithFiller (*myDSFiller);

  myResultShape = myBuilder->Result();

  return myResultShape;
}

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_TopOpe::Merge
(const TopAbs_State )
{

  myResultShape.Nullify();
  return myResultShape;

}

//=======================================================================
//function : PaveBlocksNotEmpty
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgo_TopOpe::PaveBlocksNotEmpty()
{
  Standard_Integer i, aNbFFs, aNbS;
    
//  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();
  const BOPTools_InterferencePool& anInterfPool=myDSFiller->InterfPool();
  BOPTools_InterferencePool* pInterfPool= 
    (BOPTools_InterferencePool*) &anInterfPool;
  BOPTools_CArray1OfSSInterference& aFFs=
    pInterfPool->SSInterferences();
    //

    //
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; ++i) {
    BOPTools_SSInterference& aFFi=aFFs(i);
      //
      // Old Section Edges
    const BOPTools_ListOfPaveBlock& aSectList=aFFi.PaveBlocks();
    aNbS=aSectList.Extent();
      
    if (aNbS > 0) {
      return Standard_True;
    }
  }

  return Standard_False;

}

//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================

const BOP_PBuilder& BRepAlgo_TopOpe::Builder() const
{
  return myBuilder;
}

//=======================================================================
//function : DSFiller
//purpose  : 
//=======================================================================

const BOPTools_PDSFiller& BRepAlgo_TopOpe::DSFiller() const
{
  return myDSFiller;
}

//=======================================================================
//function : History
//purpose  : 
//=======================================================================

const Handle(BOP_HistoryCollector)& BRepAlgo_TopOpe::History() const
{
  return myHistory;
}

