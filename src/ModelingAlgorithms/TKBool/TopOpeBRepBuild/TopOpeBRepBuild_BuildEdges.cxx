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

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceCU();
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::BuildEdges(const int                                       iC,
                                         const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceCU())
    std::cout << "\nBuildEdges on C " << iC << std::endl;
#endif
  const TopOpeBRepDS_Curve&                     C   = HDS->Curve(iC);
  const occ::handle<Geom_Curve>&                C3D = C.Curve();
  const occ::handle<TopOpeBRepDS_Interference>& I1  = C.GetSCI1();
  const occ::handle<TopOpeBRepDS_Interference>& I2  = C.GetSCI2();
  bool                                          nnn = C3D.IsNull() && I1.IsNull() && I2.IsNull();
  if (nnn)
  {
    return;
  }

  TopoDS_Shape              anEdge;
  const TopOpeBRepDS_Curve& curC = HDS->Curve(iC);
  myBuildTool.MakeEdge(anEdge, curC, HDS->DS());
  TopOpeBRepBuild_PaveSet    PVS(anEdge);
  TopOpeBRepDS_PointIterator CPIT(HDS->CurvePoints(iC));
  FillVertexSet(CPIT, TopAbs_IN, PVS);
  TopOpeBRepBuild_PaveClassifier VCL(anEdge);
  bool                           equalpar = PVS.HasEqualParameters();
  if (equalpar)
    VCL.SetFirstParameter(PVS.EqualParameters());
  bool closvert = PVS.ClosedVertices();
  VCL.ClosedVertices(closvert);
  PVS.InitLoop();
  if (!PVS.MoreLoop())
  {
    return;
  }
  TopOpeBRepBuild_EdgeBuilder     EDBU(PVS, VCL);
  NCollection_List<TopoDS_Shape>& EL = ChangeNewEdges(iC);
  MakeEdges(anEdge, EDBU, EL);
  NCollection_List<TopoDS_Shape>::Iterator It(EL);
  int                                      inewC = -1;
  for (; It.More(); It.Next())
  {
    TopoDS_Edge& newEdge = TopoDS::Edge(It.ChangeValue());
    myBuildTool.RecomputeCurves(curC, TopoDS::Edge(anEdge), newEdge, inewC, HDS);
    if (inewC != -1)
      ChangeNewEdges(inewC).Append(newEdge);
  }
  if (inewC != -1)
  {
    HDS->RemoveCurve(iC);
  }
  else
  {
    for (It.Initialize(EL); It.More(); It.Next())
    {
      TopoDS_Edge& newEdge = TopoDS::Edge(It.ChangeValue());
      myBuildTool.UpdateEdge(anEdge, newEdge);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::BuildEdges(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();

  myNewEdges.Clear();
  TopOpeBRepDS_CurveExplorer cex;

  int ick = 0;
  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    int  ic = cex.Index();
    bool ck = cex.IsCurveKeep(ic);
    int  im = cex.Curve(ic).Mother();
    if (ck == 1 && im != 0 && ick == 0)
    {
      ick = ic;
      break;
    }
  }
  if (ick)
  {
    for (cex.Init(BDS, true); cex.More(); cex.Next())
    {
      int ic = cex.Index();
      BDS.RemoveCurve(ic);
    }
    BDS.ChangeNbCurves(ick - 1);
  }

  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    int ic = cex.Index();
    int im = cex.Curve(ic).Mother();
    if (im != 0)
      continue;
    BuildEdges(ic, HDS);
  }

  int                      ip, np = HDS->NbPoints();
  NCollection_HArray1<int> tp(0, np, 0);
  for (cex.Init(BDS); cex.More(); cex.Next())
  {
#ifdef OCCT_DEBUG
//    const TopOpeBRepDS_Curve& C = cex.Curve();
#endif
    int                                                                ic = cex.Index();
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(
      BDS.CurveInterferences(ic));
    for (; it.More(); it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      {
        int               ig = I->Geometry();
        TopOpeBRepDS_Kind kg = I->GeometryType();
        if (kg == TopOpeBRepDS_POINT && ig <= np)
          tp.ChangeValue(ig) = tp.Value(ig) + 1;
      }
      {
        int               is = I->Support();
        TopOpeBRepDS_Kind ks = I->SupportType();
        if (ks == TopOpeBRepDS_POINT)
          tp.ChangeValue(is) = tp.Value(is) + 1;
      }
    }
  }
  int is, ns = BDS.NbShapes();
  for (is = 1; is <= ns; is++)
  {
    const TopoDS_Shape& S = BDS.Shape(is);
    if (S.IsNull())
      continue;
    bool test = (S.ShapeType() == TopAbs_EDGE);
    if (!test)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(
      BDS.ShapeInterferences(is));
    for (; it.More(); it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      {
        int               ig = I->Geometry();
        TopOpeBRepDS_Kind kg = I->GeometryType();
        if (kg == TopOpeBRepDS_POINT)
          tp.ChangeValue(ig) = tp.Value(ig) + 1;
      }
      {
        int               is1 = I->Support();
        TopOpeBRepDS_Kind ks  = I->SupportType();
        if (ks == TopOpeBRepDS_POINT)
          tp.ChangeValue(is1) = tp.Value(is1) + 1;
      }
    }
  }
  for (ip = 1; ip <= np; ip++)
    if (tp.Value(ip) == 0)
      BDS.RemovePoint(ip);
}
