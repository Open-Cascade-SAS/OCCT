// Created on: 1997-11-13
// Created by: Xuan PHAM PHU
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

#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <gp_Pnt2d.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>

#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <Standard_Failure.hxx>
#include <TopOpeBRepTool_PURGE.hxx>

#ifdef OCCT_DEBUG
// Standard_EXPORT int STATIC_PURGE_iwi = 0;
// Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> STATIC_PURGE_mapw,
// STATIC_PURGE_mapv; Standard_EXPORT NCollection_IndexedMap<TopoDS_Shape> STATIC_PURGE_mapeds,
// STATIC_CORR_mapeds;

Standard_EXPORT void debcorrUV() {};
extern bool          TopOpeBRepTool_GettracePURGE();
extern bool          TopOpeBRepTool_GettraceCORRISO();
#endif
// DEB

#define SPLITEDGE (0)
#define INCREASE (1)
#define DECREASE (-1)

#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

static void FUN_addtomap(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& map,
  const TopoDS_Shape&                                                                         key,
  const TopoDS_Shape&                                                                         item)
{
  if (map.IsBound(key))
    map.ChangeFind(key).Append(item);
  else
  {
    NCollection_List<TopoDS_Shape> los;
    los.Append(item);
    map.Bind(key, los);
  }
}

static bool FUN_getv(const TopAbs_Orientation& orivine, const TopoDS_Shape& e, TopoDS_Shape& v)
{
  v.Nullify();
  // gets <v> oriented <orivine> in <e>
  TopAbs_Orientation oe     = e.Orientation();
  bool               isnonO = M_INTERNAL(oe) || M_EXTERNAL(oe);
  TopoDS_Shape       eO;
  if (isnonO)
  {
    eO = e.Oriented(TopAbs_FORWARD);
  }
  else
  {
    eO = e;
  }
  TopExp_Explorer exv(eO, TopAbs_VERTEX);
  for (; exv.More(); exv.Next())
  {
    const TopoDS_Shape& vcur = exv.Current();
    if (vcur.Orientation() == orivine)
    {
      v = vcur;
      return true;
    }
  } // exv
  return false;
}

Standard_EXPORT bool FUN_tool_ClosedW(const TopoDS_Wire& W)
{
  // !! an edge oriented INTERNAL/EXTERNAL has all its vertices
  // oriented INTERNAL/EXTERNAL.

  // <mapvedsO> = {(v,loe)} / e is oriented :
  // <mapvedsO> = {(v,loe)} / e is not oriented (INTERNAL/EXTERNAL) :
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapvFine, mapvRine, mapvIine;

  TopExp_Explorer exe(W, TopAbs_EDGE);
  for (; exe.More(); exe.Next())
  {
    const TopoDS_Shape& e      = exe.Current();
    TopAbs_Orientation  oe     = e.Orientation();
    bool                isnonO = M_INTERNAL(oe) || M_EXTERNAL(oe);
    TopoDS_Shape        eO;
    if (isnonO)
    {
      eO = e.Oriented(TopAbs_FORWARD);
    }
    else
    {
      eO = e;
    }

    TopExp_Explorer exv(eO, TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
    {
      const TopoDS_Shape& v    = exv.Current();
      TopAbs_Orientation  oriv = v.Orientation();
      if (M_FORWARD(oriv))
        FUN_addtomap(mapvFine, v, e);
      if (M_REVERSED(oriv))
        FUN_addtomap(mapvRine, v, e);
      if (M_INTERNAL(oriv))
        FUN_addtomap(mapvIine, v, e);
    }
  }

  if (mapvFine.Extent() == 0)
    return false; // empty wire

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapvok;
  // a vertex is found valid if is - an internal vertex
  //                               - found FORWARD and REVERSED.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>
    mapvonlyFine; // {(v,e)} v F in e, v belongs to only one e
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itvFine(mapvFine);
  for (; itvFine.More(); itvFine.Next())
  {
    const TopoDS_Shape& vFine = itvFine.Key();
    bool                vIine = mapvIine.IsBound(vFine);
    if (vIine)
    {
      mapvok.Add(vFine);
      continue;
    }
    bool vRine = mapvRine.IsBound(vFine);
    if (vRine)
    {
      mapvok.Add(vFine);
      continue;
    }
    mapvonlyFine.Add(vFine);
  }
  // <mapvRinonee> = {(v,e)} v R in e, v belongs to only one e
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapvonlyRine;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itvRine(mapvRine);
  for (; itvRine.More(); itvRine.Next())
  {
    const TopoDS_Shape& vRine = itvRine.Key();
    bool                vok   = mapvok.Contains(vRine);
    if (vok)
      continue;
    bool vIine = mapvIine.IsBound(vRine);
    if (vIine)
    {
      mapvok.Add(vRine);
      continue;
    }
    mapvonlyRine.Add(vRine);
  }

  // checking vertices <mapvonlyFine> and <mapvonlyRine>
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itvonlyFRine;
  int                                                              nmap = 0;
  while (nmap <= 2)
  {
    nmap++;
    bool vFine = (nmap == 1);
    if (vFine)
      itvonlyFRine.Initialize(mapvonlyFine);
    else
      itvonlyFRine.Initialize(mapvonlyRine);

    for (; itvonlyFRine.More(); itvonlyFRine.Next())
    {
      const TopoDS_Shape&            vtocheck = itvonlyFRine.Key();
      NCollection_List<TopoDS_Shape> edsvFRine;
      if (vFine)
        edsvFRine = mapvFine.Find(vtocheck);
      else
        edsvFRine = mapvRine.Find(vtocheck);

      if (edsvFRine.Extent() > 1)
        return false; // faulty wire
      const TopoDS_Shape& e = edsvFRine.First();

      TopAbs_Orientation ovori = vFine ? TopAbs_REVERSED : TopAbs_FORWARD;
      TopoDS_Shape       ov;
      bool               ovfound = FUN_getv(ovori, e, ov);
      if (!ovfound)
        return false; // faulty edge

      // <vtocheck> is on only one edge <e>,
      // <vtocheck> is FORWARD/REVERSED in <e>,
      // <ovfound> is REVERSED/FORWARD in <e>.
      // <vtocheck> is ok if : - <ovfound> is INTERNAL in another edge
      //                       - <ovfound> is FORWARD and REVERSED in
      //                         one or two other edges.
      //                      and e is not oriented
      TopAbs_Orientation oe = e.Orientation();
      if (M_FORWARD(oe) || M_REVERSED(oe))
        return false;
      if (!mapvok.Contains(ov))
        return false;

      bool ovIine = mapvIine.IsBound(ov);
      if (ovIine)
        continue;
      bool ovFine = mapvRine.IsBound(ov);
      if (!ovFine)
        return false;
      bool ovRine = mapvRine.IsBound(ov);
      if (!ovRine)
        return false;

      const NCollection_List<TopoDS_Shape>& edsovFine = mapvFine.Find(ov);
      const NCollection_List<TopoDS_Shape>& edsovRine = mapvRine.Find(ov);
      if (edsovFine.Extent() > 1)
        continue;
      if (edsovRine.Extent() > 1)
        continue;
      if (edsovFine.First().IsEqual(e))
        return false;
      if (edsovRine.First().IsEqual(e))
        return false;
    }
  } // nmap
  return true;
}

//=================================================================================================

bool TopOpeBRepTool::PurgeClosingEdges(
  const TopoDS_Face& Fin,
  const TopoDS_Face& FF,
  //				      const NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>&
  // MWisOld,
  const NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>&,
  NCollection_IndexedMap<TopoDS_Shape>& MshNOK)
{
  // Fin is the original face
  // FF  is the split face

  // prequesitory : split edges, of edge ancestor a closing edge
  //                keep in memory the geometry of the ancestor edge,
  //                they answer True to BRep_Tool::IsClosed.
  // elsewhere : we have to get this information using geometric
  //             criteriums (TopOpeBRepTool_TOOL::IsonCLO)

#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettracePURGE();
  if (trc)
    std::cout << "\n* PurgeClosingEdges:\n\n";
#endif
  TopOpeBRepTool_CORRISO CORRISO(Fin);
  double                 tolF = BRep_Tool::Tolerance(Fin);
  double                 uperiod;
  bool                   uclosed = CORRISO.Refclosed(1, uperiod);
  double                 vperiod;
  bool                   vclosed = CORRISO.Refclosed(2, vperiod);
  if (!uclosed && !vclosed)
    return false;
  bool   inU  = uclosed;
  double xmin = inU ? (CORRISO.GASref().FirstUParameter()) : (CORRISO.GASref().FirstVParameter());
  double xper = inU ? uperiod : vperiod;
  double tolx = inU ? (CORRISO.Tol(1, tolF)) : (CORRISO.Tol(2, tolF));

  TopExp_Explorer exw(FF, TopAbs_WIRE);
  for (; exw.More(); exw.Next())
  {
    const TopoDS_Shape& W = exw.Current();

    CORRISO.Init(W);
    bool ok = CORRISO.UVClosed();
    if (ok)
      continue;

    NCollection_List<TopoDS_Shape>           cEds;
    NCollection_List<TopoDS_Shape>::Iterator ite(CORRISO.Eds());
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Edge& E       = TopoDS::Edge(ite.Value());
      bool               closing = BRep_Tool::IsClosed(E, Fin);
      if (!closing)
      { // xpu231198 : pcurve modified, the information is lost
        TopOpeBRepTool_C2DF C2DF;
        bool                isb = CORRISO.UVRep(E, C2DF);
        if (!isb)
          return false; // NYIRAISE
        bool onclo = TopOpeBRepTool_TOOL::IsonCLO(C2DF, inU, xmin, xper, tolx);
        if (onclo)
          closing = true;
      }
      if (closing)
        cEds.Append(E);
    }
    int  ncE     = cEds.Extent();
    bool nopurge = (ncE <= 1);
    if (nopurge)
      return true;

    // Checking <W>
    NCollection_List<TopoDS_Shape> fyEds;
    bool                           topurge = CORRISO.PurgeFyClosingE(cEds, fyEds);
    if (topurge)
    {
      NCollection_List<TopoDS_Shape>::Iterator it(fyEds);
      for (; it.More(); it.Next())
        MshNOK.Add(it.Value());
      MshNOK.Add(W);
      MshNOK.Add(FF);
    }

#ifdef OCCT_DEBUG
    if (trc && topurge)
      std::cout << "found FAULTY edge = ed" << std::endl;
#endif
  } // exw
  return true;
}

//=================================================================================================

bool TopOpeBRepTool::PurgeClosingEdges(
  const TopoDS_Face&                                                     Fin,
  const NCollection_List<TopoDS_Shape>&                                  LOF,
  const NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>& MWisOld,
  NCollection_IndexedMap<TopoDS_Shape>&                                  MshNOK)
{
  bool uvclosed = FUN_tool_closedS(Fin);
  if (!uvclosed)
    return true;

  NCollection_List<TopoDS_Shape>::Iterator it(LOF);
  for (; it.More(); it.Next())
  {
    const TopoDS_Face& FF = TopoDS::Face(it.Value());
    bool               ok = TopOpeBRepTool::PurgeClosingEdges(Fin, FF, MWisOld, MshNOK);
    if (!ok)
      return false;
  }
  return true;
}

/*static bool FUN_correctClosingE(TopoDS_Edge& newfE, TopoDS_Face& Fsp)
{
  double f,l,tol; occ::handle<Geom2d_Curve> PC = FUNTOOLC2D_CurveOnSurface(newfE,Fsp,f,l,tol);
  gp_Dir2d d2d; gp_Pnt2d O2d; bool isuiso,isviso;
  bool uviso = FUN_tool_IsUViso(PC,isuiso,isviso,d2d,O2d);
  if (!uviso) return false;

  double period = 2*M_PI;
  double piso = isuiso? O2d.X(): O2d.Y();
  double tol2d = 1.e-6;
  bool is0   = std::abs(piso) < tol2d;
  bool is2PI = std::abs(period-piso) < tol2d;
  // --------------------------------------------------
  //  prequesitory :  Closed Surfaces have period 2PI
  if (!is0 && !is2PI) return false;
  // --------------------------------------------------
  double factor = is0? period: -period;
  gp_Vec2d transl(1.,0.); if (isviso) transl = gp_Vec2d(0.,1.);
  transl.Multiply(factor);

  int ok = FUN_tool_translate(transl,Fsp,newfE);
  return true;
}

static bool FUN_correctDegeneratedE
(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher>& mve,TopoDS_Edge& Ein,TopoDS_Face& Fsp)
{
  TopoDS_Vertex v1,v2; TopExp::Vertices(Ein,v1,v2);
  TopAbs_Orientation ov1 = v1.Orientation();
  TopAbs_Orientation ov2 = v2.Orientation();
  bool ok1 = mve.Contains(v1); if (!ok1) return false;
  bool ok2 = mve.Contains(v2); if (!ok2) return false;

  const NCollection_List<TopoDS_Shape>& le= mve.FindFromKey(v1);

  TopoDS_Edge e1,e2; bool fe1 = false; bool fe2 = false;
  TopoDS_Vertex vfe1,vfe2;
  bool fEin = false;
  for(NCollection_List<TopoDS_Shape>::Iterator itle(le);itle.More();itle.Next()) {
    const TopoDS_Shape& ecx = TopoDS::Edge(itle.Value());
    if (ecx.IsEqual(Ein)) {
      fEin = true;
    }
    else {
      TopExp_Explorer exv;
      for (exv.Init(ecx,TopAbs_VERTEX);exv.More();exv.Next()) {
//      for (TopExp_Explorer exv(ecx,TopAbs_VERTEX);exv.More();exv.Next()) {
    const TopoDS_Vertex& vecx = TopoDS::Vertex(exv.Current());
    bool issam = vecx.IsSame(v1);
    if (issam) {
      bool iseq1 = vecx.IsEqual(v1);
      bool iseq2 = vecx.IsEqual(v2);
      if (!iseq1) {
        e1 = TopoDS::Edge(ecx);
        vfe1 = TopoDS::Vertex(vecx);
        fe1 = true;
      }
      if (!iseq2) {
        e2 = TopoDS::Edge(ecx);
        vfe2 = TopoDS::Vertex(vecx);
        fe2 = true;
      }
    }
    if (fe1 && fe2) break;
      }
      if (fe1 && fe2) break;
    }
    if (fEin && fe1 && fe2) break;
  } // itle.More()

  bool ok = (fEin && fe1 && fe2);
  if (!ok) return false;

#ifdef OCCT_DEBUG
  debcorrUV(); // call Draw_Call("av2d;dy fyf;fit;ppcu fyf")
#endif

  double f,l,tol; occ::handle<Geom2d_Curve> PC = FUNTOOLC2D_CurveOnSurface(Ein,Fsp,f,l,tol);
  gp_Dir2d d2d; gp_Pnt2d O2d; bool isuiso = 0,isviso = 0;
  bool uviso = FUN_tool_IsUViso(PC,isuiso,isviso,d2d,O2d);
  if (!uviso) return false;

  double pfEin,plEin,tolEin; occ::handle<Geom2d_Curve> PCEin =
FUNTOOLC2D_CurveOnSurface(Ein,Fsp,pfEin,plEin,tolEin); if (PCEin.IsNull()) throw Standard_Failure("
FUN_correctDegeneratedE : no 2d curve Ein"); double pf1,pl1,tol1; occ::handle<Geom2d_Curve> PC1 =
FUNTOOLC2D_CurveOnSurface(e1,Fsp,pf1,pl1,tol1); if (PC1.IsNull()) throw Standard_Failure("
FUN_correctDegeneratedE : no 2d curve e1"); double pf2,pl2,tol2; occ::handle<Geom2d_Curve> PC2 =
FUNTOOLC2D_CurveOnSurface(e2,Fsp,pf2,pl2,tol2); if (PC2.IsNull()) throw Standard_Failure("
FUN_correctDegeneratedE : no 2d curve e2");

  double parv1 = BRep_Tool::Parameter(v1,Ein,Fsp);
  double parv2 = BRep_Tool::Parameter(v2,Ein,Fsp);
  gp_Pnt2d  pv1; PCEin->D0(parv1,pv1);
  gp_Pnt2d  pv2; PCEin->D0(parv2,pv2);

  double par1 = BRep_Tool::Parameter(vfe1,e1,Fsp);
  double par2 = BRep_Tool::Parameter(vfe2,e2,Fsp);
  gp_Pnt2d  p1; PC1->D0(par1,p1);
  gp_Pnt2d  p2; PC2->D0(par2,p2);

  double cv1 = (isuiso) ? pv1.Y() : pv1.X();
  double cv2 = (isuiso) ? pv2.Y() : pv2.X();
  double c1 = (isuiso) ? p1.Y() : p1.X();
  double c2 = (isuiso) ? p2.Y() : p2.X();

  double d1 = (c1 - cv1);
  double d2 = (c2 - cv2);
  double adc = std::abs(c1 - c2);
  double adcv = std::abs(cv1 - cv2);

  double tol2d = 1.e-6;
  bool mmd = (std::abs(adc-adcv) < tol2d);
  if (mmd) { // correction de CTS20973
    gp_Vec2d transl(0.,1.); if (isviso) transl = gp_Vec2d(1.,0.);
    transl.Multiply(d1); // ou d2
    ok = FUN_tool_translate(transl,Fsp,Ein);
  }
  else {
    // redefinition des parametres de v1,v2 de Ein tels que des parametres de
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceCORRISO()) {
      std::cout<<"FUN_correctDegeneratedE : !mmd NYI"<<std::endl;
    }
#endif
    ok = false;
  }

  return ok;
} //FUN_correctDegeneratedE*/

/*static bool FUN_tool_reguUV(const TopoDS_Face& FF, TopoDS_Face& fF)
{

  const TopoDS_Face& F = TopoDS::Face(FF);
  bool uclosed,vclosed; double uperiod,vperiod;
  bool closed = FUN_tool_closedS(F,uclosed,uperiod,vclosed,vperiod);
  if (!closed) return false;
  double tolu,tolv; FUN_tool_tolUV(TopoDS::Face(fF),tolu,tolv);

  NCollection_List<TopoDS_Shape> leko;
  // --------
  NCollection_IndexedMap<TopoDS_Shape> mape;
  TopExp_Explorer ex(fF, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& ee = TopoDS::Edge(ex.Current());
    TopAbs_Orientation oee = ee.Orientation();
    TopAbs_Orientation oe = TopAbs::Complement(oee);
    TopoDS_Edge e = TopoDS::Edge(ee.Oriented(oe));
    bool hasoppo = mape.Contains(e); // e with complement ori was added to mape
    if (hasoppo) {
      // ee :
      double ff,ll; occ::handle<Geom2d_Curve> PCee = BRep_Tool::CurveOnSurface(ee,F,ff,ll);
      bool uisoee,visoee; gp_Dir2d d2dee; gp_Pnt2d O2dee;
      bool uvisoee = FUN_tool_IsUViso(PCee,uisoee,visoee,d2dee,O2dee);
      // e :
      double f,l; occ::handle<Geom2d_Curve> PCe = BRep_Tool::CurveOnSurface(e,F,f,l);
      bool uisoe,visoe; gp_Dir2d d2de; gp_Pnt2d O2de;
      bool uvisoe = FUN_tool_IsUViso(PCe,uisoe,visoe,d2de,O2de);

      // isfaulty :
      bool isfaulty = false;
      double dd = O2dee.Distance(O2de);
      if (uisoee && uisoe) isfaulty = (dd < tolu);
      if (visoee && visoe) isfaulty = (dd < tolv);

      if (isfaulty) leko.Append(ee);
    }
    else mape.Add(ee);
  }

  int nko = leko.Extent();
  if (nko != 1) return false;

  // eko = edge with faulty pcurve :
  const TopoDS_Shape& eko = leko.First();
  TopAbs_Orientation oeko = eko.Orientation();
  NCollection_List<TopoDS_Shape> edges; ex.Init(fF, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Shape& ee = ex.Current();
    if (!ee.IsSame(eko)) edges.Append(ee);
  }
  // fe = edge with vfe = vertex(ivfe) not uv-connexed :
  TopoDS_Shape fe; int ivfe=0;
  bool det = ::FUN_DetectEdgewithfaultyUV(FF,fF,edges,false,fe,ivfe);
  if (!det) return false;

  NCollection_Array1<TopoDS_Shape> Vfe(1,2); FUN_Vertices(TopoDS::Edge(fe),Vfe);
  TopAbs_Orientation ofe = fe.Orientation();
  const TopoDS_Vertex& vfe = TopoDS::Vertex(Vfe(ivfe));
  double parfe = BRep_Tool::Parameter(vfe,TopoDS::Edge(fe));
  gp_Pnt2d uvfe; bool ok = FUN_tool_paronEF(TopoDS::Edge(fe),parfe,F,uvfe);
  if (!ok) return false;
  // ivconnex :
  int ivconnex = (ivfe == 1) ? 2 : 1;
//  if (ofe == TopAbs_REVERSED) ivconnex = (ivconnex == 1) ? 2 : 1;

  // vertex(ivconnex) of eko FORWARD
  TopoDS_Edge ekoFOR = TopoDS::Edge(eko.Oriented(TopAbs_FORWARD));
  NCollection_Array1<TopoDS_Shape> Veko(1,2); FUN_Vertices(TopoDS::Edge(ekoFOR),Veko);
  const TopoDS_Vertex& veko1 = TopoDS::Vertex(Veko(1));
  const TopoDS_Vertex& veko2 = TopoDS::Vertex(Veko(2));
  int iveko = 0;
  if (veko1.IsSame(vfe)) iveko = 1;
  if (veko2.IsSame(vfe)) iveko = 2;
  if (iveko == 0) return false;

  // ett : edge same eko with pcurve to translate
  // isekoFOR=true : vfe should be connexed to vertex(ivconnex) of ekoFOR
  bool isekoFOR = (iveko == ivconnex);
  TopAbs_Orientation oett = isekoFOR ? TopAbs_FORWARD : TopAbs_REVERSED;
  TopoDS_Edge ett = TopoDS::Edge(eko.Oriented(oett));
  const TopoDS_Vertex& vtt = TopoDS::Vertex(Veko(iveko));

  double parett = BRep_Tool::Parameter(vtt,ett);
  gp_Pnt2d uvtt; ok = FUN_tool_paronEF(ett,parett,F,uvtt);
  if (!ok) return false;

  double du = uvfe.X()-uvtt.X();
  double dv = uvfe.Y()-uvtt.Y();
  bool tru=false, trv=false;
  if (uclosed) tru = (std::abs(std::abs(du)-uperiod) < tolu);
  if (vclosed) trv = (std::abs(std::abs(dv)-vperiod) < tolv);
  if (!tru && !trv) return false;

  gp_Vec2d tt;
  if (tru) tt = gp_Vec2d(du,0.);
  if (trv) tt = gp_Vec2d(0.,dv);
  double f,l; occ::handle<Geom2d_Curve> PC1 = BRep_Tool::CurveOnSurface(ett,F,f,l);
  bool uisoett,visoett; gp_Dir2d d2dett; gp_Pnt2d o2dett;
  bool uvisoett =
FUN_tool_IsUViso(PC1,uisoett,visoett,d2dett,o2dett);o2dett.Translate(tt); occ::handle<Geom2d_Line>
L2d = new Geom2d_Line(o2dett,d2dett); occ::handle<Geom2d_TrimmedCurve> PC2 = new
Geom2d_TrimmedCurve(L2d,f,l);

  BRep_Builder BB;
  double toltt = BRep_Tool::Tolerance(ett);
//  BB.UpdateEdge(TopoDS::Edge(ett),PC2,PC1,fF,toltt);
  // xpu220998 : cto cylcong A1
  occ::handle<Geom2d_Curve> nullc2d;
  BB.UpdateEdge(TopoDS::Edge(ekoFOR),nullc2d,nullc2d,fF,toltt); // clear
  if (isekoFOR) BB.UpdateEdge(TopoDS::Edge(ekoFOR),PC2,PC1,fF,toltt);
  else          BB.UpdateEdge(TopoDS::Edge(ekoFOR),PC1,PC2,fF,toltt);

  return true;
}*/

static bool FUN_connexX(const bool                              onU,
                        TopOpeBRepTool_CORRISO&                 CORRISO,
                        const NCollection_List<TopoDS_Shape>&   EdstoCheck,
                        NCollection_DataMap<TopoDS_Shape, int>& fyEds)
// purpose : Fref is x-periodic,
//           <fyEds>={(fye,recadre)}, recadre = INCREASE,DECREASE
//                                    fye has its 2 bounds faulty
{
  fyEds.Clear();
  double tolF  = BRep_Tool::Tolerance(CORRISO.Fref());
  int    Index = onU ? 1 : 2;
  double xperiod;
  bool   xclosed = CORRISO.Refclosed(Index, xperiod);
  if (!xclosed)
    return false;
  double xtol = CORRISO.Tol(Index, tolF);

  // fy has its 2 uvbounds non-connexed
  // nyixpu300998 : iterative (while ko) + map of "static" edges
  TopoDS_Shape fy;
  int          Ify   = 0;
  bool         hasfy = CORRISO.EdgeWithFaultyUV(EdstoCheck, 2, fy, Ify);
  if (!hasfy)
    return false;
  TopOpeBRepTool_C2DF C2DF;
  bool                isb = CORRISO.UVRep(TopoDS::Edge(fy), C2DF);
  if (!isb)
    return false; // NYIRAISE

  NCollection_Array1<TopoDS_Shape> vfy(1, 2);
  TopOpeBRepTool_TOOL::Vertices(TopoDS::Edge(fy), vfy);
  for (int ii = 1; ii <= 2; ii++)
  {
    // vff = vertex[ii] of fy
    const TopoDS_Vertex& vff    = TopoDS::Vertex(vfy(ii));
    double               parvff = TopOpeBRepTool_TOOL::ParE(ii, TopoDS::Edge(fy));
    gp_Pnt2d             uvff   = TopOpeBRepTool_TOOL::UVF(parvff, C2DF);
    // loe list of edges connexed to faultE
    NCollection_List<TopoDS_Shape> loe;
    isb = CORRISO.Connexity(vff, loe);
    if (!isb)
      return false; // FUNRAISE

    NCollection_List<TopoDS_Shape>::Iterator ite(loe); // iteration on connex edges of vff
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Edge&               ee = TopoDS::Edge(ite.Value());
      NCollection_Array1<TopoDS_Shape> vee(1, 2);
      TopOpeBRepTool_TOOL::Vertices(ee, vee);
      for (int ive = 1; ive <= 2; ive++)
      {
        // ve = vertex[ive] of ee
        const TopoDS_Vertex& ve    = TopoDS::Vertex(vee(ive));
        bool                 samev = ve.IsSame(vff);
        if (!samev)
          continue;
        if (ive == ii)
          continue;
        TopOpeBRepTool_C2DF C2DFe;
        isb = CORRISO.UVRep(ee, C2DFe);
        if (!isb)
          return false; // FUNRAISE
        double   paree = TopOpeBRepTool_TOOL::ParE(ive, ee);
        gp_Pnt2d uve   = TopOpeBRepTool_TOOL::UVF(paree, C2DFe);

        // xxtrsl :
        double dxx   = onU ? uve.X() - uvff.X() : uve.Y() - uvff.Y();
        bool   isper = (std::abs(xperiod - std::abs(dxx)) < xtol);
        if (!isper)
          continue;

        int recadre = (dxx > 0) ? INCREASE : DECREASE;
        fyEds.Bind(fy, recadre);
      } // ive=1..2
    } // ite(loe)
  } // ii=1..2
  return !fyEds.IsEmpty();
} // FUN_connexX

//=================================================================================================

bool TopOpeBRepTool::CorrectONUVISO(const TopoDS_Face& Fin, TopoDS_Face& Fsp)
// <Fref> is x-periodic
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceCORRISO();
  if (trc)
    std::cout << "\n####    CorrectONUVISO    ####\n\n";
  debcorrUV();
#endif

  double tolF = BRep_Tool::Tolerance(Fin);

  TopOpeBRepTool_CORRISO CORRISO(Fin);
  double                 uperiod, vperiod;
  bool                   uclosed = CORRISO.Refclosed(1, uperiod);
  bool                   vclosed = CORRISO.Refclosed(2, vperiod);
  if (!uclosed && !vclosed)
    return false;

  CORRISO.Init(Fsp);
  bool ok = CORRISO.UVClosed();
  if (ok)
    return true; // Fsp is valid

  // 1. We check connexity among all edges of <Fsp>
  // if we find on edge with 2 faulty UVbounds, we try to UVconnect it.
  //  for (int i=1; i<=2; i++) {
  int i;
  for (i = 1; i <= 2; i++)
  {
    bool                                   onU     = i == 1;
    const NCollection_List<TopoDS_Shape>&  Tocheck = CORRISO.Eds();
    NCollection_DataMap<TopoDS_Shape, int> fyEds;
    ok = ::FUN_connexX(onU, CORRISO, Tocheck, fyEds);
    if (!ok)
      continue;
    ok = CORRISO.TrslUV(onU, fyEds);
    if (!ok)
      continue;
    ok = CORRISO.UVClosed();
    if (!ok)
      continue;
    ok = CORRISO.GetnewS(Fsp);
    if (!ok)
      return false; // NYIRAISE
    return true;
  }

  // 2. x-2drep(edges) are in [xfirst,xfirst+xperiod]
  for (i = 1; i <= 2; i++)
  {
    bool   onU     = (i == 1);
    double xper    = 0.;
    bool   xclosed = CORRISO.Refclosed(i, xper);
    if (!xclosed)
      continue;
    double tolx = CORRISO.Tol(i, tolF);
    tolx *= 1.e2; // BUC60380
    NCollection_DataMap<TopoDS_Shape, int> FyEds;
    bool hasfy = CORRISO.EdgesOUTofBoundsUV(CORRISO.Eds(), onU, tolx, FyEds);
    if (!hasfy)
      continue;
    ok = CORRISO.TrslUV(onU, FyEds);
    if (!ok)
      return false;
    ok = CORRISO.UVClosed();
    if (!ok)
      continue;
    ok = CORRISO.GetnewS(Fsp);
    if (!ok)
      return false; // NYIRAISE
    return true;
  }
  return false;

  /*// xpu310898 : eC closing ff, ff sdm F(reference face), proj(eC,F) gives ee with
  //             !closing(ee,Fsp) -> 2drep(Fsp) is not closed.
  //             purpose : translate pceCFOR or pceCREV
  //   cto902B7 (ff=f7,eC=e9,F=f14)
  ok = ::FUN_tool_reguUV(Fin,Fsp);*/

  /*// JYL 980909 : reecriture complete
  // 1/ traitement de TOUTES les aretes
  //    isou et isov fautives (et non la premiere trouvee);
  // 2/ traitement des aretes degenerees fautives : CTS20973

  NCollection_List<TopoDS_Shape> lisoe,ldege;
  TopExp_Explorer exe(Fsp, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(exe.Current());
    bool degen = BRep_Tool::Degenerated(E);
    if ( degen ) {
      ldege.Append(E);
    }
    else {
      double f,l; occ::handle<Geom2d_Curve> PC = BRep_Tool::CurveOnSurface(E,Fin,f,l);
      bool uiso,viso; gp_Dir2d d2d; gp_Pnt2d O2d;
      bool uviso = FUN_tool_IsUViso(PC,uiso,viso,d2d,O2d);
      bool onclosing = (uiso && uclosed) || (viso && vclosed);
      if      ( onclosing) {
    lisoe.Append(E);
      }
    }
  }

  int nisoe = lisoe.Extent();
  int ndege = ldege.Extent();
  if (nisoe ==0 && ndege == 0) return false;

  int tmpisoe;
  int tmpdege;

  NCollection_List<TopoDS_Shape> lfyisoe; bool tocorrectisoe = false;
  tocorrectisoe = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,lisoe,true,lfyisoe,tmpisoe);

  NCollection_List<TopoDS_Shape> lfydege; bool tocorrectdege = false;
  tocorrectdege = FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,ldege,true,lfydege,tmpdege);

  tocorrect = (tocorrectisoe || tocorrectdege);
  if (!tocorrect) {
    return true;
  }

#ifdef OCCT_DEBUG
  if (trc) {
    std::cout<<"CorrectONUVISO ";
    std::cout<<"iso faulty "<<tocorrectisoe<<" deg faulty "<<tocorrectdege<<std::endl;
  }
  debcorrUV();
#endif

  if (tocorrectisoe) {
    for (NCollection_List<TopoDS_Shape>::Iterator itiso(lfyisoe);itiso.More();itiso.Next()) {
      TopoDS_Edge& fyisoe = TopoDS::Edge(itiso.Value());

      // !! if the faulty edge ON closing appears twice in <Eds>, NOTHING is done!
      // -> see processing ::PurgeClosingEdges later called in WESMakeFaces
      int nfoundisoe = 0;
      for (exe.Init(Fsp, TopAbs_EDGE); exe.More(); exe.Next()) {
    if (exe.Current().IsSame(fyisoe)) {
      nfoundisoe++;
    }
      }
      if (nfoundisoe > 1) {
    continue;
      }


      bool ok = ::FUN_correctClosingE(fyisoe,Fsp);
      if (!ok) {
    continue;
      }
    }

    int tmpisoe;
    NCollection_List<TopoDS_Shape> lffyisoe; tocorrectisoe =
FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,lfyisoe,false,lffyisoe,tmpisoe); } // end lffyisoe
process

  if (tocorrectdege) {
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher> mve; TopExp::MapShapesAndAncestors(Fsp,TopAbs_VERTEX,TopAbs_EDGE,mve);

    for (NCollection_List<TopoDS_Shape>::Iterator itdeg(lfydege);itdeg.More();itdeg.Next()) {
      TopoDS_Edge& fydege = TopoDS::Edge(itdeg.Value());


      bool ok = ::FUN_correctDegeneratedE(mve,fydege,Fsp);
      if (!ok) {
    continue;
      }
    } // itdeg

    NCollection_List<TopoDS_Shape> lffydege; tocorrectdege =
FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,lfydege,false,lffydege,tmpdege); } // end lfydege
process

  NCollection_List<TopoDS_Shape> eFsp; FUN_tool_shapes(Fsp,TopAbs_EDGE,eFsp);
  NCollection_List<TopoDS_Shape> lffydege; tocorrect =
FUN_DetectEdgeswithfaultyUV(Fsp,Fsp,eFsp,false,lffydege,tmpdege); bool done =
!tocorrect; return done;*/

} // correctONUVISO

//=================================================================================================

bool TopOpeBRepTool::MakeFaces(const TopoDS_Face&                          Fin,
                               const NCollection_List<TopoDS_Shape>&       LOF,
                               const NCollection_IndexedMap<TopoDS_Shape>& MshNOK,
                               NCollection_List<TopoDS_Shape>&             LOFF)
{
  //  TopOpeBRepDS_BuildTool BT;
  BRep_Builder BB;
  LOFF.Clear();
  NCollection_List<TopoDS_Shape>::Iterator it(LOF);
  for (; it.More(); it.Next())
  {
    const TopoDS_Face& FF    = TopoDS::Face(it.Value());
    bool               valid = !MshNOK.Contains(FF);
    if (valid)
    {
      LOFF.Append(FF);
      continue;
    }

    TopoDS_Shape aLocalShape = Fin.EmptyCopied();
    TopoDS_Face  newFace     = TopoDS::Face(aLocalShape); // BT.CopyFace(Fin,newFace);
    //    TopoDS_Face newFace = TopoDS::Face(Fin.EmptyCopied());// BT.CopyFace(Fin,newFace);
    TopExp_Explorer exw(FF, TopAbs_WIRE);

    for (; exw.More(); exw.Next())
    {
      const TopoDS_Wire& W = TopoDS::Wire(exw.Current());
      valid                = !MshNOK.Contains(W);
      //      if (valid) {BT.AddFaceWire(newFace,W); continue;}
      if (valid)
      {
        BB.Add(newFace, W);
        continue;
      }

      TopoDS_Wire newWire; // BT.MakeWire(newWire);
      BB.MakeWire(newWire);
      TopExp_Explorer exe(W, TopAbs_EDGE);
      int             ne = 0;
      for (; exe.More(); exe.Next())
      {
        const TopoDS_Edge& E = TopoDS::Edge(exe.Current());
        valid                = !MshNOK.Contains(E);
        if (!valid)
          continue;
        //	BT.AddWireEdge(newWire,E);
        BB.Add(newWire, E);
        ne++;
      } // exe
      if (ne == 0)
        continue;
      bool closed = FUN_tool_ClosedW(newWire);
      //      BT.Closed(newWire,closed);
      //      BT.AddFaceWire(newFace,newWire);
      newWire.Closed(closed);
      BB.Add(newFace, newWire);
    } // exw
    LOFF.Append(newFace);
  }
  return true;
}

/*static occ::handle<Geom2d_TrimmedCurve> FUN_translate(const gp_Vec2d& tvector, const TopoDS_Face&
fF, TopoDS_Edge& fyE)
{
  double f,l,tol; occ::handle<Geom2d_Curve> PC = FUNTOOLC2D_CurveOnSurface(fyE,fF,f,l,tol);
  bool isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d;
  bool isouv = FUN_tool_IsUViso(PC,isou,isov,d2d,o2d); o2d.Translate(tvector);
  occ::handle<Geom2d_Line> L2d = new Geom2d_Line(o2d,d2d);
  occ::handle<Geom2d_TrimmedCurve> C2d = new Geom2d_TrimmedCurve(L2d,f,l);
  return C2d;
}
Standard_EXPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF,
TopoDS_Edge& fyE)
{
  occ::handle<Geom2d_TrimmedCurve> C2d = ::FUN_translate(tvector,fF,fyE);
  double tole = BRep_Tool::Tolerance(fyE);
  BRep_Builder BB;
//  occ::handle<Geom2d_Curve> toclear; BB.UpdateEdge(fyE,toclear,fF,tole);
  BB.UpdateEdge(fyE,C2d,fF,tole);
}

static bool FUN_tool_translate(const gp_Vec2d& tvector, TopoDS_Face& fF, TopoDS_Edge&
fyE)
     // prequesitory : <fF> is on periodic surface, translates edge
     //  <fyE>'s uorviso to have its uorvpar bounded in [0.,2PI].
{
  occ::handle<Geom2d_TrimmedCurve> C2d = ::FUN_translate(tvector,fF,fyE);
  double tole = BRep_Tool::Tolerance(fyE);

  //xpu040598 : CTS20280 f37 modified when its split is modified!
  TopoDS_Face newf; bool ok = FUN_tool_pcurveonF(fF,fyE,C2d,newf);
  if (ok) fF = newf;
  return ok;
  //xpu040598
}
*/
