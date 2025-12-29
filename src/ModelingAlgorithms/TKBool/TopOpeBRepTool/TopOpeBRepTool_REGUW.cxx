// Created on: 1998-12-08
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

#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_REGUW.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

#define M_FORWARD(ori) (ori == TopAbs_FORWARD)
#define M_REVERSED(ori) (ori == TopAbs_REVERSED)
#define M_INTERNAL(ori) (ori == TopAbs_INTERNAL)
#define M_EXTERNAL(ori) (ori == TopAbs_EXTERNAL)

#ifdef OCCT_DEBUG
extern bool TopOpeBRepTool_GettraceREGUFA();
static NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> STATIC_mapf, STATIC_mapw,
  STATIC_mapv;
static NCollection_IndexedMap<TopoDS_Shape> STATIC_mapeds;

void FUN_tro(const int i)
{
  if (i == 1)
    std::cout << "FORWARD";
  else if (i == 2)
    std::cout << "REVERSED";
  else if (i == 3)
    std::cout << "INTERNAL";
  else if (i == 4)
    std::cout << "EXTERNAL";
  else if (i == 5)
    std::cout << "CLOSING";
}

Standard_EXPORT int FUN_adds(const TopoDS_Shape& s)
{
  TopAbs_ShapeEnum        typ = s.ShapeType();
  TCollection_AsciiString aa;
  int                     is = 0;
  if (typ == TopAbs_VERTEX)
  {
    aa = TCollection_AsciiString("v");
    is = STATIC_mapv.Add(s);
  }
  if (typ == TopAbs_EDGE)
  {
    aa = TCollection_AsciiString("e");
    is = STATIC_mapeds.Add(s);
  }
  if (typ == TopAbs_WIRE)
  {
    aa = TCollection_AsciiString("wi");
    is = STATIC_mapw.Add(s);
  }
  if (typ == TopAbs_FACE)
  {
    aa = TCollection_AsciiString("f");
    is = STATIC_mapf.Add(s);
  }
  return is;
}
#endif

extern void FUN_tool_tori(const TopAbs_Orientation Or);

void FUN_tool_Add(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& map,
  const TopoDS_Shape&                                                                         key,
  const TopoDS_Shape& subitem)
{
  if (map.IsBound(key))
    map.ChangeFind(key).Append(subitem);
  else
  {
    NCollection_List<TopoDS_Shape> los;
    los.Append(subitem);
    map.Bind(key, los);
  }
}

static void FUN_Raise()
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
    std::cout << "*** Raise REGUW" << std::endl;
//  throw Standard_Failure("REGUW");
#endif
}

//=================================================================================================

TopOpeBRepTool_REGUW::TopOpeBRepTool_REGUW(const TopoDS_Face& Fref)
    : myCORRISO(Fref)
{
  myS.Nullify();
  hasnewsplits = false;

  myEsplits.Clear();
  myOwNw.Clear();

  mymapvEds.Clear();
  mymapvmultiple.Clear();
  myListVmultiple.Clear();

  iStep = 0;
}

//=================================================================================================

const TopoDS_Face& TopOpeBRepTool_REGUW::Fref() const
{
  return (myCORRISO.Fref());
}

//=================================================================================================

void TopOpeBRepTool_REGUW::SetEsplits(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    Esplits)
{
  myEsplits = Esplits;
}

//=================================================================================================

void TopOpeBRepTool_REGUW::GetEsplits(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    Esplits) const
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  Esplits = myEsplits;
}

//=================================================================================================

void TopOpeBRepTool_REGUW::SetOwNw(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& OwNw)
{
  myOwNw = OwNw;
}

//=================================================================================================

void TopOpeBRepTool_REGUW::GetOwNw(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& OwNw)
  const
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  OwNw = myOwNw;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::SplitEds()
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
    std::cout << "**    SPLITTING EDGES    **" << std::endl;
#endif

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mehasIv;
  int                                                           i;
  for (i = 1; i <= mymapvEds.Extent(); i++)
  {
    const TopOpeBRepTool_connexity& co = mymapvEds(i);
    NCollection_List<TopoDS_Shape>  loe;
    int                             ni = co.IsInternal(loe);
    if (ni == 0)
      continue;
    NCollection_List<TopoDS_Shape>::Iterator ite(loe);
    for (; ite.More(); ite.Next())
      mehasIv.Add(ite.Value());
  }

  for (i = 1; i <= mehasIv.Extent(); i++)
  {
    const TopoDS_Edge&             e = TopoDS::Edge(mehasIv.FindKey(i));
    NCollection_List<TopoDS_Shape> splits;
    bool                           issp   = false;
    bool                           isdone = myEsplits.IsBound(e);
    if (isdone)
      splits.Assign(myEsplits.Find(e));
    else
      issp = TopOpeBRepTool_TOOL::SplitE(e, splits);
    if (issp)
      hasnewsplits = true;
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << "e" << FUN_adds(e) << " gives splits =" << std::endl;
#endif
    if (!(issp || isdone))
      continue; // nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnyi

    // e gives splits = {esp has vertices {vv}}
    NCollection_List<TopoDS_Shape>::Iterator ite(splits);
    for (; ite.More(); ite.Next())
    {
      const TopoDS_Shape& esp = ite.Value();
#ifdef OCCT_DEBUG
      if (trc)
      {
        std::cout << " e" << FUN_adds(esp);
      }
#endif
      TopExp_Explorer exvv(esp, TopAbs_VERTEX);
      for (; exvv.More(); exvv.Next())
      {
        const TopoDS_Shape&       vv  = exvv.Current();
        TopOpeBRepTool_connexity& co  = mymapvEds.ChangeFromKey(vv);
        bool                      isb = co.RemoveItem(INTERNAL, e);
        if (!isb)
          continue;
        int ivv = TopOpeBRepTool_TOOL::OriinSorclosed(vv, esp);
        co.AddItem(ivv, esp);
      } // exvv(exvv,VERTEX)
    } // ite(splits)
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << std::endl;
#endif
  }
  return true;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepTool_REGUW::S() const
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  return myS;
}

//=================================================================================================

void TopOpeBRepTool_REGUW::Init(const TopoDS_Shape& S)
{
  myS = S;
  InitStep(S);
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::HasInit() const
{
  return (!myS.IsNull());
}

//=================================================================================================

void TopOpeBRepTool_REGUW::InitStep(const TopoDS_Shape& S)
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  TopoDS_Shape null;
  myCORRISO.Init(null);
  myCORRISO.Init(S);

  mymapvEds.Clear();
  mymapvmultiple.Clear();
  myListVmultiple.Clear();
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::MapS()
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
    std::cout << "**    MAPPING    **" << std::endl;
#endif

  // Prequesitories :
  //        0) an edge with an INTERNAL vertex binded in <mapVedges> must be
  //           split
  //           After splitting such edges, we deal only with FORWARD and
  //           REVERSED vertices.
  //        1) a vertex belonging to a CLOSING edge is represented by 2 distinct
  //          2d points in the UV space.
  //        2) a vertex belonging to a  DEGENERATED edge has at least 2 UV rep.
  //          (if the original degenerated edge has been split).
  //          the bounds of the degenerated edge share the same TShape.
  //        3) a CLOSED edge binds the same vertex oriented FORWARD and REVERSED.

  // For each vertex get non-"singular" edges in which the vertex is oriented
  // FORWARD and REVERSED.
  // <mymapvmultiple(1)> binds v and ed if :
  //            v is FORWARD in ed FORWARD   : oriFF(v,ed) = FORWARD
  //            v is REVERSED in ed REVERSED : oriFF(v,ed) = FORWARD
  // oriFF(v,ed) = (ori(ed) == FORWARD) ? ori(ed) : ori(ed).Reverse.

  // A "singular" edge is a closed, a closing or a degenerated edge.
  // In <mymapvmultiple(5)>, we bind with the key <v>:
  // - the FORWARD and the REVERSED CLOSING ancestor edges of <v>
  // - the DEGENERATED ancestor edge of <v>
  // - the CLOSED edge ancestor edge of <v>

  // A MULTIPLE vertex has its single UV representation's connexity > 2

  const TopoDS_Shape& CS = myCORRISO.S();
  TopExp_Explorer     exe(CS, TopAbs_EDGE);
  for (; exe.More(); exe.Next())
  {
    const TopoDS_Edge& ed = TopoDS::Edge(exe.Current());

    bool isdgE = BRep_Tool::Degenerated(ed);
    bool iscE  = TopOpeBRepTool_TOOL::IsClosingE(ed, myCORRISO.S(), Fref());
    iscE       = iscE && !isdgE; // closing ed
    TopoDS_Shape vcl;
    bool         isvcl = TopOpeBRepTool_TOOL::ClosedE(ed, TopoDS::Vertex(vcl));
    isvcl              = isvcl && !isdgE;

    TopExp_Explorer exv(ed, TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
    {
      const TopoDS_Vertex&     v   = TopoDS::Vertex(exv.Current());
      bool                     isb = mymapvEds.Contains(v);
      TopOpeBRepTool_connexity theconnexity(v);
      if (!isb)
        mymapvEds.Add(v, theconnexity);
      TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);

      if (isdgE)
      {
        // avoid adding it twice
        co.RemoveItem(CLOSING, ed);
        co.AddItem(CLOSING, ed);
        break;
      }
      else if (iscE)
      {
        // FORWARD and REVERSED edge is added
        co.AddItem(CLOSING, ed);
      }
      else if (isvcl && (v.IsSame(vcl)))
      {
        // avoid adding it twice
        co.RemoveItem(CLOSING, ed);
        co.AddItem(CLOSING, ed);
      }
      else
      {
        int iov = TopOpeBRepTool_TOOL::OriinSor(v, ed, false); // iov != 0
        co.AddItem(iov, ed);
      }

    } // exv
  } // exe

#ifdef OCCT_DEBUG
  if (trc)
  {
    for (int iv = 1; iv <= mymapvEds.Extent(); iv++)
    {
      const TopoDS_Vertex& v = TopoDS::Vertex(mymapvEds.FindKey(iv));
      std::cout << "#v" << FUN_adds(v) << " :\n";
      const TopOpeBRepTool_connexity& co = mymapvEds(iv);
      for (int i = 1; i <= 5; i++)
      {
        NCollection_List<TopoDS_Shape> eds;
        int                            ieds = co.Item(i, eds);
        if (ieds == 0)
          continue;
        std::cout << " is ";
        FUN_tro(i);
        std::cout << " in ";
        NCollection_List<TopoDS_Shape>::Iterator ite(eds);
        for (; ite.More(); ite.Next())
        {
          const TopoDS_Edge& e = TopoDS::Edge(ite.Value());
          std::cout << "e" << FUN_adds(e);
          FUN_tool_tori(e.Orientation());
          TopoDS_Vertex vclo;
          bool          cloE = TopOpeBRepTool_TOOL::ClosedE(e, vclo);
          if (cloE)
            std::cout << "closed";
          bool dgE = BRep_Tool::Degenerated(e);
          if (dgE)
            std::cout << "degenerated";
          bool iscE = TopOpeBRepTool_TOOL::IsClosingE(e, myCORRISO.S(), Fref());
          if (iscE)
            std::cout << "closing";
          std::cout << " ";
        }
        std::cout << std::endl;
      } // i=1..5
    }
  } // trc
#endif

  int nV = mymapvEds.Extent();
  if (nV < 1)
    return false;

  int i;
  for (i = 1; i <= mymapvEds.Extent(); i++)
  {
    const TopoDS_Shape&             v      = mymapvEds.FindKey(i);
    const TopOpeBRepTool_connexity& co     = mymapvEds(i);
    bool                            faulty = co.IsFaulty();
    if (faulty)
      return false;
    bool multiple = co.IsMultiple();
    if (multiple)
      if (mymapvmultiple.Add(v))
        myListVmultiple.Append(v);
  }
  return true;
} // MapS

static void FUN_nextdata(const int                        iStep,
                         const TopoDS_Edge&               e,
                         const occ::handle<Geom2d_Curve>& pc,
                         TopoDS_Vertex&                   v,
                         gp_Pnt2d&                        p2d,
                         gp_Dir2d&                        tg)
// prequesitory : pc = 2drep(e),
// ori(v,e)=iv1e1 -> v, p2d=pt2d(v,e), tg=tg2d(v,e).
{
  int          iv1e1       = (iStep == 1) ? FORWARD : REVERSED;
  TopoDS_Shape aLocalShape = TopOpeBRepTool_TOOL::Vertex(iv1e1, e);
  v                        = TopoDS::Vertex(aLocalShape);
  //  v = TopoDS::Vertex(TopOpeBRepTool_TOOL::Vertex(iv1e1,e));
  double par1   = TopOpeBRepTool_TOOL::ParE(iv1e1, e);
  bool   line   = FUN_tool_line(pc);
  bool   quad   = FUN_tool_quad(pc); // mytg2d is reapproximated if PCquad
  bool   approx = quad && (!line);

  gp_Vec2d tg2d;
  if (approx)
  {
    p2d = pc->Value(par1);

    int    iv0e1 = (iStep == 1) ? REVERSED : FORWARD;
    double par0  = TopOpeBRepTool_TOOL::ParE(iv0e1, e);
    // Getting a point near point<Index> of <E>
    double   x   = 0.2345;
    double   par = (1 - x) * par1 + x * par0;
    gp_Pnt2d pmil;
    pc->D1(par, pmil, tg2d);
  }
  else
  {
    pc->D1(par1, p2d, tg2d);
  }
  tg = gp_Dir2d(tg2d);
  if (M_REVERSED(e.Orientation()))
    tg.Reverse();
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::InitBlock()
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
#endif

  myv0.Nullify();
  myp2d0 = gp_Pnt2d(1.e7, 1.e7);
  myed.Nullify();
  int iv0e1 = (iStep == 1) ? REVERSED : FORWARD;

  // updating <mymapvmultiple> and myListVmultiple
  NCollection_List<TopoDS_Shape>::Iterator itmu(myListVmultiple);
  while (itmu.More())
  {
    const TopoDS_Shape&             vmu  = itmu.Value();
    const TopOpeBRepTool_connexity& cmu  = mymapvEds.FindFromKey(vmu);
    bool                            mult = cmu.IsMultiple();
    if (!mult)
    {
      mymapvmultiple.Remove(vmu);
      myListVmultiple.Remove(itmu);
    }
    else
      itmu.Next();
  }

  // myv0 :
  if (myListVmultiple.IsEmpty())
  {
    int i;
    for (i = 1; i <= mymapvEds.Extent(); i++)
    {
      const TopoDS_Vertex&            v  = TopoDS::Vertex(mymapvEds.FindKey(i));
      const TopOpeBRepTool_connexity& co = mymapvEds(i);
      NCollection_List<TopoDS_Shape>  lea;
      int                             nea = co.Item(iv0e1, lea);
      NCollection_List<TopoDS_Shape>  leb;
      int                             neb = co.Item(CLOSING, leb);
      NCollection_List<TopoDS_Shape>  le;
      le.Append(lea);
      le.Append(leb);
      int ne = nea + neb;
      if (ne != 0)
      {
        myv0 = v;
        break;
      }
    }
  }
  else
  {
    myv0 = TopoDS::Vertex(myListVmultiple.First());
  }
  if (myv0.IsNull())
    return false;
  // myed :
  const TopOpeBRepTool_connexity& co = mymapvEds.FindFromKey(myv0);

  NCollection_List<TopoDS_Shape> lea;
  int                            nea = co.Item(iv0e1, lea);
  NCollection_List<TopoDS_Shape> leb;
  int                            neb = co.Item(CLOSING, leb);
  NCollection_List<TopoDS_Shape> le;
  le.Append(lea);
  le.Append(leb);
  int ne = nea + neb;
  if (ne == 0)
    return false;

  if (nea > 0)
    myed = TopoDS::Edge(le.First());
  else
  { // <myv0> CLOSING in <myed>
    NCollection_List<TopoDS_Shape>::Iterator itb(le);
    for (; itb.More(); itb.Next())
    {
      const TopoDS_Edge& eb   = TopoDS::Edge(itb.Value());
      bool               iscE = TopOpeBRepTool_TOOL::IsClosingE(myed, myCORRISO.S(), Fref());
      if (!iscE)
      {
        myed = eb;
        break;
      }
      int iov0 = TopOpeBRepTool_TOOL::OriinSorclosed(myv0, myed);
      if (iov0 != iv0e1)
        continue;
    } // itb(leb)
  }
  if (myed.IsNull())
    return false;

  TopExp_Explorer exv(myed, TopAbs_VERTEX);
  for (; exv.More(); exv.Next())
  {
    const TopoDS_Shape&       vcur = exv.Current();
    TopOpeBRepTool_connexity& cco  = mymapvEds.ChangeFromKey(vcur);
    // #ifdef OCCT_DEBUG
    //     bool ok =
    // #endif
    cco.RemoveItem(myed);
    //    if (!ok) return false; see for closing vertices
  }

  // myp2d0 :
  TopOpeBRepTool_C2DF c2df;
  bool                bound = myCORRISO.UVRep(myed, c2df);
  if (!bound)
    return false;
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& pc   = c2df.PC(f, l, tol);
  double                           par0 = TopOpeBRepTool_TOOL::ParE(iv0e1, myed);
  pc->D0(par0, myp2d0);

  // myv, myp2d, mytg2d :
  ::FUN_nextdata(iStep, myed, pc, myv, myp2d, mytg2d);

#ifdef OCCT_DEBUG
  if (trc)
  {
    std::cout << std::endl
              << " v0   = v" << FUN_adds(myv0) << " p2d0 = (" << myp2d0.X() << " " << myp2d0.Y()
              << ")" << std::endl;
    std::cout << " vcur = v" << FUN_adds(myv) << "  p2d = (" << myp2d.X() << " " << myp2d.Y() << ")"
              << std::endl;
    std::cout << " ecur = e" << FUN_adds(myed) << std::endl;
  }
#endif
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::NearestE(const NCollection_List<TopoDS_Shape>& loe,
                                    TopoDS_Edge&                          efound) const
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
#endif

  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  efound.Nullify();
  double fac   = 0.45678;
  double tola  = Precision::Angular();
  int    iv0e1 = (iStep == 1) ? REVERSED : FORWARD;

  // initializing
  NCollection_List<TopoDS_Shape>::Iterator ite(loe);
  efound = TopoDS::Edge(ite.Value());
  if (ite.More())
    ite.Next();
  else
    return true;

  TopOpeBRepTool_C2DF c2defound;
  bool                isbfound = myCORRISO.UVRep(efound, c2defound);
  if (!isbfound)
    return false;

  gp_Vec2d tg2dfound = TopOpeBRepTool_TOOL::tryTg2dApp(iv0e1, efound, c2defound, fac);
  if (M_REVERSED(efound.Orientation()))
    tg2dfound.Reverse();
  double angfound = 1.e7;
  if (iStep == 1)
    angfound = TopOpeBRepTool_TOOL::Matter(mytg2d, tg2dfound);
  else
    angfound = 2. * M_PI - TopOpeBRepTool_TOOL::Matter(tg2dfound, mytg2d);
#ifdef OCCT_DEBUG
  if (trc)
    std::cout << "ang(e" << FUN_adds(myed) << ",e" << FUN_adds(efound) << ")=" << angfound
              << std::endl;
#endif

  // purpose : finding out <efound> /
  //     ecur is given by <tg2d>
  //     le = {ei}
  //     iStep=1 : matterang2d(efound,ecur) = min(ei,ecur)
  //     iStep=2 : oppomatterang2d(efound,ecur) = min(ei,ecur)
  for (; ite.More(); ite.Next())
  {
    const TopoDS_Edge& ei = TopoDS::Edge(ite.Value());
    // for INTERNAL edge eI -> eF+eR
    if (ei.IsSame(myed))
      continue;

    TopOpeBRepTool_C2DF c2dei;
    bool                isbi = myCORRISO.UVRep(ei, c2dei);
    if (!isbi)
      return false;
    gp_Vec2d tg2di = TopOpeBRepTool_TOOL::tryTg2dApp(iv0e1, ei, c2dei, fac);
    if (M_REVERSED(ei.Orientation()))
      tg2di.Reverse();

    double angi = 1.e7;
    if (iStep == 1)
      angi = TopOpeBRepTool_TOOL::Matter(mytg2d, tg2di);
    else
      angi = 2. * M_PI - TopOpeBRepTool_TOOL::Matter(tg2di, mytg2d);
    bool eq = std::abs(angi - angfound) < tola;
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << "ang(e" << FUN_adds(myed) << ",e" << FUN_adds(ei) << ")=" << angi << std::endl;
#endif
    if (eq)
    {
      FUN_Raise();
      return false;
    }

    if (angi < angfound)
    {
      efound   = ei;
      angfound = angi;
    }
  }
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::NextinBlock()
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
    std::cout << std::endl
              << " vcur = v" << FUN_adds(myv) << "   p2d = (" << myp2d.X() << " " << myp2d.Y()
              << ")" << std::endl;
#endif

  int                             iv0e1 = (iStep == 1) ? REVERSED : FORWARD;
  const TopOpeBRepTool_connexity& co    = mymapvEds.FindFromKey(myv);

  // {e} : e is connexed to <myv> && ori(<myv>,e)=iv0e1
  NCollection_List<TopoDS_Shape> lea;
  int                            nea = co.Item(iv0e1, lea);
  NCollection_List<TopoDS_Shape> leb;
  int                            neb = co.Item(CLOSING, leb);
  NCollection_List<TopoDS_Shape> le;
  le.Append(lea);
  le.Append(leb);
  int ne = nea + neb;

  NCollection_List<TopoDS_Shape>::Iterator ite(le);
  while (ite.More())
  {
    const TopoDS_Edge& e = TopoDS::Edge(ite.Value());
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " e" << FUN_adds(e);
#endif

    bool issame = e.IsSame(myed);
    if (issame)
    { // xpu240299 FRA60275, fsp4
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " is same : not valid" << std::endl;
#endif
      le.Remove(ite);
      continue;
    }

    TopOpeBRepTool_C2DF c2df;
    bool                bound = myCORRISO.UVRep(e, c2df);
    if (!bound)
    {
      FUN_Raise();
      return false;
    }
    double                           f, l, tol;
    const occ::handle<Geom2d_Curve>& pc  = c2df.PC(f, l, tol);
    double                           par = TopOpeBRepTool_TOOL::ParE(iv0e1, e);
    gp_Pnt2d                         p2de;
    pc->D0(par, p2de);

    // p2d(myv,ed)=p2d(myv,e)
    bool samep2d = p2de.IsEqual(myp2d, mytol2d);
    if (!samep2d)
      le.Remove(ite);
    else
      ite.Next();
#ifdef OCCT_DEBUG
    if (trc)
    {
      if (samep2d)
        std::cout << " valid" << std::endl;
      else
        std::cout << " not valid" << std::endl;
    }
#endif
  } // ite(le)
  ne = le.Extent();
  if (ne == 0)
  {
    FUN_Raise();
    return false;
  }

  // myed :
  ne = le.Extent();
  if (ne == 1)
    myed = TopoDS::Edge(le.First());
  else
  {
    TopoDS_Edge efound;
    bool        found = NearestE(le, efound);
    if (!found)
    {
      FUN_Raise();
      return false;
    }
    myed = efound;
  }

  TopExp_Explorer exv(myed, TopAbs_VERTEX);
  for (; exv.More(); exv.Next())
  {
    TopOpeBRepTool_connexity& cco = mymapvEds.ChangeFromKey(exv.Current());
    // #ifdef OCCT_DEBUG
    //     bool ok =
    // #endif
    cco.RemoveItem(myed);
    //    if (!ok) {FUN_Raise(); return false;} closed edges
  }

  TopOpeBRepTool_C2DF c2df;
  myCORRISO.UVRep(myed, c2df);
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& pc = c2df.PC(f, l, tol);
  ::FUN_nextdata(iStep, myed, pc, myv, myp2d, mytg2d);

#ifdef OCCT_DEBUG
  if (trc)
  {
    std::cout << " vcur = v" << FUN_adds(myv) << "  p2d = (" << myp2d.X() << " " << myp2d.Y() << ")"
              << std::endl;
    std::cout << " ecur = e" << FUN_adds(myed) << std::endl;
  }
#endif
  return true;
} // NextinBlock

//=======================================================================
// function : REGU
// purpose  : step <iStep> for regularization
//=======================================================================

bool TopOpeBRepTool_REGUW::REGU(const int istep,
#ifdef OCCT_DEBUG
                                const TopoDS_Shape& Scur,
#else
                                const TopoDS_Shape&,
#endif
                                NCollection_List<TopoDS_Shape>& Splits)
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  iStep = istep;
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
    std::cout << std::endl << "**    REGU    istep=**" << iStep << std::endl;
#endif

  Splits.Clear();
  // A valid face is built on "valid" wires.
  // A valid wire has for UV representation a set of 2d edges of connexity = 2.

  // Starting element is a vertex of single UV representation with connexity greater
  // than 2.
  // Given <v0m>, we get :
  //  - <e1> an edge where <v0m> is oriented REVERSED(step1)/FORWARD(step2)
  //  - <v1> the other vertex of <e1> oriented FORWARD(step1)/REVERSED(step2)
  //  - <p2d1> is <v1>'s UV representation attached to <e1>,
  //  - tg2d<e1> = Tgt(p2d1,e1)  if <e1> FORWARD(step1)
  //             = -Tgt(p2d1,e1) if <e1> is REVERSED(step1) in the face with
  //    Tgt(p2d1,e1) = tangent to <e1>'s pcurve at point <p2d1>

  // step1 :
  // -------
  // Given a starting couple (<vi>, <p2di>, tg2d<e1>, <ei>) with :
  //  - <vi> oriented FORWARD in <ei>,
  //  - <vi>'s 2d representation falling on an UV bound of <ei>'s pcurve (if <vi> is
  //    on a closing edge),
  // We get the edge <ei+1> such that :
  //  - <ei+1> is not already touched,
  //  - <vi> is oriented REVERSED(step1) in <ei+1>,
  //  - <ei+1> has an UV bound falling into geometry <p2di>.
  //  - The oriented angle (-tg2d<ei>, tg2d<ei+1>) is the lowest,
  // ( The angle is computed in the anti-trigonometric sense;
  //   the tangents's orientation are reversed if the edge is oriented REVERSED in the face )
  //   = Smaller oriented (following trigonometric sense) angle (tg2d<ei+1>, -tg2d<ei>)
  //
  // step2 :  reverse the orientations and the angle describes non-matter.
  // -------

  // tol2d = 1.e-2*(lowest distance (p2df,p2dl) between all non-closed
  //                and non-degenerated pcurves seen during the regularization).
  // This value is updated in method chkp2dFORinE.
  // The comparison of p2d points is coupled with comparison of vertices
  // We should in fact use bounding boxes on pcurves but this is too expensive!!!
  mytol2d = 1.e-5;
  isinit0 = true;
  NCollection_List<TopoDS_Shape> loEcur, loW;

  int nite = 0;
  int nE   = myCORRISO.Eds().Extent(); // recall myCORRISO.Init(myS);
  NCollection_List<TopoDS_Shape>::Iterator ite(myCORRISO.Eds());
  for (; ite.More(); ite.Next())
  {
    TopAbs_Orientation oe = ite.Value().Orientation();
    if (M_INTERNAL(oe) || M_EXTERNAL(oe))
      nE--;
  }

  while (nite <= nE)
  {
    bool FINI = (nite == nE);

    //*** Getting starts elements if not defined
    // -----------------------------------------
    if (isinit0 && !FINI)
    {
      bool ok = InitBlock();
      if (!ok)
      {
#ifdef OCCT_DEBUG
        if (trc)
          std::cout << "** InitBlock fails" << std::endl;
#endif
        {
          FUN_Raise();
          return false;
        }
      }
      loEcur.Append(myed); // add it to the current wire
      nite++;              // increment nite
      isinit0 = false;
      continue;
    }

    //*** Checking the wire is not already closed
    // ------------------------------------------
    // If last vertex <v> is the first one <v0> and <p2d> == <p2d0>, the wire is closed
    bool wireisclosed = myp2d.IsEqual(myp2d0, mytol2d);
    if (wireisclosed)
    {

      //* Adding INTERNAL/EXTERNAL edges to wires
      // ----------------------------------------
      NCollection_List<TopoDS_Shape>::Iterator ite1(loEcur);
      for (; ite1.More(); ite1.Next())
      {
        const TopoDS_Shape& e = ite1.Value();
        TopExp_Explorer     exv(e, TopAbs_VERTEX);
        for (; exv.More(); exv.Next())
        {
          const TopoDS_Shape&                      v  = exv.Current();
          TopOpeBRepTool_connexity&                co = mymapvEds.ChangeFromKey(v);
          NCollection_List<TopoDS_Shape>&          le = co.ChangeItem(INTERNAL);
          NCollection_List<TopoDS_Shape>::Iterator itte(le);
          while (itte.More())
          {
            const TopoDS_Shape& ee = itte.Value();
            TopAbs_Orientation  oe = ee.Orientation();
            if (M_INTERNAL(oe) || M_EXTERNAL(oe))
            {
              loEcur.Append(ee);
              le.Remove(itte);
            }
            else
              itte.Next();
          } // itte(le)
        } // exv(e)
      } // ite(loEcur)
      FINI = (nite == nE);

      // if Scur = <currentW> gives only one new wire, and has no new split edges, <currentW>
      // is valid and unchanged.
      bool onewok = FINI && loW.IsEmpty() && !hasnewsplits;
      if (onewok)
      {
#ifdef OCCT_DEBUG
        if (trc)
          std::cout << "wire " << FUN_adds(Scur) << " is found valid\n";
#endif
        return true;
      } // onewok

      TopoDS_Wire newW;
      bool        wiok = FUN_tool_MakeWire(loEcur, newW);
      if (wiok)
        loW.Append(newW);
      else
      {
#ifdef OCCT_DEBUG
        if (trc)
          std::cout << "** FUN_tool_MakeWire fails" << std::endl;
#endif
        {
          FUN_Raise();
          return false;
        }
      }
#ifdef OCCT_DEBUG
      if (trc)
      {
        std::cout << "#->new wire = wi" << FUN_adds(newW) << " = ";
        for (NCollection_List<TopoDS_Shape>::Iterator it(loEcur); it.More(); it.Next())
          std::cout << " e" << FUN_adds(it.Value());
        std::cout << "\n\n";
      }
#endif
      isinit0 = true;
      loEcur.Clear();

      if (FINI)
      {
        Splits.Append(loW);
        return true;
      }
      continue;
    } // wireisclosed

    //*** Iteration on UV connexed edges
    // ---------------------------------
    // <v> is in <ed>, <p2d> = <v> current UV representation
    bool gotonext = NextinBlock();
    if (!gotonext)
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << "** NextinBlock fails" << std::endl;
#endif
      {
        FUN_Raise();
        return false;
      }
    }
    loEcur.Append(myed);
    nite++;
  } // nite

  return true;
}

//=======================================================================
// function : REGU
// purpose  : complete regularization
//=======================================================================

bool TopOpeBRepTool_REGUW::REGU()
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  NCollection_List<TopoDS_Shape> null;

  bool toregu = !myListVmultiple.IsEmpty();
  toregu      = toregu || hasnewsplits;
  NCollection_List<TopoDS_Shape> Splits;
  if (!toregu)
  {
    myOwNw.Bind(S(), Splits);
    return true;
  }

  // iStep = 1;
  NCollection_List<TopoDS_Shape> loS;
  bool                           ok = REGU(1, S(), loS);
  if (!ok)
  {
    FUN_Raise();
    return false;
  }

  // iStep = 2;
  if (loS.IsEmpty())
    loS.Append(S()); // no new shape

  NCollection_List<TopoDS_Shape>::Iterator it(loS);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& Scur = it.Value();
    InitStep(Scur);
    MapS();
    bool toregu1 = !myListVmultiple.IsEmpty();
    if (!toregu1)
    {
      Splits.Append(Scur);
      continue;
    }

    NCollection_List<TopoDS_Shape> sp;
    ok = REGU(2, Scur, sp);
    if (!ok)
    {
      FUN_Raise();
      return false;
    }
    if (sp.IsEmpty())
      sp.Append(Scur); // no new shape
    Splits.Append(sp);
  }
  myOwNw.Bind(S(), Splits);
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::GetSplits(NCollection_List<TopoDS_Shape>& Splits) const
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  bool isb = myOwNw.IsBound(S());
  if (!isb)
    return false;
  Splits = myOwNw.Find(S());
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::Connexity(const TopoDS_Vertex& v, TopOpeBRepTool_connexity& co) const
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  bool isb = mymapvEds.Contains(v);
  if (!isb)
    return false;
  co = mymapvEds.FindFromKey(v);
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::AddNewConnexity(const TopoDS_Vertex& v,
                                           const int            OriKey,
                                           const TopoDS_Edge&   e)
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  bool isb = mymapvEds.Contains(v);
  if (!isb)
    return false;

  bool ok = myCORRISO.AddNewConnexity(v, e);
  if (!ok)
    return false;

  TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);
  co.AddItem(OriKey, e);

#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
  {
    std::cout << "** setting new connexity : v" << FUN_adds(v) << " is ";
    FUN_tro(OriKey);
    std::cout << " in  e" << FUN_adds(e);
    FUN_tool_tori(e.Orientation());
    TopoDS_Vertex vclo;
    bool          cloE = TopOpeBRepTool_TOOL::ClosedE(e, vclo);
    if (cloE)
      std::cout << " closed";
    bool dgE = BRep_Tool::Degenerated(e);
    if (dgE)
      std::cout << " degenerated";
    std::cout << std::endl;
  }
#endif
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::RemoveOldConnexity(const TopoDS_Vertex& v,
                                              const int            OriKey,
                                              const TopoDS_Edge&   e)
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  bool isb = mymapvEds.Contains(v);
  if (!isb)
    return false;

  bool ok = myCORRISO.RemoveOldConnexity(v, e);
  //  if (!ok) return false;

  TopOpeBRepTool_connexity& co = mymapvEds.ChangeFromKey(v);
  ok                           = co.RemoveItem(OriKey, e);
  if (!ok)
    return false;

#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc)
  {
    std::cout << "** removing old connexity : v" << FUN_adds(v) << " for e" << FUN_adds(e);
    FUN_tool_tori(e.Orientation());
    TopoDS_Vertex vclo;
    bool          cloE = TopOpeBRepTool_TOOL::ClosedE(e, vclo);
    if (cloE)
      std::cout << " closed";
    bool dgE = BRep_Tool::Degenerated(e);
    if (dgE)
      std::cout << " degenerated";
    std::cout << std::endl;
  }
#endif
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_REGUW::UpdateMultiple(const TopoDS_Vertex& v)
{
  if (!HasInit())
    throw Standard_Failure("TopOpeBRepTool_REGUW : NO INIT");
  bool isb = mymapvEds.Contains(v);
  if (!isb)
    return false;
  const TopOpeBRepTool_connexity& co     = mymapvEds.FindFromKey(v);
  bool                            ismult = co.IsMultiple();
  if (ismult)
    if (mymapvmultiple.Add(v))
      myListVmultiple.Append(v);
  return true;
}
