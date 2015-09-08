// Created on: 1996-09-03
// Created by: Yves FRICAUD
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

//  Modified by skv - Fri Dec 26 12:20:14 2003 OCC4455

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
#include <BRepOffset_Inter3d.hxx>
#include <BRepOffset_Interval.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <Extrema_ExtPC.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepTool_BoxSort.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : BRepOffset_Inter3d
//purpose  : 
//=======================================================================
BRepOffset_Inter3d::BRepOffset_Inter3d(const Handle(BRepAlgo_AsDes)& AsDes, 
				       const TopAbs_State              Side ,
				       const Standard_Real             Tol)
:myAsDes(AsDes),
mySide(Side),
myTol(Tol)
{
}


//=======================================================================
//function : ExtentEdge
//purpose  : 
//=======================================================================

static void ExtentEdge(const TopoDS_Face& /*F*/,
		       const TopoDS_Edge& E,
		       TopoDS_Edge& NE) 
{
  TopoDS_Shape aLocalShape = E.EmptyCopied();
  NE = TopoDS::Edge(aLocalShape); 
//  NE = TopoDS::Edge(E.EmptyCopied()); 
  

  // Enough for analytic edges, in general case reconstruct the 
  // geometry of the edge recalculating the intersection of surfaces.  

  NE.Orientation(TopAbs_FORWARD);
  Standard_Real f,l;
  BRep_Tool::Range(E,f,l);
  Standard_Real length = l-f;
  f -=  100*length;
  l +=  100*length;

  BRep_Builder B;
  B.Range(NE,f,l);
  BRepAdaptor_Curve CE(E);
  TopoDS_Vertex V1 = BRepLib_MakeVertex(CE.Value(f));
  TopoDS_Vertex V2 = BRepLib_MakeVertex(CE.Value(l));
  B.Add(NE,V1.Oriented(TopAbs_FORWARD));
  B.Add(NE,V2.Oriented(TopAbs_REVERSED));
  NE.Orientation(E.Orientation());

}

//=======================================================================
//function : SelectEdge
//purpose  : 
//=======================================================================

static void SelectEdge (const TopoDS_Face& /*F*/,
			const TopoDS_Face& /*EF*/,
			const TopoDS_Edge& E,
			TopTools_ListOfShape& LInt)
{
  //------------------------------------------------------------
  // Proofing on the intersections on periodical faces
  //------------------------------------------------------------
   TopTools_ListIteratorOfListOfShape it(LInt);
//  Modified by Sergey KHROMOV - Wed Jun  5 11:43:04 2002 Begin
//   Standard_Real dU = 1.0e100;
  Standard_Real dU = RealLast();
//  Modified by Sergey KHROMOV - Wed Jun  5 11:43:05 2002 End
  TopoDS_Edge   GE;

  Standard_Real Fst, Lst, tmp;
  BRep_Tool::Range(E, Fst, Lst);
  BRepAdaptor_Curve  Ad1(E);
 
  gp_Pnt PFirst = Ad1.Value( Fst );  
  gp_Pnt PLast  = Ad1.Value( Lst );  

//  Modified by Sergey KHROMOV - Wed Jun  5 11:23:10 2002 Begin
   Extrema_ExtPC anExt;
//  Modified by Sergey KHROMOV - Wed Jun  5 11:23:11 2002 End
  //----------------------------------------------------------------------
  // Selection of edge that coversmost of the domain of the initial edge.
  //---------------------------------------------------------------------- 
  for (; it.More(); it.Next()) {
    const TopoDS_Edge& EI = TopoDS::Edge(it.Value());

    BRep_Tool::Range(EI, Fst, Lst);
    BRepAdaptor_Curve  Ad2(EI);

//  Modified by Sergey KHROMOV - Wed Jun  5 11:25:03 2002 Begin
    Standard_Integer i;
    Standard_Real    aTol       = BRep_Tool::Tolerance(EI);
    Standard_Boolean isMinFound = Standard_False;
    Standard_Real    aSqrDist1  = Precision::Infinite();
    Standard_Real    aSqrDist2  = Precision::Infinite();

    anExt.Initialize(Ad2, Fst, Lst, aTol);

// Seek for the min distance for PFirst:
    anExt.Perform(PFirst);
    if (anExt.IsDone()) {
      for (i = 1; i <= anExt.NbExt(); i++) {
	if (anExt.IsMin(i)) {
	  const gp_Pnt &aPMin = anExt.Point(i).Value();

	  aSqrDist1  = PFirst.SquareDistance(aPMin);
	  isMinFound = Standard_True;

	  break;
	}
      }
    }
    if (!isMinFound) {
      gp_Pnt aP1 = Ad2.Value(Fst);
      gp_Pnt aP2 = Ad2.Value(Lst);

      aSqrDist1 = Min(aP1.SquareDistance(PFirst), aP2.SquareDistance(PFirst));
    }

// Seek for the min distance for PLast:
    isMinFound = Standard_False;
    anExt.Perform(PLast);
    if (anExt.IsDone()) {
      for (i = 1; i <= anExt.NbExt(); i++) {
	if (anExt.IsMin(i)) {
	  const gp_Pnt &aPMin = anExt.Point(i).Value();

	  aSqrDist2  = PLast.SquareDistance(aPMin);
	  isMinFound = Standard_True;

	  break;
	}
      }
    }
    if (!isMinFound) {
      gp_Pnt aP1 = Ad2.Value(Fst);
      gp_Pnt aP2 = Ad2.Value(Lst);

      aSqrDist2 = Min(aP1.SquareDistance(PLast), aP2.SquareDistance(PLast));
    }

    tmp = aSqrDist1 + aSqrDist2;
//     gp_Pnt P1 = Ad2.Value(Fst);
//     gp_Pnt P2 = Ad2.Value(Lst);
       
//     tmp = P1.Distance(PFirst) + P2.Distance(PLast);
    if( tmp <= dU ) {
      dU = tmp;
      GE = EI;
    } 
//  Modified by Sergey KHROMOV - Wed Jun  5 11:24:54 2002 End

  }
  LInt.Clear(); 
  LInt.Append(GE);
}


//=======================================================================
//function : CompletInt
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::CompletInt(const TopTools_ListOfShape& SetOfFaces,
				    const BRepAlgo_Image&     InitOffsetFace)
{
  //---------------------------------------------------------------
  // Calculate the intersections of offset faces 
  // Distinction of intersection between faces // tangents.
  //---------------------------------------------------------------
  TopoDS_Face                        F2;
  TopTools_ListIteratorOfListOfShape it;

  //---------------------------------------------------------------
  // Construction of bounding boxes
  //---------------------------------------------------------------
  TopOpeBRepTool_BoxSort BOS;
  BRep_Builder B;
  TopoDS_Compound CompOS;
  B.MakeCompound(CompOS);
  
  for (it.Initialize(SetOfFaces); it.More(); it.Next()) {
    const TopoDS_Shape& OS = it.Value();
    B.Add(CompOS,OS);
  }
  BOS.AddBoxesMakeCOB(CompOS,TopAbs_FACE);

  //---------------------------
  // Intersection of faces // 
  //---------------------------
  for (it.Initialize(SetOfFaces); it.More(); it.Next()) {
    const TopoDS_Face& F1  = TopoDS::Face(it.Value());
    TColStd_ListIteratorOfListOfInteger itLI = BOS.Compare(F1);
    for (; itLI.More(); itLI.Next()) {
      F2     = TopoDS::Face(BOS.TouchedShape(itLI));
      FaceInter(F1,F2,InitOffsetFace);
    }
  }
}


//=======================================================================
//function : CompletInt
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::FaceInter(const TopoDS_Face& F1,
				   const TopoDS_Face& F2,
				   const BRepAlgo_Image&     InitOffsetFace)
{
  TopTools_ListOfShape LInt1, LInt2;
  TopoDS_Edge NullEdge;

  if (F1.IsSame(F2)) return;
  if (IsDone(F1,F2)) return;
  const TopoDS_Shape& InitF1 = InitOffsetFace.ImageFrom(F1);
  const TopoDS_Shape& InitF2 = InitOffsetFace.ImageFrom(F2);
  Standard_Boolean InterPipes = (InitF2.ShapeType() == TopAbs_EDGE &&
				 InitF1.ShapeType() == TopAbs_EDGE );
  Standard_Boolean InterFaces = (InitF1.ShapeType() == TopAbs_FACE && 
				 InitF2.ShapeType() == TopAbs_FACE);
  TopTools_ListOfShape LE,LV;
  LInt1.Clear(); LInt2.Clear(); 
  if (BRepOffset_Tool::HasCommonShapes(F1,F2,LE,LV) ||
      myAsDes->HasCommonDescendant(F1,F2,LE)) {
    //-------------------------------------------------
    // F1 and F2 share shapes.
    //-------------------------------------------------
    if ( LE.IsEmpty() && !LV.IsEmpty()) {
      if (InterPipes) {
	//----------------------
	// tubes share a vertex.
	//----------------------
	const TopoDS_Edge& EE1 = TopoDS::Edge(InitF1);
	const TopoDS_Edge& EE2 = TopoDS::Edge(InitF2);
	TopoDS_Vertex VE1[2],VE2[2];
	TopExp::Vertices(EE1,VE1[0],VE1[1]);
	TopExp::Vertices(EE2,VE2[0],VE2[1]);
	TopoDS_Vertex V;
	for (Standard_Integer i = 0 ; i < 2; i++) {
	  for (Standard_Integer j = 0 ; j < 2; j++) {
	    if (VE1[i].IsSame(VE2[j])) {
	      V = VE1[i];
	    }
	  }
	}
	if (!InitOffsetFace.HasImage(V)) { //no sphere
	  BRepOffset_Tool::PipeInter(F1,F2,LInt1,LInt2,mySide);
	}		
      }
      else {
	//--------------------------------------------------------
	// Intersection having only common vertices
	// and supports having common edges.
	// UNSUFFICIENT, but a larger criterion shakes too
	// many sections.
	//--------------------------------------------------------
	if (InterFaces && 
	    BRepOffset_Tool::HasCommonShapes(TopoDS::Face(InitF1),
					     TopoDS::Face(InitF2),LE,LV)) 
	  if (!LE.IsEmpty())
	    BRepOffset_Tool::Inter3D (F1,F2,LInt1,LInt2,mySide,NullEdge);
      }
    }
  }
  else {
    if (InterPipes) {
      BRepOffset_Tool::PipeInter(F1,F2,LInt1,LInt2,mySide);
    }
    else {
      BRepOffset_Tool::Inter3D (F1,F2,LInt1,LInt2,mySide,NullEdge);
    }
  }
  Store (F1,F2,LInt1,LInt2);
}


//=======================================================================
//function : ConnexIntByArc
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::ConnexIntByArc(const TopTools_ListOfShape& /*SetOfFaces*/, 
                                        const TopoDS_Shape&         ShapeInit, 
                                        const BRepOffset_Analyse&   Analyse, 
                                        const BRepAlgo_Image&       InitOffsetFace)
{
  BRepOffset_Type    OT   = BRepOffset_Concave;
  if (mySide == TopAbs_OUT) OT   = BRepOffset_Convex;
  TopExp_Explorer                Exp(ShapeInit,TopAbs_EDGE);
  TopTools_ListOfShape           LInt1,LInt2;
  TopoDS_Face                    F1,F2;
  TopoDS_Edge                    NullEdge;

  //---------------------------------------------------------------------
  // etape 1 : Intersection of faces // corresponding to the initial faces 
  //           separated by a concave edge if offset > 0, otherwise convex.
  //---------------------------------------------------------------------  
  for (; Exp.More(); Exp.Next()) {
    const TopoDS_Edge&               E = TopoDS::Edge(Exp.Current());
    const BRepOffset_ListOfInterval& L = Analyse.Type(E);
    if (!L.IsEmpty() && L.First().Type() == OT) {
      //-----------------------------------------------------------
      // edge is of the proper type , return adjacent faces.
      //-----------------------------------------------------------
      const TopTools_ListOfShape& Anc = Analyse.Ancestors(E);
      if (Anc.Extent() == 2) {
	F1 = TopoDS::Face(InitOffsetFace.Image(Anc.First()).First());
	F2 = TopoDS::Face(InitOffsetFace.Image(Anc.Last ()).First());
	if (!IsDone(F1,F2)) {
	  BRepOffset_Tool::Inter3D (F1,F2,LInt1,LInt2,mySide,E,Standard_True);
	  Store (F1,F2,LInt1,LInt2);
	}
      }	  
    }
  }
  //---------------------------------------------------------------------
  // etape 2 : Intersections of tubes sharing a vertex without sphere with:
  //           - tubes on each other edge sharing the vertex
  //           - faces containing an edge connected to vertex that has no tubes.
  //---------------------------------------------------------------------
  TopoDS_Vertex                      V[2];
  TopTools_ListIteratorOfListOfShape it; 
  
  for (Exp.Init(ShapeInit,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    const TopoDS_Edge& E1 = TopoDS::Edge(Exp.Current());
    if (InitOffsetFace.HasImage(E1)) {
      //---------------------------
      // E1 generated a tube.
      //---------------------------
      F1 = TopoDS::Face(InitOffsetFace.Image(E1).First());;
      TopExp::Vertices(E1,V[0],V[1]);
      const TopTools_ListOfShape& AncE1 = Analyse.Ancestors(E1);
      
      for (Standard_Integer i = 0; i < 2; i++) {
	if (!InitOffsetFace.HasImage(V[i])) {
	  //-----------------------------
	  // the vertex has no sphere.
	  //-----------------------------
	  const TopTools_ListOfShape& Anc     = Analyse.Ancestors(V[i]);
	  TopTools_ListOfShape TangOnV;
	  Analyse.TangentEdges(E1,V[i],TangOnV);
	  TopTools_MapOfShape MTEV;
	  for (it.Initialize(TangOnV); it.More(); it.Next()) {
	    MTEV.Add(it.Value());
	  }
	  for (it.Initialize(Anc); it.More(); it.Next()) {
	    const TopoDS_Edge& E2 = TopoDS::Edge(it.Value());
//  Modified by skv - Fri Jan 16 16:27:54 2004 OCC4455 Begin
//	    if (E1.IsSame(E2) || MTEV.Contains(E2)) continue;
	    Standard_Boolean isToSkip = Standard_False;

	    if (!E1.IsSame(E2)) {
	      const BRepOffset_ListOfInterval& aL = Analyse.Type(E2);

	      isToSkip = (MTEV.Contains(E2) && 
			  (aL.IsEmpty() ||
			  (!aL.IsEmpty() && aL.First().Type() != OT)));
	    }

	    if (E1.IsSame(E2) || isToSkip)
	      continue;
//  Modified by skv - Fri Jan 16 16:27:54 2004 OCC4455 End
	    if (InitOffsetFace.HasImage(E2)) {
	      //-----------------------------
	      // E2 generated a tube.
	      //-----------------------------
	      F2 = TopoDS::Face(InitOffsetFace.Image(E2).First());	
	      if (!IsDone(F1,F2)) {
		//---------------------------------------------------------------------
		// Intersection tube/tube if the edges are not tangent (AFINIR).
		//----------------------------------------------------------------------
		BRepOffset_Tool::PipeInter (F1,F2,LInt1,LInt2,mySide);
		Store (F1,F2,LInt1,LInt2);
	      }
	    }
	    else {
	      //-------------------------------------------------------
	      // Intersection of the tube of E1 with faces //
	      // to face containing E2 if they are not tangent
	      // to the tube or if E2 is not a tangent edge.
	      //-------------------------------------------------------
	      const BRepOffset_ListOfInterval& L = Analyse.Type(E2);
 	      if (!L.IsEmpty() && L.First().Type() == BRepOffset_Tangent) {
		continue;
	      }
	      const TopTools_ListOfShape& AncE2        = Analyse.Ancestors(E2);
	      Standard_Boolean            TangentFaces = Standard_False;
	      if (AncE2.Extent() == 2) {
		TopoDS_Face InitF2 = TopoDS::Face(AncE2.First ());
		TangentFaces = (InitF2.IsSame(AncE1.First()) || 
				InitF2.IsSame(AncE1.Last()));
		if (!TangentFaces) {
		  F2 = TopoDS::Face(InitOffsetFace.Image(InitF2).First());
		  if (!IsDone(F1,F2)) {
		    BRepOffset_Tool::Inter3D (F1,F2,LInt1,LInt2,mySide,NullEdge);
		    Store (F1,F2,LInt1,LInt2);
		  }
		}
		InitF2 = TopoDS::Face(AncE2.Last ());
		TangentFaces = (InitF2.IsSame(AncE1.First()) || 
				InitF2.IsSame(AncE1.Last()));
		if (!TangentFaces) {
		  F2 = TopoDS::Face(InitOffsetFace.Image(InitF2).First());
		  if (!IsDone(F1,F2)) {
		    BRepOffset_Tool::Inter3D (F1,F2,LInt1,LInt2,mySide,NullEdge);
		    Store (F1,F2,LInt1,LInt2);
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}


//=======================================================================
//function : ConnexIntByInt
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::ConnexIntByInt
(const TopoDS_Shape&                    SI,
 const BRepOffset_DataMapOfShapeOffset& MapSF,
 const BRepOffset_Analyse&              Analyse,
 TopTools_DataMapOfShapeShape&          MES,
 TopTools_DataMapOfShapeShape&          Build,
 TopTools_ListOfShape&                  Failed)
{
  //TopExp_Explorer Exp(SI,TopAbs_EDGE);
  TopTools_IndexedMapOfShape Emap;
  TopExp::MapShapes( SI, TopAbs_EDGE, Emap );
  TopoDS_Face     F1,F2,OF1,OF2,NF1,NF2;
  TopAbs_State    CurSide = mySide;
  BRep_Builder    B;
  TopTools_ListIteratorOfListOfShape it;

  //for (; Exp.More(); Exp.Next()) {
  for (Standard_Integer i = 1; i <= Emap.Extent(); i++) {
    //const TopoDS_Edge&               E = TopoDS::Edge(Exp.Current());
    const TopoDS_Edge& E = TopoDS::Edge(Emap(i));
    const BRepOffset_ListOfInterval& L = Analyse.Type(E);
    if (!L.IsEmpty()) {
      BRepOffset_Type    OT   = L.First().Type();
      if (OT == BRepOffset_Convex || OT == BRepOffset_Concave) {
	if (OT == BRepOffset_Concave) CurSide = TopAbs_IN;
	else                          CurSide = TopAbs_OUT;
	//-----------------------------------------------------------
	// edge is of the proper type, return adjacent faces.
	//-----------------------------------------------------------
	const TopTools_ListOfShape& Anc = Analyse.Ancestors(E);
	if (Anc.Extent() != 2) continue;
	F1  = TopoDS::Face(Anc.First());
	F2  = TopoDS::Face(Anc.Last ());
	OF1 = TopoDS::Face(MapSF(F1).Face()); OF2 = TopoDS::Face(MapSF(F2).Face());
	if (!MES.IsBound(OF1)) {
	  Standard_Boolean enlargeU = Standard_True;
	  Standard_Boolean enlargeVfirst = Standard_True, enlargeVlast = Standard_True;
	  BRepOffset_Tool::CheckBounds( F1, Analyse, enlargeU, enlargeVfirst, enlargeVlast );
	  BRepOffset_Tool::EnLargeFace(OF1,NF1,Standard_True,Standard_True,enlargeU,enlargeVfirst,enlargeVlast);
	  MES.Bind(OF1,NF1);
	}
	else {
	  NF1 = TopoDS::Face(MES(OF1));
	}
	if (!MES.IsBound(OF2)) {
	  Standard_Boolean enlargeU = Standard_True;
	  Standard_Boolean enlargeVfirst = Standard_True, enlargeVlast = Standard_True;
	  BRepOffset_Tool::CheckBounds( F2, Analyse, enlargeU, enlargeVfirst, enlargeVlast );
	  BRepOffset_Tool::EnLargeFace(OF2,NF2,Standard_True,Standard_True,enlargeU,enlargeVfirst,enlargeVlast);
	  MES.Bind(OF2,NF2); 
	}
	else {
	  NF2 = TopoDS::Face(MES(OF2));
	}
	if (!IsDone(NF1,NF2)) {
	  TopTools_ListOfShape LInt1,LInt2;
	  BRepOffset_Tool::Inter3D (NF1,NF2,LInt1,LInt2,CurSide,E,Standard_True);
	  if (LInt1.Extent() > 1)
	    { 
	      // intersection is in seceral edges (free sewing)
	      SelectEdge( NF1, NF2, E, LInt1 );
	      SelectEdge( NF1, NF2, E, LInt2 );
	    }
	  SetDone(NF1,NF2);
	  if (!LInt1.IsEmpty()) {
	    Store (NF1,NF2,LInt1,LInt2);
	    TopoDS_Compound C;
	    B.MakeCompound(C);
	    for (it.Initialize(LInt1) ; it.More(); it.Next()) {
	      B.Add(C,it.Value());
	    }
	    Build.Bind(E,C);
	  }
	  else {
	    Failed.Append(E);
	  }
	} else { // IsDone(NF1,NF2)
	  //  Modified by skv - Fri Dec 26 12:20:13 2003 OCC4455 Begin
	  const TopTools_ListOfShape &aLInt1 = myAsDes->Descendant(NF1);
	  const TopTools_ListOfShape &aLInt2 = myAsDes->Descendant(NF2);

	  if (!aLInt1.IsEmpty()) {
	    TopoDS_Compound C;
	    TopTools_ListIteratorOfListOfShape anIt2;

	    B.MakeCompound(C);

	    for (it.Initialize(aLInt1) ; it.More(); it.Next()) {
	      const TopoDS_Shape &anE1 = it.Value();

	      for (anIt2.Initialize(aLInt2) ; anIt2.More(); anIt2.Next()) {
		const TopoDS_Shape &anE2 = anIt2.Value();

		if (anE1.IsSame(anE2))
		  B.Add(C, anE1);
	      }
	    }
	    Build.Bind(E,C);
	  }
	  else {
	    Failed.Append(E);
	  }
	}
	//  Modified by skv - Fri Dec 26 12:20:14 2003 OCC4455 End
      }	  
    }
  }
}

//=======================================================================
//function : ContextIntByInt
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::ContextIntByInt
(const TopTools_IndexedMapOfShape&      ContextFaces, 
 const Standard_Boolean                 ExtentContext,
 const BRepOffset_DataMapOfShapeOffset& MapSF,
 const BRepOffset_Analyse&              Analyse,
 TopTools_DataMapOfShapeShape&          MES,
 TopTools_DataMapOfShapeShape&          Build,
 TopTools_ListOfShape&                  Failed)
{
  TopTools_MapOfShape              MV;
  TopExp_Explorer                  exp;
  TopoDS_Face                      OF,NF,WCF;
  TopoDS_Edge                      OE;
  TopoDS_Compound                  C;
  BRep_Builder                     B;
  TopTools_ListIteratorOfListOfShape it;
  Standard_Integer i;
  
  for (i = 1; i <= ContextFaces.Extent(); i++) {
    const TopoDS_Face& CF = TopoDS::Face(ContextFaces(i));
    myTouched.Add(CF);
    if (ExtentContext) {
      BRepOffset_Tool::EnLargeFace(CF,NF,0,0);
      MES.Bind(CF,NF);
    }
  }
  TopAbs_State Side = TopAbs_OUT;
 
  for (i = 1; i <= ContextFaces.Extent(); i++) {
    const TopoDS_Face& CF  = TopoDS::Face(ContextFaces(i));
    if (ExtentContext) WCF = TopoDS::Face(MES(CF));
    else               WCF = CF;

    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (!Analyse.HasAncestor(E)) {
	//----------------------------------------------------------------
	// the edges of faces of context that are not in the initial shape
	// can appear in the result.
	//----------------------------------------------------------------
	if (!ExtentContext) {
	  myAsDes->Add(CF,E);
	  myNewEdges.Add(E);
	}
	else {
	  if (!MES.IsBound(E)) {
	    TopoDS_Edge NE;
	    Standard_Real f,l,Tol;
	    BRep_Tool::Range(E,f,l);
	    Tol = BRep_Tool::Tolerance(E);
	    ExtentEdge(CF,E,NE);
	    TopoDS_Vertex V1,V2;
	    TopExp::Vertices(E,V1,V2);
	    NE.Orientation(TopAbs_FORWARD);
	    myAsDes->Add(NE,V1.Oriented(TopAbs_REVERSED));
	    myAsDes->Add(NE,V2.Oriented(TopAbs_FORWARD));
	    TopoDS_Shape aLocalShape = V1.Oriented(TopAbs_INTERNAL);
	    B.UpdateVertex(TopoDS::Vertex(aLocalShape),f,NE,Tol);
	    aLocalShape = V2.Oriented(TopAbs_INTERNAL);
	    B.UpdateVertex(TopoDS::Vertex(aLocalShape),l,NE,Tol);
//	    B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),f,NE,Tol);
//	    B.UpdateVertex(TopoDS::Vertex(V2.Oriented(TopAbs_INTERNAL)),l,NE,Tol);
	    NE.Orientation(E.Orientation());
	    myAsDes->Add(CF,NE);
	    myNewEdges.Add(NE);
	    MES.Bind(E,NE);
	  }
	  else {
	    TopoDS_Shape NE = MES(E);
	    TopoDS_Shape aLocalShape = NE.Oriented(E.Orientation());
	    myAsDes->Add(CF,aLocalShape);
//	    myAsDes->Add(CF,NE.Oriented(E.Orientation()));
	  }
	}
	continue;
      } 
      const TopTools_ListOfShape& Anc = Analyse.Ancestors(E);
      const TopoDS_Face&          F   = TopoDS::Face(Anc.First());
      OF = TopoDS::Face(MapSF(F).Face());
      TopoDS_Shape aLocalShape = MapSF(F).Generated(E);
      OE = TopoDS::Edge(aLocalShape);
//      OE = TopoDS::Edge(MapSF(F).Generated(E));
      if (!MES.IsBound(OF)) {
	BRepOffset_Tool::EnLargeFace(OF,NF,1,1);
	MES.Bind(OF,NF);
      }
      else {
	NF = TopoDS::Face(MES(OF));
      }
      if (!IsDone(NF,CF)) {
	TopTools_ListOfShape LInt1,LInt2;
	TopTools_ListOfShape LOE;
	LOE.Append(OE);
	BRepOffset_Tool::Inter3D (WCF,NF,LInt1,LInt2,Side,E,Standard_True);
	SetDone(NF,CF);
	if (!LInt1.IsEmpty()) {
	  Store (CF,NF,LInt1,LInt2);
	  if (LInt1.Extent() == 1) {
	    Build.Bind(E,LInt1.First());
	  }
	  else {
	    B.MakeCompound(C);
	    for (it.Initialize(LInt1) ; it.More(); it.Next()) {
	      B.Add(C,it.Value());
	    }
	    Build.Bind(E,C);
	  }
	}
	else {
	  Failed.Append(E);
	}
      }
    }
  }
}

//=======================================================================
//function : ContextIntByArc
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::ContextIntByArc(const TopTools_IndexedMapOfShape& ContextFaces, 
					 const Standard_Boolean            InSide,
					 const BRepOffset_Analyse&         Analyse, 
					 const BRepAlgo_Image&             InitOffsetFace, 
					       BRepAlgo_Image&             InitOffsetEdge)

{ 
  TopTools_ListOfShape                      LInt1,LInt2;
  TopTools_MapOfShape                       MV;
  TopExp_Explorer                           exp;
  TopoDS_Face                               OF1,OF2;
  TopoDS_Edge                               OE;
  BRep_Builder                              B;  
  TopoDS_Edge                               NullEdge;
  Standard_Integer j;

  for (j = 1; j <= ContextFaces.Extent(); j++) {
    const TopoDS_Face& CF = TopoDS::Face(ContextFaces(j));
    myTouched.Add(CF);
  }

  for (j = 1; j <= ContextFaces.Extent(); j++) {
    const TopoDS_Face& CF = TopoDS::Face(ContextFaces(j));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (!Analyse.HasAncestor(E)) {
	if (InSide)
	  myAsDes->Add(CF,E);
	else {
	  TopoDS_Edge NE;
	  if (!InitOffsetEdge.HasImage(E)) {
	    Standard_Real f,l,Tol;
	    BRep_Tool::Range(E,f,l);
	    Tol = BRep_Tool::Tolerance(E);
	    ExtentEdge(CF,E,NE);
	    TopoDS_Vertex V1,V2;
	    TopExp::Vertices(E,V1,V2);
	    NE.Orientation(TopAbs_FORWARD);
	    myAsDes->Add(NE,V1.Oriented(TopAbs_REVERSED));
	    myAsDes->Add(NE,V2.Oriented(TopAbs_FORWARD));
	    TopoDS_Shape aLocalShape = V1.Oriented(TopAbs_INTERNAL);
	    B.UpdateVertex(TopoDS::Vertex(aLocalShape),f,NE,Tol);
	    aLocalShape = V2.Oriented(TopAbs_INTERNAL);
	    B.UpdateVertex(TopoDS::Vertex(aLocalShape),l,NE,Tol);
//	    B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),f,NE,Tol);
//	    B.UpdateVertex(TopoDS::Vertex(V2.Oriented(TopAbs_INTERNAL)),l,NE,Tol);
	    NE.Orientation(E.Orientation());
	    myAsDes->Add(CF,NE);
	    InitOffsetEdge.Bind(E,NE);
	  }
	  else {
	    NE = TopoDS::Edge(InitOffsetEdge.Image(E).First());
	    myAsDes->Add(CF,NE.Oriented(E.Orientation()));
	  }
	}
	continue;
      }
      OE.Nullify();
      //---------------------------------------------------
      // OF1 parallel facee generated by the ancestor of E.
      //---------------------------------------------------
      const TopoDS_Shape SI = Analyse.Ancestors(E).First();
      OF1 = TopoDS::Face(InitOffsetFace.Image(SI).First());
      OE  = TopoDS::Edge(InitOffsetEdge.Image(E).First());     

      {
        //Check if OE has pcurve in CF

        Standard_Real   f,l;

        Handle (Geom2d_Curve) C1 = BRep_Tool::CurveOnSurface(OE,CF,f,l);
        Handle (Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(OE,OF1,f,l); 

        if(C1.IsNull() || C2.IsNull())
        {
          continue;
        }
      }

      //--------------------------------------------------
      // MAJ of OE on cap CF.
      //--------------------------------------------------
//      TopTools_ListOfShape LOE; LOE.Append(OE);	      
//      BRepOffset_Tool::TryProject(CF,OF1,LOE,LInt1,LInt2,mySide);
//      LInt2.Clear();
//      StoreInter3d(CF,OF1,myTouched,NewEdges,InterDone,myAsDes,
//		   LInt1,LInt2);
      LInt1.Clear(); LInt1.Append(OE);
      LInt2.Clear();    
      TopAbs_Orientation O1,O2;
      BRepOffset_Tool::OrientSection(OE,CF,OF1,O1,O2);
//      if (mySide == TopAbs_OUT) O1 = TopAbs::Reverse(O1);
      O1 = TopAbs::Reverse(O1);
      LInt1.First().Orientation(O1);
      Store(CF,OF1,LInt1,LInt2);
      
      //------------------------------------------------------
      // Processing of offsets on the ancestors of vertices.
      //------------------------------------------------------
      TopoDS_Vertex V[2];
      TopExp::Vertices (E,V[0],V[1]);
      for (Standard_Integer i = 0; i < 2; i++) {
	if (!MV.Add(V[i])) continue;
	OF1.Nullify(); 
	const TopTools_ListOfShape& LE =  Analyse.Ancestors(V[i]);
	TopTools_ListIteratorOfListOfShape itLE(LE);
	for ( ; itLE.More(); itLE.Next()) {
	  const TopoDS_Edge& EV = TopoDS::Edge(itLE.Value());
	  if (InitOffsetFace.HasImage(EV)) {
	    //-------------------------------------------------
	    // OF1 parallel face generated by an ancester edge of V[i].
	    //-------------------------------------------------
	    OF1 = TopoDS::Face(InitOffsetFace.Image(EV).First());
	    OE  = TopoDS::Edge(InitOffsetEdge.Image(V[i]).First());

      {
        //Check if OE has pcurve in CF and OF1

        Standard_Real   f,l;

        Handle (Geom2d_Curve) C1 = BRep_Tool::CurveOnSurface(OE,CF,f,l);
        Handle (Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(OE,OF1,f,l); 

        if(C1.IsNull() || C2.IsNull())
        {
          continue;
        }
      }

	    //--------------------------------------------------
	    // MAj of OE on cap CF.
	    //--------------------------------------------------
	    //	      LOE.Clear(); LOE.Append(OE);
	    //	      BRepOffset_Tool::TryProject(CF,OF1,LOE,LInt1,LInt2,mySide);
	    //	      LInt2.Clear();
	    //	      StoreInter3d(CF,OF1,myTouched,NewEdges,InterDone,myAsDes,
	    //			   LInt1,LInt2);
	    LInt1.Clear(); LInt1.Append(OE);
	    LInt2.Clear();    
	    TopAbs_Orientation anOri1, anOri2;
	    BRepOffset_Tool::OrientSection(OE,CF,OF1, anOri1,anOri2);
//	    if (mySide == TopAbs_OUT);
            anOri1 = TopAbs::Reverse(anOri1);
	    LInt1.First().Orientation(anOri1);
	    Store(CF,OF1,LInt1,LInt2);
	  }
	}
      }
    }
    
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_VERTEX); 
	 exp.More(); exp.Next()) {
      const TopoDS_Vertex&        V  = TopoDS::Vertex(exp.Current());
      if (!Analyse.HasAncestor(V)) {
	continue;
      }
      const TopTools_ListOfShape& LE =  Analyse.Ancestors(V);
      TopTools_ListIteratorOfListOfShape itLE(LE);
      for (; itLE.More(); itLE.Next()) {
	const TopoDS_Edge& EV = TopoDS::Edge(itLE.Value());
	const TopTools_ListOfShape& LF = Analyse.Ancestors(EV);
	TopTools_ListIteratorOfListOfShape itLF(LF);
	for ( ; itLF.More(); itLF.Next()) {
	  const TopoDS_Face& FEV = TopoDS::Face(itLF.Value());
	  //-------------------------------------------------
	  // OF1 parallel face generated by uneFace ancestor of V[i].
	  //-------------------------------------------------
	  OF1 = TopoDS::Face(InitOffsetFace.Image(FEV).First());
	  if (!IsDone(OF1,CF)) {
	    //-------------------------------------------------------
	    // Find if one of edges of OF1 has no trace in CF.
	    //-------------------------------------------------------
	    TopTools_ListOfShape LOE;
	    TopExp_Explorer exp2(OF1.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
	    for ( ;exp2.More(); exp2.Next()) {
	      LOE.Append(exp2.Current());
	    }
	    BRepOffset_Tool::TryProject(CF,OF1,LOE,LInt1,LInt2,mySide,myTol);
	    //-------------------------------------------------------
	    // If no trace try intersection.
	    //-------------------------------------------------------
	    if (LInt1.IsEmpty()) {
	      BRepOffset_Tool::Inter3D (CF,OF1,LInt1,LInt2,mySide,NullEdge);
	    }
	    Store (CF,OF1,LInt1,LInt2);
	  }
	}
      }
    } 
  }
}

//=======================================================================
//function : AddCommonEdges
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::AddCommonEdges(const TopTools_ListOfShape&)
{
}


//=======================================================================
//function : SetDone
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::SetDone(const TopoDS_Face& F1, 
				 const TopoDS_Face& F2)
{
  if (!myDone.IsBound(F1)) {
    TopTools_ListOfShape empty;
    myDone.Bind(F1,empty);
  }
  myDone(F1).Append(F2);
  if (!myDone.IsBound(F2)) {
    TopTools_ListOfShape empty;
    myDone.Bind(F2,empty);
  }
  myDone(F2).Append(F1);
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffset_Inter3d::IsDone(const TopoDS_Face& F1, 
					    const TopoDS_Face& F2) 
const 
{
  if (myDone.IsBound(F1)) {
    TopTools_ListIteratorOfListOfShape it (myDone(F1));
    for (; it.More(); it.Next()) {
      if (it.Value().IsSame(F2)) return Standard_True;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : TouchedFaces
//purpose  : 
//=======================================================================

TopTools_IndexedMapOfShape& BRepOffset_Inter3d::TouchedFaces()
{
  return myTouched;
}


//=======================================================================
//function : AsDes
//purpose  : 
//=======================================================================

Handle(BRepAlgo_AsDes) BRepOffset_Inter3d::AsDes() const 
{
  return myAsDes;
}


//=======================================================================
//function : NewEdges
//purpose  : 
//=======================================================================

TopTools_IndexedMapOfShape& BRepOffset_Inter3d::NewEdges() 
{
  return myNewEdges;
}



//=======================================================================
//function : Store
//purpose  : 
//=======================================================================

void BRepOffset_Inter3d::Store(const TopoDS_Face& F1, 
			       const TopoDS_Face& F2, 
			       const TopTools_ListOfShape& LInt1, 
			       const TopTools_ListOfShape& LInt2)
{
  if (!LInt1.IsEmpty()) {
    myTouched.Add(F1);
    myTouched.Add(F2);
    myAsDes->Add( F1,LInt1);
    myAsDes->Add( F2,LInt2);
    TopTools_ListIteratorOfListOfShape it(LInt1);
    for (; it.More(); it.Next()) {
      myNewEdges.Add(it.Value());
    }
  }
  SetDone(F1,F2);
}



