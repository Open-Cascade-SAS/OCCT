// Created on: 1997-12-24
// Created by: Prestataire Xuan PHAM PHU
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

#include <BRep_Tool.hxx>
#include <gp_Vec.hxx>
#include <Standard_ProgramError.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Edge3dInterferenceTool.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)

static void FUN_Raise()
{
  throw Standard_ProgramError("Edge3dInterferenceTool");
}

#define POINT (0)
#define VERTEXonref (1)
#define VERTEXonOO (2)
#define VERTEXonOref (3)

// myIsVertex :
// ------------
// POINT :<Eref> interferes with <E> at a point
// <Eref> interferes with <E> at a vertex V,
//   VERTEXonref  : V is on shape of <Eref>
//   VERTEXonOO   : V is on shape of <E>
//   VERTEXonOref : V is on 2 shapes.

// myVonOO : only for VERTEXonOO || VERTEXonOref
// --------

// myP3d : only for POINT || VERTEXonref
// -------

static bool FUN_hasparam(const occ::handle<TopOpeBRepDS_Interference>& I, double& paronE)
{
  // prequesitory : shapes <SIX> -> edge <E>
  // ? <paronE> = parameter of <G> on <E>
  TopOpeBRepDS_Kind GT     = I->GeometryType();
  bool              point  = (GT == TopOpeBRepDS_POINT);
  bool              vertex = (GT == TopOpeBRepDS_VERTEX);
  if (point)
  {
    occ::handle<TopOpeBRepDS_CurvePointInterference> CPI =
      occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I);
    if (CPI.IsNull())
      return false;
    paronE = CPI->Parameter();
  }
  if (vertex)
  {
    occ::handle<TopOpeBRepDS_EdgeVertexInterference> EVI =
      occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I);
    if (EVI.IsNull())
      return false;
    paronE = EVI->Parameter();
  }
  return true;
}

static bool FUN_paronOOE(const TopoDS_Edge&  OOE,
                         const int           IsVertex,
                         const TopoDS_Shape& VonOO,
                         const gp_Pnt&       P3d,
                         double&             paronOOE)
{
  bool ok       = false;
  bool hasVonOO = (IsVertex > 1);
  if (hasVonOO)
    ok = FUN_tool_parVonE(TopoDS::Vertex(VonOO), OOE, paronOOE);
  else
  {
    double dist;
    ok          = FUN_tool_projPonE(P3d, OOE, paronOOE, dist);
    double tol1 = BRep_Tool::Tolerance(OOE);
    double tol2 = tol1 * 1.e3;
    double tol  = tol2;
    if (tol > 1.e-2)
      tol = 1.e-2;
    if (ok)
      ok = (dist <= tol);
  }
  return ok;
}

static bool FUN_keepIonF(const gp_Vec&      tgref,
                         const double&      parE,
                         const TopoDS_Edge& E,
                         const TopoDS_Face& F,
                         const double&      tola)
// returns true if an interference I=(TonF,G=point/vertex,S=<E>)
// is to add to the Edge3dInterferenceTool resolving 3d complex transitions
// on edge E
{
  gp_Vec tmp;
  bool   ok = TopOpeBRepTool_TOOL::TggeomE(parE, E, tmp);
  if (!ok)
    return false;
  gp_Dir tgE  = gp_Dir(tmp);
  double prod = std::abs(tgref.Dot(tgE));
  if (std::abs(1 - prod) < tola)
    return false; // <Eref> & <E> are tangent edges
  gp_Vec dd;
  ok = FUN_tool_nggeomF(parE, E, F, dd);
  gp_Dir ngF(dd);
  if (!ok)
    return false;
  prod = std::abs((tgref ^ tgE).Dot(ngF));
  if (std::abs(1 - prod) < tola)
    return false;
  return true;
}

// ----------------------------------------------------------------------
//             EDGE/FACE interferences reducing :
//
// ----------------------------------------------------------------------

//=================================================================================================

TopOpeBRepDS_Edge3dInterferenceTool::TopOpeBRepDS_Edge3dInterferenceTool()
    : myFaceOriented(0),
      myrefdef(false)
{
}

//=======================================================================
// function : InitPointVertex
// purpose  : Initializes reference data for edge/face complex transition
//=======================================================================
// I = (TonF, G=POINT/VERTEX, S=<E>) interference on <Eref>
// G has parameter <paronEref> on <Eref>, <paronE> on <E>

void TopOpeBRepDS_Edge3dInterferenceTool::InitPointVertex(const int           IsVertex,
                                                          const TopoDS_Shape& VonOO)
{
  myIsVertex = IsVertex;
  if (IsVertex > 1)
    myVonOO = VonOO;
}

//=======================================================================
// function : Init
// purpose  : Initializes reference data for edge/face complex transition
//=======================================================================
// I = (T on <F>, G=POINT/VERTEX, S=<E>) interference on <Eref>
// G has parameter <paronEref> on <Eref>, <paronE> on <E>
// -- <E> is edge of <F> --

void TopOpeBRepDS_Edge3dInterferenceTool::Init(const TopoDS_Shape&                           Eref,
                                               const TopoDS_Shape&                           E,
                                               const TopoDS_Shape&                           F,
                                               const occ::handle<TopOpeBRepDS_Interference>& I)
{
  const TopoDS_Edge& EEref = TopoDS::Edge(Eref);
  const TopoDS_Edge& EE    = TopoDS::Edge(E);
  const TopoDS_Face& FF    = TopoDS::Face(F);
  myrefdef                 = false;

  myTole = Precision::Angular(); // NYI

  double pref = 0.0;
  bool   ok   = ::FUN_hasparam(I, pref);
  if (!ok)
  {
    FUN_Raise();
    return;
  }
  // <myP3d> :
  {
    BRepAdaptor_Curve BC(EEref);
    myP3d = BC.Value(pref);
  }
  gp_Vec tmp;
  ok = TopOpeBRepTool_TOOL::TggeomE(pref, EEref, tmp);
  if (!ok)
  {
    FUN_Raise();
    return;
  }
  gp_Dir tgref(tmp);

  double pOO;
  ok = ::FUN_paronOOE(EE, myIsVertex, myVonOO, myP3d, pOO);
  if (!ok)
  {
    FUN_Raise();
    return;
  }
  ok = TopOpeBRepTool_TOOL::TggeomE(pOO, EE, tmp);
  if (!ok)
  {
    FUN_Raise();
    return;
  }
  gp_Dir tgOO(tmp);

  double dot  = tgref.Dot(tgOO);
  dot         = 1 - std::abs(dot);
  double tola = Precision::Confusion();
  bool   Esdm = (std::abs(dot) < tola);
  if (Esdm)
    return;
  // NYI : il faut rejeter les interf I = (T,G,S=E) / E sdm with Eref

  // <myrefdef> :
  ok = ::FUN_keepIonF(tgref, pOO, EE, FF, myTole);
  if (!ok)
  {
    // <Eref> is tangent to <F>,
    // If the transition is FORWARD or REVERSED, it describes a 2d
    // transition (while crossing <E> on <F>), we do not keep it.
    const TopAbs_Orientation& O    = I->Transition().Orientation(TopAbs_IN);
    bool                      is2d = M_FORWARD(O) || M_REVERSED(O);
    if (is2d)
      return;
  }
  myrefdef = true;

  // <myFaceOriented> :
  myFaceOriented = I->Transition().Index();

  // <myTgtref>
  myTgtref = tgref;

  gp_Dir Norm = tgOO ^ tgref;
  myTool.Reset(tgOO, Norm);
}

//=================================================================================================

// I = (T on <F>, G=POINT/VERTEX, S=<E>) interference on <Eref>
void TopOpeBRepDS_Edge3dInterferenceTool::Add(const TopoDS_Shape&                           Eref,
                                              const TopoDS_Shape&                           E,
                                              const TopoDS_Shape&                           F,
                                              const occ::handle<TopOpeBRepDS_Interference>& I)
{
  if (!myrefdef)
  {
    Init(Eref, E, F, I);
    //    return;
  }

  if (!myrefdef)
    return;

  const TopoDS_Edge& EE = TopoDS::Edge(E);
  const TopoDS_Face& FF = TopoDS::Face(F);

  double pOO;
  bool   ok = ::FUN_paronOOE(EE, myIsVertex, myVonOO, myP3d, pOO);
  if (!ok)
    return;
  gp_Pnt2d uv;
  {
    BRepAdaptor_Curve2d BC2d(EE, FF);
    uv = BC2d.Value(pOO);
  }

  ok = ::FUN_keepIonF(myTgtref, pOO, EE, FF, myTole);
  if (!ok)
  {
    const TopAbs_Orientation& O    = I->Transition().Orientation(TopAbs_IN);
    bool                      is2d = M_FORWARD(O) || M_REVERSED(O);
    if (is2d)
      return;
  }

  TopAbs_Orientation oriloc = I->Transition().Orientation(TopAbs_IN);
  TopAbs_Orientation oritan;
  ok = FUN_tool_orientEinFFORWARD(EE, FF, oritan); // xpu : 30/12/97
  if (!ok)
    return;

  gp_Dir Norm(FUN_tool_nggeomF(uv, FF));
  myTool.Compare(myTole, Norm, oriloc, oritan);
}

//=================================================================================================

void TopOpeBRepDS_Edge3dInterferenceTool::Transition(
  const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  TopOpeBRepDS_Transition& T = I->ChangeTransition();
  I->Support(myFaceOriented);

  TopAbs_State stb = myTool.StateBefore();
  TopAbs_State sta = myTool.StateAfter();
  T.Set(stb, sta);
}
