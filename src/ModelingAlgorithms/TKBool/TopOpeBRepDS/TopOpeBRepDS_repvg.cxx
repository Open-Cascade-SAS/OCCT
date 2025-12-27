// Created on: 1998-02-14
// Created by: Jean Yves LEBEY
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

#include <TopOpeBRepDS_repvg.hxx>
#include <TopoDS.hxx>

#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>

#include <TopOpeBRepDS_EdgeInterferenceTool.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_define.hxx>

#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#define MDSdmoiloi NCollection_DataMap<int, NCollection_List<occ::handle<TopOpeBRepDS_Interference>>>
#define MDSdmiodmoiloi NCollection_DataMap<int, NCollection_List<occ::handle<TopOpeBRepDS_Interference>>>::Iterator

//------------------------------------------------------
Standard_EXPORT void FDS_repvg2(const TopOpeBRepDS_DataStructure& BDS,
                                const int            EIX,
                                const TopOpeBRepDS_Kind           GT,
                                NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&  LI,
                                NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&  RLI)
//------------------------------------------------------
{
  const TopoDS_Edge& E    = TopoDS::Edge(BDS.Shape(EIX));
  bool   isEd = BRep_Tool::Degenerated(E);
  if (isEd)
    return;

  bool ispoint  = (GT == TopOpeBRepDS_POINT);
  bool isvertex = (GT == TopOpeBRepDS_VERTEX);

  int nLI = LI.Extent();

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(LI);
  while (it1.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I1 = it1.Value();
    TopOpeBRepDS_Kind                        GT1, ST1;
    int                         G1, S1;
    TopAbs_ShapeEnum                         tsb1, tsa1;
    int                         isb1, isa1;
    FDS_Idata(I1, tsb1, isb1, tsa1, isa1, GT1, G1, ST1, S1);
    if (tsb1 != TopAbs_FACE)
    {
      it1.Next();
      continue;
    }
    if (tsa1 != TopAbs_FACE)
    {
      it1.Next();
      continue;
    }
    if (GT1 != GT)
    {
      it1.Next();
      continue;
    }

    // xpu121198 : raise in PRO16303, if G1 is a vertex same domain, make sure
    // rk(G1)= rk(E1)
    int rkG1 = BDS.AncestorRank(G1);
    int rkS1 = BDS.AncestorRank(S1);
    if (isvertex && (rkG1 != rkS1))
    {
      TopoDS_Shape     oovG;
      bool issd = FUN_ds_getoov(BDS.Shape(G1), BDS, oovG);
      if (!issd)
      {
        it1.Next();
        continue;
      } //!!NYIRAISE
      G1 = BDS.Shape(oovG);
    }

    const TopoDS_Edge& E1 = TopoDS::Edge(BDS.Shape(S1));
    const TopoDS_Face& F1 = TopoDS::Face(BDS.Shape(isb1));
    TopOpeBRepDS_Point PDS;
    TopoDS_Shape       VDS;
    if (ispoint)
      PDS = BDS.Point(G1);
    else if (isvertex)
      VDS = BDS.Shape(G1);
    else
      throw Standard_Failure("TopOpeBRepDS FDS_repvg2 1");

    bool isEd1 = BRep_Tool::Degenerated(E1);
    if (isEd1)
    {
      it1.Next();
      continue;
    }
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
    if (it2.More())
      it2.Next();
    else
    {
      it1.Next();
      continue;
    }

    TopOpeBRepDS_EdgeInterferenceTool EITool;
    bool                  memeS = false;
    TopOpeBRepDS_Transition           TrmemeS;
    bool                  isComplex = false;

    while (it2.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
      TopOpeBRepDS_Kind                        GT2, ST2;
      int                         G2, S2;
      TopAbs_ShapeEnum                         tsb2, tsa2;
      int                         isb2, isa2;
      FDS_Idata(I2, tsb2, isb2, tsa2, isa2, GT2, G2, ST2, S2);
      if (tsb2 != TopAbs_FACE)
      {
        it2.Next();
        continue;
      }
      if (tsa2 != TopAbs_FACE)
      {
        it2.Next();
        continue;
      }

      bool cond = (G1 == G2);
      if (!cond)
      {
        it2.Next();
        continue;
      }

      const TopoDS_Edge& E2    = TopoDS::Edge(BDS.Shape(S2));
      bool   isEd2 = BRep_Tool::Degenerated(E2);
      if (isEd2)
      {
        it2.Next();
        continue;
      }

      memeS = (S1 == S2);
      memeS = memeS && (nLI == 2);

      if (!isComplex && memeS)
      {
        double    pE = FDS_Parameter(I1);
        bool ok = FDS_stateEwithF2d(BDS, E, pE, GT1, G1, F1, TrmemeS);
        if (!ok)
        {
          it2.Next();
          continue;
        }

        LI.Remove(it2);
      } // !isComplex && memeS

      if (!isComplex && !memeS)
      {
        isComplex = true;
        EITool.Init(E, I1);
        if (ispoint)
          EITool.Add(E, PDS, I1);
        else if (isvertex)
          EITool.Add(E1, VDS, I1);
      } // !isComplex && !memeS

      if (isComplex && !memeS)
      {
        if (ispoint)
          EITool.Add(E, PDS, I2);
        else if (isvertex)
          EITool.Add(E2, VDS, I2);
        LI.Remove(it2);
      } // (isComplex && !memeS)

      if (isComplex && memeS)
      {
        it2.Next();
      } // (isComplex && memeS)

    } // it2

    if (!isComplex && memeS)
    {
      const TopOpeBRepDS_Transition& T1 = I1->Transition();
      TrmemeS.Index(T1.Index());
      I1->ChangeTransition() = TrmemeS;
      RLI.Append(I1);
      LI.Remove(it1);
    }
    else if (isComplex && !memeS)
    {
      EITool.Transition(I1);
      RLI.Append(I1);
      LI.Remove(it1);
    }
    else
    {
      it1.Next();
    }

  } // it1

} // FDS_repvg2

//------------------------------------------------------
Standard_EXPORT void FDS_repvg(const TopOpeBRepDS_DataStructure& BDS,
                               const int            EIX,
                               const TopOpeBRepDS_Kind           GT,
                               NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&  LOI,
                               NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&  RLOI)
//------------------------------------------------------
{
  TopOpeBRepDS_TKI tki;
  tki.FillOnGeometry(LOI);

  // xpu211098 : cto904F6 (e10,FTRA1=f14,FTRA2=f17)
  MDSdmoiloi                                    mapITRASHA;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LOI);
  while (it.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I     = it.Value();
    int                         isa   = I->Transition().Index();
    bool                         bound = mapITRASHA.IsBound(isa);
    if (!bound)
    {
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> loi;
      loi.Append(I);
      mapITRASHA.Bind(isa, loi);
    }
    else
      mapITRASHA.ChangeFind(isa).Append(I);
    it.Next();
  }

  LOI.Clear();
  MDSdmiodmoiloi itm(mapITRASHA);
  for (; itm.More(); itm.Next())
  {
    int                 isa  = itm.Key();
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi  = mapITRASHA.ChangeFind(isa);
    int                 nloi = loi.Extent();
    if (nloi < 2)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> rloi;
    FDS_repvg2(BDS, EIX, GT, loi, rloi);
    LOI.Append(loi);
    RLOI.Append(rloi);
  }

  /*LOI.Clear();
  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; int G; tki.Value(K,G);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.ChangeValue(K,G); NCollection_List<occ::handle<TopOpeBRepDS_Interference>>
  Rloi; int nloi = loi.Extent(); if      (nloi == 0) continue; else if (nloi == 1)
  LOI.Append(loi); else { FDS_repvg2(BDS,EIX,GT,loi,Rloi); LOI.Append(loi); RLOI.Append(Rloi);
    }
  }*/
}
