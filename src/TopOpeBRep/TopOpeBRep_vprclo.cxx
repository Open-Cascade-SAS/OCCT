// Created on: 1995-08-04
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_repvg.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

// LOIinfsup
#define M_ON(st)       (st == TopAbs_ON) 
#define M_REVERSED(st) (st == TopAbs_REVERSED) 

#ifdef OCCT_DEBUG
Standard_EXPORT void debarc(const Standard_Integer i);
Standard_EXPORT void debooarc(const Standard_Integer i);
#endif

Standard_EXPORT Handle(TopOpeBRepDS_Interference) MakeEPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK,
 const Standard_Boolean B); // G is a vertex (or not) of the interference master
Standard_EXPORT Handle(TopOpeBRepDS_Interference) MakeEPVInterference
(const TopOpeBRepDS_Transition& T, // transition
 const Standard_Integer S, // curve/edge index
 const Standard_Integer G, // point/vertex index
 const Standard_Real P, // parameter of G on S
 const TopOpeBRepDS_Kind GK, // POINT/VERTEX
 const TopOpeBRepDS_Kind SK,
 const Standard_Boolean B); // G is a vertex (or not) of the interference master


static Standard_Boolean FUN_INlos(const TopoDS_Shape& S, const TopTools_ListOfShape& loS)
{
  TopTools_ListIteratorOfListOfShape it(loS);
  for (; it.More(); it.Next())
    if (it.Value().IsSame(S)) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetEdgeTrans
//purpose  : Computes E<Sind> transition on <F> at point <VP>
//           Computes FORWARD or REVERSED transitions,
//           returns transition UNKNOWN elsewhere.
//=======================================================================
TopOpeBRepDS_Transition TopOpeBRep_FacesFiller::GetEdgeTrans(const TopOpeBRep_VPointInter& VP,const TopOpeBRepDS_Kind PVKind,
			 const Standard_Integer PVIndex,const Standard_Integer ShapeIndex,const TopoDS_Face& F)
{
  // VP is on couture <Ec> of rank <sind>
  //       on face <F> of rank <oosind>. 
  Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  Standard_Integer vpsind = VP.ShapeIndex();
  Standard_Boolean on2edges = (vpsind == 3);
  Standard_Boolean isvalid = on2edges || (vpsind == ShapeIndex);
  if (!isvalid) throw Standard_Failure("TopOpeBRep_FacesFiller::GetEdgeTrans");
  
  const TopoDS_Edge& edge = TopoDS::Edge(VP.Edge(ShapeIndex));
  Standard_Real paredge = VP.EdgeParameter(ShapeIndex);

  TopoDS_Edge OOedge;

  Standard_Real OOparedge = 0.;

  Standard_Boolean hasONedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  Standard_Boolean hasOOedge = (on2edges) ? Standard_True : hasONedge;
  if ( hasOOedge ) {
    if (on2edges) OOparedge = VP.EdgeParameter(OOShapeIndex);
    else          OOparedge = VP.EdgeONParameter(OOShapeIndex);
    TopoDS_Shape OOe;
    if (on2edges) OOe = VP.Edge(OOShapeIndex);
    else          OOe = VP.EdgeON(OOShapeIndex);
    OOedge = TopoDS::Edge(OOe);
  }  
  gp_Pnt2d OOuv = VP.SurfaceParameters(OOShapeIndex);
  
  Standard_Real par1,par2; 
  Standard_Integer SIedgeIndex = (myHDS->HasShape(edge)) ? myHDS->Shape(edge) : 0;
//  if (SIedgeIndex != 0) FDS_getupperlower(myHDS,SIedgeIndex,paredge,par1,par2);
  if (SIedgeIndex != 0) {
    Standard_Boolean isonboundper;
    FDS_LOIinfsup(myHDS->DS(),edge,paredge,PVKind,PVIndex,
		    myHDS->DS().ShapeInterferences(edge),par1,par2,isonboundper);
  }
  else               
    FUN_tool_bounds(edge,par1,par2);

  TopOpeBRepDS_Transition T;
  // xpu : 16-01-98 
  //       <Tr> relative to 3d <OOface> matter,
  //       we take into account <Tr> / 2d <OOface> only if <edge> is normal to <OOface>  
  Standard_Real tola = Precision::Angular()*1.e+2; //dealing with tolerances
  Standard_Boolean EtgF = FUN_tool_EtgF(paredge,edge,OOuv,F,tola);  
  Standard_Boolean rest = FUN_INlos(edge,myERL);
  Standard_Boolean isse = myHDS->DS().IsSectionEdge(edge);
  rest = rest || isse;
  Standard_Boolean interf2d = EtgF && hasOOedge && rest;  
  
  Standard_Real factor = 1.e-4; 
  TopOpeBRepTool_makeTransition MKT; 
  Standard_Boolean ok = MKT.Initialize(edge,par1,par2,paredge, F,OOuv, factor);  
  if (!ok) return T; 
  Standard_Boolean isT2d = MKT.IsT2d();
  interf2d = interf2d && isT2d;
  if (interf2d) ok = MKT.SetRest(OOedge,OOparedge);
  if (!ok) return T;

  TopAbs_State stb,sta; ok = MKT.MkTonE(stb,sta);  
  if (!ok) return T; 
  T.Before(stb); T.After(sta);
  return T;
}

//=======================================================================
//function : ProcessVPonclosingR
//purpose  : SUPPLYING INTPATCH when <VP> is on closing arc.
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessVPonclosingR(const TopOpeBRep_VPointInter& VP,
//                                const TopoDS_Shape& GFace,
                                const TopoDS_Shape& ,
                                const Standard_Integer ShapeIndex,
				const TopOpeBRepDS_Transition& transEdge,const TopOpeBRepDS_Kind PVKind, const Standard_Integer PVIndex,
//				const Standard_Boolean EPIfound,
				const Standard_Boolean ,
//                                const Handle(TopOpeBRepDS_Interference)& IEPI)
                                const Handle(TopOpeBRepDS_Interference)& )
{
//  Standard_Boolean isvertex = VP.IsVertex(ShapeIndex);
  Standard_Boolean isvertex = (PVKind == TopOpeBRepDS_VERTEX);
  Standard_Integer absindex = VP.ShapeIndex(); // 0,1,2,3
  Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  Standard_Boolean on2edges = (absindex == 3);
  Standard_Boolean hasONedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  Standard_Boolean hasOOedge = (on2edges) ? Standard_True : hasONedge;

  TopoDS_Face Face = (*this).Face(ShapeIndex);
  TopoDS_Face OOFace = (*this).Face(OOShapeIndex);
  Standard_Integer iOOFace = myDS->Shape(OOFace);
  if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);

  // current VPoint is on <edge>
  const TopoDS_Edge& edge = TopoDS::Edge(VP.Edge(ShapeIndex));
  if (!myDS->HasShape(edge)) myDS->AddShape(edge,ShapeIndex);

  Standard_Real paredge = VP.EdgeParameter(ShapeIndex);
  
  // dummy if !<hasOOedge>
  Standard_Integer OOedgeIndex = 0; 
  TopoDS_Edge OOedge;
  if ( hasOOedge ) {
    TopoDS_Shape OOe;
    if (on2edges) OOe = VP.Edge(OOShapeIndex);
    else          OOe = VP.EdgeON(OOShapeIndex);
    OOedge = TopoDS::Edge(OOe);
    if (myDS->HasShape(OOedge)) OOedgeIndex = myDS->Shape(OOedge);
    else                        OOedgeIndex = myDS->AddShape(OOedge,OOShapeIndex);
  }
  
  // ===================================================================
  //             --- Edge/(POINT,VERTEX) Interference (EPI) creation ---
  // ===================================================================
  
  // 1. <edge> enters or outers myF<OOShapeIndex> at <VP>,
  //  transition <transEdge> returned by IntPatch is valid (FORWARD/REVERSED).
  // 2. <edge> is tangent to myF<OOShapeIndex> : 
  //  transEdge should be INTERNAL/EXTERNAL.
  
  Standard_Boolean Tunk = transEdge.IsUnknown();
  TopOpeBRepDS_Transition transAdd;
  Standard_Boolean newtransEdge = Tunk;
  if (newtransEdge) transAdd = GetEdgeTrans(VP,PVKind,PVIndex,ShapeIndex,OOFace);
  else              transAdd = transEdge;

  // !!! if the compute of <transAdd> fails, we add transEdge.
  // hasOOedge  : <VP> is ON edge <edge> and ON <OOFace>
  // !hasOOedge : <VP> is ON edge <edge> and IN <OOFace>  
  {      
    TopOpeBRepDS_Transition T = transAdd; T.Index(iOOFace);
    Handle(TopOpeBRepDS_Interference) EPI = ::MakeEPVInterference
      (T,iOOFace,PVIndex,paredge,PVKind,TopOpeBRepDS_FACE,isvertex);
    myHDS->StoreInterference(EPI,edge);
  }  
  if (hasOOedge) {
    TopOpeBRepDS_Transition T = transAdd; T.Index(iOOFace);
    Handle(TopOpeBRepDS_Interference) EPI = ::MakeEPVInterference
	(T,OOedgeIndex,PVIndex,paredge,PVKind,isvertex);
    myHDS->StoreInterference(EPI,edge);
  }

} // ProcessVPonclosingR
