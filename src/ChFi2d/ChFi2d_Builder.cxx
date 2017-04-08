// Created on: 1995-07-07
// Created by: Joelle CHAUVET
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <ChFi2d.hxx>
#include <ChFi2d_Builder.hxx>
#include <ElCLib.hxx>
#include <GccEnt_Position.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

static Standard_Boolean IsIssuedFrom(const TopoDS_Edge& E,
				     const TopTools_IndexedMapOfShape& Map,
				     TopoDS_Edge& BasisEdge);

static Standard_Boolean IsLineOrCircle(const TopoDS_Edge& E,
				       const TopoDS_Face& F);


//=======================================================================
//function : ChFi2d_Builder
//purpose  : 
//=======================================================================

 ChFi2d_Builder::ChFi2d_Builder()
{
}

//=======================================================================
//function : ChFi2d_Builder
//purpose  : 
//=======================================================================

 ChFi2d_Builder::ChFi2d_Builder(const TopoDS_Face& F)
{
  if (F.IsNull()) {
    status = ChFi2d_NoFace;
    return;
  }
  TopLoc_Location Loc;
//  syntaxe invalide sur NT
//  const Handle(Geom_Surface)&  surf = BRep_Tool::Surface( F, Loc);
//  if (surf->IsKind(STANDARD_TYPE(Geom_Plane))) {
  if ( BRep_Tool::Surface( F, Loc)
                        -> IsKind(STANDARD_TYPE(Geom_Plane)) ) {
    newFace = refFace = F;
    newFace.Orientation(TopAbs_FORWARD);
    BRepLib::BuildCurves3d(newFace);
    status = ChFi2d_Ready;
  }
  else status = ChFi2d_NotPlanar;
} // ChFi2d_Builder

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ChFi2d_Builder::Init(const TopoDS_Face& F)
{
  if (F.IsNull()) {
    status = ChFi2d_NoFace;
    return;
  }
  fillets.Clear();
  chamfers.Clear();
  history.Clear();
  TopLoc_Location Loc;
//  syntaxe invalide sur NT
//  const Handle(Geom_Surface)&  surf = BRep_Tool::Surface( F, Loc);
//  if (surf->IsKind(STANDARD_TYPE(Geom_Plane))) {
  if ( BRep_Tool::Surface( F, Loc)
                        -> IsKind(STANDARD_TYPE(Geom_Plane)) ) {
    newFace = refFace = F;
    newFace.Orientation(TopAbs_FORWARD);
    status = ChFi2d_Ready;
  }
  else status = ChFi2d_NotPlanar;
} // Init


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ChFi2d_Builder::Init(const TopoDS_Face& RefFace, 
			const TopoDS_Face& ModFace)
{
  if (RefFace.IsNull() || ModFace.IsNull()) {
    status = ChFi2d_NoFace;
    return;
  }
  fillets.Clear();
  chamfers.Clear();
  history.Clear();
  TopLoc_Location loc;
//  syntaxe invalide sur NT
//  const Handle(Geom_Surface)&  surf = BRep_Tool::Surface( RefFace, Loc);
//  if (!surf->IsKind(STANDARD_TYPE(Geom_Plane))) {
  if ( ! BRep_Tool::Surface( RefFace, loc)
              -> IsKind(STANDARD_TYPE(Geom_Plane)) ) {
    status = ChFi2d_NotPlanar;
    return;
  }

  refFace = RefFace;
  newFace = ModFace;
  newFace.Orientation(TopAbs_FORWARD);
  status = ChFi2d_Ready;

  // Research in newFace all the edges which not appear in RefFace
  // The sequence newEdges will contains this edges.

  TopTools_SequenceOfShape newEdges;
  TopTools_IndexedMapOfShape refEdgesMap;
  TopExp::MapShapes(refFace, TopAbs_EDGE, refEdgesMap);
  TopExp_Explorer ex(newFace, TopAbs_EDGE);
  while (ex.More()){
    const TopoDS_Edge& currentEdge = TopoDS::Edge(ex.Current());
    if (!refEdgesMap.Contains(currentEdge))
      newEdges.Append(currentEdge);
    ex.Next();
  } // while (ex ...

  // update of history, fillets and chamfers fields
  Standard_Integer i = 1;
  Standard_Real first, last;
  TopoDS_Edge basisEdge;
  while ( i <= newEdges.Length()) {
    const TopoDS_Edge& currentEdge = TopoDS::Edge(newEdges.Value(i));
    if (IsIssuedFrom(currentEdge, refEdgesMap, basisEdge))
      history.Bind(basisEdge, currentEdge);
    else {
      // this edge is a chamfer or a fillet
//  syntaxe invalide sur NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(currentEdge, loc, first, last);
      Handle(Geom_Curve) curve = 
	BRep_Tool::Curve(currentEdge, loc, first, last);
      if (curve->IsKind(STANDARD_TYPE(Geom_Circle))) {
	fillets.Append(currentEdge);
      }
      else if (curve->IsKind(STANDARD_TYPE(Geom_Line))) {
	chamfers.Append(currentEdge);
      }
      else {
	status = ChFi2d_InitialisationError;
	return;
      } // else ...
    } // this edge is ...
    i++;
  } // while ...
} // Init


//=======================================================================
//function : IsIssuedFrom
//purpose  : Search in <Map> if <E> has a parent edge. If a parent has 
//           been find, this edge is returned in <BasisEdge>, else <E> is
//           returned in <BasisEdge>.
//=======================================================================
Standard_Boolean IsIssuedFrom(const TopoDS_Edge& E,
			      const TopTools_IndexedMapOfShape& Map,
			      TopoDS_Edge& BasisEdge)
{
  TopLoc_Location loc1, loc2;
  Standard_Real f1, L1, f2, L2;
//  syntaxe invalide sur NT
//  const Handle(Geom_Curve)& c1 = 
//    BRep_Tool::Curve(E, loc1, f1, L1);
  Handle(Geom_Curve) c1 = BRep_Tool::Curve(E, loc1, f1, L1);

  for (Standard_Integer i = 1; i <= Map.Extent(); i++ ) {
    const TopoDS_Edge& currentEdge = TopoDS::Edge(Map.FindKey(i));
//  syntaxe invalide sur NT
//    const Handle(Geom_Curve)& c2 = 
//      BRep_Tool::Curve(currentEdge, loc2, f2, L2);
    Handle(Geom_Curve) c2 = BRep_Tool::Curve(currentEdge, loc2, f2, L2);
    if (c1 == c2 &&
	(((f1 > f2 && f1 < L2) || (L1 > f2 && L1 < L2) ) ||
	( (f1 > L2 && f1 < f2) || (L1 > L2 && L1 < f2) )) ) {
      BasisEdge = currentEdge;
      BasisEdge.Orientation(E.Orientation());
      return Standard_True;
    } //  if (c1 == c2
  } // for (Standard_Integer i ...

  return Standard_False;
} // IsIssuedFrom


//=======================================================================
//function : AddFillet
//purpose  : 
//=======================================================================
 TopoDS_Edge ChFi2d_Builder::AddFillet(const TopoDS_Vertex& V,
				     const Standard_Real Radius)
{
  TopoDS_Edge adjEdge1, adjEdge2, basisEdge1, basisEdge2;
  TopoDS_Edge adjEdge1Mod, adjEdge2Mod, fillet;
  status = ChFi2d::FindConnectedEdges(newFace, V, adjEdge1, adjEdge2);
  if (status == ChFi2d_ConnexionError) return fillet;

  if (IsAFillet(adjEdge1) || IsAChamfer(adjEdge1) ||
      IsAFillet(adjEdge2) || IsAChamfer(adjEdge2)) {
    status = ChFi2d_NotAuthorized;
    return fillet;
  } //  if (IsAFillet ...

  if (!IsLineOrCircle(adjEdge1,newFace) 
      || !IsLineOrCircle(adjEdge2,newFace) ) {
    status = ChFi2d_NotAuthorized;
    return fillet;
  } //  if (!IsLineOrCircle ...

  ComputeFillet(V, adjEdge1, adjEdge2, Radius,
		adjEdge1Mod, adjEdge2Mod, fillet);
  if (status == ChFi2d_IsDone
      || status == ChFi2d_FirstEdgeDegenerated
      || status == ChFi2d_LastEdgeDegenerated
      || status == ChFi2d_BothEdgesDegenerated) {
    BuildNewWire(adjEdge1, adjEdge2, adjEdge1Mod, fillet, adjEdge2Mod);
    basisEdge1 = BasisEdge(adjEdge1);
    basisEdge2 = BasisEdge(adjEdge2);
    UpDateHistory(basisEdge1, basisEdge2,
		  adjEdge1Mod, adjEdge2Mod, fillet, 1);
    status = ChFi2d_IsDone;
    return TopoDS::Edge(fillets.Value(fillets.Length()));
  }
  return fillet;
} // AddFillet

//=======================================================================
//function : ModifyFillet
//purpose  : 
//=======================================================================

TopoDS_Edge ChFi2d_Builder::ModifyFillet(const TopoDS_Edge& Fillet,
				       const Standard_Real Radius)
{
 TopoDS_Vertex aVertex = RemoveFillet(Fillet);
  TopoDS_Edge aFillet = AddFillet(aVertex, Radius);
 return aFillet;
} // ModifyFillet

//=======================================================================
//function : RemoveFillet
//purpose  : 
//=======================================================================

TopoDS_Vertex ChFi2d_Builder::RemoveFillet(const TopoDS_Edge& Fillet)
{
  TopoDS_Vertex commonVertex;
  Standard_Integer i = 1;
  Standard_Integer IsFind = Standard_False;
  while (i <= fillets.Length()) {
    const TopoDS_Edge& aFillet = TopoDS::Edge(fillets.Value(i));
    if (aFillet.IsSame(Fillet)) {
      fillets.Remove(i);
      IsFind = Standard_True;
      break;
    }
    i++;
  }
  if (!IsFind) return commonVertex;

    
  TopoDS_Vertex firstVertex, lastVertex;
  TopExp::Vertices(Fillet, firstVertex, lastVertex);


  TopoDS_Edge adjEdge1, adjEdge2;
  status = ChFi2d::FindConnectedEdges(newFace, firstVertex,
				     adjEdge1, adjEdge2);
  if (status == ChFi2d_ConnexionError) return commonVertex;

  TopoDS_Edge basisEdge1, basisEdge2, E1, E2;
  // E1 and E2 are the adjacent edges to Fillet

  if (adjEdge1.IsSame(Fillet)) E1 = adjEdge2;
  else E1 = adjEdge1;
  basisEdge1 = BasisEdge(E1);
  status = ChFi2d::FindConnectedEdges(newFace, lastVertex,adjEdge1, adjEdge2);
  if (status == ChFi2d_ConnexionError) return commonVertex;
  if (adjEdge1.IsSame(Fillet)) E2 = adjEdge2;
  else E2 = adjEdge1;
  basisEdge2 = BasisEdge(E2);
  TopoDS_Vertex connectionE1Fillet, connectionE2Fillet;
  Standard_Boolean hasConnection = 
    ChFi2d::CommonVertex(basisEdge1, basisEdge2, commonVertex);
  if (!hasConnection) {
    status = ChFi2d_ConnexionError;
    return commonVertex;
  }
  hasConnection = ChFi2d::CommonVertex(E1, Fillet, connectionE1Fillet);
  if (!hasConnection) {
    status = ChFi2d_ConnexionError;
    return commonVertex;
  }
  hasConnection = ChFi2d::CommonVertex(E2, Fillet, connectionE2Fillet);
  if (!hasConnection) {
    status = ChFi2d_ConnexionError;
    return commonVertex;
  }

  // rebuild edges on wire
  TopoDS_Edge newEdge1, newEdge2;
  TopoDS_Vertex v, v1, v2;
  BRepLib_MakeEdge makeEdge;
  TopLoc_Location loc;
  Standard_Real first, last;
  
  TopExp::Vertices(E1, firstVertex, lastVertex);
  TopExp::Vertices(basisEdge1, v1, v2);
  if (v1.IsSame(commonVertex)) v = v2;
  else v = v1;

  if ( firstVertex.IsSame(v) || lastVertex.IsSame(v)) 
    // It means the edge support only one fillet. In this case
    // the new edge must be the basis edge.
    newEdge1 = basisEdge1;
  else {
    // It means the edge support one fillet on each end.
    if (firstVertex.IsSame(connectionE1Fillet)) {
//  syntaxe invalide sur NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(E1, loc, first, last);   
      Handle(Geom_Curve) curve = BRep_Tool::Curve(E1, loc, first, last);   
      makeEdge.Init(curve, commonVertex, lastVertex);  
      newEdge1 = makeEdge.Edge();
      newEdge1.Orientation(E1.Orientation());
      newEdge1.Location(E1.Location());
    } // if (firstVertex ...
    else if (lastVertex.IsSame(connectionE1Fillet)) { 
//  syntax wrong on NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(E1, loc, first, last);   
      Handle(Geom_Curve) curve = BRep_Tool::Curve(E1, loc, first, last);   
      makeEdge.Init(curve, firstVertex, commonVertex);
      newEdge1 = makeEdge.Edge();
      newEdge1.Orientation(E1.Orientation());
      newEdge1.Location(E1.Location());
    } // else if (lastVertex ...
  } // else ...

  TopExp::Vertices(basisEdge2, v1, v2);
  if (v1.IsSame(commonVertex)) v = v2;
  else v = v1;

  TopExp::Vertices(E2, firstVertex, lastVertex);   
  if ( firstVertex.IsSame(v) || lastVertex.IsSame(v)) 
    // It means the edge support only one fillet. In this case
    // the new edge must be the basis edge.
    newEdge2 = basisEdge2;
  else {
    // It means the edge support one fillet on each end.
    if (firstVertex.IsSame(connectionE2Fillet)) {
//  syntax wrong on NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(E2, loc, first, last);   
      Handle(Geom_Curve) curve = BRep_Tool::Curve(E2, loc, first, last);
      makeEdge.Init(curve, commonVertex, lastVertex);
      newEdge2 = makeEdge.Edge();
      newEdge2.Orientation(E2.Orientation());
      newEdge2.Location(E2.Location());
    } // if (firstVertex ...
    else if (lastVertex.IsSame(connectionE2Fillet)) {
//  syntax wrong on NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(E2, loc, first, last);   
      Handle(Geom_Curve) curve = BRep_Tool::Curve(E2, loc, first, last);
      makeEdge.Init(curve, firstVertex, commonVertex);
      newEdge2 = makeEdge.Edge();
      newEdge2.Orientation(E2.Orientation());
      newEdge2.Location(E2.Location());
    } // else if (lastVertex ... 
  } // else ...

  // rebuild the newFace
  TopExp_Explorer Ex(newFace, TopAbs_EDGE);
  TopoDS_Wire newWire;

  BRep_Builder B;
  B.MakeWire(newWire);

  while (Ex.More()) {
    const TopoDS_Edge& theEdge = TopoDS::Edge(Ex.Current());
    if (!theEdge.IsSame(E1) && 
	!theEdge.IsSame(E2) && 
	!theEdge.IsSame(Fillet)) 
      B.Add(newWire, theEdge);
    else {
      if (theEdge == E1) 
	B.Add(newWire, newEdge1);
      else if (theEdge == E2)
	B.Add(newWire, newEdge2);
    } // else
    Ex.Next();
  } // while ...
  BRepAdaptor_Surface Adaptor3dSurface(refFace);
  BRepLib_MakeFace mFace(Adaptor3dSurface.Plane(), newWire);
  newFace.Nullify();
  newFace = mFace;

  UpDateHistory(basisEdge1, basisEdge2, newEdge1, newEdge2);  

  return commonVertex;
} // RemoveFillet


//=======================================================================
//function : ComputeFillet
//purpose  : 
//=======================================================================

void ChFi2d_Builder::ComputeFillet(const TopoDS_Vertex& V, 
				 const TopoDS_Edge& E1, 
				 const TopoDS_Edge& E2, 
				 const Standard_Real Radius,
				 TopoDS_Edge& TrimE1, 
				 TopoDS_Edge& TrimE2, 
				 TopoDS_Edge& Fillet) 
{
  TopoDS_Vertex newExtr1, newExtr2;
  Standard_Boolean Degen1, Degen2;
  Fillet = BuildFilletEdge(V, E1, E2, Radius, newExtr1, newExtr2);
  if ( status != ChFi2d_IsDone) return;
  TrimE1 = BuildNewEdge(E1, V, newExtr1, Degen1);
  TrimE2 = BuildNewEdge(E2, V, newExtr2, Degen2);
  if (Degen1 && Degen2 ) status = ChFi2d_BothEdgesDegenerated;
  if (Degen1 && !Degen2 ) status = ChFi2d_FirstEdgeDegenerated;
  if (!Degen1 && Degen2 ) status = ChFi2d_LastEdgeDegenerated;
} // ComputeFillet



//=======================================================================
//function : BuildNewWire
//purpose  : 
//=======================================================================

void ChFi2d_Builder::BuildNewWire (const TopoDS_Edge& OldE1,
				   const TopoDS_Edge& OldE2, 
				   const TopoDS_Edge& E1, 
				   const TopoDS_Edge& Fillet, 
				   const TopoDS_Edge& E2)
{
  
  Standard_Boolean aClosedStatus = Standard_True;

  TopExp_Explorer Ex(refFace, TopAbs_WIRE);
  while (Ex.More()) {
    const TopoDS_Wire& aWire = TopoDS::Wire(Ex.Current());
    aClosedStatus = aWire.Closed();
    break;
  }
  

  Standard_Boolean filletIsAdded = Standard_False;

  Ex.Init(newFace, TopAbs_EDGE);
  TopoDS_Wire newWire;
  BRep_Builder B;
  B.MakeWire(newWire);

  while (Ex.More()) {
    const TopoDS_Edge& theEdge = TopoDS::Edge(Ex.Current());
    if (!theEdge.IsSame(OldE1) && !theEdge.IsSame(OldE2)) {
      B.Add(newWire, theEdge);
    }
    else {
      if (theEdge == OldE1) {
  	if (status != ChFi2d_FirstEdgeDegenerated
            && status != ChFi2d_BothEdgesDegenerated) {
		B.Add(newWire, E1);
        }
  	if ( !filletIsAdded) {
  	  B.Add(newWire, Fillet);
  	  filletIsAdded = Standard_True;
  	} // if ( !filletIsAdded ...
      } // if (theEdge == ...
      else {
  	if (status != ChFi2d_LastEdgeDegenerated
            && status != ChFi2d_BothEdgesDegenerated) {
                B.Add(newWire, E2);
        }
  	if ( !filletIsAdded) {
  	  B.Add(newWire, Fillet);
 	  filletIsAdded = Standard_True;
  	}// if ( !filletIsAdded ...
      } // else ...
    } // else ...
    Ex.Next();
  } // while ...

  newWire.Closed(aClosedStatus);
  BRepAdaptor_Surface Adaptor3dSurface(refFace);
  BRepLib_MakeFace mFace(Adaptor3dSurface.Plane(), newWire);
  newFace = mFace;

} // BuildNewWire


//=======================================================================
//function : BuildNewEdge
//purpose  : 
//=======================================================================

TopoDS_Edge ChFi2d_Builder::BuildNewEdge(const TopoDS_Edge& E1,
				       const TopoDS_Vertex& OldExtr,
				       const TopoDS_Vertex& NewExtr) const 
{
  BRepLib_MakeEdge makeEdge;
  TopLoc_Location loc;
  Standard_Real first, last;
  TopoDS_Vertex firstVertex, lastVertex;
  TopExp::Vertices(E1, firstVertex, lastVertex);
//  syntaxe invalide sur NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(E1, first, last);   
  Handle(Geom_Curve) curve = BRep_Tool::Curve(E1, first, last);   
  if (firstVertex.IsSame(OldExtr)) 
    makeEdge.Init(curve, NewExtr, lastVertex);  
  else 
    makeEdge.Init(curve, firstVertex, NewExtr);
  TopoDS_Edge anEdge = makeEdge;
  anEdge.Orientation(E1.Orientation());
//  anEdge.Location(E1.Location());
  return anEdge;
}


//=======================================================================
//function : BuildNewEdge
//purpose  : special flag if the new edge is degenerated
//=======================================================================

TopoDS_Edge ChFi2d_Builder::BuildNewEdge(const TopoDS_Edge& E1,
				       const TopoDS_Vertex& OldExtr,
				       const TopoDS_Vertex& NewExtr,
				       Standard_Boolean& IsDegenerated) const 
{
  BRepLib_MakeEdge makeEdge;
  TopLoc_Location loc;
  Standard_Real first, last;
  IsDegenerated = Standard_False;
  TopoDS_Vertex firstVertex, lastVertex;
  TopExp::Vertices(E1, firstVertex, lastVertex);
  gp_Pnt Pnew = BRep_Tool::Pnt(NewExtr);
  Standard_Boolean PonctualEdge = Standard_False;
  Standard_Real Tol = Precision::Confusion();
//  syntax wrong on NT
//      const Handle(Geom_Curve)& curve = 
//	BRep_Tool::Curve(E1, first, last);   
  Handle(Geom_Curve) curve = BRep_Tool::Curve(E1, first, last);   
  if (firstVertex.IsSame(OldExtr)) {
    makeEdge.Init(curve, NewExtr, lastVertex);
    gp_Pnt PV = BRep_Tool::Pnt(lastVertex);
    PonctualEdge = (Pnew.Distance(PV)<Tol);
  }
  else {
    makeEdge.Init(curve, firstVertex, NewExtr);
    gp_Pnt PV = BRep_Tool::Pnt(firstVertex);
    PonctualEdge = (Pnew.Distance(PV)<Tol);
  }
  TopoDS_Edge anEdge;
  BRepLib_EdgeError error = makeEdge.Error();
  if (error==BRepLib_LineThroughIdenticPoints || PonctualEdge) {
    IsDegenerated = Standard_True;
    anEdge = E1;
  }
  else {
    anEdge = makeEdge;
    anEdge.Orientation(E1.Orientation());
//    anEdge.Location(E1.Location());
  }
  return anEdge;
}


//=======================================================================
//function : UpDateHistory
//purpose  : 
//=======================================================================
void ChFi2d_Builder::UpDateHistory(const TopoDS_Edge& E1, 
				 const TopoDS_Edge& E2, 
				 const TopoDS_Edge& TrimE1, 
				 const TopoDS_Edge& TrimE2, 
				 const TopoDS_Edge& NewEdge,
				 const Standard_Integer Id)
{
  if (Id == 1) // the new edge is a fillet
  {
    fillets.Append(NewEdge);
  }
  else // the new edge is a chamfer
  {
    chamfers.Append(NewEdge);
  }

  history.UnBind(E1);
  if (status != ChFi2d_FirstEdgeDegenerated
   && status != ChFi2d_BothEdgesDegenerated)
  {
    if (!E1.IsSame(TrimE1))
    {
      history.Bind(E1, TrimE1);
    }
  }
  history.UnBind(E2);
  if (status != ChFi2d_LastEdgeDegenerated
   && status != ChFi2d_BothEdgesDegenerated)
  {
    if (!E2.IsSame(TrimE2))
    {
      history.Bind(E2, TrimE2);
    }
  }
} // UpDateHistory


//=======================================================================
//function : UpDateHistory
//purpose  : 
//=======================================================================
void ChFi2d_Builder::UpDateHistory(const TopoDS_Edge& E1, 
				 const TopoDS_Edge& E2, 
				 const TopoDS_Edge& TrimE1, 
				 const TopoDS_Edge& TrimE2) 
{


  if (history.IsBound(E1)) history.UnBind(E1);
  if (!E1.IsSame(TrimE1)) history.Bind(E1, TrimE1);
  if (history.IsBound(E2)) history.UnBind(E2);
  if (!E2.IsSame(TrimE2)) history.Bind(E2, TrimE2);
} // UpDateHistory


//=======================================================================
//function : BasisEdge
//purpose  : 
//=======================================================================
const TopoDS_Edge& ChFi2d_Builder::BasisEdge(const TopoDS_Edge& E) const 
{
  TopTools_DataMapIteratorOfDataMapOfShapeShape iterator(history);
  TopoDS_Edge anEdge;
  while (iterator.More()) {
    anEdge = TopoDS::Edge(iterator.Value());
    if (anEdge.IsSame(E)) { 
      const TopoDS_Edge& anotherEdge = TopoDS::Edge(iterator.Key());
      return anotherEdge;
    } // if (anEdge.IsSame ...
    iterator.Next();
  } // while (Iterator.More ...
  return E;
} // BasisEdge


//=======================================================================
//function : BuildFilletEdge
//purpose  : 
//=======================================================================
TopoDS_Edge ChFi2d_Builder::BuildFilletEdge(const TopoDS_Vertex& V, 
					  const TopoDS_Edge& AdjEdge1, 
					  const TopoDS_Edge& AdjEdge2, 
					  const Standard_Real Radius,
					  TopoDS_Vertex& NewExtr1,
					  TopoDS_Vertex& NewExtr2)
{
  TopoDS_Edge E1, E2;
  E1 = AdjEdge1;
  E2 = AdjEdge2;
  TopoDS_Vertex V1 = TopExp::FirstVertex(E1);
  TopoDS_Vertex V2 = TopExp::LastVertex(E1);
  TopoDS_Vertex V3 = TopExp::FirstVertex(E2);
  TopoDS_Vertex V4 = TopExp::LastVertex(E2);

  //========================================================================
  //    The first arc is found.                                        +
  //========================================================================

  TopAbs_Orientation O1;
  TopAbs_Orientation OE1;
  OE1 = E1.Orientation();
  E1.Orientation(TopAbs_FORWARD);
  E2.Orientation(TopAbs_FORWARD);
  TopoDS_Shape aLocalShape = E1.EmptyCopied();
  TopoDS_Edge Ebid1 = TopoDS::Edge(aLocalShape);
  aLocalShape = E2.EmptyCopied();
  TopoDS_Edge Ebid2 = TopoDS::Edge(aLocalShape);
//  TopoDS_Edge Ebid1 = TopoDS::Edge(E1.EmptyCopied());
//  TopoDS_Edge Ebid2 = TopoDS::Edge(E2.EmptyCopied());
  Standard_Real param1,param2,param3,param4;
  
  //========================================================================
  //    Save non-modified parts of edges concerned.      +
  //========================================================================

  if (V1.IsSame(V)) {
    param1 = BRep_Tool::Parameter(V1,E1);
    param2 = BRep_Tool::Parameter(V2,E1);
    O1 = V2.Orientation();
  }
  else {
    param1 = BRep_Tool::Parameter(V2,E1);
    param2 = BRep_Tool::Parameter(V1,E1);
    O1 = V1.Orientation();
  }
  if (V3.IsSame(V)) {
    param3 = BRep_Tool::Parameter(V3,E2);
    param4 = BRep_Tool::Parameter(V4,E2);
  }
  else {
    param3 = BRep_Tool::Parameter(V4,E2);
    param4 = BRep_Tool::Parameter(V3,E2);
  }
  
  //========================================================================
  //    Restore geometric supports.                            +
  //========================================================================

  Handle(Geom2d_Curve) C1,C2; 
  Standard_Real ufirst1,ulast1,ufirst2,ulast2,U1,U2,PU1,PU2,Vv1,Vv2;
  Standard_Real PPU1,PPU2;
  C1 = BRep_Tool::CurveOnSurface(E1,newFace,ufirst1,ulast1);
  C2 = BRep_Tool::CurveOnSurface(E2,newFace,ufirst2,ulast2);

  //========================================================================
  //   Determination of the face for fillet.                                +
  //========================================================================

  gp_Pnt2d p;
  gp_Vec2d Ve1,Ve2;
  gp_Vec2d Ve3,Ve4;
  Standard_Boolean Sens1 , Sens2;

  Handle(Geom2d_Curve) basisC1,basisC2; 
  Handle(Geom2d_TrimmedCurve) T1 = Handle(Geom2d_TrimmedCurve)::DownCast(C1);
  if (!T1.IsNull())
    basisC1 = T1->BasisCurve();
  else
    basisC1 = C1;
  Handle(Geom2d_TrimmedCurve) T2 = Handle(Geom2d_TrimmedCurve)::DownCast(C2);
  if (!T2.IsNull())
    basisC2 = T2->BasisCurve();
  else
    basisC2 = C2;

  if (basisC1->DynamicType() == STANDARD_TYPE(Geom2d_Circle)) {
    Handle(Geom2d_Circle) CC1 = Handle(Geom2d_Circle)::DownCast(basisC1);
    ElCLib::D1(param1,CC1->Circ2d(),p,Ve1);
    Sens1 = (CC1->Circ2d()).IsDirect(); 
  } // if (C1->DynamicType() ...
  else {
    Handle(Geom2d_Line) CC1 = Handle(Geom2d_Line)::DownCast(basisC1);
    ElCLib::D1(param1,CC1->Lin2d(),p,Ve1);
    Sens1=Standard_True;
  } // else ...
  if (basisC2->DynamicType() == STANDARD_TYPE(Geom2d_Circle)) {
    Handle(Geom2d_Circle) CC2 = Handle(Geom2d_Circle)::DownCast(basisC2);
    ElCLib::D1(param3,CC2->Circ2d(),p,Ve2);
    Sens2 = (CC2->Circ2d()).IsDirect(); 
  } // if if (C2->DynamicType() ...
  else {
    Handle(Geom2d_Line) CC2 = Handle(Geom2d_Line)::DownCast(basisC2);
    ElCLib::D1(param3,CC2->Lin2d(),p,Ve2);
    Sens2=Standard_True;
  } // else ...

  TopoDS_Edge filletEdge;

  Standard_Real cross = Ve1.Crossed(Ve2);
  Ve3 = Ve1;
  Ve4 = Ve2;

  // processing of tangency or downcast point 
  if (Ve1.IsParallel(Ve2,Precision::Angular())) {
    // Ve1 and Ve2 are parallel : cross at 0
    cross = 0.;
    if (param1<param2) {
      Ve3 = -Ve1;
    }
    if (param3>param4) {
      Ve4 = -Ve2;
    }

    if (! Ve4.IsOpposite(Ve3,Precision::Angular())) {
      // There is a true tangency point and the calculation is stopped
      status = ChFi2d_TangencyError;
      return filletEdge;
    }
      // Otherwise this is a downcast point, and the calculation is continued
  }

  GccEnt_Position Qual1,Qual2;
  if (cross < 0.) {
    if (param3 > param4 ) {
        if(Sens1 == Standard_True){
           Qual1 = GccEnt_enclosed;
	 }
        else {
           Qual1 = GccEnt_outside;
	 }
    }
    else {
        if(Sens1 == Standard_True){
           Qual1 = GccEnt_outside;
	 }
        else {
           Qual1 = GccEnt_enclosed;
        }
    }
    if (param1 > param2) {
        if(Sens2 == Standard_True)
          Qual2 = GccEnt_outside;
        else
           Qual2 = GccEnt_enclosed;
    }
    else {
        if(Sens2 == Standard_True)
           Qual2 = GccEnt_enclosed;
        else
           Qual2 = GccEnt_outside;      
    }
  } // if (cross < 0 ...
  else {
    if (param3 > param4 ) {
        if( Sens1 == Standard_True)
            Qual1 = GccEnt_outside;
        else
            Qual1 = GccEnt_enclosed;
    }
    else {
        if( Sens1 == Standard_True)
            Qual1 = GccEnt_enclosed;
        else
            Qual1 = GccEnt_outside;
    }
    if (param1 > param2 ) {
        if( Sens2 == Standard_True)
             Qual2 = GccEnt_enclosed;
        else
             Qual2 = GccEnt_outside;
    }
    else {
        if( Sens2 == Standard_True)
            Qual2 = GccEnt_outside;
        else
            Qual2 = GccEnt_enclosed;
    }
  } // else ...

  Standard_Real Tol = Precision::Confusion();
  Geom2dGcc_Circ2d2TanRad Fillet(Geom2dGcc_QualifiedCurve(basisC1,Qual1),
				 Geom2dGcc_QualifiedCurve(basisC2,Qual2),
				 Radius, Tol);
  if (!Fillet.IsDone() || Fillet.NbSolutions()==0) {
    status = ChFi2d_ComputationError;
    return filletEdge;
  }
  else if (Fillet.NbSolutions() >= 1) {
    status = ChFi2d_IsDone;
    Standard_Integer numsol = 1;
    Standard_Integer nsol = 1;
    TopoDS_Vertex Vertex1,Vertex2;
    gp_Pnt2d Ptg1,Ptg2;
    Standard_Real dist;
    Standard_Real dist1 = 1.e40;
    Standard_Boolean inside = Standard_False;
    while (nsol<=Fillet.NbSolutions()) {
      Fillet.Tangency1(nsol,PU1,PU2,Ptg1);
      dist = Ptg1.Distance(p);
      if (basisC1->DynamicType() == STANDARD_TYPE(Geom2d_Line)) {
        inside = (PU2<param1 && PU2>param2) || (PU2<param2 && PU2>param1);
	if ( inside && dist < dist1) {
	  numsol = nsol;
	  dist1 = dist;
	} // if ((((inside && ...
      } // if (C1->DynamicType( ...
      else {
	Fillet.Tangency2(nsol,PPU1,PPU2,Ptg2);
        dist = Ptg2.Distance(p);
        inside = (PPU2<param3 && PPU2>param4) || (PPU2<param4 && PPU2>param3);
        //  case of arc of circle passing on the sewing
        if ( ( basisC2->DynamicType() == STANDARD_TYPE(Geom2d_Circle) ) && 
            ( (2*M_PI<param3 && 2*M_PI>param4) || (2*M_PI<param4 && 2*M_PI>param3) ) ) {
        //  cas param3<param4
          inside = (param3<PPU2 && PPU2<2*M_PI) 
                     || (0<=PPU2 && PPU2<param4-2*M_PI);
        //  cas param4<param3
          inside = inside || (param4<PPU2 && PPU2<2*M_PI) 
                               || (0<=PPU2 && PPU2<param3-2*M_PI);
        }
	if ( inside && dist < dist1) {
	  numsol = nsol;
	  dist1 = dist;
	} // if ((((param3 ...
      } // else ...
      nsol++;
    } // while (nsol ...
    gp_Circ2d cir(Fillet.ThisSolution(numsol));
    Handle(Geom2d_Circle) circle = new Geom2d_Circle(cir);

    BRep_Builder B;
    BRepAdaptor_Surface Adaptor3dSurface(refFace);
    Handle(Geom_Plane) refSurf = new Geom_Plane(Adaptor3dSurface.Plane());
    Fillet.Tangency1(numsol,U1,U2,Ptg1);
    Fillet.Tangency2(numsol,Vv1,Vv2,Ptg2);

    // check the validity of parameters
    //// modified by jgv, 08.08.2011 for bug 0022695 ////
    //inside = (U2<param1 && U2>param2) || (U2<param2 && U2>param1);
    inside = (U2 < param1 && U2 >= param2) || (U2 <= param2 && U2 > param1);
    /////////////////////////////////////////////////////
    if ( (basisC1->DynamicType() == STANDARD_TYPE(Geom2d_Circle))
      &&  ( (2*M_PI<param1 && 2*M_PI>param2) || (2*M_PI<param2 && 2*M_PI>param1) ) ) {
      // arc of circle containing the circle origin
      //  case param1<param2
      inside = (param1<U2 && U2<2*M_PI) || (0<=U2 && U2<param2-2*M_PI);
      //  case param2<param1
      inside = inside || (param2<U2 && U2<2*M_PI) || (0<=U2 && U2<param1-2*M_PI);
    }
    if (!inside) {
      status = ChFi2d_ComputationError;
      return filletEdge;
    }

    //// modified by jgv, 08.08.2011 for bug 0022695 ////
    //inside = (Vv2<param3 && Vv2>param4) || (Vv2<param4 && Vv2>param3);
    inside = (Vv2 < param3 && Vv2 >= param4) || (Vv2 <= param4 && Vv2 > param3);
    /////////////////////////////////////////////////////
    if ( (basisC2->DynamicType() == STANDARD_TYPE(Geom2d_Circle))
      &&  ( (2*M_PI<param3 && 2*M_PI>param4) || (2*M_PI<param4 && 2*M_PI>param3) ) ) {
    // arc of circle containing the circle origin
      //  cas param3<param4
      inside = (param3<Vv2 && Vv2<2*M_PI) || (0<=Vv2 && Vv2<param4-2*M_PI);
      //  cas param4<param3
      inside = inside || (param4<Vv2 && Vv2<2*M_PI) || (0<=Vv2 && Vv2<param3-2*M_PI);
    }
    if (!inside) {
      status = ChFi2d_ComputationError;
      return filletEdge;
    }

    gp_Pnt p1 = Adaptor3dSurface.Value(Ptg1.X(), Ptg1.Y());
    gp_Pnt p2 = Adaptor3dSurface.Value(Ptg2.X(), Ptg2.Y());
    B.MakeVertex(Vertex1, p1,Tol);
    NewExtr1 = Vertex1;
    if (Abs(U2-ufirst1) <= Precision::PConfusion()) {
      NewExtr1 = V1;
    }
    if (Abs(U2-ulast1) <= Precision::PConfusion()) {
      NewExtr1 = V2;
    }

    B.MakeVertex(Vertex2, p2,Tol);
    NewExtr2 = Vertex2;
    if (Abs(Vv2-ufirst2) <= Precision::PConfusion()) {
      NewExtr2 = V3;
    }
    if (Abs(Vv2-ulast2) <= Precision::PConfusion()) {
      NewExtr2 = V4;
    }

    //=======================================================================
    //   Update tops of the fillet.                                  +
    //=======================================================================
    gp_Pnt Pntbid;
    gp_Pnt2d sommet;
    Pntbid = BRep_Tool::Pnt(V);
    sommet = gp_Pnt2d(Pntbid.X(),Pntbid.Y());

    gp_Pnt pntBid;
    gp_Pnt2d somBid;
    if (V1.IsSame(V)) {
      pntBid = BRep_Tool::Pnt(V2);
      somBid = gp_Pnt2d(pntBid.X(),pntBid.Y());
    }
    else {
      pntBid = BRep_Tool::Pnt(V1);
      somBid = gp_Pnt2d(pntBid.X(),pntBid.Y());
    }

    gp_Vec2d vec;
    ElCLib::D1(U1,cir,Ptg1,vec);

    gp_Vec2d vec1;
    if (basisC1->DynamicType() == STANDARD_TYPE(Geom2d_Circle)) {
      Handle(Geom2d_Circle) CC1 = Handle(Geom2d_Circle)::DownCast(basisC1);
      gp_Circ2d cir2d(CC1->Circ2d());
      Standard_Real par = ElCLib::Parameter(cir2d,Ptg1);
      gp_Pnt2d Pd;
      ElCLib::D1(par,cir2d,Pd,vec1);
    } // if (C1->DynamicType() ...
    else if (basisC1->DynamicType() == STANDARD_TYPE(Geom2d_Line)) {
      Handle(Geom2d_Line) CC1 = Handle(Geom2d_Line)::DownCast(basisC1);
      gp_Lin2d lin2d(CC1->Lin2d());
      Standard_Real par = ElCLib::Parameter(lin2d,sommet);
      vec1 = gp_Vec2d(sommet.X()-somBid.X(),sommet.Y()-somBid.Y());
      gp_Pnt2d Pd;
      ElCLib::D1(par,lin2d,Pd,vec1);
    } // else if ...

    if (OE1 == TopAbs_REVERSED) {
      vec1.Reverse();
    } // if (OE1 ...
    Standard_Boolean Sense = ( vec1*vec ) > 0.;
    if (U1 > Vv1 && U1 > 2.*M_PI) {
      ElCLib::AdjustPeriodic(0.,2.*M_PI,Precision::Confusion(),U1,Vv1);
    } // if (U1 ... 
    if ( (O1 == TopAbs_FORWARD && OE1 == TopAbs_FORWARD) ||
         (O1 == TopAbs_REVERSED && OE1 == TopAbs_REVERSED) ) {
      filletEdge = BRepLib_MakeEdge(circle, refSurf,
				    NewExtr1, NewExtr2, U1, Vv1);
    } // if (O1 == ...
    else {
      filletEdge = BRepLib_MakeEdge(circle, refSurf, 
				    NewExtr2, NewExtr1, Vv1, U1);
    } // else ...
    if (!Sense) {
      TopAbs_Orientation S1 = filletEdge.Orientation();
      if ((O1 == TopAbs_FORWARD && OE1 == TopAbs_FORWARD) ||
          (O1 == TopAbs_REVERSED && OE1 == TopAbs_REVERSED) ) {
	filletEdge = BRepLib_MakeEdge(circle, refSurf, 
				      NewExtr2, NewExtr1, Vv1, U1);
      }
      else {
	filletEdge = BRepLib_MakeEdge(circle, refSurf, 
				      NewExtr1, NewExtr2, U1, Vv1);
      }
      if (S1 == TopAbs_FORWARD) {
	filletEdge.Orientation(TopAbs_REVERSED);
      }
      else {
	filletEdge.Orientation(TopAbs_FORWARD);
      }
    } // if (!Sense

  } //  else if

  BRepLib::BuildCurves3d(filletEdge);  
  return filletEdge;
} // BuildFilletEdge


//=======================================================================
//function : IsAFillet
//purpose  : 
//=======================================================================

Standard_Boolean ChFi2d_Builder::IsAFillet(const TopoDS_Edge& E) const 
{
  Standard_Integer i = 1;
  while (i <= fillets.Length()) {
    const TopoDS_Edge& currentEdge = TopoDS::Edge(fillets.Value(i));
    if (currentEdge.IsSame(E)) return Standard_True;
    i++;
  }
  return Standard_False;
} // IsAFillet


//=======================================================================
//function : IsAChamfer
//purpose  : 
//=======================================================================

Standard_Boolean ChFi2d_Builder::IsAChamfer(const TopoDS_Edge& E) const 
{
  Standard_Integer i = 1;
  while (i <= chamfers.Length()) {
    const TopoDS_Edge& currentEdge = TopoDS::Edge(chamfers.Value(i));
    if (currentEdge.IsSame(E)) return Standard_True;
    i++;
  }
  return Standard_False;
} // IsAChamfer



//=======================================================================
//function : IsLineOrCircle
//purpose  : 
//=======================================================================

Standard_Boolean IsLineOrCircle(const TopoDS_Edge& E,
				const TopoDS_Face& F) 
{
  Standard_Real first, last;
  TopLoc_Location loc;
//  syntaxe invalide sur NT
//      const Handle(Geom2d_Curve)& C = 
//	BRep_Tool::CurveOnSurface(E,F,first,last);   
  Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(E,F,first,last);
  Handle(Geom2d_Curve) basisC; 
  Handle(Geom2d_TrimmedCurve) TC = Handle(Geom2d_TrimmedCurve)::DownCast(C);
  if (!TC.IsNull())
    basisC = TC->BasisCurve();
  else
    basisC = C;

  if ( basisC->DynamicType() == STANDARD_TYPE(Geom2d_Circle)
      || basisC->DynamicType() == STANDARD_TYPE(Geom2d_Line) ) {
    return Standard_True;
  }
  else {
    return Standard_False;
  } // else ...
} // IsLineOrCircle
