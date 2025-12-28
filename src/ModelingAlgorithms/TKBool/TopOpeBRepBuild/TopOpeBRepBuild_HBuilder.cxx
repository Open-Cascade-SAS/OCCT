// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepBuild_HBuilder, Standard_Transient)

//=================================================================================================

TopOpeBRepBuild_HBuilder::TopOpeBRepBuild_HBuilder(const TopOpeBRepDS_BuildTool& BT)
    : myBuilder(BT),
      myMakeEdgeAncestorIsDone(false),
      myMakeCurveAncestorIsDone(false),
      myMakePointAncestorIsDone(false)
{
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  myBuilder.Perform(HDS);
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                       const TopoDS_Shape&                             S1,
                                       const TopoDS_Shape&                             S2)
{
  myBuilder.Perform(HDS, S1, S2);
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::Clear()
{
  myBuilder.Clear();
  InitExtendedSectionDS();
}

//=================================================================================================

occ::handle<TopOpeBRepDS_HDataStructure> TopOpeBRepBuild_HBuilder::DataStructure() const
{
  return myBuilder.DataStructure();
}

//=================================================================================================

TopOpeBRepDS_BuildTool& TopOpeBRepBuild_HBuilder::ChangeBuildTool()
{
  TopOpeBRepDS_BuildTool& BT = myBuilder.ChangeBuildTool();
  return BT;
}

//=================================================================================================

const TopOpeBRepDS_BuildTool& TopOpeBRepBuild_HBuilder::BuildTool() const
{
  return myBuilder.BuildTool();
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::MergeShapes(const TopoDS_Shape& S1,
                                           const TopAbs_State  ToBuild1,
                                           const TopoDS_Shape& S2,
                                           const TopAbs_State  ToBuild2)
{
  myBuilder.MergeShapes(S1, ToBuild1, S2, ToBuild2);
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::MergeSolids(const TopoDS_Shape& S1,
                                           const TopAbs_State  ToBuild1,
                                           const TopoDS_Shape& S2,
                                           const TopAbs_State  ToBuild2)
{
  myBuilder.MergeSolids(S1, ToBuild1, S2, ToBuild2);
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::MergeSolid(const TopoDS_Shape& S, const TopAbs_State ToBuild)
{
  myBuilder.MergeSolid(S, ToBuild);
}

//=================================================================================================

bool TopOpeBRepBuild_HBuilder::IsSplit(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  bool res = myBuilder.IsSplit(S, ToBuild);
  return res;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_HBuilder::Splits(
  const TopoDS_Shape& S,
  const TopAbs_State  ToBuild) const
{
  const NCollection_List<TopoDS_Shape>& L = myBuilder.Splits(S, ToBuild);
  return L;
}

//=================================================================================================

bool TopOpeBRepBuild_HBuilder::IsMerged(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  bool res = myBuilder.IsMerged(S, ToBuild);
  return res;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_HBuilder::Merged(
  const TopoDS_Shape& S,
  const TopAbs_State  ToBuild) const
{
  const NCollection_List<TopoDS_Shape>& L = myBuilder.Merged(S, ToBuild);
  return L;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_HBuilder::NewVertex(const int I) const
{
  const TopoDS_Shape& V = myBuilder.NewVertex(I);
  return V;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_HBuilder::NewEdges(const int I) const
{
  const NCollection_List<TopoDS_Shape>& L = myBuilder.NewEdges(I);
  return L;
}

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_HBuilder::ChangeNewEdges(const int I)
{
  NCollection_List<TopoDS_Shape>& L = myBuilder.ChangeNewEdges(I);
  return L;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_HBuilder::NewFaces(const int I) const
{
  const NCollection_List<TopoDS_Shape>& L = myBuilder.NewFaces(I);
  return L;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_HBuilder::Section()
{
  const NCollection_List<TopoDS_Shape>& L = myBuilder.Section();
  return L;
}

static NCollection_List<TopoDS_Shape>*           PLE   = NULL;
static NCollection_List<TopoDS_Shape>::Iterator* PITLE = NULL;

//=================================================================================================

void TopOpeBRepBuild_HBuilder::InitExtendedSectionDS(const int k)
{
  if (k == 1)
  {
    myMakeCurveAncestorIsDone = false;
  }
  else if (k == 2)
  {
    myMakeEdgeAncestorIsDone = false;
  }
  else if (k == 3)
  {
    myMakeEdgeAncestorIsDone  = false;
    myMakeCurveAncestorIsDone = false;
  }
  else
    return;
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::InitSection(const int k)
{
  if (PLE == NULL)
    PLE = new NCollection_List<TopoDS_Shape>();
  if (PITLE == NULL)
    PITLE = new NCollection_List<TopoDS_Shape>::Iterator();
  PLE->Clear();
  PITLE->Initialize(*PLE);
  InitExtendedSectionDS(k);
  if (k == 1)
    myBuilder.SectionCurves(*PLE);
  else if (k == 2)
    myBuilder.SectionEdges(*PLE);
  else if (k == 3)
    myBuilder.Section(*PLE);
  else
    return;
  PITLE->Initialize(*PLE);
}

//=================================================================================================

bool TopOpeBRepBuild_HBuilder::MoreSection() const
{
  if (PITLE == NULL)
    return false;
  bool b = PITLE->More();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::NextSection()
{
  if (PITLE == NULL)
    return;
  if (PITLE->More())
    PITLE->Next();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_HBuilder::CurrentSection() const
{
  if (PITLE == NULL)
    throw Standard_ProgramError("no more CurrentSection");
  if (!PITLE->More())
    throw Standard_ProgramError("no more CurrentSection");
  return PITLE->Value();
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::MakeEdgeAncestorMap()
{
  if (myMakeEdgeAncestorIsDone)
    return;
  mySectEdgeDSEdges1.Clear();
  mySectEdgeDSEdges2.Clear();
  myDSEdgesDSFaces1.Clear();
  myDSEdgesDSFaces2.Clear();

  myMakeEdgeAncestorIsDone = true;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MF, ME;

  const TopOpeBRepDS_DataStructure& DS = DataStructure()->DS();
  //  int ns = DS.NbShapes(),is;
  int ns = DS.NbShapes();
  //  int ei, fi, re, rf ,gi, ise;
  int ei, fi, re, rf, gi;

  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>::
    Iterator                               it(myBuilder.mySplitON);
  NCollection_List<TopoDS_Shape>::Iterator its;
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& ShaSpl = it.Key();
    ei                         = DS.Shape(ShaSpl);
    re                         = DS.AncestorRank(ShaSpl);
    if (!re)
      continue;
    TopOpeBRepDS_ListOfShapeOn1State& losos1s = (*(TopOpeBRepDS_ListOfShapeOn1State*)&it.Value());
    NCollection_List<TopoDS_Shape>&   los     = losos1s.ChangeListOnState();
    its.Initialize(los);
    if (re == 1)
      for (; its.More(); its.Next())
      {
        const TopoDS_Shape& SecEdg = its.Value();
        if (!mySectEdgeDSEdges1.IsBound(SecEdg))
          mySectEdgeDSEdges1.Bind(SecEdg, ei);
      }
    else if (re == 2)
      for (; its.More(); its.Next())
      {
        const TopoDS_Shape& SecEdg = its.Value();
        if (!mySectEdgeDSEdges2.IsBound(SecEdg))
          mySectEdgeDSEdges2.Bind(SecEdg, ei);
      }
  }

  //  bool gb;
  TopOpeBRepDS_Kind gk;
  for (fi = 1; fi <= ns; fi++)
  {
    const TopoDS_Shape& fds = DS.Shape(fi);
    if (fds.IsNull())
      continue;
    if (fds.ShapeType() != TopAbs_FACE)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(
      DS.ShapeInterferences(fds));
    for (; it1.More(); it1.Next())
    {
      occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI =
        occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(it1.Value());
      if (SSI.IsNull())
        continue;
      gk = SSI->GeometryType();
      gi = SSI->Geometry();
      if (gk != TopOpeBRepDS_EDGE)
        continue;
      rf = DS.AncestorRank(fds);
      if (!rf)
        continue;
      //      if (!MF.Contains(fds) ) {
      //	MF.Add(fds);
      if (rf == 1)
      {
        if (!myDSEdgesDSFaces1.IsBound(gi))
        {
          NCollection_List<int> thelist;
          myDSEdgesDSFaces1.Bind(gi, thelist);
        }
        myDSEdgesDSFaces1.ChangeFind(gi).Append(fi);
      }
      else if (rf == 2)
      {
        if (!myDSEdgesDSFaces2.IsBound(gi))
        {
          NCollection_List<int> thelist1;
          myDSEdgesDSFaces2.Bind(gi, thelist1);
        }
        myDSEdgesDSFaces2.ChangeFind(gi).Append(fi);
      }
      //      }
    }
  }
}

//=================================================================================================

int TopOpeBRepBuild_HBuilder::GetDSEdgeFromSectEdge(const TopoDS_Shape& E, const int rank)
{
  if (!myMakeEdgeAncestorIsDone)
    MakeEdgeAncestorMap();

  int i = 0;

  if (rank == 1)
  {
    if (mySectEdgeDSEdges1.IsBound(E))
      i = mySectEdgeDSEdges1.Find(E);
  }

  if (rank == 2)
  {
    if (mySectEdgeDSEdges2.IsBound(E))
      i = mySectEdgeDSEdges2.Find(E);
  }
  return i;
}

//=================================================================================================

NCollection_List<int>& TopOpeBRepBuild_HBuilder::GetDSFaceFromDSEdge(const int indexEdg,
                                                                     const int rank)
{
  if (!myMakeEdgeAncestorIsDone)
    MakeEdgeAncestorMap();

  if (rank == 1)
  {
    if (myDSEdgesDSFaces1.IsBound(indexEdg))
    {
      NCollection_List<int>& loi = myDSEdgesDSFaces1.ChangeFind(indexEdg);
      return loi;
    }
  }

  if (rank == 2)
  {
    if (myDSEdgesDSFaces2.IsBound(indexEdg))
    {
      NCollection_List<int>& loi = myDSEdgesDSFaces2.ChangeFind(indexEdg);
      return loi;
    }
  }

  return myEmptyIntegerList;
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::MakeCurveAncestorMap()
{
  if (myMakeCurveAncestorIsDone)
    return;
  mySectEdgeDSCurve.Clear();
  myMakeCurveAncestorIsDone                   = true;
  const TopOpeBRepDS_DataStructure&        DS = DataStructure()->DS();
  NCollection_List<TopoDS_Shape>::Iterator itloe;
  TopOpeBRepDS_CurveExplorer               cex(DS, true);
  //  int ic, icm;
  int ic;
  for (; cex.More(); cex.Next())
  {
    ic                                        = cex.Index();
    const NCollection_List<TopoDS_Shape>& LOS = myBuilder.myNewEdges.Find(ic);
    itloe.Initialize(LOS);
    for (; itloe.More(); itloe.Next())
    {
      TopoDS_Shape& E = *((TopoDS_Shape*)(&itloe.Value()));
      mySectEdgeDSCurve.Bind(E, ic);
    }
  }
}

//=================================================================================================

int TopOpeBRepBuild_HBuilder::GetDSCurveFromSectEdge(const TopoDS_Shape& SectEdge)
{
  int i = 0;
  if (!myMakeCurveAncestorIsDone)
    MakeCurveAncestorMap();

  if (mySectEdgeDSCurve.IsBound(SectEdge))
  {
    i = mySectEdgeDSCurve.Find(SectEdge);
  }
  return i;
}

//=================================================================================================

int TopOpeBRepBuild_HBuilder::GetDSFaceFromDSCurve(const int indexCur, const int rank)
{
  int i = 0;
  if (!myMakeCurveAncestorIsDone)
    MakeCurveAncestorMap();

  const TopOpeBRepDS_DataStructure& DS = DataStructure()->DS();
  if (rank == 1)
  {
    const TopOpeBRepDS_Curve& DSC = DS.Curve(indexCur);
    const TopoDS_Shape&       F1  = DSC.Shape1();
    i                             = DS.Shape(F1);
  }

  if (rank == 2)
  {
    const TopOpeBRepDS_Curve& DSC = DS.Curve(indexCur);
    const TopoDS_Shape&       F2  = DSC.Shape2();
    i                             = DS.Shape(F2);
  }

  return i;
}

//=================================================================================================

int TopOpeBRepBuild_HBuilder::GetDSPointFromNewVertex(const TopoDS_Shape& NewVert)
{
  if (!myMakePointAncestorIsDone)
  {
    myMakePointAncestorIsDone      = true;
    TopOpeBRepDS_DataStructure& DS = DataStructure()->ChangeDS();
    int                         i, NbPoint = DS.NbPoints();
    for (i = 1; i <= NbPoint; i++)
    {
      const TopoDS_Shape& Vertex = NewVertex(i);
      if (!Vertex.IsNull())
        myNewVertexDSPoint.Bind(Vertex, i);
    }
  }

  int iPnt = 0;
  if (myNewVertexDSPoint.IsBound(NewVert))
    iPnt = myNewVertexDSPoint.Find(NewVert);
  return iPnt;
}

//=================================================================================================

bool TopOpeBRepBuild_HBuilder::EdgeCurveAncestors(const TopoDS_Shape& E,
                                                  TopoDS_Shape&       F1,
                                                  TopoDS_Shape&       F2,
                                                  int&                IC)
{
  F1.Nullify();
  F2.Nullify();
  IC = 0;

  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS = myBuilder.DataStructure();
  const TopOpeBRepDS_DataStructure&               DS  = HDS->DS();

  //  NCollection_List<TopoDS_Shape>::Iterator itloe;
  IC = GetDSCurveFromSectEdge(E);
  if (!IC)
    return false;

  int iF1, iF2;
  iF1 = GetDSFaceFromDSCurve(IC, 1);
  iF2 = GetDSFaceFromDSCurve(IC, 2);

  F1 = DS.Shape(iF1);
  F2 = DS.Shape(iF2);
  return true;
}

//=================================================================================================

bool TopOpeBRepBuild_HBuilder::EdgeSectionAncestors(const TopoDS_Shape&             E,
                                                    NCollection_List<TopoDS_Shape>& LF1,
                                                    NCollection_List<TopoDS_Shape>& LF2,
                                                    NCollection_List<TopoDS_Shape>& LE1,
                                                    NCollection_List<TopoDS_Shape>& LE2)
{
  if (E.ShapeType() != TopAbs_EDGE)
    return false;

  LF1.Clear();
  LF2.Clear();
  LE1.Clear();
  LE2.Clear();
  NCollection_List<int> f1, f2;
  f1.Clear();
  f2.Clear();
  int ie1, ie2, curr;

  ie1 = GetDSEdgeFromSectEdge(E, 1);
  ie2 = GetDSEdgeFromSectEdge(E, 2);
  NCollection_List<int>::Iterator it;

  if (ie1 && ie2)
  {
    NCollection_Map<int> moi;

    f1 = GetDSFaceFromDSEdge(ie1, 1);
    it.Initialize(f1);
    moi.Clear();
    for (; it.More(); it.Next())
    {
      moi.Add(it.Value());
    }
    it.Initialize(GetDSFaceFromDSEdge(ie2, 1));
    for (; it.More(); it.Next())
    {
      curr = it.Value();
      if (!moi.Contains(curr))
      {
        moi.Add(curr);
        f1.Append(curr);
      }
    }
    f2 = GetDSFaceFromDSEdge(ie1, 2);
    it.Initialize(f2);
    moi.Clear();
    for (; it.More(); it.Next())
    {
      moi.Add(it.Value());
    }
    it.Initialize(GetDSFaceFromDSEdge(ie2, 2));
    for (; it.More(); it.Next())
    {
      curr = it.Value();
      if (!moi.Contains(curr))
      {
        moi.Add(curr);
        f2.Append(curr);
      }
    }
  }
  else
  {
    if (ie1)
    {
      f1 = GetDSFaceFromDSEdge(ie1, 1);
      f2 = GetDSFaceFromDSEdge(ie1, 2);
    }
    else if (ie2)
    {
      f1 = GetDSFaceFromDSEdge(ie2, 1);
      f2 = GetDSFaceFromDSEdge(ie2, 2);
    }
  }

  const TopOpeBRepDS_DataStructure& DS = myBuilder.DataStructure()->DS();

  if (ie1)
    LE1.Append(DS.Shape(ie1));
  if (ie2)
    LE2.Append(DS.Shape(ie2));

  for (it.Initialize(f1); it.More(); it.Next())
  {
    curr = it.Value();
    LF1.Append(DS.Shape(curr));
  }
  for (it.Initialize(f2); it.More(); it.Next())
  {
    curr = it.Value();
    LF2.Append(DS.Shape(curr));
  }

  bool r = (!LF1.IsEmpty() && !LF2.IsEmpty());
  r      = r && (!LE1.IsEmpty() || !LE2.IsEmpty());
  return r;
}

//=================================================================================================

int TopOpeBRepBuild_HBuilder::IsKPart()
{
  int kp = myBuilder.IsKPart();
  return kp;
}

//=================================================================================================

void TopOpeBRepBuild_HBuilder::MergeKPart(const TopAbs_State TB1, const TopAbs_State TB2)
{
  int kp = IsKPart();
  if (kp)
    myBuilder.MergeKPart(TB1, TB2);
}

//=================================================================================================

TopOpeBRepBuild_Builder& TopOpeBRepBuild_HBuilder::ChangeBuilder()
{
  return myBuilder;
}
