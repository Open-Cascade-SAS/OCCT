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
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>

// LOIinfsup
#define M_ON(st) (st == TopAbs_ON)
#define M_REVERSED(st) (st == TopAbs_REVERSED)

#ifdef OCCT_DEBUG
Standard_EXPORT void debarc(const int i);
Standard_EXPORT void debooarc(const int i);
#endif

Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T, // transition
  const int         S, // curve/edge index
  const int         G, // point/vertex index
  const double            P, // parameter of G on S
  const TopOpeBRepDS_Kind        GK,
  const bool         B); // G is a vertex (or not) of the interference master
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T,  // transition
  const int         S,  // curve/edge index
  const int         G,  // point/vertex index
  const double            P,  // parameter of G on S
  const TopOpeBRepDS_Kind        GK, // POINT/VERTEX
  const TopOpeBRepDS_Kind        SK,
  const bool         B); // G is a vertex (or not) of the interference master

static bool FUN_INlos(const TopoDS_Shape& S, const NCollection_List<TopoDS_Shape>& loS)
{
  NCollection_List<TopoDS_Shape>::Iterator it(loS);
  for (; it.More(); it.Next())
    if (it.Value().IsSame(S))
      return true;
  return false;
}

//=======================================================================
// function : GetEdgeTrans
// purpose  : Computes E<Sind> transition on <F> at point <VP>
//           Computes FORWARD or REVERSED transitions,
//           returns transition UNKNOWN elsewhere.
//=======================================================================
TopOpeBRepDS_Transition TopOpeBRep_FacesFiller::GetEdgeTrans(const TopOpeBRep_VPointInter& VP,
                                                             const TopOpeBRepDS_Kind       PVKind,
                                                             const int        PVIndex,
                                                             const int ShapeIndex,
                                                             const TopoDS_Face&     F)
{
  // VP is on couture <Ec> of rank <sind>
  //       on face <F> of rank <oosind>.
  int OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  int vpsind       = VP.ShapeIndex();
  bool on2edges     = (vpsind == 3);
  bool isvalid      = on2edges || (vpsind == ShapeIndex);
  if (!isvalid)
    throw Standard_Failure("TopOpeBRep_FacesFiller::GetEdgeTrans");

  const TopoDS_Edge& edge    = TopoDS::Edge(VP.Edge(ShapeIndex));
  double      paredge = VP.EdgeParameter(ShapeIndex);

  TopoDS_Edge OOedge;

  double OOparedge = 0.;

  bool hasONedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  bool hasOOedge = (on2edges) ? true : hasONedge;
  if (hasOOedge)
  {
    if (on2edges)
      OOparedge = VP.EdgeParameter(OOShapeIndex);
    else
      OOparedge = VP.EdgeONParameter(OOShapeIndex);
    TopoDS_Shape OOe;
    if (on2edges)
      OOe = VP.Edge(OOShapeIndex);
    else
      OOe = VP.EdgeON(OOShapeIndex);
    OOedge = TopoDS::Edge(OOe);
  }
  gp_Pnt2d OOuv = VP.SurfaceParameters(OOShapeIndex);

  double    par1, par2;
  int SIedgeIndex = (myHDS->HasShape(edge)) ? myHDS->Shape(edge) : 0;
  //  if (SIedgeIndex != 0) FDS_getupperlower(myHDS,SIedgeIndex,paredge,par1,par2);
  if (SIedgeIndex != 0)
  {
    bool isonboundper;
    FDS_LOIinfsup(myHDS->DS(),
                  edge,
                  paredge,
                  PVKind,
                  PVIndex,
                  myHDS->DS().ShapeInterferences(edge),
                  par1,
                  par2,
                  isonboundper);
  }
  else
    FUN_tool_bounds(edge, par1, par2);

  TopOpeBRepDS_Transition T;
  // xpu : 16-01-98
  //       <Tr> relative to 3d <OOface> matter,
  //       we take into account <Tr> / 2d <OOface> only if <edge> is normal to <OOface>
  double    tola     = Precision::Angular() * 1.e+2; // dealing with tolerances
  bool EtgF     = FUN_tool_EtgF(paredge, edge, OOuv, F, tola);
  bool rest     = FUN_INlos(edge, myERL);
  bool isse     = myHDS->DS().IsSectionEdge(edge);
  rest                      = rest || isse;
  bool interf2d = EtgF && hasOOedge && rest;

  double                 factor = 1.e-4;
  TopOpeBRepTool_makeTransition MKT;
  bool              ok = MKT.Initialize(edge, par1, par2, paredge, F, OOuv, factor);
  if (!ok)
    return T;
  bool isT2d = MKT.IsT2d();
  interf2d               = interf2d && isT2d;
  if (interf2d)
    ok = MKT.SetRest(OOedge, OOparedge);
  if (!ok)
    return T;

  TopAbs_State stb, sta;
  ok = MKT.MkTonE(stb, sta);
  if (!ok)
    return T;
  T.Before(stb);
  T.After(sta);
  return T;
}

//=======================================================================
// function : ProcessVPonclosingR
// purpose  : SUPPLYING INTPATCH when <VP> is on closing arc.
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessVPonclosingR(
  const TopOpeBRep_VPointInter& VP,
  //                                const TopoDS_Shape& GFace,
  const TopoDS_Shape&,
  const int         ShapeIndex,
  const TopOpeBRepDS_Transition& transEdge,
  const TopOpeBRepDS_Kind        PVKind,
  const int         PVIndex,
  //				const bool EPIfound,
  const bool,
  //                                const occ::handle<TopOpeBRepDS_Interference>& IEPI)
  const occ::handle<TopOpeBRepDS_Interference>&)
{
  //  bool isvertex = VP.IsVertex(ShapeIndex);
  bool isvertex     = (PVKind == TopOpeBRepDS_VERTEX);
  int absindex     = VP.ShapeIndex(); // 0,1,2,3
  int OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  bool on2edges     = (absindex == 3);
  bool hasONedge    = (VP.State(OOShapeIndex) == TopAbs_ON);
  bool hasOOedge    = (on2edges) ? true : hasONedge;

  TopoDS_Face      Face    = (*this).Face(ShapeIndex);
  TopoDS_Face      OOFace  = (*this).Face(OOShapeIndex);
  int iOOFace = myDS->Shape(OOFace);
  if (iOOFace == 0)
    iOOFace = myDS->AddShape(OOFace, OOShapeIndex);

  // current VPoint is on <edge>
  const TopoDS_Edge& edge = TopoDS::Edge(VP.Edge(ShapeIndex));
  if (!myDS->HasShape(edge))
    myDS->AddShape(edge, ShapeIndex);

  double paredge = VP.EdgeParameter(ShapeIndex);

  // dummy if !<hasOOedge>
  int OOedgeIndex = 0;
  TopoDS_Edge      OOedge;
  if (hasOOedge)
  {
    TopoDS_Shape OOe;
    if (on2edges)
      OOe = VP.Edge(OOShapeIndex);
    else
      OOe = VP.EdgeON(OOShapeIndex);
    OOedge = TopoDS::Edge(OOe);
    if (myDS->HasShape(OOedge))
      OOedgeIndex = myDS->Shape(OOedge);
    else
      OOedgeIndex = myDS->AddShape(OOedge, OOShapeIndex);
  }

  // ===================================================================
  //             --- Edge/(POINT,VERTEX) Interference (EPI) creation ---
  // ===================================================================

  // 1. <edge> enters or outers myF<OOShapeIndex> at <VP>,
  //  transition <transEdge> returned by IntPatch is valid (FORWARD/REVERSED).
  // 2. <edge> is tangent to myF<OOShapeIndex> :
  //  transEdge should be INTERNAL/EXTERNAL.

  bool        Tunk = transEdge.IsUnknown();
  TopOpeBRepDS_Transition transAdd;
  bool        newtransEdge = Tunk;
  if (newtransEdge)
    transAdd = GetEdgeTrans(VP, PVKind, PVIndex, ShapeIndex, OOFace);
  else
    transAdd = transEdge;

  // !!! if the compute of <transAdd> fails, we add transEdge.
  // hasOOedge  : <VP> is ON edge <edge> and ON <OOFace>
  // !hasOOedge : <VP> is ON edge <edge> and IN <OOFace>
  {
    TopOpeBRepDS_Transition T = transAdd;
    T.Index(iOOFace);
    occ::handle<TopOpeBRepDS_Interference> EPI =
      ::MakeEPVInterference(T, iOOFace, PVIndex, paredge, PVKind, TopOpeBRepDS_FACE, isvertex);
    myHDS->StoreInterference(EPI, edge);
  }
  if (hasOOedge)
  {
    TopOpeBRepDS_Transition T = transAdd;
    T.Index(iOOFace);
    occ::handle<TopOpeBRepDS_Interference> EPI =
      ::MakeEPVInterference(T, OOedgeIndex, PVIndex, paredge, PVKind, isvertex);
    myHDS->StoreInterference(EPI, edge);
  }

} // ProcessVPonclosingR
