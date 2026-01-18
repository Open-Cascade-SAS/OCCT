// Created on: 1996-03-07
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

#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_Intersector.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_Tools.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef OCCT_DEBUG
  #define DEBSHASET(sarg, meth, shaset, str)                                                       \
    TCollection_AsciiString sarg((meth));                                                          \
    (sarg) = (sarg) + (shaset).DEBNumber() + (str);

Standard_EXPORT void debsplitf(const int i)
{
  std::cout << "++ debsplitf " << i << std::endl;
}

Standard_EXPORT void debspanc(const int i)
{
  std::cout << "++ debspanc " << i << std::endl;
}

int GLOBAL_iexF = 0;
#endif

Standard_EXPORT occ::handle<Geom2d_Curve> BASISCURVE2D(const occ::handle<Geom2d_Curve>& C);
Standard_EXPORT void                      TopOpeBRepDS_SetThePCurve(const BRep_Builder&              B,
                                                                    TopoDS_Edge&                     E,
                                                                    const TopoDS_Face&               F,
                                                                    const TopAbs_Orientation         O,
                                                                    const occ::handle<Geom2d_Curve>& C);
// Standard_IMPORT int FUN_tool_outofUVbounds
//(const TopoDS_Face& fF,const TopoDS_Edge& E,double& splitpar);

//---------------------------------------------
static int FUN_getG(const gp_Pnt&                                                   P,
                    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                    const occ::handle<TopOpeBRepDS_HDataStructure>&                 HDS,
                    int&                                                            iEinterf)
//---------------------------------------------
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ILI(LI);
  occ::handle<TopOpeBRepDS_CurvePointInterference>                   SSI;
  for (; ILI.More(); ILI.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = ILI.Value();
    SSI = occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I);
    if (SSI.IsNull())
      continue;
    int GI                          = SSI->Geometry();
    iEinterf                        = SSI->Support();
    const TopOpeBRepDS_Point& DSP   = HDS->Point(GI);
    const gp_Pnt&             P3d   = DSP.Point();
    double                    tolp  = DSP.Tolerance();
    bool                      sameP = P3d.IsEqual(P, tolp);
    if (sameP)
      return GI;
  }
  return 0;
}

//----------------------------------------------------------------------
// FUN_EPIforEvisoONperiodicF :
// Let <F> be a periodic face,
// <E> an edge with as pcurve on <F> a Viso line.
//
// if the pcurve of par u not bound in [0,2PI] : computes <CPI> interference on
// <E> to split the edge at the point p2pi of u = 2PI; returns true.
// else                                        : returns false.
//
// To split the edge, scans among the list of edge point interferences
// <EPIL> in order to get a geometry point falling into geometry P2pi on
// <F> of UV parameters = p2pi.
// Gets the new vertex of array <newV> attached to the geometry P2pi.
//----------------------------------------------------------------------

#define SPLITEDGE (0)
#define INCREASEPERIOD (1)
#define DECREASEPERIOD (-1)

static bool FUN_EPIforEvisoONperiodicF(
  const TopoDS_Edge&                                              E,
  const TopoDS_Face&                                              F,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& EPIlist,
  const occ::handle<TopOpeBRepDS_HDataStructure>&                 HDS,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       loCPI)
{
  double                 parone = -1.e7;
  TopOpeBRepTool_CORRISO CORRISO(F);
  CORRISO.Init(F);
  double uper;
  bool   onU     = CORRISO.Refclosed(1, uper);
  double tolF    = BRep_Tool::Tolerance(F);
  double tolu    = CORRISO.Tol(1, tolF);
  int    recadre = CORRISO.EdgeOUTofBoundsUV(E, onU, tolu, parone);
  // int recadre = FUN_tool_outofUVbounds(F,E,parone);
  if (recadre != SPLITEDGE)
    return false;

  gp_Pnt p3d;
  bool   ok = FUN_tool_value(parone, E, p3d);
  if (!ok)
    return false; // nyi FUN_Raise
  int iEinterf = 0;
  int iG       = FUN_getG(p3d, EPIlist, HDS, iEinterf);
  if (iG == 0)
  {
    return false;
  }
  if (HDS->Shape(iEinterf).ShapeType() != TopAbs_EDGE)
    iEinterf = 0;
  //  else V2pi = TopoDS::Vertex(newV->Array1().Value(iG));

  // <CPI> :
  int                     iS = HDS->Shape(E);
  TopOpeBRepDS_Transition T(TopAbs_IN, TopAbs_IN, TopAbs_EDGE, TopAbs_EDGE);
  T.Index(iS);
  occ::handle<TopOpeBRepDS_CurvePointInterference> CPI =
    new TopOpeBRepDS_CurvePointInterference(T,
                                            TopOpeBRepDS_EDGE,
                                            iEinterf,
                                            TopOpeBRepDS_POINT,
                                            iG,
                                            parone);
  loCPI.Append(CPI);
  return true;
} // FUN_EPIforEvisoONperiodicF

//----------------------------------------------------------------------
// FUN_GetSplitsON :
// <F> is a periodic face, <E> has for pcurve on <F> a visoline
// of par u not bound in [0,2PI].
// Splits the edge at <paronE> (UV point for <paronE> has its u=2PI)
// Recompute the pcurve for the split with (parameter on edge >= <paronE>)
//----------------------------------------------------------------------
/*static void FUN_GetSplitsON
(const TopoDS_Edge& E, TopoDS_Vertex& V2pi, const double& paronE,const TopoDS_Face& F,
NCollection_List<TopoDS_Shape>& losplits)
{
  double pf,pl,tolpc;
  TopoDS_Vertex Vf, Vl; TopExp::Vertices(E,Vf,Vl);
  occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E,F,pf,pl,tolpc);
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(F);
  double tole = BRep_Tool::Tolerance(E);
  TopOpeBRepDS_BuildTool BT; BRep_Builder BB;

  TopAbs_Orientation oriVinf, oriVsup, oriE = E.Orientation();
  oriVinf = (oriE == TopAbs_FORWARD)? TopAbs_FORWARD: TopAbs_REVERSED;
  oriVsup = (oriE == TopAbs_FORWARD)? TopAbs_REVERSED: TopAbs_FORWARD;

  // Einf2pi :
  TopoDS_Edge Einf2pi; BT.CopyEdge(E,Einf2pi);
  Vf.Orientation(oriVinf);   BB.Add(Einf2pi,Vf);   BT.Parameter(Einf2pi,Vf,pf);
  V2pi.Orientation(oriVsup); BB.Add(Einf2pi,V2pi); BT.Parameter(Einf2pi,V2pi,paronE);

  // Esup2pi :
  TopoDS_Edge Esup2pi; BT.CopyEdge(E,Esup2pi);
  V2pi.Orientation(oriVinf); BB.Add(Esup2pi,V2pi); BT.Parameter(Esup2pi,V2pi,paronE);
  Vl.Orientation(oriVsup);   BB.Add(Esup2pi,Vl);   BT.Parameter(Esup2pi,Vl,pl);
  gp_Pnt2d tmp = PC->Value(pf); double v = tmp.Y();
  occ::handle<Geom2d_Line> L2d =
    new Geom2d_Line(gp_Pnt2d(-paronE,v),gp_Dir2d(gp_Dir2d::D::X));
  occ::handle<Geom2d_TrimmedCurve> PCsup2pi = new Geom2d_TrimmedCurve(L2d,paronE,pl);
  TopOpeBRepDS_SetThePCurve(BB,Esup2pi,F,oriE,PCsup2pi);

#ifdef OCCT_DEBUG
  bool trc = false;
#endif
  losplits.Append(Einf2pi); losplits.Append(Esup2pi);
}*/

//---------------------------------------------
static void FUN_getEPI(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                       NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       EPI)
//---------------------------------------------
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ILI(LI);
  occ::handle<TopOpeBRepDS_CurvePointInterference>                   CPI;
  for (; ILI.More(); ILI.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = ILI.Value();
    CPI = occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I);
    if (CPI.IsNull())
      continue;
    TopOpeBRepDS_Kind GT, ST;
    int               GI, SI;
    FDS_data(CPI, GT, GI, ST, SI);
    if (GT != TopOpeBRepDS_POINT || ST != TopOpeBRepDS_FACE)
      continue;
    EPI.Append(I);
  }
}

//---------------------------------------------
static void FUN_getEPIonEds(const TopoDS_Shape&                                       FOR,
                            const occ::handle<TopOpeBRepDS_HDataStructure>&           HDS,
                            NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& EPI)
//---------------------------------------------
{
  TopExp_Explorer ex(FOR, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape& E = ex.Current();
    if (HDS->HasShape(E))
    {
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LII =
        HDS->DS().ShapeInterferences(E);
      FUN_getEPI(LII, EPI);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::GMergeSolids(const NCollection_List<TopoDS_Shape>& LSO1,
                                           const NCollection_List<TopoDS_Shape>& LSO2,
                                           const TopOpeBRepBuild_GTopo&          G1)
{
  if (LSO1.IsEmpty())
    return;
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);

  const TopoDS_Shape& SO1 = LSO1.First();
#ifdef OCCT_DEBUG
  int  iSO;
  bool tSPS = GtraceSPS(SO1, iSO);
  if (tSPS)
  {
    std::cout << std::endl;
    std::cout << "--- GMergeSolids " << std::endl;
    GdumpSAMDOM(LSO1, (char*)"1 : ");
    GdumpSAMDOM(LSO2, (char*)"2 : ");
  }
#endif

  mySolidReference = TopoDS::Solid(SO1);
  TopOpeBRepBuild_ShellFaceSet SFS(SO1, this);
  GFillSolidsSFS(LSO1, LSO2, G1, SFS);

  // Create a solid builder SOBU
  TopoDS_Shape SO1F = LSO1.First();
  SO1F.Orientation(TopAbs_FORWARD);
  TopOpeBRepBuild_SolidBuilder SOBU;
  bool                         ForceClassSOBU = true;
  SOBU.InitSolidBuilder(SFS, ForceClassSOBU);

  // Build new solids LSOM
  NCollection_List<TopoDS_Shape> LSOM;
  GSOBUMakeSolids(SO1F, SOBU, LSOM);

  // connect new solids as solids built TB1 on LSO1 solids
  NCollection_List<TopoDS_Shape>::Iterator it1;
  for (it1.Initialize(LSO1); it1.More(); it1.Next())
  {
    const TopoDS_Shape& aSO1     = it1.Value();
    bool                ismerged = IsMerged(aSO1, TB1);
    if (ismerged)
      continue;
    NCollection_List<TopoDS_Shape>& SOL = ChangeMerged(aSO1, TB1);
    SOL                                 = LSOM;
  }

  // connect new solids as solids built TB2 on LSO2 solids
  NCollection_List<TopoDS_Shape>::Iterator it2;
  for (it2.Initialize(LSO2); it2.More(); it2.Next())
  {
    const TopoDS_Shape& SO2      = it2.Value();
    bool                ismerged = IsMerged(SO2, TB2);
    if (ismerged)
      continue;
    NCollection_List<TopoDS_Shape>& SOL = ChangeMerged(SO2, TB2);
    SOL                                 = LSOM;
  }

} // GMergeSolids

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillSolidsSFS(const NCollection_List<TopoDS_Shape>& LS1,
                                             const NCollection_List<TopoDS_Shape>& LS2,
                                             const TopOpeBRepBuild_GTopo&          G1,
                                             TopOpeBRepBuild_ShellFaceSet&         SFS)
{

  if (LS1.IsEmpty())
    return;
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  myProcessON = (Opecom() || Opefus());
  if (myProcessON)
  {
    myONFacesMap.Clear();
  }

  mySolidReference = TopoDS::Solid(LS1.First());

  TopAbs_State                             TB;
  TopOpeBRepBuild_GTopo                    G;
  NCollection_List<TopoDS_Shape>::Iterator it;

  G  = G1;
  TB = TB1;
  it.Initialize(LS1);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& S       = it.Value();
    bool                tomerge = !IsMerged(S, TB);
#ifdef OCCT_DEBUG
    int  iS;
    bool tSPS = GtraceSPS(S, iS);
    if (tSPS)
    {
      std::cout << std::endl;
      GdumpSHASTA(S, TB, "--- GFillSolidsSFS ");
      std::cout << " tomerge : " << tomerge << std::endl;
    }
#endif
    if (tomerge)
      GFillSolidSFS(S, LS2, G, SFS);
  }

  G  = G1.CopyPermuted();
  TB = TB2;
  it.Initialize(LS2);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& S       = it.Value();
    bool                tomerge = !IsMerged(S, TB);
#ifdef OCCT_DEBUG
    int  iS;
    bool tSPS = GtraceSPS(S, iS);
    if (tSPS)
    {
      std::cout << std::endl;
      GdumpSHASTA(S, TB, "--- GFillSolidsSFS ");
      std::cout << " tomerge : " << tomerge << std::endl;
    }
#endif
    if (tomerge)
      GFillSolidSFS(S, LS1, G, SFS);
  }

  if (myProcessON)
  {
    AddONPatchesSFS(G1, SFS);
    myProcessON = false;
  }

} // GFillSolidsSFS

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillSolidSFS(const TopoDS_Shape&                   SO1,
                                            const NCollection_List<TopoDS_Shape>& LSO2,
                                            const TopOpeBRepBuild_GTopo&          G1,
                                            TopOpeBRepBuild_ShellFaceSet&         SFS)
{
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  bool RevOri1 = G1.IsToReverse1();

#ifdef OCCT_DEBUG
  int  iSO;
  bool tSPS = GtraceSPS(SO1, iSO);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHASTA(SO1, TB1, "--- GFillSolidSFS ");
    std::cout << std::endl;
  }
#endif

  // work on a FORWARD solid SOF
  TopoDS_Shape SOF = SO1;
  SOF.Orientation(TopAbs_FORWARD);
  mySolidToFill = TopoDS::Solid(SOF);

  TopOpeBRepTool_ShapeExplorer exShell(SOF, TopAbs_SHELL);
  for (; exShell.More(); exShell.Next())
  {
    TopoDS_Shape SH       = exShell.Current();
    bool         hasshape = myDataStructure->HasShape(SH);

    if (!hasshape)
    {
      // shell SH is not in DS : classify it with LSO2 solids
      bool keep = GKeepShape(SH, LSO2, TB1);
      if (keep)
      {
        TopAbs_Orientation oriSH    = SH.Orientation();
        TopAbs_Orientation neworiSH = Orient(oriSH, RevOri1);
        SH.Orientation(neworiSH);

#ifdef OCCT_DEBUG
        if (tSPS)
        {
          DEBSHASET(ss, "--- GFillSolidSFS ", SFS, " AddShape SFS+ shell ");
          GdumpSHA(SH, (void*)ss.ToCString());
          std::cout << " ";
          TopAbs::Print(TB1, std::cout) << " : 1 shell ";
          TopAbs::Print(neworiSH, std::cout);
          std::cout << std::endl;
        }
#endif

        SFS.AddShape(SH);
      }
    }
    else
    { // shell SH has faces(s) with geometry : split SH faces
      GFillShellSFS(SH, LSO2, G1, SFS);
    }
  }

} // GFillSolidSFS

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GFillSurfaceTopologySFS(const TopoDS_Shape& SO1,
#else
void TopOpeBRepBuild_Builder::GFillSurfaceTopologySFS(const TopoDS_Shape&,
#endif
                                                      const TopOpeBRepBuild_GTopo& G1,
                                                      TopOpeBRepBuild_ShellFaceSet& /*SFS*/)
{
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  TopAbs_ShapeEnum t1, t2;
  G1.Type(t1, t2);
#ifdef OCCT_DEBUG
  TopAbs_ShapeEnum ShapeInterf = t1;
#endif

#ifdef OCCT_DEBUG
  int  iSO;
  bool tSPS = GtraceSPS(SO1, iSO);
  if (tSPS)
  {
    std::cout << std::endl;
    std::cout << "--- GFillSurfaceTopologySFS ShapeInterf ";
    TopAbs::Print(ShapeInterf, std::cout);
    std::cout << std::endl;
  }
  std::cout << "GFillSurfaceTopologySFS : NYI" << std::endl;
#endif

} // GFillSurfaceTopologySFS

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillSurfaceTopologySFS(const TopOpeBRepDS_SurfaceIterator& SSit,
                                                      const TopOpeBRepBuild_GTopo&        G1,
                                                      TopOpeBRepBuild_ShellFaceSet&       SFS) const
{
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  TopOpeBRepDS_Config Conf = G1.Config1();
  TopAbs_State        TB   = TB1;
  if (Conf == TopOpeBRepDS_DIFFORIENTED)
  {
    if (TB1 == TopAbs_OUT)
      TB = TopAbs_IN;
    else if (TB1 == TopAbs_IN)
      TB = TopAbs_OUT;
  }

#ifdef OCCT_DEBUG
  int  iSO;
  bool tSPS = GtraceSPS(SFS.Solid(), iSO);
  int  iref = myDataStructure->Shape(mySolidReference);
  int  ifil = myDataStructure->Shape(mySolidToFill);
  if (tSPS)
  {
    std::cout << "ifil : " << ifil << " iref : " << iref << std::endl;
    std::cout << "solid " << ifil << " is ";
    TopOpeBRepDS::Print(Conf, std::cout);
    std::cout << std::endl;
  }
#endif

  // iG = index of new surface // NYI or existing face
  int                                      iG    = SSit.Current();
  const NCollection_List<TopoDS_Shape>&    LnewF = NewFaces(iG);
  NCollection_List<TopoDS_Shape>::Iterator Iti(LnewF);
  for (; Iti.More(); Iti.Next())
  {
    TopoDS_Shape       F   = Iti.Value();
    TopAbs_Orientation ori = SSit.Orientation(TB);
    F.Orientation(ori);

#ifdef OCCT_DEBUG
    if (tSPS)
    {
      DEBSHASET(ss, "--- GFillSurfaceTopologySFS ", SFS, " AddElement SFS+ face ");
      GdumpSHA(F, (void*)ss.ToCString());
      std::cout << " ";
      TopAbs::Print(TB, std::cout) << " : 1 face ";
      TopAbs::Print(ori, std::cout);
      std::cout << std::endl;
    }
#endif

    SFS.AddElement(F);
  } // iterate on new faces built on surface <iG>

} // GFillSurfaceTopologySFS

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillShellSFS(const TopoDS_Shape&                   SH,
                                            const NCollection_List<TopoDS_Shape>& LSO2,
                                            const TopOpeBRepBuild_GTopo&          G1,
                                            TopOpeBRepBuild_ShellFaceSet&         SFS)
{
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);

#ifdef OCCT_DEBUG
  int  ish;
  bool tSPS = GtraceSPS(SH, ish);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHA(SH, (char*)"--- GFillShellSFS ");
    std::cout << std::endl;
  }
#endif

  TopOpeBRepTool_ShapeExplorer exFace;

  // 1/ : toutes les faces HasSameDomain
  for (exFace.Init(SH, TopAbs_FACE); exFace.More(); exFace.Next())
  {
    const TopoDS_Shape& FOR = exFace.Current();
    bool                hsd = myDataStructure->HasSameDomain(FOR);
    if (hsd)
    {
      GFillFaceSFS(FOR, LSO2, G1, SFS);
    } // hsd
  } // exFace.More()

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    SFS.DumpSS();
  }
#endif

  // 2/ : toutes les faces non HasSameDomain
  for (exFace.Init(SH, TopAbs_FACE); exFace.More(); exFace.Next())
  {
    const TopoDS_Shape& FOR = exFace.Current();
    bool                hsd = myDataStructure->HasSameDomain(FOR);
    if (!hsd)
    {
      GFillFaceSFS(FOR, LSO2, G1, SFS);
    } // hsd
  }

} // GFillShellSFS

// ----------------------------------------------------------------------
static void FUNBUILD_MAPSUBSHAPES(
  const TopoDS_Shape&                                            S,
  const TopAbs_ShapeEnum                                         T,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& _IM)
{
  TopExp::MapShapes(S, T, _IM);
}

// ----------------------------------------------------------------------
static void FUNBUILD_MAPSUBSHAPES(
  const NCollection_List<TopoDS_Shape>&                          LOFS,
  const TopAbs_ShapeEnum                                         T,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& _IM)
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(LOFS); it.More(); it.Next())
    FUNBUILD_MAPSUBSHAPES(it.Value(), T, _IM);
}

// ----------------------------------------------------------------------
static void FUNBUILD_MAPANCSPLSHAPES(
  TopOpeBRepBuild_Builder& B,
  const TopoDS_Shape&      S,
  const TopAbs_State       STATE,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    _IDM)
{
  bool issp = B.IsSplit(S, STATE);
  if (issp)
  {
    const NCollection_List<TopoDS_Shape>& l = B.Splits(S, STATE);
    for (NCollection_List<TopoDS_Shape>::Iterator it(l); it.More(); it.Next())
    {
      const TopoDS_Shape& sps = it.Value(); // sps = split result of S on state STATE
      _IDM.Bound(sps, NCollection_List<TopoDS_Shape>())->Append(S);
    }
  }
}

// ----------------------------------------------------------------------
static void FUNBUILD_MAPANCSPLSHAPES(
  TopOpeBRepBuild_Builder& B,
  const TopoDS_Shape&      S,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    _IDM)
{
  FUNBUILD_MAPANCSPLSHAPES(B, S, TopAbs_IN, _IDM);
  FUNBUILD_MAPANCSPLSHAPES(B, S, TopAbs_OUT, _IDM);
}

// ----------------------------------------------------------------------
static void FUNBUILD_MAPANCSPLSHAPES(
  TopOpeBRepBuild_Builder&                                             B,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& M,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    _IDM)
{
  int n = M.Extent();
  for (int i = 1; i <= n; i++)
    FUNBUILD_MAPANCSPLSHAPES(B, M(i), _IDM);
}

static NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> stabuild_IMELF1;
static NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> stabuild_IMELF2;
static NCollection_IndexedDataMap<TopoDS_Shape,
                                  NCollection_List<TopoDS_Shape>,
                                  TopTools_ShapeMapHasher>
  stabuild_IDMEALF1;
static NCollection_IndexedDataMap<TopoDS_Shape,
                                  NCollection_List<TopoDS_Shape>,
                                  TopTools_ShapeMapHasher>
                           stabuild_IDMEALF2;
static TopOpeBRepDS_Config static_CONF1;
static TopOpeBRepDS_Config static_CONF2;

// ----------------------------------------------------------------------
Standard_EXPORT void FUNBUILD_ANCESTORRANKPREPARE(TopOpeBRepBuild_Builder&              B,
                                                  const NCollection_List<TopoDS_Shape>& LF1,
                                                  const NCollection_List<TopoDS_Shape>& LF2,
                                                  const TopOpeBRepDS_Config             CONF1,
                                                  const TopOpeBRepDS_Config             CONF2)
{
  static_CONF1 = CONF1;
  static_CONF2 = CONF2;
  FUNBUILD_MAPSUBSHAPES(LF1, TopAbs_EDGE, stabuild_IMELF1);
  FUNBUILD_MAPSUBSHAPES(LF2, TopAbs_EDGE, stabuild_IMELF2);
  FUNBUILD_MAPANCSPLSHAPES(B, stabuild_IMELF1, stabuild_IDMEALF1);
  FUNBUILD_MAPANCSPLSHAPES(B, stabuild_IMELF2, stabuild_IDMEALF2);
}

static NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> stabuild_IMEF;

// ----------------------------------------------------------------------
Standard_EXPORT void FUNBUILD_ANCESTORRANKGET(TopOpeBRepBuild_Builder& /*B*/,
                                              const TopoDS_Shape& f,
                                              bool&               of1,
                                              bool&               of2)
{
  FUNBUILD_MAPSUBSHAPES(f, TopAbs_EDGE, stabuild_IMEF);
  int ief = 1, nef = stabuild_IMEF.Extent();
  of1 = false;
  for (ief = 1; ief <= nef; ief++)
  {
    const TopoDS_Shape& e = stabuild_IMEF(ief);
    of1                   = stabuild_IDMEALF1.Contains(e);
    if (of1)
      break;
  }
  of2 = false;
  for (ief = 1; ief <= nef; ief++)
  {
    const TopoDS_Shape& e = stabuild_IMEF(ief);
    of2                   = stabuild_IDMEALF2.Contains(e);
    if (of2)
      break;
  }
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUNBUILD_ORIENTLOFS(TopOpeBRepBuild_Builder&        B,
                                         const TopAbs_State              TB1,
                                         const TopAbs_State              TB2,
                                         NCollection_List<TopoDS_Shape>& LOFS)
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(LOFS); it.More(); it.Next())
  {
    TopoDS_Shape& f = it.ChangeValue();
    bool          of1, of2;
    FUNBUILD_ANCESTORRANKGET(B, f, of1, of2);
    TopAbs_Orientation orif = f.Orientation();
    bool               r12  = TopOpeBRepBuild_Builder::Reverse(TB1, TB2);
    bool               r21  = TopOpeBRepBuild_Builder::Reverse(TB2, TB1);
    bool               rf   = false;
    if (of1 && !of2)
      rf = r12;
    else if (of2 && !of1)
      rf = r21;
    TopAbs_Orientation neworif = TopOpeBRepBuild_Builder::Orient(orif, rf);
    f.Orientation(neworif);
  }
}

Standard_EXPORT bool GLOBAL_revownsplfacori = false;
// GLOBAL_REVerseOWNSPLittedFACeORIentation = True : dans GSplitFaceSFS on
// applique le retournement d'orientation de la face splittee FS de F
// a l'orientation de FS elle-meme (au lieu de l'appliquer a l'orientation
// de la face F comme en standard)

// Standard_IMPORT extern NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>*
// GLOBAL_SplitAnc; //xpu260598
Standard_EXPORTEXTERN NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>*
                      GLOBAL_SplitAnc; // xpu260598
// static TopAbs_Orientation FUN_intTOori(const int Iori)
//{
//   if (Iori == 1)  return TopAbs_FORWARD;
//   if (Iori == 2)  return TopAbs_REVERSED;
//   if (Iori == 11) return TopAbs_INTERNAL;
//   if (Iori == 22) return TopAbs_EXTERNAL;
//   return TopAbs_EXTERNAL;
// }

// Standard_IMPORT extern NCollection_List<TopoDS_Shape>* GLOBAL_lfr1;
Standard_EXPORTEXTERN NCollection_List<TopoDS_Shape>* GLOBAL_lfr1;
// Standard_IMPORT extern bool GLOBAL_lfrtoprocess;
Standard_EXPORTEXTERN bool GLOBAL_lfrtoprocess;

//=================================================================================================

void TopOpeBRepBuild_Builder::GSplitFaceSFS(const TopoDS_Shape&                   FOR,
                                            const NCollection_List<TopoDS_Shape>& LSclass,
                                            const TopOpeBRepBuild_GTopo&          G1,
                                            TopOpeBRepBuild_ShellFaceSet&         SFS)
{
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  bool                              RevOri1 = G1.IsToReverse1();
  TopAbs_Orientation                oriF    = FOR.Orientation();
  TopAbs_Orientation                neworiF = Orient(oriF, RevOri1);
  const TopOpeBRepDS_DataStructure& BDS     = myDataStructure->DS();
#ifdef OCCT_DEBUG
  int iFOR =
#endif
    BDS.Shape(FOR);

#ifdef OCCT_DEBUG
  int  iiFOR;
  bool tSPS = GtraceSPS(FOR, iiFOR);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHASTA(FOR, TB1, "--- GSplitFaceSFS ");
    std::cout << " RevOri1 : " << RevOri1 << std::endl;
    debsplitf(iFOR);
  }
#endif

  bool issplit = IsSplit(FOR, TB1);
  if (issplit)
  {

    // LOFS faces all have the same topological orientation.
    // according to edge origin and operation performed, orientate them.
    // NYI CDLize MapEdgeAncestors or modify WES in such a way
    // that it memorizes edge ancestors of added elements.

    NCollection_List<TopoDS_Shape>& LSF = ChangeSplit(FOR, TB1);
    if (GLOBAL_revownsplfacori)
    {
      FUNBUILD_ORIENTLOFS(*this, TB1, TB2, LSF);
    }
    for (NCollection_List<TopoDS_Shape>::Iterator it(LSF); it.More(); it.Next())
    {
      TopoDS_Shape newF = it.Value();

      if (GLOBAL_SplitAnc != nullptr)
      {
        bool hasoridef = GLOBAL_SplitAnc->IsBound(newF); // xpu260598

        bool opeFus = Opefus();
        bool opec12 = Opec12();
        bool opec21 = Opec21();
        bool opeCut = opec12 || opec21;
        bool opeCom = Opecom();

        if (hasoridef)
        {
          int iAnc = GLOBAL_SplitAnc->Find(newF);

          int                rkAnc = BDS.AncestorRank(iAnc);
          TopAbs_Orientation oAnc  = BDS.Shape(iAnc).Orientation();
#ifdef OCCT_DEBUG
          int  iFanc;
          bool tSPSa = GtraceSPS(BDS.Shape(iAnc), iFanc);
          if (tSPSa)
            debspanc(iAnc);
#endif
          if (opeCom)
          {
            // xpu260598 : orifspIN = orifanc
            //  bcl1;bcl2 tspIN(f23) is splitIN(f23), f9 SDDO f23
            neworiF = oAnc;
          }
          else if (opeCut)
          {
            // xpu280598 : cto100G1 spIN(f21)
            TopAbs_State TBAnc = TopAbs_UNKNOWN;
            if (opec12)
              TBAnc = (rkAnc == 1) ? TopAbs_OUT : TopAbs_IN;
            if (opec21)
              TBAnc = (rkAnc == 2) ? TopAbs_OUT : TopAbs_IN;

            // if TBAnc == OUT : we keep orientation
            // else              we reverse it
            if (TBAnc == TopAbs_OUT)
              neworiF = oAnc;
            else
              neworiF = TopAbs::Complement(oAnc);
          }
          else if (opeFus)
          {
            neworiF = oAnc; // xpu290598
          }

          bool reverse = false;
          int  irefAnc = BDS.SameDomainRef(iAnc);
          if (irefAnc != iAnc)
          { // newFace is built on geometry of refAnc
            bool                samegeom = false;
            TopOpeBRepDS_Config cAnc     = BDS.SameDomainOri(iAnc);
            if (cAnc == TopOpeBRepDS_SAMEORIENTED)
              samegeom = true;
            else if (cAnc == TopOpeBRepDS_DIFFORIENTED)
              samegeom = false;
            TopAbs_Orientation orefAnc = BDS.Shape(irefAnc).Orientation();
            if (oAnc != orefAnc)
              samegeom = !samegeom;
            reverse = !samegeom;
          }
          if (reverse)
            neworiF = TopAbs::Complement(neworiF);

        } // hasoridef
      }

      newF.Orientation(neworiF);

      if (GLOBAL_lfrtoprocess)
      {
        GLOBAL_lfr1->Append(newF);
      }
      else
      {
#ifdef OCCT_DEBUG
        if (tSPS)
        {
          DEBSHASET(ss, "--- GSplitFaceSFS ", SFS, " AddStartElement SFS+ face ");
          GdumpSHA(newF, (void*)ss.ToCString());
          std::cout << " ";
          TopAbs::Print(TB1, std::cout) << " : 1 face ";
          TopAbs::Print(neworiF, std::cout);
          std::cout << std::endl;
        }
#endif

        SFS.AddStartElement(newF);
      }
    }
  }
  else
  {
    // FOR n'a pas de devenir de Split par TB1
    // on garde FOR si elle est situee TB1 / LSclass
    bool add = true;

    bool hs       = myDataStructure->HasShape(FOR);
    bool hg       = myDataStructure->HasGeometry(FOR);
    bool testkeep = true;
    testkeep      = (hs && (!hg)); // +12/05 macktruck

    if (testkeep)
    {
      bool keep = GKeepShape(FOR, LSclass, TB1);
      add       = keep;
    }
    if (add)
    {
      TopoDS_Shape F = FOR;
      F.Orientation(neworiF);

#ifdef OCCT_DEBUG
      if (tSPS)
      {
        DEBSHASET(ss, "--- GSplitFaceSFS ", SFS, " AddElement SFS+ face ");
        GdumpSHA(F, (void*)ss.ToCString());
        std::cout << " ";
        TopAbs::Print(TB1, std::cout) << " : 1 face ";
        TopAbs::Print(neworiF, std::cout);
        std::cout << std::endl;
      }
#endif

      SFS.AddElement(F);
    }
  }

} // GSplitFaceSFS

//=======================================================================
// function : GMergeFaceSFS
// purpose  : (methode non utilisee)
//=======================================================================
void TopOpeBRepBuild_Builder::GMergeFaceSFS(const TopoDS_Shape&           FOR,
                                            const TopOpeBRepBuild_GTopo&  G1,
                                            TopOpeBRepBuild_ShellFaceSet& SFS)
{
#ifdef OCCT_DEBUG
  int  iFOR;
  bool tSPS = GtraceSPS(FOR, iFOR);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHA(FOR, (char*)"--- GMergeFaceSFS ");
    std::cout << std::endl;
  }
#endif

  bool tomerge = GToMerge(FOR);
  if (!tomerge)
    return;

  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  bool RevOri1 = G1.IsToReverse1();

  TopAbs_Orientation oriF    = FOR.Orientation();
  TopAbs_Orientation neworiF = Orient(oriF, RevOri1);

  TopoDS_Shape FF = FOR;
  FF.Orientation(TopAbs_FORWARD);

  bool makecomsam = GTakeCommonOfSame(G1);
  bool makecomdif = GTakeCommonOfDiff(G1);
  if (!makecomsam && !makecomdif)
    return;

  // LFSO,LFDO (samedom,sameori),(samedom,diffori) des 2 shapes peres
  // LFSO1,LFDO1 (samedom,sameori),(samedom,diffori) du shape pere de F
  // LFSO2,LFDO2 (samedom,sameori),(samedom,diffori) du shape != pere de F
  NCollection_List<TopoDS_Shape> LFSO, LFDO, LFSO1, LFDO1, LFSO2, LFDO2;
  GFindSamDomSODO(FF, LFSO, LFDO);
  int rankF = GShapeRank(FF), rankX = (rankF) ? ((rankF == 1) ? 2 : 1) : 0;
  GFindSameRank(LFSO, rankF, LFSO1);
  GFindSameRank(LFDO, rankF, LFDO1);
  GFindSameRank(LFSO, rankX, LFSO2);
  GFindSameRank(LFDO, rankX, LFDO2);

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    std::cout << "--------- merge FACE " << iFOR << std::endl;
    GdumpSAMDOM(LFSO1, (char*)"LFSO1 : ");
    GdumpSAMDOM(LFDO1, (char*)"LFDO1 : ");
    GdumpSAMDOM(LFSO2, (char*)"LFSO2 : ");
    GdumpSAMDOM(LFDO2, (char*)"LFDO2 : ");
  }
#endif

  bool                           performcom = false;
  NCollection_List<TopoDS_Shape>*PtrLF1 = nullptr, *PtrLF2 = nullptr;
  int                            n1 = 0, n2 = 0;
  if (makecomsam)
  {
    n1         = LFSO1.Extent();
    n2         = LFSO2.Extent();
    performcom = (n1 != 0 && n2 != 0);
    if (performcom)
    {
      PtrLF1 = &LFSO1;
      PtrLF2 = &LFSO2;
    }
  }
  else if (makecomdif)
  {
    n1         = LFSO1.Extent();
    n2         = LFDO2.Extent();
    performcom = (n1 != 0 && n2 != 0);
    if (performcom)
    {
      PtrLF1 = &LFSO1;
      PtrLF2 = &LFDO2;
    }
  }

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    std::cout << "performcom : " << performcom << " ";
    std::cout << "makecomsam : " << makecomsam << " makcomdif : " << makecomdif << " ";
    std::cout << "n1 : " << n1 << " n2 : " << n2 << std::endl;
    std::cout << "GMergeFaceSFS RevOri1 : " << RevOri1 << std::endl;
  }
#endif

  if (performcom)
  {
    TopOpeBRepBuild_GTopo gF;
    if (makecomsam)
    {
      gF = TopOpeBRepBuild_GTool::GComUnsh(TopAbs_FACE, TopAbs_FACE);
      gF.ChangeConfig(TopOpeBRepDS_SAMEORIENTED, TopOpeBRepDS_SAMEORIENTED);
    }
    else if (makecomdif)
    {
      gF = TopOpeBRepBuild_GTool::GComUnsh(TopAbs_FACE, TopAbs_FACE);
      gF.ChangeConfig(TopOpeBRepDS_SAMEORIENTED, TopOpeBRepDS_DIFFORIENTED);
    }

    GMergeFaces(*PtrLF1, *PtrLF2, gF);

    // on prend le resultat du merge de F ssi F est HasSameDomain et
    // qu'elle est la reference de ses faces SameDomain
    bool                addmerge = false;
    int                 iFref    = myDataStructure->SameDomainReference(FOR);
    const TopoDS_Shape& Fref     = myDataStructure->Shape(iFref);
    bool                Fisref   = FOR.IsSame(Fref);
    addmerge                     = Fisref;

    if (addmerge)
    {
      const NCollection_List<TopoDS_Shape>& ME = Merged(FOR, TopAbs_IN);
      for (NCollection_List<TopoDS_Shape>::Iterator it(ME); it.More(); it.Next())
      {
        TopoDS_Shape newF = it.Value();
        newF.Orientation(neworiF);

#ifdef OCCT_DEBUG
        if (tSPS)
        {
          DEBSHASET(ss, "--- GMergeFaceSFS ", SFS, " AddStartElement SFS+ face ");
          GdumpSHA(newF, (void*)ss.ToCString());
          std::cout << " ";
          TopAbs::Print(TB1, std::cout) << " : 1 face ";
          TopAbs::Print(neworiF, std::cout);
          std::cout << std::endl;
        }
#endif
        SFS.AddStartElement(newF);
      }
    }
  } // performcom

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    std::cout << "--------- end merge FACE " << iFOR << std::endl;
  }
#endif

} // GMergeFaceSFS

static bool FUN_SplitEvisoONperiodicF(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const TopoDS_Shape&                             FF)
{
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LLI =
    HDS->DS().ShapeInterferences(FF);
  if (LLI.Extent() == 0)
    return true;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>           LI;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ILI(LLI);
  for (; ILI.More(); ILI.Next())
    LI.Append(ILI.Value());

  // LI3 = {I3 = (T(FACE),EG=EDGE,FS=FACE)}
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI1;
  int nIGtEDGE = FUN_selectGKinterference(LI, TopOpeBRepDS_EDGE, LI1);
  if (nIGtEDGE < 1)
    return true;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI2;
  int nIStFACE = FUN_selectSKinterference(LI1, TopOpeBRepDS_FACE, LI2);
  if (nIStFACE < 1)
    return true;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LI3;
  int nITRASHAFACE = FUN_selectTRASHAinterference(LI2, TopAbs_FACE, LI3);
  if (nITRASHAFACE < 1)
    return true;

  occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI;
  ILI.Initialize(LI3);
  for (; ILI.More(); ILI.Next())
  {

    SSI = occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(ILI.Value());
    TopOpeBRepDS_Kind GT, ST;
    int               GI, SI;
    FDS_data(SSI, GT, GI, ST, SI);

    const TopoDS_Face& FS = TopoDS::Face(HDS->Shape(SI));
    HDS->Shape(FS);
    //    bool FSper = FUN_periodicS(FS);
    bool FSper = FUN_tool_closedS(FS);
    if (!FSper)
      continue;

    const TopoDS_Edge& EG = TopoDS::Edge(HDS->Shape(GI));
    HDS->Shape(EG);
    bool isrest = HDS->DS().IsSectionEdge(EG);
    if (!isrest)
      continue;

    // --------------------------------------------------
    // <EG> has no representation on face <FS> yet,
    // set the pcurve on <FS>.
    // --------------------------------------------------
    double                    pf, pl, tol;
    occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(EG, FS, pf, pl, tol);
    if (PC.IsNull())
    {
      TopoDS_Edge EEG = EG;
      bool        ok  = FUN_tool_pcurveonF(FS, EEG);
      if (!ok)
        throw Standard_ProgramError("_Builder::SplitONVisolineonCyl");
      double f, l;
      PC = FC2D_CurveOnSurface(EEG, FS, f, l, tol);
    }

    bool     uiso, viso;
    gp_Dir2d d2d;
    gp_Pnt2d o2d;
    TopOpeBRepTool_TOOL::UVISO(PC, uiso, viso, d2d, o2d);
    if (!viso)
      continue;

    // a. cylinders same domain on cylindrical face, with closing edges non same domain :
    // the 2d rep. of an edge VisoLineOnCyl on the cylindrical face of the other shape
    // is not bounded in [0,2PI].
    // b. cylinder + sphere interfering on the circular edge E (section edge) of the cylinder
    // with the E's 2d rep on the spherical surface not bounded in [0,2PI] (cto 016 D*).

    // We have to split the edge at point (2PI,v), and we translate
    // the split of u >= 2PI to have it in [0,2PI].

    TopOpeBRepDS_DataStructure&                              BDS = HDS->ChangeDS();
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> EPIlist;
    FUN_getEPIonEds(FS, HDS, EPIlist);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loCPI;
    FUN_EPIforEvisoONperiodicF(EG, FS, EPIlist, HDS, loCPI);

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lIEG =
      BDS.ChangeShapeInterferences(EG);
    lIEG.Append(loCPI);
  } // ILI
  return true;
}

//=======================================================================
// function : SplitEvisoONperiodicF

// purpose  : KPart for :
//            - cylinders tangent on their cylindrical face,
//            with closing edges not same domain,
//            - cylinder + sphere interfering on the circular edge E (tangent
//            to the spherical surface) of the cylinder with  :
//            E's 2d rep on the spherical surface not bounded in [0,2PI]
//             (cto 016 D*).

//           Adding EPI to split edges with pcurve on <F> a Visoline not
//           U-bounded in [0,2PI].
// modifies : myDataStructure
//            Scans among the interferences attached to faces for FEI with
//            support <FS> = cylinder, geometry <EG>; adds pcurve on <FS>
//            for edge <EG> if necessary.
//=======================================================================
void TopOpeBRepBuild_Builder::SplitEvisoONperiodicF()
{
  //  myEsplitsONcycy.Clear();
  int nsha = myDataStructure->NbShapes();
  for (int i = 1; i <= nsha; i++)
  {
    const TopoDS_Shape& FOR    = myDataStructure->Shape(i);
    bool                isface = (FOR.ShapeType() == TopAbs_FACE);
    if (!isface)
      continue;

    TopLoc_Location                  loc;
    const occ::handle<Geom_Surface>& S        = BRep_Tool::Surface(TopoDS::Face(FOR), loc);
    bool                             periodic = S->IsUPeriodic() || S->IsVPeriodic();
    if (!periodic)
      continue;

    TopoDS_Shape FF = FOR;
    FF.Orientation(TopAbs_FORWARD);

    bool ok = FUN_SplitEvisoONperiodicF(myDataStructure, FF);
    if (!ok)
      throw Standard_ProgramError("_Builder::SplitONVisolineonCyl");
  } // i
}

//=================================================================================================

void TopOpeBRepBuild_Builder::GSplitFace(const TopoDS_Shape&                   FOR,
                                         const TopOpeBRepBuild_GTopo&          GG1,
                                         const NCollection_List<TopoDS_Shape>& LSclass)
{
  TopOpeBRepBuild_GTopo G1     = GG1;
  bool                  RevOri = false;
  G1.SetReverse(RevOri);

  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  TopAbs_ShapeEnum t1, t2;
  G1.Type(t1, t2);

  // work on a FORWARD face <FForward>
  TopoDS_Shape FF = FOR;
  FF.Orientation(TopAbs_FORWARD);

#ifdef OCCT_DEBUG
  int  iF;
  bool tSPS = GtraceSPS(FOR, iF);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHASTA(FOR, TB1, "--- GSplitFace ");
    std::cout << std::endl;
    debsplitf(iF);
  }
#endif

  // make a WireEdgeSet WES on face FF
  TopOpeBRepBuild_WireEdgeSet WES(FF, this);

  // Add ON parts (edges ON solid)
  GFillONPartsWES(FOR, G1, LSclass, WES);
#ifdef OCCT_DEBUG
  int n0 = WES.StartElements().Extent();
  if (tSPS)
    std::cout << "--> GSplitFace , after GFillONPartsWES nstartelWES = " << n0 << std::endl;
#endif

  // save these edges
  NCollection_List<TopoDS_Shape>           anEdgesON;
  NCollection_List<TopoDS_Shape>::Iterator it;
  if (myProcessON)
  {
    bool toRevOri = Opefus();
    for (it.Initialize(WES.StartElements()); it.More(); it.Next())
      anEdgesON.Append(toRevOri ? it.Value().Reversed() : it.Value());
    myONElemMap.Clear();
  }

  // split the edges of FF : add split edges to WES
  GFillFaceWES(FF, LSclass, G1, WES);
  int n1 = WES.StartElements().Extent();
#ifdef OCCT_DEBUG
  if (tSPS)
    std::cout << "--> GSplitFace , after GFillFaceWES nstartelWES = " << n1 << std::endl;
#endif

  // add edges built on curves supported by FF
  GFillCurveTopologyWES(FF, G1, WES);
  int n2 = WES.StartElements().Extent();
#ifdef OCCT_DEBUG
  if (tSPS)
    std::cout << "--> GSplitFace , after GFillCurveTopologyWES nstartelWES = " << n2 << std::endl;
#endif

  // myEdgeAvoid = StartElement edges of WES due to GFillCurveTopologyWES
  myEdgeAvoid.Clear();
  GCopyList(WES.StartElements(), (n1 + 1), n2, myEdgeAvoid);

  // mark FF as split TB1
  MarkSplit(FF, TB1);

  // build the new faces LOF on FF from the Wire/Edge set WES
  NCollection_List<TopoDS_Shape> LOF;
  GWESMakeFaces(FF, WES, LOF);

  if (myProcessON && (!anEdgesON.IsEmpty() || !myONElemMap.IsEmpty()))
  {
    // try to make patches with only ON parts.
    // prepare the map of used edges to not take the same matter two times
    NCollection_IndexedMap<TopoDS_Shape> aMapOE;
    for (it.Initialize(LOF); it.More(); it.Next())
      for (TopExp_Explorer ex(it.Value(), TopAbs_EDGE); ex.More(); ex.Next())
        aMapOE.Add(ex.Current());

    FillOnPatches(anEdgesON, FOR, aMapOE);
    myONElemMap.Clear();
  }

  // LOFS : LOF faces located TB1 / LSclass = split faces of state TB1 of FF
  NCollection_List<TopoDS_Shape>& LOFS = ChangeSplit(FF, TB1);
  LOFS.Clear();
  GKeepShapes(FF, myEmptyShapeList, TB1, LOF, LOFS);

} // GSplitFace

//=================================================================================================

void TopOpeBRepBuild_Builder::AddONPatchesSFS(const TopOpeBRepBuild_GTopo&  G1,
                                              TopOpeBRepBuild_ShellFaceSet& SFS)
{
  // select ON faces not same domain to make patches
  const double scalMin = 0.999847695; // cos(PI/180)

  // iterate on faces from the first shape
  int i, j;
  for (i = 1; i <= myONFacesMap.Extent(); i++)
  {
    const TopoDS_Shape& aFAnc1 = myONFacesMap(i);
    if (myDataStructure->DS().AncestorRank(aFAnc1) == 1)
    {
      const TopoDS_Face& aFace1 = TopoDS::Face(myONFacesMap.FindKey(i));
      // map edges of the first face
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapE1;
      TopExp::MapShapes(aFace1, TopAbs_EDGE, aMapE1);
      // find a non-degenerated edge
      TopoDS_Edge aChkEdge;
      int         k;
      for (k = 1; k <= aMapE1.Extent() && aChkEdge.IsNull(); k++)
      {
        const TopoDS_Edge& aE = TopoDS::Edge(aMapE1(k));
        if (!BRep_Tool::Degenerated(aE))
          aChkEdge = aE;
      }
      if (aChkEdge.IsNull())
        continue;
      // find a point and a normal
      BRepAdaptor_Curve2d aBAC1(aChkEdge, aFace1);
      gp_Pnt2d            aP2d;
      const double        PAR_T = 0.456321;
      double par = aBAC1.FirstParameter() * (1. - PAR_T) + aBAC1.LastParameter() * PAR_T;
      aBAC1.D0(par, aP2d);
      BRepAdaptor_Surface aBAS1(aFace1);
      gp_Pnt              aPbid;
      gp_Vec              aN1, aDU, aDV;
      aBAS1.D1(aP2d.X(), aP2d.Y(), aPbid, aDU, aDV);
      aN1         = aDU ^ aDV;
      double norm = aN1.Magnitude();
      if (norm < Precision::Confusion())
        continue;
      aN1 /= norm;
      if (aFace1.Orientation() == TopAbs_REVERSED)
        aN1.Reverse();

      // iterate on faces from the second shape
      bool ok = true;
      for (j = i + 1; j <= myONFacesMap.Extent() && ok; j++)
      {
        const TopoDS_Shape& aFAnc2 = myONFacesMap(j);
        if (myDataStructure->DS().AncestorRank(aFAnc2) == 2)
        {
          const TopoDS_Face& aFace2 = TopoDS::Face(myONFacesMap.FindKey(j));

          // check that the second face has the same boundaries
          NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapE2;
          TopExp::MapShapes(aFace2, TopAbs_EDGE, aMapE2);
          if (aMapE1.Extent() != aMapE2.Extent())
            continue;
          bool sameBnd = true;
          for (k = 1; k <= aMapE2.Extent() && sameBnd; k++)
            if (!aMapE1.Contains(aMapE2(k)))
              sameBnd = false;
          if (!sameBnd)
            continue;

          // check if it is needed to have a patch here;
          // for that the normals should be oriented in the same sense.
          BRepAdaptor_Curve2d aBAC2(aChkEdge, aFace2);
          aBAC2.D0(par, aP2d);
          BRepAdaptor_Surface aBAS2(aFace2);
          gp_Vec              aN2;
          aBAS2.D1(aP2d.X(), aP2d.Y(), aPbid, aDU, aDV);
          aN2  = aDU ^ aDV;
          norm = aN2.Magnitude();
          if (norm < Precision::Confusion())
          {
            ok = false;
            continue;
          }
          aN2 /= norm;
          if (aFace2.Orientation() == TopAbs_REVERSED)
            aN2.Reverse();
          double scal = aN1 * aN2;
          if (scal < scalMin)
          {
            ok = false;
            continue;
          }

          // select one of the two faces
          bool               takeFirst = true;
          TopoDS_Face        aPatch;
          TopAbs_Orientation neworiF;
          if (takeFirst)
          {
            aPatch  = aFace1;
            neworiF = Orient(aFAnc1.Orientation(), G1.IsToReverse1());
          }
          else
          {
            aPatch  = aFace2;
            neworiF = Orient(aFAnc2.Orientation(), G1.IsToReverse2());
          }
          aPatch.Orientation(neworiF);

          // add patch to SFS
          SFS.AddStartElement(aPatch);

          // save ON splits
          MarkSplit(aFAnc1, TopAbs_ON);
          NCollection_List<TopoDS_Shape>& aLOFS1 = ChangeSplit(aFAnc1, TopAbs_ON);
          aLOFS1.Append(aFace1);
          MarkSplit(aFAnc2, TopAbs_ON);
          NCollection_List<TopoDS_Shape>& aLOFS2 = ChangeSplit(aFAnc2, TopAbs_ON);
          aLOFS2.Append(aFace2);
        }
      }
    }
  }
}

//=================================================================================================

static bool AreFacesCoincideInArea(const TopoDS_Shape&                   theBaseFace,
                                   const TopoDS_Shape&                   theFace,
                                   const TopoDS_Shape&                   theEdge,
                                   const NCollection_List<TopoDS_Shape>& allEdges,
                                   bool&                                 isSameOri)
{
  // there are given:
  // theBaseFace, theFace - possibly coinciding faces;
  // allEdges - the edges lying on theBaseFace forming the new boundary loops,
  //            they determine the areas of coincidence;
  // theEdge - an edge from allEdges pointing to the area to check in.
  // we should check that the faces are coincide in this area and have
  // the same orientation considering the orientations of the faces.

  TopAbs_Orientation anEdgeOri = theEdge.Orientation();
  if (anEdgeOri != TopAbs_FORWARD && anEdgeOri != TopAbs_REVERSED)
    return false;
  bool reverse = (anEdgeOri == TopAbs_REVERSED);

  TopoDS_Face  aBaseFace = TopoDS::Face(theBaseFace);
  TopoDS_Face  aFace     = TopoDS::Face(theFace);
  TopoDS_Edge  anEdge    = TopoDS::Edge(theEdge);
  BRep_Builder BB;

  // create a ray from the inside of anEdge to the matter side
  double                    pf, pl, tol;
  bool                      trim3d = true;
  occ::handle<Geom2d_Curve> PCref  = BRep_Tool::CurveOnSurface(anEdge, aBaseFace, pf, pl);
  if (PCref.IsNull())
  {
    PCref = FC2D_CurveOnSurface(anEdge, aBaseFace, pf, pl, tol, trim3d);
    if (PCref.IsNull())
      return false;
    tol = BRep_Tool::Tolerance(anEdge);
    BB.UpdateEdge(anEdge, PCref, aBaseFace, tol);
  }

  const double T  = 0.456789;
  double       pm = (1. - T) * pf + T * pl;
  gp_Pnt2d     pt;
  gp_Vec2d     d1;
  PCref->D1(pm, pt, d1);
  if (d1.Magnitude() < gp::Resolution())
    return false;
  if (reverse)
    d1.Reverse();
  gp_Vec2d vecInside(-d1.Y(), d1.X());
  gp_Lin2d aLin(pt, vecInside);

  // find the nearest intersection of aLin with other edges
  bool                  hasInt  = false;
  double                pLinMin = RealLast();
  double                tol2d   = Precision::PConfusion();
  BRepClass_Intersector anInter;
  BRepClass_Edge        aBCE;
  aBCE.Face()    = aBaseFace;
  double maxDist = std::max(BRep_Tool::Tolerance(aBaseFace), BRep_Tool::Tolerance(aFace));

  bool                                     isError = false;
  NCollection_List<TopoDS_Shape>::Iterator it(allEdges);
  for (; it.More() && !isError; it.Next())
  {
    const TopoDS_Edge& aE   = TopoDS::Edge(it.Value());
    double             tolE = BRep_Tool::Tolerance(aE);
    if (tolE > maxDist)
      maxDist = tolE;
    if (aE.IsEqual(anEdge)
        || (aE.Orientation() != TopAbs_FORWARD && aE.Orientation() != TopAbs_REVERSED
            && aE.IsSame(anEdge)))
      continue; // the same pcurve
    occ::handle<Geom2d_Curve> PC = BRep_Tool::CurveOnSurface(aE, aBaseFace, pf, pl);
    if (PC.IsNull())
    {
      PC = FC2D_CurveOnSurface(aE, aBaseFace, pf, pl, tol, trim3d);
      if (PC.IsNull())
      {
        isError = true;
        break;
      }
      BB.UpdateEdge(aE, PC, aBaseFace, tolE);
    }
    aBCE.Edge() = aE;
    anInter.Perform(aLin, pLinMin, tol2d, aBCE);
    if (anInter.IsDone())
    {
      int i;
      for (i = 1; i <= anInter.NbPoints(); i++)
      {
        const IntRes2d_IntersectionPoint& aIP  = anInter.Point(i);
        double                            pLin = aIP.ParamOnFirst();
        if (pLin > tol2d && pLin < pLinMin)
        {
          pLinMin = pLin;
          hasInt  = true;
        }
      }
      for (i = 1; i <= anInter.NbSegments() && !isError; i++)
      {
        const IntRes2d_IntersectionSegment& aIS = anInter.Segment(i);
        double                              pLinF =
          aIS.HasFirstPoint() ? aIS.FirstPoint().ParamOnFirst() : -Precision::Infinite();
        double pLinL = aIS.HasLastPoint() ? aIS.LastPoint().ParamOnFirst() : Precision::Infinite();
        if (pLinF < tol2d && pLinL > -tol2d)
          isError = true;
        else if (pLinF > tol2d && pLinF < pLinMin)
        {
          pLinMin = pLinF;
          hasInt  = true;
        }
      }
    }
  }
  if (isError || !hasInt)
    return false;

  // create a point in the area and get the normal to aBaseFace at it
  gp_Pnt2d            aP2d = ElCLib::Value(pLinMin * T, aLin);
  BRepAdaptor_Surface aBAS(aBaseFace);
  gp_Pnt              aPnt;
  gp_Vec              d1u, d1v;
  aBAS.D1(aP2d.X(), aP2d.Y(), aPnt, d1u, d1v);
  gp_Vec aNormBase = d1u ^ d1v;
  double mag       = aNormBase.Magnitude();
  if (mag < gp::Resolution())
    return false;
  if (aBaseFace.Orientation() == TopAbs_REVERSED)
    mag = -mag;
  aNormBase /= mag;

  // project the point aPnt to the second face aFace
  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace);
  double                    umin, umax, vmin, vmax;
  BRepTools::UVBounds(aFace, umin, umax, vmin, vmax);
  GeomAPI_ProjectPointOnSurf aProj(aPnt, aSurf, umin, umax, vmin, vmax);
  if (!aProj.NbPoints() || aProj.LowerDistance() > maxDist)
    return false;
  double u, v;
  aProj.LowerDistanceParameters(u, v);
  aSurf->D1(u, v, aPnt, d1u, d1v);
  gp_Vec aNorm = d1u ^ d1v;
  mag          = aNorm.Magnitude();
  if (mag < gp::Resolution())
    return false;
  if (aFace.Orientation() == TopAbs_REVERSED)
    mag = -mag;
  aNorm /= mag;

  // check normales
  double       dot    = aNormBase * aNorm;
  const double minDot = 0.9999;
  if (std::abs(dot) < minDot)
    return false;
  isSameOri = (dot > 0.);

  return true;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::FillOnPatches(const NCollection_List<TopoDS_Shape>&       anEdgesON,
                                            const TopoDS_Shape&                         aBaseFace,
                                            const NCollection_IndexedMap<TopoDS_Shape>& avoidMap)
{
  TopoDS_Shape FF = aBaseFace;
  FF.Orientation(TopAbs_FORWARD);
  int rankBF = ShapeRank(aBaseFace);
  int rankOpp;
  if (rankBF == 1)
    rankOpp = 2;
  else if (rankBF == 2)
    rankOpp = 1;
  else
    return;

  TopOpeBRepBuild_WireEdgeSet                                     WES(FF, this);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>          aMapON, aMapON1;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> aMapFState;

  NCollection_List<TopoDS_Shape>           allEdges;
  NCollection_List<TopoDS_Shape>::Iterator it;
  TopoDS_Iterator                          itW;
  for (it.Initialize(anEdgesON); it.More(); it.Next())
  {
    const TopoDS_Shape& aE = it.Value();
    // is it a part of the boundary of aBaseFace ?
    if (!myONElemMap.Contains(aE) && !myONElemMap.Contains(aE.Reversed()) && !avoidMap.Contains(aE))
    {
      allEdges.Append(aE);
      aMapON.Add(aE);
    }
  }
  int  i;
  bool hasWires = false;
  for (i = 1; i <= myONElemMap.Extent(); i++)
  {
    const TopoDS_Shape& aE = myONElemMap(i);
    if (aE.ShapeType() == TopAbs_WIRE)
    {
      for (itW.Initialize(aE); itW.More(); itW.Next())
        if (!avoidMap.Contains(itW.Value()))
        {
          allEdges.Append(itW.Value());
          hasWires = true;
        }
    }
    else if (!avoidMap.Contains(aE))
    {
      allEdges.Append(aE);
      aMapON1.Add(aE);
    }
  }

  // +++
  // add elements from anEdgesON (they come from BuilderON)
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> anAncMap;
  if (!aMapON.IsEmpty())
    FillSecEdgeAncestorMap(rankOpp, aMapON, anAncMap);
  if (!anAncMap.IsEmpty())
  {
    for (it.Initialize(anEdgesON); it.More(); it.Next())
    {
      const TopoDS_Shape& aE = it.Value(); // an ON part
      if (anAncMap.IsBound(aE) && !avoidMap.Contains(aE))
      {
        const TopoDS_Shape& anAncE = anAncMap(aE);     // its ancestor edge from opposite shape
        const NCollection_List<TopoDS_Shape>& aFaces = // connex faces of anAncE
          FDSCNX_EdgeConnexityShapeIndex(anAncE, myDataStructure, rankOpp);
        // determine if aBaseFace has coinciding part on the left side of aE
        // with one of connex faces, and this pair of faces are same oriented
        bool                                     isOnFace = false;
        NCollection_List<TopoDS_Shape>           aFacesToCheck;
        NCollection_List<TopoDS_Shape>::Iterator itF;
        for (itF.Initialize(aFaces); itF.More() && !isOnFace; itF.Next())
        {
          const TopoDS_Shape& aF = itF.Value();
          if (aMapFState.IsBound(aF))
          {
            int state = aMapFState(aF);
            if (state)
              isOnFace = true;
          }
          else
            aFacesToCheck.Append(aF);
        }
        for (itF.Initialize(aFacesToCheck); itF.More() && !isOnFace; itF.Next())
        {
          const TopoDS_Shape& aF        = itF.Value();
          bool                isSameOri = false;
          bool                ok;
          if (aE.Orientation() != TopAbs_FORWARD && aE.Orientation() != TopAbs_REVERSED)
          {
            ok = AreFacesCoincideInArea(aBaseFace,
                                        aF,
                                        aE.Oriented(TopAbs_FORWARD),
                                        allEdges,
                                        isSameOri);
            ok = ok
                 || AreFacesCoincideInArea(aBaseFace,
                                           aF,
                                           aE.Oriented(TopAbs_REVERSED),
                                           allEdges,
                                           isSameOri);
          }
          else
            ok = AreFacesCoincideInArea(aBaseFace, aF, aE, allEdges, isSameOri);
          if (ok && isSameOri)
          {
            aMapFState.Bind(aF, 1);
            isOnFace = true;
          }
          else
            aMapFState.Bind(aF, 0);
        }
        if (isOnFace)
          WES.AddStartElement(aE);
      }
    }
  }

  // +++
  // add elements from myONElemMap (consisting of parts of the boundary of aBaseFace)
  anAncMap.Clear();
  if (!aMapON1.IsEmpty())
    FillSecEdgeAncestorMap(rankBF, aMapON1, anAncMap);
  if (hasWires || !anAncMap.IsEmpty())
  {
    for (i = 1; i <= myONElemMap.Extent(); i++)
    {
      const TopoDS_Shape& aE = myONElemMap(i);
      TopoDS_Shape        anEdge, anAncE;
      if (aE.ShapeType() == TopAbs_WIRE)
      {
        // for a wire get one non-degenerated edge for test
        for (itW.Initialize(aE); itW.More() && anEdge.IsNull(); itW.Next())
        {
          const TopoDS_Edge& e = TopoDS::Edge(itW.Value());
          if (avoidMap.Contains(e))
            break;
          if (!BRep_Tool::Degenerated(e))
            anEdge = anAncE = e;
        }
      }
      else if (anAncMap.IsBound(aE) && !avoidMap.Contains(aE))
      {
        anEdge = aE;
        anAncE = anAncMap(aE);
      }
      if (!anEdge.IsNull())
      {
        // find faces of the opposite shape touching anAncE
        NCollection_List<TopoDS_Shape> aFaces;
        FDSCNX_FaceEdgeConnexFaces(aBaseFace, anAncE, myDataStructure, aFaces);
        if (aFaces.IsEmpty())
          continue;
        TopoDS_Shape aCnxF = aFaces.First();
        aFaces.Clear();
        FindFacesTouchingEdge(aCnxF, anAncE, rankOpp, aFaces);
        // determine if aBaseFace has coinciding part on the left side of anEdge
        // with one of found faces, and this pair of faces are same oriented
        bool                                     isOnFace = false;
        NCollection_List<TopoDS_Shape>           aFacesToCheck;
        NCollection_List<TopoDS_Shape>::Iterator itF;
        for (itF.Initialize(aFaces); itF.More() && !isOnFace; itF.Next())
        {
          const TopoDS_Shape& aF = itF.Value();
          if (aMapFState.IsBound(aF))
          {
            int state = aMapFState(aF);
            if (state)
              isOnFace = true;
          }
          else
            aFacesToCheck.Append(aF);
        }
        for (itF.Initialize(aFacesToCheck); itF.More() && !isOnFace; itF.Next())
        {
          const TopoDS_Shape& aF        = itF.Value();
          bool                isSameOri = false;
          bool ok = AreFacesCoincideInArea(aBaseFace, aF, anEdge, allEdges, isSameOri);
          if (ok && isSameOri)
          {
            aMapFState.Bind(aF, 1);
            isOnFace = true;
          }
          else
            aMapFState.Bind(aF, 0);
        }
        if (isOnFace)
        {
          if (aE.ShapeType() == TopAbs_WIRE)
            WES.AddShape(aE);
          else
            WES.AddStartElement(aE);
        }
      }
    }
  }

  WES.InitShapes();
  WES.InitStartElements();
  if (WES.MoreShapes() || WES.MoreStartElements())
  {
    NCollection_List<TopoDS_Shape> LOF;
    GWESMakeFaces(FF, WES, LOF);
    // save ON faces
    for (it.Initialize(LOF); it.More(); it.Next())
    {
      const TopoDS_Face& aF = TopoDS::Face(it.Value());
      myONFacesMap.Add(aF, aBaseFace);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::FindFacesTouchingEdge(const TopoDS_Shape&             aFace,
                                                    const TopoDS_Shape&             anEdge,
                                                    const int                       aShRank,
                                                    NCollection_List<TopoDS_Shape>& aFaces) const
{
  const TopOpeBRepDS_DataStructure& BDS       = myDataStructure->DS();
  int                               anEdgeInd = BDS.Shape(anEdge);
  if (!anEdgeInd)
    return;

  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI =
    BDS.ShapeInterferences(aFace);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ILI(LI);
  for (; ILI.More(); ILI.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>&    I = ILI.Value();
    occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI =
      occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I);
    if (SSI.IsNull())
      continue;
    TopOpeBRepDS_Kind GT, ST;
    int               GI, SI;
    FDS_data(SSI, GT, GI, ST, SI);
    if (GT != TopOpeBRepDS_EDGE || ST != TopOpeBRepDS_FACE)
      continue;
    if (GI != anEdgeInd)
      continue;
    const TopOpeBRepDS_Transition& TFE = SSI->Transition();
    if (TFE.ShapeBefore() != TopAbs_FACE || TFE.ShapeAfter() != TopAbs_FACE)
      continue;
    const TopoDS_Shape& FS = BDS.Shape(SI);
    if (ShapeRank(FS) != aShRank)
      continue;
    aFaces.Append(FS);
  }
}
