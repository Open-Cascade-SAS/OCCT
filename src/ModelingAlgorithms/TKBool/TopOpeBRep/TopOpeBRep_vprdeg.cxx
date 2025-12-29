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

#include <TopoDS.hxx>
// #include <BRepAdaptor_Curve2d.hxx>
#include <gp_Vec.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>

#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_mkTondgE.hxx>

#define M_ON(st) (st == TopAbs_ON)
#define M_REVERSED(st) (st == TopAbs_REVERSED)

// modified by NIZHNY-MKK  Tue Nov 21 17:30:23 2000.BEGIN
static NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                             aMapOfTreatedVertexListOfEdge;
static TopOpeBRep_PLineInter localCurrentLine = nullptr;

static bool local_FindTreatedEdgeOnVertex(const TopoDS_Edge&   theEdge,
                                          const TopoDS_Vertex& theVertex);

static void local_ReduceMapOfTreatedVertices(const TopOpeBRep_PLineInter& theCurrentLine);

static bool local_FindVertex(
  const TopOpeBRep_VPointInter&                              theVP,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& theMapOfVertexEdges,
  TopoDS_Vertex&                                             theVertex);
// modified by NIZHNY-MKK  Tue Nov 21 17:30:27 2000.END

#ifdef OCCT_DEBUG
Standard_EXPORT bool FUN_debnull(const TopoDS_Shape& s);
#endif

Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T, // transition
  const int                      S, // curve/edge index
  const int                      G, // point/vertex index
  const double                   P, // parameter of G on S
  const TopOpeBRepDS_Kind        GK,
  const bool                     B); // G is a vertex (or not) of the interference master

Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T,  // transition
  const int                      S,  // curve/edge index
  const int                      G,  // point/vertex index
  const double                   P,  // parameter of G on S
  const TopOpeBRepDS_Kind        GK, // POINT/VERTEX
  const TopOpeBRepDS_Kind        SK,
  const bool                     B); // G is a vertex (or not) of the interference master

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

Standard_EXPORT void FUN_FillVof12(const TopOpeBRep_LineInter& L, TopOpeBRepDS_PDataStructure pDS)
{
  TopOpeBRep_VPointInterIterator itvp(L);
  for (; itvp.More(); itvp.Next())
  {
    const TopOpeBRep_VPointInter& vp   = itvp.CurrentVP();
    int                           sind = vp.ShapeIndex();
    if (sind != 3)
      continue;
    bool isvon1  = vp.IsVertexOnS1();
    bool isvon2  = vp.IsVertexOnS2();
    bool isvon12 = isvon1 && isvon2;
    if (!isvon12)
      continue;
    const TopoDS_Shape& v1 = vp.VertexOnS1();
    const TopoDS_Shape& v2 = vp.VertexOnS2();
    pDS->FillShapesSameDomain(v1, v2);
  }
}

static void FUN_addmapve(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& mapve,
  const TopoDS_Shape&                                                                         v,
  const TopoDS_Shape&                                                                         e)
{
  bool visb = mapve.IsBound(v);
  bool eisb = mapve.IsBound(e);
  if (!visb && !eisb)
  {
    NCollection_List<TopoDS_Shape> le;
    le.Append(e);
    mapve.Bind(v, le);
    NCollection_List<TopoDS_Shape> lv;
    lv.Append(v);
    mapve.Bind(e, lv);
  }
  else if (visb && !eisb)
  {
    mapve.ChangeFind(v).Append(e);
    NCollection_List<TopoDS_Shape> lv;
    lv.Append(v);
    mapve.Bind(e, lv);
  }
  else if (!visb && eisb)
  {
    mapve.ChangeFind(e).Append(v);
    NCollection_List<TopoDS_Shape> le;
    le.Append(e);
    mapve.Bind(v, le);
  }
  else
  {
    bool                                     found = false;
    NCollection_List<TopoDS_Shape>::Iterator it(mapve.Find(v));
    for (; it.More(); it.Next())
      if (it.Value().IsSame(e))
      {
        found = true;
        break;
      }
    if (!found)
    {
      mapve.ChangeFind(v).Append(e);
      mapve.ChangeFind(e).Append(v);
    }
  }
}

Standard_EXPORT void FUN_GetdgData(
  TopOpeBRepDS_PDataStructure& pDS,
  const TopOpeBRep_LineInter&  L,
  const TopoDS_Face&           F1,
  const TopoDS_Face&           F2,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    datamap)
{
  // purpose : fills up map datamap = {(v, (closinge,degeneratede))}
  //           with shapes with same rank

  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>
    shaperk; // rkshape = {shape,rank=1,2}
             // clang-format off
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mapvec, mapved; // mapvec = {(v,lec),(ec,lv)}, mapved = {(v,led),(ed,lv)}
             // clang-format on
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
    mapvvsd; // mapvvsd = {(v,v)}

  TopOpeBRep_VPointInterIterator itvp(L);
  for (; itvp.More(); itvp.Next())
  {
    const TopOpeBRep_VPointInter& vp   = itvp.CurrentVP();
    bool                          isv1 = vp.IsVertex(1), isv2 = vp.IsVertex(2);
    bool                          isv = isv1 || isv2;
    if (!isv)
      continue;

    int          sind = vp.ShapeIndex();
    TopoDS_Shape v    = isv1 ? vp.Vertex(1) : vp.Vertex(2);
    for (int i = 1; i <= 2; i++)
    {
      TopoDS_Face f = (i == 1) ? F1 : F2;

      bool isvi = vp.IsVertex(i);
      if (isvi)
      {
        v = vp.Vertex(i);
        shaperk.Bind(v, i);
      }

      TopoDS_Edge e;
      bool        isdg, iscl;
      isdg = iscl = false;
      bool ison   = (sind == i) || (sind == 3);
      if (ison)
      {
        e = TopoDS::Edge(vp.Edge(i));
        shaperk.Bind(e, i);

        isdg = BRep_Tool::Degenerated(e);
        if (!isdg)
          iscl = TopOpeBRepTool_ShapeTool::Closed(e, f);
        if (isdg)
          FUN_addmapve(mapved, v, e);
        if (iscl)
          FUN_addmapve(mapvec, v, e);
      } // ison
    } // i = 1..2
  } // itvp

  // filling up map mapvvsd
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itm(mapved);
  for (; itm.More(); itm.Next())
  {
    const TopoDS_Shape& v = itm.Key();
    if (v.ShapeType() != TopAbs_VERTEX)
      continue;
    int rkv = shaperk.Find(v);

    NCollection_List<TopoDS_Shape>::Iterator ite(itm.Value());
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Edge& e   = TopoDS::Edge(ite.Value());
      int                rke = shaperk.Find(e);
      if (rke != rkv)
      {
        TopExp_Explorer     ex(e, TopAbs_VERTEX);
        const TopoDS_Shape& vsd = ex.Current();
        // recall : if vsd is not bound in shaperk,
        //          it is not bound in <L> either
        mapvvsd.Bind(v, vsd);
        mapvvsd.Bind(vsd, v);
      }
    }
  } // itm(mapved)

  itm.Initialize(mapved);
  for (; itm.More(); itm.Next())
  {
    const TopoDS_Shape& dge = itm.Key();
    int                 rk  = shaperk.Find(dge);
    TopoDS_Face         f   = (rk == 1) ? F1 : F2;
    if (dge.ShapeType() != TopAbs_EDGE)
      continue;

    TopExp_Explorer      ex(dge, TopAbs_VERTEX);
    const TopoDS_Vertex& v     = TopoDS::Vertex(ex.Current());
    int                  rkv   = shaperk.Find(v);
    bool                 hassd = mapvvsd.IsBound(v);
    TopoDS_Vertex        vsd;
    if (hassd)
      vsd = TopoDS::Vertex(mapvvsd.Find(v));

    bool         hasecl = false;
    TopoDS_Shape cle;
    bool         isbv = mapvec.IsBound(v), isbvsd = hassd ? mapvec.IsBound(vsd) : false;
    if (!isbv && !isbvsd)
    {
      // **************************************************
      // interference with closing edge is not found,
      // adding new information to the ds
      // **************************************************
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        mapve;
      TopExp::MapShapesAndAncestors(f, TopAbs_VERTEX, TopAbs_EDGE, mapve);
      NCollection_List<TopoDS_Shape>::Iterator iteds(mapve.FindFromKey(v));
      for (; iteds.More(); iteds.Next())
      {
        const TopoDS_Edge& ee = TopoDS::Edge(iteds.Value());
        if (ee.IsSame(dge))
          continue;
        bool iscl = TopOpeBRepTool_ShapeTool::Closed(ee, f);
        if (!iscl)
          continue;
        isbv   = true;
        cle    = ee;
        hasecl = true;
        break;
      }
    }
    if (!hasecl && (isbv || isbvsd))
    {
      TopoDS_Vertex                            vv = isbv ? v : vsd;
      NCollection_List<TopoDS_Shape>::Iterator ite;
      if (isbv)
        ite.Initialize(mapvec.Find(v));
      for (; ite.More(); ite.Next())
      {
        const TopoDS_Shape& e   = ite.Value();
        int                 rke = shaperk.Find(e);
        if (rke == rk)
        {
          cle    = e;
          hasecl = true;
          break;
        }
      }
    }
    if (!hasecl)
      continue;

    TopoDS_Vertex                  vv = (rkv == rk) ? v : vsd;
    NCollection_List<TopoDS_Shape> ls;
    ls.Append(cle);
    ls.Append(dge);
    datamap.Bind(vv, ls);
  } // itm(mapved)

  // filling sdm shapes
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator ittm(mapvvsd);
  for (; ittm.More(); ittm.Next())
  {
    const TopoDS_Vertex& v   = TopoDS::Vertex(ittm.Value());
    const TopoDS_Vertex& ov  = TopoDS::Vertex(mapvvsd.Find(v));
    int                  rkv = shaperk.Find(v);
    TopoDS_Vertex        v1  = (rkv == 1) ? v : ov;
    TopoDS_Vertex        v2  = (rkv == 2) ? v : ov;
    pDS->FillShapesSameDomain(v1, v2);
  }
} // FUN_GetdgData

#define NOI (0)
#define MKI1 (1)
#define MKI2 (2)
#define MKI12 (3)

static int FUN_putInterfonDegenEd(
  const TopOpeBRep_VPointInter& VP,
  const TopoDS_Face&            F1,
  const TopoDS_Face&            F2,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                            DataforDegenEd, // const but for copy &
  occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
  int&                                      is,
  TopoDS_Edge&                              dgE,
  // int& makeinterf, // 1,2,3 : compute interf1, or2 or the 2 interfs
  int&, // 1,2,3 : compute interf1, or2 or the 2 interfs
  TopOpeBRepDS_Transition& Trans1,
  double&                  param1,
  TopOpeBRepDS_Transition& Trans2,
  double&                  param2,
  TopoDS_Edge&             OOEi,
  double&                  paronOOEi,
  bool&                    hasOOEi,
  bool&                    isT2d)
{
  OOEi.Nullify();

  bool on3   = (VP.ShapeIndex() == 3); // <VP> is shared by edge of 1 and edge of 2.
  bool onv12 = VP.IsVertexOnS1() && VP.IsVertexOnS2();

  const TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  TopoDS_Vertex                     v;
  int                               rkv = 0;
  //  int iv;
  TopoDS_Vertex ov;
  for (int ShapeIndex = 1; ShapeIndex <= 2; ShapeIndex++)
  {
    bool isv = (ShapeIndex == 1) ? (VP.IsVertexOnS1()) : (VP.IsVertexOnS2());
    if (!isv)
      continue;
    v = (ShapeIndex == 1) ? TopoDS::Vertex(VP.VertexOnS1()) : TopoDS::Vertex(VP.VertexOnS2());
    bool hasdegened = DataforDegenEd.IsBound(v);
    if (!hasdegened)
      continue;
    rkv = ShapeIndex;
    break;
  } // ShapeIndex = 1..2
  if (rkv == 0)
    return NOI; // compute interference once only.
  bool isvsd = HDS->HasSameDomain(v);

  // edges dge, cle on shape<rkdg>
  const NCollection_List<TopoDS_Shape>& loe = DataforDegenEd.Find(v);
  const TopoDS_Edge&                    cle = TopoDS::Edge(loe.First());
  const TopoDS_Edge&                    dge = TopoDS::Edge(loe.Last());
  dgE                                       = dge;
  int rkdg                                  = 0;
  if (BDS.HasShape(dge))
    rkdg = BDS.AncestorRank(dge);
  else
  {
    bool vindge = FUN_tool_inS(v, dge);
    if (vindge)
      rkdg = rkv;
    else
      rkdg = (rkv == 1) ? 2 : 1;
  }
  is      = rkdg;
  int rki = (rkdg == 1) ? 2 : 1;

  gp_Pnt2d    uvi;
  TopoDS_Face fi, f;
  {
    //     double u,v;
    //     if (rki == 1) VP.ParametersOnS1(u,v);
    //     else          VP.ParametersOnS2(u,v);
    //     uvi = gp_Pnt2d(u,v);
    // modified by NIZHNY-MKK  Tue Nov 21 17:44:56 2000.BEGIN
    double upar, vpar;
    if (rki == 1)
      VP.ParametersOnS1(upar, vpar);
    else
      VP.ParametersOnS2(upar, vpar);
    uvi = gp_Pnt2d(upar, vpar);
    // modified by NIZHNY-MKK  Tue Nov 21 17:44:59 2000.END
    fi = (rki == 1) ? F1 : F2;
    f  = (rkdg == 1) ? F1 : F2;
  }
  TopOpeBRepTool_mkTondgE mktdg;
  bool                    ok = mktdg.Initialize(dge, f, uvi, fi);
  if (!ok)
    return NOI;
  ok = mktdg.SetclE(cle);
  if (!ok)
    return NOI;

  if (onv12 || isvsd)
  {
    if (onv12)
      ov = (rkv == 2) ? TopoDS::Vertex(VP.VertexOnS1()) : TopoDS::Vertex(VP.VertexOnS2());
    else
    {
      // modified by NIZHNY-MKK  Tue Nov 21 17:45:46 2000.BEGIN
      //       bool ok = FUN_ds_getoov(v,HDS,ov);
      //       if (!ok) return false;
      bool found = FUN_ds_getoov(v, HDS, ov);
      if (!found)
        return NOI;
      // modified by NIZHNY-MKK  Tue Nov 21 17:45:50 2000.END
    }
    // clang-format off
    if (rkv != rkdg) {TopoDS_Vertex tmp = v; v = ov; ov = tmp; rkv = rkdg;} // ensure v is vertex of dge
    // clang-format on
  }

  int    mkt  = 0;
  double par1 = 0., par2 = 0.;
  if (on3)
  {
    TopoDS_Edge ei   = (rki == 1) ? TopoDS::Edge(VP.ArcOnS1()) : TopoDS::Edge(VP.ArcOnS2());
    double      pari = (rki == 1) ? VP.ParameterOnArc1() : VP.ParameterOnArc2();
    // if okrest, ei interfers in the compute of transitions for dge
    mktdg.SetRest(pari, ei);
    ok = mktdg.MkTonE(ei, mkt, par1, par2);
    if ((!ok) || (mkt == NOI))
      return NOI;
    OOEi      = ei;
    paronOOEi = pari;
    hasOOEi   = true;
  } // on3
  else
  {
    // modified by NIZHNY-MKK  Tue Nov 21 17:31:14 2000.BEGIN
    // This search, compute and check the data which was not computed by intersector.
    if ((rki == 1 && VP.IsOnDomS1()) || (rki == 2 && VP.IsOnDomS2()))
    {
      TopoDS_Edge ei   = (rki == 1) ? TopoDS::Edge(VP.ArcOnS1()) : TopoDS::Edge(VP.ArcOnS2());
      double      pari = (rki == 1) ? VP.ParameterOnArc1() : VP.ParameterOnArc2();
      mktdg.SetRest(pari, ei);
      ok = mktdg.MkTonE(ei, mkt, par1, par2);
      if (ok && mkt != NOI)
      {
        OOEi      = ei;
        paronOOEi = pari;
        hasOOEi   = true;
      }
    }
    else
    {
      bool        edgefound = false;
      TopoDS_Face aFace     = (rki == 1) ? F1 : F2;
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        aMapOfVertexEdges;
      TopExp::MapShapesAndAncestors(aFace, TopAbs_VERTEX, TopAbs_EDGE, aMapOfVertexEdges);
      TopoDS_Vertex aVertex;
      bool          vertexfound = local_FindVertex(VP, aMapOfVertexEdges, aVertex);

      if (vertexfound && !aVertex.IsNull())
      {
        NCollection_List<TopoDS_Shape>::Iterator anIt(aMapOfVertexEdges.FindFromKey(aVertex));
        for (; !edgefound && anIt.More(); anIt.Next())
        {
          const TopoDS_Edge& ei   = TopoDS::Edge(anIt.Value());
          double             pari = BRep_Tool::Parameter(aVertex, ei);
          if (!BRep_Tool::Degenerated(ei))
          {
            edgefound = !local_FindTreatedEdgeOnVertex(ei, aVertex);
          }
          if (edgefound)
          {
            mktdg.SetRest(pari, ei);
            ok = mktdg.MkTonE(ei, mkt, par1, par2);
            if (ok && mkt != NOI)
            {
              OOEi      = ei;
              paronOOEi = pari;
              hasOOEi   = true;
            }
            if (!aMapOfTreatedVertexListOfEdge.IsBound(aVertex))
            {
              NCollection_List<TopoDS_Shape> thelist;
              aMapOfTreatedVertexListOfEdge.Bind(aVertex, thelist);
            }
            aMapOfTreatedVertexListOfEdge(aVertex).Append(ei);
          }
        }
      }
      if (!edgefound)
      {
        ok = mktdg.MkTonE(mkt, par1, par2);
      }
    }
    // modified by NIZHNY-MKK  Tue Nov 21 17:31:36 2000.END
    if ((!ok) || (mkt == NOI))
      return NOI;
  }
  isT2d = mktdg.IsT2d();

  if ((mkt == MKI1) || (mkt == MKI12))
  {
    Trans1.Set(TopAbs_FORWARD);
    param1 = par1;
  }
  if ((mkt == MKI2) || (mkt == MKI12))
  {
    Trans2.Set(TopAbs_REVERSED);
    param2 = par2;
  }
  return mkt;
  //  **********   iterate on restrictions of fi  **********
  //  NCollection_List<TopoDS_Shape> lei; mktdg.GetAllRest(lei);
  //  NCollection_List<TopoDS_Shape>::Iterator ite(lei);
  //  for (; ite.More(); ite.Next()){
  //    bool oki = mktdg.MkTonE(ei,mkt,par1,par2);
  //    ... NYI
  //  }
} // FUN_putInterfonDegenEd

//=======================================================================
// function : ProcessVPondgE
// purpose  : SUPPLYING INTPATCH when a degenerated edge is touched.
//=======================================================================

#define s_NOIdgE (0) // do NOT compute any interference
#define s_IdgE (1)   // compute interference(s) on dgE
#define s_IOOEi (2)  // compute interference(s) on OOEi

bool TopOpeBRep_FacesFiller::ProcessVPondgE(const TopOpeBRep_VPointInter&           VP,
                                            const int                               ShapeIndex,
                                            TopOpeBRepDS_Kind&                      PVKind,
                                            int&                                    PVIndex, // out
                                            bool&                                   EPIfound,
                                            occ::handle<TopOpeBRepDS_Interference>& IEPI, // out
                                            bool&                                   CPIfound,
                                            occ::handle<TopOpeBRepDS_Interference>& ICPI) // out
{
  if (PVIndex == 0)
    FUN_VPIndex((*this),
                (*myLine),
                VP,
                ShapeIndex,
                myHDS,
                myDSCIL, // in
                PVKind,
                PVIndex, // out
                EPIfound,
                IEPI, // out
                CPIfound,
                ICPI, // out
                M_FINDVP);

  // kpart : sphere/box, with one sphere's degenerated edge lying on one boxe's
  // face, IN or ON the face
  // if (mIdgEorOOEi), adds interferences on degenerated edge

  // If interferences should be added, finds out <VP>'s geometry
  // in existing interferences (see out parameters <EPIfound>..);
  // adds a new point/vertex to the DS if necessary.

  bool                    hasOOEi = false;
  TopoDS_Edge             OOEi;
  double                  parOOEi;
  TopOpeBRepDS_Transition T1ondg, T2ondg;
  int                     rankdg = 0, Iiondg = 0;
  double                  par1ondg = 0., par2ondg = 0.;
  bool                    hasdgdata = !myDataforDegenEd.IsEmpty();
  if (!hasdgdata)
  {
    return false;
  }

  // modified by NIZHNY-MKK  Tue Nov 21 17:35:29 2000
  local_ReduceMapOfTreatedVertices(myLine);

  bool        isT2d = false;
  TopoDS_Edge dgEd;
  int         makeI = FUN_putInterfonDegenEd(VP,
                                     myF1,
                                     myF2,
                                     myDataforDegenEd,
                                     myHDS,
                                     rankdg,
                                     dgEd,
                                     Iiondg,
                                     T1ondg,
                                     par1ondg,
                                     T2ondg,
                                     par2ondg,
                                     OOEi,
                                     parOOEi,
                                     hasOOEi,
                                     isT2d);
  if (makeI == NOI)
  {
    return false;
  }

  // -------------------------------------------------------------------
  //             --- DS geometry Management ---
  // -------------------------------------------------------------------

  if (PVIndex == 0)
    FUN_VPIndex((*this),
                (*myLine),
                VP,
                ShapeIndex,
                myHDS,
                myDSCIL, // in
                PVKind,
                PVIndex, // out
                EPIfound,
                IEPI, // out
                CPIfound,
                ICPI, // out
                M_MKNEWVP);

  // -------------------------------------------------------------------
  //             --- EVI on degenerated edge ---
  //             ---       on OOEi           ---
  // -------------------------------------------------------------------

  int rankFi = (rankdg == 1) ? 2 : 1;
  //  TopoDS_Shape dgEd = VP.Edge(rankdg);
  TopoDS_Face Fi;
  if (rankFi == 1)
    Fi = myF1;
  else
    Fi = myF2;
  int iFi = myDS->AddShape(Fi, rankFi);
  myDS->AddShape(dgEd, rankdg);
  //  int iOOEi = 0;
  //  if (hasOOEi) iOOEi = myDS->AddShape(OOEi,rankFi);

  int rkv = myDS->AncestorRank(myDS->Shape(PVIndex));

  if ((makeI == MKI1) || (makeI == MKI12))
  {
    T1ondg.Index(iFi);
    bool isvertex1 = (rkv == 1);

    /*
        if (hasOOEi) {
          occ::handle<TopOpeBRepDS_Interference> EVI1i =
       ::MakeEPVInterference(T1ondg,iOOEi,PVIndex,par1ondg,
                          TopOpeBRepDS_VERTEX,TopOpeBRepDS_EDGE,isvertex1);
          myHDS->StoreInterference(EVI1i,dgEd);
        }
    */
    if (!isT2d)
    {
      occ::handle<TopOpeBRepDS_Interference> EVI1 = ::MakeEPVInterference(T1ondg,
                                                                          iFi,
                                                                          PVIndex,
                                                                          par1ondg,
                                                                          TopOpeBRepDS_VERTEX,
                                                                          TopOpeBRepDS_FACE,
                                                                          isvertex1);
      myHDS->StoreInterference(EVI1, dgEd);
    }
  }
  if ((makeI == MKI2) || (makeI == MKI12))
  {
    T2ondg.Index(iFi);
    bool isvertex2 = (rkv == 2);

    /*
        if (hasOOEi) {
          occ::handle<TopOpeBRepDS_Interference> EVI2i =
       ::MakeEPVInterference(T2ondg,iOOEi,PVIndex,par2ondg,
                          TopOpeBRepDS_VERTEX,TopOpeBRepDS_EDGE,isvertex2);
          myHDS->StoreInterference(EVI2i,dgEd);
        }
    */
    if (!isT2d)
    {
      occ::handle<TopOpeBRepDS_Interference> EVI2 = ::MakeEPVInterference(T2ondg,
                                                                          iFi,
                                                                          PVIndex,
                                                                          par2ondg,
                                                                          TopOpeBRepDS_VERTEX,
                                                                          TopOpeBRepDS_FACE,
                                                                          isvertex2);
      myHDS->StoreInterference(EVI2, dgEd);
    }
  }

  return true;
} // ProcessVPondgE

// modified by NIZHNY-MKK  Tue Nov 21 17:32:52 2000.BEGIN
static bool local_FindTreatedEdgeOnVertex(const TopoDS_Edge&   theEdge,
                                          const TopoDS_Vertex& theVertex)
{
  bool found = false;
  if (aMapOfTreatedVertexListOfEdge.IsBound(theVertex))
  {
    NCollection_List<TopoDS_Shape>::Iterator anIt(aMapOfTreatedVertexListOfEdge(theVertex));
    for (; !found && anIt.More(); anIt.Next())
    {
      if (theEdge.IsSame(anIt.Value()))
      {
        found = true;
      }
    }
  }
  return found;
}

static bool local_FindVertex(
  const TopOpeBRep_VPointInter&                              theVP,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& theMapOfVertexEdges,
  TopoDS_Vertex&                                             theVertex)
{
  gp_Pnt aVPPoint     = theVP.Value();
  double aVPTolerance = theVP.Tolerance();
  bool   vertexfound  = false;
  for (int itVertex = 1; !vertexfound && itVertex <= theMapOfVertexEdges.Extent(); itVertex++)
  {
    theVertex     = TopoDS::Vertex(theMapOfVertexEdges.FindKey(itVertex));
    gp_Pnt aPoint = BRep_Tool::Pnt(theVertex);
    if (aVPPoint.IsEqual(aPoint, aVPTolerance))
    {
      vertexfound = true;
    }
  }
  return vertexfound;
}

static void local_ReduceMapOfTreatedVertices(const TopOpeBRep_PLineInter& theCurrentLine)
{

  if (localCurrentLine == nullptr)
  {
    localCurrentLine = theCurrentLine;
    aMapOfTreatedVertexListOfEdge.Clear();
  }
  else
  {
    if (localCurrentLine != theCurrentLine)
    {
      localCurrentLine = theCurrentLine;
      aMapOfTreatedVertexListOfEdge.Clear();
    }
  }
}

// modified by NIZHNY-MKK  Tue Nov 21 17:32:55 2000.END
