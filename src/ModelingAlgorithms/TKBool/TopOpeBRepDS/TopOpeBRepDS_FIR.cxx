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

// Robert Boehne 30 May 2000 : Dec Osf

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepDS_FaceInterferenceTool.hxx>
#include <TopOpeBRepDS_FIR.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_ShapeData.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_SC.hxx>

#define MDSke TopOpeBRepDS_EDGE
#define MDSkf TopOpeBRepDS_FACE

Standard_EXPORT bool FUN_Parameters(const gp_Pnt& Pnt, const TopoDS_Shape& F, double& u, double& v);
Standard_EXPORT bool FUN_edgeofface(const TopoDS_Shape& E, const TopoDS_Shape& F);

//------------------------------------------------------
bool FUN_isPonF(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIF,
                const gp_Pnt&                                                   P,
                const TopOpeBRepDS_DataStructure&                               BDS,
                const TopoDS_Edge&                                              E)
{
  bool              Pok = true;
  TopOpeBRepDS_Kind GT1, ST1;
  int               G1, S1;

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itF(LIF);
  for (; itF.More(); itF.Next())
  {
    occ::handle<TopOpeBRepDS_Interference> IF = itF.Value();
    FDS_data(IF, GT1, G1, ST1, S1);
    const TopoDS_Face& F = TopoDS::Face(BDS.Shape(S1));
    TopAbs_Orientation oEinF;
    bool               edonfa = FUN_tool_orientEinFFORWARD(E, F, oEinF);
    if (edonfa)
      Pok = true;
    else
    {
      // P est NOK pour une face de LIF : arret
      double u, v;
      Pok = FUN_Parameters(P, F, u, v);
      if (!Pok)
        break;
    }
  }
  return Pok;
}

//------------------------------------------------------
bool FUN_findPonF(const TopoDS_Edge&                                              E,
                  const TopOpeBRepDS_DataStructure&                               BDS,
                  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIF,
                  gp_Pnt&                                                         P,
                  double&                                                         par)
{
  bool                                                            Pok = false;
  BRepAdaptor_Curve                                               BAC(E);
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIE = BDS.ShapeInterferences(E);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itI;
  itI.Initialize(LIE);

  if (!itI.More())
  {
    Pok = FUN_tool_findPinBAC(BAC, P, par);
    Pok = FUN_isPonF(LIF, P, BDS, E);
    return Pok;
  }

  TopOpeBRepDS_Kind GT1, ST1;
  int               G1, S1;
  for (; itI.More(); itI.Next())
  {
    bool pardef = false;

    occ::handle<TopOpeBRepDS_Interference>& I = itI.ChangeValue();
    FDS_data(I, GT1, G1, ST1, S1);
    occ::handle<TopOpeBRepDS_CurvePointInterference> CPI(
      occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I));
    occ::handle<TopOpeBRepDS_ShapeShapeInterference> SSI(
      occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(I));
    if (!CPI.IsNull())
    {
      par    = CPI->Parameter();
      pardef = true;
    }
    else if (!SSI.IsNull())
    {
      bool gb = SSI->GBound();
      if (gb)
      {
        const TopoDS_Vertex& V = TopoDS::Vertex(BDS.Shape(G1));
        P                      = BRep_Tool::Pnt(V);
        par                    = BRep_Tool::Parameter(V, E);
        pardef                 = true;
      }
      else
      {
        pardef = false;
        if (GT1 == TopOpeBRepDS_POINT)
          P = BDS.Point(G1).Point();
        else if (GT1 == TopOpeBRepDS_VERTEX)
          P = BRep_Tool::Pnt(TopoDS::Vertex(BDS.Shape(G1)));
        if (pardef)
        {
          double dist;
          pardef = FUN_tool_projPonC(P, BAC, par, dist);
        }
      }
    }
    else
    {
      continue;
    }

    if (!pardef)
    {
      continue;
    }

    BAC.D0(par, P);
    Pok = FUN_isPonF(LIF, P, BDS, E);
    // P est OK pour toutes les faces de LIF : on arrete de chercher
    if (Pok)
    {
      break;
    }
  }
  return Pok;
}

// --------------------------------------------------------
static void FDS_ADDEDGE(const bool,
                        const TCollection_AsciiString&,
                        const int,
                        TopOpeBRepDS_FaceInterferenceTool&            FITool,
                        const TopoDS_Shape&                           FI,
                        const TopoDS_Shape&                           F,
                        const TopoDS_Shape&                           Ecpx,
                        const bool                                    isEGsp,
                        const occ::handle<TopOpeBRepDS_Interference>& I)
{
  FITool.Add(FI, F, Ecpx, isEGsp, I);
}

//------------------------------------------------------
// EGsp = edge splittee de iEG ( Null si iEG n'est pas splittee)
void FUN_reduceEDGEgeometry1(NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                             const TopOpeBRepDS_DataStructure&                         BDS,
                             const int                                                 iFI,
                             const int                                                 iEG,
                             const TopoDS_Shape&                                       EGsp,
                             // const NCollection_DataMap<TopoDS_Shape,
                             // TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MEsp)
                             const NCollection_DataMap<TopoDS_Shape,
                                                       TopOpeBRepDS_ListOfShapeOn1State,
                                                       TopTools_ShapeMapHasher>&)
{
  bool                                                               TRCF = false;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator ili(LI);
  if (!ili.More())
    return;

  // choix de l'arete Ecpx, lieu de resolution de la transition complexe
  const TopoDS_Face& FI     = TopoDS::Face(BDS.Shape(iFI));
  bool               isEGsp = (!EGsp.IsNull());
  TopoDS_Edge        Ecpx;
  if (isEGsp)
    Ecpx = TopoDS::Edge(EGsp);
  else
    Ecpx = TopoDS::Edge(BDS.Shape(iEG));

  TopOpeBRepDS_PDataStructure       pbds = (TopOpeBRepDS_PDataStructure)(void*)&BDS;
  TopOpeBRepDS_FaceInterferenceTool FITool(pbds);
  gp_Pnt                            Pok;
  bool                              isPok = false;
  double                            parPok;
  if (LI.Extent() >= 2)
  {
    if (isEGsp)
      isPok = FUN_tool_findPinE(Ecpx, Pok, parPok);
    else
      isPok = FUN_findPonF(Ecpx, BDS, LI, Pok, parPok); // NYI pas necessaire
    if (!isPok)
    {
      LI.Clear();
      return;
    }
    FITool.SetEdgePntPar(Pok, parPok);
  }

  // xpu :090498 :
  //      CTS20205 : sp(e5) = sp(e4 of rank=1) and c3d(e5) c3d(e4) are diff oriented
  //            As transitions on face<iFI> are given relative to the geometry of e5,
  //            we have to complement them.
  //  bool toreverse = false;
  //  bool hsdm = !BDS.ShapeSameDomain(iEG).IsEmpty();
  //  if (hsdm) {
  //    bool sameoriented = false;
  //    bool ok =
  //    FUN_tool_curvesSO(TopoDS::Edge(Ecpx),parPok,TopoDS::Edge(BDS.Shape(iEG)),
  //			       sameoriented);
  //    if (ok) toreverse = !sameoriented;
  //  }
  // xpu :090498

  // FI = face de reference (shape), iFI (indice)
  // E = arete geometrie d'interference (shape), iEG (indice)
  // LI = liste d'interf de geom iEG et dont les Support() sont a transitionner complexe

  occ::handle<TopOpeBRepDS_Interference>                             I1, I2;
  TopOpeBRepDS_Kind                                                  GT1, ST1, GT2, ST2;
  int                                                                G1, S1, G2, S2;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1;
  it1.Initialize(LI);
  while (it1.More())
  {
    bool u1 = FDS_data(it1, I1, GT1, G1, ST1, S1);
    if (u1)
    {
      it1.Next();
      continue;
    }
    bool ya1 = (GT1 == MDSke);
    if (!ya1)
    {
      it1.Next();
      continue;
    }

    bool               isComplex = false; // True if at least two interfs on the same edge
    const TopoDS_Face& F1        = TopoDS::Face(BDS.Shape(S1));

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it2(it1);
    it2.Next();
    while (it2.More())
    {
      bool u2 = FDS_data(it2, I2, GT2, G2, ST2, S2);
      if (u2)
      {
        it2.Next();
        continue;
      }
      bool ya2 = (GT2 == GT1 && G2 == G1 && ST2 == ST1);
      if (!ya2)
      {
        it2.Next();
        continue;
      }
      const TopoDS_Face& F2 = TopoDS::Face(BDS.Shape(S2));
      if (!isComplex)
      {
        isComplex = true;

        //	TopOpeBRepDS_Transition T1 = I1->Transition(); TopAbs_Orientation O1 =
        // T1.Orientation(TopAbs_IN); // xpu :090498 	bool revT1 = toreverse &&
        //(M_FORWARD(O1) || M_REVERSED(O1));      // xpu :090498 	if (revT1)
        // I1->ChangeTransition() = T1.Complement();  //xpu :090498
        FITool.Init(FI, Ecpx, isEGsp, I1);
        FDS_ADDEDGE(TRCF, "\ninit transition complexe F", iFI, FITool, FI, F1, Ecpx, isEGsp, I1);
        //	if (revT1) I1->ChangeTransition() = T1.Complement();  //xpu :090498
      }

      //      TopOpeBRepDS_Transition T2 = I2->Transition(); TopAbs_Orientation O2 =
      //      T2.Orientation(TopAbs_IN);  // xpu :090498 bool revT2 = toreverse &&
      //      (M_FORWARD(O2) || M_REVERSED(O2));       // xpu :090498 if (revT2)
      //      I2->ChangeTransition() = T2.Complement();  //xpu :090498
      FDS_ADDEDGE(TRCF, "add transition complexe F", iFI, FITool, FI, F2, Ecpx, isEGsp, I2);
      //      if (revT2) I2->ChangeTransition() = T2.Complement();  //xpu :090498

      LI.Remove(it2);
    }
    if (isComplex)
    {
      FITool.Transition(I1);
    }
    it1.Next();
  } // it1.More()
} // FUN_reduceEDGEgeometry1

//------------------------------------------------------
void FUN_GmapS(
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>&                                  LI,
  const TopOpeBRepDS_DataStructure&                                                          BDS,
  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeData, TopTools_ShapeMapHasher>& mosd)
{
  mosd.Clear();
  TopOpeBRepDS_Kind GT1, ST1;
  int               G1, S1;
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it1(LI); it1.More();
       it1.Next())
  {
    occ::handle<TopOpeBRepDS_Interference>& I1 = it1.ChangeValue();
    FDS_data(I1, GT1, G1, ST1, S1);
    if (GT1 != MDSke || ST1 != MDSkf)
      continue;
    const TopoDS_Shape& SG1 = BDS.Shape(G1);
    mosd.Bound(SG1, TopOpeBRepDS_ShapeData())->ChangeInterferences().Append(I1);
  }
}

//------------------------------------------------------
TopAbs_State FUN_stateedgeface(const TopoDS_Shape& E, const TopoDS_Shape& F, gp_Pnt& P)
{
  TopAbs_State state = TopAbs_UNKNOWN;
  double       par;
  FUN_tool_findPinE(E, P, par);
  double u, v;
  bool   Pok = FUN_Parameters(P, F, u, v);
  if (Pok)
  { // classifier u,v dans F
    TopOpeBRepTool_ShapeClassifier& PSC = FSC_GetPSC(F);
    gp_Pnt2d                        Puv(u, v);
    PSC.StateP2DReference(Puv);
    state = PSC.State();
  }
  return state;
}

#define M_IN(ssstate) ((ssstate) == TopAbs_IN)
#define M_ON(ssstate) ((ssstate) == TopAbs_ON)
#define M_OUT(ssstate) ((ssstate) == TopAbs_OUT)
#define M_UNK(ssstate) ((ssstate) == TopAbs_UNK)

//------------------------------------------------------
void FUN_reduceEDGEgeometry(NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                            const TopOpeBRepDS_DataStructure&                         BDS,
                            const int                                                 iFI,
                            const NCollection_DataMap<TopoDS_Shape,
                                                      TopOpeBRepDS_ListOfShapeOn1State,
                                                      TopTools_ShapeMapHasher>&       MEsp)
{
  if (!LI.Extent())
    return;

  NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_ShapeData, TopTools_ShapeMapHasher> mosd;
  FUN_GmapS(LI, BDS, mosd);

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> LIout;
  // modified by NIZNHY-PKV Thu Mar 16 09:44:24 2000 f
  int i, aN;
  aN = mosd.Extent();
  // for(int i=1,n=mosd.Extent(); i<=n; i++) {
  // modified by NIZNHY-PKV Thu Mar 16 09:44:27 2000 t
  for (i = 1; i <= aN; i++)
  {
    const TopoDS_Shape& EG  = mosd.FindKey(i);
    int                 iEG = BDS.Shape(EG);

    // donnees samedomain attachees a l'arete iEG
    const NCollection_List<TopoDS_Shape>& esdeg    = BDS.ShapeSameDomain(iEG);
    bool                                  egissect = BDS.IsSectionEdge(TopoDS::Edge(EG));
    bool                                  eghasesd = (!esdeg.IsEmpty());

    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LIEG =
      mosd.ChangeFromKey(EG).ChangeInterferences();
    int nExt = LIEG.Extent();
    // LIEG = toutes les interferences dont le Support() est une
    // face possedant une interference dont la Geometry() est EG.
    if (nExt == 0)
    {
      continue;
    }
    if (nExt == 1)
    {
      LIout.Append(LIEG);
    }

    else if (nExt >= 2)
    {
      bool isEGsp = MEsp.IsBound(EG);
      // modified by NIZNHY-PKV Thu Mar 16 11:03:44 2000 from
      // int nEGsp = 0;
      // modified by NIZNHY-PKV Thu Mar 16 11:03:49 2000 to
      if (isEGsp)
      {
        const TopOpeBRepDS_ListOfShapeOn1State& los1 = MEsp.Find(EG);
        isEGsp                                       = los1.IsSplit();
        // modified by NIZNHY-PKV Thu Mar 16 11:02:40 2000 from
        // if ( isEGsp ) {
        //   const NCollection_List<TopoDS_Shape>& los = los1.ListOnState();
        //   nEGsp = los.Extent();
        // }
        // modified by NIZNHY-PKV Thu Mar 16 11:02:46 2000 to
      }

      if (isEGsp)
      {
        const NCollection_List<TopoDS_Shape>&    los = MEsp.Find(EG).ListOnState();
        NCollection_List<TopoDS_Shape>::Iterator itlos(los);
        for (; itlos.More(); itlos.Next())
        {
          // EGsp est une arete splitee de EG.
          const TopoDS_Shape& EGsp = itlos.Value();

          // LISFIN = liste des interferences de LI dont le Support()
          // est une face contenant geometriquement l'arete EGsp
          NCollection_List<occ::handle<TopOpeBRepDS_Interference>>           LISFIN;
          NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator itLIEG(LIEG);
          for (; itLIEG.More(); itLIEG.Next())
          {
            const occ::handle<TopOpeBRepDS_Interference>& ILIEG = itLIEG.Value();
            int                                           iS    = ILIEG->Support();
            TopOpeBRepDS_Kind                             kS    = ILIEG->SupportType();
            if (kS == MDSkf)
            {
              const TopoDS_Shape& SFILIEG = BDS.Shape(iS);
              gp_Pnt              P;
              TopAbs_State        staef = FUN_stateedgeface(EGsp, SFILIEG, P);

              bool Pok = M_IN(staef);
              if (eghasesd || egissect)
              {
                Pok = Pok || M_ON(staef);
              }

              if (Pok)
              {
                LISFIN.Append(ILIEG);
              }
            }
          } // itLIEG.More

          int nLISFIN = LISFIN.Extent();
          if (nLISFIN >= 2)
          {
            bool gb;
            gb = occ::down_cast<TopOpeBRepDS_ShapeShapeInterference>(LISFIN.First())->GBound();

            if (gb)
            {
              // modified by NIZNHY-PKV Thu Mar 16 10:40:57 2000 f
              //  we have to rest at least one  Interference on the face.
              //  To kill all of them is too bravely.
              occ::handle<TopOpeBRepDS_Interference> anInterference = LISFIN.First();
              LISFIN.Clear();
              LISFIN.Append(anInterference);
              // modified by NIZNHY-PKV Thu Mar 16 10:41:01 2000 t
            }
            else
              FUN_reduceEDGEgeometry1(LISFIN, BDS, iFI, iEG, EGsp, MEsp);
          }

          nLISFIN = LISFIN.Extent();
          if (nLISFIN)
            LIout.Append(LISFIN);
        }
      } // isEGsp
      else
      {
        // iFI = face de reference (indice)
        // E = arete geometrie d'interference (shape), iEG (indice)
        // LIEG = liste d'interferences de geometrie EG
        //        et dont les Support() sont a transitionner complexe
        TopoDS_Shape Enull;
        FUN_reduceEDGEgeometry1(LIEG, BDS, iFI, iEG, Enull, MEsp);
        LIout.Append(LIEG);
      }
    }
  }

  LI.Clear();
  LI.Append(LIout);
} // FUN_reduceEDGEgeometry

//=================================================================================================

TopOpeBRepDS_FIR::TopOpeBRepDS_FIR(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
    : myHDS(HDS)
{
}

//=================================================================================================

void TopOpeBRepDS_FIR::ProcessFaceInterferences(
  const NCollection_DataMap<TopoDS_Shape,
                            TopOpeBRepDS_ListOfShapeOn1State,
                            TopTools_ShapeMapHasher>& M)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  int                         i, nshape = BDS.NbShapes();
  for (i = 1; i <= nshape; i++)
  {
    const TopoDS_Shape& S = BDS.Shape(i);
    if (S.IsNull())
      continue;
    if (S.ShapeType() == TopAbs_FACE)
    {
      ProcessFaceInterferences(i, M);
    }
  }
}

//=================================================================================================

void TopOpeBRepDS_FIR::ProcessFaceInterferences(
  const int                                           SIX,
  const NCollection_DataMap<TopoDS_Shape,
                            TopOpeBRepDS_ListOfShapeOn1State,
                            TopTools_ShapeMapHasher>& MEsp)
{
  TopOpeBRepDS_DataStructure&                               BDS = myHDS->ChangeDS();
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI  = BDS.ChangeShapeInterferences(SIX);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>  lw, lE, lFE, lFEF, lF;
  lw.Assign(LI);

  ::FUN_selectTRASHAinterference(lw, TopAbs_FACE, lF);
  ::FUN_selectGKinterference(lF, MDSke, lFE);
  ::FUN_selectSKinterference(lFE, MDSkf, lFEF);
  ::FUN_selectTRASHAinterference(lw, TopAbs_EDGE, lE);
  FUN_reduceEDGEgeometry(lFEF, BDS, SIX, MEsp);

  LI.Clear();
  LI.Append(lF);
  LI.Append(lFE);
  LI.Append(lFEF);
  LI.Append(lE);
  // MSV: filter duplicates
  ::FUN_reducedoublons(LI, BDS, SIX);
}
