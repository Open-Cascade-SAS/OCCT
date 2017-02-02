// Created on: 1996-01-05
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_Loop.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_WireEdgeClassifier.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//#include <BRepAdaptor_Curve2d.hxx>
#undef RM_HANGING
// MSV: RM_HANGING behaviour: when state of wire is UNCLOSEDW we do not
// remove the whole wire but remove the chains of hanging edges. This would
// produce a good result in some cases. But :-( it gives regressions on grid
// tests (1cto 021 W4,X4). Therefore I leaved this code not active.
#ifdef RM_HANGING
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#endif

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettracePURGE();
void debifb() {}
#endif

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

//=======================================================================
//function : TopOpeBRepBuild_FaceBuilder
//purpose  : 
//=======================================================================
TopOpeBRepBuild_FaceBuilder::TopOpeBRepBuild_FaceBuilder()
{
}

//=======================================================================
//function : TopOpeBRepBuild_FaceBuilder
//purpose  : 
//=======================================================================
TopOpeBRepBuild_FaceBuilder::TopOpeBRepBuild_FaceBuilder(TopOpeBRepBuild_WireEdgeSet& WES,const TopoDS_Shape& F,const Standard_Boolean ForceClass)
{
  InitFaceBuilder(WES,F,ForceClass);
}

//=======================================================================
//function : InitFaceBuilder
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::InitFaceBuilder(TopOpeBRepBuild_WireEdgeSet& WES,const TopoDS_Shape& F,const Standard_Boolean ForceClass) 
{
  myFace = TopoDS::Face(F);
  MakeLoops(WES);
  TopOpeBRepBuild_BlockBuilder& BB = myBlockBuilder;
  TopOpeBRepBuild_WireEdgeClassifier WEC(F,BB);
  TopOpeBRepBuild_LoopSet& LS = myLoopSet;
  myFaceAreaBuilder.InitFaceAreaBuilder(LS,WEC,ForceClass);
}

//---------------------------------------------------------------
void FUN_DetectVerticesOn1Edge(const TopoDS_Shape& W,TopTools_IndexedDataMapOfShapeShape& mapVon1E)
{
  // Fills the map <mapVon1edge>,with vertices of <W> of 3d connexity 1.
  TopTools_IndexedDataMapOfShapeListOfShape mapVedges;
  TopExp::MapShapesAndAncestors(W,TopAbs_VERTEX,TopAbs_EDGE,mapVedges);
  Standard_Integer nV = mapVedges.Extent();

  for (Standard_Integer i = 1;i <= nV;i++) {
    const TopoDS_Shape& V = mapVedges.FindKey(i);
    if (V.Orientation() == TopAbs_INTERNAL) continue;

    const TopTools_ListOfShape& loE = mapVedges.FindFromIndex(i);
    if (loE.Extent() < 2) {
      // Keeping INTERNAL or EXTERNAL edges
      const TopoDS_Shape& E = loE.First();
      TopAbs_Orientation oriE = E.Orientation();
      if ((oriE == TopAbs_INTERNAL) || (oriE == TopAbs_EXTERNAL)) continue;
      mapVon1E.Add(V,E);
    }
  }
} 

#define ISUNKNOWN -1
#define ISVERTEX  0
#define GCLOSEDW  1
#define UNCLOSEDW 2
#define CLOSEDW   10
Standard_Integer FUN_AnalyzemapVon1E(const TopTools_IndexedDataMapOfShapeShape& mapVon1E,
			TopTools_IndexedDataMapOfShapeShape& mapVV)
{
#ifdef DRAW
  Standard_Boolean trc = TopOpeBRepBuild_GettracePURGE();
  if (trc) cout<<endl<<"* DetectUnclosedWire :"<<endl;
#endif

  Standard_Integer res = ISUNKNOWN;

  Standard_Integer nV = mapVon1E.Extent();
  if      (nV == 0) {
    res = CLOSEDW;
  }
  else if (nV == 1) {
    const TopoDS_Shape& E = mapVon1E.FindFromIndex(1);
    Standard_Boolean Eclosed = BRep_Tool::IsClosed(E);
    Standard_Boolean dgE = BRep_Tool::Degenerated(TopoDS::Edge(E));
    if      (dgE)     res = ISVERTEX;
    else if (Eclosed) res = CLOSEDW;
    else              res = UNCLOSEDW;
  } 
  else {
    // Finding among all vertices,couple of vertices falling on same
    // geometry.
    // Filling up map <mapVV>,with (vi,vj),vi and vj are on same point.
    Standard_Real tol = Precision::Confusion();
    for (Standard_Integer i = 1;i <= nV;i++) {
      const TopoDS_Vertex& vi = TopoDS::Vertex(mapVon1E.FindKey(i));
      gp_Pnt pi = BRep_Tool::Pnt(vi);
      for (Standard_Integer j = i+1;j <= nV;j++) {
	const TopoDS_Vertex& vj = TopoDS::Vertex(mapVon1E.FindKey(j));
	gp_Pnt pj = BRep_Tool::Pnt(vj);
	Standard_Boolean same = pi.IsEqual(pj,tol);
	if (same) {
	  mapVV.Add(vi,vj);
	  mapVV.Add(vj,vi);
	  break;
	}
      } // j
    } // i
    Standard_Integer nVV = mapVV.Extent();
#ifdef RM_HANGING
    // MSV Oct 4, 2001: consider GCLOSEDW even if not all vertices from mapVon1E
    //                  hit into mapVV; reason is the left vertices may start
    //                  useless chains of hanging edges that can be removed to
    //                  achieve a closed wire.
    if (nVV > 0)   res = GCLOSEDW;
#else
    if (nVV == nV) res = GCLOSEDW;
#endif
    else           res = UNCLOSEDW;
  }
  return res;
} // FUN_AnalyzemapVon1E

#ifdef DRAW
void FUN_AnalyzemapVon1EDRAW(const Standard_Integer res,
			     const TopTools_IndexedDataMapOfShapeShape& mapVon1E,
			     const TopTools_IndexedDataMapOfShapeShape& mapVV,
			     const TopoDS_Shape& W,const Standard_Integer iiwi,
			     TopTools_IndexedDataMapOfShapeShape& mapVon1EdgeDRAW,
			     TopTools_IndexedDataMapOfShapeShape& mapVVsameGDRAW)
{
  Standard_Boolean trc = TopOpeBRepBuild_GettracePURGE();
  if (!trc) return;
  cout<<"wire "<<iiwi;
  if      (res == ISVERTEX) {
    cout<<" is vertex"<<endl;
  }
  else if (res == CLOSEDW) {
    cout<<" is closed"<<endl;
  }
  else if (res == GCLOSEDW) {
    cout<<" is Gclosed :"<<endl;
    TCollection_AsciiString aa("w_");FUN_tool_draw(aa,W,iiwi);
    Standard_Integer i ;
    for ( i = 1;i <= mapVV.Extent();i++) {
      Standard_Integer iV = mapVVsameGDRAW.Add(mapVV.FindKey(i),mapVV.FindFromIndex(i));
      cout<<" on vve_"<<iV; aa = "vve_";
      FUN_tool_draw(aa,mapVVsameGDRAW.FindKey(iV),iV);
    }
    for (i = 1;i <= mapVon1E.Extent();i++) {
      Standard_Integer iE = mapVon1EdgeDRAW.Add(mapVon1E.FindKey(i),mapVon1E.FindFromIndex(i));
      cout<<" on eed_"<<iE; aa = "eed_";
      FUN_tool_draw(aa,mapVon1EdgeDRAW.FindFromIndex(iE),iE);
    }
    cout<<endl;
  }
  else if (res == UNCLOSEDW) {
    cout<<" is unclosed "<<endl;
    TCollection_AsciiString aa("w_");FUN_tool_draw(aa,W,iiwi);
  }
  cout<<endl;
} // FUN_AnalyzemapVon1EDRAW
#endif

//=======================================================================
//function : DetectUnclosedWire
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::DetectUnclosedWire(TopTools_IndexedDataMapOfShapeShape& mapVVsameG,
			      TopTools_IndexedDataMapOfShapeShape& mapVon1Edge)
{
  // wire unclosed : has vertices of connexity == 1
  // exception : a wire of one closed edge with only one vertex describing
  //             a face or a degenerated edge.

  mapVVsameG.Clear();
  mapVon1Edge.Clear();

  // During the wire processing,
  // * IF THE WIRE IS G-CLOSED,we fill up the maps :
  //  - <mapVVsameG> with vertices falling on same geometry
  //  - <mapVon1Edge> with (key = vertex,item = edge),
  //    the vertex is connected to only one unclosed,undegenerated edge.
  // * Else,if it is unclosed,we delete it (or it`s hanging edges).

#ifdef DRAW
  TopTools_IndexedDataMapOfShapeShape mapVon1EdgeDRAW,mapVVsameGDRAW;
  Standard_Boolean trc = TopOpeBRepBuild_GettracePURGE();
  if (trc) cout<<endl<<"* DetectUnclosedWire :"<<endl<<endl;
#endif

  Standard_Integer iiwi = 0; // DEB

  InitFace();
  for (;MoreFace();NextFace()) {
   InitWire();
    for (;MoreWire();NextWire()) { 
      iiwi++;
      Standard_Boolean isold = IsOldWire();
#ifdef DRAW
      if ( trc && isold ) cout<<"wire "<<iiwi<<" is old wire => closed"<<endl;
#endif
      if (isold) continue;
      
      TopoDS_Compound cmp;BRep_Builder BB;BB.MakeCompound(cmp);
      InitEdge();
      for(;MoreEdge();NextEdge()) AddEdgeWire(Edge(),cmp);
      TopoDS_Shape W = cmp;

      // <mapVon1E> binds vertices of connexity 1 attached to one non-closed,non-degenerated edge.
      TopTools_IndexedDataMapOfShapeShape mapVon1E;
      FUN_DetectVerticesOn1Edge(W,mapVon1E);

      TopTools_IndexedDataMapOfShapeShape mapVV;
      Standard_Integer res = FUN_AnalyzemapVon1E(mapVon1E,mapVV);
#ifdef DRAW
      FUN_AnalyzemapVon1EDRAW(res,mapVon1E,mapVV,W,iiwi,mapVon1EdgeDRAW,mapVVsameGDRAW);
#endif

      if      (res == ISVERTEX) {
	continue;
      }
      else if (res == CLOSEDW) {
	continue;
      }
      else if (res == GCLOSEDW) {
	Standard_Integer i;
	for (i = 1;i <= mapVV.Extent();i++) {
          mapVVsameG.Add(mapVV.FindKey(i),mapVV.FindFromIndex(i));
	}
	for (i = 1;i <= mapVon1E.Extent();i++) {
           mapVon1Edge.Add(mapVon1E.FindKey(i),mapVon1E.FindFromIndex(i));
	}
      }
      else if (res == UNCLOSEDW) {
#ifdef RM_HANGING
        // MSV Oct 4, 2001: remove hanging edges
        TopTools_IndexedDataMapOfShapeListOfShape mapVE;
        TopExp::MapShapesAndAncestors (W, TopAbs_VERTEX, TopAbs_EDGE, mapVE);
        Standard_Integer nV = mapVon1E.Extent();
        for (Standard_Integer i = 1; i <= nV; i++)
        {
          TopoDS_Vertex V = TopoDS::Vertex (mapVon1E.FindKey(i));
          if (mapVV.Contains(V)) continue;  // V is in same geometry pair
          while (1)
          {
            const TopTools_ListOfShape &LE = mapVE.FindFromKey(V);

            // get not yet processed edge, count the number of such edges
            Standard_Integer nEdges = 0;
            TopoDS_Edge Edge;
            TColStd_ListOfInteger LOI;
            TopTools_ListIteratorOfListOfShape itE (LE);
            for (; itE.More() && nEdges <= 1; itE.Next())
            {
              const TopoDS_Edge &E = TopoDS::Edge (itE.Value());
              Standard_Integer I = myBlockBuilder.Element(E);
              if (!BRep_Tool::IsClosed(E) && myBlockBuilder.ElementIsValid(I))
              {
                TopoDS_Vertex Vf,Vl;
                TopExp::Vertices (E, Vf, Vl);
                LOI.Append(I);
                // consider not small edges only
                if (!Vf.IsSame(Vl))
                {
                  Edge = E;
                  nEdges++;
                }
              }
            }
            if (nEdges != 1) break;  // stop this chain

            // remove edges from Block Builder
            TColStd_ListIteratorOfListOfInteger itLOI (LOI);
            for (; itLOI.More(); itLOI.Next())
              myBlockBuilder.SetValid (itLOI.Value(), Standard_False);

            // get other vertex
            TopoDS_Vertex aV1, aV2, otherV;
            TopExp::Vertices (Edge, aV1, aV2);
            if (aV1.IsSame (V))
              otherV = aV2;
            else if (aV2.IsSame (V))
              otherV = aV1;
            if (otherV.IsNull()) break;
            V = otherV;
          }
        }
#else
	TopExp_Explorer ex;
	for (ex.Init(W,TopAbs_EDGE);ex.More();ex.Next()) {
//	for (TopExp_Explorer ex(W,TopAbs_EDGE);ex.More();ex.Next()) {
	  Standard_Integer I = myBlockBuilder.Element(ex.Current());
	  myBlockBuilder.SetValid(I,Standard_False);
	}
#endif
      }
    } // MoreWire
  } // MoreFace
} // DetectUnclosedWire

//=======================================================================
//function : CorrectGclosedWire
//purpose  :
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::CorrectGclosedWire(const TopTools_IndexedDataMapOfShapeShape& mapVVref,
			       const TopTools_IndexedDataMapOfShapeShape& mapVon1Edge)
{
  // prequesitory : edges described by <mapVon1Edge> are not closed,not degenerated
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettracePURGE()) {
    cout<<endl<<"* CorrectGclosedWire :"<<endl<<endl;
  }
#endif
  
  Standard_Integer nVV = mapVVref.Extent();
  for (Standard_Integer i = 1;i <= nVV;i++) {
    const TopoDS_Vertex& V = TopoDS::Vertex(mapVVref.FindKey(i));
    const TopoDS_Vertex& Vref = TopoDS::Vertex(mapVVref.FindFromIndex(i));
    
    if (V.IsSame(Vref)) continue;

    TopoDS_Edge E = TopoDS::Edge(mapVon1Edge.FindFromKey(V));
    Standard_Real paronE = BRep_Tool::Parameter(V,E);
    
    BRep_Builder BB;E.Free(Standard_True);
    BB.Remove(E,V);
    TopoDS_Shape aLocalShape = Vref.Oriented(V.Orientation());
    TopoDS_Vertex newVref = TopoDS::Vertex(aLocalShape);
//    TopoDS_Vertex newVref = TopoDS::Vertex(Vref.Oriented(V.Orientation()));
    BB.Add(E,newVref);
    TopOpeBRepDS_BuildTool BT;
    BT.Parameter(E,newVref,paronE);
  }
}

//=======================================================================
//function : DetectPseudoInternalEdge
//purpose  :
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::DetectPseudoInternalEdge(TopTools_IndexedMapOfShape& MapE)
{
  TopoDS_Compound cmp;BRep_Builder BB;BB.MakeCompound(cmp);
  InitFace();
  for (;MoreFace();NextFace()) {
    InitWire();
    for (;MoreWire();NextWire()) {
      Standard_Boolean isold = IsOldWire(); if (isold) continue;
      InitEdge();
      for(;MoreEdge();NextEdge()) AddEdgeWire(Edge(),cmp);
    } // MoreWire
  } // MoreFace

  TopTools_IndexedDataMapOfShapeListOfShape mapVOE;
  TopExp::MapShapesAndAncestors(cmp,TopAbs_VERTEX,TopAbs_EDGE,mapVOE);
  Standard_Integer nv = mapVOE.Extent();

  MapE.Clear();
  for (Standard_Integer i = 1; i <= nv; i++) {
    const TopTools_ListOfShape& le = mapVOE.FindFromIndex(i);
    Standard_Integer ne = le.Extent();
    if (ne == 2) {
      TopTools_ListIteratorOfListOfShape ile(le); const TopoDS_Shape& e1 = ile.Value();
      ile.Next();        const TopoDS_Shape& e2 = ile.Value();
      Standard_Boolean same = e1.IsSame(e2);
      TopAbs_Orientation o1 = e1.Orientation();
      TopAbs_Orientation o2 = e2.Orientation();
      Standard_Boolean o1co2 = (o1 == TopAbs::Complement(o2));

      if ( same && o1co2 ) {
	MapE.Add(e1);

	Standard_Integer ie1 = myBlockBuilder.Element(e1);
	myBlockBuilder.SetValid(ie1,Standard_False);

	Standard_Integer ie2 = myBlockBuilder.Element(e2);
	myBlockBuilder.SetValid(ie2,Standard_False);
      }
    }
  }

}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepBuild_FaceBuilder::Face() const 
{
  return myFace;
}

//=======================================================================
//function : InitFace
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_FaceBuilder::InitFace()
{
  Standard_Integer n = myFaceAreaBuilder.InitArea();
  return n;
}

//=======================================================================
//function : MoreFace
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_FaceBuilder::MoreFace() const
{
  Standard_Boolean b = myFaceAreaBuilder.MoreArea();
  return b;
}

//=======================================================================
//function : NextFace
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::NextFace()
{
  myFaceAreaBuilder.NextArea();
}

//=======================================================================
//function : InitWire
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_FaceBuilder::InitWire()
{
  Standard_Integer n = myFaceAreaBuilder.InitLoop();
  return n;
}

//=======================================================================
//function : MoreWire
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_FaceBuilder::MoreWire() const
{
  Standard_Boolean b = myFaceAreaBuilder.MoreLoop();
  return b;
}

//=======================================================================
//function : NextWire
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::NextWire()
{
  myFaceAreaBuilder.NextLoop();
}

//=======================================================================
//function : IsOldWire
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_FaceBuilder::IsOldWire() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = myFaceAreaBuilder.Loop();
  Standard_Boolean b = L->IsShape();
  return b;
}

//=======================================================================
//function : OldWire
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepBuild_FaceBuilder::OldWire() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = myFaceAreaBuilder.Loop();
  const TopoDS_Shape& B = L->Shape();
  return B;
}

//=======================================================================
//function : FindNextValidElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::FindNextValidElement()
{
  // prerequisites : myBlockIterator.Initialize
  myFaceAreaBuilder.Loop();
  Standard_Boolean found = Standard_False;

  while ( myBlockIterator.More()) {
    const Standard_Integer i = myBlockIterator.Value();
    found = myBlockBuilder.ElementIsValid(i);
    if (found) break;
    else myBlockIterator.Next();
  }
}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_FaceBuilder::InitEdge()
{
  const Handle(TopOpeBRepBuild_Loop)& L = myFaceAreaBuilder.Loop();
  if ( L->IsShape() )
    throw Standard_DomainError("TopOpeBRepBuild_FaceBuilder:InitEdge");
  else {
    myBlockIterator = L->BlockIterator();
    myBlockIterator.Initialize();
    FindNextValidElement();
  }
  Standard_Integer n = myBlockIterator.Extent();
  return n;
}

//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_FaceBuilder::MoreEdge() const
{
  Standard_Boolean b = myBlockIterator.More();
  return b;
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::NextEdge()
{
  myBlockIterator.Next();
  FindNextValidElement();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepBuild_FaceBuilder::Edge() const
{
  if (!myBlockIterator.More()) throw Standard_Failure("OutOfRange");

  const Standard_Integer i = myBlockIterator.Value();
  Standard_Boolean isvalid = myBlockBuilder.ElementIsValid(i);
  if (!isvalid) throw Standard_Failure("Edge not Valid");

  const TopoDS_Shape& E = myBlockBuilder.Element(i);
  return E;
}

//=======================================================================
//function : EdgeConnexity
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_FaceBuilder::EdgeConnexity(const TopoDS_Shape& /*E*/) const
{
#ifdef OCCT_DEBUG
  throw Standard_ProgramError("FaceBuilder::EdgeConnexity management disactivated");
#else
  return 0;
#endif
//  Standard_Boolean inmosi = myMOSI.IsBound(E);
//  Standard_Integer nmosi = (inmosi) ? myMOSI.Find(E) : 0;
//  return nmosi;
}

//=======================================================================
//function : AddEdgeWire
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_FaceBuilder::AddEdgeWire(const TopoDS_Shape& E,TopoDS_Shape& W) const
{
  Standard_Integer nadd = 0;
  BRep_Builder BB;
  BB.Add(W,E);nadd++;
//  Standard_Integer nmosi = EdgeConnexity(E);
//  Standard_Boolean addEC = (nmosi == 1);
//  if (addEC) {
//    TopAbs_Orientation oe = E.Orientation();
//    TopAbs_Orientation oc = TopAbs::Complement(oe);
//    TopoDS_Shape EC = E.Oriented(oc);
//    BB.Add(W,EC);nadd++;
//  }
  return nadd;
}

//=======================================================================
//function : MakeLoops
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_FaceBuilder::MakeLoops(TopOpeBRepBuild_ShapeSet& SS)
{
  TopOpeBRepBuild_BlockBuilder& BB = myBlockBuilder;
  TopOpeBRepBuild_ListOfLoop& LL = myLoopSet.ChangeListOfLoop();

  // Build blocks on elements of SS
  BB.MakeBlock(SS);

  // make list of loop (LL) of the LoopSet
  // - on shapes of the ShapeSet (SS)
  // - on blocks of the BlockBuilder (BB)

  // Add shapes of SS as shape loops
  LL.Clear();
  for(SS.InitShapes();SS.MoreShapes();SS.NextShape()) {
    const TopoDS_Shape& S = SS.Shape();
    Handle(TopOpeBRepBuild_Loop) ShapeLoop = new TopOpeBRepBuild_Loop(S);
    LL.Append(ShapeLoop);
  }
  
  // Add blocks of BB as block loops
  for (BB.InitBlock();BB.MoreBlock();BB.NextBlock()) {
    TopOpeBRepBuild_BlockIterator BI = BB.BlockIterator();
    Handle(TopOpeBRepBuild_Loop) BlockLoop = new TopOpeBRepBuild_Loop(BI);
    LL.Append(BlockLoop);
  }

}
