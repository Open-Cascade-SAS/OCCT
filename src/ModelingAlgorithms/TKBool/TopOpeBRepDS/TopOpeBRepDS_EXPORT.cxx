// Created on: 1997-12-15
// Created by: Jean Yves LEBEY
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

// Modified by xpu, Wed May 20 10:47:38 1998

#include <gp_Pnt2d.hxx>
#include <TopoDS.hxx>
#include <BRepTools.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>

#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>
#include <Standard_ProgramError.hxx>

#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopOpeBRepDS_TOOL.hxx>
#include <TopOpeBRepDS_define.hxx>

#define M_IN(st) (st == TopAbs_IN)
#define M_ON(st) (st == TopAbs_ON)
#define M_OUT(st) (st == TopAbs_OUT)
#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_UNKNOWN(st) (st == TopAbs_UNKNOWN)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

//------------------------------------------------------
Standard_EXPORT void FDS_SetT(TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Transition& T0)
//------------------------------------------------------
{
  TopAbs_State stb = T0.Before(), sta = T0.After();
  T.Before(stb);
  T.After(sta);
}

//------------------------------------------------------
Standard_EXPORT bool FDS_hasUNK(const TopOpeBRepDS_Transition& T)
//------------------------------------------------------
{
  TopAbs_State stb = T.Before(), sta = T.After();
  bool         hasunknown = (M_UNKNOWN(stb) || M_UNKNOWN(sta));
  return hasunknown;
}

//------------------------------------------------------
Standard_EXPORT void FDS_copy(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                              NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       LII)
//------------------------------------------------------
{
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
       it.Next())
    LII.Append(it.Value());
}

//------------------------------------------------------
Standard_EXPORT void FDS_copy(const NCollection_List<TopoDS_Shape>& LI,
                              NCollection_List<TopoDS_Shape>&       LII)
//------------------------------------------------------
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(LI); it.More(); it.Next())
    LII.Append(it.Value());
}
//------------------------------------------------------
Standard_EXPORT void FDS_assign(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                                NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       LII)
//------------------------------------------------------
{
  LII.Clear();
  FDS_copy(LI, LII);
}

//------------------------------------------------------
Standard_EXPORT void FDS_assign(const NCollection_List<TopoDS_Shape>& LI,
                                NCollection_List<TopoDS_Shape>&       LII)
//------------------------------------------------------
{
  LII.Clear();
  FDS_copy(LI, LII);
}

//------------------------------------------------------
Standard_EXPORT void FUN_ds_samRk(const TopOpeBRepDS_DataStructure& BDS,
                                  const int                         Rk,
                                  NCollection_List<TopoDS_Shape>&   LI,
                                  NCollection_List<TopoDS_Shape>&   LIsrk)
//------------------------------------------------------
{
  LIsrk.Clear();
  NCollection_List<TopoDS_Shape>::Iterator it(LI);
  while (it.More())
  {
    const TopoDS_Shape& S  = it.Value();
    int                 rk = BDS.AncestorRank(S);
    if (rk == Rk)
    {
      LIsrk.Append(S);
      LI.Remove(it);
    }
    else
      it.Next();
  }
}

//------------------------------------------------------
Standard_EXPORT void FDS_Tdata
  //------------------------------------------------------
  (const occ::handle<TopOpeBRepDS_Interference>& I,
   TopAbs_ShapeEnum&                             SB,
   int&                                          IB,
   TopAbs_ShapeEnum&                             SA,
   int&                                          IA)
{
  if (I.IsNull())
    return;
  const TopOpeBRepDS_Transition T = I->Transition();
  SB                              = T.ShapeBefore();
  IB                              = T.IndexBefore();
  SA                              = T.ShapeAfter();
  IA                              = T.IndexAfter();
}

//------------------------------------------------------
Standard_EXPORT void FDS_data
  //------------------------------------------------------
  (const occ::handle<TopOpeBRepDS_Interference>& I,
   TopOpeBRepDS_Kind&                            GT1,
   int&                                          G1,
   TopOpeBRepDS_Kind&                            ST1,
   int&                                          S1)
{
  if (I.IsNull())
    return;
  GT1 = I->GeometryType();
  G1  = I->Geometry();
  ST1 = I->SupportType();
  S1  = I->Support();
}

// true si la transition de l'interference courante es UNKNOWN
//------------------------------------------------------
Standard_EXPORT bool FDS_data
  //------------------------------------------------------
  (const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& it1,
   occ::handle<TopOpeBRepDS_Interference>&                                   I1,
   TopOpeBRepDS_Kind&                                                        GT1,
   int&                                                                      G1,
   TopOpeBRepDS_Kind&                                                        ST1,
   int&                                                                      S1)
{
  if (!it1.More())
    return false;
  I1                                = it1.Value();
  const TopOpeBRepDS_Transition& T1 = I1->Transition();
  FDS_data(I1, GT1, G1, ST1, S1);
  bool isunk1 = T1.IsUnknown();
  return isunk1;
}

//------------------------------------------------------
Standard_EXPORT void FDS_Idata
  //------------------------------------------------------
  (const occ::handle<TopOpeBRepDS_Interference>& I,
   TopAbs_ShapeEnum&                             SB,
   int&                                          IB,
   TopAbs_ShapeEnum&                             SA,
   int&                                          IA,
   TopOpeBRepDS_Kind&                            GT1,
   int&                                          G1,
   TopOpeBRepDS_Kind&                            ST1,
   int&                                          S1)
{
  if (I.IsNull())
    return;
  FDS_Tdata(I, SB, IB, SA, IA);
  FDS_data(I, GT1, G1, ST1, S1);
}

//------------------------------------------------------
Standard_EXPORT bool FUN_ds_getVsdm(const TopOpeBRepDS_DataStructure& BDS, const int iV, int& iVsdm)
//------------------------------------------------------
{
  iVsdm      = 0;
  bool found = false;
  // getting <Vsdm> shape same domain with <V>.
  int  imax  = BDS.NbShapes();
  bool undef = (iV < 1) || (iV > imax);
  if (undef)
    return false;
  const TopoDS_Shape&                      V = BDS.Shape(iV);
  NCollection_List<TopoDS_Shape>::Iterator issdm(BDS.ShapeSameDomain(V));
  for (; issdm.More(); issdm.Next())
  {
    const TopoDS_Shape& VV = issdm.Value();
    if (V.IsSame(VV))
      continue;
    iVsdm = BDS.Shape(VV);
    found = true;
    break;
  }
  return found;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_ds_sdm(const TopOpeBRepDS_DataStructure& BDS,
                                const TopoDS_Shape&               s1,
                                const TopoDS_Shape&               s2)
//------------------------------------------------------
{
  if (!BDS.HasShape(s1) || !BDS.HasShape(s2))
    return false;
  const NCollection_List<TopoDS_Shape>&    sdm1 = BDS.ShapeSameDomain(s1);
  NCollection_List<TopoDS_Shape>::Iterator it1(sdm1);
  for (; it1.More(); it1.Next())
    if (it1.Value().IsSame(s2))
      return true;
  return false;
}

// True si les faces isb1,isb2, issues du meme shape origine, connexes par l'arete SI, sont
// tangentes. 1/ analyse du codage SameDomain dans la DS. 2/ si pas de codage SameDomain dans la DS,
// analyse geometrique.
//    cette deuxieme analyse est necessaire car l'info SameDomain n'est pas recherchee entre les
//    faces d'un meme shape origine.
//------------------------------------------------------------------------------------
Standard_EXPORT bool FDS_aresamdom(const TopOpeBRepDS_DataStructure& BDS,
                                   const TopoDS_Shape&               ES,
                                   const TopoDS_Shape&               F1,
                                   const TopoDS_Shape&               F2)
//------------------------------------------------------------------------------------
{
  bool                                     trfa_samdom = false;
  const NCollection_List<TopoDS_Shape>&    ls          = BDS.ShapeSameDomain(F1);
  NCollection_List<TopoDS_Shape>::Iterator it(ls);
  for (; it.More(); it.Next())
  { // NYI : syntaxe rapide de BDS IsShapeSameDomainofShape
    const TopoDS_Shape& ss = it.Value();
    trfa_samdom            = ss.IsSame(F2);
    if (trfa_samdom)
      break;
  }
  if (!trfa_samdom)
  {
    occ::handle<Geom_Surface> su1 = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F1));
    GeomAdaptor_Surface       gs1(su1);
    GeomAbs_SurfaceType       st1 = gs1.GetType();
    occ::handle<Geom_Surface> su2 = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F2));
    GeomAdaptor_Surface       gs2(su2);
    GeomAbs_SurfaceType       st2  = gs2.GetType();
    bool                      plpl = (st1 == GeomAbs_Plane) && (st2 == GeomAbs_Plane);
    if (plpl)
    {
      // NYI a arranger
      gp_Pnt2d p2d1, p2d2;
      BRep_Tool::UVPoints(TopoDS::Edge(ES), TopoDS::Face(F1), p2d1, p2d2);
      gp_Dir d1   = FUN_tool_ngS(p2d1, su1);
      gp_Dir d2   = FUN_tool_ngS(p2d2, su2);
      double tola = Precision::Angular();
      double dot  = d1.Dot(d2);
      trfa_samdom = (std::abs(1. - std::abs(dot)) < tola);
    }
  }
  return trfa_samdom;
}

// True si les faces isb1,isb2, issues du meme shape origine, connexes par l'arete SI, sont
// tangentes. 1/ analyse du codage SameDomain dans la DS. 2/ si pas de codage SameDomain dans la DS,
// analyse geometrique.
//    cette deuxieme analyse est necessaire car l'info SameDomain n'est pas recherchee entre les
//    faces d'un meme shape origine.
//------------------------------------------------------------------------------------
Standard_EXPORT bool FDS_aresamdom(const TopOpeBRepDS_DataStructure& BDS,
                                   const int                         SI,
                                   const int                         isb1,
                                   const int                         isb2)
//------------------------------------------------------------------------------------
{
  bool                trfa_samdom = false;
  const TopoDS_Shape& ES          = BDS.Shape(SI); // edge de F1 et F2
  if (ES.ShapeType() != TopAbs_EDGE)
    return false;
  const TopoDS_Shape& F1 = BDS.Shape(isb1);
  if (F1.ShapeType() != TopAbs_FACE)
    return false;
  const TopoDS_Shape& F2 = BDS.Shape(isb2);
  if (F2.ShapeType() != TopAbs_FACE)
    return false;
  trfa_samdom = FDS_aresamdom(BDS, ES, F1, F2);
  return trfa_samdom;
}

//----------------------------------------------------
Standard_EXPORT bool FDS_EdgeIsConnexToSameDomainFaces(
  const TopoDS_Shape&                             E,
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS) // not used
//----------------------------------------------------
{
  const TopOpeBRepDS_DataStructure&     BDS = HDS->DS();
  const NCollection_List<TopoDS_Shape>& lf  = FDSCNX_EdgeConnexitySameShape(E, HDS);
  int                                   nlf = lf.Extent();
  if (nlf < 2)
    return false;

  bool                                     samdom = false;
  NCollection_List<TopoDS_Shape>::Iterator i1(lf);
  for (; i1.More(); i1.Next())
  {
    const TopoDS_Shape&                      f1 = i1.Value();
    NCollection_List<TopoDS_Shape>::Iterator i2(i1);
    for (; i2.More(); i2.Next())
    {
      const TopoDS_Shape& f2 = i2.Value();
      samdom                 = FDS_aresamdom(BDS, E, f1, f2);
      if (samdom)
        break;
    }
    if (samdom)
      break;
  }
  if (samdom)
    return true;
  return false;
} // not used

// T si ShapeIndex SI est la GeometrieIndex d'une Interference
// d'un Shape (Before ou After) de la Transition de I
//----------------------------------------------------
Standard_EXPORT bool FDS_SIisGIofIofSBAofTofI(const TopOpeBRepDS_DataStructure&             BDS,
                                              const int                                     SI,
                                              const occ::handle<TopOpeBRepDS_Interference>& I)
//----------------------------------------------------
{
  if (SI == 0)
    return false;
  if (I.IsNull())
    return false;
  bool ya = false;

  TopAbs_ShapeEnum  SB1 = TopAbs_SHAPE, SA1 = TopAbs_SHAPE;
  int               IB1 = 0, IA1 = 0;
  TopOpeBRepDS_Kind GT1 = TopOpeBRepDS_UNKNOWN, ST1 = TopOpeBRepDS_UNKNOWN;
  int               G1 = 0, S1 = 0;
  FDS_Idata(I, SB1, IB1, SA1, IA1, GT1, G1, ST1, S1);

  if (SB1 == TopAbs_FACE)
  {
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& Bloi =
      BDS.ShapeInterferences(IB1);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(Bloi);
    for (; it.More(); it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& IB  = it.Value();
      TopAbs_ShapeEnum                              SBB = TopAbs_SHAPE, SAB = TopAbs_SHAPE;
      int                                           IBB = 0, IAB = 0;
      TopOpeBRepDS_Kind GTB = TopOpeBRepDS_UNKNOWN, STB = TopOpeBRepDS_UNKNOWN;
      int               GB = 0, SB = 0;
      FDS_Idata(IB, SBB, IBB, SAB, IAB, GTB, GB, STB, SB);
      if (GTB == TopOpeBRepDS_EDGE && GB == SI)
      {
        // la face IB1 a une interference dont la geometrie est l'arete SI.
        ya = true;
        break;
      }
    }
  } // SB1 == FACE
  else if (SA1 == TopAbs_FACE)
  {
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& Aloi =
      BDS.ShapeInterferences(IA1);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(Aloi);
    for (; it.More(); it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& IA  = it.Value();
      TopAbs_ShapeEnum                              SBA = TopAbs_SHAPE, SAA = TopAbs_SHAPE;
      int                                           IBA = 0, IAA = 0;
      TopOpeBRepDS_Kind GTA = TopOpeBRepDS_UNKNOWN, STA = TopOpeBRepDS_UNKNOWN;
      int               GA = 0, SA = 0;
      FDS_Idata(IA, SBA, IBA, SAA, IAA, GTA, GA, STA, SA);
      if (GTA == TopOpeBRepDS_EDGE && GA == SI)
      {
        // la face IA1 a une interference dont la geometrie est l'arete IS.
        ya = true;
        break;
      }
    }
  } // SA1 == FACE

  return ya;
}

//---------------------------------------------------------
Standard_EXPORT bool FDS_Parameter(const occ::handle<TopOpeBRepDS_Interference>& I, double& par)
//---------------------------------------------------------
{
  bool isEVI = I->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference));
  bool isCPI = I->IsKind(STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference));
  if (!isEVI && !isCPI)
    return false;
  par = FDS_Parameter(I);
  return true;
}

//----------------------------------------------------
Standard_EXPORT double FDS_Parameter(const occ::handle<TopOpeBRepDS_Interference>& I)
//----------------------------------------------------
{
  double p = 0;
  if (I->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference)))
    p = occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->Parameter();
  else if (I->IsKind(STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference)))
    p = occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I)->Parameter();
  else
  {
    throw Standard_Failure("FDS_Parameter");
  }
  return p;
}

//----------------------------------------------------------------------
Standard_EXPORT bool FDS_HasSameDomain3d(const TopOpeBRepDS_DataStructure& BDS,
                                         const TopoDS_Shape&               E,
                                         NCollection_List<TopoDS_Shape>*   PLSD)
//-----------------------------------------------------------------------
{
  const NCollection_List<TopoDS_Shape>& lssd = BDS.ShapeSameDomain(E);
  bool                                  hsd  = (!lssd.IsEmpty());
  if (PLSD != nullptr)
    PLSD->Clear();
  if (!hsd)
    return false;

  bool                                     hsd3d = false;
  NCollection_List<TopoDS_Shape>::Iterator it(lssd);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& esd = it.Value();
    TopOpeBRepDS_Config c   = BDS.SameDomainOri(esd);
    bool                ok  = true;
    ok                      = ok && (c == TopOpeBRepDS_UNSHGEOMETRY);
    if (ok)
    {
      hsd3d = true;
      if (PLSD != nullptr)
        PLSD->Append(esd);
      else
        break;
    }
  }
  return hsd3d;
}

//-----------------------------------------------------------------------
Standard_EXPORT bool FDS_Config3d(const TopoDS_Shape&  E1,
                                  const TopoDS_Shape&  E2,
                                  TopOpeBRepDS_Config& c)
//-----------------------------------------------------------------------
{
  bool same = true;

  gp_Pnt PE1;
  double pE1;
  bool   ok1 = FUN_tool_findPinE(TopoDS::Edge(E1), PE1, pE1);
  gp_Vec VE1;
  if (ok1)
    ok1 = TopOpeBRepTool_TOOL::TggeomE(pE1, TopoDS::Edge(E1), VE1);

  double pE2, dE2;
  bool   ok2 = FUN_tool_projPonE(PE1, TopoDS::Edge(E2), pE2, dE2);
  gp_Vec VE2;
  if (ok2)
    ok2 = TopOpeBRepTool_TOOL::TggeomE(pE2, TopoDS::Edge(E2), VE2);

  if (!ok1 || !ok2)
    return false;

  gp_Dir DE1(VE1);
  gp_Dir DE2(VE2);
  double dot = DE1.Dot(DE2);
  same       = (dot > 0);
  c          = (same) ? TopOpeBRepDS_SAMEORIENTED : TopOpeBRepDS_DIFFORIENTED;
  return true;
}

//----------------------------------------------------------------------
Standard_EXPORT bool FDS_HasSameDomain2d(const TopOpeBRepDS_DataStructure& BDS,
                                         const TopoDS_Shape&               E,
                                         NCollection_List<TopoDS_Shape>*   PLSD)
//-----------------------------------------------------------------------
{
  const NCollection_List<TopoDS_Shape>& lssd = BDS.ShapeSameDomain(E);
  bool                                  hsd  = (!lssd.IsEmpty());
  if (PLSD != nullptr)
    PLSD->Clear();
  if (!hsd)
    return false;

  bool                                     hsd2d = false;
  NCollection_List<TopoDS_Shape>::Iterator it(lssd);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& esd = it.Value();
    TopOpeBRepDS_Config c   = BDS.SameDomainOri(esd);
    bool                ok  = (c == TopOpeBRepDS_SAMEORIENTED || c == TopOpeBRepDS_DIFFORIENTED);
    if (ok)
    {
      hsd2d = true;
      if (PLSD != nullptr)
        PLSD->Append(esd);
      else
        break;
    }
  }
  return hsd2d;
}

//-----------------------------------------------------------------------
Standard_EXPORT void FDS_getupperlower(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                       const int                                       edgeIndex,
                                       const double                                    paredge,
                                       double&                                         p1,
                                       double&                                         p2)
//-----------------------------------------------------------------------
{
  TopoDS_Edge E = TopoDS::Edge(HDS->Shape(edgeIndex));
  FUN_tool_bounds(E, p1, p2);

  // get p1, p2, p1<paredge<p2 with pmin, pmax nearest parameters
  double                     par;
  TopOpeBRepDS_PointIterator pIte = HDS->EdgePoints(E);
  for (; pIte.More(); pIte.Next())
  {
    par          = pIte.Parameter();
    bool parsup1 = (par > p1), parinfe = (par < paredge);
    bool parinf2 = (par < p2), parsupe = (par > paredge);
    if (parsup1 && parinfe)
      p1 = par;
    if (parinf2 && parsupe)
      p2 = par;
  }
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_ds_getoov(const TopoDS_Shape&               v,
                                   const TopOpeBRepDS_DataStructure& BDS,
                                   TopoDS_Shape&                     oov)
//-----------------------------------------------------------------------
{
  // prequesitory : the DS binds at most 2 vertices same domain
  TopoDS_Shape nullS;
  oov                                          = nullS;
  const NCollection_List<TopoDS_Shape>&    vsd = BDS.ShapeSameDomain(v);
  NCollection_List<TopoDS_Shape>::Iterator itlov(vsd);
  for (; itlov.More(); itlov.Next())
  {
    const TopoDS_Shape& vcur = itlov.Value();
    if (vcur.IsSame(v))
      continue;
    oov = vcur;
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_ds_getoov(const TopoDS_Shape&                             v,
                                   const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                   TopoDS_Shape&                                   oov)
//-----------------------------------------------------------------------
{
  // prequesitory : the DS binds at most 2 vertices same domain
  TopoDS_Shape nullS;
  oov = nullS;
  if (HDS->HasSameDomain(v))
  {
    NCollection_List<TopoDS_Shape>::Iterator itlov(HDS->SameDomain(v));
    for (; itlov.More(); itlov.Next())
    {
      const TopoDS_Shape& vcur = itlov.Value();
      if (vcur.IsSame(v))
        continue;
      oov = vcur;
      return true;
    }
  }
  return false;
}

Standard_EXPORT bool FUN_selectTRAINTinterference(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& li,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       liINTERNAL)
{
  liINTERNAL.Clear();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(li);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I   = it.Value();
    const TopAbs_Orientation                      ori = I->Transition().Orientation(TopAbs_IN);
    if (M_INTERNAL(ori))
      liINTERNAL.Append(I);
  }
  bool hasINT = !liINTERNAL.IsEmpty();
  return hasINT;
}

static bool FUN_ds_hasSDMancestorfaces(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                       const TopoDS_Edge&                              E1,
                                       const TopoDS_Edge&                              E2,
                                       TopoDS_Face&                                    Fsd1,
                                       TopoDS_Face&                                    Fsd2)
{
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();

  const NCollection_List<TopoDS_Shape>& lFcE1 = FDSCNX_EdgeConnexitySameShape(E1, HDS);
  const NCollection_List<TopoDS_Shape>& lFcE2 = FDSCNX_EdgeConnexitySameShape(E2, HDS);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapfcE2;
  NCollection_List<TopoDS_Shape>::Iterator                      it2(lFcE2);
  for (; it2.More(); it2.Next())
    mapfcE2.Add(it2.Value());

  NCollection_List<TopoDS_Shape>::Iterator it1(lFcE1);
  for (; it1.More(); it1.Next())
  {
    const TopoDS_Shape& fcE1  = it1.Value();
    bool                hsdm1 = HDS->HasSameDomain(fcE1);
    if (!hsdm1)
      continue;
    const NCollection_List<TopoDS_Shape>& fsdm2 = BDS.ShapeSameDomain(fcE1);
    it2.Initialize(fsdm2);
    for (; it2.More(); it2.Next())
    {
      const TopoDS_Shape& f2   = it2.Value();
      bool                isb2 = mapfcE2.Contains(f2);
      if (!isb2)
        continue;
      Fsd1 = TopoDS::Face(fcE1);
      Fsd2 = TopoDS::Face(f2);
      return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_PURGEforE9(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
{
  // xpu040998 : line gives myLineINL=true, interference on geometry
  //             edge/face describing contact on geometry -> EPIe + EPIf = 3dI
  //             EPIf is faulty if edge is in face's geometry
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         ns  = BDS.NbShapes();
  for (int i = 1; i <= ns; i++)
  {
    const TopoDS_Shape& EE = BDS.Shape(i);
    if (EE.ShapeType() != TopAbs_EDGE)
      continue;

    const TopoDS_Edge& E     = TopoDS::Edge(EE);
    bool               isdgE = BRep_Tool::Degenerated(E);
    if (isdgE)
      continue;

    int                                                             IE = BDS.Shape(E);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(E);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        LIcopy;
    FDS_assign(LI, LIcopy);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l3dF;
    int n3dF = FUN_selectSKinterference(LIcopy, TopOpeBRepDS_FACE, l3dF);
    if (n3dF == 0)
      continue;

    const NCollection_List<TopoDS_Shape>& lfcxE  = FDSCNX_EdgeConnexitySameShape(E, HDS);
    int                                   nlfcxE = lfcxE.Extent();
    if (nlfcxE == 0)
      continue; // NYIRaise
    NCollection_List<TopoDS_Shape>::Iterator                      itf(lfcxE);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapf;
    for (; itf.More(); itf.Next())
      mapf.Add(itf.Value());

    bool                                                               removed = false;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(l3dF);
    while (it.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I  = it.Value();
      TopAbs_ShapeEnum                              SB = TopAbs_SHAPE, SA = TopAbs_SHAPE;
      int                                           IB = 0, IA = 0;
      TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
      int               G = 0, S = 0;
      FDS_Idata(I, SB, IB, SA, IA, GT, G, ST, S);
      bool FhasGE = FDS_SIisGIofIofSBAofTofI(BDS, IE, I);
      // clang-format off
      if (FhasGE) {removed = true; l3dF.Remove(it); continue;} // E has split ON F (cto904A3;e19,f14)
      // clang-format on
      const TopoDS_Shape& F    = BDS.Shape(S);
      bool                hsdm = HDS->HasSameDomain(F);
      if (!hsdm)
      {
        it.Next();
        continue;
      }
      NCollection_List<TopoDS_Shape>::Iterator issdm(BDS.ShapeSameDomain(F));
      bool                                     foundinsdm = false;
      for (; issdm.More(); issdm.Next())
        if (mapf.Contains(issdm.Value()))
        {
          foundinsdm = true;
          break;
        }
      if (!foundinsdm)
      {
        it.Next();
        continue;
      } // E is IN geometry(F) (cto002D2;e33,f31)

      // E is edge of FF sdm with F
      removed = true;
      l3dF.Remove(it);
    } // it(l3dF)

    if (!removed)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII = BDS.ChangeShapeInterferences(E);
    LII.Clear();
    LII.Append(LIcopy);
    LII.Append(l3dF);
  } // i=1..ns
} // PURGEforE9

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE1(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
{
  // xpu160398 complement for INTERNAL transitions on section edge
  // recall : we add geometry G on section edge SE only if
  //             SE has ISE=(T(face F),G,edge ES),
  //          && F  has IF =(T,SE,S).
  //
  // purpose : SE has ISE=(T(face F'),G,edge),
  //           F' has no interference with support==SE
  //          => looking for F / F and F' share ES,
  //                             F sdm with Fanc (one face ancestor of ES)
  //                             F has I' = (T,SE,S).
  // cto 009 D1

  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();

  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge&                                              SE  = BDS.SectionEdge(i);
    int                                                             ISE = BDS.Shape(SE);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(SE);

    TopOpeBRepDS_TKI tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {

      // ISE = (INTERNAL(FACE),G,EDGE) :
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        loicopy;
      FDS_assign(loi, loicopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lI1;
      bool hasINT = ::FUN_selectTRAINTinterference(loicopy, lI1);
      if (!hasINT)
        continue;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lI2;
      int nI = FUN_selectTRASHAinterference(lI1, TopAbs_FACE, lI2);
      if (nI < 1)
        continue;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lI3;
      nI = FUN_selectSKinterference(lI2, TopOpeBRepDS_EDGE, lI3);
      if (nI < 1)
        continue;

      bool keepI = false;
      //      for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lI3)
      //      ;it.More(); it.Next()){
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lI3);
      for (; it.More(); it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
        keepI                                           = FDS_SIisGIofIofSBAofTofI(BDS, ISE, I);
        if (keepI)
          break;
      }
      if (keepI)
        continue;

      // ISE = (INTERNAL(F),G,ES), F has no I with G==SE
      // find fSE / fSE (SE's ancestor face) has  IfSE = (T,SE,support)
      //            - fSE sdm & fES (ES's ancestor face) -
      //      for (it.Initialize(lI3) ;it.More(); it.Next()){
      it.Initialize(lI3);
      for (; it.More(); it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
        TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
        int               G1 = 0, S = 0;
        FDS_data(I, GT, G1, ST, S);
        TopAbs_ShapeEnum tsb, tsa;
        int              isb, isa;
        FDS_Tdata(I, tsb, isb, tsa, isa);
        const TopoDS_Edge& ES = TopoDS::Edge(BDS.Shape(S));
        TopoDS_Face        fSE, fES;
        bool               sdmf = ::FUN_ds_hasSDMancestorfaces(HDS, SE, ES, fSE, fES);
        if (!sdmf)
          continue;
        int IfES = BDS.Shape(fES);

        const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIf =
          BDS.ShapeInterferences(fES);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIfcopy;
        FDS_copy(LIf, LIfcopy);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIfound;
        int nfound = FUN_selectGIinterference(LIfcopy, ISE, LIfound);
        if (nfound < 1)
          continue;

        // cto 009 B1 : Ifor=(FORWARD(IfES),G1,S) && Irev=(REVERSED(IfES),G1,S)
        //         -> do NOT reduce to I = (INTERNAL(IfES),G1,S) (we can have EXTERNAL Tr)
        FDS_copy(loi, loicopy);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lI4;
        FUN_selectITRASHAinterference(loicopy, IfES, lI4);
        bool                                                     hasFORREV = false;
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lfor;
        int nFOR = FUN_selectTRAORIinterference(lI4, TopAbs_FORWARD, lfor);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lrev;
        int nREV  = FUN_selectTRAORIinterference(lI4, TopAbs_REVERSED, lrev);
        hasFORREV = (nFOR > 0) || (nREV > 0);
        if (hasFORREV)
          break;

        // newI :
        // -----
        TopOpeBRepDS_Transition newT(TopAbs_INTERNAL);
        newT.Index(IfES);
        double par   = FDS_Parameter(I);
        bool   isevi = I->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference));
        bool   B     = false;
        if (isevi)
          B = occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->GBound();
        occ::handle<TopOpeBRepDS_Interference> newI =
          MakeEPVInterference(newT, S, G1, par, K, TopOpeBRepDS_EDGE, B);
        HDS->StoreInterference(newI, SE);
        break;
      } // it(lI3)
    } // tki
  } // i=1..nse
} // completeforSE1

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE2(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
{
  // xpu250398 compute 3dI
  // recall : to have spOUT(SE) / spIN(SE) with SE section edge on bound G
  //          we need IF = (T(F),G,F) with G not SE bound.
  //
  // purpose : Sometimes for gap's reasons we can miss such interference.
  //           attached to ES, IFE = (T(FTRA),G,ES) with G not Gbound
  //           we find no IF = (T(FTRA),G,FTRA)
  //           1. look for FCX / FCX shares ES with FTA (recall : rkG==rkFCX)
  //           2. compute I3d = (T2(FCX),G,FCX)
  //   !!! if SE has splits ON F near G (if we have a 2dI near G) -> do NOT add I3d
  // PRO12696 : on se 46 (create FEI/ f50)

  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();

  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge& SE   = TopoDS::Edge(BDS.SectionEdge(i));
    int                rkSE = BDS.AncestorRank(SE);
    BDS.Shape(SE);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(SE);

    TopOpeBRepDS_TKI tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {

      TopOpeBRepDS_Kind                                               K;
      int                                                             G;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      bool point  = (K == TopOpeBRepDS_POINT);
      bool vertex = (K == TopOpeBRepDS_VERTEX);

      // interferences on GBound=1 not treated
      bool try1 = true;
      if (vertex)
      {
        const TopoDS_Vertex& vG = TopoDS::Vertex(BDS.Shape(G));
        TopoDS_Vertex        OOv;
        bool                 hasOO = FUN_ds_getoov(vG, HDS, OOv);
        int                  ovSE  = FUN_tool_orientVinE(vG, SE);
        if ((ovSE == 0) && hasOO)
          ovSE = FUN_tool_orientVinE(OOv, SE);
        if (ovSE != 0)
          try1 = false;
      }
      if (!try1)
        continue;

      // SE has    {I=(T(face),    G not Gbound, edge)}
      //    has NO {I'=(T'(face'), G not Gbound, face')}
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loicopy;
      FDS_assign(loi, loicopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1;
      FUN_selectTRASHAinterference(loicopy, TopAbs_FACE, l1);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lF;
      int nF = FUN_selectSKinterference(l1, TopOpeBRepDS_FACE, lF);
      if (nF > 1)
        continue;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lFE;
      int nFE = FUN_selectSKinterference(l1, TopOpeBRepDS_EDGE, lFE);
      if (nFE == 0)
        continue;

      // I = (T(FTRA),G not Gbound, ES)
      const occ::handle<TopOpeBRepDS_Interference>& I   = lFE.First();
      double                                        par = FDS_Parameter(I);

      TopOpeBRepDS_Kind ST = TopOpeBRepDS_UNKNOWN;
      int               S  = 0;
      FDS_data(I, K, G, ST, S);
      TopAbs_ShapeEnum tsb = TopAbs_SHAPE, tsa = TopAbs_SHAPE;
      int              isb = 0, isa = 0;
      FDS_Tdata(I, tsb, isb, tsa, isa);
      const TopoDS_Face& FTRA = TopoDS::Face(BDS.Shape(isb));
      const TopoDS_Edge& ES   = TopoDS::Edge(BDS.Shape(S));

      // lfCX = {fCX / fCX shares ES with FTRA}
      const NCollection_List<TopoDS_Shape>& lfCX = FDSCNX_EdgeConnexitySameShape(ES, HDS);
      for (NCollection_List<TopoDS_Shape>::Iterator itfcx(lfCX); itfcx.More(); itfcx.Next())
      {
        const TopoDS_Face& FCX  = TopoDS::Face(itfcx.Value());
        int                IFCX = BDS.Shape(FCX);
        if (FCX.IsSame(FTRA))
          continue;

        // xpu140498 : NYI check SE has part ON FCX(with bound G)
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2;
        int n2 = FUN_selectITRASHAinterference(lFE, IFCX, l2);
        if (n2 > 0)
          continue;

        double OOpar = 0.;
        if (point)
        {
          gp_Pnt p3d = BDS.Point(G).Point();
          double t1  = BDS.Point(G).Tolerance();
          double t2  = FUN_tool_maxtol(ES);
          double t   = (t1 > t2) ? t1 : t2;
          double d   = 1.e1;
          bool   ok  = FUN_tool_projPonE(p3d, ES, OOpar, d);
          if (!ok)
          {
            continue;
          }
          if (d > t)
          {
            continue;
          }
        }
        if (vertex)
        { // rkG == rkES
          const TopoDS_Vertex& vG  = TopoDS::Vertex(BDS.Shape(G));
          int                  rkG = BDS.AncestorRank(G);

          // modified by NIZHNY-MKK  Mon Apr  2 15:38:11 2001.BEGIN
          // 	  if (rkG == rkSE) OOpar = BRep_Tool::Parameter(vG,ES);
          if (rkG == rkSE)
          {
            int hasvG = FUN_tool_orientVinE(vG, ES);
            if (hasvG == 0)
            {
              continue;
            }
            OOpar = BRep_Tool::Parameter(vG, ES);
          }
          // modified by NIZHNY-MKK  Mon Apr  2 15:38:30 2001.END
          else
          {
            TopoDS_Shape oov;
            bool         hasoov = FUN_ds_getoov(vG, BDS, oov);
            if (!hasoov)
            {
              double t1 = BRep_Tool::Tolerance(vG);
              double t2 = FUN_tool_maxtol(ES);
              double t  = (t1 > t2) ? t1 : t2;
              gp_Pnt p  = BRep_Tool::Pnt(vG);
              double parES, dd = 1.e1;
              bool   ok = FUN_tool_projPonE(p, ES, parES, dd);
              if (!ok)
              {
                continue;
              }
              if (dd > t)
              {
                continue;
              }
              OOpar = parES;
            }
            else
              OOpar = BRep_Tool::Parameter(TopoDS::Vertex(oov), ES);
          }
        }

        gp_Pnt2d OOuv;
        bool     ok = FUN_tool_paronEF(ES, OOpar, FCX, OOuv);
        if (!ok)
        {
          continue;
        }

        TopOpeBRepDS_Transition newT;
        // -------
        bool   isonper = false;
        double par1 = 0.0, par2 = 0.0;
        double factor = 1.e-4;
        FDS_LOIinfsup(BDS, SE, par, K, G, BDS.ShapeInterferences(SE), par1, par2, isonper);

        TopOpeBRepTool_makeTransition MKT;
        TopAbs_State                  stb = TopAbs_UNKNOWN, sta = TopAbs_UNKNOWN;
        ok = MKT.Initialize(SE, par1, par2, par, FCX, OOuv, factor);
        if (ok)
          ok = MKT.SetRest(ES, OOpar);
        if (ok)
          ok = MKT.MkTonE(stb, sta);
        if (!ok)
        {
          continue;
        }
        newT.Before(stb);
        newT.After(sta);
        newT.Index(IFCX);

        occ::handle<TopOpeBRepDS_Interference> newI;
        // --------
        bool B = false;
        if (vertex)
          B = occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->GBound();
        newI = MakeEPVInterference(newT, IFCX, G, par, K, TopOpeBRepDS_FACE, B);
        HDS->StoreInterference(newI, SE);
      } // itfcx
    } // tki
  } // nse
} // completeforSE2

static bool FUN_ds_completeforSE3(
  const TopOpeBRepDS_DataStructure& BDS,
  const TopoDS_Edge&                SE,
  const TopOpeBRepDS_Kind           K,
  //                                 const int G,
  const int,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi,
  double&                                                         parE,
  int&                                                            IES,
  int&                                                            ITRASHA,
  TopOpeBRepDS_Transition&                                        Tr)
{
  if (K == TopOpeBRepDS_VERTEX)
    return false;
  int ISE = BDS.Shape(SE);

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loicopy;
  FDS_assign(loi, loicopy);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lINT;
  int nINT = FUN_selectTRAORIinterference(loicopy, TopAbs_INTERNAL, lINT);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lEXT;
  int nEXT = FUN_selectTRAORIinterference(loicopy, TopAbs_EXTERNAL, lEXT);
  int n1   = nINT + nEXT;
  if (n1 < 1)
    return false;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1;
  l1.Append(lINT);
  l1.Append(lEXT);

  // b. I = (INT/EXT(F),G,S), F has I with G==ES
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2;
  int n2 = FUN_selectTRASHAinterference(l1, TopAbs_FACE, l2);
  if (n2 < 1)
    return false;
  bool okb = false;
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(l2); it2.More();
       it2.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
    okb                                              = FDS_SIisGIofIofSBAofTofI(BDS, ISE, I2);
    if (okb)
      break;
  }
  if (!okb)
    return false;

  // a. I3d = IFE+IF
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2e;
  int n2e = FUN_selectSKinterference(l2, TopOpeBRepDS_EDGE, l2e);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2f;
  int n2f = FUN_selectSKinterference(l2, TopOpeBRepDS_FACE, l2f);
  if ((n2e == 0) || (n2f == 0))
    return false;
  int                                                      sI3d = l2f.First()->Support();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l3d;
  int n3d = FUN_selectITRASHAinterference(l2e, sI3d, l3d);
  if (n3d < 1)
    return false;

  // Tr
  //---
  const occ::handle<TopOpeBRepDS_Interference>& I3 = l3d.First();
  TopOpeBRepDS_Kind K3 = TopOpeBRepDS_UNKNOWN, ST3 = TopOpeBRepDS_UNKNOWN;
  int               G3 = 0, S3 = 0;
  FDS_data(I3, K3, G3, ST3, S3);
  TopAbs_ShapeEnum tsb3 = TopAbs_SHAPE, tsa3 = TopAbs_SHAPE;
  int              isb3 = 0, isa3 = 0;
  FDS_Tdata(I3, tsb3, isb3, tsa3, isa3);
  IES     = S3;
  ITRASHA = isb3;

  const TopoDS_Edge& Eline = TopoDS::Edge(BDS.Shape(IES));
  const TopoDS_Face& F     = TopoDS::Face(BDS.Shape(ITRASHA));
  parE                     = FDS_Parameter(I3);

  double parline = 0.0;
  bool   ok      = FUN_tool_parE(SE, parE, Eline, parline);
  if (!ok)
  {
    return false;
  }
  gp_Pnt2d uv;
  ok = FUN_tool_paronEF(Eline, parline, F, uv);
  if (!ok)
  {
    return false;
  }
  double par1 = 0.0, par2 = 0.0;
  FUN_tool_bounds(SE, par1, par2);
  double factor = 1.e-4;

  TopOpeBRepTool_makeTransition MKT;
  TopAbs_State                  stb = TopAbs_UNKNOWN, sta = TopAbs_UNKNOWN;
  ok = MKT.Initialize(SE, par1, par2, parE, F, uv, factor);
  if (ok)
    ok = MKT.SetRest(Eline, parline);
  if (ok)
    ok = MKT.MkTonE(stb, sta);
  if (!ok)
  {
    return false;
  }
  Tr.Before(stb);
  Tr.After(sta);
  Tr.Index(ITRASHA);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE3(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
{
  // xpu170498 : t3 (splits of e7)
  // prequesitory :
  //  edge SE - a. is tangent to FTRA on ES at G, has NO SPLIT ON FTRA (near G).
  //          - b. has splitON FCX (FCX and FTRA share ES)
  //  a. IFE=(T(FTRA),Gpoint,ES) && IF=(T(FTRA),Gpoint,FTRA)
  //     T describes T3d
  //  b. IFE=(T'(FCX),Gpoint,S) / FCX has I=(T'',SE,SS)
  //
  // purpose : when T(FTRA) is INTERNAL/EXTERNAL, the compute of spIN/spOU fails,
  //           => compute interference I3d=IFE + IF with T==FORWARD/REVERSED.

  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();
  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge&                                              SE  = BDS.SectionEdge(i);
    int                                                             ISE = BDS.Shape(SE);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(SE);

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newLI;
    bool                                                     hasnewLI = false;
    TopOpeBRepDS_TKI                                         tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {

      TopOpeBRepDS_Kind                                               K    = TopOpeBRepDS_UNKNOWN;
      int                                                             G    = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi  = tki.Value(K, G);
      double                                                          parE = 0.0;
      int                                                             IES = 0, ITRASHA = 0;
      TopOpeBRepDS_Transition                                         Tr;
      bool ok = FUN_ds_completeforSE3(BDS, SE, K, G, loi, parE, IES, ITRASHA, Tr);

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loicopy;
      FDS_assign(loi, loicopy);
      if (!ok)
      {
        newLI.Append(loicopy);
        continue;
      }

      // delete interferences =(T(face IF),G,S)
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lIdel;
      FUN_selectITRASHAinterference(loicopy, ITRASHA, lIdel);

      occ::handle<TopOpeBRepDS_Interference> TrFE, TrFF;
      //--------------
      hasnewLI = true;
      TrFE     = MakeEPVInterference(Tr, ISE, G, parE, K, TopOpeBRepDS_EDGE, false);
      TrFF     = MakeEPVInterference(Tr, ITRASHA, G, parE, K, TopOpeBRepDS_FACE, false);
      newLI.Append(TrFF);
      newLI.Append(TrFE);
      newLI.Append(loicopy);
    } // tki

    if (hasnewLI)
    {
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII =
        BDS.ChangeShapeInterferences(SE);
      LII.Clear();
      LII.Append(newLI);
    }

  } // i=1..nse
} // completeforSE3

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_ds_shareG(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                   const int                                       iF1,
                                   const int                                       iF2,
                                   const int                                       iE2,
                                   const TopoDS_Edge&                              Esp,
                                   bool&                                           shareG)
// ----------------------------------------------------------------------
// F1 sdm F2, share Esp (split of E2)
{
  const TopOpeBRepDS_DataStructure& BDS  = HDS->DS();
  bool                              hsdm = HDS->HasSameDomain(BDS.Shape(iE2));
  if (!hsdm)
    return false;

  const TopoDS_Face& F1 = TopoDS::Face(BDS.Shape(iF1));
  const TopoDS_Face& F2 = TopoDS::Face(BDS.Shape(iF2));
  const TopoDS_Edge& E2 = TopoDS::Edge(BDS.Shape(iE2));

  double tol = Precision::Confusion() * 1.e3;
  double f = 0.0, l = 0.0;
  FUN_tool_bounds(Esp, f, l);
  double x   = 0.45678;
  double par = (1 - x) * f + x * l;
  gp_Pnt P;
  bool   ok = FUN_tool_value(par, Esp, P);
  if (!ok)
    return false;

  double d2 = 0.0, par2 = 0.0;
  ok = FUN_tool_projPonE(P, E2, par2, d2);
  if (!ok)
    return false;
  if (d2 > tol)
    return false;

  gp_Vec nggeomF2;
  ok = FUN_tool_nggeomF(par2, E2, F2, nggeomF2);
  if (!ok)
    return false;
  gp_Dir nxx2;
  ok = FUN_tool_getxx(F2, E2, par2, nggeomF2, nxx2);
  if (!ok)
    return false;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mE1;
  TopExp::MapShapes(F1, TopAbs_EDGE, mE1);
  const NCollection_List<TopoDS_Shape>& sdmE1 = BDS.ShapeSameDomain(iE2);
  for (NCollection_List<TopoDS_Shape>::Iterator it(sdmE1); it.More(); it.Next())
  {
    const TopoDS_Edge& E1  = TopoDS::Edge(it.Value());
    bool               isb = mE1.Contains(E1);
    if (!isb)
      continue;

    double d1 = 0.0, par1 = 0.0;
    ok = FUN_tool_projPonE(P, E1, par1, d1);
    if (!ok)
      continue;
    if (d1 > tol)
      continue;

    // E1 on F1, E2 on F2, E1 and E2 share split Esp(nyi : check it)
    gp_Vec nggeomF1;
    ok = FUN_tool_nggeomF(par1, E1, F1, nggeomF1);
    if (!ok)
      return false;
    gp_Dir nxx1;
    ok = FUN_tool_getxx(F1, E1, par1, nggeomF1, nxx1);
    if (!ok)
      return false;

    double prod = nxx1.Dot(nxx2);
    shareG      = (prod > 0.);
    return true;
  }
  return false;
}
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_ds_mkTonFsdm(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const int                                       iF1,
                                      const int                                       iF2,
                                      const int                                       iE2,
                                      const int                                       iEG,
                                      const double                                    paronEG,
                                      const TopoDS_Edge&                              Esp,
                                      const bool                                      pardef,
                                      TopOpeBRepDS_Transition&                        T)
// ----------------------------------------------------------------------
// =================================
// prequesitory : F1 and F2 are SDSO
// =================================
// F1 sdm F2,  EG has split ON F1 shared by F2
//
// EG and E2 share geometric point G (of param paronEG)
// pardef = true : paronEG is defined
// else         : use Esp to find paronEG, Esp is split of EG
// (!! we can have EG==E2)
//
// purpose : computes 2d transition of F1 when crossing EG relative to
//           2dmatter(F2) limited by E2 :
//        -> 2d FEI attached to F1 = (T(F2), G=EG, S=F2)
{
  const TopOpeBRepDS_DataStructure& BDS  = HDS->DS();
  bool                              hsdm = HDS->HasSameDomain(BDS.Shape(iE2));
  if (!hsdm)
    return false;

  const TopoDS_Face& F1 = TopoDS::Face(BDS.Shape(iF1));
  const TopoDS_Face& F2 = TopoDS::Face(BDS.Shape(iF2));
  const TopoDS_Edge& E2 = TopoDS::Edge(BDS.Shape(iE2));
  const TopoDS_Edge& EG = TopoDS::Edge(BDS.Shape(iEG));

  bool   EGisE2 = (iEG == iE2);
  double tol    = Precision::Confusion() * 1.e3;

  // beafter :
  // ---------
  bool   ok = false;
  gp_Pnt P;
  double parEG = 0.0;
  if (pardef)
    parEG = paronEG;
  else
  {
    double f = 0.0, l = 0.0;
    FUN_tool_bounds(Esp, f, l);
    double dEG = 0.0;
    ok         = FUN_tool_projPonE(P, EG, parEG, dEG);
    if (!ok)
      return false;
    if (dEG > tol)
      return false;
  }
  ok = FUN_tool_value(parEG, EG, P);
  if (!ok)
    return false;
  gp_Vec tgtEG;
  ok = TopOpeBRepTool_TOOL::TggeomE(parEG, EG, tgtEG);
  if (!ok)
    return false;
  gp_Vec ngF1;
  ok = FUN_tool_nggeomF(parEG, EG, F1, ngF1);
  if (!ok)
    return false;
  gp_Vec beafter = ngF1 ^ tgtEG;

  // nxx2 :
  // ------
  double par2 = 0.0;
  if (EGisE2)
    par2 = parEG;
  else
  {
    double d2 = 0.0;
    ok        = FUN_tool_projPonE(P, E2, par2, d2);
    if (!ok)
      return false;
    if (d2 > tol)
      return false;
  }
  gp_Vec ngF2;
  ok = FUN_tool_nggeomF(par2, E2, F2, ngF2);
  if (!ok)
    return false;
  gp_Dir nxx2;
  ok = FUN_tool_getxx(F2, E2, par2, ngF2, nxx2);
  if (!ok)
    return false;

  // T :
  // ---
  bool sdmEGE2 = EGisE2;
  if (!sdmEGE2)
    sdmEGE2 = FUN_ds_sdm(BDS, EG, E2);
  if (!sdmEGE2)
    return false;

  double prod = beafter.Dot(nxx2);
  double tola = Precision::Angular() * 1.e3;
  ok          = (std::abs(1 - std::abs(prod)) < tola);
  if (!ok)
    return false;

  if (prod > 0.)
    T = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
  else
    T = TopOpeBRepDS_Transition(TopAbs_IN, TopAbs_OUT);
  return true;
}

#define UNKNOWN (0)
#define ONSAMESHA (1)
#define CLOSESAME (11)
#define ONOPPOSHA (2)
#define CLOSEOPPO (22)
#define FORREVOPPO (222)

Standard_EXPORT int FUN_ds_oriEinF(const TopOpeBRepDS_DataStructure& BDS,
                                   const TopoDS_Edge&                E,
                                   const TopoDS_Shape&               F,
                                   TopAbs_Orientation&               O)
// purpose :
//  * E is edge of F :returns orientation of E in F
//  * E is edge of Fsd : Fsd sdm with F,
//  returns orientation of E in F /E describes the same 2d area in F as in Fsd
//  * !! if E is closing edge, returns CLOSESAME or CLOSEOPPO
{
  O       = TopAbs_EXTERNAL;
  int rkF = BDS.AncestorRank(F);
  int rkE = BDS.AncestorRank(E);

  const TopoDS_Edge& EE = TopoDS::Edge(E);
  const TopoDS_Face& FF = TopoDS::Face(F);
  int                iF = BDS.Shape(F);
  TopAbs_Orientation oF = BDS.Shape(iF).Orientation();

  if (rkF == rkE)
  {
    bool samsha = FUN_tool_orientEinFFORWARD(EE, FF, O);
    if (samsha)
    {
      bool iscE = BRep_Tool::IsClosed(EE, FF);
      if (iscE)
        return CLOSESAME;
      else
        return ONSAMESHA;
    }
    else
      return UNKNOWN;
  }
  else
  {
    const NCollection_List<TopoDS_Shape>& sdmFs = BDS.ShapeSameDomain(FF);
    bool                                  hsdm  = (sdmFs.Extent() > 0);

    if (hsdm)
    {
      // clang-format off
      bool hasFOR=false, hasREV=false; // xpu120898 (PRO14785 : e36 shared by f34 & f39,
                                  // faces sdm with f16)
      // clang-format on
      TopOpeBRepDS_Config C = BDS.SameDomainOri(FF);

      for (NCollection_List<TopoDS_Shape>::Iterator it(sdmFs); it.More(); it.Next())
      {
        const TopoDS_Face& Fsdm  = TopoDS::Face(it.Value());
        int                iFsdm = BDS.Shape(Fsdm);
        int                rksdm = BDS.AncestorRank(Fsdm);
        if (rksdm == rkF)
          continue;

        bool samsha = FUN_tool_orientEinFFORWARD(EE, Fsdm, O);
        if (!samsha)
          continue;

        bool iscE = BRep_Tool::IsClosed(EE, Fsdm);
        if (iscE)
          return CLOSEOPPO;
        else
        {
          TopOpeBRepDS_Config Csdm       = BDS.SameDomainOri(Fsdm);
          bool                toreverse1 = (Csdm != C) && (!M_INTERNAL(O)) && (!M_EXTERNAL(O));
          if (toreverse1)
            O = TopAbs::Complement(O);

          TopAbs_Orientation oFsdm = BDS.Shape(iFsdm).Orientation();
          bool toreverse2          = (oF != oFsdm) && (!M_INTERNAL(oFsdm)) && (!M_EXTERNAL(oFsdm));
          if (toreverse2)
            O = TopAbs::Complement(O);
          if (!hasFOR)
            hasFOR = M_FORWARD(O);
          if (!hasREV)
            hasREV = M_REVERSED(O);
        }
      } // it(sdmFs)
      if (hasFOR && hasREV)
        return FORREVOPPO;
      if (hasFOR || hasREV)
      {
        O = hasFOR ? TopAbs_FORWARD : TopAbs_REVERSED;
        return ONOPPOSHA;
      }
    }
    else
      return UNKNOWN;
  }
  return UNKNOWN;
}

#define NONE (0)
#define EREFandESUP (2)
#define FIRST (1)
#define LAST (2)

static int FUN_EisSE2(const TopOpeBRepDS_DataStructure& BDS,
                      //                                   const NCollection_Map<TopoDS_Shape,
                      //                                   TopTools_ShapeMapHasher>& Fsdm,
                      const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&,
                      const TopoDS_Edge&                            E,
                      const occ::handle<TopOpeBRepDS_Interference>& I)
{
  const TopOpeBRepDS_Transition& T  = I->Transition();
  const TopAbs_Orientation       O  = T.Orientation(TopAbs_IN);
  TopAbs_ShapeEnum               SB = TopAbs_SHAPE, SA = TopAbs_SHAPE;
  int                            IB = 0, IA = 0;
  TopOpeBRepDS_Kind              GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
  int                            G = 0, S = 0;
  FDS_Idata(I, SB, IB, SA, IA, GT, G, ST, S);
  if (GT != TopOpeBRepDS_VERTEX)
    return NONE;
  if (M_EXTERNAL(O))
    return NONE;

  // E sdm ES
  if (SB != TopAbs_EDGE)
    return NONE;
  //  BDS.Shape(S);

  int                  rkE   = BDS.AncestorRank(E);
  int                  rkG   = BDS.AncestorRank(G);
  const TopoDS_Vertex& VG    = TopoDS::Vertex(BDS.Shape(G));
  int                  Gsd   = 0;
  bool                 Ghsd  = FUN_ds_getVsdm(BDS, G, Gsd);
  int                  oGinE = 0;
  if (Ghsd)
  {
    const TopoDS_Vertex& VGsd = TopoDS::Vertex(BDS.Shape(Gsd));
    if (rkE == rkG)
      oGinE = FUN_tool_orientVinE(VG, E);
    else
      oGinE = FUN_tool_orientVinE(VGsd, E);
  }
  else
    oGinE = FUN_tool_orientVinE(VG, E);

  if (oGinE == 0)
    return EREFandESUP; // G in IN E

  bool noshare = ((oGinE == LAST) && M_FORWARD(O));
  noshare      = noshare || ((oGinE == FIRST) && M_REVERSED(O));
  if (noshare)
    return NONE;

  if (SB == TopAbs_EDGE)
  { // E sdm edge(ST)
    if (!Ghsd)
      return EREFandESUP;
    return EREFandESUP;
  }

  return NONE;
}

Standard_EXPORT void FUN_ds_FillSDMFaces(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         ns  = BDS.NbShapes();

  for (int i = 1; i <= ns; i++)
  {
    const TopoDS_Shape& S = BDS.Shape(i);
    if (S.ShapeType() != TopAbs_FACE)
      continue;

    int                                                    rkS = BDS.AncestorRank(S);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Fsdm;
    NCollection_List<TopoDS_Shape>::Iterator               itf(BDS.ShapeSameDomain(S));
    for (; itf.More(); itf.Next())
    {
      const TopoDS_Shape& f   = itf.Value();
      int                 rkf = BDS.AncestorRank(f);
      if (rkf != rkS)
        Fsdm.Add(f);
    }
    bool hsd = (Fsdm.Extent() > 0);
    if (!hsd)
      continue;

    TopExp_Explorer ex(S, TopAbs_EDGE);
    for (; ex.More(); ex.Next())
    {
      const TopoDS_Edge& E    = TopoDS::Edge(ex.Current());
      bool               hase = BDS.HasShape(E);
      if (!hase)
        continue;
      bool isse = BDS.IsSectionEdge(E);
      if (isse)
        continue;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI =
        BDS.ShapeInterferences(E);
      int nI = LI.Extent();
      if (nI < 1)
        continue;

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it;
      for (it.Initialize(LI); it.More(); it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I     = it.Value();
        int                                           isSE2 = FUN_EisSE2(BDS, Fsdm, E, I);
        if (isSE2 == NONE)
          continue;
        BDS.AddSectionEdge(E);
        if (isSE2 == EREFandESUP)
        {
          int                ST = I->Support();
          const TopoDS_Edge& ES = TopoDS::Edge(BDS.Shape(ST));
          BDS.AddSectionEdge(ES);
        }
        break;
      }
    } // ex(S, TopAbs_EDGE);
  } // i = i..ns
}

Standard_EXPORT void FUN_ds_addSEsdm1d(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         ns  = BDS.NbShapes();
  for (int i = 1; i <= ns; i++)
  {
    const TopoDS_Shape& S = BDS.Shape(i);
    if (S.ShapeType() != TopAbs_EDGE)
      continue;
    const TopoDS_Edge& E   = TopoDS::Edge(S);
    bool               dgE = BRep_Tool::Degenerated(E);
    if (dgE)
      continue;

    bool isse = BDS.IsSectionEdge(E);
    if (isse)
      continue;
    int rkE = BDS.AncestorRank(E);
    if (rkE != 1)
      continue;

    bool                           shareG = false;
    NCollection_List<TopoDS_Shape> lsd;
    TopOpeBRepDS_TOOL::EShareG(HDS, E, lsd);
    NCollection_List<TopoDS_Shape>::Iterator itsd(lsd);
    if (itsd.More())
      shareG = true;
    for (; itsd.More(); itsd.Next())
      BDS.AddSectionEdge(TopoDS::Edge(itsd.Value()));
    if (shareG)
      BDS.AddSectionEdge(E);
  } // i=1..ns
} // FUN_ds_addSEsdm1d

Standard_EXPORT int FUN_ds_hasI2d(
  //                              const int EIX,
  const int,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LLI,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       LI2d)
{
  // LI : attached to EIX at given G
  // recall : I3d if I1=(T(FTRA),G,FTRA) && I2=(T(FTRA),G,E)
  //          I2d if only I=(T(FTRA),G,E) (=>EIX is on FF sdm with FTRA)

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI;
  FDS_assign(LLI, LI);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> L1dE;
  FUN_selectTRASHAinterference(LI, TopAbs_EDGE, L1dE);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIF;
  FUN_selectSKinterference(LI, TopOpeBRepDS_FACE, LIF);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIE;
  FUN_selectSKinterference(LI, TopOpeBRepDS_EDGE, LIE);
  LI.Append(L1dE);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itE(LIE);
  for (; itE.More(); itE.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& IE = itE.Value();
    TopOpeBRepDS_Kind GTE = TopOpeBRepDS_UNKNOWN, STE = TopOpeBRepDS_UNKNOWN;
    int               GE = 0, SE = 0;
    FDS_data(IE, GTE, GE, STE, SE);
    TopAbs_ShapeEnum tsb = TopAbs_SHAPE, tsa = TopAbs_SHAPE;
    int              isb = 0, isa = 0;
    FDS_Tdata(IE, tsb, isb, tsa, isa);

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itF(LIF);
    bool                                                               is3d = false;
    for (; itF.More(); itF.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& IF = itF.Value();
      TopOpeBRepDS_Kind GTF = TopOpeBRepDS_UNKNOWN, STF = TopOpeBRepDS_UNKNOWN;
      int               GF = 0, SF = 0;
      FDS_data(IF, GTF, GF, STF, SF);
      if (GE != GF)
        continue;
      if (SF == isb)
      {
        is3d = true;
        break;
      }
    } // itF
    if (!is3d)
      LI2d.Append(IE);
  } // itE
  int nLI2d = LI2d.Extent();
  return nLI2d;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE4(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu160698 : section edge SE intersects ES at G=point
//             ES closing edge on FTRA
//             we need to have transitions :
//             T1=(OU/IN(FTRA),GP,ES)
//             T2=(IN/OU(FTRA),GP,ES), for the compute of splitON
// PRO6965, (SE=e13,F=f6,ES=e12)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();
  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge& SE = BDS.SectionEdge(i);
    BDS.Shape(SE);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(SE);

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newLI;
    TopOpeBRepDS_TKI                                         tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      if (K != TopOpeBRepDS_POINT)
        continue;

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loicopy;
      FDS_assign(loi, loicopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1;
      FUN_selectSKinterference(loicopy, TopOpeBRepDS_EDGE, l1);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2;
      int n2 = FUN_selectTRASHAinterference(l1, TopAbs_FACE, l2);
      if (n2 < 1)
        continue;

      const occ::handle<TopOpeBRepDS_Interference>& I = l2.First();
      TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
      int               S = 0;
      FDS_data(I, GT, G, ST, S);
      TopAbs_ShapeEnum tsb = TopAbs_SHAPE, tsa = TopAbs_SHAPE;
      int              isb = 0, isa = 0;
      FDS_Tdata(I, tsb, isb, tsa, isa);
      const TopoDS_Edge& ES   = TopoDS::Edge(BDS.Shape(S));
      const TopoDS_Face& FTRA = TopoDS::Face(BDS.Shape(isb));

      bool closing = FUN_tool_IsClosingE(ES, FTRA, FTRA);
      if (!closing)
        continue;

      bool hasFOR = false, hasREV = false;
      for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(l2); it.More();
           it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I2 = it.Value();
        TopOpeBRepDS_Kind GT2 = TopOpeBRepDS_UNKNOWN, ST2 = TopOpeBRepDS_UNKNOWN;
        int               G2 = 0, S2 = 0;
        FDS_data(I2, GT2, G2, ST2, S2);
        TopAbs_ShapeEnum tsb2 = TopAbs_SHAPE, tsa2 = TopAbs_SHAPE;
        int              isb2 = 0, isa2 = 0;
        FDS_Tdata(I2, tsb2, isb2, tsa2, isa2);
        bool error = (S2 != S) || (isb2 != isb);
        if (error)
          return; // nyi raise
        TopAbs_Orientation O2 = I2->Transition().Orientation(TopAbs_IN);
        if (!hasFOR)
          hasFOR = M_FORWARD(O2);
        if (!hasREV)
          hasREV = M_REVERSED(O2);
      }
      if (!hasFOR && !hasREV)
        continue;
      if (hasFOR && hasREV)
        continue;
      TopAbs_Orientation      newO = hasFOR ? TopAbs_REVERSED : TopAbs_FORWARD;
      TopOpeBRepDS_Transition newT(newO);
      newT.Index(isb);
      double                                 par  = FDS_Parameter(I);
      occ::handle<TopOpeBRepDS_Interference> newI = MakeEPVInterference(newT, S, G, par, K, false);
      HDS->StoreInterference(newI, SE);
    } // tki
  } // i=1..nse
} // completeforSE4

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE5(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu190698 : section edge SE has spON face F,
//             FCX is tangent to F at G (FCX and F are on same shape)
//             SE has I  = (T(F),G,S),         T=(OUT,OUT)
//                    I' = (T'(Fsd),G,Sedge), T' FORWARD or REVERSED
//             T describes states (ON/OUT) or (OUT/ON)
// PRO12695 (SE20,P1,FCX16,F29)
// xpu131098 : SE has I  = (T(F),G,S),         T=(IN,IN)
//                    I' = (T'(Fsd),G,Sedge), T' FORWARD or REVERSED
//             T describes states (ON/IN) or (OUT/IN)
//
//    recall : for the compute of spOU/IN(ES), we need to interferences
//             TskFACE = (T(FF),G,FF) T=FORWARD/REVERSED
// cto 902 D9 (SE17,P4,F29,FCX4)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();
  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge& SE = BDS.SectionEdge(i);
    BDS.Shape(SE);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI =
      BDS.ShapeInterferences(SE);

    // xpu020399 : cto901A2 (e26)
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LOIc;
    FDS_copy(LOI, LOIc);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI;
    int nI = FUN_selectSKinterference(LOIc, TopOpeBRepDS_EDGE, LI);
    if (nI < 1)
      continue;

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newLI;
    bool                                                     hasnewLI = false;
    TopOpeBRepDS_TKI                                         tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      if (K != TopOpeBRepDS_POINT)
      {
        FDS_copy(loi, newLI);
        continue;
      }

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loicopy;
      FDS_assign(loi, loicopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lext;
      int  iext   = FUN_selectTRAORIinterference(loicopy, TopAbs_EXTERNAL, lext);
      bool hasext = (iext > 0);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lint;
      int  iint   = FUN_selectTRAORIinterference(loicopy, TopAbs_INTERNAL, lint);
      bool hasint = (iint > 0);
      if (!hasext && !hasint)
      {
        FDS_copy(loi, newLI);
        continue;
      }
      if (hasext && hasint)
      {
        FDS_copy(loi, newLI);
        continue;
      } // nyiFUN_RAISE : incoherent data
        // ------------------------------

      // NYI : only select among 2dI!!! ******************************
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lfor;
      int ifor = FUN_selectTRAORIinterference(loicopy, TopAbs_FORWARD, lfor);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lrev;
      int  irev   = FUN_selectTRAORIinterference(loicopy, TopAbs_REVERSED, lrev);
      bool hasrev = (irev > 0), hasfor = (ifor > 0);
      if (!hasrev && !hasfor)
      {
        FDS_copy(loi, newLI);
        continue;
      }
      if (hasrev && hasfor)
      {
        FDS_copy(loi, newLI);
        continue;
      } // nyiFUN_RAISE : incoherent data

      // newO :
      // -----
      occ::handle<TopOpeBRepDS_Interference> I2d   = hasfor ? lfor.First() : lrev.First();
      int                                    S2    = I2d->Support();
      int                                    iF2   = I2d->Transition().Index();
      const TopoDS_Edge&                     E2    = TopoDS::Edge(BDS.Shape(S2));
      double                                 parSE = FDS_Parameter(I2d);
      const TopoDS_Face&                     F2    = TopoDS::Face(BDS.Shape(iF2));
      double                                 parE2;
      bool                                   ok = FUN_tool_parE(SE, parSE, E2, parE2);
      if (!ok)
        return;
      gp_Pnt2d uv2;
      ok = FUN_tool_paronEF(E2, parE2, F2, uv2);
      if (!ok)
        return;
      gp_Dir ngF2 = FUN_tool_nggeomF(uv2, F2);
      gp_Dir xxF2;
      ok = FUN_tool_getxx(F2, E2, parE2, ngF2, xxF2);
      if (!ok)
        return;

      occ::handle<TopOpeBRepDS_Interference> I3d = hasext ? lext.First() : lint.First();
      int                                    iF3 = I3d->Transition().Index();
      const TopoDS_Face&                     F3  = TopoDS::Face(BDS.Shape(iF3));
      // recall : G is a point => E2 = edge(S3) (I3d : {(T,G,S3),(T,G,F3)})
      gp_Pnt2d uv3;
      ok = FUN_tool_paronEF(E2, parE2, F3, uv3);
      if (!ok)
        return;
      gp_Dir ngF3 = FUN_tool_nggeomF(uv3, F3);
      gp_Dir xxF3;
      ok = FUN_tool_getxx(F3, E2, parE2, ngF3, xxF3);
      if (!ok)
        return;

      double dot      = xxF2.Dot(xxF3);
      bool   positive = (dot > 0); // as F and FCX are tangent, dot= +1 or -1
      if (positive)
        continue;

      TopAbs_Orientation newO = hasfor ? TopAbs_FORWARD : TopAbs_REVERSED;
      if (hasint)
        newO = TopAbs::Complement(newO);

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it;
      if (hasint)
        it.Initialize(lint);
      else
        it.Initialize(lext);
      for (; it.More(); it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I    = it.Value();
        occ::handle<TopOpeBRepDS_Interference>        newI = I;
        newI->ChangeTransition().Set(newO);
        newLI.Append(I);
        hasnewLI = true;
      } // it
      newLI.Append(lrev);
      newLI.Append(lfor);
      newLI.Append(loicopy);
    } // tki
    if (hasnewLI)
    {
      newLI.Append(LOIc);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII =
        BDS.ChangeShapeInterferences(SE);
      LII.Clear();
      LII.Append(newLI);
    }
  } // i=1..nse
} // completeforSE5

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE6(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu280798 : - section edge E sdm3d Esd,
//             - E has I2d(T(F),VG,E'),
//             - VG vertex of Esd, VG not sdm
//             - Esd edge of F
//             => add newI1d(newT(Esd),VG,Esd)
//  cto902B4 (e15,v22)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();
  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge&             SE  = BDS.SectionEdge(i);
    int                            ISE = BDS.Shape(SE);
    NCollection_List<TopoDS_Shape> lEsd3d;
    bool                           hassd3d = FDS_HasSameDomain3d(BDS, SE, &lEsd3d);
    if (!hassd3d)
      continue;

    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(SE);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        LIcopy;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        LIa;
    FDS_assign(LI, LIcopy);
    int na = FUN_selectGKinterference(LIcopy, TopOpeBRepDS_VERTEX, LIa);
    if (na == 0)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIb;
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LIa); it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& Ia = it.Value();
      int                                           G  = Ia->Geometry();
      TopoDS_Shape                                  vGsd;
      bool                                          hassd = FUN_ds_getoov(BDS.Shape(G), HDS, vGsd);
      if (!hassd)
        LIb.Append(Ia);
    }

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2dFE;
    FDS_assign(LIb, LIcopy);
    FUN_ds_hasI2d(ISE, LIcopy, l2dFE);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1dE;
    FDS_assign(LIb, LIcopy);
    FUN_selectTRASHAinterference(LIcopy, TopAbs_EDGE, l1dE);

    // attached to SE : l1dE  = {I1d=(T(Esd),vG,Esd) / vG !hsd}
    //                  l2dFE = {I2dF=(T(F),vG,E) / vG !hsd}

    for (NCollection_List<TopoDS_Shape>::Iterator itsd3(lEsd3d); itsd3.More(); itsd3.Next())
    {
      const TopoDS_Edge& Esd = TopoDS::Edge(itsd3.Value());
      TopoDS_Vertex      vf, vl;
      TopExp::Vertices(Esd, vf, vl);
      bool degen = BRep_Tool::Degenerated(Esd);
      if (degen)
        continue;

      bool closed = vf.IsSame(vl);
      int  iEsd   = BDS.Shape(Esd);
      int  ivf    = BDS.Shape(vf);
      int  ivl    = BDS.Shape(vl);

      for (int iv = 1; iv <= 2; iv++)
      {
        int G = (iv == 1) ? ivf : ivl;
        if (G == 0)
          continue;
        const TopoDS_Vertex& vG = TopoDS::Vertex(BDS.Shape(G));
        TopoDS_Shape         vGsd;
        bool                 hassd = FUN_ds_getoov(vG, HDS, vGsd);
        if (hassd)
          continue;

        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1dG;
        FUN_selectGIinterference(l1dE, G, l1dG);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2dG;
        int n2dG = FUN_selectGIinterference(l2dFE, G, l2dG);
        if (n2dG == 0)
          continue; // no 2dI at G

        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1dGEsd;
        int n1dGEsd = FUN_selectITRASHAinterference(l1dG, iEsd, l1dGEsd);
        if (n1dGEsd != 0)
          continue; // 1dI(Esd) at G exists already

        for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2d(l2dG);
             it2d.More();
             it2d.Next())
        {
          const occ::handle<TopOpeBRepDS_Interference>& I2d     = it2d.Value();
          int                                           iTRASHA = I2d->Transition().IndexBefore();
          TopAbs_Orientation O = I2d->Transition().Orientation(TopAbs_IN);
          const TopoDS_Face& F = TopoDS::Face(BDS.Shape(iTRASHA));
          TopAbs_Orientation dum;
          bool               EsdofF = FUN_tool_orientEinF(Esd, F, dum);
          if (!EsdofF)
            continue;

          // we found I2d = (T(F),vG,E'), vG is vertex of Esd, vG !hsdm, Esd is edge of F
          // compute newI1d=(newT(Esd),vG,Esd)
          TopOpeBRepDS_Transition newT(TopAbs_OUT, TopAbs_OUT, TopAbs_EDGE, TopAbs_EDGE);
          double                  parE = FDS_Parameter(I2d);
          if (closed)
            newT.Set(TopAbs_INTERNAL);
          else
          {
            if (M_FORWARD(O) || M_REVERSED(O))
            {
              newT.Set(O);
            }
            else if (M_EXTERNAL(O))
            {
              newT.Set(O);
            }
            else if (M_INTERNAL(O))
            {
              double parEsd = BRep_Tool::Parameter(vG, Esd);
              gp_Vec tgEsd;
              TopOpeBRepTool_TOOL::TggeomE(parEsd, Esd, tgEsd); // dir
              gp_Vec tgE;
              TopOpeBRepTool_TOOL::TggeomE(parE, SE, tgE); // dir
              double dot    = tgEsd.Dot(tgE);
              bool   SO     = (dot > 0.);
              bool   isvf   = (iv == 1);
              bool   isforw = (SO && isvf) || (!SO && !isvf);
              if (isforw)
              {
                newT.Set(TopAbs_FORWARD);
              }
              else
              {
                newT.Set(TopAbs_REVERSED);
              }
            }
          }
          newT.Index(iEsd);
          occ::handle<TopOpeBRepDS_Interference> newI1d =
            MakeEPVInterference(newT, iEsd, G, parE, TopOpeBRepDS_VERTEX, false);
          HDS->StoreInterference(newI1d, SE);
        } // it2d(l2dFE)
      }
    } // itsd3

  } // i=1..nse
} // completeforSE6

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforE7(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu130898 : attached to edge E, lI3d = {I=(FOR/REV(F),G,F)}
//             reduction of lI3d if it describes INTERNAL/EXTERNAL transitions
// purpose : I3dFOR/REV => faulty spIN/OU of E on G
// CTS21199 (E6, G=v7/p3, FTR=f5)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         ns  = BDS.NbShapes();
  for (int i = 1; i <= ns; i++)
  {
    const TopoDS_Shape& EE = BDS.Shape(i);
    if (EE.ShapeType() != TopAbs_EDGE)
      continue;

    const TopoDS_Edge& E     = TopoDS::Edge(EE);
    bool               isdgE = BRep_Tool::Degenerated(E);
    if (isdgE)
      continue;
    BDS.Shape(E);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(E);

    TopOpeBRepDS_TKI tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K;
      int                                                             G;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        loicopy;
      FDS_assign(loi, loicopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1;
      FUN_selectSKinterference(loicopy, TopOpeBRepDS_FACE, l1);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lFOR;
      int nFOR = FUN_selectTRAORIinterference(l1, TopAbs_FORWARD, lFOR);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lREV;
      int nREV = FUN_selectTRAORIinterference(l1, TopAbs_REVERSED, lREV);
      if ((nFOR == 0) || (nREV == 0))
        continue;

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>           lnewI;
      int                                                                iFS = 0;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lFOR);
      for (; it.More(); it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>&            IFOR = it.Value();
        int                                                      IFS  = IFOR->Support();
        const TopoDS_Face&                                       FS = TopoDS::Face(BDS.Shape(IFS));
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lFS;
        int nFS = FUN_selectSIinterference(loicopy, IFS, lREV);
        if (nFS == 0)
          continue;

        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lFSE;
        int    nFSE    = FUN_selectITRASHAinterference(loicopy, IFS, lFSE);
        double par     = FDS_Parameter(IFOR);
        bool   isonper = false;
        double par1 = 0.0, par2 = 0.0;
        double factor = 1.e-4;
        FDS_LOIinfsup(BDS, E, par, K, G, BDS.ShapeInterferences(E), par1, par2, isonper);

        TopOpeBRepDS_Transition newT;
        // -------
        // we have IFOR=(FORWARD(FS),G,FS) + IREV=(REVERSED(FS),G,FS)
        int IES = 0;
        if (nFSE == 0)
        {
          gp_Pnt2d uvFS;
          bool     ok = FUN_tool_parF(E, par, FS, uvFS);
          if (!ok)
          {
            continue;
          }

          TopOpeBRepTool_makeTransition MKT;
          TopAbs_State                  stb = TopAbs_UNKNOWN, sta = TopAbs_UNKNOWN;
          ok = MKT.Initialize(E, par1, par2, par, FS, uvFS, factor);
          if (ok)
            ok = MKT.MkTonE(stb, sta);
          if (!ok)
          {
            continue;
          }
          newT.Before(stb);
          newT.After(sta);
        }
        else
        {
          IES                   = lFSE.First()->Support();
          const TopoDS_Edge& ES = TopoDS::Edge(BDS.Shape(IES));
          double             parES;
          bool               ok = FUN_tool_parE(E, par, ES, parES);
          if (!ok)
          {
            continue;
          }
          gp_Pnt2d uvFS;
          ok = FUN_tool_paronEF(ES, parES, FS, uvFS);
          if (!ok)
          {
            continue;
          }

          TopOpeBRepTool_makeTransition MKT;
          TopAbs_State                  stb = TopAbs_UNKNOWN, sta = TopAbs_UNKNOWN;
          ok = MKT.Initialize(E, par1, par2, par, FS, uvFS, factor);
          if (ok)
            ok = MKT.SetRest(ES, parES);
          if (ok)
            ok = MKT.MkTonE(stb, sta);
          if (!ok)
          {
            continue;
          }
          newT.Before(stb);
          newT.After(sta);
        }

        // newI :
        //------
        iFS = IFS;
        newT.Index(IFS);
        bool B = false;
        if (K == TopOpeBRepDS_VERTEX)
          B = occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(IFOR)->GBound();
        occ::handle<TopOpeBRepDS_Interference> newI =
          MakeEPVInterference(newT, IFS, G, par, K, TopOpeBRepDS_FACE, B);
        lnewI.Append(newI);
        if (nFSE != 0)
        {
          occ::handle<TopOpeBRepDS_Interference> newIFE =
            MakeEPVInterference(newT, IES, G, par, K, TopOpeBRepDS_EDGE, B);
          lnewI.Append(newIFE);
        }
      } // it(lFOR)

      if (iFS != 0)
      {
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loii = tki.ChangeValue(K, G);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>  lEFS;
        FUN_selectITRASHAinterference(loii, iFS, lEFS);
        for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator iti(lnewI);
             iti.More();
             iti.Next())
        {
          occ::handle<TopOpeBRepDS_Interference> newI = iti.Value();
          loii.Append(newI);
        }
      }
    } // tki

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII = BDS.ChangeShapeInterferences(E);
    LII.Clear();
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      FDS_copy(loi, LII);
    }
  } // i = 1..ns
} // completeforE7

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE8(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu020998 : Attached to section edge SE :
//  - {I1d=(Tr(Esd),vG,Esd), I2d=(Tr(F),vG,E)}
//  - E and Esd are F edges
//   I2d may be incomplete, "reduce" 2d/1d -> {I1d,newI2d}
// cto902A6 (ES5,Esd10,vG7,F35)
// prequesitory : A vertex is shared by at most 2 edges in the same face.
//                An edge is shared by at most 2 faces of same rank.
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();

  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge&                                              SE  = BDS.SectionEdge(i);
    int                                                             ISE = BDS.Shape(SE);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(SE);

    bool             hasnew = false;
    TopOpeBRepDS_TKI tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               KK;
      int                                                             GG;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(KK, GG);
      bool isvertex = (KK == TopOpeBRepDS_VERTEX);

      if (isvertex)
      {
        bool Ghsdm = HDS->HasSameDomain(BDS.Shape(GG));
        if (Ghsdm)
          continue;
      }
      // li -> l1dE + l2dFE + lFE(<=>l3dFE) + li(<=>lFF)
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> li;
      FDS_assign(loi, li);
      // clang-format off
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1dE;  int n1d = FUN_selectTRASHAinterference(li,TopAbs_EDGE,l1dE);  // li->l1dE+li(<=>lF?)
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lFE;
      FUN_selectSKinterference(li,TopOpeBRepDS_EDGE,lFE); // li(<=>lF?)->lFE+li(<=>lFF)
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2dFE; int n2d = FUN_selectpure2dI(li,lFE,l2dFE);                    // lFE->l2dFE+lFE(<=>l3dFE)
      // clang-format on
      bool redu2d = (n1d > 0) && (n2d > 0);
      // -------------------------------
      if (redu2d)
      { // {I1d=(Tr(Esd),vG,Esd), I2d=(Tr(F),vG,E)}
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(l2dFE);
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>           al2dFE;
        while (it.More())
        {
          occ::handle<TopOpeBRepDS_Interference>& I2dFE = it.ChangeValue();
          TopOpeBRepDS_Transition                 newT2d;
          bool ok = FUN_ds_redu2d1d(BDS, ISE, I2dFE, l1dE, newT2d);
          if (!ok)
          {
            it.Next();
            continue;
          }
          I2dFE->ChangeTransition() = newT2d;
          al2dFE.Append(I2dFE);
          l2dFE.Remove(it);
        }
        l2dFE.Append(al2dFE);
      } // redu2d

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loii = tki.ChangeValue(KK, GG);
      loii.Clear();
      loii.Append(l1dE);
      loii.Append(lFE);
      loii.Append(l2dFE);
      loii.Append(li);
    } // tki

    if (!hasnew)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII =
      BDS.ChangeShapeInterferences(SE);
    LII.Clear();
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                         KK  = TopOpeBRepDS_UNKNOWN;
      int                                                       GG  = 0;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.ChangeValue(KK, GG);
      LII.Append(loi);
    }
  } // i=1..nse
} // completeforSE8

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_completeforSE9(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu011098 : CTS21180(ES13)
// purpose : ES Section edge, sdm with EsdSE={Esd}, ES has no interference
//   compute interferences for ES.
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();

  for (int i = 1; i <= nse; i++)
  {
    const TopoDS_Edge& SE   = BDS.SectionEdge(i);
    int                rkSE = BDS.AncestorRank(SE);
    int                ISE  = BDS.Shape(SE);
    bool               hsd  = HDS->HasSameDomain(SE);
    if (!hsd)
      continue;
    if (!BDS.ShapeInterferences(SE).IsEmpty())
      continue;

    const NCollection_List<TopoDS_Shape>&    EsdSE = BDS.ShapeSameDomain(SE);
    NCollection_List<TopoDS_Shape>::Iterator ite(EsdSE);
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Edge& Esd   = TopoDS::Edge(ite.Value());
      int                iEsd  = BDS.Shape(Esd);
      int                rkEsd = BDS.AncestorRank(Esd);
      if (rkEsd == rkSE)
        continue;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI =
        BDS.ShapeInterferences(Esd);
      if (LI.IsEmpty())
        continue;

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIcopy;
      FDS_assign(LI, LIcopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LISE;
      FUN_selectSIinterference(LIcopy, ISE, LISE);

      TopOpeBRepDS_TKI tki;
      tki.FillOnGeometry(LISE);
      for (tki.Init(); tki.More(); tki.Next())
      {
        TopOpeBRepDS_Kind K = TopOpeBRepDS_UNKNOWN;
        int               G = 0;
        //        const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi =
        tki.Value(K, G);
        const TopoDS_Vertex& vG   = TopoDS::Vertex(BDS.Shape(G));
        bool                 hsd1 = HDS->HasSameDomain(vG);
        if (hsd1)
          continue; // nyixpu011098
        int rkG = BDS.AncestorRank(G);
        if (rkG != rkSE)
          continue; // nyixpu011098

        // newI :
        // -----
        TopOpeBRepDS_Transition newT(TopAbs_IN, TopAbs_IN, TopAbs_EDGE, TopAbs_EDGE);
        newT.Index(iEsd);
        // modified by NIZHNY-MKK  Mon Apr  2 15:39:14 2001.BEGIN
        int hasvG = FUN_tool_orientVinE(vG, SE);
        if (hasvG == 0)
        {
          continue;
        }
        // modified by NIZHNY-MKK  Mon Apr  2 15:39:17 2001.END

        double                                 par = BRep_Tool::Parameter(vG, SE);
        occ::handle<TopOpeBRepDS_Interference> newI =
          MakeEPVInterference(newT, iEsd, G, par, TopOpeBRepDS_VERTEX, TopOpeBRepDS_EDGE, true);
        HDS->StoreInterference(newI, SE);
      }
    } // ite(EsdSE)
  } // i=1..nse
} // completeforSE9

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_PointToVertex(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu090698 : for gap reasons, intersector IntPatch can find a intersection
// point POINT, whereas it should be a VERTEX,
// if we find edge eb / eb : EPI(T1,G,ea1),
//                           EPI(T2,G,a2) with rk(ea1) = rk(ea2),
// then G should be VERTEX (2 edges of same shape cannot
// intersect) - recall : in the DS, we do not bind twice the same closing edge-
// CTS20339
{
  int                           i;
  TopOpeBRepDS_DataStructure&   BDS = HDS->ChangeDS();
  int                           ns  = BDS.NbShapes();
  NCollection_DataMap<int, int> iPiV;

  for (i = 1; i <= ns; i++)
  {
    const TopoDS_Shape& s = BDS.Shape(i);
    if (s.ShapeType() != TopAbs_EDGE)
      continue;
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(s);
    if (LI.IsEmpty())
      continue;

    TopOpeBRepDS_TKI tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      if (K == TopOpeBRepDS_VERTEX)
        continue;

      int  Scur    = 0;
      bool Gfaulty = false;
      for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loi); it.More();
           it.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
        TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
        int               G1 = 0, S = 0;
        FDS_data(I, GT, G1, ST, S);
        if (ST != TopOpeBRepDS_EDGE)
          continue;
        if (Scur == 0)
        {
          Scur = S;
        }
        if (S != Scur)
        {
          Gfaulty = true;
          break;
        }
      } // it
      if (Gfaulty)
      {
        // in DS : pG --> vG
        gp_Pnt             pG   = BDS.Point(G).Point();
        int                rkES = BDS.AncestorRank(Scur);
        const TopoDS_Edge& ES   = TopoDS::Edge(BDS.Shape(Scur));
        TopoDS_Vertex      vf, vl;
        TopExp::Vertices(ES, vf, vl);
        gp_Pnt        pf = BRep_Tool::Pnt(vf);
        gp_Pnt        pl = BRep_Tool::Pnt(vl);
        double        df = pf.Distance(pG);
        double        dl = pl.Distance(pG);
        TopoDS_Vertex vG;
        if (df < dl)
          vG = vf;
        else
          vG = vl;

        int ivG = BDS.AddShape(vG, rkES);
        iPiV.Bind(G, ivG);
      } // Gfaulty
    } // tki
  } // i

  if (iPiV.IsEmpty())
    return;
  for (i = 1; i <= ns; i++)
  {
    const TopoDS_Shape& s = BDS.Shape(i);
    if (s.ShapeType() != TopAbs_EDGE)
      continue;
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(s);
    if (LI.IsEmpty())
      continue;

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newLI;
    int                                                      rks = BDS.AncestorRank(s);
    TopOpeBRepDS_TKI                                         tki;
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      bool                                                            Gisbound = iPiV.IsBound(G);
      if (!Gisbound)
      {
        FDS_copy(loi, newLI);
        continue;
      }

      int                  ivG    = iPiV.Find(G);
      const TopoDS_Vertex& vG     = TopoDS::Vertex(BDS.Shape(ivG));
      int                  rkG    = BDS.AncestorRank(ivG);
      bool                 Gbound = (rkG == rks);

      for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itl(loi); itl.More();
           itl.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>&    I = itl.Value();
        occ::handle<TopOpeBRepDS_CurvePointInterference> CPI(
          occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I));
        if (CPI.IsNull())
          continue;

        double            par = CPI->Parameter();
        TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
        int               G1 = 0, S = 0;
        FDS_data(I, GT, G1, ST, S);
        const TopOpeBRepDS_Transition& T     = I->Transition();
        double                         parvG = 0.0;
        bool                           ok    = FUN_tool_parVonE(vG, TopoDS::Edge(s), parvG);
        // modified by NIZHNY-MKK  Mon Apr  2 15:39:59 2001.BEGIN
        // 	if (!ok) par = parvG;
        if (!ok)
          continue;
        par = parvG;
        // modified by NIZHNY-MKK  Mon Apr  2 15:40:04 2001.END
        occ::handle<TopOpeBRepDS_Interference> newI =
          MakeEPVInterference(T, S, ivG, par, TopOpeBRepDS_VERTEX, ST, Gbound);
        newLI.Append(newI);
      }
    } // tki
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII = BDS.ChangeShapeInterferences(s);
    LII.Clear();
    LII.Append(newLI);
  } // i

  NCollection_DataMap<int, int>::Iterator itm(iPiV);
  for (; itm.More(); itm.Next())
  {
    int G = itm.Key();
    BDS.RemovePoint(G);
  }
} // PointToVertex

static bool FUN_redusamshaonE(const TopOpeBRepDS_DataStructure&             BDS,
                              const occ::handle<TopOpeBRepDS_Interference>& I,
                              const int                                     EIX,
                              occ::handle<TopOpeBRepDS_Interference>&       newI)
// attached to edge(EIX) : IFOR=(FORWARD(ES),G,ES) + IREV=(REVERSED(ES),G,ES)
{
  newI.Nullify();
  TopAbs_ShapeEnum  SB = TopAbs_SHAPE, SA = TopAbs_SHAPE;
  int               IB = 0, IA = 0;
  TopOpeBRepDS_Kind GT = TopOpeBRepDS_UNKNOWN, ST = TopOpeBRepDS_UNKNOWN;
  int               G = 0, S = 0;
  FDS_Idata(I, SB, IB, SA, IA, GT, G, ST, S);
  const TopoDS_Edge& E    = TopoDS::Edge(BDS.Shape(EIX));
  double             parE = FDS_Parameter(I);
  double             f, l;
  FUN_tool_bounds(E, f, l);
  const TopoDS_Edge& ES    = TopoDS::Edge(BDS.Shape(S));
  const TopoDS_Face& FTRA  = TopoDS::Face(BDS.Shape(IB));
  double             parES = 0.0;
  bool               ok    = FUN_tool_parE(E, parE, ES, parES);
  if (!ok)
    return false;
  gp_Pnt2d uv;
  ok = FUN_tool_paronEF(ES, parES, FTRA, uv);
  if (!ok)
    return false;

  double                        factor = 1.e-2;
  TopAbs_State                  stb = TopAbs_UNKNOWN, sta = TopAbs_UNKNOWN;
  TopOpeBRepTool_makeTransition MKT;
  ok = MKT.Initialize(E, f, l, parE, FTRA, uv, factor);
  if (ok)
    ok = MKT.SetRest(ES, parES);
  if (ok)
    ok = MKT.MkTonE(stb, sta);
  if (!ok)
    return false;
  TopOpeBRepDS_Transition newT;
  newT.Index(IB);
  newT.Before(stb);
  newT.After(sta);

  ok = FDS_stateEwithF2d(BDS, E, parE, GT, G, FTRA, newT);
  if (!ok)
    return false;

  bool B = false;
  if (GT == TopOpeBRepDS_VERTEX)
    B = occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->GBound();
  newI = MakeEPVInterference(newT, S, G, parE, GT, TopOpeBRepDS_EDGE, B);
  return true;
} // FUN_redusamshaonE

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_redusamsha(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// xpu201098 : reduce 2d interferences on same G and same S :
//   {I1=(OU/IN(F),G,ES), I2=(IN/OU(F),G,ES)}
// cto009K1 (SE7,ES8,G2,FTRA27)
// xpu050299 (FRA60618 e13,ftrasha14)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         ns  = BDS.NbShapes();

  for (int i = 1; i <= ns; i++)
  {
    if (BDS.Shape(i).ShapeType() != TopAbs_EDGE)
      continue;

    const TopoDS_Edge&                                              E  = TopoDS::Edge(BDS.Shape(i));
    int                                                             IE = BDS.Shape(E);
    TopOpeBRepDS_TKI                                                tki;
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(i);
    tki.FillOnGeometry(LI);
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                               K   = TopOpeBRepDS_UNKNOWN;
      int                                                             G   = 0;
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K, G);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        loicopy;
      FDS_copy(loi, loicopy); // loi -> l1+l2
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l0;
      FUN_selectTRASHAinterference(loicopy, TopAbs_EDGE, l0); // xpu091198(cylcong)
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1;
      int nfound = FUN_selectTRASHAinterference(loicopy, TopAbs_FACE, l1);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l2;
      nfound = FUN_selectSKinterference(l1, TopOpeBRepDS_EDGE, l2);
      // l2 = {I=(T(faceTRASHA),G,Sedge)}
      if (nfound == 0)
        continue;

      //***
      TopOpeBRepDS_TKI tkis;
      tkis.FillOnSupport(l2);
      for (tkis.Init(); tkis.More(); tkis.Next())
      {
        TopOpeBRepDS_Kind                                         k   = TopOpeBRepDS_UNKNOWN;
        int                                                       s   = 0;
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& li  = tkis.ChangeValue(k, s);
        int                                                       nli = li.Extent();
        if (nli < 2)
          continue;

        occ::handle<TopOpeBRepDS_Interference>                             newI;
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(li);
        while (it1.More())
        {
          const occ::handle<TopOpeBRepDS_Interference>& I1 = it1.Value();
          const TopOpeBRepDS_Transition&                T1 = I1->Transition();
          TopAbs_Orientation                            O1 = T1.Orientation(TopAbs_IN);
          if (!M_FORWARD(O1) && !M_REVERSED(O1))
          {
            it1.Next();
            continue;
          }
          TopAbs_ShapeEnum  SB1 = TopAbs_SHAPE, SA1 = TopAbs_SHAPE;
          int               IB1 = 0, IA1 = 0;
          TopOpeBRepDS_Kind GT1 = TopOpeBRepDS_UNKNOWN, ST1 = TopOpeBRepDS_UNKNOWN;
          int               G1 = 0, S1 = 0;
          FDS_Idata(I1, SB1, IB1, SA1, IA1, GT1, G1, ST1, S1);
          if (IB1 != IA1)
          {
            it1.Next();
            continue;
          }

          bool                                                               oppofound = false;
          NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
          it2.Next();
          while (it2.More())
          {
            const occ::handle<TopOpeBRepDS_Interference>& I2  = it2.Value();
            const TopOpeBRepDS_Transition&                T2  = I2->Transition();
            TopAbs_Orientation                            O2  = T2.Orientation(TopAbs_IN);
            TopAbs_ShapeEnum                              SB2 = TopAbs_SHAPE, SA2 = TopAbs_SHAPE;
            int                                           IB2 = 0, IA2 = 0;
            TopOpeBRepDS_Kind GT2 = TopOpeBRepDS_UNKNOWN, ST2 = TopOpeBRepDS_UNKNOWN;
            int               G2 = 0, S2 = 0;
            FDS_Idata(I2, SB2, IB2, SA2, IA2, GT2, G2, ST2, S2);
            if (IB2 != IA2)
            {
              it2.Next();
              continue;
            }
            if (IB1 != IB2)
            {
              it2.Next();
              continue;
            } // same fTRASHA
            if (S1 != S2)
            {
              it2.Next();
              continue;
            } // same Sedge

            if (O1 == TopAbs::Complement(O2))
            {
              oppofound = true;
              break;
            }
            else
            {
              it2.Next();
              continue;
            }
          } // it2(it1)

          if (!oppofound)
          {
            it1.Next();
            continue;
          }
          bool ok = FUN_redusamshaonE(BDS, I1, IE, newI);
          if (!ok)
          {
            it1.Next();
            continue;
          }
          else
            break;
        } // it1(li)
        if (newI.IsNull())
          continue;
        li.Clear();
        li.Append(newI);
      } // tkis(l2)
      //***

      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& newloi = tki.ChangeValue(K, G);
      newloi.Clear();
      for (tkis.Init(); tkis.More(); tkis.Next())
      {
        TopOpeBRepDS_Kind                                         k  = TopOpeBRepDS_UNKNOWN;
        int                                                       g  = 0;
        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& li = tkis.ChangeValue(k, g);
        newloi.Append(li);
      }
      newloi.Append(l0);
      newloi.Append(l1);
    } // tki(LI)

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& newLI =
      BDS.ChangeShapeInterferences(E);
    newLI.Clear();
    for (tki.Init(); tki.More(); tki.Next())
    {
      TopOpeBRepDS_Kind                                         KK  = TopOpeBRepDS_UNKNOWN;
      int                                                       GG  = 0;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.ChangeValue(KK, GG);
      newLI.Append(loi);
    }
  } // i=1..nse
} // FUN_ds_redusamsha

Standard_EXPORT bool FUN_ds_hasFEI(const TopOpeBRepDS_PDataStructure& pDS2d,
                                   const TopoDS_Shape&                F,
                                   const int                          GI,
                                   const int                          ITRA)
{
  bool hasF = pDS2d->HasShape(F);
  if (!hasF)
    return false;

  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = pDS2d->ShapeInterferences(F);
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
       it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    const TopOpeBRepDS_Transition&                T = I->Transition();
    TopOpeBRepDS_Kind                             GT, ST;
    int                                           G, S;
    FDS_data(I, GT, G, ST, S);
    bool found = (G == GI) && (T.Index() == ITRA);
    if (found)
      return true;
  }
  return false;
} // FUN_ds_hasFEI

Standard_EXPORT bool FUN_ds_ONesd(const TopOpeBRepDS_DataStructure& BDS,
                                  const int                         IE,
                                  const TopoDS_Shape&               EspON,
                                  int&                              IEsd)
{
  const TopoDS_Shape&                      E = BDS.Shape(IE);
  NCollection_List<TopoDS_Shape>::Iterator it(BDS.ShapeSameDomain(E));
  double                                   f = 0.0, l = 0.0;
  FUN_tool_bounds(TopoDS::Edge(EspON), f, l);
  double x   = 0.456789;
  double par = (1 - x) * f + x * l;
  gp_Pnt p3d;
  bool   ok = FUN_tool_value(par, TopoDS::Edge(EspON), p3d);
  if (!ok)
    return false;

  for (; it.More(); it.Next())
  {
    const TopoDS_Edge& esd = TopoDS::Edge(it.Value());
    double             d   = 0., parp;
    ok                     = FUN_tool_projPonE(p3d, esd, parp, d);
    if (!ok)
      continue;
    double tolesd = BRep_Tool::Tolerance(esd);
    ok            = (d < tolesd * 1.e3); // nyi checktole
    if (!ok)
      continue;
    IEsd = BDS.Shape(esd);
    return true;
  }
  return false;
} // FUN_ds_ONesd

/*#define FIRST (1)
#define LAST  (2)
static bool FUN_oriTOsta(const int o, const TopAbs_State sta,
TopAbs_Orientation& ori)
{
  bool ok = (sta == TopAbs_OUT) || (sta == TopAbs_IN);
  if (!ok) return false;
  ok = (o == FIRST) || (o == LAST);
  if (!ok) return false;

  ori = TopAbs_EXTERNAL;
  if (o == FIRST) ori = TopAbs_FORWARD;
  if (o == LAST)  ori = TopAbs_REVERSED;
  if (sta == TopAbs_OUT) ori = TopAbs::Complement(ori);
  return true;
} //FUN_oriTOsta

Standard_EXPORT void FUN_ds_unkeepEVIonGb1(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
const TopAbs_State sta)
// xpu290698 :
// Completing filter process for edges : delete of interferences
// EVI on gbound1, we keep only interferences on Gb1 describing transitions
// on e / e is IN the solid (if sta = IN)
//        e is OU the solid (if sta = OU)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int ns = BDS.NbShapes();
  for (int i = 1; i <= ns; i++) {
    const TopoDS_Shape& s = BDS.Shape(i);
    if (s.ShapeType() != TopAbs_EDGE) continue;

    const TopoDS_Edge& E = TopoDS::Edge(s);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ChangeShapeInterferences(s);
    if (LI.IsEmpty()) continue;

    bool dgE = BRep_Tool::Degenerated(E);
    if (dgE) continue;
    TopoDS_Vertex dum; bool closed = FUN_tool_ClosedE(E,dum);
    if (closed) continue;

    TopOpeBRepDS_Kind K; int G; bool ok = false;
    TopOpeBRepDS_TKI tki; tki.FillOnGeometry(LI);

    for (tki.Init(); tki.More(); tki.Next()) {
      tki.Value(K,G);
      if (K != TopOpeBRepDS_VERTEX) break;
      ok = true;
    } // tki
    if (!ok) continue;

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newLI;
    for (tki.Init(); tki.More(); tki.Next()) {
      tki.Value(K,G);
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.Value(K,G);

      const TopoDS_Vertex& V = TopoDS::Vertex(BDS.Shape(G));
      int o = FUN_tool_orientVinE(V,E);
      if (o == 0) {FDS_copy(loi,newLI); continue;}
      TopAbs_Orientation ori; bool okk = FUN_oriTOsta(o,sta,ori);
      if (!okk)   {FDS_copy(loi,newLI); continue;}

      // attached to E at G : loi = {I} / I is EVIonGb1
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newloi;
      for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loi); it.More();
it.Next()) { const occ::handle<TopOpeBRepDS_Interference>& I = it.Value(); const TopAbs_Orientation&
O = I->Transition().Orientation(TopAbs_IN); if (O == ori) { newloi.Append(I); continue;
    }
      }
      int nnewI = newloi.Extent();
      int nI = loi.Extent();
      bool ko = (nnewI > 1) && (nnewI != nI); // complex transitions at G
                                             // NYI : correcting reducing cto 001 W3 (e35)
      if (!ko) newLI.Append(newloi);
//      newLI.Append(newloi); //xpu280898 : cto905J1 (e25,v8)
    } // tki
    LI.Clear(); LI.Append(newLI);

  } // i=1..ns
} //FUN_ds_unkeepEVIonGb1*/

Standard_EXPORT bool FDS_LOIinfsup(
  const TopOpeBRepDS_DataStructure&,
  const TopoDS_Edge&                                              E,
  const double                                                    pE,
  const TopOpeBRepDS_Kind                                         KDS,
  const int                                                       GDS,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI,
  double&                                                         pbef,
  double&                                                         paft,
  bool&                                                           isonboundper)
{
  double f = 0.0, l = 0.0;
  FUN_tool_bounds(E, f, l);
  pbef  = f;
  paft  = l;
  int n = LOI.Extent();
  if (n == 0)
    return true;

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LOIsansGDS;
  TopOpeBRepDS_TKI                                         tki;
  tki.FillOnGeometry(LOI);

  for (tki.Init(); tki.More(); tki.Next())
  {
    TopOpeBRepDS_Kind K;
    int               G;
    tki.Value(K, G);
    bool PV = (K == TopOpeBRepDS_POINT) || (K == TopOpeBRepDS_VERTEX);
    if (!PV)
      continue;
    bool mk  = (K == KDS);
    bool mg  = (G == GDS);
    bool mkg = (mk && mg);
    if (mkg)
      continue;

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.ChangeValue(K, G);

    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loi); it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      TopOpeBRepDS_Kind                             GT1, ST1;
      int                                           G1, S1;
      TopAbs_ShapeEnum                              tsb1, tsa1;
      int                                           isb1, isa1;
      FDS_Idata(I, tsb1, isb1, tsa1, isa1, GT1, G1, ST1, S1);
      if (tsb1 != TopAbs_FACE)
        continue;
      if (tsa1 != TopAbs_FACE)
        continue;
      occ::handle<TopOpeBRepDS_CurvePointInterference> cpi(
        occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I));
      occ::handle<TopOpeBRepDS_EdgeVertexInterference> evi(
        occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I));
      if (cpi.IsNull() && evi.IsNull())
        continue;
      LOIsansGDS.Append(I);
      break;
    }
  }

  n = LOIsansGDS.Extent();
  if (n == 0)
    return true;

  TopoDS_Vertex v;
  bool          Eclosed = TopOpeBRepTool_TOOL::ClosedE(E, v);
  double        tole    = BRep_Tool::Tolerance(E);
  double        tol     = Precision::Parametric(tole);
  isonboundper          = false;
  if (Eclosed)
  {
    double tolv = BRep_Tool::Tolerance(v);
    tolv        = Precision::Parametric(tolv);
    if (tolv > tol)
      tol = tolv;
    bool pEisEf  = (std::abs(pE - f) <= tol);
    bool pEisEl  = (std::abs(pE - l) <= tol);
    isonboundper = pEisEf || pEisEl;
  }

  if (isonboundper)
  {
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LOIsansGDS);
         it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      double                                        p = FDS_Parameter(I);
      // pbef > paft
      if (p > pbef)
        pbef = p;
      if (p < paft)
        paft = p;
    }
    return true;
  }
  else
  {
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LOIsansGDS);
         it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      double                                        p = FDS_Parameter(I);
      if (p > pbef && p < pE)
        pbef = p;
      if (p < paft && p > pE)
        paft = p;
    }
  }
  return true;
} // FDS_LOIinfsup

// sur arete E : pbef < pE < paft, calcul de p1,t2 / :
// p1 dans [pbef,pE[
// p2 dans ]pE,paft]
// si isonboundper = T, les bornes pbef et paft sont prises sur les bornes de E.
Standard_EXPORT bool FDS_parbefaft(
  //                                    const TopOpeBRepDS_DataStructure& BDS,
  const TopOpeBRepDS_DataStructure&,
  const TopoDS_Edge& E,
  const double       pE,
  const double&      pbef,
  const double&      paft,
  const bool&        isonboundper,
  double&            p1,
  double&            p2)
{
  double t = 0.3178947713;
  double f, l;
  FUN_tool_bounds(E, f, l);
  if (isonboundper)
  {
    p1 = (1 - t) * pbef + t * l;
    p2 = (1 - t) * f + t * paft;
  }
  else
  {
    p1 = (1 - t) * pbef + t * pE;
    p2 = (1 - t) * pE + t * paft;
  }
  return true;
} // FDS_parbefaft

Standard_EXPORT bool FDS_stateEwithF2d(const TopOpeBRepDS_DataStructure& BDS,
                                       const TopoDS_Edge&                E,
                                       const double                      pE,
                                       const TopOpeBRepDS_Kind           KDS,
                                       const int                         GDS,
                                       const TopoDS_Face&                F1,
                                       TopOpeBRepDS_Transition&          TrmemeS)
{
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI  = BDS.ShapeInterferences(E);
  double                                                          pbef = 0.0, paft = 0.0;
  bool                                                            isonper = false;
  bool ok = FDS_LOIinfsup(BDS, E, pE, KDS, GDS, LOI, pbef, paft, isonper);
  if (!ok)
    return false;
  double t1 = 0.0, t2 = 0.0;
  ok = FDS_parbefaft(BDS, E, pE, pbef, paft, isonper, t1, t2);
  gp_Pnt P1;
  bool   ok1 = FUN_tool_value(t1, E, P1);
  gp_Pnt P2;
  bool   ok2 = FUN_tool_value(t2, E, P2);
  if (!ok1 || !ok2)
    return false;

  TopOpeBRepTool_ShapeClassifier& PSC  = FSC_GetPSC(F1);
  TopAbs_State                    sta1 = FSC_StatePonFace(P1, F1, PSC);
  TopAbs_State                    sta2 = FSC_StatePonFace(P2, F1, PSC);

  // xpu190898 : cto014I2 (e5,fTRASHA14,vG7,eS10)
  if (sta1 == TopAbs_ON)
    sta1 = TopAbs_IN;
  if (sta2 == TopAbs_ON)
    sta2 = TopAbs_IN;

  TrmemeS.Before(sta1, TopAbs_FACE);
  TrmemeS.After(sta2, TopAbs_FACE);
  return true;
} // FDS_stateEwithF2d

#define SAMEORIENTED (1)
#define DIFFORIENTED (2)

Standard_EXPORT void FUN_ds_FEIGb1TO0(occ::handle<TopOpeBRepDS_HDataStructure>&           HDS,
                                      const NCollection_DataMap<TopoDS_Shape,
                                                                TopOpeBRepDS_ListOfShapeOn1State,
                                                                TopTools_ShapeMapHasher>& MEspON)
{
  // xpu250199 :  F has FEI(Gb1,  EG, FS), EG sdm Esd
  //             -> to prevent from the loss of information, replace interference with
  //                    FEI'(Gb0, EGsd, FS)
  // CTS21754, f26 has FEI(Gb1,EG=e13, FS=f16), EG13 sd Esd11

  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         ns  = BDS.NbShapes();
  for (int I = 1; I <= ns; I++)
  {

    const TopoDS_Shape& F = BDS.Shape(I);
    if (F.ShapeType() != TopAbs_FACE)
      continue;
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = BDS.ShapeInterferences(F);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>
      LGb1; // LI = LII + LGb1 (= {FEI with GB1})
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LII;
    FDS_copy(LI, LII);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LII);
    while (it.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>&  Intf = it.Value();
      occ::handle<TopOpeBRepDS_FaceEdgeInterference> FEI =
        occ::down_cast<TopOpeBRepDS_FaceEdgeInterference>(Intf);
      if (FEI.IsNull())
      {
        it.Next();
        continue;
      }
      bool GB = FEI->GBound();
      if (GB != 1)
      {
        it.Next();
        continue;
      }
      LGb1.Append(Intf);
      LII.Remove(it);
    } // it(LII)
    int nGb1 = LGb1.Extent();
    if (nGb1 == 0)
      continue;

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>
      LGb0; // LGb1={FEIGb1(EG,FS)} -> LGb0={FEIGb0(Esd,FS)}
    it.Initialize(LGb1);
    while (it.More())
    {
      occ::handle<TopOpeBRepDS_FaceEdgeInterference> IGb1 =
        occ::down_cast<TopOpeBRepDS_FaceEdgeInterference>(it.Value());
      int                S  = IGb1->Support();
      const TopoDS_Face& FS = TopoDS::Face(BDS.Shape(S));
      int                G  = IGb1->Geometry();
      int                Gsd;
      bool               findSD = TopOpeBRepDS_TOOL::GetEsd(HDS, FS, G, Gsd);
      if (!findSD)
      {
        it.Next();
        continue;
      }

      // IGb0 is already stored in the list of interferences :
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itt(LII);
      bool                                                               stored = false;
      for (; itt.More(); itt.Next())
      {
        const occ::handle<TopOpeBRepDS_Interference>& II = itt.Value();
        bool isfound = (II->Support() == S) && (II->Geometry() == Gsd);
        if (isfound)
        {
          stored = true;
          break;
        }
      } // itt(LII)
      if (stored)
      {
        LGb1.Remove(it);
        continue;
      }

      TopOpeBRepDS_Transition newT = IGb1->Transition();
      TopAbs_Orientation      newO = newT.Orientation(TopAbs_IN);
      if (M_INTERNAL(newO) || M_EXTERNAL(newO))
      {
        // nothing's done
      }
      else
      {
        int  conf = 0;
        bool ok   = TopOpeBRepDS_TOOL::GetConfig(HDS, MEspON, G, Gsd, conf);
        if (!ok)
        {
          it.Next();
          continue;
        }
        if (conf == DIFFORIENTED)
          newO = TopAbs::Complement(newO);
      }
      newT.Set(newO);
      IGb1->SetGeometry(Gsd);
      IGb1->SetGBound(false);
      IGb1->Transition(newT);
      LGb0.Append(IGb1);
      LGb1.Remove(it);
    } // it(LGb1)

    if (LGb0.IsEmpty())
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& newLI =
      BDS.ChangeShapeInterferences(F);
    newLI.Clear();
    newLI.Append(LII);
    newLI.Append(LGb1);
    newLI.Append(LGb0);
  } // i=1..ns
} // FUN_ds_FEIGb1TO0

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_complete1dForSESDM(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
// ----------------------------------------------------------------------
// MSV 25.03.2002 : OCC251
// purpose : SE Section edge, SDM with Esd, SE has no interference
//   for one of vertices of Esd while should.
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         nse = BDS.NbSectionEdges();

  for (int is = 1; is <= nse; is++)
  {
    const TopoDS_Edge& SE = BDS.SectionEdge(is);
    if (BRep_Tool::Degenerated(SE))
      continue;
    int  rkSE = BDS.AncestorRank(SE);
    int  iSE  = BDS.Shape(SE);
    bool hsd  = HDS->HasSameDomain(SE);
    if (!hsd)
      continue;
    const NCollection_List<TopoDS_Shape>& LEsd = BDS.ShapeSameDomain(SE);
    if (LEsd.IsEmpty())
      continue;

    double tolSE = BRep_Tool::Tolerance(SE);
    // map of vertices of SE or same domain with them
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aGBMap;
    TopoDS_Vertex                                          VSE[2];
    TopExp::Vertices(SE, VSE[0], VSE[1]);
    int i;
    for (i = 0; i < 2; i++)
      if (!VSE[i].IsNull())
      {
        aGBMap.Add(VSE[i]);
        const NCollection_List<TopoDS_Shape>&    LV = BDS.ShapeSameDomain(VSE[i]);
        NCollection_List<TopoDS_Shape>::Iterator it(LV);
        for (; it.More(); it.Next())
          aGBMap.Add(it.Value());
      }

    NCollection_List<TopoDS_Shape>::Iterator ite(LEsd);
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Edge& Esd   = TopoDS::Edge(ite.Value());
      int                iEsd  = BDS.Shape(Esd);
      int                rkEsd = BDS.AncestorRank(Esd);
      if (rkEsd == rkSE)
        continue;

      if (BRep_Tool::Degenerated(Esd))
        continue;
      bool isSO = false;
      bool ok   = FUN_tool_curvesSO(Esd, SE, isSO);
      if (!ok)
        continue;

      double tolEsd = std::max(BRep_Tool::Tolerance(Esd), tolSE);
      // prepare the list of interferences of SE with Esd
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIall =
        BDS.ShapeInterferences(iSE);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI, LI1;
      FDS_assign(LIall, LI);
      int ni = FUN_selectTRAUNKinterference(LI, LI1);
      LI1.Clear();
      ni = FUN_selectTRASHAinterference(LI, TopAbs_EDGE, LI1);
      LI.Clear();
      ni = FUN_selectITRASHAinterference(LI1, iEsd, LI);
      LI1.Clear();
      ni = FUN_selectSKinterference(LI, TopOpeBRepDS_EDGE, LI1);
      LI.Clear();
      ni = FUN_selectSIinterference(LI1, iEsd, LI);
      LI1.Clear();
      ni = FUN_selectGKinterference(LI, TopOpeBRepDS_VERTEX, LI1);
      LI.Clear();
      LI.Append(LI1);

      // process vertices of Esd
      TopoDS_Vertex Vsd[2];
      TopExp::Vertices(Esd, Vsd[0], Vsd[1]);
      for (i = 0; i < 2; i++)
      {
        const TopoDS_Vertex& aV = Vsd[i];
        if (aV.IsNull())
          continue;
        // do not create interferences on GBound
        if (aGBMap.Contains(aV))
          continue;

        TopAbs_Orientation ori = aV.Orientation();
        if (!isSO)
          ori = TopAbs::Reverse(ori);

        // check that SE has no yet interference with geometry aV oriented ori
        if (ni)
        {
          NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI2;
          FDS_assign(LI, LI1);
          int nio = FUN_selectTRAORIinterference(LI1, ori, LI2);
          if (nio)
          {
            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aVGMap;
            aVGMap.Add(aV);
            const NCollection_List<TopoDS_Shape>&    LV = BDS.ShapeSameDomain(aV);
            NCollection_List<TopoDS_Shape>::Iterator it(LV);
            for (; it.More(); it.Next())
              aVGMap.Add(it.Value());
            NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator iti(LI2);
            for (; iti.More(); iti.Next())
            {
              const occ::handle<TopOpeBRepDS_Interference>& I1  = iti.Value();
              int                                           gi  = I1->Geometry();
              const TopoDS_Shape&                           aVG = BDS.Shape(gi);
              if (aVGMap.Contains(aVG))
                break;
            }
            if (iti.More())
              continue; // has already
          }
        }

        // make new interference
        double par    = 0.0;
        double tol    = std::max(BRep_Tool::Tolerance(aV), tolEsd);
        double parEsd = BRep_Tool::Parameter(aV, Esd);
        ok            = FUN_tool_parE(Esd, parEsd, SE, par, tol);
        if (!ok)
          continue;
        TopOpeBRepDS_Transition aT(ori);
        aT.ShapeBefore(TopAbs_EDGE);
        aT.ShapeAfter(TopAbs_EDGE);
        aT.Index(iEsd);
        int                 iV       = BDS.AddShape(aV, rkEsd);
        bool                isGBound = false;
        TopOpeBRepDS_Config aConfig =
          (isSO ? TopOpeBRepDS_SAMEORIENTED : TopOpeBRepDS_DIFFORIENTED);
        occ::handle<TopOpeBRepDS_Interference> aI =
          TopOpeBRepDS_InterferenceTool::MakeEdgeVertexInterference(aT,
                                                                    iEsd,
                                                                    iV,
                                                                    isGBound,
                                                                    aConfig,
                                                                    par);
        HDS->StoreInterference(aI, SE);
      }
    }
  }
}
