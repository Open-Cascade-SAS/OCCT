// Created on: 1997-08-13
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <BRepAlgo_BooleanOperations.hxx>
#include <BRepAlgo_DSAccess.hxx>
#include <BRepAlgo_EdgeConnector.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Check.hxx>
#include <TopOpeBRepDS_CheckStatus.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceIterator.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_Reducer.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool_GeomTool.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>


//=======================================================================
//function : Create
//purpose  : 
//=======================================================================

BRepAlgo_DSAccess::BRepAlgo_DSAccess() {
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::Init()
{
  if(myHDS.IsNull()) 
    myHDS = new TopOpeBRepDS_HDataStructure();
  else
    myHDS->ChangeDS().Init();
  myRecomputeBuilderIsDone = Standard_False;
  myGetSectionIsDone = Standard_False;
  myListOfCompoundOfEdgeConnected.Clear();
  myEC = new BRepAlgo_EdgeConnector();
  myHB.Nullify();
  
  // init of the builder
  Standard_Real tol3dAPPROX = 1e-7;
  Standard_Real tol2dAPPROX = 1e-7;
  // set tolerance values used by the APPROX process
  TopOpeBRepTool_GeomTool GT;
  GT.Define(TopOpeBRepTool_APPROX);
  GT.SetTolerances(tol3dAPPROX,tol2dAPPROX);
  TopOpeBRepDS_BuildTool BT(GT);
  myHB = new TopOpeBRepBuild_HBuilder(BT);
  myHB->ChangeBuilder().ChangeClassify(Standard_False);

  myState1 = TopAbs_UNKNOWN;
  myState2 = TopAbs_UNKNOWN;

}


// Filling of the DS

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::Load(const TopoDS_Shape& S)
{
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  myS1 = S;
  DS.AddShape(S, 1);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::Load(TopoDS_Shape& S1, 
			     TopoDS_Shape& S2)
{
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  
  if ( S1.Orientation() == TopAbs_REVERSED ) {
    S1.Orientation(TopAbs_FORWARD);
  }
  if ( S2.Orientation() == TopAbs_REVERSED ) {
    S2.Orientation(TopAbs_FORWARD);
  }
  
  DS.AddShape(S1,1);
  DS.AddShape(S2,2);

  TopOpeBRepTool_ShapeExplorer ex1,ex2;
  for (ex1.Init(S1,TopAbs_SOLID); ex1.More(); ex1.Next()) {
    const TopoDS_Shape& so1 = ex1.Current();
    for (ex2.Init(S2,TopAbs_SOLID); ex2.More(); ex2.Next()) {
      const TopoDS_Shape& so2 = ex2.Current();
      DS.FillShapesSameDomain(so1,so2);
    }
  }
  
  myS1 = S1;
  myS2 = S2;

}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::Intersect()
{
  myRecomputeBuilderIsDone = Standard_False;
  
  if(!myS1.IsNull() && !myS2.IsNull())
    myDSFiller.Insert(myS1, myS2, myHDS);
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::Intersect
(const TopoDS_Shape& S1,
 const TopoDS_Shape& S2)
{
  myRecomputeBuilderIsDone = Standard_False;
  
  if(S1.IsNull() || S2.IsNull()) {
    return;
  }
  
  Standard_Boolean orientFORWARD = Standard_False;
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
  myDSFiller.Insert(S1, S2, myHDS, orientFORWARD);
}

//=======================================================================
//function : SameDomain
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::SameDomain
(const TopoDS_Shape& S1,
 const TopoDS_Shape& S2)
{
  myRecomputeBuilderIsDone = Standard_False;
  
  if(S1.IsNull() || S2.IsNull())
    return;

  TopExp_Explorer exp1, exp2;
    exp1.Init(S1, TopAbs_FACE);
    if(!exp1.More())
      return;
    exp2.Init(S2, TopAbs_FACE);
    if(!exp2.More())
      return;
  
  myDSFiller.Insert2d(S1, S2, myHDS);
}


// Construction of Sections

#define FindKeep Standard_False

//=======================================================================
//function : GetSectionEdgeSet
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_DSAccess::GetSectionEdgeSet
(const TopoDS_Shape& S1,
 const TopoDS_Shape& S2)
{
  GetSectionEdgeSet();

  // Check if S1 and S2 contain faces
  TopExp_Explorer exp1, exp2;
  exp1.Init(S1, TopAbs_FACE);
  if(!exp1.More())
    return myEmptyListOfShape;
  exp2.Init(S2, TopAbs_FACE);
  if(!exp2.More())
    return myEmptyListOfShape;
  
  for(exp1.Init(S1, TopAbs_FACE); exp1.More(); exp1.Next()) {
    if(!myHDS->HasShape(exp1.Current(), FindKeep))
      return myEmptyListOfShape;
  }
  for(exp2.Init(S2, TopAbs_FACE); exp2.More(); exp2.Next())
    if(!myHDS->HasShape(exp2.Current(), FindKeep))
      return myEmptyListOfShape;
  
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  TopOpeBRepBuild_Builder& Builder = myHB->ChangeBuilder();
  
  // The purpose is to find interferences associated with faces,
  // edges that come from their Geometry (= Edge || Curve)
  TopTools_ListOfShape LE;
  LE.Clear();
  TopExp_Explorer exp;
  for(exp1.Init(S1, TopAbs_FACE); exp1.More(); exp1.Next()) {
    const TopoDS_Shape& F1 = exp1.Current();
    
    TopOpeBRepDS_ListOfInterference& lof = DS.ChangeShapeInterferences(F1);
    TopOpeBRepDS_InterferenceIterator li(lof);
    li.SupportKind(TopOpeBRepDS_FACE);
    for(exp2.Init(S2, TopAbs_FACE); exp2.More(); exp2.Next()) {
      const TopoDS_Shape& F2 = exp2.Current();
      Standard_Integer si = DS.Shape(F2, FindKeep);
      li.Support(si);
      
      for(; li.More(); li.Next()) {
	const TopOpeBRepDS_Interference& CurrInt = li.Value();
	TopOpeBRepDS_Kind gk = CurrInt.GeometryType();
	Standard_Integer gi = CurrInt.Geometry();
	const TopoDS_Shape& geosha = DS.Shape(gi, FindKeep);
	if(gk == TopOpeBRepDS_CURVE) {
	  const TopTools_ListOfShape& lEdge = myHB->NewEdges(gi);
	  LE.Append(lEdge.First());
	} else {
	  const TopTools_ListOfShape& lEdge = Builder.Splits(geosha, TopAbs_ON);
	  TopTools_ListIteratorOfListOfShape it(lEdge);
	  for(; it.More(); it.Next()) {
	    const TopoDS_Shape& CurrEdge = it.Value();
	    Standard_Integer ipv1, ipv2;
	    TopOpeBRepDS_Kind pvk1, pvk2;
	    PntVtxOnSectEdge(CurrEdge, ipv1, pvk1, ipv2, pvk2);
	    if(pvk1 != TopOpeBRepDS_VERTEX) {
	      ipv1 = 0;
	      if(pvk2 != TopOpeBRepDS_VERTEX) continue;
	    } else {
	      if(pvk2 != TopOpeBRepDS_VERTEX)
		ipv2 = 0;
	    }
	    for(exp.Init(F1, TopAbs_VERTEX); exp.More(); exp.Next()) {
	      Standard_Integer iVert = DS.Shape(exp.Current());
	      if(iVert) {
		if((iVert == ipv1) || (iVert == ipv2)) {
		  LE.Append(CurrEdge);
		  break;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  // find all groups of connected Edges associated to LE
  TopTools_ListIteratorOfListOfShape ILE;
  myCurrentList.Clear();
  TopTools_MapOfShape ME;
  ME.Clear();
  TopTools_ListIteratorOfListOfShape ILC;
  TopExp_Explorer ECE;
  ILE.Initialize(LE);
  for(;ILE.More();ILE.Next()) {
    const TopoDS_Shape& E = ILE.Value();
    ILC.Initialize(myListOfCompoundOfEdgeConnected);
    for(;ILC.More();ILC.Next()) {
      const TopoDS_Shape& Com = ILC.Value();
      ECE.Init(Com, TopAbs_EDGE);
      for(;ECE.More();ECE.Next()) {
	if(ECE.Current().IsSame(E)) {
	  if(!ME.Contains(Com)) {
	    myCurrentList.Append(Com);
	    ME.Add(Com);
	    break;
	  }
	}
      }
    }
  }
  
  return myCurrentList;
}

//=======================================================================
//function : GetSectionEdgeSet
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_DSAccess::GetSectionEdgeSet()
{
  if(!myRecomputeBuilderIsDone) {
    // it is possible to call the method many times consecutively
    myHDS->AddAncestors(myS1);
    // start of lpa modification
    if (!myS1.IsSame(myS2) && !myS2.IsNull()) {
      myHDS->AddAncestors(myS2);
      myHB->Perform(myHDS,myS1,myS2);
    }
    else {
      myHB->Perform(myHDS);
    }
    // end of modif lpa
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
  myHB->InitSection();
  for(; myHB->MoreSection(); myHB->NextSection()) {
    const TopoDS_Edge& ES = TopoDS::Edge(myHB->CurrentSection());
    if(ME.Contains(ES)) continue;
    ME.Add(ES);
    EC->AddStart(ES);
  }
  TopTools_ListOfShape& LW = EC->MakeBlock();
  
  // the wires are tranformed into compounds.
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
//function : IsWire
//purpose  : 
//=======================================================================

Standard_Boolean BRepAlgo_DSAccess::IsWire(const TopoDS_Shape& S)
{
  Standard_Boolean b = Standard_False;
  if(myEC->IsDone()) {
    if (myCompoundWireMap.IsBound(S))
      b = myEC->IsWire(myCompoundWireMap(S));
  }
  return b;
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_DSAccess::Wire(const TopoDS_Shape& S)
{
  if(!IsWire(S)) {
    myWire.Nullify();
  }
  else {
    BRep_Builder BB;
    BB.MakeWire(myWire);
    TopExp_Explorer Explor(S, TopAbs_EDGE);
    for(;Explor.More(); Explor.Next()) BB.Add(myWire, Explor.Current());
  }
  return myWire;
}

//=======================================================================
//function : SectionVertex
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_DSAccess::SectionVertex
(const TopoDS_Shape& F,
 const TopoDS_Shape& E)
{
  myListOfVertex.Clear();
  if(F.ShapeType() != TopAbs_FACE || E.ShapeType() != TopAbs_EDGE)
    return myListOfVertex;
  Standard_Integer iF = myHDS->Shape(F), iE = myHDS->Shape(E);
  if((iF == 0) || (iE == 0))
    return myListOfVertex;

  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  const TopOpeBRepDS_ListOfInterference& LI = 
    DS.ShapeInterferences(E, Standard_False);
  TopOpeBRepDS_InterferenceIterator II(LI);
  Standard_Integer goodIndex = 0;
  TopOpeBRepDS_Kind goodKind;
  for(;II.More();II.Next()) {
    const Handle(TopOpeBRepDS_Interference)& I = II.Value();
    const TopOpeBRepDS_Transition& T = I->Transition();
    if((T.ONAfter() == TopAbs_FACE) &&
       (T.IndexAfter()  == iF)) {
      goodKind  = I->GeometryType();
      goodIndex = I->Geometry();
      if(goodKind == TopOpeBRepDS_VERTEX)
	myListOfVertex.Append(myHDS->Shape(goodIndex));
      else 
	if (goodKind == TopOpeBRepDS_POINT)
	  myListOfVertex.Append(myHB->NewVertex(goodIndex));
    }
  }
  return myListOfVertex;
}


// Editing of the DS

//=======================================================================
//function : SuppressEdgeSet
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::SuppressEdgeSet
(const TopoDS_Shape& C)
{
  // It is checked if C really is a Coumpound of connected Edges

  myHB->InitExtendedSectionDS();
//  myGetSectionIsDone = Standard_False;

  TopTools_ListIteratorOfListOfShape LLS(myListOfCompoundOfEdgeConnected);
  for(;LLS.More(); LLS.Next())
    if(C == LLS.Value())
      break;
  if(!LLS.More())
    return;
  
  // Cleaning
  TopoDS_Shape Empty;
  Empty.Nullify();
  Suppress(C, Empty);
  myListOfCompoundOfEdgeConnected.Remove(LLS);
}

//=======================================================================
//function : ChangeEdgeSet
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::ChangeEdgeSet
(const TopoDS_Shape& Old, const TopoDS_Shape& New)
{
  // It is checked if Old is a Coumpound of connected Edges

  myHB->InitExtendedSectionDS();

  TopTools_ListIteratorOfListOfShape LLS(myListOfCompoundOfEdgeConnected);
  for(;LLS.More(); LLS.Next())
    if(Old == LLS.Value())
      break;
  if(!LLS.More())
    return;

  // The compound of Edges to be rotated is constructed
  BRep_Builder B;
  Standard_Boolean Trouve;
  Standard_Integer iC;
  TopoDS_Compound C;
  TopoDS_Edge E;
  B.MakeCompound(C);
  TColStd_PackedMapOfInteger RPoint; //The points to be controlled 
  
 TopOpeBRepDS_ListIteratorOfListOfInterference iter;
  TopExp_Explorer exp(Old, TopAbs_EDGE);
  TopExp_Explorer exp2;
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& Edge = exp.Current(); 
    for(exp2.Init(New, TopAbs_EDGE), Trouve=Standard_False;
	exp2.More() && (!Trouve); exp2.Next()) {
      E = TopoDS::Edge(exp2.Current());
      Trouve = E.IsSame(Edge);
    }

    if (!Trouve) B.Add(C, Edge); // Edge to be removed
    else if (!E.IsEqual(Edge)) {
      // It is necessary to change Interferences => take the complement
      iC = myHB->GetDSCurveFromSectEdge(Edge);
      if (!iC) {
#ifdef OCCT_DEBUG
	cout << "Warning DSAccess: Modifications of Edge are not implemented" << endl;
#endif
      }
      else {
	// Complete the interferences Curve/Face
	Standard_Integer iF;
	Handle(TopOpeBRepDS_Interference) interf;

	iF = myHB->GetDSFaceFromDSCurve(iC, 1);	
	TopOpeBRepDS_ListOfInterference& list1 = 
	   myHDS->ChangeDS().ChangeShapeInterferences(iF);
	for(iter.Initialize(list1); iter.More(); iter.Next()) {
	  interf = iter.Value();
	  if (interf->Geometry() == iC)
	    interf->Transition(interf->Transition().Complement());
	}
	iF = myHB->GetDSFaceFromDSCurve(iC, 2);	
	TopOpeBRepDS_ListOfInterference& list2 = 
	  myHDS->ChangeDS().ChangeShapeInterferences(iF);
	for(iter.Initialize(list2); iter.More(); iter.Next()) {
	  interf = iter.Value();
	  if (interf->Geometry() == iC)
	    interf->Transition(interf->Transition().Complement());
	}
	// The associated points are recorded
	Standard_Integer ipv1, ipv2;
	//Standard_Boolean bid; // skl
	TopOpeBRepDS_Kind k1, k2;	
	PntVtxOnCurve(iC, ipv1, k1, ipv2, k2);
	if (ipv1 != 0) /*bid = */RPoint.Add(ipv1); // skl
	if (ipv2 != 0) /*bid = */RPoint.Add(ipv2); // skl
      }
    }
  }


  // Cleaning
  Suppress(C, New);

  // Is it necessary to invert the Interferences "Edge on Fa"
  if (!RPoint.IsEmpty()) {
    const TopOpeBRepDS_DataStructure & DS = myHDS->DS();
    Standard_Integer iP,iE, nbShape = DS.NbShapes();
    Handle(TopOpeBRepDS_Interference) interf;
    for (iE=1; iE<=nbShape; iE++) {
      if (DS.Shape(iE,0).ShapeType() == TopAbs_EDGE) { 
	const TopOpeBRepDS_ListOfInterference& List = 
	  myHDS->DS().ShapeInterferences(iE);
	for(iter.Initialize(List); iter.More(); iter.Next()) {
	  interf = iter.Value();
	  if (interf->GeometryType() == TopOpeBRepDS_POINT) {
	    iP = interf->Geometry();
	    if (RPoint.Contains(iP))
	      interf->Transition(interf->Transition().Complement());
	  }
	}
      }
    }
  }

  // The old is replaced by new
  LLS.Value() = New;
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::Suppress(const TopoDS_Shape& C,
				 const TopoDS_Shape& Keep)
{
 TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
 TopOpeBRepBuild_Builder& Builder = myHB->ChangeBuilder();
 Standard_Integer i, iC = 0, iF1, iF2,iE1, iE2;
// TopOpeBRepDS_ListIteratorOfListOfInterference;
 TColStd_ListIteratorOfListOfInteger it1, it2;

  //A group of points to be kept is constructed
 mySetOfKeepPoint.Clear();
 if (!Keep.IsNull()) {
   //Standard_Boolean B; // skl
   Standard_Integer ipv1, ipv2;
   TopOpeBRepDS_Kind k1, k2;
   TopExp_Explorer exp(Keep, TopAbs_EDGE);
   for(; exp.More(); exp.Next()) {
     const TopoDS_Shape& SectEdge = exp.Current();
     iC = myHB->GetDSCurveFromSectEdge(SectEdge);
     if(!iC) 
       PntVtxOnSectEdge(SectEdge, ipv1, k1, ipv2, k2);
     else 
       PntVtxOnCurve(iC, ipv1, k1, ipv2, k2);
     if (ipv1 != 0) /*B = */mySetOfKeepPoint.Add(ipv1); // skl
     if (ipv2 != 0) /*B = */mySetOfKeepPoint.Add(ipv2); // skl
   }
 }

  // The curves, which generated the the Edges, are found
  // during the parsing the Edges which come from Edge are found
  // (= MapOfInteger : ESE)
  
  // First, the interferences of support 1d.
  TopExp_Explorer exp(C, TopAbs_EDGE);
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& SectEdge = exp.Current();
    iC = myHB->GetDSCurveFromSectEdge(SectEdge);
    if(!iC) {
      // the Edges that come from Edge are processed
      // the interferences connected with Edges are processed :
      iE1 = myHB->GetDSEdgeFromSectEdge(SectEdge, 1);
      iE2 = myHB->GetDSEdgeFromSectEdge(SectEdge, 2);
      
      RemoveEdgeInterferences(iE1,iE2,SectEdge);
      
      TColStd_ListOfInteger& loi11 = myHB->GetDSFaceFromDSEdge(iE1, 1);
      TColStd_ListOfInteger& loi12 = myHB->GetDSFaceFromDSEdge(iE1, 2);
      for(it1.Initialize(loi11); it1.More(); it1.Next()) {
	iF1 = it1.Value();
	for(it2.Initialize(loi12); it2.More(); it2.Next()) {
	  iF2 = it2.Value();
	  // similar to the case of SectEdges coming from curve.
	  RemoveEdgeInterferences(iF1,iF2,SectEdge);
	}
      }
      TColStd_ListOfInteger& loi21 = myHB->GetDSFaceFromDSEdge(iE2, 1);
      TColStd_ListOfInteger& loi22 = myHB->GetDSFaceFromDSEdge(iE2, 2);
      for(it1.Initialize(loi21); it1.More(); it1.Next()) {
	iF1 = it1.Value();
	for(it2.Initialize(loi22); it2.More(); it2.Next()) {
	  iF2 = it2.Value();
	  // similar to the case of  SectEdges coming from curve.
	  RemoveEdgeInterferences(iF1,iF2,SectEdge);
	}
      }
      continue;
    }    
    // The Edges that come from Curve are processed
    iF1 = myHB->GetDSFaceFromDSCurve(iC, 1);
    iF2 = myHB->GetDSFaceFromDSCurve(iC, 2);
    
    RemoveEdgeInterferences(iF1, iF2, iC);
    DS.ChangeKeepCurve(iC, FindKeep);
  }
  
  // Secondly, the interferences of 2D support.
  exp.Init(C, TopAbs_EDGE);
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& SectEdge = exp.Current();
    iC = myHB->GetDSCurveFromSectEdge(SectEdge);
    if(!iC) {
      iE1 = myHB->GetDSEdgeFromSectEdge(SectEdge, 1);
      iE2 = myHB->GetDSEdgeFromSectEdge(SectEdge, 2);
      TColStd_ListOfInteger& loi11 = myHB->GetDSFaceFromDSEdge(iE1, 1);
      TColStd_ListOfInteger& loi12 = myHB->GetDSFaceFromDSEdge(iE1, 2);
      for(it1.Initialize(loi11); it1.More(); it1.Next()) {
	iF1 = it1.Value();
	for(it2.Initialize(loi12); it2.More(); it2.Next()) {
	  iF2 = it2.Value();
	  if(iF1 == iF2)
	    continue;
	  RemoveFaceInterferences(iF1, iF2, iE1, iE2);
	}
      }
      TColStd_ListOfInteger& loi21 = myHB->GetDSFaceFromDSEdge(iE2, 1);
      TColStd_ListOfInteger& loi22 = myHB->GetDSFaceFromDSEdge(iE2, 2);
      for(it1.Initialize(loi21); it1.More(); it1.Next()) {
	iF1 = it1.Value();
	for(it2.Initialize(loi22); it2.More(); it2.Next()) {
	  iF2 = it2.Value();
	  if(iF1 == iF2)
	    continue;
	  RemoveFaceInterferences(iF1, iF2, iE1, iE2);
	}
      }
    }
    else {
      iF1 = myHB->GetDSFaceFromDSCurve(iC, 1);
      iF2 = myHB->GetDSFaceFromDSCurve(iC, 2);
      
      RemoveFaceInterferences(iF1, iF2, iC);
    }
  }
  
  // Thirdly, RemoveSameDomain is done for the faces that contain all Edges of C,
  // and are SameDomain and without Geometry.
  
  RemoveFaceSameDomain(C);
  
  // Fourthly, the faces, that were not concerned, are removed
  Standard_Integer NbSh = DS.NbShapes();
  for(i = 1; i <= NbSh; i++) {
    const TopoDS_Shape& Face = DS.Shape(i);
    if(Face.IsNull())
      continue;
    if((Face.ShapeType() != TopAbs_FACE) || DS.HasGeometry(Face) ||
       (myHDS->HasSameDomain(Face)))
      continue;
    for(exp.Init(Face, TopAbs_EDGE); exp.More(); exp.Next()){
      const TopoDS_Shape& Edge = exp.Current();
      if(DS.HasShape(Edge))
	break;
    }
    if(exp.More())
      continue;
    DS.ChangeKeepShape(Face, Standard_False);
  }
  
  // Builder.myKPMAPf1f2 is reconstructed
  Builder.FindIsKPart(); 
  
  //  The Edges of section are removed from Builder.mySplitON
  exp.Init(C, TopAbs_EDGE);
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& SectE= exp.Current();
    TopTools_ListOfShape& losob = Builder.ChangeSplit(SectE, TopAbs_ON);
    losob.Clear();
  }
}

//=======================================================================
//function : SuppressSectionVertex
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::SuppressSectionVertex
(const TopoDS_Vertex& /*V*/)
{
  if(!myRecomputeBuilderIsDone)
    return;
}


// Reconstruction of Shapes

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_DSAccess::Merge
(const TopAbs_State state1,
 const TopAbs_State state2)
{
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
  GetSectionEdgeSet();
  
  myHB->Clear();
  myHB->MergeShapes(myS1,state1,myS2,state2);
  const TopTools_ListOfShape& L1 = myHB->Merged(myS1,state1);
  
  BRep_Builder BB;
  myResultShape.Nullify();
  BB.MakeCompound(TopoDS::Compound(myResultShape));
  TopTools_ListIteratorOfListOfShape it(L1);
  for(;it.More(); it.Next()) {
    BB.Add(myResultShape, it.Value());
  }
  return myResultShape;
}

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_DSAccess::Merge
(const TopAbs_State state1)
{
  if((state1 != TopAbs_IN) &&
     (state1 != TopAbs_OUT))
    return myEmptyShape;
  GetSectionEdgeSet();

  myHB->Clear();
  myHB->MergeSolid(myS1,state1);
  const TopTools_ListOfShape& L1 = myHB->Merged(myS1,state1);
  
  BRep_Builder BB;
  myResultShape.Nullify();
  BB.MakeCompound(TopoDS::Compound(myResultShape));
  TopTools_ListIteratorOfListOfShape it(L1);
  for(;it.More(); it.Next()) {
    BB.Add(myResultShape, it.Value());
  }
  return myResultShape;
}

//=======================================================================
//function : Propagate
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_DSAccess::Propagate
(const TopAbs_State what,
 const TopoDS_Shape& /*fromShape*/,
 const TopoDS_Shape& /*LoadShape*/)
{
  if((what != TopAbs_IN) &&
     (what != TopAbs_OUT))
    return myEmptyShape;
  if(!myRecomputeBuilderIsDone)
    return myEmptyShape;

//  myHB->MergeShapes(myS1,t1,myS2,t2);

  //POP for NT;
  static TopoDS_Shape bid;
  return bid;
}

//=======================================================================
//function : PropagateFromSection
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepAlgo_DSAccess::PropagateFromSection
(const TopoDS_Shape& SectionShape)
{
  GetSectionEdgeSet();
  TopTools_ListIteratorOfListOfShape ils(myListOfCompoundOfEdgeConnected);
  TopExp_Explorer exp;
  for(; ils.More(); ils.Next()) {
    const TopoDS_Shape& SetEdgSet = ils.Value();
    exp.Init(SetEdgSet, TopAbs_EDGE);
    for(; exp.More(); exp.Next()) {
      if(SectionShape.IsSame(exp.Current()))
	return SetEdgSet;
    }
  }
  return myEmptyShape;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_DSAccess::Modified (const TopoDS_Shape& Shape) 
{
  myModified.Clear() ;

//  Handle(TopOpeBRepBuild_HBuilder)& HBuilder = myDSA.myHB ;
  TopTools_ListIteratorOfListOfShape Iterator ;
  
  if (myHB->IsSplit (Shape, TopAbs_OUT)) {
    for (Iterator.Initialize (myHB->Splits (Shape, TopAbs_OUT)) ;
	 Iterator.More() ;
	 Iterator.Next()) {
      myModified.Append (Iterator.Value()) ;
    }
  }
  if (myHB->IsSplit (Shape, TopAbs_IN)) {
    for (Iterator.Initialize (myHB->Splits (Shape, TopAbs_IN)) ;
	 Iterator.More() ;
	 Iterator.Next()) {
      myModified.Append (Iterator.Value()) ;
    }
  }
  if (myHB->IsSplit (Shape, TopAbs_ON)) {
    for (Iterator.Initialize (myHB->Splits (Shape, TopAbs_ON)) ;
	 Iterator.More() ;
	 Iterator.Next()) {
      myModified.Append (Iterator.Value()) ;
    }
  }

  if (myHB->IsMerged (Shape, TopAbs_OUT)) {
    for (Iterator.Initialize (myHB->Merged (Shape, TopAbs_OUT)) ;
	 Iterator.More() ;
	 Iterator.Next()) {
      myModified.Append (Iterator.Value()) ;
    }
  }
  if (myHB->IsMerged(Shape, TopAbs_IN)) {
    for (Iterator.Initialize (myHB->Merged (Shape, TopAbs_IN)) ;
	 Iterator.More() ;
	 Iterator.Next()) {
      myModified.Append (Iterator.Value()) ;
    }
  }
  if (myHB->IsMerged(Shape, TopAbs_ON)) {
    for (Iterator.Initialize (myHB->Merged (Shape, TopAbs_ON)) ;
	 Iterator.More() ;
	 Iterator.Next()) {
      myModified.Append (Iterator.Value()) ;
    }
  }

  return myModified ;
}



//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

BRepAlgo_CheckStatus BRepAlgo_DSAccess::Check()
{
//  TopOpeBRepDS_Check Ck(HDS);
// to be precised : in Ck, there is a possibility to know 
// exactly the n*n of shapes/points/curves/surfaces, 
// which are not correct in the DS.
//  Standard_Boolean IsOK = Ck.ChkIntgSamDom() ;
//  IsOK = IsOK && Ck.OneVertexOnPnt();
//  IsOK = IsOK && Ck.ChkIntg();
//  if(IsOK)
//    return TopOpeBRepDS_OK;
  return BRepAlgo_NOK;
}

//=======================================================================
//function : RemoveEdgeInterferences
//purpose  : case of SectEdge coming from Edge(s)
//       
//     if iE1 and iE2 are Edges :
//     Remove interferences of DSEdge(= iE1 or iE2) of
//     geometry a vertex of SectEdge, and if there is nothing else, 
//     make unkeep on DSEdge 
//     if iE1 or iE2 == 0, no interference on Edges in the DS 
//     NYI : management of SameDomain
//       
//     if iE1 and iE2 are Faces :
//     for each of faces F1 and F2, explode into Edges
//     for each Edge :
//	     remove the interferences of a SectEdge vertex
//	     on geometry. If there is no other interferences attached to 
//           these Edges, and if these Edges are not SameDomain,
//           make unKeepShape.
//=======================================================================

void BRepAlgo_DSAccess::RemoveEdgeInterferences
(const Standard_Integer iE1,
 const Standard_Integer iE2,
 const TopoDS_Shape& SectEdge)
{
  if(!iE1 || !iE2)
    return;

  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  TopOpeBRepDS_Kind kind1, kind2;
  TopExp_Explorer exp(SectEdge, TopAbs_VERTEX);
  Standard_Integer i = 1, ipv1, ipv2;
  
  // the Vertex/Points of SectEdge are retrieved
  PntVtxOnSectEdge(SectEdge, ipv1, kind1, ipv2, kind2);
  
  const TopoDS_Shape& Shape = DS.Shape(iE1, FindKeep);
  if(Shape.IsNull())
    return;
  if(Shape.ShapeType() == TopAbs_FACE) {
    Standard_Integer iF1 = iE1, iF2 = iE2;
    RemoveEdgeInterferencesFromFace(iF1, iF2, ipv1, kind1, ipv2, kind2);
    return;
  }
  else if(Shape.ShapeType() != TopAbs_EDGE)
    return;
  
  // the interferences are taken from the DS
  TopOpeBRepDS_ListIteratorOfListOfInterference lioloi;
  TopOpeBRepDS_Kind gk;
  Standard_Integer iCurrE1, iCurrE2, gi;
//  Standard_Boolean RemInterf;
  
  for(i = 1; i <= 2; i++) {
    iCurrE1 = ((i == 1) ? iE1 : iE2);
    iCurrE2 = ((i == 1) ? iE2 : iE1);
    const TopoDS_Shape& DSEdge = DS.Shape(iCurrE1, FindKeep);
    if(DSEdge.IsNull())
      continue;
    TopOpeBRepDS_ListOfInterference& loi = 
      DS.ChangeShapeInterferences(DSEdge);
    //    RemInterf = Standard_True;
    for(lioloi.Initialize(loi); lioloi.More(); lioloi.Next()) {
      Handle(TopOpeBRepDS_Interference) I = lioloi.Value();
      if (I.IsNull()) continue;
      if((I->SupportType() != TopOpeBRepDS_EDGE) ||
	 (I->Support() != iCurrE2)) {
	//	RemInterf = Standard_False;//debug ...
	continue;
      }
      gk = I->GeometryType();
      gi = I->Geometry();
      if(gk == kind1) {
	if(gi == ipv1) {
	  DS.RemoveShapeInterference(DSEdge, I);
	  if(!DS.HasGeometry(DSEdge)) {
	    //	    if(RemInterf || (!lioloi.More())) {
	    RemoveEdgeSameDomain(iCurrE1, iCurrE2); // NYI : SameDomain
	    DS.ChangeKeepShape(iCurrE1, FindKeep);
	    //	  } 
	  } 
	}
      }
      else if(gk == kind2) {
	if(gi == ipv2) {
	  DS.RemoveShapeInterference(DSEdge, I);
	  if(!DS.HasGeometry(DSEdge)) {
	    //	    if(RemInterf || (!lioloi.More())) {//debug
	    RemoveEdgeSameDomain(iCurrE1, iCurrE2); // NYI : SameDomain
	    DS.ChangeKeepShape(iCurrE1, FindKeep);
	    //	  } 
	  }
	}
      }
    }
  }
}

//=======================================================================
//function : RemoveEdgeInterferences
//purpose  : case of SectEdge coming from Curve
//           for each of faces F1 and F2, explode into Edges
//	     for each Edge :
//	     remove the interferences that have a vertex of SectEdge
//           as a geometry. If no other interferences are attached to  
//           these Edges, and if the Edges are not SameDomain,
//           make unKeepShape.
//=======================================================================

void BRepAlgo_DSAccess::RemoveEdgeInterferences
(const Standard_Integer iF1,
 const Standard_Integer iF2,
 const Standard_Integer iCurve)
{
  TopOpeBRepDS_Kind gk1, gk2;
  Standard_Integer gi1, gi2;
  
  PntVtxOnCurve(iCurve, gi1, gk1, gi2, gk2);

  if (!mySetOfKeepPoint.IsEmpty()) {
    if (mySetOfKeepPoint.Contains(gi1)) gi1 = 0;
    if (mySetOfKeepPoint.Contains(gi2)) gi2 = 0;   
  }
  
  if (gi1 || gi2)
    RemoveEdgeInterferencesFromFace(iF1, iF2, gi1, gk1, gi2, gk2);
}

//=======================================================================
//function : RemoveFaceInterferences
//purpose  : case of SectEdge coming from Edge(s)
//        Remove interferences between F1 and F2 concerning 
//        DSEdge (= E1 or E2) :
//		a) if DSEdge is not SameDomain -> the edge is Removed
//		b) if among other interferences of DSEdge of 
//                 GeomtryType == VERTEX, there is none 
//                 with Edge of DSFace(= F1 or F2)
//	  if DSFace has no more interferences and is not SameDomain,
//        make unkeep DSFace.
//=======================================================================

void BRepAlgo_DSAccess::RemoveFaceInterferences
(const Standard_Integer iF1,
 const Standard_Integer iF2,
 const Standard_Integer iE1,
 const Standard_Integer iE2)
{
  if(!iF1 || !iF2)
    return;
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  TopOpeBRepDS_ListIteratorOfListOfInterference lioloi;//, lioloei, liolofi;
  TopTools_ListIteratorOfListOfShape liolos;
  TopOpeBRepDS_Kind gk;
  TopExp_Explorer exp;
  Standard_Integer i, iCurrF1, iCurrF2, j, iCurrE1, /*iCurrE2,*/ gi; // skl
  Standard_Boolean RemInterf;
  
  for(i = 1; i <= 2; i++) {
    iCurrF1 = ((i == 1) ? iF1 : iF2);
    iCurrF2 = ((i == 1) ? iF2 : iF1);
    const TopoDS_Shape& DSFace = DS.Shape(iCurrF1);
    if(DSFace.IsNull())
      continue;
    const TopOpeBRepDS_ListOfInterference& loi = DS.ShapeInterferences(DSFace);
    for(lioloi.Initialize(loi); lioloi.More(); lioloi.Next()) {
      Handle(TopOpeBRepDS_Interference) I = lioloi.Value();
      if (I.IsNull()) continue;
      if((I->SupportType() != TopOpeBRepDS_FACE) ||
	 (I->Support() != iCurrF2)) {
	continue;
      }
      gk = I->GeometryType();
      gi = I->Geometry();
      if(gk != TopOpeBRepDS_EDGE) continue;
      for(j = 1; j <= 2; j++) {
	iCurrE1 = ((j == 1) ? iE1 : iE2);
	//iCurrE2 = ((j == 1) ? iE2 : iE1); // skl
	if(gi != iCurrE1) continue;
	// a) if DSEdge is not SameDomain -> the interference is Removed
	//    et DSEdge
	const TopoDS_Shape& DSEdge = DS.Shape(iCurrE1, FindKeep);
	if(DSEdge.IsNull())
	  continue;
	if(!myHDS->HasSameDomain(DSEdge)) {
	  if(!DS.HasGeometry(DSEdge)) {
	    DS.RemoveShapeInterference(DSFace, I);
	    DS.ChangeKeepShape(DSEdge, FindKeep);
	  } else {
	    // NYI : manage the case when the geometry of DSEdge 
	    // NYI : is not connected anyhow with two faces
	  }
	  if(!DS.HasGeometry(DSFace)) {
	    DS.ChangeKeepShape(DSFace, FindKeep);
	  }
	  continue;
	}
	// b) if no Edges of  SameDomain(DSEdge),
	//    belong to DSFace(= F1 or F2)
	//     -> the interference is removed
	const TopoDS_Shape& Edge = DS.Shape(iCurrE1, FindKeep);
	if(Edge.IsNull())
	  continue;
	const TopTools_ListOfShape& loe = DS.ShapeSameDomain(Edge);
	RemInterf = Standard_True;
	for(liolos.Initialize(loe); liolos.More(); liolos.Next()) {
	  const TopoDS_Shape& ESD = liolos.Value();
	  for(exp.Init(DSFace, TopAbs_EDGE); exp.More(); exp.Next()) {
	    if(ESD.IsSame(exp.Current())) {
	      RemInterf = Standard_False;
	      break;
	    }
	  }
	  if(!RemInterf) break;
	}
	if(RemInterf) {
	  //	  RemoveSameDomain(iCurrF1, iCurrF2);
	  
	  if(!DS.HasGeometry(DSFace)) {
	    if(!myHDS->HasSameDomain(DSFace))
	      DS.ChangeKeepShape(DSFace, FindKeep);
	  }
	}
	if(!DS.HasGeometry(DSFace) && !myHDS->HasSameDomain(DSFace))
	  DS.ChangeKeepShape(DSFace, FindKeep);
      }
    }
  }
}

//=======================================================================
//function : RemoveFaceInterferences
//purpose  : case of SectEdge from Curve
//           remove interferences of Geometry iCurve between F1 and F2.
//           if Face(= F1 or F2) has noother interference, and if Face
//           is not SameDomain, make unKeepShape Face.
//=======================================================================

void BRepAlgo_DSAccess::RemoveFaceInterferences
(const Standard_Integer iF1,
 const Standard_Integer iF2,
 const Standard_Integer iCurve)
{
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  TopOpeBRepDS_ListIteratorOfListOfInterference lioloi;
  TopOpeBRepDS_Kind gk;
  Standard_Integer i, iCurrF1, iCurrF2, gi;

  for(i = 1; i <= 2; i++) {
    iCurrF1 = ((i == 1) ? iF1 : iF2);
    iCurrF2 = ((i == 1) ? iF2 : iF1);
    const TopoDS_Shape& DSFace = DS.Shape(iCurrF1);
    const TopOpeBRepDS_ListOfInterference& loi = DS.ShapeInterferences(DSFace);
    for(lioloi.Initialize(loi); lioloi.More(); lioloi.Next()) {
      Handle(TopOpeBRepDS_Interference) I = lioloi.Value();
      if (I.IsNull()) continue;
      if((I->SupportType() != TopOpeBRepDS_FACE) ||
	 (I->Support() != iCurrF2)) {
	break;;
      }
    }
    for(lioloi.Initialize(loi); lioloi.More(); lioloi.Next()) {
      Handle(TopOpeBRepDS_Interference) I = lioloi.Value();
      if (I.IsNull()) continue;
      if((I->SupportType() != TopOpeBRepDS_FACE) ||
	 (I->Support() != iCurrF2)) {
	continue;
      }
      gk = I->GeometryType();
      gi = I->Geometry();
      if(gk != TopOpeBRepDS_CURVE) continue;
      if(gi != iCurve) continue;
      DS.RemoveShapeInterference(DSFace, I);
//      const TopoDS_Shape& interferenceface = DS.Shape(iCurrF2);
//      DS.RemoveShapeInterference(interferenceface, I);
      if(!DS.HasGeometry(DSFace)) {
	const TopTools_ListOfShape& los = DS.ShapeSameDomain(DSFace);
	if(los.IsEmpty())
	  DS.ChangeKeepShape(DSFace, FindKeep);
      }
//      if(!DS.HasGeometry(interferenceface)) {
//	const TopTools_ListOfShape& los = DS.ShapeSameDomain(interferenceface);
//	if(los.IsEmpty())
//	  DS.ChangeKeepShape(interferenceface, FindKeep);
//      }
    }
  }
}

//=======================================================================
//function : RemoveEdgeInterferencesFromFace
//purpose  : Remove interferences of Edges from iF1 or iF2
//           that have GeometryType kind1/kind2 and 
//           Geometry ipv1/ipv2.
//           if kind1/kind2 == TopAbs_VERTEX -> RemoveEdgeFromFace
//=======================================================================

void BRepAlgo_DSAccess::RemoveEdgeInterferencesFromFace
(const Standard_Integer iF1,
 const Standard_Integer iF2,
 const Standard_Integer ipv1,
 const TopOpeBRepDS_Kind kind1,
 const Standard_Integer ipv2,
 const TopOpeBRepDS_Kind kind2)
{
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  TopOpeBRepDS_ListIteratorOfListOfInterference lioloi;
  TopExp_Explorer exp, exp2;
  TopOpeBRepDS_Kind sk, gk;
  Standard_Integer i, iCurrF1, iCurrF2, iE = 0, si, gi;

  for(i = 1; i <= 2; i++) {
    iCurrF1 = ((i == 1) ? iF1 : iF2);
    iCurrF2 = ((i == 1) ? iF2 : iF1);
    const TopoDS_Shape& DSFace = DS.Shape(iCurrF1, FindKeep);
    if(DSFace.IsNull())
      continue;
    exp.Init(DSFace, TopAbs_EDGE);
    for(; exp.More(); exp.Next()) {
      const TopoDS_Shape& DSEdge = exp.Current();
      iE = DS.Shape(DSEdge, FindKeep);
      if(!iE) continue;
      const TopOpeBRepDS_ListOfInterference& loi =
	DS.ShapeInterferences(DSEdge);
      for(lioloi.Initialize(loi); lioloi.More(); lioloi.Next()) {
	Handle(TopOpeBRepDS_Interference) I = lioloi.Value();
	if (I.IsNull()) continue;
	sk = I->SupportType();
	si = I->Support();
	if((sk != TopOpeBRepDS_FACE) || (si != iCurrF2)) {
	  if(sk != TopOpeBRepDS_EDGE)
	    continue;
	  const TopoDS_Shape& DSFace2 = DS.Shape(iCurrF2, FindKeep);
	  exp2.Init(DSFace2, TopAbs_EDGE);
	  for(; exp2.More(); exp2.Next()) {
	    if(si == DS.Shape(exp2.Current(), FindKeep))
	      break;
	  }
	  if(!exp2.More())
	    continue;
	}
	gk = I->GeometryType();
	gi = I->Geometry();
	if(gk == kind1) {
	  if(gi == ipv1) {
	    DS.RemoveShapeInterference(DSEdge, I);
//	    if(!DS.HasGeometry(DSEdge)) {
//	      const TopTools_ListOfShape& los = DS.ShapeSameDomain(DSEdge);
//	      if(los.IsEmpty()) {
//		DS.ChangeKeepShape(iE, FindKeep);
//	      }
//	    }
	  }
	  else if(gk == kind2) {
	    if(gi == ipv2) {
	      DS.RemoveShapeInterference(DSEdge, I);
//	      if(!DS.HasGeometry(DSEdge)) {
//		const TopTools_ListOfShape& los = DS.ShapeSameDomain(DSEdge);
//		if(los.IsEmpty()) {
//		  DS.ChangeKeepShape(iE, FindKeep);
//		}
//	      }
	    }
	  }
	  else continue;
	}
      }
    }
    if(kind1 == TopOpeBRepDS_VERTEX)
      RemoveEdgeFromFace(iCurrF1,ipv1);
    if(kind2 == TopOpeBRepDS_VERTEX)
      RemoveEdgeFromFace(iCurrF1,ipv2);
  }
}

//=======================================================================
//function : RemoveEdgeFromFace
//purpose  : Remove from DS the Edges, which belong to iF
//           and have iV as vertex if they do not have Geometry and
//           are not SameDomain.
//=======================================================================

void BRepAlgo_DSAccess::RemoveEdgeFromFace
(const Standard_Integer iF,
 const Standard_Integer iV)
{
  if(!iF || !iV)
    return;
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  const TopoDS_Shape& DSFace = DS.Shape(iF, FindKeep);
  const TopoDS_Shape& Vertex = DS.Shape(iV, FindKeep);
  if(DSFace.IsNull() || Vertex.IsNull())
    return;
  TopExp_Explorer exp(DSFace, TopAbs_EDGE), exp2;
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& Edge = exp.Current();
#ifdef OCCT_DEBUG
//    Standard_Integer iEdge2 = DS.Shape(Edge, FindKeep);
//    Standard_Integer iEdge3 = DS.Shape(Edge);
#endif
                              
    if(!DS.HasShape(Edge)) 
      continue;
    exp2.Init(Edge, TopAbs_VERTEX);
    for(; exp2.More(); exp2.Next()) {
#ifdef OCCT_DEBUG
//      Standard_Integer iEdge5 = DS.Shape(Vertex, FindKeep);
//      Standard_Integer iEdge4 = DS.Shape(Vertex);
//      Standard_Integer iEdge6 = DS.Shape(exp2.Current(), FindKeep);
//      Standard_Integer iEdge7 = DS.Shape(exp2.Current());
#endif
                                
      if(Vertex.IsSame(exp2.Current())) {
	if(!DS.HasGeometry(Edge)) {
	  const TopTools_ListOfShape& los = DS.ShapeSameDomain(Edge);
	  if(los.IsEmpty()) {
#ifdef OCCT_DEBUG
//	    Standard_Integer iEdge = DS.Shape(Edge);
#endif
	    DS.ChangeKeepShape(Edge, FindKeep);
	  }
	}
      }
    }
  }
}

//=======================================================================
//function : PntVtxOnCurve
//purpose  : To find the points/vertices on curves
//=======================================================================

void BRepAlgo_DSAccess::PntVtxOnCurve
(const Standard_Integer iCurve,
 Standard_Integer& ipv1,
 TopOpeBRepDS_Kind& pvk1,
 Standard_Integer& ipv2,
 TopOpeBRepDS_Kind& pvk2)
{
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  
  const TopOpeBRepDS_Curve& C = DS.Curve(iCurve);
  TopOpeBRepDS_Kind pvk; 
  Standard_Integer ipv, iMother = C.Mother(), igoodC = iCurve, comp = 0;
  if(iMother) igoodC = iMother;
//#ifndef OCCT_DEBUG
  TopOpeBRepDS_PointIterator PII = myHDS->CurvePoints(igoodC);
  TopOpeBRepDS_PointIterator& PIt = PII; // skl : I change "M_PI" to "PIt"
//#else
//  TopOpeBRepDS_PointIterator& PIt = myHDS->CurvePoints(igoodC);
//#endif
  for(;PIt.More(); PIt.Next()) {
    comp++;
    if(comp > 2)
      // Standard_Error ...
      return;
    ipv = PIt.Current();
    // a point or a vertex is removed from the DS
    if(PIt.IsPoint()) {
      pvk = TopOpeBRepDS_POINT;
      DS.ChangeKeepPoint(ipv, FindKeep);
    }
    else if(PIt.IsVertex()) {
      pvk = TopOpeBRepDS_VERTEX;
      DS.ChangeKeepShape(ipv, FindKeep);
    }
    else continue;
    ((comp == 1) ? ipv1 : ipv2) = ipv;
    ((comp == 1) ? pvk1 : pvk2) = pvk;
  }
}

//=======================================================================
//function : PntVtxOnSectEdge
//purpose  : Points/Vertexes  on  SectEdge are found
//=======================================================================

void BRepAlgo_DSAccess::PntVtxOnSectEdge
(const TopoDS_Shape& SectEdge,
 Standard_Integer& ipv1,
 TopOpeBRepDS_Kind& pvk1,
 Standard_Integer& ipv2,
 TopOpeBRepDS_Kind& pvk2)
{
  ipv1 = ipv2 = 0;
  pvk1 = pvk2 = TopOpeBRepDS_UNKNOWN;

//  myHB->ChangeBuilder();
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  TopOpeBRepDS_Kind kind = TopOpeBRepDS_POINT;
  TopExp_Explorer exp(SectEdge, TopAbs_VERTEX);
  Standard_Integer i = 1, ipv;
  
  for(; exp.More(); exp.Next(), i++) {    
    const TopoDS_Shape& DSVertex = exp.Current();
    ipv = myHB->GetDSPointFromNewVertex(DSVertex);
    if(!ipv) {
      ipv = DS.Shape(DSVertex, FindKeep);
      kind = TopOpeBRepDS_VERTEX;
      if(!ipv)
	// Standard_Error ...
	return;
    }
    
    if(i == 1) {
      ipv1 = ipv;
      pvk1 = kind;
    }    
    else if(i == 2) {
      ipv2 = ipv;
      pvk2 = kind;
    }
    else
      // Standard_Error ...
      return;
  }
}

//=======================================================================
//function : RemoveEdgeSameDomain
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::RemoveEdgeSameDomain
(const Standard_Integer /*iE1*/,
 const Standard_Integer /*iE2*/)
{
  return;
/*  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  const TopoDS_Shape& E1 = DS.Shape(iE1);
  const TopoDS_Shape& E2 = DS.Shape(iE2);
  TopAbs_ShapeEnum ts1, ts2;
  ts1 = E1.ShapeType();
  ts2 = E2.ShapeType();
  if((ts1 != TopAbs_EDGE) ||
     (ts2 != TopAbs_EDGE)) 
    return;
  TopTools_ListOfShape& lossd = DS.ChangeShapeSameDomain(E1);
  if(lossd.IsEmpty())
    return;
  Standard_Integer exte = lossd.Extent();
  if(exte == 1) {
    if(lossd.First().IsSame(E2))
      DS.UnfillShapesSameDomain(E1,E2);
    return;
  }*/
}

//=======================================================================
//function : RemoveFaceSameDomain
//purpose  : remove SameDomain information of glued faces 
//=======================================================================

void BRepAlgo_DSAccess::RemoveFaceSameDomain
(const TopoDS_Shape& C)
{
//  myHB->ChangeBuilder();
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();

//TColStd_ListIteratorOfListOfInteger it;
  TopExp_Explorer exp(C, TopAbs_EDGE);
  Standard_Integer  iE1, iE2, iE, /*NbF,*/ iF1, iF2, iCurrF1, iCurrF2,   iC =0; // skl
  iF1 = iF2 = iCurrF1 = iCurrF2 = 0;
  Standard_Boolean b;
  const TopoDS_Shape& SectEdge = exp.Current();
  
  for(; exp.More(); exp.Next()) {
    iC = myHB->GetDSCurveFromSectEdge(SectEdge);
    if(!iC && !SectEdge.IsNull())
      break;
//  const TopoDS_Shape& SectEdge = exp.Current();
  }
  if(!iC && !SectEdge.IsNull()) {
    iE1 = myHB->GetDSEdgeFromSectEdge(SectEdge, 1);
    iE2 = myHB->GetDSEdgeFromSectEdge(SectEdge, 2);
    if(iE1 && iE2) return;
    iE = (iE1 ? iE1 : iE2);
    if(!iE) return;
    
    TColStd_ListOfInteger& loi = FindGoodFace(iE, iF1, b);
    if(!b) return;
    if(exp.More())
      exp.Next();
    //NbF = loi.Extent(); // skl
    for(; exp.More(); exp.Next()) {
      // skl : I change "SectEdge" to "SectEdg"
      const TopoDS_Shape& SectEdg = exp.Current();
      iC = myHB->GetDSCurveFromSectEdge(SectEdg);
      if(!iC) {
	iE1 = myHB->GetDSEdgeFromSectEdge(SectEdg, 1);
	iE2 = myHB->GetDSEdgeFromSectEdge(SectEdg, 2);
	if(iE1 && iE2) return;
	iE = (iE1 ? iE1 : iE2);
	if(!iE) return;
	
	TColStd_ListOfInteger& loi2 = FindGoodFace(iE, iCurrF1, b);
	if(!b) return;
	if(!iCurrF1 || !iF1) return;
	if(iCurrF1 != iF1) {
	  if(loi2.Extent() == 1) iCurrF2 = loi2.First();
	  if(iCurrF2 == iF1) continue;
	  if(loi.Extent() == 1) iF2 = loi.First();

	  if(!iCurrF2 || !iF2) return;
	  if((iCurrF1 == iF2) ||
	     (iCurrF2 == iF2)) {
	    iF1 = iF2;
	    continue;
	  }
	  return;
	} 
      }
    }
    
    const TopoDS_Shape& FSD = DS.Shape(iF1);
    if(FSD.IsNull()) 
      return;
    TopTools_ListOfShape& ssd = DS.ChangeShapeSameDomain(FSD);
    TopTools_ListIteratorOfListOfShape itssd(ssd);
    TopExp_Explorer exp2;
    for(; itssd.More(); itssd.Next()) {
      exp2.Init(itssd.Value(), TopAbs_VERTEX);
      for(; exp2.More(); exp2.Next()) {
	const TopoDS_Shape& exp2Curr = exp2.Current();
	exp.Init(C, TopAbs_VERTEX);
	for(; exp.More(); exp.Next()) {
	  if(exp2Curr.IsSame(exp.Current()))
	    break;
	}
	if(exp.More())
	  break;
      }
      if(exp2.More())
	break;
    }
    
    if(exp2.More()) {
      const TopoDS_Shape& FSD2 = itssd.Value();
      Standard_Integer iFSD = DS.Shape(FSD), iFSD2 = DS.Shape(FSD2);
      RemoveFaceSameDomain(iFSD, iFSD2);      
//      DS.UnfillShapesSameDomain(FSD, FSD2);
    }
  }
}

//=======================================================================
//function : FindGoodFace
//purpose  : 
//=======================================================================

TColStd_ListOfInteger& BRepAlgo_DSAccess::FindGoodFace
(const Standard_Integer iE,
 Standard_Integer& iF1,
 Standard_Boolean& b)
{
//  myHB->ChangeBuilder();
  b = Standard_False;
  TColStd_ListOfInteger& loi = myHB->GetDSFaceFromDSEdge(iE, 1);
  if(loi.Extent() == 1) {
    iF1 = loi.First();
    b = Standard_True;
    TColStd_ListOfInteger& loi2 = myHB->GetDSFaceFromDSEdge(iE, 2);
    return loi2;
  }
  else {
    TColStd_ListOfInteger& loi2 = myHB->GetDSFaceFromDSEdge(iE, 2);
    if(loi2.Extent() == 1) {
      b = Standard_True;
      iF1 = loi2.First();
      return loi;
    }
  }
  b = Standard_False;
  return myEmptyListOfInteger;
}

//=======================================================================
//function : RemoveFaceSameDomain
//purpose  : 
//=======================================================================

void BRepAlgo_DSAccess::RemoveFaceSameDomain
(const Standard_Integer iF1,
 const Standard_Integer iF2)
{
  TopOpeBRepDS_DataStructure& DS = myHDS->ChangeDS();
  const TopoDS_Shape& F1 = DS.Shape(iF1, FindKeep);
  const TopoDS_Shape& F2 = DS.Shape(iF2, FindKeep);
  if(F1.IsNull() || F2.IsNull())
    return;


  Standard_Integer iref1 = DS.SameDomainRef(F1),
  iref2 = DS.SameDomainRef(F2), istart, iend;
  if(iref1 == iF1)
    DS.SameDomainRef(F2,iF2);
  if(iref2 == iF1)
    DS.SameDomainRef(F1,iF1);
  DS.UnfillShapesSameDomain(F1,F2);

  if(iref1 != iref2)
    return;
  Standard_Boolean iF1iF2IsConnected = Standard_False;
  TColStd_IndexedMapOfInteger moi;
  moi.Clear();
  if(iref2 == iF2) {
    istart = iF2;
    iend = iF1;
  }
  else {
    istart = iF1;
    iend = iF2;
  }
  moi.Add(istart);
  Standard_Integer NbConnect = 0, icurr;
  while(moi.Extent() > NbConnect) {
    NbConnect++;
    icurr = moi.FindKey(NbConnect);
    DS.SameDomainRef(icurr, istart);
    const TopTools_ListOfShape& los = DS.ShapeSameDomain(icurr);
    if(los.IsEmpty()) {
      const TopoDS_Shape& SNSD = DS.Shape(icurr);
      DS.SameDomainRef(SNSD, 0);
    }
    TopTools_ListIteratorOfListOfShape li(los);
    for(; li.More(); li.Next()) {
      Standard_Integer iCurrShap = DS.Shape(li.Value(), FindKeep);
      if(!iCurrShap)
	return;
      if(iCurrShap == iend)
	iF1iF2IsConnected = Standard_True;
      moi.Add(iCurrShap);
    }
  }
  if(!iF1iF2IsConnected) {
    moi.Clear();
    moi.Add(iend);
    NbConnect = 0;
    while(moi.Extent() > NbConnect) {
      NbConnect++;
      icurr = moi.FindKey(NbConnect);
      DS.SameDomainRef(icurr, iend);
      const TopTools_ListOfShape& los = DS.ShapeSameDomain(icurr);
      if(los.IsEmpty()) {
	const TopoDS_Shape& SNSD = DS.Shape(icurr);
	DS.SameDomainRef(SNSD, 0);
      }
      TopTools_ListIteratorOfListOfShape li(los);
      for(; li.More(); li.Next()) {
	Standard_Integer iCurrShap = DS.Shape(li.Value(), FindKeep);
	if(!iCurrShap)
	  return;
	moi.Add(iCurrShap);
      }
    }
  }
}

//=======================================================================
//function : DS
//purpose  : 
//=======================================================================

const Handle(TopOpeBRepDS_HDataStructure)&
BRepAlgo_DSAccess::DS() const
{
  return myHDS;
}

//=======================================================================
//function : changeDS
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_HDataStructure)&
BRepAlgo_DSAccess::ChangeDS()
{
  return myHDS;
}

//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================

const Handle(TopOpeBRepBuild_HBuilder)& 
BRepAlgo_DSAccess::Builder() const
{
  return myHB;
}

//=======================================================================
//function : ChangeBuilder
//purpose  : 
//=======================================================================

Handle(TopOpeBRepBuild_HBuilder)& 
BRepAlgo_DSAccess::ChangeBuilder()
{
  return myHB;
}
