// Created on: 1997-02-14
// Created by: Jean Yves LEBEY
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

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <gp_Vec.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepTools.hxx>

#include <TopOpeBRepTool_GEOMETRY.hxx>

#include <TopOpeBRepTool_PROJECT.hxx>

#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepDS_FaceInterferenceTool.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>

Standard_EXPORT void FUN_UNKFstasta(const TopoDS_Face&              FF,
                                    const TopoDS_Face&              FS,
                                    const TopoDS_Edge&              EE,
                                    const bool          EEofFF,
                                    TopAbs_State&                   stateb,
                                    TopAbs_State&                   statea,
                                    TopOpeBRepTool_PShapeClassifier pClassif);

#define MDShfei occ::handle<TopOpeBRepDS_FaceEdgeInterference>
#define MAKEFEI(IJKLM) (occ::down_cast<TopOpeBRepDS_FaceEdgeInterference>(IJKLM))

Standard_EXPORT bool FUN_Parameters(const gp_Pnt&       Pnt,
                                                const TopoDS_Shape& F,
                                                double&      u,
                                                double&      v);
Standard_EXPORT bool FUN_Parameters(const double& Param,
                                                const TopoDS_Shape&  E,
                                                const TopoDS_Shape&  F,
                                                double&       u,
                                                double&       v);

//=======================================================================
// 3D
// purpose : The compute of transition face <F> /face <FS>,
//           or edge <E>
// prequesitory : <E> is IN 2dmatter(F) and IN 2dmatter(FS)
//=======================================================================

Standard_EXPORT bool FUN_mkTonF(const TopoDS_Face&       F,
                                            const TopoDS_Face&       FS,
                                            const TopoDS_Edge&       E,
                                            TopOpeBRepDS_Transition& T)
{
  bool isdgE = BRep_Tool::Degenerated(E);
  if (isdgE)
    return false;
  T.Set(TopAbs_UNKNOWN, TopAbs_UNKNOWN);

  double tola = 1.e-6; // nyitol
  double f, l;
  FUN_tool_bounds(E, f, l);
  const double PAR_T = 0.456789;
  double       pmil  = (1. - PAR_T) * f + PAR_T * l;
  gp_Vec              tgE;
  bool    ok;

  ok = TopOpeBRepTool_TOOL::TggeomE(pmil, E, tgE);
  // modified by NIZNHY-PKV Fri Aug  4 10:59:44 2000 f
  if (!ok)
  {
    return false;
  }
  // modified by NIZNHY-PKV Fri Aug  4 10:59:48 2000 t

  gp_Pnt2d uvF;
  ok = FUN_tool_parF(E, pmil, F, uvF);
  if (!ok)
    return false;

  gp_Pnt2d uvFS;
  ok = FUN_tool_parF(E, pmil, FS, uvFS);
  if (!ok)
    return false;

  gp_Dir           ngF = FUN_tool_nggeomF(uvF, F);
  double    xx  = std::abs(ngF.Dot(tgE));
  bool tgt = (std::abs(1 - xx) < tola);
  if (tgt)
    return false;

  gp_Dir ntFS;
  ok = TopOpeBRepTool_TOOL::Nt(uvFS, FS, ntFS);
  if (!ok)
    return false;
  gp_Dir           beafter = ngF ^ tgE;
  double    yy      = beafter.Dot(ntFS);
  bool unk     = (std::abs(yy) < tola);
  if (unk)
    return false;

  if (yy < 0.)
    T.Set(TopAbs_FORWARD);
  else
    T.Set(TopAbs_REVERSED);
  return true;
}

//------------------------------------------------------
// FUN_edgeofface :  True si le edge E est un edge de F
Standard_EXPORT bool FUN_edgeofface
  //------------------------------------------------------
  (const TopoDS_Shape& E, const TopoDS_Shape& F)
{
  bool isv = false;
  TopExp_Explorer  ex;
  for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next())
    //  for (TopExp_Explorer ex(F,TopAbs_EDGE); ex.More(); ex.Next())
    if (ex.Current().IsSame(E))
    {
      isv = true;
      break;
    }
  return isv;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_keepFinterference
  //------------------------------------------------------
  (const TopOpeBRepDS_DataStructure&        DS,
   const occ::handle<TopOpeBRepDS_Interference>& I,
   const TopoDS_Shape&                      F)
{
  TopOpeBRepDS_Kind GT1, ST1;
  int  G1, S1;
  FDS_data(I, GT1, G1, ST1, S1);

  bool res = true;
  if (I->IsKind(STANDARD_TYPE(TopOpeBRepDS_FaceEdgeInterference)))
  {

    const TopoDS_Shape& EG = DS.Shape(I->Geometry());
    // I rejetee si son edge-geometrie est une arete de la face qui accede I.
    bool k3 = !::FUN_edgeofface(EG, F);
    res                 = res && k3;
  }

  return res;
}

//------------------------------------------------------
Standard_EXPORT void FUN_unkeepFdoubleGBoundinterferences
  //------------------------------------------------------
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
   const TopOpeBRepDS_DataStructure& /*BDS*/,
   const int)
{
  //                 BDS.Shape(SIX);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1;

  // process interferences of LI with VERTEX geometry

  it1.Initialize(LI);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference>& I1 = it1.ChangeValue();
    TopOpeBRepDS_Kind                  GT1, ST1;
    int                   G1, S1;
    const TopOpeBRepDS_Transition&     T1     = I1->Transition();
    bool                   isunk1 = T1.IsUnknown();
    if (isunk1)
    {
      it1.Next();
      continue;
    }

    FDS_data(I1, GT1, G1, ST1, S1);
    occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI1 =
      occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I1);
    if (SSI1.IsNull())
    {
      it1.Next();
      continue;
    }

    bool isB1 = SSI1->GBound();

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
    it2.Next();
    bool cond1 = false;
    bool cond2 = false;

    while (it2.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
      TopOpeBRepDS_Kind                        GT2, ST2;
      int                         G2, S2;
      const TopOpeBRepDS_Transition&           T2     = I2->Transition();
      bool                         isunk2 = T2.IsUnknown();
      if (isunk2)
      {
        it2.Next();
        continue;
      }

      FDS_data(I2, GT2, G2, ST2, S2);
      occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI2 =
        occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I2);
      if (SSI2.IsNull())
      {
        it2.Next();
        continue;
      }

      bool isB2 = SSI2->GBound();
      cond2                 = (GT2 == GT1 && GT1 == TopOpeBRepDS_EDGE && G2 == G1 && ST2 == ST1
               && ST1 == TopOpeBRepDS_FACE && S2 != S1 && isB1 && isB2);

      if (cond2)
      {
        cond1 = true;
        LI.Remove(it2);
      }
      else
        it2.Next();
    } // it2.More()

    if (cond1)
    {
      LI.Remove(it1);
    }
    else
      it1.Next();
  } // it1.More()

} // FUN_unkeepFdoubleGBoundinterferences

//------------------------------------------------------
Standard_EXPORT void FUN_resolveFUNKNOWN
  //------------------------------------------------------
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&                      LI,
   TopOpeBRepDS_DataStructure&                           BDS,
   const int                                SIX,
   const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MEsp,
   TopOpeBRepTool_PShapeClassifier                       pClassif)
{
  const TopoDS_Shape&                           F = BDS.Shape(SIX);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1;

  const TopoDS_Face& FF = TopoDS::Face(F);
  //  double fE,lE; BRep_Tool::Range(EE,fE,lE);

  // process interferences of LI with UNKNOWN transition

  for (it1.Initialize(LI); it1.More(); it1.Next())
  {
    occ::handle<TopOpeBRepDS_Interference>& I1    = it1.ChangeValue();
    const TopOpeBRepDS_Transition&     T1    = I1->Transition();
    bool                   isunk = T1.IsUnknown();
    if (!isunk)
      continue;

    TopOpeBRepDS_Kind GT1, ST1;
    int  G1, S1;
    TopAbs_ShapeEnum  tsb1, tsa1;
    int  isb1, isa1;
    FDS_Idata(I1, tsb1, isb1, tsa1, isa1, GT1, G1, ST1, S1);
    bool idt  = (tsb1 == TopAbs_FACE && tsa1 == TopAbs_FACE && GT1 == TopOpeBRepDS_EDGE
                            && ST1 == TopOpeBRepDS_FACE);
    bool idi  = (isb1 == S1 && isa1 == S1);
    bool etgf = idt && idi; // face tangent a une face en 1 edge
    if (!etgf)
      continue;

    const TopoDS_Edge& EE = TopoDS::Edge(BDS.Shape(G1));
    double      fE, lE;
    BRep_Tool::Range(EE, fE, lE);

    occ::handle<TopOpeBRepDS_FaceEdgeInterference> fei = MAKEFEI(I1);
    if (fei.IsNull())
      continue;

    const TopoDS_Face& FS = TopoDS::Face(BDS.Shape(S1));

    //    if (!go) continue;

    // la face FF transitionne par la transition T1.IsUnknown()
    // en l'arete EE par rapport a la face FS.
    // range de EE = [fE,lE].
    // EE est une arete de FF ou non.
    // EE est une arete de section.
    // EE est une arete de couture.
    bool isEEGB = fei->GBound();
    bool isEEsp = MEsp.IsBound(EE);
    TopoDS_Edge      EEsp   = EE;
    if (isEEsp)
    {
      const TopOpeBRepDS_ListOfShapeOn1State& los1 = MEsp.Find(EE);
      isEEsp                                       = los1.IsSplit();
      if (isEEsp)
      {
        const NCollection_List<TopoDS_Shape>& los = los1.ListOnState();
        int            n   = los.Extent();
        if (n)
        {
          EEsp = TopoDS::Edge(los.First());
          if (!EEsp.IsSame(EE))
            isEEGB = false;
          if (n > 1)
          {
            // MSV: treat the case of multiple splits:
            //      select the split which lies on both faces
            NCollection_List<TopoDS_Shape>::Iterator it(los);
            for (; it.More(); it.Next())
            {
              const TopoDS_Edge& aE = TopoDS::Edge(it.Value());
              double      f, l;
              FUN_tool_bounds(aE, f, l);
              const double PAR_T = 0.456789;
              double       pmil  = (1. - PAR_T) * f + PAR_T * l;
              gp_Pnt2d            uvF;
              if (FUN_tool_parF(aE, pmil, FF, uvF) && FUN_tool_parF(aE, pmil, FS, uvF))
              {
                EEsp = aE;
                break;
              }
            }
          }
        }
      }
    }
    bool isSO = true;
    if (!EEsp.IsSame(EE))
      if (!FUN_tool_curvesSO(EEsp, EE, isSO))
        continue;

    TopAbs_State            stateb, statea;
    TopOpeBRepDS_Transition T;
    bool        ok = FUN_mkTonF(FF, FS, EEsp, T); // xpu230498
    if (ok)
    {
      stateb = T.Before();
      statea = T.After();
    } // xpu230498
    else
    {
      TopOpeBRepTool_PShapeClassifier pClass = 0;
      if (pClassif)
      {
        // MSV: find Solids of the same object rank as FS
        //      to determine transition relatively solid rather then face
        //      if possible (see pb. in CFE002 C2, when SIX==13)
        int    rankFS = BDS.AncestorRank(S1);
        const TopoDS_Shape& aSRef  = BDS.Shape(rankFS);
        TopExp_Explorer     ex(aSRef, TopAbs_SOLID);
        if (ex.More())
        {
          pClass = pClassif;
          pClass->SetReference(aSRef);
        }
      }
      FUN_UNKFstasta(FF, FS, EEsp, isEEGB, stateb, statea, pClass);
    }
    if (stateb == TopAbs_UNKNOWN || statea == TopAbs_UNKNOWN)
      continue;

    TopOpeBRepDS_Transition& newT1 = I1->ChangeTransition();
    if (!isSO)
    {
      TopAbs_State stmp = stateb;
      stateb            = statea;
      statea            = stmp;
    }
    newT1.Set(stateb, statea, tsb1, tsa1);
  }
  FUN_unkeepUNKNOWN(LI, BDS, SIX);
}
