// Created on: 1999-01-25
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_TOOL.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_REVERSED(O) (O == TopAbs_REVERSED)

static void FDS_sortGb(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                       const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&     LI,
                       NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&           LIGb0,
                       NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&           LIGb1,
                       NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&           LIGbsd)
{
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  LIGb0.Clear();
  LIGb1.Clear();
  LIGbsd.Clear();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>&    I = it.Value();
    occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI =
      occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I);
    if (SSI.IsNull())
    {
      LIGb0.Append(I);
      continue;
    }

    bool gb1 = SSI->GBound();
    int G   = I->Geometry();
    bool hsd = HDS->HasSameDomain(BDS.Shape(G));
    if (hsd)
    {
      LIGbsd.Append(I);
      continue;
    }

    if (gb1)
      LIGb1.Append(I);
    else
      LIGb0.Append(I);
  } // it(LI)
}

//=================================================================================================

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

int TopOpeBRepDS_TOOL::EShareG(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                            const TopoDS_Edge&                         E,
                                            NCollection_List<TopoDS_Shape>&                      lEsd)
{
  lEsd.Clear();
  bool dgE = BRep_Tool::Degenerated(E);
  if (dgE)
  {
    bool hsd = HDS->HasSameDomain(E);
    if (!hsd)
      return 0;
    NCollection_List<TopoDS_Shape>::Iterator itsd(HDS->SameDomain(E));
    for (; itsd.More(); itsd.Next())
      lEsd.Append(itsd.Value());
    return lEsd.Extent();
  }

  const TopOpeBRepDS_DataStructure&      BDS = HDS->DS();
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI  = BDS.ShapeInterferences(E);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        LII;
  FDS_copy(LI, LII);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> L1d;
  int                n1d = FUN_selectTRASHAinterference(LII, TopAbs_EDGE, L1d);
  if (n1d == 0)
    return 0;

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>             mapesd;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1gb0, l1gb1, l1gbsd;
  FDS_sortGb(HDS, L1d, l1gb0, l1gb1, l1gbsd);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it0(l1gb0);
  for (; it0.More(); it0.Next())
    mapesd.Add(BDS.Shape(it0.Value()->Support()));

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(l1gb1);
  for (; it1.More(); it1.Next())
    mapesd.Add(BDS.Shape(it1.Value()->Support()));

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itsd(l1gbsd);
  for (; itsd.More(); itsd.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I   = itsd.Value();
    const TopoDS_Edge&                       Esd = TopoDS::Edge(BDS.Shape(I->Support()));
    bool                         isb = mapesd.Contains(Esd);
    if (isb)
      continue;

    int     G  = I->Geometry();
    const TopoDS_Vertex& vG = TopoDS::Vertex(BDS.Shape(G));
    TopoDS_Vertex        vsd;
    bool     ok = FUN_ds_getoov(vG, BDS, vsd);
    if (!ok)
      continue;
    bool Gb1  = occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I)->GBound();
    TopoDS_Vertex    vE   = Gb1 ? vG : vsd;
    TopoDS_Vertex    vEsd = Gb1 ? vsd : vG;

    int ovE;
    gp_Vec           tgE;
    ok = TopOpeBRepTool_TOOL::TgINSIDE(vE, E, tgE, ovE);
    if (!ok)
      continue;
    int ovEsd;
    gp_Vec           tgEsd;
    ok = TopOpeBRepTool_TOOL::TgINSIDE(vEsd, Esd, tgEsd, ovEsd);
    if (!ok)
      continue;
    bool inE   = (ovE == CLOSING) || (ovE == INTERNAL);
    bool inEsd = (ovEsd == CLOSING) || (ovEsd == INTERNAL);
    if (inE || inEsd)
    {
      mapesd.Add(Esd);
      continue;
    }
    double dot = gp_Dir(tgE).Dot(gp_Dir(tgEsd));
    if (dot > 0.)
      mapesd.Add(Esd);
  }
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itm(mapesd);
  for (; itm.More(); itm.Next())
    lEsd.Append(itm.Key());
  return (lEsd.Extent());
}

//=================================================================================================

bool TopOpeBRepDS_TOOL::ShareG(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                           const int                     i1,
                                           const int                     i2)
{
  const TopoDS_Shape& s1 = HDS->Shape(i1);
  const TopoDS_Shape& s2 = HDS->Shape(i2);

  bool hsd1 = HDS->HasSameDomain(s1);
  if (!hsd1)
    return false;
  NCollection_List<TopoDS_Shape>::Iterator it1(HDS->SameDomain(s1));
  for (; it1.More(); it1.Next())
  {
    bool same = it1.Value().IsSame(s2);
    if (!same)
      continue;
    return true;
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepDS_TOOL::GetEsd(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                           const TopoDS_Shape&                        S,
                                           const int                     ie,
                                           int&                          iesd)
{
  // recall : method ::SameDomain(s) returns an iterator on the list of shapes
  //          sdm to s (ie actually sharing geometric domain with s)
  iesd = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mesdS;
  TopExp_Explorer     ex(S, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape& e  = ex.Current();
    bool    hs = HDS->HasShape(e);
    if (!hs)
      continue;
    bool hsd = HDS->HasSameDomain(e);
    if (!hsd)
      continue;
    mesdS.Add(e);
    //    NCollection_List<TopoDS_Shape>::Iterator itt(HDS->SameDomain(e));
    //    for (; itt.More(); itt.Next()) mesdS.Add(itt.Value());
  }

  NCollection_List<TopoDS_Shape>::Iterator it(HDS->SameDomain(HDS->Shape(ie)));
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& esd = it.Value();
    bool    isb = mesdS.Contains(esd);
    if (!isb)
      continue;
    iesd = HDS->Shape(esd);
    return true;
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepDS_TOOL::ShareSplitON(
  const occ::handle<TopOpeBRepDS_HDataStructure>&            HDS,
  const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MEspON,
  const int                                i1,
  const int                                i2,
  TopoDS_Shape&                                         spON)
{
  spON.Nullify();
  bool shareg = TopOpeBRepDS_TOOL::ShareG(HDS, i1, i2);
  if (!shareg)
    return false;

  const TopoDS_Shape& s1 = HDS->Shape(i1);
  const TopoDS_Shape& s2 = HDS->Shape(i2);

  const TopOpeBRepDS_ListOfShapeOn1State& los1 = MEspON.Find(s1);
  bool                        issp = los1.IsSplit();
  if (!issp)
    return false;
  const NCollection_List<TopoDS_Shape>& lsp1 = los1.ListOnState();
  int            nsp1 = lsp1.Extent();
  if (nsp1 == 0)
    return false;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                mesp1; // map of splits on of <s1>
  NCollection_List<TopoDS_Shape>::Iterator it(lsp1);
  for (; it.More(); it.Next())
    mesp1.Add(it.Value());

  const TopOpeBRepDS_ListOfShapeOn1State& los2  = MEspON.Find(s2);
  bool                        issp2 = los2.IsSplit();
  if (!issp2)
    return false;
  const NCollection_List<TopoDS_Shape>& lsp2 = los2.ListOnState();
  int            nsp2 = lsp2.Extent();
  if (nsp2 == 0)
    return false;

  it.Initialize(lsp2);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& esp = it.Value();
    bool    isb = mesp1.Contains(esp);
    if (!isb)
      continue;
    spON = esp;
    return true;
  }
  return false;
}

//=======================================================================
// function : GetConfig
// purpose  : returns relative geometric config
//=======================================================================
#define SAMEORIENTED (1)
#define DIFFORIENTED (2)

bool TopOpeBRepDS_TOOL::GetConfig(
  const occ::handle<TopOpeBRepDS_HDataStructure>&            HDS,
  const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MEspON,
  const int                                ie,
  const int                                iesd,
  int&                                     config)
{
  config                  = 0;
  bool shareg = TopOpeBRepDS_TOOL::ShareG(HDS, ie, iesd);
  if (!shareg)
    return false;

  const TopoDS_Edge&  e      = TopoDS::Edge(HDS->Shape(ie));
  TopAbs_Orientation  oe     = e.Orientation();
  const TopoDS_Edge&  esd    = TopoDS::Edge(HDS->Shape(iesd));
  TopAbs_Orientation  oesd   = esd.Orientation();
  TopOpeBRepDS_Config conf   = HDS->SameDomainOrientation(e);
  bool    unsh   = (conf == TopOpeBRepDS_UNSHGEOMETRY);
  TopOpeBRepDS_Config confsd = HDS->SameDomainOrientation(esd);
  bool    unshsd = (confsd == TopOpeBRepDS_UNSHGEOMETRY);
  if (!unsh && !unshsd)
  {
    bool sameori = (conf == confsd);
    if (M_REVERSED(oe))
      sameori = !sameori;
    if (M_REVERSED(oesd))
      sameori = !sameori;
    config = sameori ? SAMEORIENTED : DIFFORIENTED;
    return true;
  }

  TopoDS_Shape eON;
  shareg = TopOpeBRepDS_TOOL::ShareSplitON(HDS, MEspON, ie, iesd, eON);
  if (!shareg)
    return false;

  double f, l;
  FUN_tool_bounds(TopoDS::Edge(eON), f, l);
  double    x     = 0.45678;
  double    parON = (1 - x) * f + x * l;
  double    tole  = BRep_Tool::Tolerance(TopoDS::Edge(e));
  double    pare;
  bool ok = FUN_tool_parE(TopoDS::Edge(eON), parON, e, pare, tole);
  if (!ok)
    return false;
  double tolesd = BRep_Tool::Tolerance(TopoDS::Edge(esd));
  double paresd;
  ok = FUN_tool_parE(TopoDS::Edge(eON), parON, esd, paresd, tolesd);
  if (!ok)
    return false;
  bool so;
  ok = FUN_tool_curvesSO(e, pare, esd, paresd, so);
  if (!ok)
    return false;
  config = (so) ? SAMEORIENTED : DIFFORIENTED;
  return true;
}
