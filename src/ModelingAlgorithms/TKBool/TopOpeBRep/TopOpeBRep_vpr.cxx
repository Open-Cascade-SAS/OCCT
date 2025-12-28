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

#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#ifdef DRAW
  #include <TopOpeBRepDS_DRAW.hxx>
#endif

#include <Standard_DomainError.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <gp_Vec.hxx>

#include <TopOpeBRepTool_GEOMETRY.hxx>

#include <TopOpeBRepTool_PROJECT.hxx>

#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>

#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_Dumper.hxx>

#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRep.hxx>

#define M_ON(st) (st == TopAbs_ON)
#define M_UNKNOWN(st) (st == TopAbs_UNKNOWN)
#define M_REVERSED(st) (st == TopAbs_REVERSED)

#ifdef OCCT_DEBUG
extern bool TopOpeBRep_GettraceNVP(int a, int b, int c, int d, int e);

bool GLOBAL_bvpr = false;

void debvpr() {};

void debvprmess(int f1, int f2, int il, int vp, int si)
{
  std::cout << "f1,f2,il,vp,si : " << f1 << "," << f2 << "," << il << "," << vp << "," << si
            << std::endl;
  std::cout.flush();
  debvpr();
}

void debpoint(int i)
{
  std::cout << "+ debpoint" << i << std::endl;
}

void debvertex(int i)
{
  std::cout << "+ debvertex" << i << std::endl;
}

Standard_EXPORT void debarc(const int i)
{
  std::cout << "+ debarc " << i << std::endl;
}

Standard_EXPORT void debooarc(const int i)
{
  std::cout << "+ debooarc " << i << std::endl;
}
#endif

Standard_EXPORT bool FDS_LOIinfsup(
  const TopOpeBRepDS_DataStructure&                               BDS,
  const TopoDS_Edge&                                              E,
  const double                                                    pE,
  const int                                                       GIP,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI,
  double&                                                         pbef,
  double&                                                         paft,
  bool&                                                           isonboundper);
Standard_EXPORT bool FUNBREP_topokpart(
  const occ::handle<TopOpeBRepDS_Interference>&                   Ifound,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
  const TopOpeBRep_LineInter&                                     L,
  const TopOpeBRep_VPointInter&                                   VP,
  const TopOpeBRepDS_DataStructure&                               BDS,
  const TopoDS_Shape&                                             E,
  const TopoDS_Shape&                                             F,
  const double                                                    toluv,
  double&                                                         parline,
  TopOpeBRepDS_Transition&                                        transLine);

//-----------------------------------------------------------------------
// Search, among a list of interferences accessed by the iterator <IT>,
// a geometry whose parameter on edge point is identical to <par>.
// return True if such an interference has been found, False else.
// if True, iterator <IT> points (by the Value() method) on the first
// interference found.
//-----------------------------------------------------------------------

Standard_EXPORT bool FUN_GetGonParameter(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& it,
  const double&                                                       par,
  const double&                                                       tolp,
  int&                                                                G,
  TopOpeBRepDS_Kind&                                                  GT)
{
  while (it.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    double                                        ipar;
    bool                                          haspar = FDS_Parameter(I, ipar);
    if (!haspar)
    {
      it.Next();
      continue;
    }
    bool samepar = (std::abs(par - ipar) < tolp);
    if (!samepar)
    {
      it.Next();
      continue;
    }
    TopOpeBRepDS_Kind ST;
    int               S;
    FDS_data(I, GT, G, ST, S);
    return true;
  }
  return false;
}

static bool FUN_INlos(const TopoDS_Shape& S, const NCollection_List<TopoDS_Shape>& loS)
{
  NCollection_List<TopoDS_Shape>::Iterator it(loS);
  for (; it.More(); it.Next())
    if (it.Value().IsSame(S))
      return true;
  return false;
}

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessVPIonR(TopOpeBRep_VPointInterIterator& VPI,
                                           const TopOpeBRepDS_Transition&  Trans,
                                           const TopoDS_Shape&             Face,
                                           const int                       ShapeIndex) // 1,2
{
  const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
  ProcessVPonR(VP, Trans, Face, ShapeIndex);
} // ProcessVPIonR

//-----------------------------------------------------------------------
static void FUN_transForWL(const TopOpeBRep_LineInter& L,
                           const int                   iVP,
                           const int                   ShapeIndex,
                           TopOpeBRepDS_Transition&    transLine)
//-----------------------------------------------------------------------
{
  // premier VP avec indetermine : on prend le complement
  // du suivant determine
  TopOpeBRep_VPointInterIterator VPIbis;
  for (VPIbis.Init(L); VPIbis.More(); VPIbis.Next())
  {
    const TopOpeBRep_VPointInter& VPbis  = VPIbis.CurrentVP();
    bool                          tokeep = VPbis.Keep();
    if (!tokeep)
      continue;
    int iVPbis = VPIbis.CurrentVPIndex();
    if (iVPbis <= iVP)
      continue;
    int absindexbis   = VPbis.ShapeIndex(); // 0,1,2,3
    int shapeindexbis = (absindexbis == 3) ? ShapeIndex : absindexbis;
    if (shapeindexbis == 0)
      continue;
    const TopoDS_Shape&     edgebis    = VPbis.Edge(shapeindexbis);
    TopAbs_Orientation      edgeoribis = edgebis.Orientation();
    TopOpeBRepDS_Transition transLinebis;
    transLinebis =
      TopOpeBRep_FFTransitionTool::ProcessLineTransition(VPbis, shapeindexbis, edgeoribis);
    bool trliunkbis = transLinebis.IsUnknown();
    if (trliunkbis)
      continue;
    transLine = transLinebis.Complement();
    break;
  }
}

//-----------------------------------------------------------------------
static void FUN_VPgeometryfound(
  TopOpeBRep_FacesFiller&                                         FF,
  const TopOpeBRep_LineInter&                                     L,
  const TopOpeBRep_VPointInter&                                   VP,
  const int                                                       ShapeIndex,
  const occ::handle<TopOpeBRepDS_HDataStructure>&                 HDS,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
  TopOpeBRepDS_Kind&                                              PVKind,
  int&                                                            PVIndex,
  bool&                                                           EPIfound,
  occ::handle<TopOpeBRepDS_Interference>&                         IEPI,
  bool&                                                           CPIfound,
  occ::handle<TopOpeBRepDS_Interference>&                         ICPI,
  bool&                                                           OOEPIfound,
  occ::handle<TopOpeBRepDS_Interference>&                         IOOEPI) // (only if on2edges)
//-----------------------------------------------------------------------
{
  bool         Lrest = (L.TypeLineCurve() == TopOpeBRep_RESTRICTION);
  TopoDS_Shape Erest;
  double       parErest = 0;
  int          rkErest  = 0;
  if (Lrest)
  {
    Erest        = L.Arc();
    parErest     = VP.ParameterOnLine();
    bool isedge1 = L.ArcIsEdge(1);
    bool isedge2 = L.ArcIsEdge(2);
    rkErest      = (isedge1) ? 1 : (isedge2) ? 2 : 0;
  }

  int          absindex     = VP.ShapeIndex();
  int          OOabsindex   = (absindex == 1) ? 2 : 1;
  int          OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  bool         on2edges     = (absindex == 3) || (Lrest && (rkErest == OOabsindex));
  TopoDS_Shape edge         = (rkErest == ShapeIndex) ? Erest : VP.Edge(ShapeIndex);

  PVIndex  = 0; // POINT or VERTEX index
  EPIfound = CPIfound = OOEPIfound = false;
  double par  = (rkErest == ShapeIndex) ? parErest : VP.EdgeParameter(ShapeIndex);
  double tole = FUN_tool_maxtol(edge);
  double tolp = Precision::Parametric(tole);

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  if (BDS.HasShape(edge))
  {
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& EPIL =
      BDS.ShapeInterferences(edge);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itEPIL(EPIL);
    EPIfound = FF.GetGeometry(itEPIL, VP, PVIndex, PVKind);
    if (!EPIfound)
    {
      itEPIL.Initialize(EPIL);
      EPIfound = FUN_GetGonParameter(itEPIL, par, tolp, PVIndex, PVKind);
    }
    if (EPIfound)
      IEPI = itEPIL.Value();
  }

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itCPIL(DSCIL);
  CPIfound = FF.GetGeometry(itCPIL, VP, PVIndex, PVKind);
  if (CPIfound)
    ICPI = itCPIL.Value();

  // - <VP> is of shapeindex 3 : is on <edge> and <OOedge>,
  // - <VP> is of shapeindex <ShapeIndex> and <VP> is given ON another edge <OOedge>
  // If <OOedge> is defined, we look among the list of interferences attached
  // to the other edge <OOedge> for an interference of geometry falling into <VP>'s.

  bool hasOOedge = true;
  if (on2edges)
    hasOOedge = true;
  else
    hasOOedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  if (hasOOedge)
  {
    TopoDS_Shape OOedge;

    if (on2edges)
      OOedge = (rkErest == OOShapeIndex) ? Erest : VP.Edge(OOShapeIndex);
    else
      OOedge = VP.EdgeON(OOShapeIndex);

    double OOpar = 0.;

    if (on2edges)
      OOpar = (rkErest == OOShapeIndex) ? parErest : VP.EdgeParameter(OOShapeIndex);
    else
      OOpar = VP.EdgeONParameter(OOShapeIndex);

    double tolOOe = FUN_tool_maxtol(OOedge);
    double OOtolp = Precision::Parametric(tolOOe);
    if (BDS.HasShape(OOedge))
    {
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& OOEPIL =
        BDS.ShapeInterferences(OOedge);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator OOitEPIL(OOEPIL);
      OOEPIfound = FF.GetGeometry(OOitEPIL, VP, PVIndex, PVKind);
      if (!OOEPIfound)
      {
        OOitEPIL.Initialize(OOEPIL);
        FUN_GetGonParameter(OOitEPIL, OOpar, OOtolp, PVIndex, PVKind);
      }
      if (OOEPIfound)
        IOOEPI = OOitEPIL.Value();
    }
  }
}

#define M_FINDVP (0)  // only look for new vp
#define M_MKNEWVP (1) // only make newvp
#define M_GETVP (2)   // steps (0) [+(1) if (O) fails]

//-----------------------------------------------------------------------
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
  const int                                                       mkVP)
//-----------------------------------------------------------------------
{
  PVIndex           = 0; // POINT or VERTEX index
  int  OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  bool SIisvertex   = VP.IsVertex(ShapeIndex);
  bool OOisvertex   = VP.IsVertex(OOShapeIndex);

  // search for an interference with a equal 3D geometry
  // if found, set PVIndex to index of geometry found
  // if not found, make a new geometry PVIndex with 3d point or vertex

  bool                                   OOEPIfound = false;
  occ::handle<TopOpeBRepDS_Interference> IOOEPI;
  if ((mkVP == M_FINDVP) || (mkVP == M_GETVP))
  {
    FUN_VPgeometryfound(FF,
                        L,
                        VP,
                        ShapeIndex,
                        HDS,
                        DSCIL, // in
                        PVKind,
                        PVIndex, // out
                        EPIfound,
                        IEPI, // out
                        CPIfound,
                        ICPI, // out
                        OOEPIfound,
                        IOOEPI); // out (only if on2edges)
    if (mkVP == M_FINDVP)
    {
      // JMB 27 Dec 1999
      // modified by NIZHNY-MZV  Tue Apr 25 09:27:15 2000
      if (!EPIfound && !CPIfound && !OOEPIfound)
        PVIndex =
          0; // if we just want to find (M_FINDVP) then we must readjust PVIndex to 0
             // because OOEPIfound is not treated in the upper function. The upper function
             // will detect that we found a geometry because PVIndex != 0 but as EPIfound and
             // CPIfound are FALSE, the resulting VP geometry give unpredictable results.
      return;
    }
  }
  // Gfound = VP corresponds with an existing geometry of ShapeIndex
  bool Gfound = (EPIfound || CPIfound);
  // Gfound =             or with an existing geometry of OOShapeIndex
  Gfound = Gfound || OOEPIfound;

  bool on2edges  = (VP.ShapeIndex() == 3);
  bool hasOOedge = true;
  if (on2edges)
    hasOOedge = true;
  else
    hasOOedge = (VP.State(OOShapeIndex) == TopAbs_ON);
  // If v shares same domain with a vertex of the other shape,
  // v has already been stored in the DS

  if (PVIndex == 0)
    PVKind = (SIisvertex || OOisvertex) ? TopOpeBRepDS_VERTEX : TopOpeBRepDS_POINT;

  if (hasOOedge && !Gfound)
  {
    if (!OOEPIfound)
    {
      if (SIisvertex)
        PVIndex = FF.MakeGeometry(VP, ShapeIndex, PVKind);
      else if (OOisvertex)
        PVIndex = FF.MakeGeometry(VP, OOShapeIndex, PVKind);
      else
        PVIndex = FF.MakeGeometry(VP, ShapeIndex, PVKind);
    }
  }
  if (!hasOOedge && !Gfound)
  {
    bool found = FF.GetFFGeometry(VP, PVKind, PVIndex);
    if (!found)
    {
      if (SIisvertex)
        PVIndex = FF.MakeGeometry(VP, ShapeIndex, PVKind);
      else if (OOisvertex)
        PVIndex = FF.MakeGeometry(VP, OOShapeIndex, PVKind);
      else
        PVIndex = FF.MakeGeometry(VP, ShapeIndex, PVKind);
    }
  }
} // FUN_VPIndex

//-----------------------------------------------------------------------
static bool FUN_LineRestF(const TopoDS_Face&                    F,
                          const TopOpeBRep_LineInter&           L,
                          const NCollection_List<TopoDS_Shape>& ERL,
                          TopoDS_Edge&                          ER)
//-----------------------------------------------------------------------
{
  // returns true if <L> is ON a restriction <ER> of <F>
  // <ERL> is the list of the faces intersector.
  // prequesitory : <L> is on edge
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapE;
  TopExp::MapShapes(F, TopAbs_EDGE, mapE);
  NCollection_List<TopoDS_Shape>::Iterator itER(ERL);
  NCollection_List<TopoDS_Shape>           ERLonF;
  for (; itER.More(); itER.Next())
  {
    const TopoDS_Shape& e = itER.Value();
    if (mapE.Contains(e))
      ERLonF.Append(e);
  }
  itER.Initialize(ERLonF);
  NCollection_List<TopoDS_Shape> ERLonFonL;
  for (; itER.More(); itER.Next())
  {
    const TopoDS_Shape&            e = itER.Value();
    NCollection_List<TopoDS_Shape> eL;
    eL.Append(e);
    bool isonL = TopOpeBRep_FacesFiller::LSameDomainERL(L, eL);
    if (isonL)
      ERLonFonL.Append(e);
  }
  // <L> is on at most one edge restriction.
  if (ERLonFonL.Extent() != 1)
    return false;
  ER = TopoDS::Edge(ERLonFonL.First());
  return true;
}

//-----------------------------------------------------------------------
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
                                      TopOpeBRepDS_Transition&                        T)
//-----------------------------------------------------------------------
{
  T.Before(TopAbs_UNKNOWN);
  T.After(TopAbs_UNKNOWN);
  const TopoDS_Face& OOface = FF.Face(OOShapeIndex);
  const TopoDS_Face& FIE    = OOface;
  {
    TopAbs_Orientation oFIE = FIE.Orientation();
    if (oFIE == TopAbs_INTERNAL || oFIE == TopAbs_EXTERNAL)
    {
      T.Set(oFIE);
      return true;
    }
  }

  // compute of transition on edge <edge> while crossing <VP>.
  // <VP> given by <paredge> on <edge>, <uv> on <OOface>,
  //               <paronline> on Line.

  // <C>, <pf>, <pl>, <paredge> :
  double paredge;
  bool   ok = VP.ParonE(edge, paredge);
  if (!ok)
    return false;

  double par1, par2;
  if (HDS->HasShape(edge))
  {
    bool isonper;
    if (PVIndex == 0)
      FDS_getupperlower(HDS, HDS->DS().Shape(edge), paredge, par1, par2);
    else
      FDS_LOIinfsup(HDS->DS(),
                    edge,
                    paredge,
                    PVKind,
                    PVIndex,
                    HDS->DS().ShapeInterferences(edge),
                    par1,
                    par2,
                    isonper);
  }
  else
    FUN_tool_bounds(edge, par1, par2);

  gp_Pnt2d uv = VP.SurfaceParameters(OOShapeIndex);

#ifdef OCCT_DEBUG
  TopOpeBRepDS_Transition Tr;
#endif
  //       <Tr> relative to 3d <OOface> matter,
  //       we take into account <Tr> / 2d <OOface> only if <edge> is normal to <OOface>
  double tola       = Precision::Angular() * 1.e+4; // dealing with tolerances
  bool   EtgOOF     = FUN_tool_EtgF(paredge, edge, uv, OOface, tola);
  bool   inERL      = FUN_INlos(edge, ERL);
  bool   isse       = HDS->DS().IsSectionEdge(edge);
  bool   rest       = inERL || isse;
  bool   interf2d   = EtgOOF && Lonrest && rest;
  bool   interf3dtg = EtgOOF && rest && !interf2d; // xpu260898 :cto902D6,(e15,p3,f9)

  double                        factor = 1.e-2;
  TopOpeBRepTool_makeTransition MKT;
  ok = MKT.Initialize(edge, par1, par2, paredge, OOface, uv, factor);
  if (!ok)
    return false;
  bool isT2d = MKT.IsT2d();
  interf2d   = interf2d && isT2d;

  TopAbs_State stb, sta;
  if (interf2d)
  {
    // <tgLine> :
    TopoDS_Edge              OOER;
    bool                     onOOface = false;
    TopOpeBRep_TypeLineCurve typL     = L.TypeLineCurve();
    if (typL == TopOpeBRep_RESTRICTION)
    {
      onOOface = true;
      OOER     = TopoDS::Edge(L.Arc());
    }
    else
    {
      onOOface = ::FUN_LineRestF(OOface, L, ERL, OOER);
    }
    if (!onOOface)
      return false;

    double OOpar;
    ok = VP.ParonE(OOER, OOpar);
    if (!ok)
      ok = FUN_tool_parE(edge, paredge, OOER, OOpar);
    if (!ok)
      return false;

    // xpu051098 : cto900L4 (edge18,OOface5)
    ok = MKT.SetRest(OOER, OOpar);
    if (!ok)
      return false;
  }
  else if (interf3dtg)
  {
    int  absindex  = VP.ShapeIndex(); // 0,1,2,3
    bool on2edges  = (absindex == 3);
    bool hasONedge = (VP.State(OOShapeIndex) == TopAbs_ON);
    bool hasOOedge = (on2edges) ? true : hasONedge;

    if (hasOOedge)
    {
      TopoDS_Edge OOedge;
      double      OOpar = 1.e7;
      if (on2edges)
      {
        OOedge = TopoDS::Edge(VP.Edge(OOShapeIndex));
        OOpar  = VP.EdgeParameter(OOShapeIndex);
      }
      else
      {
        OOedge = TopoDS::Edge(VP.EdgeON(OOShapeIndex));
        OOpar  = VP.EdgeONParameter(OOShapeIndex);
      }

      ok = MKT.SetRest(OOedge, OOpar);
      if (!ok)
        return false;
    }
  }

  ok = MKT.MkTonE(stb, sta);
  if (!ok)
    return false;
  T.Before(stb);
  T.After(sta);
  return true;
} // FUN_newtransEdge

//-----------------------------------------------------------------------
static void FUN_ScanInterfList(const TopOpeBRepDS_Point&                                       PDS,
                               const occ::handle<TopOpeBRepDS_HDataStructure>&                 HDS,
                               const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loI,
                               NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loIfound)
//-----------------------------------------------------------------------
{
  // looks among the list of interferences <loI> for interferences
  // of geometry falling into <PDS>, add them to <loIfound>
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(loI);
  while (it.More())
  {
    bool found = HDS->ScanInterfList(it, PDS);
    if (found)
    {
      loIfound.Append(it.Value());
      if (it.More())
        it.Next();
    }
    else
      return;
  }
}

static bool FUN_selectTRAISHAinterference(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lI,
  const int                                                       ITRASHA,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lITRAonISHA)
// purpose : <lITRAonISHA> = {I = (T on ITRASHA,G,S)}
{
  lITRAonISHA.Clear();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I       = it.Value();
    const TopOpeBRepDS_Transition&                T       = I->Transition();
    int                                           iTRASHA = T.Index();
    // BUG :
    // POP : pb  : comparaison entre 2 enum differentes : on prend la valeur correspondante
    if (T.Orientation(TopAbs_IN) == TopAbs_EXTERNAL)
      continue; // xpu030998
    //    if (T.Orientation(TopAbs_IN) == TopAbs_UNKNOWN) continue; //xpu030998
    if (iTRASHA == ITRASHA)
      lITRAonISHA.Append(I);
  }
  bool noIfound = lITRAonISHA.IsEmpty();
  return !noIfound;
}

static bool FUN_selectGinterference(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lI,
  const int                                                       G,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lIonG)
{
  lIonG.Clear();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lI);
  for (; it.More(); it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    if (I->Geometry() == G)
      lIonG.Append(I);
  }
  bool noIfound = lIonG.IsEmpty();
  return !noIfound;
}

static bool FUN_sameGsameS(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loI,
                           const int&                                                      G,
                           const int&                                                      S,
                           NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       loIfound)
{
  loIfound.Clear();
  // Gets among the list <loI> the interferences of :
  //  geometry <G>, and support <S>
  TopOpeBRepDS_PointIterator PI(loI);
  for (; PI.More(); PI.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& EPI  = PI.Value();
    int                                           GEPI = EPI->Geometry();
    int                                           SEPI = EPI->Support();
    if (GEPI == G && SEPI == S)
      loIfound.Append(EPI);
  }
  return (loIfound.Extent() > 0);
}

//-----------------------------------------------------------------------
static void FUN_processCPI(TopOpeBRep_FacesFiller&        FF,
                           const TopOpeBRep_VPointInter&  VP,
                           const TopoDS_Shape&            F,
                           const int                      ShapeIndex,
                           const TopOpeBRep_LineInter&    L,
                           TopOpeBRepDS_PDataStructure    pDS,
                           const TopOpeBRepDS_Transition& transLine,
                           const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
                           const occ::handle<TopOpeBRepDS_Interference>&                   Ifound,
                           const bool&                                                     Gfound,
                           const TopOpeBRepDS_Kind&                                        PVKind,
                           const int&                                                      PVIndex,
                           int& keptVPnbr)
//-----------------------------------------------------------------------
{
  int OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;

  TopOpeBRepDS_Transition ttransLine = transLine;
  // prequesitory : current line is not on edge.
  double              parline    = VP.ParameterOnLine();
  bool                SIisvertex = VP.IsVertex(ShapeIndex);
  bool                OOisvertex = VP.IsVertex(OOShapeIndex);
  const TopoDS_Shape& E          = VP.Edge(ShapeIndex);

  // xpu010299 : we do not keep interferences with same parameters on curve
  //             PRO16120(f3,f4 -> null c1)
  if (!DSCIL.IsEmpty())
  {
    double par = FDS_Parameter(DSCIL.Last()); // parameter on curve
    double dd  = std::abs(par - parline);     // en fait, ce sont des entiers
    if (dd == 0)
      return;
  }

  // dist(p2d1,p2d2) < toluv => p2d1, p2d2 are considered equal.
  // NYI : compute uvtol with the original faces. By default, we set toluv = TolClass
  double toluv = 1.e-8;
  bool   keep  = FUNBREP_topokpart(Ifound, DSCIL, L, VP, (*pDS), E, F, toluv, parline, ttransLine);

  if (keep)
  {
    keptVPnbr++;
    if (keptVPnbr > 2)
      keep = false;
  }
  if (!keep)
    return;

  occ::handle<TopOpeBRepDS_Interference> CPI;
  {
    TopOpeBRepDS_Kind GKCPV;
    if (Gfound)
      GKCPV = PVKind;
    else
      GKCPV = (SIisvertex || OOisvertex) ? TopOpeBRepDS_VERTEX : TopOpeBRepDS_POINT;

    CPI = ::MakeCPVInterference(ttransLine, 0, PVIndex, parline, GKCPV);
    FF.StoreCurveInterference(CPI);
  }
}

static bool FUN_onedge(const TopOpeBRepDS_Point& PDS, const TopoDS_Edge& E)
{
  gp_Pnt        P    = PDS.Point();
  double        tolP = PDS.Tolerance();
  double        tolE = BRep_Tool::Tolerance(E);
  double        tol  = std::max(tolP, tolE);
  TopoDS_Vertex vf, vl;
  TopExp::Vertices(E, vf, vl);
  gp_Pnt pf    = BRep_Tool::Pnt(vf);
  bool   isonf = P.IsEqual(pf, tol);
  gp_Pnt pl    = BRep_Tool::Pnt(vl);
  bool   isonl = P.IsEqual(pl, tol);
  return isonf || isonl;
}

#ifdef OCCT_DEBUG
Standard_EXPORT void funraise()
{
  std::cout << "!!!!!!!!!! PVIndex = 0 !!!!!!!!!!" << std::endl;
}
#endif

//=================================================================================================

void TopOpeBRep_FacesFiller::ProcessVPonR(const TopOpeBRep_VPointInter&  VP,
                                          const TopOpeBRepDS_Transition& Trans,
                                          // const TopoDS_Shape& GFace,
                                          const TopoDS_Shape&,
                                          const int ShapeIndex) // 1,2
{
  int  absindex     = VP.ShapeIndex(); // 0,1,2,3
  int  iVP          = VP.Index();
  int  OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
  bool on2edges     = (absindex == 3);
  bool hasONedge    = (VP.State(OOShapeIndex) == TopAbs_ON);
  bool hasOOedge    = (on2edges) ? true : hasONedge;

  TopoDS_Face Face    = (*this).Face(ShapeIndex);
  int         iSIFace = myDS->Shape(Face);
  if (iSIFace == 0)
    iSIFace = myDS->AddShape(Face, ShapeIndex);
  TopoDS_Face OOFace  = (*this).Face(OOShapeIndex);
  int         iOOFace = myDS->Shape(OOFace);
  if (iOOFace == 0)
    iOOFace = myDS->AddShape(OOFace, OOShapeIndex);

  // current VPoint is on <edge>
  int                SIedgeIndex = 0;
  const TopoDS_Edge& edge        = TopoDS::Edge(VP.Edge(ShapeIndex));
  if (myDS->HasShape(edge))
    SIedgeIndex = myDS->Shape(edge);
  double paredge = VP.EdgeParameter(ShapeIndex);
  bool   isrest  = myDS->IsSectionEdge(edge);
  bool   closing = TopOpeBRepTool_ShapeTool::Closed(edge, Face);
  bool   dge     = BRep_Tool::Degenerated(edge);

  // dummy if !<hasOOedge>
  int  OOedgeIndex = 0;
  bool OOclosing, OOisrest;
  OOclosing = OOisrest = false;
  TopoDS_Edge OOedge;
  double      OOparedge = 0.;
  bool        dgOOe     = false;
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
    if (myDS->HasShape(OOedge))
      OOedgeIndex = myDS->Shape(OOedge);
    OOisrest  = myDS->IsSectionEdge(OOedge);
    OOclosing = TopOpeBRepTool_ShapeTool::Closed(OOedge, OOFace);
    dgOOe     = BRep_Tool::Degenerated(OOedge);
  }

#ifdef OCCT_DEBUG
  int ili = myLine->Index(), ivp = iVP, isi = ShapeIndex;
  GLOBAL_bvpr = TopOpeBRep_GettraceNVP(myexF1, myexF2, ili, ivp, isi);
  if (GLOBAL_bvpr)
    debvprmess(myexF1, myexF2, ili, ivp, isi);
#endif

  // degenerated edge processing
  // ---------------------------
  int               PVIndex = 0; // POINT or VERTEX index
  TopOpeBRepDS_Kind PVKind;
  bool              EPIfound, CPIfound;
  EPIfound = CPIfound = false;
  occ::handle<TopOpeBRepDS_Interference> IEPI, ICPI;
  ProcessVPondgE(VP,
                 ShapeIndex,
                 PVKind,
                 PVIndex, // out
                 EPIfound,
                 IEPI, // out
                 CPIfound,
                 ICPI); // out
  bool foundPVIndex = (PVIndex != 0);

  // ===================================================================
  //              <TransLine>, <transEdge>
  // ===================================================================

  bool wline        = (myLine->TypeLineCurve() == TopOpeBRep_WALKING);
  bool grestriction = (myLine->TypeLineCurve() == TopOpeBRep_RESTRICTION);
  bool glinenotoned = !wline && !grestriction && !myLineIsonEdge;

  // lasttransLine (for walking line)
  // -------------
  // set lasttransLine for a WALKING line
  bool dscilempty  = myDSCIL.IsEmpty();
  bool setlastonwl = wline && !dscilempty;
  if (setlastonwl)
  { // xpu171198, FRA61896 (f7,f13-> null DSC1)
    double parline = VP.ParameterOnLine();
    double par     = FDS_Parameter(myDSCIL.Last()); // parameter on curve
    double dd      = std::abs(par - parline);       // en fait, ce sont des entiers
    if (dd == 0)
      setlastonwl = false;
  }
  TopOpeBRepDS_Transition lasttransLine;
  if (setlastonwl)
    lasttransLine = myDSCIL.Last()->Transition();

  // edgeori, transLine
  // ------------------
  TopAbs_Orientation      edgeori = edge.Orientation();
  TopOpeBRepDS_Transition transLine;
  transLine    = TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP, ShapeIndex, edgeori);
  bool trliunk = transLine.IsUnknown();

  // 1_ If vpmin has transition OUT/IN, and vpmax is UNKNOWN,
  //    we change vpmax transition as IN/OUT
  //
  // 2_ If vpmin is UNKNOWN (if vp is first on line and transition is UNKNOWN,
  //    vpmin's transition is ON/ON the OOshape)
  //    we change vpmin transition as OUT/IN
  //
  // (kpart : sphere/box, with the sphere's sewing edge lying on one boxe's
  //  face and one of the edge's vertices IN the same face)

  int iINON1, iINONn, nINON;
  myLine->VPBounds(iINON1, iINONn, nINON);
  bool islastvp  = (iVP == iINONn);
  bool isfirstvp = (iVP == iINON1);

  bool keepvp = false;
  bool ret1   = false;
  if (trliunk)
  {
    // <transLine> is unknown :
    //  for a walking ->
    //     - if <myDSCIL> is not empty,
    //                   we set it as the last transition complemented
    //     - else,
    //          we look after an determinate transition on VP(i>iVP)
    //          and set transLine as this last complemented.
    //
    //  for a gline not on edge ->
    //     - if the transition on edge is unknown too and !<keepvp>,
    //       we do not keep it.
    //  elsewhere -> we do not keep <VP>

    if (wline)
    {
      if (setlastonwl)
        transLine = lasttransLine.Complement();
      else
        ::FUN_transForWL(*myLine, iVP, ShapeIndex, transLine);

      //  walki vpfirst on 3, vplast on 0, nvpkept = 2 kept
      if (transLine.IsUnknown())
      {
        // modified by NIZHNY-MKK  Mon Jul  3 11:30:03 2000.BEGIN
        bool keepvpfirst = dscilempty && isfirstvp && (nINON == 2);
        if (absindex == 3)
          keepvpfirst = keepvpfirst && myLastVPison0;
        // modified by NIZHNY-MKK  Mon Jul  3 11:30:21 2000.END
        if (keepvpfirst)
          transLine.Set(TopAbs_FORWARD);
        ret1 = false;
      }
    }
    else if (glinenotoned)
    {
      //       if (islastvp)  keepvp = !dscilempty;
      //       if (isfirstvp) keepvp = true;
      if (isfirstvp)
        keepvp = true;
      else
      {
        if (islastvp)
          keepvp = !dscilempty;
        else
        {
          if (!dge && !dgOOe && (VP.IsVertexOnS1() || VP.IsVertexOnS2()))
          {
            // If VP is on vertex we should compute at least one interference for the edge.
            // This interference is necessary at least to indicate that the edge intersect
            // something.
            const TopOpeBRep_VPointInter& aFirstPoint = myLine->VPoint(iINON1);
            const TopOpeBRep_VPointInter& aLastPoint  = myLine->VPoint(iINONn);

            for (int faceindex = 1; !keepvp && faceindex <= 2; faceindex++)
            {
              bool VPIsVertex = (faceindex == 1) ? VP.IsVertexOnS1() : VP.IsVertexOnS2();
              bool FirstPointIsVertex =
                (faceindex == 1) ? aFirstPoint.IsVertexOnS1() : aFirstPoint.IsVertexOnS2();
              bool LastPointIsVertex =
                (faceindex == 1) ? aLastPoint.IsVertexOnS1() : aLastPoint.IsVertexOnS2();
              if (VPIsVertex)
              {
                const TopoDS_Shape& aV1 = (faceindex == 1) ? VP.VertexOnS1() : VP.VertexOnS2();
                if (FirstPointIsVertex)
                {
                  const TopoDS_Shape& aV2 =
                    (faceindex == 1) ? aFirstPoint.VertexOnS1() : aFirstPoint.VertexOnS2();
                  if (aV1.IsSame(aV2))
                  {
                    keepvp = true;
                  }
                }
                if (!keepvp && LastPointIsVertex)
                {
                  const TopoDS_Shape& aV2 =
                    (faceindex == 1) ? aLastPoint.VertexOnS1() : aLastPoint.VertexOnS2();
                  if (aV1.IsSame(aV2))
                  {
                    keepvp = !dscilempty;
                  }
                }
              }
            }
          }
        }
      }
      ret1 = !keepvp;
    }
    else
      ret1 = true;
  }
  trliunk = transLine.IsUnknown();
  if (ret1)
    return;

  // Transori, transEdge
  // -------------------
  TopAbs_Orientation      Transori = Trans.Orientation(TopAbs_IN);
  TopOpeBRepDS_Transition transEdge =
    TopOpeBRep_FFTransitionTool::ProcessEdgeTransition(VP, ShapeIndex, Transori);
  bool               Tunknown = FDS_hasUNK(transEdge);
  TopOpeBRepDS_Point PDS      = TopOpeBRep_PointGeomTool::MakePoint(VP); // <VP>'s geometry
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>
       lITOOFonVP; // {I on <edge> = (T on <OOface>, G on <VP>, S)}
  bool found = false;
  if (SIedgeIndex != 0)
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        lI;
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lIedge =
      myDS->ShapeInterferences(edge);
    if (PVIndex == 0)
      ::FUN_ScanInterfList(PDS, myHDS, lIedge, lI);
    else
      ::FUN_selectGinterference(lIedge, PVIndex, lI);
    found = ::FUN_selectTRAISHAinterference(lI, iOOFace, lITOOFonVP);
  }

  //  if (found && myLineINL && Tunknown) return; //xpu220998 : cto cylcong A1 (edge8,OOface4)

  // <Transori> = INTERNAL or EXTERNAL (tangent cases), compute <transEdge>
  bool               newtransEdge = (Transori == TopAbs_INTERNAL) || (Transori == TopAbs_EXTERNAL);
  TopAbs_Orientation otransEdge   = transEdge.Orientation(TopAbs_IN);
  bool               allINT = (Transori == TopAbs_INTERNAL) || (otransEdge == TopAbs_INTERNAL);
  bool               allEXT = (Transori == TopAbs_EXTERNAL) || (otransEdge == TopAbs_EXTERNAL);

  newtransEdge = newtransEdge && (!allINT) && (!allEXT);
  newtransEdge = newtransEdge || Tunknown;
  // -> intersection fails for closing edges
  // 1. <edge> touches closing <OOedge> at <VP> && <OOedge> is tangent to <OOFace>,
  // intersection -> 1 forward && 1 reversed instead of internal/external.
  // 2. if <edge> is tangent to <OOFace> at <VP> on walking
  newtransEdge = newtransEdge || closing || OOclosing;
  newtransEdge = newtransEdge && (!myLineINL);
  if (newtransEdge)
  {
    myDS->Shape(edge);
    newtransEdge = !found;
    if (found)
    {
      // Getting first transition found
      // prequesitory : transition on edge / <OOF> on same geometry point is unchanged
      TopOpeBRepDS_Transition Tr = lITOOFonVP.First()->Transition();
      transEdge.Before(Tr.Before());
      transEdge.After(Tr.After());
    }
    if (newtransEdge)
    {
      // Compute of <transEdge> : transition on <edge> at geometry <VP> / <OOface>
      // if line on a restriction OOedge of <OOface> : gets <edge> transition/<OOface> when
      //                                               at <VP> on OOedge.
      // if line is not on restriction : gets <edge> transition/<OOface>.
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
      if (ok)
      {
        transEdge.Before(Tr.Before());
        transEdge.After(Tr.After());
      }
      newtransEdge = ok;
    }
  } // newtransEdge

  bool tredunk = transEdge.IsUnknown();
  bool ret2    = false;
  if (tredunk)
  {
    if (!trliunk)
      transEdge = transLine.Complement();
    if (trliunk && !keepvp)
      ret2 = true;
  }
  if (ret2)
    return;
  tredunk = transEdge.IsUnknown();

  // ===================================================================
  //              DS geometry Management
  // ===================================================================
  // SI*** : data issued from shape ShapeIndex
  // OO*** : data issued from other shape

  if (SIedgeIndex == 0)
    SIedgeIndex = myDS->AddShape(edge, ShapeIndex);

  bool SIisvertex = VP.IsVertex(ShapeIndex);
  bool OOisvertex = VP.IsVertex(OOShapeIndex);

  // <PVIndex>, <PVKind> :
  // --------------------
  // search for an interference with a equal 3D geometry
  // if found, set <PVIndex> to index of geometry found
  // if not found, make a new geometry PVIndex with 3d point or vertex

  // modified by NIZHNY-MKK  Tue Apr  3 12:08:38 2001.BEGIN
  bool ismultiplekind = foundPVIndex && !EPIfound && !CPIfound && (SIisvertex || OOisvertex)
                        && (PVKind == TopOpeBRepDS_POINT);

  //   if (!foundPVIndex) FUN_VPIndex ((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL, //in
  if (!foundPVIndex || ismultiplekind)
    FUN_VPIndex((*this),
                (*myLine),
                VP,
                ShapeIndex,
                myHDS,
                myDSCIL, // in
                // modified by NIZHNY-MKK  Tue Apr  3 12:13:17 2001.END
                PVKind,
                PVIndex, // out
                EPIfound,
                IEPI, // out
                CPIfound,
                ICPI, // out
                M_MKNEWVP);
  if (PVIndex == 0)
  {
#ifdef OCCT_DEBUG
    funraise();
#endif
  }

  bool VPonedge = false;
  if (PVKind == TopOpeBRepDS_VERTEX)
    VPonedge = ::FUN_onedge(PDS, edge);
  if (myLineINL)
  {
    double tolang = Precision::Angular() * 1.e5; //=1.e-7 NYITOLXPU

    gp_Vec   tgE = FUN_tool_tggeomE(paredge, edge);
    gp_Pnt2d OOuv;
    bool     ok = false;
    if (VPonedge)
    {
      OOuv = VP.SurfaceParameters(OOShapeIndex);
      ok   = true;
    }
    else
    {
      ok = FUN_tool_paronEF(OOedge, OOparedge, OOFace, OOuv);
    }
    gp_Vec ntOOF;
    if (ok)
      ntOOF = FUN_tool_nggeomF(OOuv, OOFace);
    if (OOFace.Orientation() == TopAbs_REVERSED)
      ntOOF.Reverse();

    double tol = 1.e-7;
    if (ok)
      ok = (tgE.Magnitude() > tol) && (ntOOF.Magnitude() > tol);
    double dot = 1.e7;
    if (ok)
      dot = gp_Dir(tgE).Dot(gp_Dir(ntOOF));

    occ::handle<Geom_Surface> su   = BRep_Tool::Surface(OOFace);
    bool                      apex = FUN_tool_onapex(OOuv, su);
    TopOpeBRepDS_Transition   T;
    if (!apex && ok && (std::abs(dot) > tolang))
    {
      TopAbs_Orientation ori = (dot < 0.) ? TopAbs_FORWARD : TopAbs_REVERSED;
      T.Set(ori);
    }

    if (VPonedge && (!dge))
    {
      // xpu231098 : cto904C8(edge11)
      //  xpu131198 : CTS21802(edge31)
      if (iOOFace == 0)
        iOOFace = myDS->AddShape(OOFace, OOShapeIndex);
      occ::handle<TopOpeBRepDS_Interference> EPIf;
      {
        T.Index(iOOFace);
        EPIf =
          MakeEPVInterference(T, iOOFace, PVIndex, paredge, PVKind, TopOpeBRepDS_FACE, SIisvertex);
      }
      myHDS->StoreInterference(EPIf, edge);
      if (on2edges || hasONedge)
      {
        if (OOedgeIndex == 0)
          OOedgeIndex = myDS->AddShape(OOedge, OOShapeIndex);
        occ::handle<TopOpeBRepDS_Interference> EPI;
        {
          T.Index(iOOFace);
          EPI = MakeEPVInterference(T, OOedgeIndex, PVIndex, paredge, PVKind, SIisvertex);
        }
        myHDS->StoreInterference(EPI, edge);
      }
      return;
    } // VPonedge
    else
    {
      // compute interferences later on
      // modified by NIZHNY-MZV  Thu Dec 23 13:27:10 1999
      if (!T.IsUnknown())
      {
        transEdge.Before(T.Before());
        transEdge.After(T.After());
      }
    }
  } // myLineINL

  // Gfound = VP corresponds with an existing geometry of ShapeIndex
  bool Gfound = (EPIfound || CPIfound);
#ifdef OCCT_DEBUG
  if (GLOBAL_bvpr)
    debvprmess(myexF1, myexF2, ili, ivp, isi);
#endif

  // ===================================================================
  //          Current VPoint VP is kept
  // ===================================================================

  // ------------------------------------------
  // -- Curve/(POINT,VERTEX) Interference (CPI)
  // ------------------------------------------

  bool noCPI = myLineIsonEdge;
  noCPI      = noCPI || (!on2edges && hasOOedge && (OOisrest || isrest));

  bool condi = (!noCPI);
  condi      = condi && (!myLineINL); // INL
  if (condi)
  {
    int keptVPnbr = mykeptVPnbr;
    FUN_processCPI((*this),
                   VP,
                   Face,
                   ShapeIndex,
                   (*myLine),
                   myDS,
                   transLine,
                   myDSCIL,
                   ICPI,
                   Gfound,
                   PVKind,
                   PVIndex,
                   keptVPnbr);
    mykeptVPnbr = keptVPnbr;
  }

  // ------------------------------------------
  // --- Edge/(POINT,VERTEX) Interference (EPI)
  // ------------------------------------------

  //  if (on2edges && !Gfound && !closing) {
  bool condi2 = (on2edges && !closing);
  condi2      = condi2 || (hasONedge && !closing);
  if (condi2 && (!dge))
  {
    if (OOedgeIndex == 0)
      OOedgeIndex = myDS->AddShape(OOedge, OOShapeIndex);

    occ::handle<TopOpeBRepDS_Interference> EPI;
    {
      TopOpeBRepDS_Transition T = transEdge;
      if (iOOFace == 0)
        iOOFace = myDS->AddShape(OOFace, OOShapeIndex);
      T.Index(iOOFace);
      EPI = MakeEPVInterference(T, OOedgeIndex, PVIndex, paredge, PVKind, SIisvertex);
    }
    myHDS->StoreInterference(EPI, edge);
  } // condi2

  // ===================================================================
  // manip corrective d'un pb. d'intersection
  // - le VPoint est donne sur une restriction de ShapeIndex (1 ou 2),
  // - le VPoint est ON une restriction de l'autre shape (OOShapeIndex)
  // --> le VPoint n'est PAS donne sur restriction de OOShapeIndex NYI
  //     par les intersections (a ameliorer). NYI
  // L'etat ON sur OOShapeIndex indique que le point est sur une
  // de ses restrictions :
  // - on ne met PAS le point dans les CPIs
  // - on met le point dans les EPIs de l'arete de OOShapeIndex
  // ===================================================================

  bool correctON   = !on2edges && hasONedge && !dgOOe;
  bool correctedON = false;
  if (correctON)
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lITFonVP;
    bool                                                     OOfound = false;
    if (OOedgeIndex != 0)
    {
      const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lIOOedge =
        myDS->ShapeInterferences(OOedge);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lI;
      ::FUN_ScanInterfList(PDS, myHDS, lIOOedge, lI);
      OOfound   = ::FUN_selectTRAISHAinterference(lI, iSIFace, lITFonVP);
      correctON = !OOfound;
    }
  }
  if (correctON)
  {
    if (OOedgeIndex == 0)
      OOedgeIndex = myDS->AddShape(OOedge, OOShapeIndex);

    // VP a ete classifie ON sur l'edge <OOedge>.
    // calcul de la transition <tOOedge> sur l'arete <OOedge>
    // (de l'autre face en jeu, OOShapeIndex) ou le VP est donne ON.
    // On tient compte de l'orientation de <edge> dans <Face>.
    // (bug IntPatch_Line : VP n'a pas de donnees en OOShapeIndex
    // alors qu'il est dessus)

    TopOpeBRepDS_Transition tOOedge;
    // distinguish whether OOedge is the edge on which geometric line lies.
    // OOedge == edge(line) ==> tOOedge = f(orientation of <edge> in <Face> FORWARD)
    // OOedge != edge(line) ==> tOOedge = f(orientation of <Face>)
    double OOpar1, OOpar2;
    bool   isonper;
    FDS_LOIinfsup((*myDS),
                  OOedge,
                  OOparedge,
                  PVKind,
                  PVIndex,
                  myDS->ShapeInterferences(OOedge),
                  OOpar1,
                  OOpar2,
                  isonper);
    // FDS_getupperlower(myHDS,OOedgeIndex,OOparedge,par1,par2);
    gp_Pnt2d OOuv = VP.SurfaceParameters(ShapeIndex);

    //       <Tr> relative to 3d <OOface> matter,
    //       we take into account <Tr> / 2d <OOface> only if <edge> is normal to <OOface>
    double tola = Precision::Angular() * 1.e+2; // dealing with tolerances

    // KK : supplying tolerances pbm (tola too small)
    bool EsdmEofF = myHDS->HasSameDomain(OOedge);
    if (EsdmEofF)
    {
      TopExp_Explorer ex;
      for (ex.Init(Face, TopAbs_EDGE); ex.More(); ex.Next())
        if (FUN_ds_sdm(*myDS, ex.Current(), OOedge))
        {
          EsdmEofF = true;
          break;
        }
    }
    bool OOEtgF = true;
    if (!EsdmEofF)
      OOEtgF = FUN_tool_EtgF(OOparedge, OOedge, OOuv, Face, tola);
    bool OOrest   = FUN_INlos(edge, myERL);
    bool interf2d = OOEtgF && (OOisrest || OOrest);

    double                        factor = 1.e-2;
    TopOpeBRepTool_makeTransition MKT;
    bool ok = MKT.Initialize(OOedge, OOpar1, OOpar2, OOparedge, Face, OOuv, factor);

    if (ok && !(interf2d && !MKT.IsT2d()))
    {
      MKT.SetRest(edge, paredge);
      TopAbs_State stb, sta;
      ok = MKT.MkTonE(stb, sta);
      if (ok)
      {
        tOOedge.Before(stb);
        tOOedge.After(sta);
        occ::handle<TopOpeBRepDS_Interference> OOEPIe;
        {
          if (iSIFace == 0)
            iSIFace = myDS->AddShape(Face, ShapeIndex);
          TopOpeBRepDS_Transition OOT = tOOedge;
          OOT.Index(iSIFace);
          OOEPIe = MakeEPVInterference(OOT, SIedgeIndex, PVIndex, OOparedge, PVKind, OOisvertex);
        }
        myHDS->StoreInterference(OOEPIe, OOedge);

        // xpu : 09-03-98
        // hsd3d => interf2d  : only IwithSkEDGE interf
        // elsewhere            : add an IwithSkFACE interference.
        bool                           addEPIf = !myLineIsonEdge;
        NCollection_List<TopoDS_Shape> dummy;
        bool                           hsd3d = FDS_HasSameDomain3d(*myDS, OOedge, &dummy);
        if (hsd3d)
          addEPIf = false;
        if (addEPIf)
        {
          TopOpeBRepDS_Transition OOT = tOOedge;
          OOT.Index(iSIFace);
          occ::handle<TopOpeBRepDS_Interference> OOEPIf = MakeEPVInterference(OOT,
                                                                              iSIFace,
                                                                              PVIndex,
                                                                              OOparedge,
                                                                              PVKind,
                                                                              TopOpeBRepDS_FACE,
                                                                              OOisvertex);
          myHDS->StoreInterference(OOEPIf, OOedge);
        }
        // xpu : 09-03-98
        correctedON = true;
      } // ok
    }
  } // correctON

  if (correctON && !correctedON && noCPI && !myLineIsonEdge)
  {
    // MSV: correct ON failed, so store CPI
    int keptVPnbr = mykeptVPnbr;
    FUN_processCPI((*this),
                   VP,
                   Face,
                   ShapeIndex,
                   (*myLine),
                   myDS,
                   transLine,
                   myDSCIL,
                   ICPI,
                   Gfound,
                   PVKind,
                   PVIndex,
                   keptVPnbr);
    mykeptVPnbr = keptVPnbr;
  }

  // closing edge processing
  // -----------------------
  if ((OOclosing || closing) && !found)
  {
    ProcessVPonclosingR(VP, Face, ShapeIndex, transEdge, PVKind, PVIndex, EPIfound, IEPI);
    return;
  }

  // VP processing
  // -------------

  bool addEPI = false;
  if (!Gfound)
  {
    addEPI = true;
  }
  else
  { // EPIfound
    TopAbs_Orientation anOtransEdge = transEdge.Orientation(TopAbs_IN);

    bool opporifound, memorifound;
    opporifound = memorifound = false;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>        loIfound;
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& EPIL =
      myDS->ShapeInterferences(edge);
    bool ok = FUN_sameGsameS(EPIL, PVIndex, iOOFace, loIfound);
    if (ok)
    {
      TopOpeBRepDS_PointIterator PI(loIfound);
      // on cree une EPI orientee <transEdge> ssi :
      // - il en existe deja une d'orientation opposee a TransEdge
      // - il n'en existe pas deja une d'orientation identique a TransEdge
      for (; PI.More(); PI.Next())
      {
        TopAbs_Orientation oEPI = PI.Value()->Transition().Orientation(TopAbs_IN);
        if (!memorifound)
          memorifound = (oEPI == anOtransEdge);
        if (!opporifound)
          opporifound = (oEPI == TopAbs::Complement(anOtransEdge));
        addEPI = (opporifound && !memorifound);
        if (addEPI)
          break;
      }
    }
    if (!ok)
      addEPI = true;
  } // EPIfound

#ifdef OCCT_DEBUG
  if (GLOBAL_bvpr)
    debvprmess(myexF1, myexF2, ili, ivp, isi);
#endif

  // xpu030998 : edge has restriction on OOface, do NOT append EPIf
  //     cto904A3 (edge19,OOface14,vG16),
  if (myLineINL)
  {
    double   tola   = Precision::Angular() * 1.e+4; // dealing with tolerances
    gp_Pnt2d uv     = VP.SurfaceParameters(OOShapeIndex);
    bool     EtgOOF = FUN_tool_EtgF(paredge, edge, uv, OOFace, tola);
    bool     inERL  = FUN_INlos(edge, myERL);
    if (EtgOOF && inERL)
      return; // cto904A3
  }

  if (addEPI && (!dge))
  {
    // ShapeIndex = 1,2 --> OOShapeIndex = 2,1
    // point est sur une seule arete <edge> de <ShapeIndex>
    // le Support de l'interference est l'autre
    // face (OOShapeIndex) / ShapeIndex
    if (iOOFace == 0)
      iOOFace = myDS->AddShape(OOFace, OOShapeIndex);
    occ::handle<TopOpeBRepDS_Interference> EPIf;
    {
      TopOpeBRepDS_Transition T = transEdge;
      T.Index(iOOFace);
      EPIf =
        MakeEPVInterference(T, iOOFace, PVIndex, paredge, PVKind, TopOpeBRepDS_FACE, SIisvertex);
    }
    myHDS->StoreInterference(EPIf, edge);
  } // addEPI

} // ProcessVPonR
