// Created on: 1995-11-10
// Created by: Yves FRICAUD
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

#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepAlgo_Loop.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Ax2.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Sequence.hxx>

#include <cstdio>
// #define OCCT_DEBUG_ALGO
#ifdef OCCT_DEBUG_ALGO
bool         AffichLoop = true;
int          NbLoops    = 0;
int          NbWires    = 1;
static char* name       = new char[100];
#endif

//=================================================================================================

BRepAlgo_Loop::BRepAlgo_Loop()
    : myTolConf(0.001)
{
}

//=================================================================================================

void BRepAlgo_Loop::Init(const TopoDS_Face& F)
{
  myConstEdges.Clear();
  myEdges.Clear();
  myVerOnEdges.Clear();
  myNewWires.Clear();
  myNewFaces.Clear();
  myCutEdges.Clear();
  myFace = F;
}

//=======================================================================
// function : Bubble
// purpose  : Orders the sequence of vertices by increasing parameter.
//=======================================================================

static void Bubble(const TopoDS_Edge& E, NCollection_Sequence<TopoDS_Shape>& Seq)
{
  // Remove duplicates
  for (int i = 1; i < Seq.Length(); i++)
    for (int j = i + 1; j <= Seq.Length(); j++)
      if (Seq(i) == Seq(j))
      {
        Seq.Remove(j);
        j--;
      }

  bool          Invert   = true;
  int           NbPoints = Seq.Length();
  double        U1, U2;
  TopoDS_Vertex V1, V2;

  while (Invert)
  {
    Invert = false;
    for (int i = 1; i < NbPoints; i++)
    {
      TopoDS_Shape aLocalV = Seq.Value(i).Oriented(TopAbs_INTERNAL);
      V1                   = TopoDS::Vertex(aLocalV);
      aLocalV              = Seq.Value(i + 1).Oriented(TopAbs_INTERNAL);
      V2                   = TopoDS::Vertex(aLocalV);
      //      V1 = TopoDS::Vertex(Seq.Value(i)  .Oriented(TopAbs_INTERNAL));
      //      V2 = TopoDS::Vertex(Seq.Value(i+1).Oriented(TopAbs_INTERNAL));

      U1 = BRep_Tool::Parameter(V1, E);
      U2 = BRep_Tool::Parameter(V2, E);
      if (U2 < U1)
      {
        Seq.Exchange(i, i + 1);
        Invert = true;
      }
    }
  }
}

//=================================================================================================

void BRepAlgo_Loop::AddEdge(TopoDS_Edge& E, const NCollection_List<TopoDS_Shape>& LV)
{
  myEdges.Append(E);
  myVerOnEdges.Bind(E, LV);
}

//=================================================================================================

void BRepAlgo_Loop::AddConstEdge(const TopoDS_Edge& E)
{
  myConstEdges.Append(E);
}

//=================================================================================================

void BRepAlgo_Loop::AddConstEdges(const NCollection_List<TopoDS_Shape>& LE)
{
  NCollection_List<TopoDS_Shape>::Iterator itl(LE);
  for (; itl.More(); itl.Next())
  {
    myConstEdges.Append(itl.Value());
  }
}

//=================================================================================================

void BRepAlgo_Loop::SetImageVV(const BRepAlgo_Image& theImageVV)
{
  myImageVV = theImageVV;
}

//=======================================================================
// function : UpdateClosedEdge
// purpose  : If the first or the last vertex of intersection
//           coincides with the closing vertex, it is removed from SV.
//           it will be added at the beginning and the end of SV by the caller.
//=======================================================================

static TopoDS_Vertex UpdateClosedEdge(const TopoDS_Edge& E, NCollection_Sequence<TopoDS_Shape>& SV)
{
  TopoDS_Vertex VB[2], V1, V2, VRes;
  gp_Pnt        P, PC;
  bool          OnStart = false, OnEnd = false;
  //// modified by jgv, 13.04.04 for OCC5634 ////
  TopExp::Vertices(E, V1, V2);
  double Tol = BRep_Tool::Tolerance(V1);
  ///////////////////////////////////////////////

  if (SV.IsEmpty())
    return VRes;

  VB[0] = TopoDS::Vertex(SV.First());
  VB[1] = TopoDS::Vertex(SV.Last());
  PC    = BRep_Tool::Pnt(V1);

  for (int i = 0; i < 2; i++)
  {
    P = BRep_Tool::Pnt(VB[i]);
    if (P.IsEqual(PC, Tol))
    {
      VRes = VB[i];
      if (i == 0)
        OnStart = true;
      else
        OnEnd = true;
    }
  }
  if (OnStart && OnEnd)
  {
    if (!VB[0].IsSame(VB[1]))
    {
#ifdef OCCT_DEBUG_ALGO
      if (AffichLoop)
        std::cout << "Two different vertices on the closing vertex" << std::endl;
#endif
    }
    else
    {
      SV.Remove(1);
      if (!SV.IsEmpty())
        SV.Remove(SV.Length());
    }
  }
  else if (OnStart)
    SV.Remove(1);
  else if (OnEnd)
    SV.Remove(SV.Length());

  return VRes;
}

//=================================================================================================

static void RemovePendingEdges(
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    MVE)
{
  //--------------------------------
  // Remove hanging edges.
  //--------------------------------
  NCollection_List<TopoDS_Shape>           ToRemove;
  NCollection_List<TopoDS_Shape>::Iterator itl;
  bool                                     YaSupress = true;
  TopoDS_Vertex                            V1, V2;

  while (YaSupress)
  {
    YaSupress = false;
    NCollection_List<TopoDS_Shape>                         VToRemove;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> EToRemove;

    for (int iV = 1; iV <= MVE.Extent(); iV++)
    {
      const TopoDS_Shape&                   aVertex = MVE.FindKey(iV);
      const NCollection_List<TopoDS_Shape>& anEdges = MVE(iV);
      if (anEdges.IsEmpty())
      {
        VToRemove.Append(aVertex);
      }
      if (anEdges.Extent() == 1)
      {
        const TopoDS_Edge& E = TopoDS::Edge(anEdges.First());
        TopExp::Vertices(E, V1, V2);
        if (!V1.IsSame(V2))
        {
          VToRemove.Append(aVertex);
          EToRemove.Add(anEdges.First());
        }
      }
    }

    if (!VToRemove.IsEmpty())
    {
      YaSupress = true;
      for (itl.Initialize(VToRemove); itl.More(); itl.Next())
      {
        MVE.RemoveKey(itl.Value());
      }
      if (!EToRemove.IsEmpty())
      {
        for (int iV = 1; iV <= MVE.Extent(); iV++)
        {
          NCollection_List<TopoDS_Shape>& LE = MVE.ChangeFromIndex(iV);
          itl.Initialize(LE);
          while (itl.More())
          {
            if (EToRemove.Contains(itl.Value()))
            {
              LE.Remove(itl);
            }
            else
              itl.Next();
          }
        }
      }
    }
  }
}

//=================================================================================================

static bool SamePnt2d(const TopoDS_Vertex& V, TopoDS_Edge& E1, TopoDS_Edge& E2, TopoDS_Face& F)
{
  double       f1, f2, l1, l2;
  gp_Pnt2d     P1, P2;
  TopoDS_Shape aLocalF = F.Oriented(TopAbs_FORWARD);
  TopoDS_Face  FF      = TopoDS::Face(aLocalF);
  //  TopoDS_Face FF = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
  occ::handle<Geom2d_Curve> C1 = BRep_Tool::CurveOnSurface(E1, FF, f1, l1);
  occ::handle<Geom2d_Curve> C2 = BRep_Tool::CurveOnSurface(E2, FF, f2, l2);
  if (E1.Orientation() == TopAbs_FORWARD)
    P1 = C1->Value(f1);
  else
    P1 = C1->Value(l1);

  if (E2.Orientation() == TopAbs_FORWARD)
    P2 = C2->Value(l2);
  else
    P2 = C2->Value(f2);
  double Tol  = 100 * BRep_Tool::Tolerance(V);
  double Dist = P1.Distance(P2);
  return Dist < Tol;
}

//=======================================================================
// function : SelectEdge
// purpose  : Find edge <NE> connected to <CE> by vertex <CV> in the
//           list <LE>. <NE> is removed from the list. If <CE> is
//           also in the list <LE> with the same orientation, it is
//           removed from the list.
//=======================================================================

static bool SelectEdge(const TopoDS_Face&              F,
                       const TopoDS_Edge&              CE,
                       const TopoDS_Vertex&            CV,
                       TopoDS_Edge&                    NE,
                       NCollection_List<TopoDS_Shape>& LE)
{
  NCollection_List<TopoDS_Shape>::Iterator itl;
  NE.Nullify();
#ifdef OCCT_DEBUG_ALGO
  if (AffichLoop)
  {
    if (LE.Extent() > 2)
    {
      std::cout << "vertex on more than 2 edges in a face." << std::endl;
    }
  }
#endif
  for (itl.Initialize(LE); itl.More(); itl.Next())
  {
    if (itl.Value().IsEqual(CE))
    {
      LE.Remove(itl);
      break;
    }
  }
  if (LE.Extent() > 1)
  {
    //--------------------------------------------------------------
    // Several edges possible.
    // - Test edges different from CE , Selection of edge
    // for which CV has U,V closer to the face
    // than corresponding to CE.
    // - If several edges give representation less than the tolerance.
    // discrimination on tangents.
    //--------------------------------------------------------------
    TopLoc_Location L;
    double          f, l;
    TopoDS_Face     FForward = F;
    FForward.Orientation(TopAbs_FORWARD);

    occ::handle<Geom2d_Curve> C = BRep_Tool::CurveOnSurface(CE, FForward, f, l);
    int                       k = 1, kmin   = 0;
    double                    dist, distmin = 100 * BRep_Tool::Tolerance(CV);
    double                    u;
    if (CE.Orientation() == TopAbs_FORWARD)
      u = l;
    else
      u = f;

    gp_Pnt2d P2, PV = C->Value(u);

    for (itl.Initialize(LE); itl.More(); itl.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(itl.Value());
      if (!E.IsSame(CE))
      {
        C = BRep_Tool::CurveOnSurface(E, FForward, f, l);
        if (E.Orientation() == TopAbs_FORWARD)
          u = f;
        else
          u = l;
        P2   = C->Value(u);
        dist = PV.Distance(P2);
        if (dist <= distmin)
        {
          kmin    = k;
          distmin = dist;
        }
      }
      k++;
    }
    if (kmin == 0)
      return false;

    k = 1;
    itl.Initialize(LE);
    while (k < kmin)
    {
      k++;
      itl.Next();
    }
    NE = TopoDS::Edge(itl.Value());
    LE.Remove(itl);
  }
  else if (LE.Extent() == 1)
  {
    NE = TopoDS::Edge(LE.First());
    LE.RemoveFirst();
  }
  else
  {
    return false;
  }
  return true;
}

//=================================================================================================

static void PurgeNewEdges(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                NewEdges,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& UsedEdges)
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator it(NewEdges);
  for (; it.More(); it.Next())
  {
    NCollection_List<TopoDS_Shape>&          LNE = NewEdges.ChangeFind(it.Key());
    NCollection_List<TopoDS_Shape>::Iterator itL(LNE);
    while (itL.More())
    {
      const TopoDS_Shape& NE = itL.Value();
      if (!UsedEdges.Contains(NE))
      {
        LNE.Remove(itL);
      }
      else
      {
        itL.Next();
      }
    }
  }
}

//=================================================================================================

static void StoreInMVE(
  const TopoDS_Face& F,
  TopoDS_Edge&       E,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                            MVE,
  bool&                                                                     YaCouture,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& VerticesForSubstitute,
  const double                                                              theTolConf)
{
  TopoDS_Vertex                  V1, V2, V;
  NCollection_List<TopoDS_Shape> Empty;

  gp_Pnt       P1, P;
  BRep_Builder BB;
  for (int iV = 1; iV <= MVE.Extent(); iV++)
  {
    V = TopoDS::Vertex(MVE.FindKey(iV));
    P = BRep_Tool::Pnt(V);
    NCollection_List<TopoDS_Shape> VList;
    TopoDS_Iterator                VerExp(E);
    for (; VerExp.More(); VerExp.Next())
      VList.Append(VerExp.Value());
    NCollection_List<TopoDS_Shape>::Iterator itl(VList);
    for (; itl.More(); itl.Next())
    {
      V1 = TopoDS::Vertex(itl.Value());
      P1 = BRep_Tool::Pnt(V1);
      if (P.IsEqual(P1, theTolConf) && !V.IsSame(V1))
      {
        V.Orientation(V1.Orientation());
        if (VerticesForSubstitute.IsBound(V1))
        {
          TopoDS_Shape OldNewV = VerticesForSubstitute(V1);
          if (!OldNewV.IsSame(V))
          {
            VerticesForSubstitute.Bind(OldNewV, V);
            VerticesForSubstitute(V1) = V;
          }
        }
        else
        {
          if (VerticesForSubstitute.IsBound(V))
          {
            TopoDS_Shape NewNewV = VerticesForSubstitute(V);
            if (!NewNewV.IsSame(V1))
              VerticesForSubstitute.Bind(V1, NewNewV);
          }
          else
          {
            VerticesForSubstitute.Bind(V1, V);
            NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator
              mapit(VerticesForSubstitute);
            for (; mapit.More(); mapit.Next())
              if (mapit.Value().IsSame(V1))
                VerticesForSubstitute(mapit.Key()) = V;
          }
        }
        E.Free(true);
        BB.Remove(E, V1);
        BB.Add(E, V);
      }
    }
  }

  TopExp::Vertices(E, V1, V2);
  if (V1.IsNull() && V2.IsNull())
  {
    YaCouture = false;
    return;
  }
  MVE.Bound(V1, Empty)->Append(E);
  if (!V1.IsSame(V2))
  {
    MVE.Bound(V2, Empty)->Append(E);
  }
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);
  if (BRep_Tool::IsClosed(E, S, L))
  {
    MVE.ChangeFromKey(V2).Append(E.Reversed());
    if (!V1.IsSame(V2))
    {
      MVE.ChangeFromKey(V1).Append(E.Reversed());
    }
    YaCouture = true;
  }
}

//=================================================================================================

void BRepAlgo_Loop::Perform()
{
  NCollection_List<TopoDS_Shape>::Iterator itl, itl1;
  TopoDS_Vertex                            V1, V2;
  bool                                     YaCouture = false;

#ifdef OCCT_DEBUG_ALGO
  if (AffichLoop)
  {
    std::cout << "NewLoop" << std::endl;
    NbLoops++;
    for (itl.Initialize(myEdges); itl.More(); itl.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(itl.Value());
    }
    for (itl.Initialize(myConstEdges); itl.More(); itl.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(itl.Value());
    }
  }
#endif
  //------------------------------------------------
  // Cut edges
  //------------------------------------------------
  for (itl.Initialize(myEdges); itl.More(); itl.Next())
  {
    const TopoDS_Edge&                    anEdge = TopoDS::Edge(itl.Value());
    NCollection_List<TopoDS_Shape>        LCE;
    const NCollection_List<TopoDS_Shape>* pVertices = myVerOnEdges.Seek(anEdge);
    if (pVertices)
    {
      CutEdge(anEdge, *pVertices, LCE);
      myCutEdges.Bind(anEdge, LCE);
    }
  }
  //-----------------------------------
  // Construction map vertex => edges
  //-----------------------------------
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MVE;

  // add cut edges.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
  for (itl.Initialize(myEdges); itl.More(); itl.Next())
  {
    const NCollection_List<TopoDS_Shape>* pLCE = myCutEdges.Seek(itl.Value());
    if (pLCE)
    {
      for (itl1.Initialize(*pLCE); itl1.More(); itl1.Next())
      {
        TopoDS_Edge& E = TopoDS::Edge(itl1.ChangeValue());
        if (!Emap.Add(E))
          continue;
        StoreInMVE(myFace, E, MVE, YaCouture, myVerticesForSubstitute, myTolConf);
      }
    }
  }

  // add const edges
  // Sewn edges can be doubled or not in myConstEdges
  // => call only once StoreInMVE which should double them
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> DejaVu;
  for (itl.Initialize(myConstEdges); itl.More(); itl.Next())
  {
    TopoDS_Edge& E = TopoDS::Edge(itl.ChangeValue());
    if (DejaVu.Add(E))
      StoreInMVE(myFace, E, MVE, YaCouture, myVerticesForSubstitute, myTolConf);
  }

  //-----------------------------------------------
  // Construction of wires and new faces.
  //----------------------------------------------
  TopoDS_Vertex VF, VL, CV;
  TopoDS_Edge   CE, NE, EF;
  BRep_Builder  B;
  TopoDS_Wire   NW;
  bool          End;

  UpdateVEmap(MVE);

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> UsedEdges;

  while (MVE.Extent() > 0)
  {
    B.MakeWire(NW);
    //--------------------------------
    // Removal of hanging edges.
    //--------------------------------
    RemovePendingEdges(MVE);

    if (MVE.Extent() == 0)
      break;
    //--------------------------------
    // Start edge.
    //--------------------------------
    EF = CE = TopoDS::Edge(MVE(1).First());
    TopExp::Vertices(CE, V1, V2);
    //--------------------------------
    // VF vertex start of new wire
    //--------------------------------
    if (CE.Orientation() == TopAbs_FORWARD)
    {
      CV = VF = V1;
    }
    else
    {
      CV = VF = V2;
    }
    if (!MVE.Contains(CV))
      continue;
    NCollection_List<TopoDS_Shape>& aListEdges = MVE.ChangeFromKey(CV);
    for (itl.Initialize(aListEdges); itl.More(); itl.Next())
    {
      if (itl.Value().IsEqual(CE))
      {
        aListEdges.Remove(itl);
        break;
      }
    }
    End = false;

    while (!End)
    {
      //-------------------------------
      // Construction of a wire.
      //-------------------------------
      TopExp::Vertices(CE, V1, V2);
      if (!CV.IsSame(V1))
        CV = V1;
      else
        CV = V2;

      B.Add(NW, CE);
      UsedEdges.Add(CE);

      if (!MVE.Contains(CV) || MVE.FindFromKey(CV).IsEmpty())
      {
        End = true;
      }
      else
      {
        End = !SelectEdge(myFace, CE, CV, NE, MVE.ChangeFromKey(CV));
        if (!End)
        {
          CE = NE;
          if (MVE.FindFromKey(CV).IsEmpty())
            MVE.RemoveKey(CV);
        }
      }
    }
    //--------------------------------------------------
    // Add new wire to the set of wires
    //------------------------------------------------

    if (VF.IsSame(CV))
    {
      if (SamePnt2d(VF, EF, CE, myFace))
      {
        NW.Closed(true);
        myNewWires.Append(NW);
      }
      else if (BRep_Tool::Tolerance(VF) < myTolConf)
      {
        BRep_Builder aBB;
        aBB.UpdateVertex(VF, myTolConf);
        if (SamePnt2d(VF, EF, CE, myFace))
        {
          NW.Closed(true);
          myNewWires.Append(NW);
        }
#ifdef OCCT_DEBUG_ALGO
        else
        {
          std::cout << "BRepAlgo_Loop: Open Wire" << std::endl;
          if (AffichLoop)
            std::cout << "OpenWire is : NW_" << NbLoops << "_" << NbWires << std::endl;
        }
#endif
      }
    }
#ifdef OCCT_DEBUG_ALGO
    else
    {
      std::cout << "BRepAlgo_Loop: Open Wire" << std::endl;
      if (AffichLoop)
        std::cout << "OpenWire is : NW_" << NbLoops << "_" << NbWires << std::endl;
    }
#endif
  }

  PurgeNewEdges(myCutEdges, UsedEdges);
}

//=================================================================================================

void BRepAlgo_Loop::CutEdge(const TopoDS_Edge&                    E,
                            const NCollection_List<TopoDS_Shape>& VOnE,
                            NCollection_List<TopoDS_Shape>&       NE) const
{
  TopoDS_Shape aLocalE = E.Oriented(TopAbs_FORWARD);
  TopoDS_Edge  WE      = TopoDS::Edge(aLocalE);

  double                                   U1, U2;
  TopoDS_Vertex                            V1, V2;
  NCollection_Sequence<TopoDS_Shape>       SV;
  NCollection_List<TopoDS_Shape>::Iterator it(VOnE);
  BRep_Builder                             B;

  for (; it.More(); it.Next())
  {
    SV.Append(it.Value());
  }
  //--------------------------------
  // Parse vertices on the edge.
  //--------------------------------
  Bubble(WE, SV);

  int NbVer = SV.Length();
  //----------------------------------------------------------------
  // Construction of new edges.
  // Note :  vertices at the extremities of edges are not
  //         onligatorily in the list of vertices
  //----------------------------------------------------------------
  if (SV.IsEmpty())
  {
    NE.Append(E);
    return;
  }
  TopoDS_Vertex VF, VL;
  double        f, l;
  BRep_Tool::Range(WE, f, l);
  TopExp::Vertices(WE, VF, VL);

  if (NbVer == 2)
  {
    if (SV(1).IsEqual(VF) && SV(2).IsEqual(VL))
    {
      NE.Append(E);
      return;
    }
  }
  //----------------------------------------------------
  // Processing of closed edges
  // If a vertex of intersection is on the common vertex
  // it should appear at the beginning and end of SV.
  //----------------------------------------------------
  TopoDS_Vertex VCEI;
  if (!VF.IsNull() && VF.IsSame(VL))
  {
    VCEI = UpdateClosedEdge(WE, SV);
    if (!VCEI.IsNull())
    {
      TopoDS_Shape aLocalV = VCEI.Oriented(TopAbs_FORWARD);
      VF                   = TopoDS::Vertex(aLocalV);
      aLocalV              = VCEI.Oriented(TopAbs_REVERSED);
      VL                   = TopoDS::Vertex(aLocalV);
    }
    SV.Prepend(VF);
    SV.Append(VL);
  }
  else
  {
    //-----------------------------------------
    // Eventually all extremities of the edge.
    //-----------------------------------------
    if (!VF.IsNull() && !VF.IsSame(SV.First()))
      SV.Prepend(VF);
    if (!VL.IsNull() && !VL.IsSame(SV.Last()))
      SV.Append(VL);
  }

  while (!SV.IsEmpty())
  {
    while (!SV.IsEmpty() && SV.First().Orientation() != TopAbs_FORWARD)
    {
      SV.Remove(1);
    }
    if (SV.IsEmpty())
      break;
    V1 = TopoDS::Vertex(SV.First());
    SV.Remove(1);
    if (SV.IsEmpty())
      break;
    if (SV.First().Orientation() == TopAbs_REVERSED)
    {
      V2 = TopoDS::Vertex(SV.First());
      SV.Remove(1);
      //-------------------------------------------
      // Copy the edge and restriction by V1 V2.
      //-------------------------------------------
      TopoDS_Shape NewEdge    = WE.EmptyCopied();
      TopoDS_Shape aLocalEdge = V1.Oriented(TopAbs_FORWARD);
      B.Add(NewEdge, aLocalEdge);
      aLocalEdge = V2.Oriented(TopAbs_REVERSED);
      B.Add(TopoDS::Edge(NewEdge), aLocalEdge);
      if (V1.IsSame(VF))
        U1 = f;
      else
      {
        TopoDS_Shape aLocalV = V1.Oriented(TopAbs_INTERNAL);
        U1                   = BRep_Tool::Parameter(TopoDS::Vertex(aLocalV), WE);
      }
      if (V2.IsSame(VL))
        U2 = l;
      else
      {
        TopoDS_Shape aLocalV = V2.Oriented(TopAbs_INTERNAL);
        U2                   = BRep_Tool::Parameter(TopoDS::Vertex(aLocalV), WE);
      }
      B.Range(TopoDS::Edge(NewEdge), U1, U2);
      NE.Append(NewEdge.Oriented(E.Orientation()));
    }
  }

  // Remove edges with size <= tolerance
  double Tol = 0.001; // 5.e-05; //5.e-07;
  it.Initialize(NE);
  while (it.More())
  {
    // skl : I change "E" to "EE"
    TopoDS_Edge EE = TopoDS::Edge(it.Value());
    double      fpar, lpar;
    BRep_Tool::Range(EE, fpar, lpar);
    if (lpar - fpar <= Precision::Confusion())
      NE.Remove(it);
    else
    {
      gp_Pnt2d pf, pl;
      BRep_Tool::UVPoints(EE, myFace, pf, pl);
      if (pf.Distance(pl) <= Tol && !BRep_Tool::IsClosed(EE))
        NE.Remove(it);
      else
        it.Next();
    }
  }
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepAlgo_Loop::NewWires() const
{
  return myNewWires;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepAlgo_Loop::NewFaces() const
{
  return myNewFaces;
}

//=================================================================================================

void BRepAlgo_Loop::WiresToFaces()
{
  if (!myNewWires.IsEmpty())
  {
    BRepAlgo_FaceRestrictor FR;
    TopoDS_Shape            aLocalS = myFace.Oriented(TopAbs_FORWARD);
    FR.Init(TopoDS::Face(aLocalS), false);
    //    FR.Init (TopoDS::Face(myFace.Oriented(TopAbs_FORWARD)),
    //	     false);
    NCollection_List<TopoDS_Shape>::Iterator it(myNewWires);
    for (; it.More(); it.Next())
    {
      FR.Add(TopoDS::Wire(it.ChangeValue()));
    }

    FR.Perform();

    if (FR.IsDone())
    {
      TopAbs_Orientation OriF = myFace.Orientation();
      for (; FR.More(); FR.Next())
      {
        myNewFaces.Append(FR.Current().Oriented(OriF));
      }
    }
  }
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepAlgo_Loop::NewEdges(const TopoDS_Edge& E) const
{
  return myCutEdges(E);
}

//=================================================================================================

void BRepAlgo_Loop::GetVerticesForSubstitute(
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& VerVerMap) const
{
  VerVerMap = myVerticesForSubstitute;
}

//=================================================================================================

void BRepAlgo_Loop::VerticesForSubstitute(
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& VerVerMap)
{
  myVerticesForSubstitute = VerVerMap;
}

//=================================================================================================

void BRepAlgo_Loop::UpdateVEmap(
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theVEmap)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    VerLver;

  for (int ii = 1; ii <= theVEmap.Extent(); ii++)
  {
    const TopoDS_Vertex&                  aVertex = TopoDS::Vertex(theVEmap.FindKey(ii));
    const NCollection_List<TopoDS_Shape>& aElist  = theVEmap(ii);
    if (aElist.Extent() == 1 && myImageVV.IsImage(aVertex))
    {
      const TopoDS_Vertex& aProVertex = TopoDS::Vertex(myImageVV.ImageFrom(aVertex));
      if (VerLver.Contains(aProVertex))
      {
        NCollection_List<TopoDS_Shape>& aVlist = VerLver.ChangeFromKey(aProVertex);
        aVlist.Append(aVertex.Oriented(TopAbs_FORWARD));
      }
      else
      {
        NCollection_List<TopoDS_Shape> aVlist;
        aVlist.Append(aVertex.Oriented(TopAbs_FORWARD));
        VerLver.Add(aProVertex, aVlist);
      }
    }
  }

  if (VerLver.IsEmpty())
    return;

  BRep_Builder aBB;
  for (int ii = 1; ii <= VerLver.Extent(); ii++)
  {
    const NCollection_List<TopoDS_Shape>& aVlist = VerLver(ii);
    if (aVlist.Extent() == 1)
      continue;

    double                     aMaxTol = 0.;
    NCollection_Array1<gp_Pnt> Points(1, aVlist.Extent());

    NCollection_List<TopoDS_Shape>::Iterator itl(aVlist);
    int                                      jj = 0;
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(itl.Value());
      double               aTol    = BRep_Tool::Tolerance(aVertex);
      aMaxTol                      = std::max(aMaxTol, aTol);
      gp_Pnt aPnt                  = BRep_Tool::Pnt(aVertex);
      Points(++jj)                 = aPnt;
    }

    gp_Ax2 anAxis;
    bool   IsSingular;
    GeomLib::AxeOfInertia(Points, anAxis, IsSingular);
    gp_Pnt aCentre  = anAxis.Location();
    double aMaxDist = 0.;
    for (jj = 1; jj <= Points.Upper(); jj++)
    {
      double aSqDist = aCentre.SquareDistance(Points(jj));
      aMaxDist       = std::max(aMaxDist, aSqDist);
    }
    aMaxDist = std::sqrt(aMaxDist);
    aMaxTol  = std::max(aMaxTol, aMaxDist);

    // Find constant vertex
    TopoDS_Vertex aConstVertex;
    for (itl.Initialize(aVlist); itl.More(); itl.Next())
    {
      const TopoDS_Vertex&                     aVertex = TopoDS::Vertex(itl.Value());
      const NCollection_List<TopoDS_Shape>&    aElist  = theVEmap.FindFromKey(aVertex);
      const TopoDS_Shape&                      anEdge  = aElist.First();
      NCollection_List<TopoDS_Shape>::Iterator itcedges(myConstEdges);
      for (; itcedges.More(); itcedges.Next())
        if (anEdge.IsSame(itcedges.Value()))
        {
          aConstVertex = aVertex;
          break;
        }
      if (!aConstVertex.IsNull())
        break;
    }
    if (aConstVertex.IsNull())
      aConstVertex = TopoDS::Vertex(aVlist.First());
    aBB.UpdateVertex(aConstVertex, aCentre, aMaxTol);

    for (itl.Initialize(aVlist); itl.More(); itl.Next())
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(itl.Value());
      if (aVertex.IsSame(aConstVertex))
        continue;

      const NCollection_List<TopoDS_Shape>& aElist = theVEmap.FindFromKey(aVertex);
      TopoDS_Edge                           anEdge = TopoDS::Edge(aElist.First());
      anEdge.Orientation(TopAbs_FORWARD);
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(anEdge, aV1, aV2);
      TopoDS_Vertex aVertexToRemove = (aV1.IsSame(aVertex)) ? aV1 : aV2;
      anEdge.Free(true);
      aBB.Remove(anEdge, aVertexToRemove);
      aBB.Add(anEdge, aConstVertex.Oriented(aVertexToRemove.Orientation()));
    }
  }

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
  for (int ii = 1; ii <= theVEmap.Extent(); ii++)
  {
    const NCollection_List<TopoDS_Shape>&    aElist = theVEmap(ii);
    NCollection_List<TopoDS_Shape>::Iterator itl(aElist);
    for (; itl.More(); itl.Next())
      Emap.Add(itl.Value());
  }

  theVEmap.Clear();
  for (int ii = 1; ii <= Emap.Extent(); ii++)
    TopExp::MapShapesAndAncestors(Emap(ii), TopAbs_VERTEX, TopAbs_EDGE, theVEmap);
}
