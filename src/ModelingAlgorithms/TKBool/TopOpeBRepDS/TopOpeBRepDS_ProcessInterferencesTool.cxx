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

#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>
// #include <BRepAdaptor_Curve2d.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <Standard_ProgramError.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>

//-----------------------------------------------------------------------
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeCPVInterference(
  const TopOpeBRepDS_Transition& T,  // transition
  const int                      SI, // curve/edge index
  const int                      GI, // point/vertex index
  const double                   P,  // parameter of G on S
  const TopOpeBRepDS_Kind        GK)        // POINT/VERTEX
//-----------------------------------------------------------------------
{
  occ::handle<TopOpeBRepDS_Interference> I;
  TopOpeBRepDS_Kind                      SK = TopOpeBRepDS_CURVE;
  I = TopOpeBRepDS_InterferenceTool::MakeCurveInterference(T, SK, SI, GK, GI, P);
  return I;
}

//-----------------------------------------------------------------------
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T,  // transition
  const int                      SI, // curve/edge index
  const int                      GI, // point/vertex index
  const double                   P,  // parameter of G on S
  const TopOpeBRepDS_Kind        GK,
  const bool                     B) // G is a vertex (or not) of the interference master
//-----------------------------------------------------------------------
{
  occ::handle<TopOpeBRepDS_Interference> I;
  TopOpeBRepDS_Kind                      SK = TopOpeBRepDS_EDGE;
  if (GK == TopOpeBRepDS_POINT)
  {
    I = TopOpeBRepDS_InterferenceTool::MakeEdgeInterference(T, SK, SI, GK, GI, P);
  }
  else if (GK == TopOpeBRepDS_VERTEX)
  {
    I = TopOpeBRepDS_InterferenceTool::MakeEdgeVertexInterference(T,
                                                                  SI,
                                                                  GI,
                                                                  B,
                                                                  TopOpeBRepDS_UNSHGEOMETRY,
                                                                  P);
    I->GeometryType(GK);
  }
  return I;
}

//-----------------------------------------------------------------------
Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> MakeEPVInterference(
  const TopOpeBRepDS_Transition& T,  // transition
  const int                      S,  // curve/edge index
  const int                      G,  // point/vertex index
  const double                   P,  // parameter of G on S
  const TopOpeBRepDS_Kind        GK, // POINT/VERTEX
  const TopOpeBRepDS_Kind        SK,
  const bool                     B) // G is a vertex (or not) of the interference master
//-----------------------------------------------------------------------
{
  occ::handle<TopOpeBRepDS_Interference> I = ::MakeEPVInterference(T, S, G, P, GK, B);
  I->SupportType(SK);
  return I;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_hasStateShape
  //------------------------------------------------------
  // FUN_hasStateShape : True if transition T is (state,shape) before or after
  (const TopOpeBRepDS_Transition& T, const TopAbs_State state, const TopAbs_ShapeEnum shape)
{
  TopAbs_State     staB = T.Before(), staA = T.After();
  TopAbs_ShapeEnum shaB = T.ShapeBefore(), shaA = T.ShapeAfter();
  bool             B      = (staB == state) && (shaB == shape);
  bool             A      = (staA == state) && (shaA == shape);
  bool             result = B || A;
  return result;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectTRASHAinterference
  //------------------------------------------------------
  // selection des interf (sha2) de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const TopAbs_ShapeEnum                                    sha2,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference> I1 = it1.Value();
    const TopOpeBRepDS_Transition&         T1 = I1->Transition();

    TopAbs_ShapeEnum shab = T1.ShapeBefore(), shaa = T1.ShapeAfter();
    bool             sel = ((shab == sha2) || (shaa == sha2));
    if (sel)
    {
      L2.Append(I1);
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectITRASHAinterference
  //------------------------------------------------------
  // selection des interf (sha2) de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const int                                                 Index,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  if (Index == 0)
    return 0;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference> I1  = it1.Value();
    const TopOpeBRepDS_Transition&         T1  = I1->Transition();
    int                                    Ind = T1.Index();
    if (Ind == Index)
    {
      L2.Append(I1);
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectTRAUNKinterference
  //------------------------------------------------------
  // selection des interf de transition (UNK,UNK) de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference> I1  = it1.Value();
    const TopOpeBRepDS_Transition&         T1  = I1->Transition();
    bool                                   sel = T1.IsUnknown();
    if (sel)
    {
      L2.Append(I1);
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectTRAORIinterference
  //------------------------------------------------------
  // selection des interf d'orientation donnee O de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const TopAbs_Orientation                                  O,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference> I1  = it1.Value();
    const TopOpeBRepDS_Transition&         T1  = I1->Transition();
    TopAbs_Orientation                     ori = T1.Orientation(TopAbs_IN);
    bool                                   sel = (ori == O);
    if (sel)
    {
      L2.Append(I1);
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectGKinterference
  //------------------------------------------------------
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const TopOpeBRepDS_Kind                                   GK,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    TopOpeBRepDS_Kind gki = it1.Value()->GeometryType();
    if (gki == GK)
    {
      L2.Append(it1.Value());
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectSKinterference
  //------------------------------------------------------
  // selection des interf de type de support SK de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const TopOpeBRepDS_Kind                                   SK,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    TopOpeBRepDS_Kind ski = it1.Value()->SupportType();
    if (ski == SK)
    {
      L2.Append(it1.Value());
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectGIinterference
  //------------------------------------------------------
  // selection des interf d' index de geometrie GI de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const int                                                 GI,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  if (GI == 0)
    return 0;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    int gi = it1.Value()->Geometry();
    if (gi == GI)
    {
      L2.Append(it1.Value());
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectSIinterference
  //------------------------------------------------------
  // selection des interf d' index de support SI de L1 dans L2. retourne leur nombre.
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L1,
   const int                                                 SI,
   NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L2)
{
  if (SI == 0)
    return 0;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(L1);
  while (it1.More())
  {
    int si = it1.Value()->Support();
    if (si == SI)
    {
      L2.Append(it1.Value());
      L1.Remove(it1);
    }
    else
      it1.Next();
  }
  int n2 = L2.Extent();
  return n2;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_interfhassupport
  //------------------------------------------------------
  // FUN_interfhassupport : True si shape support de l 'interf I est IsSame(S).
  (const TopOpeBRepDS_DataStructure&             DS,
   const occ::handle<TopOpeBRepDS_Interference>& I,
   const TopoDS_Shape&                           S)
{
  bool                h     = true;
  const int           index = I->Support();
  const TopoDS_Shape& SofI  = DS.Shape(index);
  if (SofI.IsSame(S))
    h = true;
  else
    h = false;
  return h;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_transitionEQUAL
  //------------------------------------------------------
  (const TopOpeBRepDS_Transition& T1, const TopOpeBRepDS_Transition& T2)
{
  bool id1 = FUN_transitionSTATEEQUAL(T1, T2);
  bool id2 = FUN_transitionSHAPEEQUAL(T1, T2);
  bool id3 = FUN_transitionINDEXEQUAL(T1, T2);
  bool id  = id1 && id2 && id3;
  return id;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_transitionSTATEEQUAL
  //------------------------------------------------------
  (const TopOpeBRepDS_Transition& T1, const TopOpeBRepDS_Transition& T2)
{
  bool id = T1.Before() == T2.Before() && T1.After() == T2.After();
  return id;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_transitionSHAPEEQUAL
  //------------------------------------------------------
  (const TopOpeBRepDS_Transition& T1, const TopOpeBRepDS_Transition& T2)
{
  bool id = T1.ShapeBefore() == T2.ShapeBefore() && T1.ShapeAfter() == T2.ShapeAfter();
  return id;
}

//------------------------------------------------------
Standard_EXPORT bool FUN_transitionINDEXEQUAL
  //------------------------------------------------------
  (const TopOpeBRepDS_Transition& T1, const TopOpeBRepDS_Transition& T2)
{
  bool id = T1.IndexBefore() == T2.IndexBefore() && T1.IndexAfter() == T2.IndexAfter();
  return id;
}

//------------------------------------------------------
Standard_EXPORT void FUN_reducedoublons
  //------------------------------------------------------
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
   const TopOpeBRepDS_DataStructure&                         BDS,
   const int                                                 SIX)
{

  const TopoDS_Shape&                                                E = BDS.Shape(SIX);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1;

  // process interferences of LI with VERTEX geometry
  it1.Initialize(LI);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference>& I1 = it1.ChangeValue();
    const TopOpeBRepDS_Transition&          T1 = I1->Transition();
    TopOpeBRepDS_Kind                       GT1, ST1;
    int                                     G1, S1;
    FDS_data(I1, GT1, G1, ST1, S1);
    TopAbs_ShapeEnum tsb1, tsa1;
    int              isb1, isa1;
    FDS_Tdata(I1, tsb1, isb1, tsa1, isa1);

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
    it2.Next();
    while (it2.More())
    {

      const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
      const TopOpeBRepDS_Transition&                T2 = I2->Transition();
      TopOpeBRepDS_Kind                             GT2, ST2;
      int                                           G2, S2;
      FDS_data(I2, GT2, G2, ST2, S2);
      TopAbs_ShapeEnum tsb2, tsa2;
      int              isb2, isa2;
      FDS_Tdata(I2, tsb2, isb2, tsa2, isa2);

      bool idGS = (GT2 == GT1 && G2 == G1 && ST2 == ST1 && S2 == S1);
      if (idGS)
      {

        bool id1 = FUN_transitionSTATEEQUAL(T1, T2);
        bool id2 = FUN_transitionSHAPEEQUAL(T1, T2);
        bool id3 = FUN_transitionINDEXEQUAL(T1, T2);
        bool idT = id1 && id2 && id3;

        if (idT)
        {
          const occ::handle<TopOpeBRepDS_EdgeVertexInterference> EVI1 =
            occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I1);
          const occ::handle<TopOpeBRepDS_EdgeVertexInterference> EVI2 =
            occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I2);
          bool evi = (!EVI1.IsNull()) && (!EVI2.IsNull());
          if (evi)
          {
            // xpu130898 : if vG is on E's closing vertex -> filter
            const TopoDS_Vertex& vG = TopoDS::Vertex(BDS.Shape(G1));
            TopoDS_Vertex        OOv;
            bool                 hasOO = FUN_ds_getoov(vG, BDS, OOv);
            TopoDS_Vertex        vclo;
            bool                 Eclosed = TopOpeBRepTool_TOOL::ClosedE(TopoDS::Edge(E), vclo);
            bool                 onvclo  = false;
            if (Eclosed)
            {
              onvclo = vG.IsSame(vG);
              if (hasOO && !onvclo)
                onvclo = vG.IsSame(OOv);
            }
            if (onvclo)
              idT = true;
            else
            {
              // xpu100697 : if interf are EVI compare parameters
              double tolE    = FUN_tool_maxtol(E);
              double t       = Precision::Parametric(tolE);
              double t1      = EVI1->Parameter();
              double t2      = EVI2->Parameter();
              double dd      = t1 - t2;
              bool   samepar = (std::abs(dd) <= t);
              idT            = samepar;
            }
          } // evi
        }
        if (idT)
        {
          // les 2 interferences I1 et I2 sont identiques : on supprime I2
          LI.Remove(it2);
        }
        else
          it2.Next();
      }
      else
        it2.Next();
    }
    it1.Next();
  } // it1.More()
} // reducedoublons

//------------------------------------------------------
Standard_EXPORT void FUN_unkeepUNKNOWN
  //------------------------------------------------------
  (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
   TopOpeBRepDS_DataStructure& /*BDS*/,
   const int)
{
  //                 BDS.Shape(SIX);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1;

  // process interferences of LI with UNKNOWN transition

  it1.Initialize(LI);
  while (it1.More())
  {
    occ::handle<TopOpeBRepDS_Interference>& I1    = it1.ChangeValue();
    const TopOpeBRepDS_Transition&          T1    = I1->Transition();
    bool                                    isunk = T1.IsUnknown();

    if (isunk)
    {
      LI.Remove(it1);
    }
    else
      it1.Next();
  } // it1.More()
} // unkeepUNKNOWN

// -----------------------------------------------------------
static int FUN_select3dI(const int                                                 SIX,
                         TopOpeBRepDS_DataStructure&                               BDS,
                         NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFE,
                         NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFEresi,
                         NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFE)
//------------------------------------------------------
{
  l3dFE.Clear();
  lFEresi.Clear();
  int n3d = 0;
  int nFE = lFE.Extent();
  if (nFE <= 1)
    return n3d;

  const TopoDS_Edge& E     = TopoDS::Edge(BDS.Shape(SIX));
  int                rankE = BDS.AncestorRank(E);
  TopoDS_Shape       OOv;
  int                OOG = 0, Gsta = 0;

  // attached to a given edge :
  // <lFE> = {I=(T(face),G=POINT,S=EDGE)}/ {I=(T(face),G=VERTEX,S=EDGE)}
  //             = set of interferences of same geometry kind.
  //    -> <l3dFE> + <lFE>
  // <l3dFE> = sets of interferences with same geometry,
  //           describing 3d complex transitions.

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(lFE);
  while (it1.More())
  {

    bool                                          complex3d = false;
    const occ::handle<TopOpeBRepDS_Interference>& I1        = it1.Value();
    TopOpeBRepDS_Kind                             GT1, ST1;
    int                                           G1, S1;
    FDS_data(I1, GT1, G1, ST1, S1);
    TopAbs_ShapeEnum SB1, SA1;
    int              IB1, IA1;
    FDS_Tdata(I1, SB1, IB1, SA1, IA1);
    bool vertex1 = (GT1 == TopOpeBRepDS_VERTEX);

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
    if (it2.More())
      it2.Next();
    else
      break;

    // <Gsta>, <OOv>
    if (vertex1)
    {
      TopoDS_Vertex vG1     = TopoDS::Vertex(BDS.Shape(G1));
      int           rankvG1 = BDS.AncestorRank(vG1);
      bool          G1hsd   = FUN_ds_getVsdm(BDS, G1, OOG);
      if (rankvG1 != rankE) // vG1 not on E
      {
        OOv  = vG1;
        Gsta = G1hsd ? 3 : 2;
      }
      else // vG on E
      {
        if (G1hsd)
          OOv = BDS.Shape(OOG);
        Gsta = G1hsd ? 3 : 1;
      }
    }

    while (it2.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
      TopOpeBRepDS_Kind                             GT2, ST2;
      int                                           G2, S2;
      FDS_data(I2, GT2, G2, ST2, S2);
      TopAbs_ShapeEnum SB2, SA2;
      int              IB2, IA2;
      FDS_Tdata(I2, SB2, IB2, SA2, IA2);

      bool sameG = (GT2 == GT1);
      if (!sameG)
        break;
      sameG = (G2 == G1) || (G2 == OOG);

      const TopoDS_Face& F1 = TopoDS::Face(BDS.Shape(IB1));
      const TopoDS_Face& F2 = TopoDS::Face(BDS.Shape(IB2));

      // same domain faces, -> 2d interferences
      bool sdmFT = (IB1 == IB2) || FUN_ds_sdm(BDS, F1, F2);
      //      if (sdmFT) {it2.Next(); continue;}
      if (sdmFT)
      {
        lFEresi.Append(I2);
        lFE.Remove(it2);
        continue;
      }

      // a. if (S2 == S1) ->ok
      // b. else :
      //     if (G is POINT on Fi i = 1,2) ->continue
      //     else : look for E1(edge S1) on F1(IB1) shared by F2(IB2)
      //            with bound G.
      bool sameS = (ST2 == ST1) && (S2 == S1);

      bool sameSorsharedEbyTRASHA = sameS;
      bool hasOOv                 = (Gsta > 1);
      if (!sameSorsharedEbyTRASHA && hasOOv)
      {
        TopoDS_Shape Eshared;
        bool         foundsh = FUN_tool_Eshared(OOv, F1, F2, Eshared);
        if (!foundsh)
        {
          it2.Next();
          continue;
        }

        sameSorsharedEbyTRASHA = true;
        if (!BDS.HasShape(Eshared))
        {
          int OOrank = BDS.AncestorRank(OOv);
          BDS.AddShape(Eshared, OOrank);
        }
        S1 = BDS.Shape(Eshared);
        S2 = S1;
      }

      if (sameSorsharedEbyTRASHA)
      { // xpu : 09-03-98
        bool sdm = FUN_ds_sdm(BDS, BDS.Shape(SIX), BDS.Shape(S1));
        if (sdm)
        {
          it2.Next();
          continue;
        }
      } // xpu : 09-03-98

      if (sameSorsharedEbyTRASHA)
      {
        l3dFE.Append(I2);
        lFE.Remove(it2);
        complex3d = true;
      }
      else
        it2.Next();
    } // it2(it1);

    if (complex3d)
    {
      l3dFE.Append(I1);
      lFE.Remove(it1);
    }
    else
      it1.Next();
  } // it1(lFE);

  // xpu170898 : cto009H1 (I1(T1(F),G,S1),I1'(T2(F),G,S2), I2)
  //             (I1,I2) -> to reduce => I1' to delete
  n3d       = l3dFE.Extent();
  int nresi = lFEresi.Extent();
  if (nresi == 0)
    return n3d;

  if (n3d != 0)
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(lFEresi);
    while (it.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>&            I   = it.Value();
      int                                                      TRA = I->Transition().Index();
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lcopy;
      FDS_assign(l3dFE, lcopy);
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lfound;
      int nfound = FUN_selectITRASHAinterference(lcopy, TRA, lfound);
      if (nfound == 0)
        lFE.Remove(it);
      else
        it.Next();
    }
  }
  else
    lFE.Append(lFEresi);
  return n3d;
}

static bool FUN_find3dISEsameISF(
  const occ::handle<TopOpeBRepDS_Interference>&                       I1,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& it2)
// I1 = (T0,G0,S)
// looking among interferences of <it2> for I2/
// I2 = (T0,G0,S')
{
  TopAbs_Orientation O1 = I1->Transition().Orientation(TopAbs_IN);
  TopOpeBRepDS_Kind  GT1, ST1;
  int                G1, S1;
  FDS_data(I1, GT1, G1, ST1, S1);
  TopAbs_ShapeEnum SB1, SA1;
  int              IB1, IA1;
  FDS_Tdata(I1, SB1, IB1, SA1, IA1);

  while (it2.More())
  {
    occ::handle<TopOpeBRepDS_Interference>& I2 = it2.ChangeValue();
    TopAbs_Orientation                      O2 = I2->Transition().Orientation(TopAbs_IN);
    TopOpeBRepDS_Kind                       GT2, ST2;
    int                                     G2, S2;
    FDS_data(I2, GT2, G2, ST2, S2);
    TopAbs_ShapeEnum SB2, SA2;
    int              IB2, IA2;
    FDS_Tdata(I2, SB2, IB2, SA2, IA2);
    bool sameT = (SB1 == SB2) && (IB1 == IB2) && (O1 == O2);
    bool sameG = (GT1 == GT2) && (G1 == G2);
    if (sameT && sameG)
      return true;
    else
      it2.Next();
  } // it2
  return false;
}

// ------------------------------------------------------------
static int FUN_select3dISEsameISF(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFEresi,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lF,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dF)
//-------------------------------------------------------------
{
  //    I3d in <l3dFE>, I3d = (T(face),G=POINT/VERTEX,S=EDGE)
  // <->I3d' in <l3dF>,  I3d'= (T(face),G=POINT/VERTEX,S=FACE)

  // <l3dFEold> -> {I3d} + <l3dFEnew>
  // <lFEold>   -> <lFE> + (<l3dFEold> - <l3dFEnew>)
  // <lFold>    -> {I3d'}+ <lFnew>;    {I3d'}=<l3dF>

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(l3dFE);
  while (it1.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I1 = it1.Value();
    //    TopAbs_Orientation O1 = I1->Transition().Orientation(TopAbs_IN);
    //    TopOpeBRepDS_Kind GT1,ST1; int G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
    //    TopAbs_ShapeEnum SB1,SA1; int IB1,IA1; FDS_Tdata(I1,SB1,IB1,SA1,IA1);

    //    bool found = false;
    //    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(lF);
    //    while ( it2.More()){
    //      occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
    //      TopAbs_Orientation O2 = I2->Transition().Orientation(TopAbs_IN);
    //      TopOpeBRepDS_Kind GT2,ST2; int G2,S2; FDS_data(I2,GT2,G2,ST2,S2);
    //      TopAbs_ShapeEnum SB2,SA2; int IB2,IA2; FDS_Tdata(I2,SB2,IB2,SA2,IA2);
    //      bool sameT = (SB1 == SB2) && (IB1 == IB2) && (O1 == O2);
    //      bool sameG = (GT1 == GT2) && (G1 == G2);
    //      if (sameT && sameG) {found = true; l3dF.Append(I2); lF.Remove(it2);break;}
    //      else                it2.Next();
    //    } // it2
    //    if (found) it1.Next();
    //    else       {lFE.Append(I1); l3dFE.Remove(it1);}

    // {IFE = (T0,G0,Sedge); IFF = (T0,G0,Sface)}
    // => l3dFE += IFE, l3dF += iFF
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(lF);
    bool found = FUN_find3dISEsameISF(I1, it2);

    if (found)
    {
      l3dF.Append(it2.Value());
      lF.Remove(it2);
      it1.Next();
    }
    else
    {
      // xpu : 16-02-98 :
      // IFE1 = (T0,G0,Sedge1), IFE2 = (T0,G0,Sedge2)
      // IFF = (T0,G0,Sface)
      // => l3dFE += IFE1, l3dF += iFF,
      //    l3dFEresi += IFE2
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it3(l3dF);
      found = FUN_find3dISEsameISF(I1, it3);

      if (found)
        l3dFEresi.Append(I1);
      else
        lFE.Append(I1);
      l3dFE.Remove(it1);
    }
  } // it1
  return l3dF.Extent();
}

//------------------------------------------------------
Standard_EXPORT int FUN_select2dI(const int                                                 SIX,
                                  TopOpeBRepDS_DataStructure&                               BDS,
                                  const TopAbs_ShapeEnum                                    TRASHAk,
                                  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lI,
                                  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l2dI)
//------------------------------------------------------
// <lI> -> <l2dI> + <lI>
// TRASHAk = TopAbs_FACE : <l2dI> = {I2d = (T on same domain faces, G,S)}
// TRASHAk = TopAbs_EDGE : <l2dI> = {I2d = (T on same edge, G,S)}
{
  l2dI.Clear();
  int n2d = 0;
  int nFE = lI.Extent();
  if (nFE <= 1)
    return n2d;

  bool TonFace = (TRASHAk == TopAbs_FACE);

  // xpu201098 : check FEI is not 3dFEI (cto904F6,SIX10,G6)
  NCollection_Map<int>                                     mapftra;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> lIE;
  FDS_copy(BDS.ShapeInterferences(SIX), lIE);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> l3dF;
  FUN_selectSKinterference(lIE, TopOpeBRepDS_FACE, l3dF);
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itt(l3dF); itt.More();
       itt.Next())
    mapftra.Add(itt.Value()->Support());
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>           lII;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(lI);
  while (it1.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I       = it1.Value();
    int                                           ITRASHA = I->Transition().Index();
    if (mapftra.Contains(ITRASHA))
    {
      it1.Next();
    }
    else
    {
      lII.Append(I);
      lI.Remove(it1);
    }
  }

  it1.Initialize(lII);
  while (it1.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I1 = it1.Value();
    TopOpeBRepDS_Kind                             GT1, ST1;
    int                                           G1, S1;
    FDS_data(I1, GT1, G1, ST1, S1);
    TopAbs_ShapeEnum SB1, SA1;
    int              IB1, IA1;
    FDS_Tdata(I1, SB1, IB1, SA1, IA1);

    if (SB1 != TRASHAk)
    {
      it1.Next();
      continue;
    }

    bool                                                               complex2d = false;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
    if (it2.More())
      it2.Next();
    else
      break;
    while (it2.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
      TopOpeBRepDS_Kind                             GT2, ST2;
      int                                           G2, S2;
      FDS_data(I2, GT2, G2, ST2, S2);
      TopAbs_ShapeEnum SB2, SA2;
      int              IB2, IA2;
      FDS_Tdata(I2, SB2, IB2, SA2, IA2);

      bool cond = (SB1 == SB2) && (IB1 == IB2) && (IA1 == IA2);
      if (!cond)
      {
        it2.Next();
        continue;
      }

      bool sameG  = (GT2 == GT1) && (G2 == G1);
      bool sameST = (ST2 == ST1);
      cond        = sameG && sameST;
      if (!cond)
      {
        it2.Next();
        continue;
      }

      complex2d = (IB1 == IB2);
      if (TonFace)
      {
        const TopoDS_Face& F1 = TopoDS::Face(BDS.Shape(IB1));
        const TopoDS_Face& F2 = TopoDS::Face(BDS.Shape(IB2));
        complex2d             = complex2d || FUN_ds_sdm(BDS, F1, F2);
      }
      if (complex2d)
      {
        l2dI.Append(I2);
        lII.Remove(it2);
      }
      else
        it2.Next();
    } // it2
    if (complex2d)
    {
      l2dI.Append(I1);
      lII.Remove(it1);
    }
    else
      it1.Next();
  } // it1
  lI.Append(lII);
  n2d = l2dI.Extent();
  return n2d;
}

//------------------------------------------------------
Standard_EXPORT int FUN_selectpure2dI(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lF,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       lFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&       l2dFE)
//------------------------------------------------------
// <lFE> -> <lFE> + <l2dFE>
// <l2dFE> = {I2dFE = (T(face),G,S=edge) / we cannot find (T(face),G,face)}
{
  l2dFE.Clear();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itFE(lFE);
  while (itFE.More())
  {
    const occ::handle<TopOpeBRepDS_Interference>& IFE = itFE.Value();
    const TopOpeBRepDS_Transition&                TFE = IFE->Transition();
    int                                           IB = TFE.IndexBefore(), IA = TFE.IndexAfter();
    if (IB != IA)
    {
      itFE.Next();
      continue;
    }

    bool foundIF = false;
    for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itF(lF); itF.More();
         itF.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& IF = itF.Value();
      int                                           S  = IF->Support();
      if (S == IB)
      {
        foundIF = true;
        break;
      }
    }
    if (foundIF)
    {
      itFE.Next();
      continue;
    }

    l2dFE.Append(IFE);
    lFE.Remove(itFE);
  }
  int n2dFE = l2dFE.Extent();
  return n2dFE;
} // selectpure2dI

//------------------------------------------------------
Standard_EXPORT int FUN_select1dI(const int                                                 SIX,
                                  TopOpeBRepDS_DataStructure&                               BDS,
                                  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                                  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l1dI)
//------------------------------------------------------
// attached to edge E : <lI>={I=(T(ES),G,ES)} -> <l1dI> + <lI>
// l1dI contains {(I1,I2) / I1=(T1(ES1),VG,ES1), I2=(T2(ES2),VG,ES2),
//                          E sdm ES1, ES2}
{
  l1dI.Clear();
  int n1d = 0;
  int nFE = LI.Extent();
  if (nFE <= 1)
    return n1d;

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> newLI;
  const TopoDS_Shape&                                      EIX = BDS.Shape(SIX);
  TopOpeBRepDS_TKI                                         tki;
  tki.FillOnGeometry(LI);
  for (tki.Init(); tki.More(); tki.Next())
  {
    TopOpeBRepDS_Kind K;
    int               G;
    tki.Value(K, G);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = tki.ChangeValue(K, G);
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>  Rloi;
    int                                                       nloi = loi.Extent();

    bool ok = (K == TopOpeBRepDS_VERTEX);
    ok      = ok && (nloi > 1);
    if (!ok)
    {
      newLI.Append(loi);
      continue;
    }

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(loi);
    while (it1.More())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I1 = it1.Value();
      TopOpeBRepDS_Kind                             GT1, ST1;
      int                                           G1, S1;
      FDS_data(I1, GT1, G1, ST1, S1);
      TopAbs_ShapeEnum SB1, SA1;
      int              IB1, IA1;
      FDS_Tdata(I1, SB1, IB1, SA1, IA1);

      bool cond1 = (SB1 == TopAbs_EDGE) && (IB1 == IA1);
      if (!cond1)
      {
        newLI.Append(I1);
        it1.Next();
        continue;
      }
      cond1 = FUN_ds_sdm(BDS, EIX, BDS.Shape(S1));
      if (!cond1)
      {
        newLI.Append(I1);
        it1.Next();
        continue;
      }

      bool                                                               complex1d = false;
      NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
      if (it2.More())
        it2.Next();
      else
        break;
      while (it2.More())
      {
        const occ::handle<TopOpeBRepDS_Interference>& I2 = it2.Value();
        TopOpeBRepDS_Kind                             GT2, ST2;
        int                                           G2, S2;
        FDS_data(I2, GT2, G2, ST2, S2);
        TopAbs_ShapeEnum SB2, SA2;
        int              IB2, IA2;
        FDS_Tdata(I2, SB2, IB2, SA2, IA2);

        bool cond2 = (SB1 == SB2) && (IB2 == IA2);
        if (!cond2)
        {
          newLI.Append(I2);
          it2.Next();
          continue;
        }
        complex1d = FUN_ds_sdm(BDS, EIX, BDS.Shape(S2));
        if (!complex1d)
        {
          newLI.Append(I2);
          it2.Next();
          continue;
        }
        //                 LI.Extent(); //deb
        l1dI.Append(I2);
        it2.Next();
      } // it2
      if (complex1d)
      {
        l1dI.Append(I1);
        it1.Next();
      }
      else
        it1.Next();
    } // it1
  } // tki
  LI.Clear();
  LI.Append(newLI);
  n1d = l1dI.Extent();
  return n1d;
} // select1dI

//------------------------------------------------------
Standard_EXPORT void FUN_select3dinterference(
  const int                                                 SIX,
  TopOpeBRepDS_DataStructure&                               BDS,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lF,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dF,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& lFEresi,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFE,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l3dFEresi,
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l2dFE)
//------------------------------------------------------
{
  const TopoDS_Edge& E    = TopoDS::Edge(BDS.Shape(SIX));
  bool               isdg = BRep_Tool::Degenerated(E);
  if (isdg)
    return;

  l3dF.Clear();
  l3dFE.Clear();
  // attached to an edge the list of interferences :
  // IN  : <lF>  = {I=(T(face),G=POINT/VERTEX,S=FACE)}
  //       <lFE> = {I=(T(face),G=POINT/VERTEX,S=EDGE)}

  // OUT : <lFE> -> <l3dFE> = {I describing 3d interferences on S=EDGE}
  //                   is the list of 3d interferences to reduce
  //                +<lFE> +lFEresi
  //       <lF>  -> <l3dF> = {I describing 3d interferences on S=FACE}
  //                +<lF>
  //     I  = (T(face),G=POINT/VERTEX,S=EDGE) in <l3dFE>
  // <-> I' = (T(face),G=POINT/VERTEX,S=FACE) in <l3dF>

  ::FUN_select3dI(SIX, BDS, lFE, lFEresi, l3dFE);
  ::FUN_select3dISEsameISF(lFE, l3dFE, l3dFEresi, lF, l3dF);
  FUN_select2dI(SIX, BDS, TopAbs_FACE, lFE, l2dFE);
}
