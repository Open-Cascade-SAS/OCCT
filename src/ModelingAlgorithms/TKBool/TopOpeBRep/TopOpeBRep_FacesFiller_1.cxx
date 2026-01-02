// Created on: 1994-02-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BndLib_Add3dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_GeomTool.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_defineG.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT void debrest(const int i)
{
  std::cout << "+ debrest " << i << std::endl;
}

Standard_EXPORT void debrline()
{
  std::cout << "+ debrline" << std::endl;
}

extern bool TopOpeBRep_GettraceNVP(int a, int b, int c, int d, int e);
extern bool GLOBAL_bvpr;
extern void debvprmess(int f1, int f2, int il, int vp, int si);
extern bool TopOpeBRep_GetcontextNOPUNK();

static void FUN_traceRLine(const TopOpeBRep_LineInter&)
{}

static void FUN_traceGLine(const TopOpeBRep_LineInter&) {}
#endif

#define M_FORWARD(o) (o == TopAbs_FORWARD)
#define M_REVERSED(o) (o == TopAbs_REVERSED)

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

Standard_EXPORT void FUN_GetdgData(
  TopOpeBRepDS_PDataStructure& pDS,
  const TopOpeBRep_LineInter&  L,
  const TopoDS_Face&           F1,
  const TopoDS_Face&           F2,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    datamap);
Standard_EXPORT void FUN_FillVof12(const TopOpeBRep_LineInter& L, TopOpeBRepDS_PDataStructure pDS);

#define M_FINDVP (0)  // only look for new vp
#define M_MKNEWVP (1) // only make newvp
#define M_GETVP (2)   // steps (0) [+(1) if (O) fails]
Standard_EXPORT void FUN_VPIndex(
  TopOpeBRep_FacesFiller&                                         FF,
  const TopOpeBRep_LineInter&                                     L,
  const TopOpeBRep_VPointInter&                                   VP,
  const int                                                       ShapeIndex,
  const occ::handle<TopOpeBRepDS_HDataStructure>&                 HDS,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
  TopOpeBRepDS_Kind&                                              PVKind,
  int&                                                            PVIndex, // out
  bool&                                                           EPIfound,
  occ::handle<TopOpeBRepDS_Interference>&                         IEPI, // out
  bool&                                                           CPIfound,
  occ::handle<TopOpeBRepDS_Interference>&                         ICPI, // out
  const int                                                       mkVP);
Standard_EXPORT bool FUN_newtransEdge(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const TopOpeBRep_FacesFiller&                   FF,
                                      const TopOpeBRep_LineInter&                     L,
                                      const bool&                                     Lonrest,
                                      const TopOpeBRep_VPointInter&                   VP,
                                      const TopOpeBRepDS_Kind                         PVKind,
                                      const int                                       PVIndex,
                                      const int&                                      OOShapeIndex,
                                      const TopoDS_Edge&                              edge,
                                      const NCollection_List<TopoDS_Shape>&           ERL,
                                      TopOpeBRepDS_Transition&                        T);
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)

static bool FUN_IwithsuppiS(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loI,
                            const int                                                       iS,
                            NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loIfound)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    if (I->Support() == iS)
      loIfound.Append(I);
  }
  bool ok = (loIfound.Extent() > 0);
  return ok;
}

static bool FUN_IwithsuppkS(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loI,
                            const TopOpeBRepDS_Kind&                                        kS,
                            NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loIfound)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    if (I->SupportType() == kS)
      loIfound.Append(I);
  }
  bool ok = (loIfound.Extent() > 0);
  return ok;
}

static bool FUN_IwithToniS(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loI,
                           const int                                                       iS,
                           NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       loIfound)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    if (I->Transition().Index() == iS)
      loIfound.Append(I);
  }
  bool ok = (loIfound.Extent() > 0);
  return ok;
}

static bool FUN_supponF(const TopOpeBRepDS_PDataStructure                               pDS,
                        const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loI,
                        const int                                                       iF,
                        NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lIsupponF,
                        NCollection_List<int>&                                          losupp)
{
  //<losupp> = list of support S / I in <loI> : I = (T,G,S = Edge on <F>);
  bool ok = (0 < iF) && (iF <= pDS->NbShapes());
  if (!ok)
    return false;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MOOE;
  TopExp::MapShapes(pDS->Shape(iF), TopAbs_EDGE, MOOE);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I     = it.Value();
    int                                           iS    = I->Support();
    TopOpeBRepDS_Kind                             skind = I->SupportType();
    bool                                          add   = false;
    if (skind == TopOpeBRepDS_EDGE)
      add = MOOE.Contains(pDS->Shape(iS));
    if (add)
    {
      losupp.Append(iS);
      lIsupponF.Append(I);
    }
  }
  return losupp.Extent() >= 1;
}

static bool FUN_IoflSsuppS(const TopOpeBRepDS_PDataStructure                         pDS,
                           const int                                                 iS,
                           const NCollection_List<int>&                              lShape,
                           NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& IsuppiS)
{
  bool ok = false;
  // E in <losupp> /
  // I on E : I = (T, G, S=iS)

  // looking for interferences attached to shapes of <lShape> with support <iS>
  NCollection_List<int>::Iterator iti(lShape);
  for (; iti.More(); iti.Next())
  {
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lI =
      pDS->ShapeInterferences(iti.Value());
    ok = FUN_IwithsuppiS(lI, iS, IsuppiS);
  }
  return ok;
}

//=======================================================================
// 3D
// purpose : The compute of a transition edge/face given interferences
//           attached to the edge (stored in the DS).
//=======================================================================
static bool FUN_findTF(const TopOpeBRepDS_PDataStructure pDS,
                       const int                         iE,
                       const int,
                       const int                iOOF,
                       TopOpeBRepDS_Transition& TF)
{
  double factor = 0.5;
  // ----------------------------------------------------------------------
  // <Ifound> on <E> : Ifound = (T, S=OOF, G=POINT/VERTEX on <E>)
  //                            (T, S=edge of OOF, G=POINT/VERTEX on <E>)
  // ----------------------------------------------------------------------

  // <lITonOOF>
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lITonOOF;
  bool ok = FUN_IwithToniS(pDS->ShapeInterferences(iE), iOOF, lITonOOF);
  if (!ok)
    return false;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lITOOFskFACE;
  bool found = FUN_IwithsuppkS(lITonOOF, TopOpeBRepDS_FACE, lITOOFskFACE);
  if (found)
  {
    // NYI : a deeper analysis is needed, for the moment, we make the following
    // prequesitory : transition on E of F on point of ES is valid for
    //                all the ES (here restriction) ie :
    //                TF : transition face(F) / face(OOF) on G = ES =
    //                TE : transition edge(E) / face(OOF) at G = POINT/VERTEX
    // Ifound on <E> : Ifound = (T(on face OOF), S=FACE, G=POINT/VERTEX on <E>)
    const occ::handle<TopOpeBRepDS_Interference>& Ifound = lITOOFskFACE.First();
    TF                                                   = Ifound->Transition();
  }

  bool                                                     done = false;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lITOOFskEDGE;
  if (!found)
    done = FUN_IwithsuppkS(lITonOOF, TopOpeBRepDS_EDGE, lITOOFskEDGE);
  if (done)
  {
    // Ifound on <E> : Ifound = (T(on face OOF), S=FACE, G=POINT/VERTEX on <E>)
    // if <Ifound> found : compute TE at G / <OOF>.
    // TE ->TF.
    const occ::handle<TopOpeBRepDS_Interference>& Ifound = lITOOFskEDGE.First();
    const TopoDS_Edge&                            OOE = TopoDS::Edge(pDS->Shape(Ifound->Support()));
    double                                        paronE;
    bool                                          OOdone = FDS_Parameter(Ifound, paronE);
    if (!OOdone)
      return false;

    const TopoDS_Edge& E   = TopoDS::Edge(pDS->Shape(iE));
    const TopoDS_Face& OOF = TopoDS::Face(pDS->Shape(iOOF));

    double f, l;
    FUN_tool_bounds(E, f, l);
    TopOpeBRepTool_makeTransition MKT;

    bool OOEboundOOF = FUN_tool_EboundF(OOE, OOF);
    bool iscl        = TopOpeBRepTool_TOOL::IsClosingE(OOE, OOF);
    if (OOEboundOOF && (!iscl))
    {
      double oopar;
      bool   ok1 = FUN_tool_parE(E, paronE, OOE, oopar);
      if (!ok1)
        return false;
      gp_Pnt2d uv;
      ok1 = FUN_tool_paronEF(OOE, oopar, OOF, uv);
      if (!ok1)
        return false;

      ok = MKT.Initialize(E, f, l, paronE, OOF, uv, factor);
      if (ok)
        ok = MKT.SetRest(OOE, oopar);
    }
    else
    {
      gp_Pnt2d uv;
      bool     ok1 = FUN_tool_parF(E, paronE, OOF, uv);
      if (!ok1)
        return false;

      ok = MKT.Initialize(E, f, l, paronE, OOF, uv, factor);
    }
    TopAbs_State stb, sta;
    ok = MKT.MkTonE(stb, sta);
    if (!ok)
      return false;
    TF.Before(stb);
    TF.After(sta);
    return true;
  }
  ok = found || done;
  return ok;
}

static bool FUN_findTOOF(const TopOpeBRepDS_PDataStructure pDS,
                         const int                         iE,
                         const int                         iF,
                         const int                         iOOF,
                         TopOpeBRepDS_Transition&          TOOF)
{
  double factor = 0.5;

  // ----------------------------------------------------------------------
  // <E> bound of <F>,
  // <OOE> on <OOF> /
  // <OOIfound> on <OOE>  : OOIfound = (T, S=iF, G=POINT/VERTEX on <E>)
  // ----------------------------------------------------------------------

  // <lIsuppOOE> = list of interferences attached to <E> of support S = edge of <OOF>
  // <liOOE> = list of supports of <lIsuppOOE>.
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loIE =
    pDS->ShapeInterferences(iE);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lITonOOF;
  bool ok = FUN_IwithToniS(loIE, iOOF, lITonOOF);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lIsuppOOE;
  NCollection_List<int>                                    liOOEGonE;
  if (ok)
  {
    ok = FUN_IwithsuppkS(lITonOOF, TopOpeBRepDS_EDGE, lIsuppOOE);
    if (ok)
    {
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lIsuppOOE);
      for (; it.More(); it.Next())
        liOOEGonE.Append(it.Value()->Support());
    }
  }
  else
    ok = FUN_supponF(pDS, loIE, iOOF, lIsuppOOE, liOOEGonE);
  if (!ok)
    return false;

  //  TopAbs_Orientation oritransOOE;

  // <lOOIfound> = list of I attached to shapes of <liOOE> /
  //               I = (T, S=F, G=POINT/VERTEX on <E>)
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lIOOEsuppFGonE;
  bool OOfound = FUN_IoflSsuppS(pDS, iF, liOOEGonE, lIOOEsuppFGonE);
  if (OOfound)
  {
    // NYI : a deeper analysis is needed, for the moment, we make the following
    // prequesitory : transition on OOE of OOF on point of ES is valid for
    //                all the ES (here restriction) ie :
    //                TOOF : transition face(OOF) / face(F) on (G == ES)
    //            <=> TOOE : transition edge(OOE) / face(F) at G = POINT/VERTEX
    const occ::handle<TopOpeBRepDS_Interference>& OOIfound = lIOOEsuppFGonE.First();
    TOOF                                                   = OOIfound->Transition();
  }

  bool OOdone = false;
  if (!OOfound)
  {
    // Ifound on <E> : Ifound = (T, S=EDGE on <OOF>, G=POINT/VERTEX on <E>)
    // if <Ifound> found : compute TOOE at G / <F>
    // TOOE ->TOOF.
    const occ::handle<TopOpeBRepDS_Interference>& Ifound = lIsuppOOE.First();
    const TopoDS_Edge&                            OOE = TopoDS::Edge(pDS->Shape(Ifound->Support()));
    double                                        paronE;
    OOdone = FDS_Parameter(Ifound, paronE);
    if (!OOdone)
      return false;

    const TopoDS_Edge& E = TopoDS::Edge(pDS->Shape(iE));
    const TopoDS_Face& F = TopoDS::Face(pDS->Shape(iF));

    double oopar;
    bool   ok1 = FUN_tool_parE(E, paronE, OOE, oopar);
    if (!ok1)
      return false;
    gp_Pnt2d uv;
    ok1 = FUN_tool_paronEF(E, paronE, F, uv);
    if (!ok1)
      return false;
    double f, l;
    FUN_tool_bounds(OOE, f, l);

    TopAbs_State                  stb = TopAbs_UNKNOWN, sta = TopAbs_UNKNOWN;
    TopOpeBRepTool_makeTransition MKT;
    OOdone = MKT.Initialize(OOE, f, l, oopar, F, uv, factor);
    if (OOdone)
      OOdone = MKT.SetRest(E, paronE);
    if (OOdone)
      OOdone = MKT.MkTonE(stb, sta);
    if (OOdone)
    {
      TOOF.Before(stb);
      TOOF.After(sta);
    }
  }
  ok = OOfound || OOdone;
  return ok;
}

Standard_EXPORT bool GLOBAL_btcx = false;

Standard_EXPORT void debtcxmess(int f1, int f2, int il)
{
  std::cout << "f1,f2,il : " << f1 << "," << f2 << "," << il << std::endl;
  std::cout.flush();
}

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessLine()
{
  bool reject = (!myLineOK || myLine == nullptr);
  if (reject)
    return;
  ResetDSC();

  bool HasVPonR = myLine->HasVPonR();
  if (HasVPonR)
    FillLineVPonR();
  else
    FillLine();

  bool inl = myLine->INL();
  if (inl)
    return;

  myHDS->SortOnParameter(myDSCIL);

  AddShapesLine();
}

//=================================================================================================

void TopOpeBRep_FacesFiller::ResetDSC()
{
  myDSCIndex = 0;
  myDSCIL.Clear();
}

//=======================================================================
// function : ProcessVPInotonR
// purpose  : Same as ProcessVPnotonR.
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessVPInotonR(TopOpeBRep_VPointInterIterator& VPI)
{
  const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
  ProcessVPnotonR(VP);
}

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessVPnotonR(const TopOpeBRep_VPointInter& VP)
{
  int ShapeIndex = 0;
  int iVP        = VP.Index();

#ifdef OCCT_DEBUG
  int ili = myLine->Index(), ivp = iVP, isi = ShapeIndex;
  GLOBAL_bvpr = TopOpeBRep_GettraceNVP(myexF1, myexF2, ili, ivp, isi);
  if (GLOBAL_bvpr)
    debvprmess(myexF1, myexF2, ili, ivp, isi);
#endif

  int iINON1, iINONn, nINON;
  myLine->VPBounds(iINON1, iINONn, nINON);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itCPIL(myDSCIL);

  TopOpeBRepDS_Kind PVKind;
  int               PVIndex;
  bool              CPIfound = GetGeometry(itCPIL, VP, PVIndex, PVKind);
  if (!CPIfound)
  {
    if (iVP != iINON1 && iVP != iINONn)
    {
#ifdef OCCT_DEBUG
      std::cout << "VP " << iVP << " on " << 0 << " : point d'intersection anormal : rejet"
                << std::endl;
#endif
      return;
    }
  }

  if (!CPIfound)
  {
    bool found = GetFFGeometry(VP, PVKind, PVIndex);
    if (!found)
      PVIndex = MakeGeometry(VP, ShapeIndex, PVKind);
  }

  TopOpeBRepDS_Transition transLine;
  if (CPIfound)
  {
    const occ::handle<TopOpeBRepDS_Interference>& I  = itCPIL.Value();
    const TopOpeBRepDS_Transition&                TI = I->Transition();
    transLine                                        = TI.Complement();
  }
  else
  {
    if (iVP == iINON1)
      transLine.Set(TopAbs_FORWARD);
    else if (iVP == iINONn)
      transLine.Set(TopAbs_REVERSED);
  }

  double                                 parline = VP.ParameterOnLine();
  occ::handle<TopOpeBRepDS_Interference> CPI =
    TopOpeBRepDS_InterferenceTool::MakeCurveInterference(transLine,
                                                         TopOpeBRepDS_CURVE,
                                                         0,
                                                         PVKind,
                                                         PVIndex,
                                                         parline);
  StoreCurveInterference(CPI);

} // ProcessVPnotonR

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessVPR(TopOpeBRep_FacesFiller&       FF,
                                        const TopOpeBRep_VPointInter& VP)
{
  TopOpeBRepDS_Transition LineTonF1 = FaceFaceTransition(1);
  TopOpeBRepDS_Transition LineTonF2 = FaceFaceTransition(2);
  TopoDS_Face             F1        = myF1;
  TopoDS_Face             F2        = myF2;
  // --- check interiority of VPoint to the restrictions
  bool tokeep = VP.Keep();
  if (!tokeep)
    return;

  int ShapeIndex = VP.ShapeIndex();

  if (ShapeIndex == 0)
  {
    FF.ProcessVPnotonR(VP);
  }
  else if (ShapeIndex == 1)
  {
    FF.ProcessVPonR(VP, LineTonF1, F1, 1);
  }
  else if (ShapeIndex == 2)
  {
    FF.ProcessVPonR(VP, LineTonF2, F2, 2);
  }
  else if (ShapeIndex == 3)
  {

    bool isV1 = VP.IsVertexOnS1();
    bool isV2 = VP.IsVertexOnS2();

    int shin1 = 1;
    if (isV2 && !isV1)
      shin1 = 2;

    if (shin1 == 1)
    {
      FF.ProcessVPonR(VP, LineTonF1, F1, 1);
      FF.ProcessVPonR(VP, LineTonF2, F2, 2);
    }
    else if (shin1 == 2)
    {
      FF.ProcessVPonR(VP, LineTonF2, F2, 2);
      FF.ProcessVPonR(VP, LineTonF1, F1, 1);
    }
  }
} // FUNvponr

static bool FUN_brep_ONfirstP(const TopOpeBRep_VPointInter& vpf, const TopOpeBRep_VPointInter& VP)
// prequesitory : gline is on edge
{
  double parfirst = vpf.ParameterOnLine();
  double parcur   = VP.ParameterOnLine();
  double d        = parcur - parfirst;
  double tol      = Precision::Confusion(); // nyixpu051098 : see lbr...
  bool   ONfirstP = (std::abs(d) < tol);
  return ONfirstP;
}

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessRLine()
{
  if (myLine->TypeLineCurve() != TopOpeBRep_RESTRICTION)
  {
    return;
  }

  bool addIFE = true;
  if (!addIFE)
    return;

  const TopoDS_Edge& Erest    = TopoDS::Edge(myLine->Arc());
  bool               FIisrest = myFacesIntersector->IsRestriction(Erest);
  if (!FIisrest)
    return;

  bool isedge1     = myLine->ArcIsEdge(1);
  bool isedge2     = myLine->ArcIsEdge(2);
  int  EShapeIndex = (isedge1) ? 1 : (isedge2) ? 2 : 0;

  int iErest = myDS->AddShape(Erest, EShapeIndex);
  int rank   = myDS->AncestorRank(iErest);
  int OOrank = (rank == 1) ? 2 : 1;

  int                                    iF1 = myDS->AddShape(myF1, 1);
  int                                    iF2 = myDS->AddShape(myF2, 2);
  occ::handle<TopOpeBRepDS_Interference> IFE;

  TopOpeBRepDS_Transition T1 = FaceFaceTransition(1);
  T1.Index(iF2);
  TopOpeBRepDS_Transition T2 = FaceFaceTransition(2);
  T2.Index(iF1);

  bool T1unk      = T1.IsUnknown();
  bool T2unk      = T2.IsUnknown();
  bool processUNK = false;
#ifdef OCCT_DEBUG
  bool nopunk = TopOpeBRep_GetcontextNOPUNK();
  if (nopunk)
    processUNK = false;
#endif
  if (processUNK && (T1unk || T2unk))
  {
    TopoDS_Shape F        = (*this).Face(rank);
    int          iF       = myDS->Shape(F);
    TopoDS_Shape OOF      = (*this).Face(OOrank);
    int          iOOF     = myDS->Shape(OOF);
    bool         findTOOF = (T1unk && (OOrank == 1)) || (T2unk && (OOrank == 2));
    bool         findTF   = (T1unk && (rank == 1)) || (T2unk && (rank == 2));

    if (findTOOF)
    {
      // <Erest> on <F>,
      // ?<OOE> on <OOF> /
      // ?<OOIfound> on <OOE>  : OOIfound = (T, S=iF, G=POINT/VERTEX on <Erest>)
      TopOpeBRepDS_Transition T;
      bool                    OOTok = FUN_findTOOF(myDS, iErest, iF, iOOF, T);
      if (OOTok)
      {
        if (OOrank == 1)
          FDS_SetT(T1, T);
        else
          FDS_SetT(T2, T);
      }
    } // !findTOOF
    if (findTF)
    {
      // ?Ifound on <Erest> : Ifound = (T on FACE=iOOF, S, G=POINT/VERTEX on <Erest>)
      // if <Ifound> found : compute TErest at G / <OOF>
      TopOpeBRepDS_Transition T;
      bool                    Tok = FUN_findTF(myDS, iErest, iF, iOOF, T);
      if (Tok)
      {
        if (rank == 1)
          FDS_SetT(T1, T);
        else
          FDS_SetT(T2, T);
      }
    }
    T1unk = T1.IsUnknown();
    T2unk = T2.IsUnknown();
  } // processUNK && (T1unk || T2unk)

  IFE = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference(T1,
                                                                iF2,
                                                                iErest,
                                                                isedge1,
                                                                TopOpeBRepDS_UNSHGEOMETRY);
  myHDS->StoreInterference(IFE, iF1);

  IFE = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference(T2,
                                                                iF1,
                                                                iErest,
                                                                isedge2,
                                                                TopOpeBRepDS_UNSHGEOMETRY);
  myHDS->StoreInterference(IFE, iF2);

  // #################### Rline Processing ####################
  //  xpu061098
  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init((*myLine));
  double                        tola = Precision::Angular() * 1.e5; // NYIXPUTOL
  const TopOpeBRep_VPointInter& vpf  = VPI.CurrentVP();
  for (; VPI.More(); VPI.Next())
  {
    const TopOpeBRep_VPointInter& VP       = VPI.CurrentVP();
    int                           absindex = VP.ShapeIndex(); // 0,1,2,3
    double                        parRest;
    bool                          okR = VP.ParonE(Erest, parRest);
    if (!okR)
      parRest = VP.ParameterOnLine();
    bool on2edges = (absindex == 3) || (absindex == OOrank);

    if (!on2edges)
    {
      // MSV: treat the case when an edge is touched by interior of a face

      // MSV: the commented code below leads to exception on 1cto 025 H3
      //      (object and tool have same subshapes), so to enable it
      //      the debug is needed

      //        bool SIisvertex = VP.IsVertex(EShapeIndex);
      //        if (SIisvertex) continue;
      //        int ShapeIndex = EShapeIndex;
      //        int OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
      //        TopoDS_Face OOFace = (*this).Face(OOShapeIndex);
      //        int iOOFace = myDS->Shape(OOFace);

      //        // make PVIndex
      //        TopOpeBRepDS_Kind PVKind = TopOpeBRepDS_POINT;
      //        int PVIndex = 0;
      //        bool EPIfound=false,CPIfound=false;
      //        occ::handle<TopOpeBRepDS_Interference> IEPI,ICPI;
      //        FUN_VPIndex((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL,
      //                    PVKind,PVIndex,EPIfound,IEPI,CPIfound,ICPI,
      //                    M_GETVP);
      //        bool Efound = (EPIfound || CPIfound);
      //        bool Ifound = (PVIndex != 0);
      //        bool condmake = (!Efound && !Ifound);
      //        if (condmake)
      //          PVIndex = MakeGeometry(VP,ShapeIndex,PVKind);

      //        // make transition on edge
      //        TopOpeBRepDS_Transition transEdge;
      //        TopOpeBRepDS_Transition Trans = FaceFaceTransition(ShapeIndex);
      //        bool TransUNK = Trans.IsUnknown();
      //        if (!TransUNK) {
      //          TopAbs_Orientation oriErest = Erest.Orientation();
      //          transEdge =
      //          TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP,ShapeIndex,oriErest);
      //  	TransUNK = FDS_hasUNK(transEdge);
      //        }
      //        if (TransUNK) {
      //          bool ONfirstP = ::FUN_brep_ONfirstP(vpf,VP);
      //          TopAbs_Orientation OVP = ONfirstP ? TopAbs_FORWARD : TopAbs_REVERSED;
      //          transEdge.Set(OVP);
      //          if (ONfirstP) transEdge.StateAfter(TopAbs_ON);
      //          else          transEdge.StateBefore(TopAbs_ON);
      //          TransUNK = FDS_hasUNK(transEdge);
      //        }
      //        if (TransUNK) continue;

      //        // see if there is already such interference in DS
      //        TopAbs_Orientation otransEdge = transEdge.Orientation(TopAbs_IN);
      //        const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lIedge =
      //        myHDS->DS().ShapeInterferences(Erest);
      //        NCollection_List<occ::handle<TopOpeBRepDS_Interference>> copy;
      //        FDS_copy(lIedge,copy); NCollection_List<occ::handle<TopOpeBRepDS_Interference>>
      //        l1,l2; int nfound = FUN_selectGIinterference(copy,PVIndex,l1); if (nfound) {
      //  	if (iOOFace != 0) {
      //  	  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l3,l4;
      //            nfound = FUN_selectITRASHAinterference(l2,iOOFace,l3);
      //  	  if (nfound != 0) nfound = FUN_selectTRAORIinterference(l3,otransEdge,l4);
      //  	  if (nfound) continue;
      //  	}
      //        }

      //        // make and store interference
      //        occ::handle<TopOpeBRepDS_Interference> EPIf;
      //        if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);
      //        TopOpeBRepDS_Transition T = transEdge; T.Index(iOOFace);
      //        EPIf = MakeEPVInterference(T,iOOFace,PVIndex,parRest,PVKind,
      //                                   TopOpeBRepDS_FACE,SIisvertex);
      //        myHDS->StoreInterference(EPIf,Erest);

      continue;
    }

    TopoDS_Edge OOE = TopoDS::Edge(VP.Edge(OOrank));
    int         iOO = myDS->AddShape(OOE, OOrank);

    double OOpar;
    VP.ParonE(OOE, OOpar);

    // xpu091198 : 1d interf done in EdgesFiller processing (cto cylcong *)
    bool sdmeds = FUN_ds_sdm((*myDS), Erest, OOE);
    if (sdmeds)
      continue;

    // clang-format off
    int obRest = TopOpeBRepTool_TOOL::OnBoundary(parRest,Erest); //vertex can be missed
    int obOO   = TopOpeBRepTool_TOOL::OnBoundary(OOpar,OOE);     //vertex can be missed
    // clang-format on

    if ((obRest == EXTERNAL) || (obOO == EXTERNAL))
    {
#ifdef OCCT_DEBUG
      if (obRest == EXTERNAL)
        std::cout << "***********ProcessRLine : faulty parameter on Erest" << std::endl;
      if (obOO == EXTERNAL)
        std::cout << "***********ProcessRLine : faulty parameter on OOE" << std::endl;
#endif
    }

    bool tgeds = FUN_tool_EtgOOE(parRest, Erest, OOpar, OOE, tola);

    // clang-format off
    TopOpeBRepDS_Kind PVKind = TopOpeBRepDS_POINT; int PVIndex = 0;  // POINT or VERTEX index
    // clang-format on

    for (int ShapeIndex = 1; ShapeIndex <= 2; ShapeIndex++)
    {
      int  OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
      bool SIErest      = (ShapeIndex == rank);

      bool        SIisvertex = VP.IsVertex(ShapeIndex);
      bool        OOisvertex = VP.IsVertex(OOShapeIndex);
      TopoDS_Face OOFace     = (*this).Face(OOShapeIndex);
      int         iOOFace    = myDS->Shape(OOFace);

      TopoDS_Edge edge, OOedge;
      int         OOedgeIndex;
      double      paredge;
      int         onbound;
      if (SIErest)
      {
        edge        = Erest;
        paredge     = parRest;
        onbound     = obRest;
        OOedge      = OOE;
        OOedgeIndex = iOO;
      }
      else
      {
        OOedge      = Erest;
        OOedgeIndex = iErest;
        onbound     = obOO;
        edge        = OOE;
        paredge     = OOpar;
      }
      // PVIndex :
      // --------
      // xpu150399 : BUC60382
      bool                                   EPIfound = false, CPIfound = false;
      occ::handle<TopOpeBRepDS_Interference> IEPI, ICPI;
      ProcessVPondgE(VP,
                     ShapeIndex,
                     PVKind,
                     PVIndex, // out
                     EPIfound,
                     IEPI, // out
                     CPIfound,
                     ICPI); // out

      if (PVIndex == 0)
      {
        //	bool EPIfound=false,CPIfound=false;
        // occ::handle<TopOpeBRepDS_Interference> IEPI,ICPI;
        FUN_VPIndex((*this),
                    (*myLine),
                    VP,
                    ShapeIndex,
                    myHDS,
                    myDSCIL,
                    PVKind,
                    PVIndex,
                    EPIfound,
                    IEPI,
                    CPIfound,
                    ICPI,
                    M_GETVP);
        bool Efound   = (EPIfound || CPIfound);
        bool Ifound   = (PVIndex != 0);
        bool condmake = (!Efound && !Ifound);
        if (condmake)
        {
          if (SIisvertex)
            PVIndex = MakeGeometry(VP, ShapeIndex, PVKind);
          else if (OOisvertex)
            PVIndex = MakeGeometry(VP, OOShapeIndex, PVKind);
          else
            PVIndex = MakeGeometry(VP, ShapeIndex, PVKind);
        }
      }

      // transEdge :
      // ----------
      if (OOedgeIndex == 0)
        OOedgeIndex = myDS->AddShape(OOedge, OOShapeIndex);
      const TopOpeBRepDS_Transition& llt1     = FaceFaceTransition(1);
      const TopOpeBRepDS_Transition& llt2     = FaceFaceTransition(2);
      TopOpeBRepDS_Transition        Trans    = (ShapeIndex == 1) ? llt1 : llt2;
      bool                           TransUNK = Trans.IsUnknown();

      TopOpeBRepDS_Transition transEdge;
      bool                    Tunk = true;
      if (!TransUNK)
      { // xpu281098 PRO12875(edge9,OOface11)
        if ((absindex == ShapeIndex) || (absindex == 3))
        {
          if (SIErest)
          {
            // transition on Erest at VP / OOface = transition at VP on Line restriction
            TopAbs_Orientation oriErest = Erest.Orientation();
            transEdge =
              TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP, ShapeIndex, oriErest);

            if (((onbound == 1) || (onbound == 2)) && tgeds) // xpu290399 : edge is restriction,
            // edge15,OOedge14,OOface13
            {
              transEdge.Before(TopAbs_UNKNOWN);
              transEdge.After(TopAbs_UNKNOWN);
            }
          }
          else
          {
            // transition on edge at VP / OOface ?=
            //  TopOpeBRep_FFTransitionTool::ProcessEdgeTransition(VP,ShapeIndex,Transori);
            // nyi
          }
        }
        Tunk = FDS_hasUNK(transEdge);
      }
      if (Tunk)
      {
        if (SIErest)
        {
          // As edge=Erest is on OOFace, we only compute 2d interferences
          bool               ONfirstP = ::FUN_brep_ONfirstP(vpf, VP);
          TopAbs_Orientation OVP      = ONfirstP ? TopAbs_FORWARD : TopAbs_REVERSED;
          TopAbs_Orientation oOO;
          bool               ok = FUN_tool_orientEinFFORWARD(OOedge, OOFace, oOO);
          if (!ok)
            continue;
          if (M_INTERNAL(oOO))
            OVP = TopAbs_INTERNAL;

          // xpu240399 : cto015I2 (e15,v16)
          //             edge and OOedge are tangent, we do not keep the orientation
          if (!tgeds)
            transEdge.Set(OVP);
        }
        else
        {
          TopOpeBRepDS_Transition Tr;
          bool                    ok = FUN_newtransEdge(myHDS,
                                     (*this),
                                     (*myLine),
                                     myLineIsonEdge,
                                     VP,
                                     PVKind,
                                     PVIndex,
                                     OOShapeIndex,
                                     edge,
                                     myERL,
                                     Tr);
          if (!ok)
            continue;
          transEdge.Before(Tr.Before());
          transEdge.After(Tr.After());
        }
      } // Tunk
      Tunk = FDS_hasUNK(transEdge);
      if (Tunk)
        continue;

      TopAbs_Orientation otransEdge = transEdge.Orientation(TopAbs_IN);
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lIedge =
        myHDS->DS().ShapeInterferences(edge);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> copy;
      FDS_copy(lIedge, copy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l1, l2;
      int nfound = FUN_selectGIinterference(copy, PVIndex, l1);
      if (OOedgeIndex != 0)
        nfound = FUN_selectSIinterference(l1, OOedgeIndex, l2);
      if (nfound)
      {
        if (sdmeds)
        {
          NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l3, l4;
          nfound = FUN_selectITRASHAinterference(l2, OOedgeIndex, l3);
          if (nfound != 0)
            nfound = FUN_selectTRAORIinterference(l3, otransEdge, l4);
          if (nfound)
            continue; // has I1d=(transEdge(OOedgeIndex),PVIndex,OOedgeIndex);
        }
        else if (iOOFace != 0)
        {
          NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l3, l4;
          nfound = FUN_selectITRASHAinterference(l2, iOOFace, l3);
          if (nfound != 0)
            nfound = FUN_selectTRAORIinterference(l3, otransEdge, l4);
          if (nfound)
            continue; // has I2d=(transEdge(iOOFace),PVIndex,OOedgeIndex)
        }
      } // nfound

      // EPI :
      // ----
      occ::handle<TopOpeBRepDS_Interference> EPI;
      {
        if (iOOFace == 0)
          iOOFace = myDS->AddShape(OOFace, OOShapeIndex);
        TopOpeBRepDS_Transition T = transEdge;
        T.Index(iOOFace);
        EPI = MakeEPVInterference(T, OOedgeIndex, PVIndex, paredge, PVKind, SIisvertex);
      }
      myHDS->StoreInterference(EPI, edge);

      // EPIf :
      // -----
      if (!SIErest)
      {
        occ::handle<TopOpeBRepDS_Interference> EPIf;
        TopOpeBRepDS_Transition                T = transEdge;
        T.Index(iOOFace);
        EPIf =
          MakeEPVInterference(T, iOOFace, PVIndex, paredge, PVKind, TopOpeBRepDS_FACE, SIisvertex);
        myHDS->StoreInterference(EPIf, edge);
      }

    } // ShapeIndex=1..2
  } // VPI
  // ####################
}

static bool FUN_haslastvpon0(const TopOpeBRep_LineInter& L)
{
  const bool wline = (L.TypeLineCurve() == TopOpeBRep_WALKING);
  if (!wline)
    return false;

  int iINON1, iINONn, nINON;
  L.VPBounds(iINON1, iINONn, nINON);

  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init(L);
  for (; VPI.More(); VPI.Next())
  {
    const TopOpeBRep_VPointInter& VP       = VPI.CurrentVP();
    const int                     absindex = VP.ShapeIndex();
    const int                     iVP      = VP.Index();
    if (iVP == iINONn && absindex == 0)
      return true;
  }
  return false;
}

//=================================================================================================

void TopOpeBRep_FacesFiller::FillLineVPonR()
{
#ifdef OCCT_DEBUG
  bool trline = false;
#endif
  // if a VP is on degenerated edge, adds the triplet
  // (vertex, closing edge, degenerated edge) to the
  // map as vertex for key.
  //  myDataforDegenEd.Clear();
  FUN_GetdgData(myDS, (*myLine), myF1, myF2, myDataforDegenEd);
  FUN_FillVof12((*myLine), myDS);

  mykeptVPnbr = 0;

  if (myLine->TypeLineCurve() == TopOpeBRep_RESTRICTION)
  {
#ifdef OCCT_DEBUG
    if (trline)
      FUN_traceRLine(*myLine);
#endif
    ProcessRLine();
    return;
  }

  int iINON1, iINONn, nINON;
  myLine->VPBounds(iINON1, iINONn, nINON);
  if (nINON == 0)
  {
    return;
  }

#ifdef OCCT_DEBUG
  if (trline)
    FUN_traceGLine(*myLine);
#endif
  myLineIsonEdge = LSameDomainERL(*myLine, myERL);

  // walking (case mouch1a 1 1) : line (vpfirst on 3,vplast on 0,nvpkept = 2) => kept
  myLastVPison0 = ::FUN_haslastvpon0(*myLine);

  //----------------------------------------------------------------------  // IMPORTANT :
  // Some of Curve/Point transitions for vpoints keep on RESTRICTION lines
  // sharing same domain with the current geometric line are computed here
  //----------------------------------------------------------------------

#ifdef OCCT_DEBUG
#endif

  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init((*myLine));
  for (; VPI.More(); VPI.Next())
  {
    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    ProcessVPR((*this), VP);
  }

  if (myLineIsonEdge && (!myDSCIL.IsEmpty()))
  {
    myDSCIL.Clear();
  }
}

//=================================================================================================

void TopOpeBRep_FacesFiller::FillLine()
{
  int iINON1, iINONn, nINON;
  myLine->VPBounds(iINON1, iINONn, nINON);
  if (nINON == 0)
    return;

  int                                    ShapeIndex = 0;
  occ::handle<TopOpeBRepDS_Interference> CPI;

  TopOpeBRep_VPointInterIterator VPI;
  for (VPI.Init((*myLine)); VPI.More(); VPI.Next())
  {

    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    if (!VP.Keep())
      continue;

    int                                                                PVIndex;
    TopOpeBRepDS_Kind                                                  PVKind;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itCPIL(myDSCIL);
    bool                                                               CPIfound;
    CPIfound = GetGeometry(itCPIL, VP, PVIndex, PVKind);
    if (!CPIfound)
    {
      bool found = GetFFGeometry(VP, PVKind, PVIndex);
      if (!found)
        PVIndex = MakeGeometry(VP, ShapeIndex, PVKind);
    }

    TopOpeBRepDS_Transition transLine;
    if (!CPIfound)
    {
      int iVP = VPI.CurrentVPIndex();
      if (iVP == iINON1)
        transLine.Set(TopAbs_FORWARD);
      else if (iVP == iINONn)
        transLine.Set(TopAbs_REVERSED);
    }
    else
      transLine = itCPIL.Value()->Transition().Complement();

    double parline = VPI.CurrentVP().ParameterOnLine();
    CPI            = TopOpeBRepDS_InterferenceTool::MakeCurveInterference(transLine,
                                                               TopOpeBRepDS_CURVE,
                                                               0,
                                                               PVKind,
                                                               PVIndex,
                                                               parline);
    StoreCurveInterference(CPI);

  } //   loop on VPoints

} // FillLine

//=================================================================================================

void TopOpeBRep_FacesFiller::AddShapesLine()
{
  bool dscilemp = myDSCIL.IsEmpty();
  if (dscilemp)
    return;

  bool inl = myLine->INL();
  if (inl)
    return;

  TopOpeBRepDS_Curve& DSC = myDS->ChangeCurve(myDSCIndex);

  occ::handle<Geom_Curve>                C3D;
  occ::handle<Geom2d_Curve>              PC1, PC2;
  occ::handle<TopOpeBRepDS_Interference> FCI1, FCI2;

  int iF1 = myDS->AddShape(myF1, 1);
  int iF2 = myDS->AddShape(myF2, 2);

  double pmin, pmax;
  myHDS->MinMaxOnParameter(myDSCIL, pmin, pmax);

  double d     = std::abs(pmin - pmax);
  bool   id    = (d <= Precision::PConfusion());
  bool   isper = myLine->IsPeriodic();
  id           = (id && !isper);

  bool wline   = (myLine->TypeLineCurve() == TopOpeBRep_WALKING);
  bool vclosed = myLine->IsVClosed();
  if (wline && !isper && vclosed)
  {
    // xpu240399 : USA60298 : avoid creating curve
    //  MSV: take into account that geometry can be of type VERTEX
    int               ipf = myDSCIL.First()->Geometry();
    TopOpeBRepDS_Kind kpf = myDSCIL.First()->GeometryType();
    gp_Pnt            ptf;
    double            tol, tolf, toll;
    if (kpf == TopOpeBRepDS_POINT)
    {
      TopOpeBRepDS_Point pf = myDS->Point(ipf);
      ptf                   = pf.Point();
      tolf                  = pf.Tolerance();
    }
    else
    { // VERTEX
      TopoDS_Vertex vf = TopoDS::Vertex(myDS->Shape(ipf));
      ptf              = BRep_Tool::Pnt(vf);
      tolf             = BRep_Tool::Tolerance(vf);
    }

    int               ipl = myDSCIL.Last()->Geometry();
    TopOpeBRepDS_Kind kpl = myDSCIL.Last()->GeometryType();
    if (kpl == TopOpeBRepDS_POINT)
    {
      TopOpeBRepDS_Point pl = myDS->Point(ipl);
      toll                  = pl.Tolerance();
    }
    else
    { // VERTEX
      TopoDS_Vertex vl = TopoDS::Vertex(myDS->Shape(ipl));
      toll             = BRep_Tool::Tolerance(vl);
    }

    tol          = std::max(tolf, toll);
    bool onsampt = true;
    for (int ii = 1; ii <= myLine->NbWPoint(); ii++)
    {
      TopOpeBRep_WPointInter wp = myLine->WPoint(ii);
      gp_Pnt                 pp = wp.Value();
      if (!pp.IsEqual(ptf, tol))
      {
        onsampt = false;
        break;
      }
    }
    if (onsampt)
      id = true;
  }

  if (id)
  {
    DSC.ChangeKeep(false);
    return;
  }

  TopOpeBRep_GeomTool::MakeCurves(pmin, pmax, (*myLine), myF1, myF2, DSC, PC1, PC2);

  // Patch: avoid making too small edges. Made for the bug buc60926 by jgv, 14.06.01.
  double fpar, lpar;
  DSC.Range(fpar, lpar);
  GeomAdaptor_Curve theCurve(DSC.Curve(), fpar, lpar);
  Bnd_Box           theBox;
  BndLib_Add3dCurve::Add(theCurve, 0., theBox);
  double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, MaxSide;
  theBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  MaxSide       = std::max(std::max(Xmax - Xmin, Ymax - Ymin), Zmax - Zmin);
  double MinTol = std::min(BRep_Tool::Tolerance(myF1), BRep_Tool::Tolerance(myF2));
  if (MaxSide < MinTol)
  {
    DSC.ChangeKeep(false);
    return;
  }

  double tolDSC = 1.e-8;
  DSC.Tolerance(tolDSC);
  const TopOpeBRepDS_Transition& lllt1 = FaceFaceTransition(1);
  const TopOpeBRepDS_Transition& lllt2 = FaceFaceTransition(2);

  myDS->ChangeCurveInterferences(myDSCIndex).Append(myDSCIL);
  {
    TopOpeBRepDS_Transition T1 = lllt1;
    T1.Index(iF2);
    FCI1 = TopOpeBRepDS_InterferenceTool::MakeFaceCurveInterference(T1, iF2, myDSCIndex, PC1);
    myHDS->StoreInterference(FCI1, myF1);
  }

  {
    TopOpeBRepDS_Transition T2 = lllt2;
    T2.Index(iF1);
    FCI2 = TopOpeBRepDS_InterferenceTool::MakeFaceCurveInterference(T2, iF1, myDSCIndex, PC2);
    myHDS->StoreInterference(FCI2, myF2);
  }

  DSC.SetShapes(myF1, myF2);
  DSC.SetSCI(FCI1, FCI2);
}

//=================================================================================================

void TopOpeBRep_FacesFiller::StoreCurveInterference(const occ::handle<TopOpeBRepDS_Interference>& I)
{
  if (myDSCIndex == 0)
  {
    TopOpeBRepDS_Curve DSC;
    myDSCIndex = myDS->AddCurve(DSC);
  }

  I->Support(myDSCIndex);
  myHDS->StoreInterference(I, myDSCIL);
}

//=================================================================================================

bool TopOpeBRep_FacesFiller::GetGeometry(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& IT,
  const TopOpeBRep_VPointInter&                                       VP,
  int&                                                                G,
  TopOpeBRepDS_Kind&                                                  K)
{
  TopOpeBRepDS_Point DSP = TopOpeBRep_PointGeomTool::MakePoint(VP);
  bool               b   = myHDS->GetGeometry(IT, DSP, G, K);
  return b;
}

//=================================================================================================

int TopOpeBRep_FacesFiller::MakeGeometry(const TopOpeBRep_VPointInter& VP,
                                         const int                     ShapeIndex,
                                         TopOpeBRepDS_Kind&            K)
{
  int  G;
  bool isvertex = VP.IsVertex(ShapeIndex);
  if (isvertex)
  {
    const TopoDS_Shape& S = VP.Vertex(ShapeIndex);
    G                     = myDS->AddShape(S, ShapeIndex);
    K                     = TopOpeBRepDS_VERTEX;
  }
  else
  {
    TopOpeBRepDS_Point P = TopOpeBRep_PointGeomTool::MakePoint(VP);
    G                    = myDS->AddPoint(P);
    K                    = TopOpeBRepDS_POINT;
  }

  return G;
}

//=================================================================================================

bool TopOpeBRep_FacesFiller::GetFFGeometry(const TopOpeBRepDS_Point& DSP,
                                           TopOpeBRepDS_Kind&        K,
                                           int&                      G) const
{
  bool found = false;
  int  i = myFFfirstDSP, n = myDS->NbPoints();
  for (; i <= n; i++)
  {
    const TopOpeBRepDS_Point& OODSP = myDS->Point(i);
    found                           = TopOpeBRep_PointGeomTool::IsEqual(DSP, OODSP);
    if (found)
      break;
  }
  if (found)
  {
    K = TopOpeBRepDS_POINT;
    G = i;
  }
  return found;
}

//=================================================================================================

bool TopOpeBRep_FacesFiller::GetFFGeometry(const TopOpeBRep_VPointInter& VP,
                                           TopOpeBRepDS_Kind&            K,
                                           int&                          G) const
{
  TopOpeBRepDS_Point DSP   = TopOpeBRep_PointGeomTool::MakePoint(VP);
  bool               found = GetFFGeometry(DSP, K, G);
  return found;
}
