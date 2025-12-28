// Created on: 1998-08-21
// Created by: Yves FRICAUD
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_GapTool.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_GapTool, Standard_Transient)

//=================================================================================================

TopOpeBRepDS_GapTool::TopOpeBRepDS_GapTool() {}

//=================================================================================================

TopOpeBRepDS_GapTool::TopOpeBRepDS_GapTool(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  Init(HDS);
}

//=================================================================================================

static void StoreGToI(
  NCollection_DataMap<int, NCollection_List<occ::handle<TopOpeBRepDS_Interference>>>& GToI,
  const occ::handle<TopOpeBRepDS_Interference>&                                       I)
{
  int G = I->Geometry();
  if (!GToI.IsBound(G))
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> empty;
    GToI.Bind(G, empty);
  }
  GToI(G).Append(I);
}

//=================================================================================================

void TopOpeBRepDS_GapTool::Init(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  myHDS = HDS;
  int i, Nb = myHDS->NbShapes();
  for (i = 1; i <= Nb; i++)
  {
    const TopoDS_Shape&                                             S = myHDS->Shape(i);
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI =
      myHDS->DS().ShapeInterferences(S);
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      if (it.Value()->GeometryType() == TopOpeBRepDS_POINT)
      {
        myInterToShape.Bind(it.Value(), S);
        StoreGToI(myGToI, it.Value());
      }
    }
  }
  int NbCurves = myHDS->NbCurves();
  for (i = 1; i <= NbCurves; i++)
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI =
      myHDS->ChangeDS().ChangeCurveInterferences(i);
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      if (it.Value()->GeometryType() == TopOpeBRepDS_POINT)
        StoreGToI(myGToI, it.Value());
    }
  }
}

//=================================================================================================

bool TopOpeBRepDS_GapTool::Curve(const occ::handle<TopOpeBRepDS_Interference>& I,
                                 TopOpeBRepDS_Curve&                           C) const
{
  if (I->GeometryType() == TopOpeBRepDS_POINT)
  {
    TopOpeBRepDS_Kind GK, SK;
    int               G, S;

    I->GKGSKS(GK, G, SK, S);
    if (SK == TopOpeBRepDS_CURVE)
    {
      C = myHDS->Curve(S);
      return 1;
    }
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = myGToI(G);
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      it.Value()->GKGSKS(GK, G, SK, S);
      if (SK == TopOpeBRepDS_CURVE)
      {
        C = myHDS->Curve(S);
        return 1;
      }
    }
  }
  return 0;
}

//=================================================================================================

const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_GapTool::Interferences(
  const int IP) const
{
  return myGToI(IP);
}

//=================================================================================================

const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_GapTool::
  SameInterferences(const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  return myGToI(I->Geometry());
}

//=================================================================================================

NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_GapTool::
  ChangeSameInterferences(const occ::handle<TopOpeBRepDS_Interference>& I)
{
  return myGToI.ChangeFind(I->Geometry());
}

//=================================================================================================

bool TopOpeBRepDS_GapTool::EdgeSupport(const occ::handle<TopOpeBRepDS_Interference>& I,
                                       TopoDS_Shape&                                 E) const
{
  if (I->GeometryType() == TopOpeBRepDS_POINT)
  {
    if (myInterToShape.IsBound(I))
    {
      const TopoDS_Shape& S = myInterToShape(I);
      if (S.ShapeType() == TopAbs_EDGE)
      {
        E = S;
        return 1;
      }
    }
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = myGToI(I->Geometry());
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& II = it.Value();
      if (myInterToShape.IsBound(II))
      {
        const TopoDS_Shape& S = myInterToShape(II);
        if (S.ShapeType() == TopAbs_EDGE)
        {
          E = S;
          return 1;
        }
      }
    }
  }
  return 0;
}

//=================================================================================================

bool TopOpeBRepDS_GapTool::FacesSupport(const occ::handle<TopOpeBRepDS_Interference>& I,
                                        TopoDS_Shape&                                 F1,
                                        TopoDS_Shape&                                 F2) const
{
  TopOpeBRepDS_Curve C;
  if (Curve(I, C))
  {
    C.GetShapes(F1, F2);
    return 1;
  }
  return 0;
}

//=================================================================================================

bool TopOpeBRepDS_GapTool::ParameterOnEdge(const occ::handle<TopOpeBRepDS_Interference>& I,
                                           const TopoDS_Shape&                           E,
                                           double&                                       U) const
{
  if (I->GeometryType() == TopOpeBRepDS_POINT)
  {
    if (myInterToShape.IsBound(I))
    {
      const TopoDS_Shape& S = myInterToShape(I);
      if (S.IsSame(E))
      {
        U = occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I)->Parameter();
        return 1;
      }
    }
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = myGToI(I->Geometry());
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& II = it.Value();
      if (myInterToShape.IsBound(II))
      {
        const TopoDS_Shape& S = myInterToShape(II);
        if (S.IsSame(E))
        {
          U = occ::down_cast<TopOpeBRepDS_CurvePointInterference>(II)->Parameter();
          return 1;
        }
      }
    }
  }
  return 0;
}

//=================================================================================================

void TopOpeBRepDS_GapTool::SetParameterOnEdge(const occ::handle<TopOpeBRepDS_Interference>& I,
                                              const TopoDS_Shape&                           E,
                                              const double                                  U)
{
  if (I->GeometryType() == TopOpeBRepDS_POINT)
  {
    if (myInterToShape.IsBound(I))
    {
      const TopoDS_Shape& S = myInterToShape(I);
      if (S.IsSame(E))
      {
        occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I)->Parameter(U);
      }
    }
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = myGToI(I->Geometry());
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& II = it.Value();
      if (myInterToShape.IsBound(II))
      {
        const TopoDS_Shape& S = myInterToShape(II);
        if (S.IsSame(E))
        {
          occ::down_cast<TopOpeBRepDS_CurvePointInterference>(II)->Parameter(U);
        }
      }
    }
  }
}

//=================================================================================================

void TopOpeBRepDS_GapTool::SetPoint(const occ::handle<TopOpeBRepDS_Interference>& I, const int IP)
{
  if (IP != I->Geometry())
  {
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI = myGToI(I->Geometry());
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
         it.Next())
    {
      occ::handle<TopOpeBRepDS_Interference> II = it.Value();
      II->Geometry(IP);
      StoreGToI(myGToI, II);
    }
  }
}
