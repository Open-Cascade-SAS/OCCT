// Created on: 1995-10-20
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_Surface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_Interval.hxx>
#include <BRepOffset_ListIteratorOfListOfInterval.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//
static void Correct2dPoint(const TopoDS_Face& theF, gp_Pnt2d& theP2d);
//
static BRepOffset_Type DefineConnectType(const TopoDS_Edge&         E,
			                                   const TopoDS_Face&         F1,
			                                   const TopoDS_Face&         F2,
			                                   const Standard_Real        SinTol,
                                         const Standard_Boolean     CorrectPoint);
//
static void CorrectOrientationOfTangent(gp_Vec& TangVec,
                                        const TopoDS_Vertex& aVertex,
                                        const TopoDS_Edge& anEdge)
{
  TopoDS_Vertex Vlast = TopExp::LastVertex(anEdge);
  if (aVertex.IsSame(Vlast))
    TangVec.Reverse();
}
//=======================================================================
//function : BRepOffset_Analyse
//purpose  : 
//=======================================================================

BRepOffset_Analyse::BRepOffset_Analyse()
:myDone(Standard_False)
{
}


//=======================================================================
//function : BRepOffset_Analyse
//purpose  : 
//=======================================================================

BRepOffset_Analyse::BRepOffset_Analyse(const TopoDS_Shape& S, 
				       const Standard_Real Angle)
:myDone(Standard_False)
{
  Perform( S, Angle);
}

//=======================================================================
//function : EdgeAnlyse
//purpose  : 
//=======================================================================

static void EdgeAnalyse(const TopoDS_Edge&         E,
			                  const TopoDS_Face&         F1,
			                  const TopoDS_Face&         F2,
			                  const Standard_Real        SinTol,
			                        BRepOffset_ListOfInterval& LI)
{
  
  Standard_Real   f,l;
  BRep_Tool::Range(E, F1, f, l);
  BRepOffset_Interval I;
  I.First(f); I.Last(l);
  //
  // Tangent if the regularity is at least G1.
  if (BRep_Tool::HasContinuity(E,F1,F2)) {
    if (BRep_Tool::Continuity(E,F1,F2) > GeomAbs_C0) {
      I.Type(BRepOffset_Tangent);
      LI.Append(I);
      return;
    }
  }
  //
  BRepOffset_Type aType = DefineConnectType(E, F1, F2, SinTol, Standard_False);
  if(aType != BRepOffset_Tangent)
  {
    aType = DefineConnectType(E, F1, F2, SinTol, Standard_True);
  }
  I.Type(aType);
  LI.Append(I);
}

//=======================================================================
//function : BuildAncestors
//purpose  : 
//=======================================================================

static void BuildAncestors (const TopoDS_Shape&                        S,
                            TopTools_IndexedDataMapOfShapeListOfShape& MA)
{  
  MA.Clear();
  TopExp::MapShapesAndUniqueAncestors(S,TopAbs_VERTEX,TopAbs_EDGE,MA);
  TopExp::MapShapesAndUniqueAncestors(S,TopAbs_EDGE  ,TopAbs_FACE,MA);
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffset_Analyse::IsDone() const 
{
  return myDone;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::Perform (const TopoDS_Shape& S, 
				  const Standard_Real Angle)
{
  myShape = S;

  angle                = Angle;
  Standard_Real SinTol = Sin(Angle);

  // Build ancestors.
  BuildAncestors (S,ancestors);

  
  TopExp_Explorer Exp(S.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
  for ( ; Exp.More(); Exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
    if (!mapEdgeType.IsBound(E)) {
      BRepOffset_ListOfInterval LI;
      mapEdgeType.Bind(E,LI);
      
      const TopTools_ListOfShape& L = Ancestors(E);
      if ( L.IsEmpty()) 
	continue;

      if (L.Extent() == 2) {
	const TopoDS_Face& F1 = TopoDS::Face(L.First());
	const TopoDS_Face& F2 = TopoDS::Face(L.Last ());
	EdgeAnalyse(E,F1,F2,SinTol,mapEdgeType(E));
      }
      else if (L.Extent() == 1) {
	Standard_Real U1,U2;
	const TopoDS_Face& F = TopoDS::Face(L.First());
	BRep_Tool::Range(E,F,U1,U2);
	BRepOffset_Interval Inter(U1,U2,BRepOffset_Other);
	
	if (! BRepTools::IsReallyClosed(E,F)) {
	  Inter.Type(BRepOffset_FreeBoundary);
	}
	mapEdgeType(E).Append(Inter);
      }
      else {  
#ifdef OCCT_DEBUG
	cout <<"edge shared by more than two faces"<<endl;
#endif	
      }
    }
  }
  myDone = Standard_True;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::Clear()
{
  myDone = Standard_False;
  myShape     .Nullify();
  mapEdgeType.Clear();
  ancestors  .Clear();
}





//=======================================================================
//function : BRepOffset_ListOfInterval&
//purpose  : 
//=======================================================================

const BRepOffset_ListOfInterval& BRepOffset_Analyse::Type(const TopoDS_Edge& E)
const 
{
  return mapEdgeType (E);
}


//=======================================================================
//function : Edges
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::Edges(const TopoDS_Vertex&  V, 
			       const BRepOffset_Type T,
			       TopTools_ListOfShape& LE) 
const 
{
  LE.Clear();
  const TopTools_ListOfShape& L = Ancestors (V);
  TopTools_ListIteratorOfListOfShape it(L);
  
  for ( ;it.More(); it.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(it.Value());
    TopoDS_Vertex V1,V2;
    BRepOffset_Tool::EdgeVertices (E,V1,V2);
    if (V1.IsSame(V)) {
      if (mapEdgeType(E).Last().Type() == T)
	LE.Append(E);
    }
    if (V2.IsSame(V)) {
      if (mapEdgeType(E).First().Type() == T)
	LE.Append(E);
    }
  }
}


//=======================================================================
//function : Edges
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::Edges(const TopoDS_Face&    F, 
			       const BRepOffset_Type T,
			       TopTools_ListOfShape& LE) 
const 
{
  LE.Clear();
  TopExp_Explorer exp(F, TopAbs_EDGE);
  
  for ( ;exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());

    const BRepOffset_ListOfInterval& Lint = Type(E);
    BRepOffset_ListIteratorOfListOfInterval it(Lint);
    for ( ;it.More(); it.Next()) {
      if (it.Value().Type() == T) LE.Append(E);
    }
  }
}

//=======================================================================
//function : TangentEdges
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::TangentEdges(const TopoDS_Edge&    Edge  ,
				      const TopoDS_Vertex&  Vertex,
				      TopTools_ListOfShape& Edges  ) const 
{
  gp_Vec V,VRef;


  Standard_Real U,URef;
  BRepAdaptor_Curve C3d, C3dRef;

  URef   = BRep_Tool::Parameter(Vertex,Edge);
  C3dRef = BRepAdaptor_Curve(Edge);
  VRef   = C3dRef.DN(URef,1);
  CorrectOrientationOfTangent(VRef, Vertex, Edge);
  if (VRef.SquareMagnitude() < gp::Resolution()) return;

  Edges.Clear();

  const TopTools_ListOfShape& Anc = Ancestors(Vertex);
  TopTools_ListIteratorOfListOfShape it(Anc);
  for ( ; it.More(); it.Next()) {
    const TopoDS_Edge& CurE = TopoDS::Edge(it.Value());
    if ( CurE.IsSame(Edge)) continue;
    U   = BRep_Tool::Parameter(Vertex,CurE);
    C3d = BRepAdaptor_Curve(CurE);
    V   = C3d.DN(U,1);
    CorrectOrientationOfTangent(V, Vertex, CurE);
    if (V.SquareMagnitude() < gp::Resolution()) continue;
    if (V.IsOpposite(VRef,angle)) {
      Edges.Append(CurE);
    }
  }
}



//=======================================================================
//function : HasAncestor
//purpose  : 
//=======================================================================

Standard_Boolean  BRepOffset_Analyse::HasAncestor (const TopoDS_Shape& S) const 
{
  return ancestors.Contains(S);
}


//=======================================================================
//function : Ancestors
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffset_Analyse::Ancestors 
(const TopoDS_Shape& S) const 
{
  return ancestors.FindFromKey(S);
}


//=======================================================================
//function : Explode
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::Explode(      TopTools_ListOfShape& List,
				 const BRepOffset_Type       T   ) const 
{
  List.Clear();
  BRep_Builder B;
  TopTools_MapOfShape Map;
  
  TopExp_Explorer Fexp;
  for (Fexp.Init(myShape,TopAbs_FACE); Fexp.More(); Fexp.Next()) {
    if ( Map.Add(Fexp.Current())) {
      TopoDS_Face Face = TopoDS::Face(Fexp.Current());
      TopoDS_Compound Co;
      B.MakeCompound(Co);
      B.Add(Co,Face);
      // add to Co all faces from the cloud of faces
      // G1 created from <Face>
      AddFaces(Face,Co,Map,T);
      List.Append(Co);
    }
  }
}

//=======================================================================
//function : Explode
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::Explode(      TopTools_ListOfShape& List,
				 const BRepOffset_Type       T1,
				 const BRepOffset_Type       T2) const 
{
  List.Clear();
  BRep_Builder B;
  TopTools_MapOfShape Map;
  
  TopExp_Explorer Fexp;
  for (Fexp.Init(myShape,TopAbs_FACE); Fexp.More(); Fexp.Next()) {
    if ( Map.Add(Fexp.Current())) {
      TopoDS_Face Face = TopoDS::Face(Fexp.Current());
      TopoDS_Compound Co;
      B.MakeCompound(Co);
      B.Add(Co,Face);
      // add to Co all faces from the cloud of faces
      // G1 created from  <Face>
      AddFaces(Face,Co,Map,T1,T2);
      List.Append(Co);
    }
  }
}


//=======================================================================
//function : AddFaces
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::AddFaces (const TopoDS_Face&    Face,
				   TopoDS_Compound&      Co,
				   TopTools_MapOfShape&  Map,
				   const BRepOffset_Type T) const 
{
  BRep_Builder B;
  TopExp_Explorer exp(Face,TopAbs_EDGE);
  for ( ; exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    const BRepOffset_ListOfInterval& LI = Type(E);
    if (!LI.IsEmpty() && LI.First().Type() == T) {
      // so <NewFace> is attached to G1 by <Face>
      const TopTools_ListOfShape& L = Ancestors(E);
      if (L.Extent() == 2) {
	TopoDS_Face F1 = TopoDS::Face(L.First());
	if ( F1.IsSame(Face)) 
	  F1 = TopoDS::Face(L.Last ());
	if ( Map.Add(F1)) {
	  B.Add(Co,F1);
	  AddFaces(F1,Co,Map,T);
	}
      }
    }
  }
}
//=======================================================================
//function : AddFaces
//purpose  : 
//=======================================================================

void BRepOffset_Analyse::AddFaces (const TopoDS_Face&    Face,
				   TopoDS_Compound&      Co,
				   TopTools_MapOfShape&  Map,
				   const BRepOffset_Type T1,
				   const BRepOffset_Type T2) const 
{
  BRep_Builder B;
  TopExp_Explorer exp(Face,TopAbs_EDGE);
  for ( ; exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    const BRepOffset_ListOfInterval& LI = Type(E);
    if (!LI.IsEmpty() && 
	(LI.First().Type() == T1 || LI.First().Type() == T2)) {
      // so <NewFace> is attached to G1 by <Face>
      const TopTools_ListOfShape& L = Ancestors(E);
      if (L.Extent() == 2) {
	TopoDS_Face F1 = TopoDS::Face(L.First());
	if ( F1.IsSame(Face)) 
	  F1 = TopoDS::Face(L.Last ());
	if ( Map.Add(F1)) {
	  B.Add(Co,F1);
	  AddFaces(F1,Co,Map,T1,T2);
	}
      }
    }
  }
}

//=======================================================================
//function : Correct2dPoint
//purpose  : 
//=======================================================================
void Correct2dPoint(const TopoDS_Face& theF, gp_Pnt2d& theP2d)
{
  BRepAdaptor_Surface aBAS(theF, Standard_False);
  if (aBAS.GetType() < GeomAbs_BezierSurface) {
    return;
  }
  //
  const Standard_Real coeff = 0.01;
  Standard_Real eps;
  Standard_Real u1, u2, v1, v2;
  //
  aBAS.Initialize(theF, Standard_True);
  u1 = aBAS.FirstUParameter();
  u2 = aBAS.LastUParameter();
  v1 = aBAS.FirstVParameter();
  v2 = aBAS.LastVParameter();
  if (!(Precision::IsInfinite(u1) || Precision::IsInfinite(u2)))
  {
    eps = Max(coeff*(u2 - u1), Precision::PConfusion());
    if (Abs(theP2d.X() - u1) < eps)
    {
      theP2d.SetX(u1 + eps);
    }
    if (Abs(theP2d.X() - u2) < eps)
    {
      theP2d.SetX(u2 - eps);
    }
  }
  if (!(Precision::IsInfinite(v1) || Precision::IsInfinite(v2)))
  {
    eps = Max(coeff*(v2 - v1), Precision::PConfusion());
    if (Abs(theP2d.Y() - v1) < eps)
    {
      theP2d.SetY(v1 + eps);
    }
    if (Abs(theP2d.Y() - v2) < eps)
    {
      theP2d.SetY(v2 - eps);
    }
  }
}

//=======================================================================
//function : DefineConnectType
//purpose  : 
//=======================================================================
BRepOffset_Type DefineConnectType(const TopoDS_Edge&     E,
                                  const TopoDS_Face&     F1,
                                  const TopoDS_Face&     F2,
                                  const Standard_Real    SinTol,
                                  const Standard_Boolean CorrectPoint)
{
  TopLoc_Location L;
  Standard_Real   f,l;
  
  const Handle(Geom_Surface)& S1 = BRep_Tool::Surface(F1);
  const Handle(Geom_Surface)& S2 = BRep_Tool::Surface(F2);
  //
  Handle (Geom2d_Curve) C1 = BRep_Tool::CurveOnSurface(E,F1,f,l);
  Handle (Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(E,F2,f,l);

  BRepAdaptor_Curve C(E);
  f = C.FirstParameter();
  l = C.LastParameter();
//
  Standard_Real ParOnC = 0.5*(f+l);
  gp_Vec T1 = C.DN(ParOnC,1).Transformed(L.Transformation());
  if (T1.SquareMagnitude() > gp::Resolution()) {
    T1.Normalize();
  }
  
  if (BRepOffset_Tool::OriEdgeInFace(E,F1) == TopAbs_REVERSED) {
    T1.Reverse();
  }
  if (F1.Orientation() == TopAbs_REVERSED) T1.Reverse();

  gp_Pnt2d P  = C1->Value(ParOnC);
  gp_Pnt   P3;
  gp_Vec   D1U,D1V;
  
  if(CorrectPoint) 
    Correct2dPoint(F1, P);
  //
  S1->D1(P.X(),P.Y(),P3,D1U,D1V);
  gp_Vec DN1(D1U^D1V);
  if (F1.Orientation() == TopAbs_REVERSED) DN1.Reverse();
  
  P = C2->Value(ParOnC);
  if(CorrectPoint) 
    Correct2dPoint(F2, P);
  S2->D1(P.X(),P.Y(),P3,D1U,D1V);
  gp_Vec DN2(D1U^D1V);
  if (F2.Orientation() == TopAbs_REVERSED) DN2.Reverse();

  DN1.Normalize();
  DN2.Normalize();

  gp_Vec        ProVec     = DN1^DN2;
  Standard_Real NormProVec = ProVec.Magnitude(); 

  if (Abs(NormProVec) < SinTol) {
    // plane
    if (DN1.Dot(DN2) > 0) {   
      //Tangent
      return BRepOffset_Tangent;
    }
    else  {                   
      //Mixed not finished!
#ifdef OCCT_DEBUG
      cout <<" faces locally mixed"<<endl;
#endif
      return BRepOffset_Convex;
    }
  }
  else {  
    if (NormProVec > gp::Resolution())
      ProVec.Normalize();
    Standard_Real Prod  = T1.Dot(DN1^DN2);
    if (Prod > 0.) {       
      //
      return BRepOffset_Convex;
    }
    else {                       
      //reenters
      return BRepOffset_Concave;
    }
  }
}
