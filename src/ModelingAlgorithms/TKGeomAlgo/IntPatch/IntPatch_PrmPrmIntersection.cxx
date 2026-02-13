// Created on: 1993-02-02
// Created by: Laurent BUCHARD
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

// modified by Edward AGAPOV (eap) Tue Jan 22 12:29:55 2002
// modified by Oleg FEDYAED  (ofv) Fri Nov 29 16:08:02 2002

#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <IntPatch_InterferencePolyhedron.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <IntPatch_PrmPrmIntersection.hxx>
#include <IntPatch_PrmPrmIntersection_T3Bits.hxx>
#include <IntPatch_RstInt.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPolyh_Intersection.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <NCollection_List.hxx>
#include <IntWalk_PWalking.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>

static void SectionPointToParameters(const Intf_SectionPoint&   Sp,
                                     const IntPatch_Polyhedron& Surf1,
                                     const IntPatch_Polyhedron& Surf2,
                                     double&                    u1,
                                     double&                    v1,
                                     double&                    u2,
                                     double&                    v2);

static void AdjustOnPeriodic(const occ::handle<Adaptor3d_Surface>&             Surf1,
                             const occ::handle<Adaptor3d_Surface>&             Surf2,
                             NCollection_Sequence<occ::handle<IntPatch_Line>>& aSLin);

static IntSurf_PntOn2S MakeNewPoint(const IntSurf_PntOn2S& replacePnt,
                                    const IntSurf_PntOn2S& oldPnt,
                                    const double*          Periods);

static bool IsPointOnLine(const IntSurf_PntOn2S&             thePOn2S,
                          const occ::handle<IntPatch_WLine>& theWLine,
                          const double                       Deflection);

static void AddWLine(NCollection_Sequence<occ::handle<IntPatch_Line>>& theLines,
                     const occ::handle<IntPatch_WLine>&                theWLine,
                     const double                                      Deflection);

static void SeveralWlinesProcessing(const occ::handle<Adaptor3d_Surface>& theSurf1,
                                    const occ::handle<Adaptor3d_Surface>& theSurf2,
                                    const NCollection_Sequence<occ::handle<IntPatch_Line>>& theSLin,
                                    const double* const          thePeriodsArr,
                                    const IntSurf_TypeTrans      theTrans1,
                                    const IntSurf_TypeTrans      theTrans2,
                                    const double                 theTol,
                                    const double                 theMaxStepS1,
                                    const double                 theMaxStepS2,
                                    occ::handle<IntPatch_WLine>& theWLline)
{
  if (theSLin.Length() == 0)
    return;

  double aU1 = 0.0, aV1 = 0.0, aU2 = 0.0, aV2 = 0.0;

  int cnbV = theWLline->NbVertex();
  int ciV;
  for (ciV = 1; ciV <= cnbV; ciV++)
  {
    double pntDMin = 1.e+100;
    int    VDMin   = 0;
    int    WLDMin  = 0;
    gp_Pnt cPV     = theWLline->Vertex(ciV).Value();
    theWLline->Vertex(ciV).Parameters(aU1, aV1, aU2, aV2);
    const gp_Pnt2d aPCS1(aU1, aV1), aPCS2(aU2, aV2);
    int            iL;
    for (iL = 1; iL <= theSLin.Length(); iL++)
    {
      const occ::handle<IntPatch_Line>& aSLine = theSLin.Value(iL);
      IntPatch_IType                    aType  = aSLine->ArcType();
      if (aType != IntPatch_Walking)
        continue;
      const occ::handle<IntPatch_WLine> aWLine = occ::down_cast<IntPatch_WLine>(aSLine);
      int                               tnbV   = aWLine->NbVertex();
      int                               tiV;
      for (tiV = 1; tiV <= tnbV; tiV++)
      {
        gp_Pnt tPV = aWLine->Vertex(tiV).Value();

        aWLine->Vertex(tiV).Parameters(aU1, aV1, aU2, aV2);
        const gp_Pnt2d aPTS1(aU1, aV1), aPTS2(aU2, aV2);

        double tDistance = cPV.Distance(tPV);
        double uRs1      = theSurf1->UResolution(tDistance);
        double vRs1      = theSurf1->VResolution(tDistance);
        double uRs2      = theSurf2->UResolution(tDistance);
        double vRs2      = theSurf2->VResolution(tDistance);
        double RmaxS1    = std::max(uRs1, vRs1);
        double RmaxS2    = std::max(uRs2, vRs2);

        if (RmaxS1 < theMaxStepS1 && RmaxS2 < theMaxStepS2)
        {
          if (pntDMin > tDistance && tDistance > Precision::PConfusion())
          {
            const double aSqDist1 = aPCS1.SquareDistance(aPTS1),
                         aSqDist2 = aPCS2.SquareDistance(aPTS2);
            if ((aSqDist1 < RmaxS1 * RmaxS1) && (aSqDist2 < RmaxS2 * RmaxS2))
            {
              pntDMin = tDistance;
              VDMin   = tiV;
              WLDMin  = iL;
            }
          }
        }
      }
    }

    if (VDMin != 0)
    {
      const occ::handle<IntPatch_Line>& aSLine = theSLin.Value(WLDMin);
      const occ::handle<IntPatch_WLine> aWLine = occ::down_cast<IntPatch_WLine>(aSLine);
      int                               tiVpar = (int)aWLine->Vertex(VDMin).ParameterOnLine();
      int                               ciVpar = (int)theWLline->Vertex(ciV).ParameterOnLine();
      double                            u11 = 0., u12 = 0., v11 = 0., v12 = 0.;
      double                            u21 = 0., u22 = 0., v21 = 0., v22 = 0.;
      theWLline->Point(ciVpar).Parameters(u11, v11, u12, v12);
      aWLine->Point(tiVpar).Parameters(u21, v21, u22, v22);

      occ::handle<IntSurf_LineOn2S> newL2s     = new IntSurf_LineOn2S();
      IntSurf_PntOn2S               replacePnt = aWLine->Point(tiVpar);
      int                           cNbP       = theWLline->NbPnts();

      NCollection_Sequence<int> VPold;
      int                       iPo;
      for (iPo = 1; iPo <= cnbV; iPo++)
      {
        double Po  = theWLline->Vertex(iPo).ParameterOnLine();
        int    IPo = (int)Po;
        VPold.Append(IPo);
      }

      bool removeNext = false;
      bool removePrev = false;
      if (ciV == 1)
      {
        int dPar = std::abs(VPold.Value(ciV) - VPold.Value(ciV + 1));
        if (dPar > 10)
        {
          removeNext = true;
          for (iPo = (ciV + 1); iPo <= cnbV; iPo++)
            VPold.SetValue(iPo, VPold.Value(iPo) - 1);
        }
      }
      else if (ciV == cnbV)
      {
        int dPar = std::abs(VPold.Value(ciV) - VPold.Value(ciV - 1));
        if (dPar > 10)
        {
          removePrev = true;
          VPold.SetValue(ciV, VPold.Value(ciV) - 1);
        }
      }
      else
      {
        int dParMi = std::abs(VPold.Value(ciV) - VPold.Value(ciV - 1));
        int dParMa = std::abs(VPold.Value(ciV) - VPold.Value(ciV + 1));
        if (dParMi > 10)
        {
          removePrev = true;
          VPold.SetValue(ciV, VPold.Value(ciV) - 1);
        }

        if (dParMa > 10)
        {
          removeNext = true;
          for (iPo = (ciV + 1); iPo <= cnbV; iPo++)
          {
            if (dParMi > 10)
              VPold.SetValue(iPo, VPold.Value(iPo) - 2);
            else
              VPold.SetValue(iPo, VPold.Value(iPo) - 1);
          }
        }
        else
        {
          if (dParMi > 10)
            for (iPo = (ciV + 1); iPo <= cnbV; iPo++)
              VPold.SetValue(iPo, VPold.Value(iPo) - 1);
        }
      }

      int pI = ciVpar;

      int iP;
      for (iP = 1; iP <= cNbP; iP++)
      {
        if (pI == iP)
        {
          IntSurf_PntOn2S newPnt = MakeNewPoint(replacePnt, theWLline->Point(iP), thePeriodsArr);
          newL2s->Add(newPnt);
        }
        else if (removeNext && iP == (pI + 1))
          continue;
        else if (removePrev && iP == (pI - 1))
          continue;
        else
          newL2s->Add(theWLline->Point(iP));
      }

      IntPatch_Point newVtx;
      gp_Pnt         Pnt3dV = aWLine->Vertex(VDMin).Value();
      newVtx.SetValue(Pnt3dV, theTol, false);
      newVtx.SetParameters(u21, v21, u22, v22);
      newVtx.SetParameter(VPold.Value(ciV));

      occ::handle<IntPatch_WLine> NWLine = new IntPatch_WLine(newL2s, false, theTrans1, theTrans2);
      NWLine->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

      int iV;
      for (iV = 1; iV <= cnbV; iV++)
      {
        if (iV == ciV)
          NWLine->AddVertex(newVtx);
        else
        {
          IntPatch_Point theVtx = theWLline->Vertex(iV);
          theVtx.SetParameter(VPold.Value(iV));
          NWLine->AddVertex(theVtx);
        }
      }

      theWLline = NWLine;
    } // if( VDMin != 0 )
  } // for( ciV = 1; ciV <= cnbV; ciV++ )
}

//=======================================================================
// function : DublicateOfLinesProcessing
// purpose  : Decides, if rejecting current line is necessary
//=======================================================================
static void DublicateOfLinesProcessing(const IntWalk_PWalking&                           thePW,
                                       const int                                         theWLID,
                                       NCollection_Sequence<occ::handle<IntPatch_Line>>& theLines,
                                       bool& theIsRejectReq)
{
  const occ::handle<IntPatch_WLine>& anExistWL =
    *((occ::handle<IntPatch_WLine>*)&theLines.Value(theWLID));
  const int aNbPrevPoints = anExistWL->NbPnts();
  const int aNbCurrPoints = thePW.NbPoints();

  if (aNbPrevPoints < aNbCurrPoints)
  { // Remove preview line
    theLines.Remove(theWLID);
    theIsRejectReq = false;
  }
  else if (aNbPrevPoints == aNbCurrPoints)
  {
    double aLPrev = 0.0, aLCurr = 0.0;
    for (int aNbPP = 1; aNbPP < aNbPrevPoints; aNbPP++)
    {
      const gp_Pnt aP1prev(anExistWL->Point(aNbPP).Value()),
        aP2prev(anExistWL->Point(aNbPP + 1).Value());
      const gp_Pnt aP1curr(thePW.Value(aNbPP).Value()), aP2curr(thePW.Value(aNbPP + 1).Value());

      aLPrev += aP1prev.Distance(aP2prev);
      aLCurr += aP1curr.Distance(aP2curr);
    }

    if (aLPrev < aLCurr)
    { // Remove preview line
      theLines.Remove(theWLID);
      theIsRejectReq = false;
    }
  }
}

//=================================================================================================

IntPatch_PrmPrmIntersection::IntPatch_PrmPrmIntersection()
    : done(false),
      empt(true)
{
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment)
{
  IntPatch_Polyhedron Poly1(Surf1, D1->NbSamplesU(), D1->NbSamplesV());
  Perform(Surf1, Poly1, D1, TolTangency, Epsilon, Deflection, Increment);
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const IntPatch_Polyhedron&              Poly1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const occ::handle<Adaptor3d_Surface>&   Surf2,
                                          const occ::handle<Adaptor3d_TopolTool>& D2,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment)
{
  IntPatch_Polyhedron Poly2(Surf2);
  Perform(Surf1, Poly1, D1, Surf2, Poly2, D2, TolTangency, Epsilon, Deflection, Increment);
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const occ::handle<Adaptor3d_Surface>&   Surf2,
                                          const IntPatch_Polyhedron&              Poly2,
                                          const occ::handle<Adaptor3d_TopolTool>& D2,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment)
{
  IntPatch_Polyhedron Poly1(Surf1);
  Perform(Surf1, Poly1, D1, Surf2, Poly2, D2, TolTangency, Epsilon, Deflection, Increment);
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const IntPatch_Polyhedron&              Poly1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const occ::handle<Adaptor3d_Surface>&   Surf2,
                                          const IntPatch_Polyhedron&              Poly2,
                                          const occ::handle<Adaptor3d_TopolTool>& D2,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment)
{
  IntPatch_InterferencePolyhedron Interference(Poly1, Poly2);
  empt = true;
  done = true;
  SLin.Clear();

  int nbLigSec = Interference.NbSectionLines();
  int nbTanZon = Interference.NbTangentZones();

  int NbLigCalculee = 0;

  double U1, U2, V1, V2;
  double pu1, pu2, pv1, pv2;

  NCollection_Array1<double> StartParams(1, 4);

  IntWalk_PWalking PW(Surf1, Surf2, TolTangency, Epsilon, Deflection, Increment);

  double SeuildPointLigne = 15.0 * Increment * Increment; //-- 10 est insuffisant
  double incidence;
  double dminiPointLigne;

  bool HasStartPoint, RejetLigne;

  IntSurf_PntOn2S StartPOn2S;

  int ver;

  gp_Pnt Point3dDebut, Point3dFin;

  if (nbLigSec >= 1)
  {
    int* TabL = new int[nbLigSec + 1];
    int  ls;
    for (ls = 1; ls <= nbLigSec; ls++)
      TabL[ls] = ls;

    bool triok;
    do
    {
      triok = true;
      for (int b = 2; b <= nbLigSec; b++)
      {
        int nb_B = Interference.LineValue(TabL[b]).NumberOfPoints();
        int nb_A = Interference.LineValue(TabL[b - 1]).NumberOfPoints();
        if (nb_B > nb_A)
        {
          int tyu     = TabL[b];
          TabL[b]     = TabL[b - 1];
          TabL[b - 1] = tyu;
          triok       = false;
        }
      }
    } while (!triok);

    for (ls = 1; ls <= nbLigSec; ls++)
    {
      const Intf_SectionLine& LineSec = Interference.LineValue(TabL[ls]);
      int                     nbp     = LineSec.NumberOfPoints();

      int* TabPtDep = new int[nbp + 1];
      int  ilig;
      for (ilig = 1; ilig <= nbp; ilig++)
        TabPtDep[ilig] = 0;

      double UminLig1, VminLig1, UmaxLig1, VmaxLig1;
      double UminLig2, VminLig2, UmaxLig2, VmaxLig2;

      SectionPointToParameters(LineSec.GetPoint(1),
                               Poly1,
                               Poly2,
                               UminLig1,
                               VminLig1,
                               UminLig2,
                               VminLig2);

      UmaxLig1 = UminLig1;
      VmaxLig1 = VminLig1;
      UmaxLig2 = UminLig2;
      VmaxLig2 = VminLig2;

      for (ilig = 2; ilig <= nbp; ilig++)
      {
        SectionPointToParameters(LineSec.GetPoint(ilig), Poly1, Poly2, U1, V1, U2, V2);

        if (U1 > UmaxLig1)
          UmaxLig1 = U1;
        if (V1 > VmaxLig1)
          VmaxLig1 = V1;
        if (U2 > UmaxLig2)
          UmaxLig2 = U2;
        if (V2 > VmaxLig2)
          VmaxLig2 = V2;

        if (U1 < UminLig1)
          UminLig1 = U1;
        if (V1 < VminLig1)
          VminLig1 = V1;
        if (U2 < UminLig2)
          UminLig2 = U2;
        if (V2 < VminLig2)
          VminLig2 = V2;
      }

      int  nbps2                               = (nbp > 3) ? (nbp / 2) : 1;
      int  NombreDePointsDeDepartDuCheminement = 0;
      int  IndicePointdeDepart1 = 0, IndicePointdeDepart2 = 0;
      bool lignetrouvee = false;

      do
      {
        NombreDePointsDeDepartDuCheminement++;
        if (NombreDePointsDeDepartDuCheminement == 1)
        {
          incidence  = 3.0;
          int nbp1_4 = nbp / 4;
          int nbp3_4 = nbp - nbp1_4;

          int nsp;
          for (nsp = nbp / 2; nsp < nbp3_4; nsp++)
          {
            double CurrentIncidence = LineSec.GetPoint(nsp).Incidence();
            if (CurrentIncidence < incidence)
            {
              nbps2     = nsp;
              incidence = 0.9 * CurrentIncidence;
            }
          }

          for (nsp = nbp / 2; nsp > nbp1_4; nsp--)
          {
            double CurrentIncidence = LineSec.GetPoint(nsp).Incidence();
            if (CurrentIncidence < incidence)
            {
              nbps2     = nsp;
              incidence = 0.9 * CurrentIncidence;
            }
          }

          if (nbp < 3)
            NombreDePointsDeDepartDuCheminement = 3;

          IndicePointdeDepart1 = nbps2;
        }
        else if (NombreDePointsDeDepartDuCheminement == 2)
        {
          if (IndicePointdeDepart1 == 1)
          {
            nbps2                = nbp / 2;
            IndicePointdeDepart2 = nbps2;
          }
          else
          {
            nbps2                = 1;
            IndicePointdeDepart2 = 1;
          }
        }
        else if (NombreDePointsDeDepartDuCheminement == 3)
        {
          if (IndicePointdeDepart1 == nbp)
            nbps2 = (IndicePointdeDepart1 + IndicePointdeDepart2) / 2;
          else
            nbps2 = nbp;
        }
        else
        {
          nbps2 = NombreDePointsDeDepartDuCheminement - 3;
          NombreDePointsDeDepartDuCheminement++;
        }

        if (TabPtDep[nbps2] == 0)
        {
          TabPtDep[nbps2] = 1;
          SectionPointToParameters(LineSec.GetPoint(nbps2), Poly1, Poly2, U1, V1, U2, V2);

          StartParams(1) = U1;
          StartParams(2) = V1;
          StartParams(3) = U2;
          StartParams(4) = V2;

          HasStartPoint   = PW.PerformFirstPoint(StartParams, StartPOn2S);
          dminiPointLigne = SeuildPointLigne + SeuildPointLigne;

          if (HasStartPoint)
          {
            StartPOn2S.Parameters(pu1, pv1, pu2, pv2);
            NbLigCalculee = SLin.Length();
            int l;
            for (l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
            {
              const occ::handle<IntPatch_WLine>& testwline =
                *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));

              if (IsPointOnLine(StartPOn2S, testwline, Deflection))
              {
                dminiPointLigne = 0.0;
              }
            } // for ( l ...

            if (dminiPointLigne > SeuildPointLigne)
            {
              PW.Perform(StartParams,
                         UminLig1,
                         VminLig1,
                         UminLig2,
                         VminLig2,
                         UmaxLig1,
                         VmaxLig1,
                         UmaxLig2,
                         VmaxLig2);
              if (PW.IsDone())
              {
                if (PW.NbPoints() > 2)
                {
                  RejetLigne                      = false;
                  Point3dDebut                    = PW.Value(1).Value();
                  const IntSurf_PntOn2S& PointFin = PW.Value(PW.NbPoints());
                  Point3dFin                      = PointFin.Value();
                  for (ver = 1; ver <= NbLigCalculee; ver++)
                  {
                    const occ::handle<IntPatch_WLine>& verwline =
                      *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));

                    // Check end point if it is on existing line.
                    // Start point is checked before.
                    if (IsPointOnLine(PointFin, verwline, Deflection))
                    {
                      RejetLigne = true;
                      break;
                    }

                    const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
                    const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
                    if ((Point3dDebut.Distance(verPointDebut.Value()) <= TolTangency)
                        && (Point3dFin.Distance(verPointFin.Value()) <= TolTangency))
                    {
                      RejetLigne = true;
                      break;
                    }
                  }

                  if (RejetLigne)
                  {
                    DublicateOfLinesProcessing(PW, ver, SLin, RejetLigne);
                  }

                  if (!RejetLigne)
                  {
                    // Calculation transition
                    IntSurf_TypeTrans trans1, trans2;
                    double            locu, locv;
                    gp_Vec            norm1, norm2, d1u, d1v;
                    gp_Pnt            ptbid;
                    int               indextg;
                    gp_Vec            tgline(PW.TangentAtLine(indextg));
                    PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
                    Surf1->D1(locu, locv, ptbid, d1u, d1v);
                    norm1 = d1u.Crossed(d1v);
                    PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
                    Surf2->D1(locu, locv, ptbid, d1u, d1v);
                    norm2 = d1u.Crossed(d1v);
                    if (tgline.DotCross(norm2, norm1) > 0.)
                    {
                      trans1 = IntSurf_Out;
                      trans2 = IntSurf_In;
                    }
                    else
                    {
                      trans1 = IntSurf_In;
                      trans2 = IntSurf_Out;
                    }

                    double                      TolTang = TolTangency;
                    occ::handle<IntPatch_WLine> wline =
                      new IntPatch_WLine(PW.Line(), false, trans1, trans2);
                    wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

                    // the method PutVertexOnLine can reduce the number of points in <wline>
                    IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
                    if (wline->NbPnts() < 2)
                      continue;
                    IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
                    if (wline->NbPnts() < 2)
                      continue;

                    if (wline->NbVertex() == 0)
                    {
                      IntPatch_Point  vtx;
                      IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                      POn2S.Parameters(pu1, pv1, pu2, pv2);
                      vtx.SetValue(Point3dDebut, TolTang, false);
                      vtx.SetParameters(pu1, pv1, pu2, pv2);
                      vtx.SetParameter(1);
                      wline->AddVertex(vtx);

                      POn2S = PW.Line()->Value(wline->NbPnts());
                      POn2S.Parameters(pu1, pv1, pu2, pv2);
                      vtx.SetValue(Point3dFin, TolTang, false);
                      vtx.SetParameters(pu1, pv1, pu2, pv2);
                      vtx.SetParameter(wline->NbPnts());
                      wline->AddVertex(vtx);
                    }

                    lignetrouvee = true;
                    AddWLine(SLin, wline, Deflection);
                    empt = false;
                  } // !RejetLigne
                } // PW.NbPoints()>2
              } // done is True
            } // dminiPointLigne > SeuildPointLigne
          } // HasStartPoint
        } // TabPtDep[nbps2]==0
      } while (nbp > 5
               && (((NombreDePointsDeDepartDuCheminement < 3) || !lignetrouvee)
                   && ((NombreDePointsDeDepartDuCheminement - 3 < nbp) || lignetrouvee)));

      delete[] TabPtDep;
    } // for( ls ...

    delete[] TabL;
  } // nbLigSec >= 1

  double UminLig1, VminLig1, UmaxLig1, VmaxLig1;
  double UminLig2, VminLig2, UmaxLig2, VmaxLig2;

  UminLig1 = VminLig1 = UminLig2 = VminLig2 = RealLast();
  UmaxLig1 = VmaxLig1 = UmaxLig2 = VmaxLig2 = -UminLig1;

  int z;
  for (z = 1; z <= nbTanZon; z++)
  {
    const Intf_TangentZone& TangentZone = Interference.ZoneValue(z);
    for (int pz = 1; pz <= TangentZone.NumberOfPoints(); pz++)
    {
      SectionPointToParameters(TangentZone.GetPoint(pz), Poly1, Poly2, U1, V1, U2, V2);

      if (U1 > UmaxLig1)
        UmaxLig1 = U1;
      if (V1 > VmaxLig1)
        VmaxLig1 = V1;
      if (U2 > UmaxLig2)
        UmaxLig2 = U2;
      if (V2 > VmaxLig2)
        VmaxLig2 = V2;

      if (U1 < UminLig1)
        UminLig1 = U1;
      if (V1 < VminLig1)
        VminLig1 = V1;
      if (U2 < UminLig2)
        UminLig2 = U2;
      if (V2 < VminLig2)
        VminLig2 = V2;
    }
  }

  for (z = 1; z <= nbTanZon; z++)
  {
    const Intf_TangentZone& TangentZone = Interference.ZoneValue(z);
    int                     pz;
    for (pz = 1; pz <= TangentZone.NumberOfPoints(); pz++)
    {
      SectionPointToParameters(TangentZone.GetPoint(pz), Poly1, Poly2, U1, V1, U2, V2);

      StartParams(1) = U1;
      StartParams(2) = V1;
      StartParams(3) = U2;
      StartParams(4) = V2;

      HasStartPoint = PW.PerformFirstPoint(StartParams, StartPOn2S);
      if (HasStartPoint)
      {
        StartPOn2S.Parameters(pu1, pv1, pu2, pv2);
        NbLigCalculee   = SLin.Length();
        dminiPointLigne = SeuildPointLigne + SeuildPointLigne;
        int l;
        for (l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
        {
          const occ::handle<IntPatch_WLine>& testwline =
            *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));

          if (IsPointOnLine(StartPOn2S, testwline, Deflection))
          {
            dminiPointLigne = 0.0;
          }
        } // for( l ...

        if (dminiPointLigne > SeuildPointLigne)
        {
          PW.Perform(StartParams,
                     UminLig1,
                     VminLig1,
                     UminLig2,
                     VminLig2,
                     UmaxLig1,
                     VmaxLig1,
                     UmaxLig2,
                     VmaxLig2);
          if (PW.IsDone())
          {
            if (PW.NbPoints() > 2)
            {
              RejetLigne                      = false;
              Point3dDebut                    = PW.Value(1).Value();
              const IntSurf_PntOn2S& PointFin = PW.Value(PW.NbPoints());
              Point3dFin                      = PointFin.Value();
              for (ver = 1; ver <= NbLigCalculee; ver++)
              {
                const occ::handle<IntPatch_WLine>& verwline =
                  *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));

                // Check end point if it is on existing line.
                // Start point is checked before.
                if (IsPointOnLine(PointFin, verwline, Deflection))
                {
                  RejetLigne = true;
                  break;
                }

                const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
                const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
                if ((Point3dDebut.Distance(verPointDebut.Value()) < TolTangency)
                    || (Point3dFin.Distance(verPointFin.Value()) < TolTangency))
                {
                  RejetLigne = true;
                  break;
                }
              }

              if (RejetLigne)
              {
                DublicateOfLinesProcessing(PW, ver, SLin, RejetLigne);
              }

              if (!RejetLigne)
              {
                IntSurf_TypeTrans trans1, trans2;
                double            locu, locv;
                gp_Vec            norm1, norm2, d1u, d1v;
                gp_Pnt            ptbid;
                int               indextg;
                gp_Vec            tgline(PW.TangentAtLine(indextg));
                PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
                Surf1->D1(locu, locv, ptbid, d1u, d1v);
                norm1 = d1u.Crossed(d1v);
                PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
                Surf2->D1(locu, locv, ptbid, d1u, d1v);
                norm2 = d1u.Crossed(d1v);
                if (tgline.DotCross(norm2, norm1) > 0.)
                {
                  trans1 = IntSurf_Out;
                  trans2 = IntSurf_In;
                }
                else
                {
                  trans1 = IntSurf_In;
                  trans2 = IntSurf_Out;
                }

                double                      TolTang = TolTangency;
                occ::handle<IntPatch_WLine> wline =
                  new IntPatch_WLine(PW.Line(), false, trans1, trans2);
                wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

                // the method PutVertexOnLine can reduce the number of points in <wline>
                IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
                if (wline->NbPnts() < 2)
                  continue;
                IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
                if (wline->NbPnts() < 2)
                  continue;

                if (wline->NbVertex() == 0)
                {
                  IntPatch_Point  vtx;
                  IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                  POn2S.Parameters(pu1, pv1, pu2, pv2);
                  vtx.SetValue(Point3dDebut, TolTang, false);
                  vtx.SetParameters(pu1, pv1, pu2, pv2);
                  vtx.SetParameter(1);
                  wline->AddVertex(vtx);

                  POn2S = PW.Line()->Value(wline->NbPnts());
                  POn2S.Parameters(pu1, pv1, pu2, pv2);
                  vtx.SetValue(Point3dFin, TolTang, false);
                  vtx.SetParameters(pu1, pv1, pu2, pv2);
                  vtx.SetParameter(wline->NbPnts());
                  wline->AddVertex(vtx);
                }

                AddWLine(SLin, wline, Deflection);
                empt = false;
              } // if !RejetLigne
            } // PW.NbPoints()>2
          } // done is True
        } // dminiPointLigne > SeuildPointLigne
      } // HasStartPoint
    } // for( pz ...
  } // for( z ...
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const IntPatch_Polyhedron&              Poly1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment)
{
  IntPatch_InterferencePolyhedron Interference(Poly1);
  empt = true;
  done = true;
  SLin.Clear();

  int nbLigSec = Interference.NbSectionLines();
  int nbTanZon = Interference.NbTangentZones();

  int NbPntOn2SOnLine;
  int NbLigCalculee = 0;

  double U1, U2, V1, V2;
  double pu1, pu2, pv1, pv2;

  NCollection_Array1<double> StartParams(1, 4);
  IntWalk_PWalking           PW(Surf1, Surf1, TolTangency, Epsilon, Deflection, Increment);

  double SeuildPointLigne = 15.0 * Increment * Increment; //-- 10 est insuffisant
  double incidence;
  double dminiPointLigne;

  bool HasStartPoint, RejetLigne;

  IntSurf_PntOn2S StartPOn2S;

  int ver;

  gp_Pnt Point3dDebut, Point3dFin;

  if (nbLigSec >= 1)
  {
    int ls;
    for (ls = 1; ls <= nbLigSec; ls++)
    {
      const Intf_SectionLine& LineSec                             = Interference.LineValue(ls);
      int                     nbp                                 = LineSec.NumberOfPoints();
      int                     nbps2                               = (nbp > 3) ? (nbp / 2) : 1;
      int                     NombreDePointsDeDepartDuCheminement = 0;
      int                     IndicePointdeDepart1 = 0, IndicePointdeDepart2 = 0;
      do
      {
        NombreDePointsDeDepartDuCheminement++;
        if (NombreDePointsDeDepartDuCheminement == 1)
        {
          incidence = 0.0;
          int nsp1;
          for (nsp1 = nbp / 2; nsp1 >= 1; nsp1--)
          {
            SectionPointToParameters(LineSec.GetPoint(nsp1), Poly1, Poly1, U1, V1, U2, V2);
            double CurrentIncidence = std::abs(U1 - U2) + std::abs(V1 - V2);
            if (CurrentIncidence > incidence)
            {
              nbps2     = nsp1;
              incidence = CurrentIncidence;
            }
          }
          for (nsp1 = nbp / 2; nsp1 <= nbp; nsp1++)
          {
            SectionPointToParameters(LineSec.GetPoint(nsp1), Poly1, Poly1, U1, V1, U2, V2);
            double CurrentIncidence = std::abs(U1 - U2) + std::abs(V1 - V2);
            if (CurrentIncidence > incidence)
            {
              nbps2     = nsp1;
              incidence = CurrentIncidence;
            }
          }

          if (nbp < 3)
            NombreDePointsDeDepartDuCheminement = 3;

          IndicePointdeDepart1 = nbps2;
        }
        else if (NombreDePointsDeDepartDuCheminement == 2)
        {
          if (IndicePointdeDepart1 == 1)
          {
            nbps2                = nbp / 2;
            IndicePointdeDepart2 = nbps2;
          }
          else
          {
            nbps2                = 1;
            IndicePointdeDepart2 = 1;
          }
        }
        else
        {
          if (IndicePointdeDepart1 == nbp)
            nbps2 = (IndicePointdeDepart1 + IndicePointdeDepart2) / 2;
          else
            nbps2 = nbp;
        }

        SectionPointToParameters(LineSec.GetPoint(nbps2), Poly1, Poly1, U1, V1, U2, V2);

        StartParams(1) = U1;
        StartParams(2) = V1;
        StartParams(3) = U2;
        StartParams(4) = V2;

        HasStartPoint   = PW.PerformFirstPoint(StartParams, StartPOn2S);
        dminiPointLigne = SeuildPointLigne + SeuildPointLigne;
        if (HasStartPoint)
        {
          StartPOn2S.Parameters(pu1, pv1, pu2, pv2);
          if (std::abs(pu1 - pu2) > 1e-7 || std::abs(pv1 - pv2) > 1e-7)
          {
            NbLigCalculee = SLin.Length();
            int l;
            for (l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
            {
              const occ::handle<IntPatch_WLine>& testwline =
                *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));
              if ((!testwline->IsOutSurf1Box(gp_Pnt2d(pu1, pv1)))
                  && (!testwline->IsOutSurf2Box(gp_Pnt2d(pu2, pv2)))
                  && (!testwline->IsOutBox(StartPOn2S.Value())))
              {
                NbPntOn2SOnLine = testwline->NbPnts();
                int ll;
                for (ll = 1; (ll < NbPntOn2SOnLine) && (dminiPointLigne >= SeuildPointLigne); ll++)
                {
                  double t, Au1, Av1, Au2, Av2, Bu1, Bv1, Bu2, Bv2;
                  testwline->Point(ll).Parameters(Au1, Av1, Au2, Av2);
                  testwline->Point(ll + 1).Parameters(Bu1, Bv1, Bu2, Bv2);
                  if (Au1 > Bu1)
                  {
                    t   = Au1;
                    Au1 = Bu1;
                    Bu1 = t;
                  }
                  if (Av1 > Bv1)
                  {
                    t   = Av1;
                    Av1 = Bv1;
                    Bv1 = t;
                  }
                  Au1 -= 1.0e-7;
                  Av1 -= 1.0e-7;
                  Bu1 += 1.0e-7;
                  Bv1 += 1.0e-7;

                  if ((pu1 >= Au1) && (pu1 <= Bu1) && (pv1 >= Av1) && (pv1 <= Bv1))
                    dminiPointLigne = 0.0;
                  else
                  {
                    if ((pu2 >= Au1) && (pu2 <= Bu1) && (pv2 >= Av1) && (pv2 <= Bv1))
                      dminiPointLigne = 0.0;
                  }
                } // for( ll ...
              } // if ...
            } // for( l ...

            if (dminiPointLigne > SeuildPointLigne)
            {
              PW.Perform(StartParams);
              if (PW.IsDone())
              {
                if (PW.NbPoints() > 2)
                {
                  RejetLigne   = false;
                  Point3dDebut = PW.Value(1).Value();
                  Point3dFin   = PW.Value(PW.NbPoints()).Value();
                  for (ver = 1; ver <= NbLigCalculee; ver++)
                  {
                    const occ::handle<IntPatch_WLine>& verwline =
                      *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));
                    const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
                    const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
                    if ((Point3dDebut.Distance(verPointDebut.Value()) < TolTangency)
                        || (Point3dFin.Distance(verPointFin.Value()) < TolTangency))
                    {
                      RejetLigne = true;
                      break;
                    }
                  }

                  if (RejetLigne)
                  {
                    DublicateOfLinesProcessing(PW, ver, SLin, RejetLigne);
                  }

                  if (!RejetLigne)
                  {
                    IntSurf_TypeTrans trans1, trans2;
                    double            locu, locv;
                    gp_Vec            norm1, norm2, d1u, d1v;
                    gp_Pnt            ptbid;
                    int               indextg;
                    gp_Vec            tgline(PW.TangentAtLine(indextg));
                    PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
                    Surf1->D1(locu, locv, ptbid, d1u, d1v);
                    norm1 = d1u.Crossed(d1v);
                    PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
                    Surf1->D1(locu, locv, ptbid, d1u, d1v);
                    norm2 = d1u.Crossed(d1v);
                    if (tgline.DotCross(norm2, norm1) > 0.)
                    {
                      trans1 = IntSurf_Out;
                      trans2 = IntSurf_In;
                    }
                    else
                    {
                      trans1 = IntSurf_In;
                      trans2 = IntSurf_Out;
                    }

                    IntSurf_LineOn2S LineOn2S;
                    int              nbpw, imin, imax, i;
                    nbpw = PW.Line()->NbPoints();
                    double u1, v1, u2, v2;
                    i = 0;
                    do
                    {
                      i++;
                      imin                      = i;
                      const IntSurf_PntOn2S& Pi = PW.Line()->Value(i);
                      Pi.Parameters(u1, v1, u2, v2);
                    } while ((i < nbpw)
                             && (std::abs(u1 - u2) <= 1e-6 && std::abs(v1 - v2) <= 1e-6));

                    if (imin > 2)
                      imin--;

                    i = nbpw + 1;
                    do
                    {
                      i--;
                      imax                      = i;
                      const IntSurf_PntOn2S& Pi = PW.Line()->Value(i);
                      Pi.Parameters(u1, v1, u2, v2);
                    } while ((i > 2) && (std::abs(u1 - u2) <= 1e-6 && std::abs(v1 - v2) <= 1e-6));

                    if (imax < nbpw)
                      imax++;

                    if (imin < imax)
                    {
                      occ::handle<IntSurf_LineOn2S> PWLine = new IntSurf_LineOn2S();
                      for (i = imin; i <= imax; i++)
                        PWLine->Add(PW.Line()->Value(i));

                      double                      TolTang = TolTangency;
                      occ::handle<IntPatch_WLine> wline =
                        new IntPatch_WLine(PWLine, false, trans1, trans2);
                      wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

                      const IntSurf_PntOn2S& POn2SDeb = wline->Point(1);
                      const IntSurf_PntOn2S& POn2SFin = wline->Point(wline->NbPnts());
                      if ((POn2SDeb.Value()).Distance(POn2SFin.Value()) <= TolTangency)
                      {
                        double u1t, v1t, u2t, v2t;
                        POn2SDeb.Parameters(u1t, v1t, u2t, v2t);
                        IntPatch_Point vtx;
                        vtx.SetValue(POn2SDeb.Value(), TolTang, false);
                        vtx.SetParameters(u2t, v2t, u1t, v1t);
                        vtx.SetParameter(wline->NbPnts());
                        wline->SetPoint(wline->NbPnts(), vtx);
                      }
                      // the method PutVertexOnLine can reduce the number of points in <wline>
                      IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf1, true, TolTang);
                      if (wline->NbPnts() < 2)
                        continue;
                      if (wline->NbVertex() == 0)
                      {
                        IntPatch_Point  vtx;
                        IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                        POn2S.Parameters(pu1, pv1, pu2, pv2);
                        vtx.SetValue(Point3dDebut, TolTang, false);
                        vtx.SetParameters(pu1, pv1, pu2, pv2);
                        vtx.SetParameter(1);
                        wline->AddVertex(vtx);

                        POn2S = PW.Line()->Value(wline->NbPnts());
                        POn2S.Parameters(pu1, pv1, pu2, pv2);
                        vtx.SetValue(Point3dFin, TolTang, false);
                        vtx.SetParameters(pu1, pv1, pu2, pv2);
                        vtx.SetParameter(wline->NbPnts());
                        wline->AddVertex(vtx);
                      }
                      SLin.Append(wline);
                      empt = false;
                    } // imin<imax
                  } // !RejetLigne
                } // PW.NbPoints()>2
              } // done is True
            } // dminiPointLigne > SeuildPointLigne
          } // Abs || Abs
        } // HasStartPoint
      } while (nbp > 5 && NombreDePointsDeDepartDuCheminement < 3);
    } // for( ls ...
  } // nbLigSec>=1

  int z;
  for (z = 1; z <= nbTanZon; z++)
  {
    const Intf_TangentZone& TangentZone = Interference.ZoneValue(z);
    for (int pz = 1; pz <= TangentZone.NumberOfPoints(); pz++)
    {
      SectionPointToParameters(TangentZone.GetPoint(pz), Poly1, Poly1, U1, V1, U2, V2);

      StartParams(1) = U1;
      StartParams(2) = V1;
      StartParams(3) = U2;
      StartParams(4) = V2;

      HasStartPoint = PW.PerformFirstPoint(StartParams, StartPOn2S);
      if (HasStartPoint)
      {
        StartPOn2S.Parameters(pu1, pv1, pu2, pv2);
        if (std::abs(pu1 - pu2) > 1e-7 || std::abs(pv1 - pv2) > 1e-7)
        {
          NbLigCalculee   = SLin.Length();
          dminiPointLigne = SeuildPointLigne + SeuildPointLigne;
          int l;
          for (l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
          {
            const occ::handle<IntPatch_WLine>& testwline =
              *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));
            if ((!testwline->IsOutSurf1Box(gp_Pnt2d(pu1, pv1)))
                && (!testwline->IsOutSurf2Box(gp_Pnt2d(pu2, pv2)))
                && (!testwline->IsOutBox(StartPOn2S.Value())))
            {
              NbPntOn2SOnLine = testwline->NbPnts();
              int ll;
              for (ll = 1; (ll < NbPntOn2SOnLine) && (dminiPointLigne >= SeuildPointLigne); ll++)
              {
                double t, Au1, Av1, Au2, Av2, Bu1, Bv1, Bu2, Bv2;
                testwline->Point(ll).Parameters(Au1, Av1, Au2, Av2);
                testwline->Point(ll + 1).Parameters(Bu1, Bv1, Bu2, Bv2);
                if (Au1 > Bu1)
                {
                  t   = Au1;
                  Au1 = Bu1;
                  Bu1 = t;
                }
                if (Av1 > Bv1)
                {
                  t   = Av1;
                  Av1 = Bv1;
                  Bv1 = t;
                }
                Au1 -= 1.0e-7;
                Av1 -= 1.0e-7;
                Bu1 += 1.0e-7;
                Bv1 += 1.0e-7;
                if ((pu1 >= Au1) && (pu1 <= Bu1) && (pv1 >= Av1) && (pv1 <= Bv1))
                  dminiPointLigne = 0.0;
                else
                {
                  if ((pu2 >= Au1) && (pu2 <= Bu1) && (pv2 >= Av1) && (pv2 <= Bv1))
                    dminiPointLigne = 0.0;
                }
              } // for( ll ...
            } // if ...
          } // for( l ...

          if (dminiPointLigne > SeuildPointLigne)
          {
            PW.Perform(StartParams);
            if (PW.IsDone())
            {
              if (PW.NbPoints() > 2)
              {
                RejetLigne   = false;
                Point3dDebut = PW.Value(1).Value();
                Point3dFin   = PW.Value(PW.NbPoints()).Value();
                for (ver = 1; ver <= NbLigCalculee; ver++)
                {
                  const occ::handle<IntPatch_WLine>& verwline =
                    *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));
                  const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
                  const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
                  if ((Point3dDebut.Distance(verPointDebut.Value()) < TolTangency)
                      || (Point3dFin.Distance(verPointFin.Value()) < TolTangency))
                  {
                    RejetLigne = true;
                    break;
                  }
                }

                if (RejetLigne)
                {
                  DublicateOfLinesProcessing(PW, ver, SLin, RejetLigne);
                }

                if (!RejetLigne)
                {
                  IntSurf_TypeTrans trans1, trans2;
                  double            locu, locv;
                  gp_Vec            norm1, norm2, d1u, d1v;
                  gp_Pnt            ptbid;
                  int               indextg;
                  gp_Vec            tgline(PW.TangentAtLine(indextg));
                  PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
                  Surf1->D1(locu, locv, ptbid, d1u, d1v);
                  norm1 = d1u.Crossed(d1v);
                  PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
                  Surf1->D1(locu, locv, ptbid, d1u, d1v);
                  norm2 = d1u.Crossed(d1v);
                  if (tgline.DotCross(norm2, norm1) > 0.)
                  {
                    trans1 = IntSurf_Out;
                    trans2 = IntSurf_In;
                  }
                  else
                  {
                    trans1 = IntSurf_In;
                    trans2 = IntSurf_Out;
                  }

                  IntSurf_LineOn2S LineOn2S;
                  int              nbp, imin, imax, i;
                  nbp = PW.Line()->NbPoints();
                  double u1, v1, u2, v2;
                  i = 0;
                  do
                  {
                    i++;
                    imin                      = i;
                    const IntSurf_PntOn2S& Pi = PW.Line()->Value(i);
                    Pi.Parameters(u1, v1, u2, v2);
                  } while ((i < nbp) && (std::abs(u1 - u2) <= 1e-6 && std::abs(v1 - v2) <= 1e-6));

                  if (imin > 2)
                    imin--;

                  i = nbp + 1;
                  do
                  {
                    i--;
                    imax                      = i;
                    const IntSurf_PntOn2S& Pi = PW.Line()->Value(i);
                    Pi.Parameters(u1, v1, u2, v2);
                  } while ((i > 2) && (std::abs(u1 - u2) <= 1e-6 && std::abs(v1 - v2) <= 1e-6));

                  if (imax < nbp)
                    imax++;

                  if (imin < imax)
                  {
                    occ::handle<IntSurf_LineOn2S> PWLine = new IntSurf_LineOn2S();
                    for (i = imin; i <= imax; i++)
                      PWLine->Add(PW.Line()->Value(i));

                    double                      TolTang = TolTangency;
                    occ::handle<IntPatch_WLine> wline =
                      new IntPatch_WLine(PWLine, false, trans1, trans2);
                    wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

                    const IntSurf_PntOn2S& POn2SDeb = wline->Point(1);
                    const IntSurf_PntOn2S& POn2SFin = wline->Point(wline->NbPnts());
                    if ((POn2SDeb.Value()).Distance(POn2SFin.Value()) <= TolTangency)
                    {
                      double u1t, v1t, u2t, v2t;
                      POn2SDeb.Parameters(u1t, v1t, u2t, v2t);
                      IntPatch_Point vtx;
                      vtx.SetValue(POn2SDeb.Value(), TolTang, false);
                      vtx.SetParameters(u2t, v2t, u1t, v1t);
                      vtx.SetParameter(wline->NbPnts());
                      wline->SetPoint(wline->NbPnts(), vtx);
                    }

                    // the method PutVertexOnLine can reduce the number of points in <wline>
                    IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf1, true, TolTang);
                    if (wline->NbPnts() < 2)
                      continue;

                    if (wline->NbVertex() == 0)
                    {
                      IntPatch_Point  vtx;
                      IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                      POn2S.Parameters(pu1, pv1, pu2, pv2);
                      vtx.SetValue(Point3dDebut, TolTang, false);
                      vtx.SetParameters(pu1, pv1, pu2, pv2);
                      vtx.SetParameter(1);
                      wline->AddVertex(vtx);

                      POn2S = PW.Line()->Value(wline->NbPnts());
                      POn2S.Parameters(pu1, pv1, pu2, pv2);
                      vtx.SetValue(Point3dFin, TolTang, false);
                      vtx.SetParameters(pu1, pv1, pu2, pv2);
                      vtx.SetParameter(wline->NbPnts());
                      wline->AddVertex(vtx);
                    }

                    SLin.Append(wline);
                    empt = false;
                  } // imin<imax
                } // !RejetLigne
              } // PW.NbPoints()>2
            } // done a True
          } // dminiPointLigne > SeuildPointLigne
        } // Abs || Abs
      } // HasStartPoint
    } // for ( pz ...
  } // for( z ...
}

//=================================================================================================

occ::handle<IntPatch_Line> IntPatch_PrmPrmIntersection::NewLine(
  const occ::handle<Adaptor3d_Surface>& Surf1,
  const occ::handle<Adaptor3d_Surface>& Surf2,
  const int                             NumLine,
  const int                             Low,
  const int                             High,
  const int                             NbPntsToInsert) const
{
  int NbPnts = NbPntsToInsert + High - Low;
  if (NumLine > NbLines() || NumLine < 1 || Low >= High)
    throw Standard_OutOfRange(" IntPatch_PrmPrmIntersection NewLine ");
  //------------------------------------------------------------------
  //--  Indice     :   Low       Low+1     I    I+1         High    --
  //--                                                              --
  //--  Abs.Curv.  :  S(Low)              S(I)  S(I+1)      S(High) --
  //--                                                              --
  //--                On echantillonne a abcisse curviligne         --
  //--                constante.                                    --
  //--                L abcisse est calculee sur les params U1,V1   --
  //------------------------------------------------------------------
  NCollection_Array1<double> U1(Low, High);
  NCollection_Array1<double> V1(Low, High);
  NCollection_Array1<double> U2(Low, High);
  NCollection_Array1<double> V2(Low, High);
  NCollection_Array1<double> AC(Low, High);

  double                      s, ds;
  occ::handle<IntPatch_WLine> TheLine = occ::down_cast<IntPatch_WLine>(Line(NumLine));
  const IntSurf_PntOn2S&      Point   = TheLine->Point(Low);
  double                      u1, v1, u2, v2;
  Point.Parameters(u1, v1, u2, v2);
  U1(Low) = u1;
  V1(Low) = v1;
  U2(Low) = u2;
  V2(Low) = v2;
  AC(Low) = 0.0;

  IntWalk_PWalking PW(Surf1, Surf2, 0.000001, 0.000001, 0.001, 0.001);

  int i;
  for (i = Low + 1; i <= High; i++)
  {
    const IntSurf_PntOn2S& Pointi = TheLine->Point(i);
    Pointi.Parameters(u1, v1, u2, v2);
    U1(i) = u1;
    V1(i) = v1;
    U2(i) = u2;
    V2(i) = v2;

    double du1 = u1 - U1(i - 1);
    double dv1 = v1 - V1(i - 1);

    AC(i) = AC(i - 1) + std::sqrt((du1 * du1) + (dv1 * dv1));
  }

  occ::handle<IntSurf_LineOn2S> ResultPntOn2SLine = new IntSurf_LineOn2S();

  IntSurf_PntOn2S            StartPOn2S;
  NCollection_Array1<double> StartParams(1, 4);

  ResultPntOn2SLine->Add(TheLine->Point(Low));

  ds         = AC(High) / (NbPnts - 1);
  int Indice = Low;

  double dsmin = ds * 0.3;
  double smax  = AC(High);

  for (i = 2, s = ds; (i < NbPnts) && (s < smax); i++, s += ds)
  {
    while (AC(Indice + 1) <= s)
    {
      ResultPntOn2SLine->Add(TheLine->Point(Indice));
      Indice++;
    }
    double a   = s - AC(Indice);
    double b   = AC(Indice + 1) - s;
    double nab = 1.0 / (a + b);
    //----------------------------------------------------------
    //-- Verification :  Si Dist au prochain  point < dsmin   --
    //--                 Si Dist au precedent point < dsmin   --
    //--                                                      --
    //----------------------------------------------------------
    if ((nab > ds) && (a > dsmin) && (b > dsmin))
    {
      StartParams(1) = (U1(Indice) * b + U1(Indice + 1) * a) * nab;
      StartParams(2) = (V1(Indice) * b + V1(Indice + 1) * a) * nab;
      StartParams(3) = (U2(Indice) * b + U2(Indice + 1) * a) * nab;
      StartParams(4) = (V2(Indice) * b + V2(Indice + 1) * a) * nab;

      bool HasStartPoint = PW.PerformFirstPoint(StartParams, StartPOn2S);
      if (HasStartPoint)
        ResultPntOn2SLine->Add(StartPOn2S);
    }
    else
      s += dsmin;
  }

  ResultPntOn2SLine->Add(TheLine->Point(High));

  occ::handle<IntPatch_WLine> aRWL = new IntPatch_WLine(ResultPntOn2SLine, false);
  aRWL->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

  return (aRWL);
}

//=================================================================================================

void SectionPointToParameters(const Intf_SectionPoint&   Sp,
                              const IntPatch_Polyhedron& Poly1,
                              const IntPatch_Polyhedron& Poly2,
                              double&                    u1,
                              double&                    v1,
                              double&                    u2,
                              double&                    v2)
{
  Intf_PIType typ;
  int         Adr1, Adr2;
  double      Param, u, v;
  gp_Pnt      P(Sp.Pnt());

  int Pt1, Pt2, Pt3;

  Sp.InfoFirst(typ, Adr1, Adr2, Param);
  switch (typ)
  {
    case Intf_VERTEX: //-- Adr1 est le numero du vertex
    {
      Poly1.Parameters(Adr1, u1, v1);
      break;
    }
    case Intf_EDGE: {
      Poly1.Parameters(Adr1, u1, v1);
      Poly1.Parameters(Adr2, u, v);
      u1 += Param * (u - u1);
      v1 += Param * (v - v1);
      break;
    }
    case Intf_FACE: {
      double ua, va, ub, vb, uc, vc, ca, cb, cc, cabc;
      Poly1.Triangle(Adr1, Pt1, Pt2, Pt3);
      gp_Pnt PA(Poly1.Point(Pt1));
      gp_Pnt PB(Poly1.Point(Pt2));
      gp_Pnt PC(Poly1.Point(Pt3));
      Poly1.Parameters(Pt1, ua, va);
      Poly1.Parameters(Pt2, ub, vb);
      Poly1.Parameters(Pt3, uc, vc);
      gp_Vec Normale(gp_Vec(PA, PB).Crossed(gp_Vec(PA, PC)));
      cc   = (gp_Vec(PA, PB).Crossed(gp_Vec(PA, P))).Dot(Normale);
      ca   = (gp_Vec(PB, PC).Crossed(gp_Vec(PB, P))).Dot(Normale);
      cb   = (gp_Vec(PC, PA).Crossed(gp_Vec(PC, P))).Dot(Normale);
      cabc = ca + cb + cc;

      ca /= cabc;
      cb /= cabc;
      cc /= cabc;

      u1 = ca * ua + cb * ub + cc * uc;
      v1 = ca * va + cb * vb + cc * vc;
      break;
    }
    default: {
      //-- std::cout<<" Default dans SectionPointToParameters "<<std::endl;
      break;
    }
  }

  Sp.InfoSecond(typ, Adr1, Adr2, Param);
  switch (typ)
  {
    case Intf_VERTEX: //-- Adr1 est le numero du vertex
    {
      Poly2.Parameters(Adr1, u2, v2);
      break;
    }
    case Intf_EDGE: {
      Poly2.Parameters(Adr1, u2, v2);
      Poly2.Parameters(Adr2, u, v);
      u2 += Param * (u - u2);
      v2 += Param * (v - v2);
      break;
    }
    case Intf_FACE: {
      double ua, va, ub, vb, uc, vc, ca, cb, cc, cabc;
      Poly2.Triangle(Adr1, Pt1, Pt2, Pt3);
      gp_Pnt PA(Poly2.Point(Pt1));
      gp_Pnt PB(Poly2.Point(Pt2));
      gp_Pnt PC(Poly2.Point(Pt3));
      Poly2.Parameters(Pt1, ua, va);
      Poly2.Parameters(Pt2, ub, vb);
      Poly2.Parameters(Pt3, uc, vc);
      gp_Vec Normale(gp_Vec(PA, PB).Crossed(gp_Vec(PA, PC)));
      cc   = (gp_Vec(PA, PB).Crossed(gp_Vec(PA, P))).Dot(Normale);
      ca   = (gp_Vec(PB, PC).Crossed(gp_Vec(PB, P))).Dot(Normale);
      cb   = (gp_Vec(PC, PA).Crossed(gp_Vec(PC, P))).Dot(Normale);
      cabc = ca + cb + cc;

      ca /= cabc;
      cb /= cabc;
      cc /= cabc;

      u2 = ca * ua + cb * ub + cc * uc;
      v2 = ca * va + cb * vb + cc * vc;
      break;
    }
    default: {
      //-- std::cout<<" Default dans SectionPointToParameters "<<std::endl;
      break;
    }
  }
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::RemplitLin(const int                           x1,
                                             const int                           y1,
                                             const int                           z1,
                                             const int                           x2,
                                             const int                           y2,
                                             const int                           z2,
                                             IntPatch_PrmPrmIntersection_T3Bits& Map) const
{
  int xg, yg, zg;
  xg = x1 - x2;
  if (xg < 0)
    xg = -xg;
  yg = y1 - y2;
  if (yg < 0)
    yg = -yg;
  zg = z1 - z2;
  if (zg < 0)
    zg = -zg;
  if (DansGrille(x1) && DansGrille(y1) && DansGrille(z1))
  {
    int t = GrilleInteger(x1, y1, z1);
    Map.Add(t);
  }
  if (xg <= 1 && yg <= 1 && zg <= 1)
    return;
  xg = (x1 + x2) >> 1;
  yg = (y1 + y2) >> 1;
  zg = (z1 + z2) >> 1;
  RemplitLin(x1, y1, z1, xg, yg, zg, Map);
  RemplitLin(x2, y2, z2, xg, yg, zg, Map);
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::RemplitTri(const int                           x1,
                                             const int                           y1,
                                             const int                           z1,
                                             const int                           x2,
                                             const int                           y2,
                                             const int                           z2,
                                             const int                           x3,
                                             const int                           y3,
                                             const int                           z3,
                                             IntPatch_PrmPrmIntersection_T3Bits& Map) const
{
  if (x1 == x2 && x1 == x3 && y1 == y2 && y1 == y3 && z1 == z2 && z1 == z3)
  {
    if (DansGrille(x1) && DansGrille(y1) && DansGrille(z1))
    {
      int t = GrilleInteger(x1, y1, z1);
      Map.Add(t);
    }
    return;
  }
  else
  {
    int xg = (x1 + x2 + x3) / 3;
    int yg = (y1 + y2 + y3) / 3;
    int zg = (z1 + z2 + z3) / 3;
    if (xg == x1 && yg == y1 && zg == z1)
    {
      RemplitLin(x1, y1, z1, x2, y2, z2, Map);
      RemplitLin(x1, y1, z1, x3, y3, z3, Map);
      return;
    }
    if (xg == x2 && yg == y2 && zg == z2)
    {
      RemplitLin(x2, y2, z2, x1, y1, z1, Map);
      RemplitLin(x2, y2, z2, x3, y3, z3, Map);
      return;
    }
    if (xg == x3 && yg == y3 && zg == z3)
    {
      RemplitLin(x3, y3, z3, x2, y2, z2, Map);
      RemplitLin(x3, y3, z3, x1, y1, z1, Map);
      return;
    }
    if (DansGrille(xg) && DansGrille(yg) && DansGrille(zg))
    {
      int t = GrilleInteger(xg, yg, zg);
      Map.Add(t);
    }
    if (xg != x3 || yg != y3 || zg != z3)
      RemplitTri(x1, y1, z1, x2, y2, z2, xg, yg, zg, Map);
    if (xg != x1 || yg != y1 || zg != z1)
      RemplitTri(xg, yg, zg, x2, y2, z2, x3, y3, z3, Map);
    if (xg != x2 || yg != y2 || zg != z2)
      RemplitTri(x1, y1, z1, xg, yg, zg, x3, y3, z3, Map);
  }
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Remplit(const int                           a,
                                          const int                           b,
                                          const int                           c,
                                          IntPatch_PrmPrmIntersection_T3Bits& Map) const
{
  int iax, iay, iaz, ibx, iby, ibz, icx, icy, icz;
  if (a != -1)
    Map.Add(a);
  if (b != -1)
    Map.Add(b);
  if (c != -1)
    Map.Add(c);

  if (a != -1 && b != -1 && c != -1)
  {
    IntegerGrille(a, iax, iay, iaz);
    IntegerGrille(b, ibx, iby, ibz);
    IntegerGrille(c, icx, icy, icz);
    RemplitTri(iax, iay, iaz, ibx, iby, ibz, icx, icy, icz, Map);
  }
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const occ::handle<Adaptor3d_Surface>&   Surf2,
                                          const occ::handle<Adaptor3d_TopolTool>& D2,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment,
                                          NCollection_List<IntSurf_PntOn2S>&      LOfPnts)
{
  if (LOfPnts.IsEmpty())
  {
    done = true;
    return;
  }

  empt = true;
  SLin.Clear();

  double UminLig1, VminLig1, UmaxLig1, VmaxLig1;
  double UminLig2, VminLig2, UmaxLig2, VmaxLig2;
  double U1, U2, V1, V2;

  UminLig1 = Surf1->FirstUParameter();
  VminLig1 = Surf1->FirstVParameter();
  UmaxLig1 = Surf1->LastUParameter();
  VmaxLig1 = Surf1->LastVParameter();
  UminLig2 = Surf2->FirstUParameter();
  VminLig2 = Surf2->FirstVParameter();
  UmaxLig2 = Surf2->LastUParameter();
  VmaxLig2 = Surf2->LastVParameter();

  double Periods[4];
  Periods[0] = (Surf1->IsUPeriodic()) ? Surf1->UPeriod() : 0.;
  Periods[1] = (Surf1->IsVPeriodic()) ? Surf1->VPeriod() : 0.;
  Periods[2] = (Surf2->IsUPeriodic()) ? Surf2->UPeriod() : 0.;
  Periods[3] = (Surf2->IsVPeriodic()) ? Surf2->VPeriod() : 0.;

  NCollection_List<IntSurf_PntOn2S>::Iterator IterLOP1(LOfPnts);
  if (Surf1->IsUClosed() || Surf1->IsVClosed() || Surf2->IsUClosed() || Surf2->IsVClosed())
  {
    constexpr double                  TolPar = Precision::PConfusion();
    NCollection_List<IntSurf_PntOn2S> AdditionalPnts;
    double                            NewU1, NewV1, NewU2, NewV2;
    for (; IterLOP1.More(); IterLOP1.Next())
    {
      IntSurf_PntOn2S Pnt = IterLOP1.Value();
      Pnt.Parameters(U1, V1, U2, V2);
      IntSurf_PntOn2S NewPnt;
      if (Surf1->IsUClosed())
      {
        if (std::abs(U1 - Surf1->FirstUParameter()) <= TolPar)
        {
          NewU1 = Surf1->LastUParameter();
          NewPnt.SetValue(NewU1, V1, U2, V2);
          AdditionalPnts.Append(NewPnt);
        }
        else if (std::abs(U1 - Surf1->LastUParameter()) <= TolPar)
        {
          NewU1 = Surf1->FirstUParameter();
          NewPnt.SetValue(NewU1, V1, U2, V2);
          AdditionalPnts.Append(NewPnt);
        }
      }
      if (Surf1->IsVClosed())
      {
        if (std::abs(V1 - Surf1->FirstVParameter()) <= TolPar)
        {
          NewV1 = Surf1->LastVParameter();
          NewPnt.SetValue(U1, NewV1, U2, V2);
          AdditionalPnts.Append(NewPnt);
        }
        else if (std::abs(V1 - Surf1->LastVParameter()) <= TolPar)
        {
          NewV1 = Surf1->FirstVParameter();
          NewPnt.SetValue(U1, NewV1, U2, V2);
          AdditionalPnts.Append(NewPnt);
        }
      }
      if (Surf2->IsUClosed())
      {
        if (std::abs(U2 - Surf2->FirstUParameter()) <= TolPar)
        {
          NewU2 = Surf2->LastUParameter();
          NewPnt.SetValue(U1, V1, NewU2, V2);
          AdditionalPnts.Append(NewPnt);
        }
        else if (std::abs(U2 - Surf2->LastUParameter()) <= TolPar)
        {
          NewU2 = Surf2->FirstUParameter();
          NewPnt.SetValue(U1, V1, NewU2, V2);
          AdditionalPnts.Append(NewPnt);
        }
      }
      if (Surf2->IsVClosed())
      {
        if (std::abs(V2 - Surf2->FirstVParameter()) <= TolPar)
        {
          NewV2 = Surf2->LastVParameter();
          NewPnt.SetValue(U1, V1, U2, NewV2);
          AdditionalPnts.Append(NewPnt);
        }
        else if (std::abs(V2 - Surf2->LastVParameter()) <= TolPar)
        {
          NewV2 = Surf2->FirstVParameter();
          NewPnt.SetValue(U1, V1, U2, NewV2);
          AdditionalPnts.Append(NewPnt);
        }
      }
    }
    // Cut repeated points
    for (IterLOP1.Initialize(LOfPnts); IterLOP1.More(); IterLOP1.Next())
    {
      IntSurf_PntOn2S aPnt = IterLOP1.Value();
      aPnt.Parameters(U1, V1, U2, V2);
      NCollection_List<IntSurf_PntOn2S>::Iterator iter2(AdditionalPnts);
      while (iter2.More())
      {
        IntSurf_PntOn2S aNewPnt = iter2.Value();
        aNewPnt.Parameters(NewU1, NewV1, NewU2, NewV2);
        if (std::abs(U1 - NewU1) <= TolPar && std::abs(V1 - NewV1) <= TolPar
            && std::abs(U2 - NewU2) <= TolPar && std::abs(V2 - NewV2) <= TolPar)
          AdditionalPnts.Remove(iter2);
        else
          iter2.Next();
      }
    }

    LOfPnts.Append(AdditionalPnts);
  }

  for (IterLOP1.Initialize(LOfPnts); IterLOP1.More(); IterLOP1.Next())
  {
    IntSurf_PntOn2S Pnt = IterLOP1.Value();
    Pnt.Parameters(U1, V1, U2, V2);
    if (U1 > UmaxLig1)
      UmaxLig1 = U1;
    if (V1 > VmaxLig1)
      VmaxLig1 = V1;
    if (U2 > UmaxLig2)
      UmaxLig2 = U2;
    if (V2 > VmaxLig2)
      VmaxLig2 = V2;

    if (U1 < UminLig1)
      UminLig1 = U1;
    if (V1 < VminLig1)
      VminLig1 = V1;
    if (U2 < UminLig2)
      UminLig2 = U2;
    if (V2 < VminLig2)
      VminLig2 = V2;
  }

  double SeuildPointLigne = 15.0 * Increment * Increment;

  int             NbLigCalculee = 0, ver;
  double          pu1, pu2, pv1, pv2, dminiPointLigne;
  bool            HasStartPoint, RejetLigne;
  IntSurf_PntOn2S StartPOn2S;
  gp_Pnt          Point3dDebut, Point3dFin;

  NCollection_Array1<double> StartParams(1, 4);
  IntWalk_PWalking           PW(Surf1, Surf2, TolTangency, Epsilon, Deflection, Increment);

  NCollection_List<IntSurf_PntOn2S>::Iterator IterLOP2(LOfPnts);
  for (; IterLOP2.More(); IterLOP2.Next())
  {

    IntSurf_PntOn2S cPnt = IterLOP2.Value();
    cPnt.Parameters(U1, V1, U2, V2);

    StartParams(1) = U1;
    StartParams(2) = V1;
    StartParams(3) = U2;
    StartParams(4) = V2;

    HasStartPoint   = PW.PerformFirstPoint(StartParams, StartPOn2S);
    dminiPointLigne = SeuildPointLigne + SeuildPointLigne;
    if (HasStartPoint)
    {
      StartPOn2S.Parameters(pu1, pv1, pu2, pv2);
      NbLigCalculee = SLin.Length();
      int l;
      for (l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
      {
        const occ::handle<IntPatch_WLine>& testwline =
          *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));

        if (IsPointOnLine(StartPOn2S, testwline, Deflection))
        {
          dminiPointLigne = 0.0;
        }
      } // for( l ...

      if (dminiPointLigne > SeuildPointLigne)
      {
        PW.Perform(StartParams,
                   UminLig1,
                   VminLig1,
                   UminLig2,
                   VminLig2,
                   UmaxLig1,
                   VmaxLig1,
                   UmaxLig2,
                   VmaxLig2);
        if (PW.IsDone())
        {
          if (PW.NbPoints() > 2)
          {
            // Try to extend the intersection line to boundary, if it is possible.
            PW.PutToBoundary(Surf1, Surf2);

            RejetLigne                      = false;
            Point3dDebut                    = PW.Value(1).Value();
            const IntSurf_PntOn2S& PointFin = PW.Value(PW.NbPoints());
            Point3dFin                      = PointFin.Value();
            for (ver = 1; ver <= NbLigCalculee; ver++)
            {
              const occ::handle<IntPatch_WLine>& verwline =
                *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));

              // Check end point if it is on existing line.
              // Start point is checked before.
              if (IsPointOnLine(PointFin, verwline, Deflection))
              {
                RejetLigne = true;
                break;
              }

              const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
              const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
              if ((Point3dDebut.Distance(verPointDebut.Value()) <= TolTangency)
                  && (Point3dFin.Distance(verPointFin.Value()) <= TolTangency))
              {
                RejetLigne = true;
                break;
              }
            }

            if (RejetLigne)
            {
              DublicateOfLinesProcessing(PW, ver, SLin, RejetLigne);
            }

            if (!RejetLigne)
            {
              IntSurf_TypeTrans trans1, trans2;
              double            locu, locv;
              gp_Vec            norm1, norm2, d1u, d1v;
              gp_Pnt            ptbid;
              int               indextg;
              gp_Vec            tgline(PW.TangentAtLine(indextg));
              PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
              Surf1->D1(locu, locv, ptbid, d1u, d1v);
              norm1 = d1u.Crossed(d1v);
              PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
              Surf2->D1(locu, locv, ptbid, d1u, d1v);
              norm2 = d1u.Crossed(d1v);
              if (tgline.DotCross(norm2, norm1) >= 0.)
              {
                trans1 = IntSurf_Out;
                trans2 = IntSurf_In;
              }
              else
              {
                trans1 = IntSurf_In;
                trans2 = IntSurf_Out;
              }

              double                      TolTang = TolTangency;
              occ::handle<IntPatch_WLine> wline =
                new IntPatch_WLine(PW.Line(), false, trans1, trans2);
              wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

              // the method PutVertexOnLine can reduce the number of points in <wline>
              IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
              if (wline->NbPnts() < 2)
                continue;
              IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
              if (wline->NbPnts() < 2)
                continue;

              if (wline->NbVertex() == 0)
              {
                IntPatch_Point  vtx;
                IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                POn2S.Parameters(pu1, pv1, pu2, pv2);
                vtx.SetValue(Point3dDebut, TolTang, false);
                vtx.SetParameters(pu1, pv1, pu2, pv2);
                vtx.SetParameter(1);
                wline->AddVertex(vtx);

                POn2S = PW.Line()->Value(wline->NbPnts());
                POn2S.Parameters(pu1, pv1, pu2, pv2);
                vtx.SetValue(Point3dFin, TolTang, false);
                vtx.SetParameters(pu1, pv1, pu2, pv2);
                vtx.SetParameter(wline->NbPnts());
                wline->AddVertex(vtx);
              }

              SeveralWlinesProcessing(Surf1,
                                      Surf2,
                                      SLin,
                                      Periods,
                                      trans1,
                                      trans2,
                                      TolTang,
                                      std::max(PW.MaxStep(0), PW.MaxStep(1)),
                                      std::max(PW.MaxStep(2), PW.MaxStep(3)),
                                      wline);

              AddWLine(SLin, wline, Deflection);
              empt = false;
            } // !RejetLigne
          } // PW points > 2
        } // done is True
      } // dminiPointLigne > SeuildPointLigne
    } // HasStartPoint
  } // for( IterLOP ...
  done = true;
  return;
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const occ::handle<Adaptor3d_Surface>&   Surf2,
                                          const occ::handle<Adaptor3d_TopolTool>& D2,
                                          const double                            U1Depart,
                                          const double                            V1Depart,
                                          const double                            U2Depart,
                                          const double                            V2Depart,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment)
{
  //    int NbU1 = D1->NbSamplesU();
  //    int NbV1 = D1->NbSamplesV();
  //    int NbU2 = D2->NbSamplesU();
  //    int NbV2 = D2->NbSamplesV();

  //-- Traitement des Lignes de sections
  empt = true;
  done = true;
  SLin.Clear();

  //------------------------------------------------------------

  double pu1, pu2, pv1, pv2;

  NCollection_Array1<double> StartParams(1, 4);

  //    int MaxOscill = NbU1;
  //    if(MaxOscill < NbU2) MaxOscill=NbU2;
  //    if(MaxOscill < NbV1) MaxOscill=NbV1;
  //    if(MaxOscill < NbV2) MaxOscill=NbV2;

  //    double nIncrement=Increment;
  //    if(MaxOscill>10) {
  //  #ifdef OCCT_DEBUG
  //      std::cout<<"\n IntPatch_PrmPrmIntersection.gxx : Increment:"<<Increment<<" ->
  //      "<<Increment/(0.5*MaxOscill)<<std::endl;
  //  #endif
  //      nIncrement/=0.5*MaxOscill;
  //    }

  IntWalk_PWalking PW(Surf1, Surf2, TolTangency, Epsilon, Deflection,
                      Increment); // nIncrement);

  // double    SeuildPointLigne = 15.0 * Increment * Increment; //-- 10 est insuffisant
  // double    incidence;
  // double    dminiPointLigne;

  bool HasStartPoint; //,RejetLigne;

  IntSurf_PntOn2S StartPOn2S;

  // int ver;

  gp_Pnt Point3dDebut, Point3dFin;

  //------------------------------------------------------------

  StartParams(1) = U1Depart;
  StartParams(2) = V1Depart;
  StartParams(3) = U2Depart;
  StartParams(4) = V2Depart;

  //-----------------------------------------------------------------------
  //-- Calcul du premier point de cheminement a partir du point approche --
  //-----------------------------------------------------------------------
  HasStartPoint = PW.PerformFirstPoint(StartParams, StartPOn2S);
  if (HasStartPoint)
  {
    //-------------------------------------------------
    //-- Un point a ete trouve                       --
    //-- On verifie qu il n appartient pas           --
    //--  a une ligne de cheminement deja calculee.  --
    //-------------------------------------------------

    PW.Perform(StartParams);
    if (PW.IsDone())
    {
      if (PW.NbPoints() > 2)
      {
        Point3dDebut = PW.Value(1).Value();
        Point3dFin   = PW.Value(PW.NbPoints()).Value();

        IntSurf_TypeTrans trans1, trans2;
        double            locu, locv;
        gp_Vec            norm1, norm2, d1u, d1v;
        gp_Pnt            ptbid;
        int               indextg;
        gp_Vec            tgline(PW.TangentAtLine(indextg));
        PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
        Surf1->D1(locu, locv, ptbid, d1u, d1v);
        norm1 = d1u.Crossed(d1v);
        PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
        Surf2->D1(locu, locv, ptbid, d1u, d1v);
        norm2 = d1u.Crossed(d1v);
        if (tgline.DotCross(norm2, norm1) > 0.)
        {
          trans1 = IntSurf_Out;
          trans2 = IntSurf_In;
        }
        else
        {
          trans1 = IntSurf_In;
          trans2 = IntSurf_Out;
        }

        double                      TolTang = TolTangency;
        occ::handle<IntPatch_WLine> wline   = new IntPatch_WLine(PW.Line(), false, trans1, trans2);
        wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

        // the method PutVertexOnLine can reduce the number of points in <wline>
        IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
        if (wline->NbPnts() < 2)
          return;
        IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
        if (wline->NbPnts() < 2)
          return;

        //---------------
        if (wline->NbVertex() == 0)
        {
          IntPatch_Point  vtx;
          IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
          POn2S.Parameters(pu1, pv1, pu2, pv2);
          vtx.SetValue(Point3dDebut, TolTang, false);
          vtx.SetParameters(pu1, pv1, pu2, pv2);
          vtx.SetParameter(1);
          wline->AddVertex(vtx);

          POn2S = PW.Line()->Value(wline->NbPnts());
          POn2S.Parameters(pu1, pv1, pu2, pv2);
          vtx.SetValue(Point3dFin, TolTang, false);
          vtx.SetParameters(pu1, pv1, pu2, pv2);
          vtx.SetParameter(wline->NbPnts());
          wline->AddVertex(vtx);
        }

        //---------------
        SLin.Append(wline);
        empt = false;
      } // if(PW.NbPoints()>2)
    } // if(PW.IsDone())
  } // if(HasStartPoint)
}

//=================================================================================================

void AdjustOnPeriodic(const occ::handle<Adaptor3d_Surface>&             Surf1,
                      const occ::handle<Adaptor3d_Surface>&             Surf2,
                      NCollection_Sequence<occ::handle<IntPatch_Line>>& aSLin)
{
  bool   bIsPeriodic[4], bModified, bIsNull, bIsPeriod;
  int    i, j, k, aNbLines, aNbPx, aIndx, aIndq;
  double aPeriod[4], dPeriod[4], ux[4], uq[4], aEps, du;
  //
  aEps = Precision::Confusion();
  //
  for (k = 0; k < 4; ++k)
  {
    aPeriod[k] = 0.;
  }
  //
  bIsPeriodic[0] = Surf1->IsUPeriodic();
  bIsPeriodic[1] = Surf1->IsVPeriodic();
  bIsPeriodic[2] = Surf2->IsUPeriodic();
  bIsPeriodic[3] = Surf2->IsVPeriodic();
  //
  if (bIsPeriodic[0])
  {
    aPeriod[0] = Surf1->UPeriod();
  }
  if (bIsPeriodic[1])
  {
    aPeriod[1] = Surf1->VPeriod();
  }
  if (bIsPeriodic[2])
  {
    aPeriod[2] = Surf2->UPeriod();
  }
  if (bIsPeriodic[3])
  {
    aPeriod[3] = Surf2->VPeriod();
  }
  //
  for (k = 0; k < 4; ++k)
  {
    dPeriod[k] = 0.25 * aPeriod[k];
  }
  //
  aNbLines = aSLin.Length();
  for (i = 1; i <= aNbLines; ++i)
  {
    occ::handle<IntPatch_WLine>   aIL = occ::down_cast<IntPatch_WLine>(aSLin.Value(i));
    occ::handle<IntSurf_LineOn2S> aL  = aIL->Curve();

    aNbPx = aL->NbPoints();
    if (aNbPx < 10)
    {
      continue;
    }
    //
    for (j = 0; j < 2; ++j)
    {
      bModified = false;
      aIndx     = 1;
      aIndq     = 2;
      if (j)
      {
        aIndx = aNbPx;
        aIndq = aNbPx - 1;
      }
      //
      const IntSurf_PntOn2S& aPSx = aL->Value(aIndx);
      const IntSurf_PntOn2S& aPSq = aL->Value(aIndq);
      //
      aPSx.Parameters(ux[0], ux[1], ux[2], ux[3]);
      aPSq.Parameters(uq[0], uq[1], uq[2], uq[3]);
      //
      for (k = 0; k < 4; ++k)
      {
        bIsNull   = false;
        bIsPeriod = false;
        //
        if (!bIsPeriodic[k])
        {
          continue;
        }
        //
        if (fabs(ux[k]) < aEps)
        {
          bModified = true;
          bIsNull   = true;
        }
        //
        else if (fabs(ux[k] - aPeriod[k]) < aEps)
        {
          bModified = true;
          bIsPeriod = true;
        }
        //
        if (bModified)
        {
          du = fabs(ux[k] - uq[k]);
          if (du > dPeriod[k])
          {
            if (bIsNull)
            {
              ux[k] = aPeriod[k];
            }
            if (bIsPeriod)
            {
              ux[k] = 0.;
            }
          }
        }
      } // for (k=0; k<4; ++k)
      if (bModified)
      {
        IntSurf_PntOn2S aPntOn2S;
        //
        aPntOn2S = aPSx;
        aPntOn2S.SetValue(ux[0], ux[1], ux[2], ux[3]);
        aL->Value(aIndx, aPntOn2S);
      }
    } // for (j=0; j<1; ++j) {
  } // for (i=1; i<=aNbLines; ++i)
}

//=================================================================================================

IntSurf_PntOn2S MakeNewPoint(const IntSurf_PntOn2S& replacePnt,
                             const IntSurf_PntOn2S& oldPnt,
                             const double*          Periods)
{
  IntSurf_PntOn2S NewPoint;
  NewPoint.SetValue(replacePnt.Value());

  double OldParams[4], NewParams[4];
  oldPnt.Parameters(OldParams[0], OldParams[1], OldParams[2], OldParams[3]);
  replacePnt.Parameters(NewParams[0], NewParams[1], NewParams[2], NewParams[3]);

  int i;
  for (i = 0; i < 4; i++)
  {
    if (Periods[i] != 0.)
    {
      if (std::abs(NewParams[i] - OldParams[i]) >= 0.5 * Periods[i])
      {
        if (NewParams[i] < OldParams[i])
          NewParams[i] += Periods[i];
        else
          NewParams[i] -= Periods[i];
      }
    }
  }

  NewPoint.SetValue(NewParams[0], NewParams[1], NewParams[2], NewParams[3]);
  return NewPoint;
}

//=================================================================================================

void IntPatch_PrmPrmIntersection::Perform(const occ::handle<Adaptor3d_Surface>&   Surf1,
                                          const occ::handle<Adaptor3d_TopolTool>& D1,
                                          const occ::handle<Adaptor3d_Surface>&   Surf2,
                                          const occ::handle<Adaptor3d_TopolTool>& D2,
                                          const double                            TolTangency,
                                          const double                            Epsilon,
                                          const double                            Deflection,
                                          const double                            Increment,
                                          const bool                              ClearFlag)
{
  int Limit = 2500;
  int NbU1 = 10, NbV1 = 10, NbU2 = 10, NbV2 = 10;
  //
  D1->SamplePnts(Deflection, NbU1, NbV1);
  D2->SamplePnts(Deflection, NbU2, NbV2);
  //
  NbU1 = D1->NbSamplesU();
  NbV1 = D1->NbSamplesV();
  NbU2 = D2->NbSamplesU();
  NbV2 = D2->NbSamplesV();

  NCollection_Array1<double> anUpars1(1, NbU1), aVpars1(1, NbV1);
  NCollection_Array1<double> anUpars2(1, NbU2), aVpars2(1, NbV2);
  //
  D1->UParameters(anUpars1);
  D1->VParameters(aVpars1);
  D2->UParameters(anUpars2);
  D2->VParameters(aVpars2);

  double Periods[4];
  Periods[0] = (Surf1->IsUPeriodic()) ? Surf1->UPeriod() : 0.;
  Periods[1] = (Surf1->IsVPeriodic()) ? Surf1->VPeriod() : 0.;
  Periods[2] = (Surf2->IsUPeriodic()) ? Surf2->UPeriod() : 0.;
  Periods[3] = (Surf2->IsVPeriodic()) ? Surf2->VPeriod() : 0.;

  //---------------------------------------------
  if ((NbU1 * NbV1 <= Limit && NbV2 * NbU2 <= Limit))
  {
    empt = true;
    if (ClearFlag)
    {
      SLin.Clear();
    }
    //
    IntPolyh_Intersection* pInterference = nullptr;

    if (D1->IsUniformSampling() || D2->IsUniformSampling())
    {
      pInterference = new IntPolyh_Intersection(Surf1, NbU1, NbV1, Surf2, NbU2, NbV2);
    }
    else
    {
      pInterference = new IntPolyh_Intersection(Surf1, anUpars1, aVpars1, Surf2, anUpars2, aVpars2);
    }

    if (!pInterference)
    {
      done = false;
      return;
    }
    //
    IntPolyh_Intersection& Interference = *pInterference;
    //
    done = Interference.IsDone();
    if (!done)
    {
      delete pInterference;
      pInterference = nullptr;

      return;
    }

    int    nbLigSec         = Interference.NbSectionLines();
    int    nbTanZon         = Interference.NbTangentZones();
    double SeuildPointLigne = 15.0 * Increment * Increment;

    int             NbLigCalculee = 0, ver;
    double          pu1, pu2, pv1, pv2, incidence, dminiPointLigne;
    bool            HasStartPoint = false, RejectLine = false;
    IntSurf_PntOn2S StartPOn2S;
    gp_Pnt          Point3dDebut, Point3dFin;

    NCollection_Array1<double> StartParams(1, 4);
    IntWalk_PWalking           PW(Surf1, Surf2, TolTangency, Epsilon, Deflection, Increment);

    if (nbLigSec >= 1)
    {
      int* TabL = new int[nbLigSec + 1];
      for (int ls = 1; ls <= nbLigSec; ++ls)
      {
        TabL[ls] = ls;
      }
      //----------------------------------------1.1
      {
        bool triok;
        int  nb_A, nb_B, tyu;
        do
        {
          triok = true;
          for (int b = 2; b <= nbLigSec; ++b)
          {
            nb_B = Interference.NbPointsInLine(TabL[b]);
            nb_A = Interference.NbPointsInLine(TabL[b - 1]);

            if (nb_B > nb_A)
            {
              tyu         = TabL[b];
              TabL[b]     = TabL[b - 1];
              TabL[b - 1] = tyu;
              triok       = false;
            }
          }
        } while (!triok);
      }

      //----------------------------------------
      // 1.2 For the line "ls" get 2D-bounds U,V for surfaces 1,2
      //
      for (int ls = 1; ls <= nbLigSec; ++ls)
      {
        int nbp = Interference.NbPointsInLine(TabL[ls]);
        if (!nbp)
        {
          continue;
        }
        //
        int* TabPtDep = new int[nbp + 1];
        for (int ilig = 1; ilig <= nbp; ++ilig)
        {
          TabPtDep[ilig] = 0;
        }
        //
        double UminLig1, VminLig1, UmaxLig1, VmaxLig1;
        double UminLig2, VminLig2, UmaxLig2, VmaxLig2;
        double _x, _y, _z;
        //
        Interference
          .GetLinePoint(TabL[ls], 1, _x, _y, _z, UminLig1, VminLig1, UminLig2, VminLig2, incidence);

        UmaxLig1 = UminLig1;
        VmaxLig1 = VminLig1;
        UmaxLig2 = UminLig2;
        VmaxLig2 = VminLig2;
        //
        for (int ilig = 2; ilig <= nbp; ilig++)
        {
          double U1, U2, V1, V2;
          Interference.GetLinePoint(TabL[ls], ilig, _x, _y, _z, U1, V1, U2, V2, incidence);
          //
          if (U1 > UmaxLig1)
            UmaxLig1 = U1;
          if (V1 > VmaxLig1)
            VmaxLig1 = V1;
          if (U2 > UmaxLig2)
            UmaxLig2 = U2;
          if (V2 > VmaxLig2)
            VmaxLig2 = V2;
          //
          if (U1 < UminLig1)
            UminLig1 = U1;
          if (V1 < VminLig1)
            VminLig1 = V1;
          if (U2 < UminLig2)
            UminLig2 = U2;
          if (V2 < VminLig2)
            VminLig2 = V2;
        } // for( ilig = 2; ilig <= nbp; ilig++ ) {
        //
        //----------------------------------------
        // 1.3
        int       nbps2                               = (nbp > 3) ? (nbp / 2) : 1;
        int       NombreDePointsDeDepartDuCheminement = 0;
        bool      lignetrouvee                        = false;
        const int NbDePointsDeDepartDuChmLimit        = 5;
        //
        do
        {
          NombreDePointsDeDepartDuCheminement++;
          switch (NombreDePointsDeDepartDuCheminement)
          {
            case 1:
              nbps2 = (nbp > 1) ? nbp / 2 : 1;
              if (nbp < 3)
                NombreDePointsDeDepartDuCheminement = NbDePointsDeDepartDuChmLimit;

              break;
            case 2:
              nbps2 = 1;
              break;
            case 3:
              nbps2 = nbp - 1;
              break;

            case 4:
              nbps2 = 3 * nbp / 4;
              break;

            case 5:
              nbps2 = nbp / 4;
              break;
            default:
              nbps2 = NombreDePointsDeDepartDuCheminement - 3;
              NombreDePointsDeDepartDuCheminement++;
          }

          //
          if (TabPtDep[nbps2] == 0)
          {
            double U1, U2, V1, V2;

            TabPtDep[nbps2] = 1;
            Interference.GetLinePoint(TabL[ls], nbps2, _x, _y, _z, U1, V1, U2, V2, incidence);

            StartParams(1) = U1;
            StartParams(2) = V1;
            StartParams(3) = U2;
            StartParams(4) = V2;

            HasStartPoint   = PW.PerformFirstPoint(StartParams, StartPOn2S);
            dminiPointLigne = SeuildPointLigne + SeuildPointLigne;
            if (HasStartPoint)
            {
              StartPOn2S.Parameters(pu1, pv1, pu2, pv2);
              NbLigCalculee = SLin.Length();
              int l;
              for (l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
              {
                const occ::handle<IntPatch_WLine>& testwline =
                  *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));

                if (IsPointOnLine(StartPOn2S, testwline, Deflection))
                {
                  dminiPointLigne = 0.0;
                }
              } // for( l ...

              if (dminiPointLigne > SeuildPointLigne)
              {
                PW.Perform(StartParams,
                           UminLig1,
                           VminLig1,
                           UminLig2,
                           VminLig2,
                           UmaxLig1,
                           VmaxLig1,
                           UmaxLig2,
                           VmaxLig2);

                //
                bool   bPWIsDone;
                double aD11, aD12, aD21, aD22, aDx;
                //
                bPWIsDone = PW.IsDone();

                if (bPWIsDone)
                {
                  bool hasBeenAdded = false;
                  if (PW.NbPoints() > 2)
                  {
                    // Try to extend the intersection line to the boundary,
                    // if it is possibly
                    PW.PutToBoundary(Surf1, Surf2);
                    //
                    if (PW.NbPoints() < 3)
                      continue;

                    const int aMinNbPoints = 40;
                    if (PW.NbPoints() < aMinNbPoints)
                    {
                      hasBeenAdded = PW.SeekAdditionalPoints(Surf1, Surf2, aMinNbPoints);
                    }

                    int iPWNbPoints = PW.NbPoints(), aNbPointsVer = 0;
                    RejectLine   = false;
                    Point3dDebut = PW.Value(1).Value();
                    Point3dFin   = PW.Value(iPWNbPoints).Value();
                    for (ver = 1; (!RejectLine) && (ver <= NbLigCalculee); ++ver)
                    {
                      const occ::handle<IntPatch_WLine>& verwline =
                        *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));
                      //
                      aNbPointsVer = verwline->NbPnts();
                      if (aNbPointsVer < 3)
                      {
                        continue;
                      }
                      //
                      const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
                      const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
                      // xf
                      const gp_Pnt& aP21 = verPointDebut.Value();
                      const gp_Pnt& aP22 = verPointFin.Value();
                      //
                      aD11 = Point3dDebut.Distance(aP21);
                      aD12 = Point3dDebut.Distance(aP22);
                      aD21 = Point3dFin.Distance(aP21);
                      aD22 = Point3dFin.Distance(aP22);
                      //
                      if ((aD11 <= TolTangency && aD22 <= TolTangency)
                          || (aD12 <= TolTangency && aD21 <= TolTangency))
                      {
                        int m, mx;
                        //
                        mx = aNbPointsVer / 2;
                        if (aNbPointsVer % 2)
                        {
                          ++mx;
                        }
                        //
                        const gp_Pnt& aPx = verwline->Point(mx).Value();
                        for (m = 1; m < iPWNbPoints; ++m)
                        {
                          const gp_Pnt& aP1 = PW.Value(m).Value();
                          const gp_Pnt& aP2 = PW.Value(m + 1).Value();
                          gp_Vec        aVec12(aP1, aP2);
                          if (aVec12.SquareMagnitude() < 1.e-20)
                          {
                            continue;
                          }

                          //
                          gp_Dir aDir12(aVec12);
                          gp_Lin aLin12(aP1, aDir12);
                          aDx = aLin12.Distance(aPx);

                          // modified by NIZNHY-PKV Tue May 10 11:08:07 2011f
                          if (aDx <= 2. * Epsilon)
                          {
                            // if (aDx<=TolTangency) {
                            // modified by NIZNHY-PKV Tue May 10 11:08:13 2011t

                            RejectLine = true;
                            ver--;
                            break;
                          }
                        } // for(m=1; m<iPWNbPoints; ++m){
                      } // if((aD11<=TolTangency && aD22<=TolTangency) ||...
                    } // for( ver = 1 ; (!RejetLigne) && (ver<= NbLigCalculee) ; ver++) {
                    //

                    if (RejectLine)
                    {
                      DublicateOfLinesProcessing(PW, ver, SLin, RejectLine);
                    }

                    if (!RejectLine)
                    {
                      IntSurf_TypeTrans trans1, trans2;
                      double            locu, locv;
                      gp_Vec            norm1, norm2, d1u, d1v;
                      gp_Pnt            ptbid;
                      int               indextg;
                      gp_Vec            tgline(PW.TangentAtLine(indextg));
                      PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
                      Surf1->D1(locu, locv, ptbid, d1u, d1v);
                      norm1 = d1u.Crossed(d1v);
                      PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
                      Surf2->D1(locu, locv, ptbid, d1u, d1v);
                      norm2 = d1u.Crossed(d1v);
                      if (tgline.DotCross(norm2, norm1) >= 0.)
                      {
                        trans1 = IntSurf_Out;
                        trans2 = IntSurf_In;
                      }
                      else
                      {
                        trans1 = IntSurf_In;
                        trans2 = IntSurf_Out;
                      }

                      double                      TolTang = TolTangency;
                      occ::handle<IntPatch_WLine> wline =
                        new IntPatch_WLine(PW.Line(), false, trans1, trans2);
                      wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);
                      wline->EnablePurging(!hasBeenAdded);
                      // the method PutVertexOnLine can reduce the number of points in <wline>
                      IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
                      if (wline->NbPnts() < 2)
                        continue;
                      IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
                      if (wline->NbPnts() < 2)
                        continue;
                      if (wline->NbVertex() == 0)
                      {
                        IntPatch_Point  vtx;
                        IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                        POn2S.Parameters(pu1, pv1, pu2, pv2);
                        vtx.SetValue(Point3dDebut, TolTang, false);
                        vtx.SetParameters(pu1, pv1, pu2, pv2);
                        vtx.SetParameter(1);
                        wline->AddVertex(vtx);

                        POn2S = PW.Line()->Value(wline->NbPnts());
                        POn2S.Parameters(pu1, pv1, pu2, pv2);
                        vtx.SetValue(Point3dFin, TolTang, false);
                        vtx.SetParameters(pu1, pv1, pu2, pv2);
                        vtx.SetParameter(wline->NbPnts());
                        wline->AddVertex(vtx);
                      }

                      lignetrouvee = true;

                      SeveralWlinesProcessing(Surf1,
                                              Surf2,
                                              SLin,
                                              Periods,
                                              trans1,
                                              trans2,
                                              TolTang,
                                              std::max(PW.MaxStep(0), PW.MaxStep(1)),
                                              std::max(PW.MaxStep(2), PW.MaxStep(3)),
                                              wline);

                      AddWLine(SLin, wline, Deflection);
                      empt = false;
                    } // !RejetLigne
                  } // PW points > 2
                } // done is True
              } // dminiPointLigne > SeuildPointLigne
            } // HasStartPoint
          } // if TabPtDep[nbps2] == 0
        } while (
          nbp > 5
          && (NombreDePointsDeDepartDuCheminement < NbDePointsDeDepartDuChmLimit || !lignetrouvee)
          && (NombreDePointsDeDepartDuCheminement - 3 < nbp || !(!lignetrouvee)));
        delete[] TabPtDep;
      } // for( ls ...

      delete[] TabL;

    } // if nbLigSec >= 1
    //
    AdjustOnPeriodic(Surf1, Surf2, SLin);
    //

    //--------------------------------------------------------------------
    //-- Calcul des parametres approches a partir des Zones De Tangence --
    //--------------------------------------------------------------------
    double UminLig1, VminLig1, UmaxLig1, VmaxLig1;
    double UminLig2, VminLig2, UmaxLig2, VmaxLig2;

    UminLig1 = VminLig1 = UminLig2 = VminLig2 = RealLast();
    UmaxLig1 = VmaxLig1 = UmaxLig2 = VmaxLig2 = -UminLig1;

    // NbPointsInTangentZone always == 1 (eap)

    int z;
    for (z = 1; z <= nbTanZon; z++)
    {
      // int NbPointsInTangentZone=Interference.NbPointsInTangentZone(z);
      // for(int pz=1; pz<=NbPointsInTangentZone; pz++) {
      int    pz = 1;
      double _x, _y, _z;
      double U1, U2, V1, V2;
      Interference.GetTangentZonePoint(z, pz, _x, _y, _z, U1, V1, U2, V2);

      if (U1 > UmaxLig1)
        UmaxLig1 = U1;
      if (V1 > VmaxLig1)
        VmaxLig1 = V1;
      if (U2 > UmaxLig2)
        UmaxLig2 = U2;
      if (V2 > VmaxLig2)
        VmaxLig2 = V2;

      if (U1 < UminLig1)
        UminLig1 = U1;
      if (V1 < VminLig1)
        VminLig1 = V1;
      if (U2 < UminLig2)
        UminLig2 = U2;
      if (V2 < VminLig2)
        VminLig2 = V2;
      //}
    }

    for (z = 1; z <= nbTanZon; z++)
    {
      // int NbPointsInTangentZone=Interference.NbPointsInTangentZone(z);
      // for(int pz=1; pz<=NbPointsInTangentZone; pz++) {
      int    pz = 1;
      double _x, _y, _z;
      double U1, U2, V1, V2;
      Interference.GetTangentZonePoint(z, pz, _x, _y, _z, U1, V1, U2, V2);

      StartParams(1) = U1;
      StartParams(2) = V1;
      StartParams(3) = U2;
      StartParams(4) = V2;

      //-----------------------------------------------------------------------
      //-- Calcul du premier point de cheminement a partir du point approche --
      //-----------------------------------------------------------------------
      HasStartPoint = PW.PerformFirstPoint(StartParams, StartPOn2S);
      if (HasStartPoint)
      {
        //-------------------------------------------------
        //-- Un point a ete trouve                       --
        //-- On verifie qu il n appartient pas           --
        //--  a une ligne de cheminement deja calculee.  --
        //-------------------------------------------------
        StartPOn2S.Parameters(pu1, pv1, pu2, pv2);

        NbLigCalculee   = SLin.Length();
        dminiPointLigne = SeuildPointLigne + SeuildPointLigne;

        for (int l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
        {
          const occ::handle<IntPatch_WLine>& testwline =
            *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));

          if (IsPointOnLine(StartPOn2S, testwline, Deflection))
          {
            dminiPointLigne = 0.0;
          }
        }

        //-- Fin d exploration des lignes
        if (dminiPointLigne > SeuildPointLigne)
        {
          //---------------------------------------------------
          //-- Le point de depart du nouveau cheminement     --
          //-- n est present dans aucune ligne deja calculee.--
          //---------------------------------------------------
          PW.Perform(StartParams,
                     UminLig1,
                     VminLig1,
                     UminLig2,
                     VminLig2,
                     UmaxLig1,
                     VmaxLig1,
                     UmaxLig2,
                     VmaxLig2);

          if (PW.IsDone())
          {
            bool hasBeenAdded = false;
            if (PW.NbPoints() > 2)
            {
              const int aMinNbPoints = 40;
              if (PW.NbPoints() < aMinNbPoints)
              {
                hasBeenAdded = PW.SeekAdditionalPoints(Surf1, Surf2, aMinNbPoints);
              }

              //-----------------------------------------------
              //-- Verification a posteriori :
              //-- On teste si le point de depart et de fin de
              //-- la ligne de cheminement est present dans une
              //-- autre ligne .
              //-----------------------------------------------
              RejectLine                      = false;
              Point3dDebut                    = PW.Value(1).Value();
              const IntSurf_PntOn2S& PointFin = PW.Value(PW.NbPoints());
              Point3dFin                      = PointFin.Value();

              for (ver = 1; (!RejectLine) && (ver <= NbLigCalculee); ver++)
              {
                const occ::handle<IntPatch_WLine>& verwline =
                  *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));
                //-- occ::handle<IntPatch_WLine>
                // verwline=occ::down_cast<IntPatch_WLine>(SLin.Value(ver));

                // Check end point if it is on existing line.
                // Start point is checked before.
                if (IsPointOnLine(PointFin, verwline, Deflection))
                {
                  RejectLine = true;
                  break;
                }

                const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
                const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
                if (Point3dDebut.Distance(verPointDebut.Value()) < TolTangency)
                {
                  RejectLine = true;
                }
                else
                {
                  if (Point3dFin.Distance(verPointFin.Value()) < TolTangency)
                  {
                    RejectLine = true;
                  }
                }
              }

              if (!RejectLine)
              {
                IntSurf_TypeTrans trans1, trans2;
                double            locu, locv;
                gp_Vec            norm1, norm2, d1u, d1v;
                gp_Pnt            ptbid;
                int               indextg;
                gp_Vec            tgline(PW.TangentAtLine(indextg));
                PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
                Surf1->D1(locu, locv, ptbid, d1u, d1v);
                norm1 = d1u.Crossed(d1v);
                PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
                Surf2->D1(locu, locv, ptbid, d1u, d1v);
                norm2 = d1u.Crossed(d1v);
                if (tgline.DotCross(norm2, norm1) > 0.)
                {
                  trans1 = IntSurf_Out;
                  trans2 = IntSurf_In;
                }
                else
                {
                  trans1 = IntSurf_In;
                  trans2 = IntSurf_Out;
                }

                double                      TolTang = TolTangency;
                occ::handle<IntPatch_WLine> wline =
                  new IntPatch_WLine(PW.Line(), false, trans1, trans2);
                wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);
                wline->EnablePurging(!hasBeenAdded);
                // the method PutVertexOnLine can reduce the number of points in <wline>
                IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
                if (wline->NbPnts() < 2)
                  continue;
                IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
                if (wline->NbPnts() < 2)
                  continue;

                //---------------
                if (wline->NbVertex() == 0)
                {
                  IntPatch_Point  vtx;
                  IntSurf_PntOn2S POn2S = PW.Line()->Value(1);
                  POn2S.Parameters(pu1, pv1, pu2, pv2);
                  vtx.SetValue(Point3dDebut, TolTang, false);
                  vtx.SetParameters(pu1, pv1, pu2, pv2);
                  vtx.SetParameter(1);
                  wline->AddVertex(vtx);

                  POn2S = PW.Line()->Value(wline->NbPnts());
                  POn2S.Parameters(pu1, pv1, pu2, pv2);
                  vtx.SetValue(Point3dFin, TolTang, false);
                  vtx.SetParameters(pu1, pv1, pu2, pv2);
                  vtx.SetParameter(wline->NbPnts());
                  wline->AddVertex(vtx);
                }

                //---------------
                AddWLine(SLin, wline, Deflection);
                empt = false;
              }
              else
              {
                //-- std::cout<<" ----- REJET DE LIGNE (POINT DE DEPART) ----- "<<std::endl;
              }
              //------------------------------------------------------------
            }
          } //--  le cheminement a reussi (done a True)
        } //--  le point approche ne renvoie pas sur une ligne existante
      } //-- Si HasStartPoint
      //} //-- Boucle Sur les Points de la Tangent Zone
    } //-- Boucle sur Les Tangent Zones

    if (pInterference)
    {
      delete pInterference;
      pInterference = nullptr;
    }

    return;
  } // if((NbU1*NbV1<=Limit && NbV2*NbU2<=Limit)) {

  occ::handle<IntSurf_LineOn2S> LOn2S = new IntSurf_LineOn2S();
  PointDepart(LOn2S, Surf1, NbU1, NbV1, Surf2, NbU2, NbV2);
  empt = true;
  done = true;

  int    NbLigCalculee = 0;
  double U1, U2, V1, V2;
  double pu1, pu2, pv1, pv2;

  NCollection_Array1<double> StartParams(1, 4);
  int                        MaxOscill = NbU1;
  if (MaxOscill < NbU2)
    MaxOscill = NbU2;
  if (MaxOscill < NbV1)
    MaxOscill = NbV1;
  if (MaxOscill < NbV2)
    MaxOscill = NbV2;

  double nIncrement = Increment;
  // if(MaxOscill>10)
  // nIncrement/=0.5*MaxOscill;

  IntWalk_PWalking PW(Surf1, Surf2, TolTangency, Epsilon, Deflection, nIncrement);
  double           SeuildPointLigne = 15.0 * Increment * Increment; //-- 10 est insuffisant
  double           dminiPointLigne;
  bool             HasStartPoint, RejetLigne;
  IntSurf_PntOn2S  StartPOn2S;
  int              ver;
  gp_Pnt           Point3dDebut, Point3dFin;

  //------------------------------------------------------------
  //-- Calcul des parametres approches a partir des Zones De Tangence --
  //--------------------------------------------------------------------
  int nbTanZon = LOn2S->NbPoints();
  for (int z = 1; z <= nbTanZon; z++)
  {
    const IntSurf_PntOn2S& POn2S = LOn2S->Value(z);
    POn2S.Parameters(U1, V1, U2, V2);
    StartParams(1) = U1;
    StartParams(2) = V1;
    StartParams(3) = U2;
    StartParams(4) = V2;

    //-----------------------------------------------------------------------
    //-- Calcul du premier point de cheminement a partir du point approche --
    //-----------------------------------------------------------------------
    HasStartPoint = PW.PerformFirstPoint(StartParams, StartPOn2S);
    if (HasStartPoint)
    {
      //-------------------------------------------------
      //-- Un point a ete trouve                       --
      //-- On verifie qu il n appartient pas           --
      //--  a une ligne de cheminement deja calculee.  --
      //-------------------------------------------------
      StartPOn2S.Parameters(pu1, pv1, pu2, pv2);

      NbLigCalculee   = SLin.Length();
      dminiPointLigne = SeuildPointLigne + SeuildPointLigne;

      for (int l = 1; (l <= NbLigCalculee) && (dminiPointLigne >= SeuildPointLigne); l++)
      {
        const occ::handle<IntPatch_WLine>& testwline =
          *((occ::handle<IntPatch_WLine>*)&SLin.Value(l));

        if (IsPointOnLine(StartPOn2S, testwline, Deflection))
        {
          dminiPointLigne = 0.0;
        }
      }

      //-- Fin d exploration des lignes
      if (dminiPointLigne > SeuildPointLigne)
      {
        //---------------------------------------------------
        //-- Le point de depart du nouveau cheminement     --
        //-- n est present dans aucune ligne deja calculee.--
        //---------------------------------------------------
        PW.Perform(StartParams);
        if (PW.IsDone())
        {
          if (PW.NbPoints() > 2)
          {
            //-----------------------------------------------
            //-- Verification a posteriori :
            //-- On teste si le point de depart et de fin de
            //-- la ligne de cheminement est present dans une
            //-- autre ligne .
            //-----------------------------------------------
            RejetLigne                      = false;
            Point3dDebut                    = PW.Value(1).Value();
            const IntSurf_PntOn2S& PointFin = PW.Value(PW.NbPoints());
            Point3dFin                      = PointFin.Value();

            for (ver = 1; ver <= NbLigCalculee; ver++)
            {
              const occ::handle<IntPatch_WLine>& verwline =
                *((occ::handle<IntPatch_WLine>*)&SLin.Value(ver));
              //-- occ::handle<IntPatch_WLine>
              // verwline=occ::down_cast<IntPatch_WLine>(SLin.Value(ver));

              // Check end point if it is on existing line.
              // Start point is checked before.
              if (IsPointOnLine(PointFin, verwline, Deflection))
              {
                RejetLigne = true;
                break;
              }

              const IntSurf_PntOn2S& verPointDebut = verwline->Point(1);
              const IntSurf_PntOn2S& verPointFin   = verwline->Point(verwline->NbPnts());
              if ((Point3dDebut.Distance(verPointDebut.Value()) < TolTangency)
                  || (Point3dFin.Distance(verPointFin.Value()) < TolTangency))
              {
                RejetLigne = true;
                break;
              }
            }

            if (RejetLigne)
            {
              DublicateOfLinesProcessing(PW, ver, SLin, RejetLigne);
            }

            if (!RejetLigne)
            {
              IntSurf_TypeTrans trans1, trans2;
              double            locu, locv;
              gp_Vec            norm1, norm2, d1u, d1v;
              gp_Pnt            ptbid;
              int               indextg;
              gp_Vec            tgline(PW.TangentAtLine(indextg));
              PW.Line()->Value(indextg).ParametersOnS1(locu, locv);
              Surf1->D1(locu, locv, ptbid, d1u, d1v);
              norm1 = d1u.Crossed(d1v);
              PW.Line()->Value(indextg).ParametersOnS2(locu, locv);
              Surf2->D1(locu, locv, ptbid, d1u, d1v);
              norm2 = d1u.Crossed(d1v);
              if (tgline.DotCross(norm2, norm1) > 0.)
              {
                trans1 = IntSurf_Out;
                trans2 = IntSurf_In;
              }
              else
              {
                trans1 = IntSurf_In;
                trans2 = IntSurf_Out;
              }

              double                      TolTang = TolTangency;
              occ::handle<IntPatch_WLine> wline =
                new IntPatch_WLine(PW.Line(), false, trans1, trans2);
              wline->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLPrmPrm);

              // the method PutVertexOnLine can reduce the number of points in <wline>
              IntPatch_RstInt::PutVertexOnLine(wline, Surf1, D1, Surf2, true, TolTang);
              if (wline->NbPnts() < 2)
                continue;
              IntPatch_RstInt::PutVertexOnLine(wline, Surf2, D2, Surf1, false, TolTang);
              if (wline->NbPnts() < 2)
                continue;

              //---------------
              if (wline->NbVertex() == 0)
              {
                IntPatch_Point         vtx;
                const IntSurf_PntOn2S& POn2Sf = PW.Line()->Value(1);
                POn2Sf.Parameters(pu1, pv1, pu2, pv2);
                vtx.SetValue(Point3dDebut, TolTang, false);
                vtx.SetParameters(pu1, pv1, pu2, pv2);
                vtx.SetParameter(1);
                wline->AddVertex(vtx);

                const IntSurf_PntOn2S& POn2Sl = PW.Line()->Value(wline->NbPnts());
                POn2Sl.Parameters(pu1, pv1, pu2, pv2);
                vtx.SetValue(Point3dFin, TolTang, false);
                vtx.SetParameters(pu1, pv1, pu2, pv2);
                vtx.SetParameter(wline->NbPnts());
                wline->AddVertex(vtx);
              }

              //---------------
              AddWLine(SLin, wline, Deflection);
              empt = false;
            }
            else
            {
              //-- std::cout<<" ----- REJET DE LIGNE (POINT DE DEPART) ----- "<<std::endl;
            }
            //------------------------------------------------------------
          }
        } //--  le cheminement a reussi (done a True)
      } //--  le point approche ne renvoie pas sur une ligne existante
    } //-- Si HasStartPoint
  } //-- Boucle sur Les Tangent Zones
}

// modified by NIZNHY-PKV Wed May 25 09:39:07 2011f
//=================================================================================================

class IntPatch_InfoPD
{
public:
  //----------------------------------------C-tor
  IntPatch_InfoPD(const int aNBI)
  {
    int aNBI2, i, j;
    myNBI = aNBI;
    //
    aNBI2   = aNBI * aNBI;
    myP1DS2 = new char[aNBI2];
    myP2DS1 = new char[aNBI2];
    myIP1   = new int[aNBI2];
    myIP2   = new int[aNBI2];
    myP1    = new gp_Pnt[aNBI2];
    myP2    = new gp_Pnt[aNBI2];
    //
    for (i = 0; i < myNBI; ++i)
    {
      for (j = 0; j < myNBI; ++j)
      {
        xP1DS2(i, j) = 0;
        xP2DS1(i, j) = 0;
        xIP1(i, j)   = 0;
        xIP2(i, j)   = 0;
        xP1(i, j).SetCoord(0., 0., 0.);
        xP2(i, j).SetCoord(0., 0., 0.);
      }
    }
  };

  //---------------------------------------- D-tor
  ~IntPatch_InfoPD()
  {
    delete[] (char*)myP1DS2;
    delete[] (char*)myP2DS1;
    delete[] (int*)myIP1;
    delete[] (int*)myIP2;
    delete[] (gp_Pnt*)myP1;
    delete[] (gp_Pnt*)myP2;
  };

  //---------------------------------------- Index
  int Index(const int i, const int j) const { return i * myNBI + j; };

  //---------------------------------------- NBI
  int NBI() const { return myNBI; };

  //----------------------------------------xP1DS2
  char& xP1DS2(const int i, const int j) { return myP1DS2[Index(i, j)]; };

  //----------------------------------------xP2DS1
  char& xP2DS1(const int i, const int j) { return myP2DS1[Index(i, j)]; };

  //----------------------------------------xIP1
  int& xIP1(const int i, const int j) { return myIP1[Index(i, j)]; };

  //----------------------------------------xIP2
  int& xIP2(const int i, const int j) { return myIP2[Index(i, j)]; };

  //----------------------------------------xP1
  gp_Pnt& xP1(const int i, const int j) { return myP1[Index(i, j)]; };

  //----------------------------------------xP1
  gp_Pnt& xP2(const int i, const int j) { return myP2[Index(i, j)]; };

private:
  IntPatch_InfoPD(const IntPatch_InfoPD&)            = delete;
  IntPatch_InfoPD& operator=(const IntPatch_InfoPD&) = delete;

private:
  int     myNBI;
  char*   myP1DS2;
  char*   myP2DS1;
  int*    myIP1;
  int*    myIP2;
  gp_Pnt* myP1;
  gp_Pnt* myP2;
};

// modified by NIZNHY-PKV Tue May 24 11:38:55 2011t
//=================================================================================================

void IntPatch_PrmPrmIntersection::PointDepart(occ::handle<IntSurf_LineOn2S>&        LineOn2S,
                                              const occ::handle<Adaptor3d_Surface>& S1,
                                              const int                             SU_1,
                                              const int                             SV_1,
                                              const occ::handle<Adaptor3d_Surface>& S2,
                                              const int                             SU_2,
                                              const int                             SV_2) const
{
  int i, j, xNBI;
  // modified by NIZNHY-PKV Tue May 24 11:37:38 2011f
  xNBI = 200;
  IntPatch_InfoPD aIPD(xNBI);
  // modified by NIZNHY-PKV Wed May 25 06:47:12 2011t
  int     iC15, SU1, SV1, SU2, SV2;
  double  U0, U1, V0, V1, U, V;
  double  resu0, resv0;
  double  du1, du2, dv1, dv2, dmaxOn1, dmaxOn2;
  double  x0, y0, z0, x1, y1, z1, d;
  Bnd_Box Box1, Box2;
  //
  iC15 = 15;
  SU1  = iC15 * SU_1;
  SV1  = iC15 * SV_1;
  SU2  = iC15 * SU_2;
  SV2  = iC15 * SV_2;
  //
  if (xNBI < SU1)
  {
    SU1 = xNBI;
  }
  if (xNBI < SV1)
  {
    SV1 = xNBI;
  }
  if (xNBI < SU2)
  {
    SU2 = xNBI;
  }
  if (xNBI < SV2)
  {
    SV2 = xNBI;
  }
  //
  U0 = S1->FirstUParameter();
  U1 = S1->LastUParameter();
  V0 = S1->FirstVParameter();
  V1 = S1->LastVParameter();
  //
  resu0 = U0;
  resv0 = V0;
  //
  dmaxOn1 = 0.0;
  dmaxOn2 = 0.0;
  //-----
  du1 = (U1 - U0) / (SU1 - 1);
  dv1 = (V1 - V0) / (SV1 - 1);
  for (U = U0, i = 0; i < SU1; i++, U += du1)
  {
    for (V = V0, j = 0; j < SV1; V += dv1, j++)
    {
      aIPD.xP1(i, j) = S1->Value(U, V);
      Box1.Add(aIPD.xP1(i, j));
      if (i > 0 && j > 0)
      {
        aIPD.xP1(i, j).Coord(x0, y0, z0);
        aIPD.xP1(i - 1, j - 1).Coord(x1, y1, z1);
        //
        d = std::abs(x1 - x0) + std::abs(y1 - y0) + std::abs(z1 - z0);
        if (d > dmaxOn1)
        {
          dmaxOn1 = d;
        }
      }
    }
  }
  Box1.Enlarge(1.e-8);
  //
  U0 = S2->FirstUParameter();
  U1 = S2->LastUParameter();
  V0 = S2->FirstVParameter();
  V1 = S2->LastVParameter();
  //
  du2 = (U1 - U0) / (SU2 - 1);
  dv2 = (V1 - V0) / (SV2 - 1);
  for (U = U0, i = 0; i < SU2; i++, U += du2)
  {
    for (V = V0, j = 0; j < SV2; V += dv2, j++)
    {
      aIPD.xP2(i, j) = S2->Value(U, V);
      Box2.Add(aIPD.xP2(i, j));
      if (i > 0 && j > 0)
      {
        aIPD.xP2(i, j).Coord(x0, y0, z0);
        aIPD.xP2(i - 1, j - 1).Coord(x1, y1, z1);
        d = std::abs(x1 - x0) + std::abs(y1 - y0) + std::abs(z1 - z0);
        if (d > dmaxOn2)
        {
          dmaxOn2 = d;
        }
      }
    }
  }
  Box2.Enlarge(1.e-8);
  //--------
  //
  if (Box1.IsOut(Box2))
  {

    return;
  }
  //
  int    aNbPG;
  double x10, y10, z10, x11, y11, z11;
  double x20, y20, z20, x21, y21, z21;
  double dx, dy, dz, dmax;
  double dx2, dy2, dz2;
  //
  Box1.Get(x10, y10, z10, x11, y11, z11);
  Box2.Get(x20, y20, z20, x21, y21, z21);
  //
  x0 = (x10 > x20) ? x10 : x20;
  y0 = (y10 > y20) ? y10 : y20;
  z0 = (z10 > z20) ? z10 : z20;
  //
  x1 = (x11 < x21) ? x11 : x21;
  y1 = (y11 < y21) ? y11 : y21;
  z1 = (z11 < z21) ? z11 : z21;
  //
  if (dmaxOn2 > dmaxOn1)
  {
    dmaxOn1 = dmaxOn2;
  }
  //
  dmaxOn1 += dmaxOn1;
  x0 -= dmaxOn1;
  y0 -= dmaxOn1;
  z0 -= dmaxOn1;
  x1 += dmaxOn1;
  y1 += dmaxOn1;
  z1 += dmaxOn1;
  //
  x10 -= dmaxOn1;
  y10 -= dmaxOn1;
  z10 -= dmaxOn1;
  x11 += dmaxOn1;
  y11 += dmaxOn1;
  z11 += dmaxOn1;

  x20 -= dmaxOn1;
  y20 -= dmaxOn1;
  z20 -= dmaxOn1;
  x21 += dmaxOn1;
  y21 += dmaxOn1;
  z21 += dmaxOn1;

  aNbPG = NbPointsGrille();
  dx    = (x1 - x0) / aNbPG;
  dy    = (y1 - y0) / aNbPG;
  dz    = (z1 - z0) / aNbPG;
  //
  dmax = dx;
  if (dy > dmax)
  {
    dmax = dy;
  }
  if (dz > dmax)
  {
    dmax = dz;
  }
  //
  if (dx < dmax * 0.01)
  {
    dx = dmax * 0.01;
  }
  if (dy < dmax * 0.01)
  {
    dy = dmax * 0.01;
  }
  if (dz < dmax * 0.01)
  {
    dz = dmax * 0.01;
  }
  //
  dx2 = dx * 0.5;
  dy2 = dy * 0.5;
  dz2 = dz * 0.5;
  //
  IntPatch_PrmPrmIntersection_T3Bits M1(_BASE);
  IntPatch_PrmPrmIntersection_T3Bits M2(_BASE);
  //
  for (i = 0; i < SU1; i++)
  {
    for (j = 0; j < SV1; j++)
    {
      aIPD.xIP1(i, j)   = -1;
      const gp_Pnt& P   = aIPD.xP1(i, j);
      aIPD.xP1DS2(i, j) = (char)CodeReject(x20, y20, z20, x21, y21, z21, P.X(), P.Y(), P.Z());
      int ix            = (int)((P.X() - x0 + dx2) / dx);
      if (DansGrille(ix))
      {
        int iy = (int)((P.Y() - y0 + dy2) / dy);
        if (DansGrille(iy))
        {
          int iz = (int)((P.Z() - z0 + dz2) / dz);
          if (DansGrille(iz))
          {
            aIPD.xIP1(i, j) = GrilleInteger(ix, iy, iz);
          }
        }
      }
    }
  }
  //-- std::cout<<" Grille  du 1 fini "<<std::endl;
  for (i = 0; i < SU2; i++)
  {
    for (j = 0; j < SV2; j++)
    {
      aIPD.xIP2(i, j)   = -1;
      const gp_Pnt& P   = aIPD.xP2(i, j);
      aIPD.xP2DS1(i, j) = (char)CodeReject(x10, y10, z10, x11, y11, z11, P.X(), P.Y(), P.Z());
      int ix            = (int)((P.X() - x0 + dx2) / dx);
      if (DansGrille(ix))
      {
        int iy = (int)((P.Y() - y0 + dy2) / dy);
        if (DansGrille(iy))
        {
          int iz = (int)((P.Z() - z0 + dz2) / dz);
          if (DansGrille(iz))
          {
            aIPD.xIP2(i, j) = GrilleInteger(ix, iy, iz);
          }
        }
      }
    }
  }
  //
  for (i = 0; i < SU1 - 1; i += 1)
  {
    for (j = 0; j < SV1 - 1; j += 1)
    {
      if (!((aIPD.xP1DS2(i, j) & aIPD.xP1DS2(i + 1, j))
            || (aIPD.xP1DS2(i, j) & aIPD.xP1DS2(i + 1, j + 1))))
      {
        Remplit(aIPD.xIP1(i, j), aIPD.xIP1(i + 1, j), aIPD.xIP1(i + 1, j + 1), M1);
      }
      if (!((aIPD.xP1DS2(i, j) & aIPD.xP1DS2(i, j + 1))
            || (aIPD.xP1DS2(i, j) & aIPD.xP1DS2(i + 1, j + 1))))
      {
        Remplit(aIPD.xIP1(i, j), aIPD.xIP1(i, j + 1), aIPD.xIP1(i + 1, j + 1), M1);
      }
    }
  }
  //
  for (i = 0; i < SU2 - 1; i += 1)
  {
    for (j = 0; j < SV2 - 1; j += 1)
    {
      if (!((aIPD.xP2DS1(i, j) & aIPD.xP2DS1(i + 1, j))
            || (aIPD.xP2DS1(i, j) & aIPD.xP2DS1(i + 1, j + 1))))
      {
        Remplit(aIPD.xIP2(i, j), aIPD.xIP2(i + 1, j), aIPD.xIP2(i + 1, j + 1), M2);
      }
      if (!((aIPD.xP2DS1(i, j) & aIPD.xP2DS1(i, j + 1))
            || (aIPD.xP2DS1(i, j) & aIPD.xP2DS1(i + 1, j + 1))))
      {
        Remplit(aIPD.xIP2(i, j), aIPD.xIP2(i, j + 1), aIPD.xIP2(i + 1, j + 1), M2);
      }
    }
  }
  //
  M1.ResetAnd();
  M2.ResetAnd();
  //
  int newind            = 0;
  int ok                = 0;
  int indicepointtraite = 0;
  int k, nu, nv;
  //
  do
  {
    indicepointtraite--;
    ok = M1.And(M2, newind);
    if (ok)
    {
      IntegerGrille(newind, i, j, k);
      int nb  = 0;
      int LIM = 3;
      if (DansGrille(i - 1) && DansGrille(j - 1) && DansGrille(k - 1) && DansGrille(i + 1)
          && DansGrille(j + 1) && DansGrille(k + 1))
      {
        int si, sj, sk;
        for (si = -1; si <= 1 && nb < LIM; si++)
        {
          for (sj = -1; sj <= 1 && nb < LIM; sj++)
          {
            for (sk = -1; sk <= 1 && nb < LIM; sk++)
            {
              int lu = GrilleInteger(i + si, j + sj, k + sk);
              if (M1.Val(lu) && M2.Val(lu))
              {
                nb++;
              }
            }
          }
        }
        if (nb >= LIM)
        {
          for (si = -1; si <= 1; si++)
          {
            for (sj = -1; sj <= 1; sj++)
            {
              for (sk = -1; sk <= 1; sk++)
              {
                if (si || sj || sk)
                {
                  int lu = GrilleInteger(i + si, j + sj, k + sk);
                  M1.Raz(lu);
                }
              }
            }
          }
        }
      }
      //
      gp_Pnt P(dx * i + x0, dy * j + y0, dz * k + z0);
      //
      int nu1 = -1, nu2 = -1;
      int nv1 = 0, nv2 = 0;
      for (nu = 0; nu1 < 0 && nu < SU1; nu++)
      {
        for (nv = 0; nu1 < 0 && nv < SV1; nv++)
        {
          if (aIPD.xIP1(nu, nv) == (int)newind)
          {
            aIPD.xIP1(nu, nv) = indicepointtraite;
            nu1               = nu;
            nv1               = nv;
          }
        }
      }
      if (nu1 >= 0)
      {
        for (nu = 0; nu2 < 0 && nu < SU2; nu++)
        {
          for (nv = 0; nu2 < 0 && nv < SV2; nv++)
          {
            if (aIPD.xIP2(nu, nv) == (int)newind)
            {
              aIPD.xIP2(nu, nv) = indicepointtraite;
              nu2               = nu;
              nv2               = nv;
            }
          }
        }
      }
      if (nu1 >= 0 && nu2 >= 0)
      {
        IntSurf_PntOn2S POn2S;
        POn2S.SetValue(P,
                       S1->FirstUParameter() + nu1 * du1,
                       S1->FirstVParameter() + nv1 * dv1,
                       S2->FirstUParameter() + nu2 * du2,
                       S2->FirstVParameter() + nv2 * dv2);
        LineOn2S->Add(POn2S);
      }
      else
      {
        //-- aucun point du triangle n a ete trouve assez proche
        //-- on recherche les 3 points les plus proches de P
        //-- dans chacun des tableaux
        double Dist3[3], u3[3] = {0.0, 0.0, 0.0}, v3[3] = {0.0, 0.0, 0.0};
        Dist3[0] = Dist3[1] = Dist3[2] = RealLast();
        for (U = resu0, i = 0; i < SU1; i++, U += du1)
        {
          for (V = resv0, j = 0; j < SV1; V += dv1, j++)
          {
            //-- On place les 3 meilleures valeurs dans Dist1,Dist2,Dist3
            double t = aIPD.xP1(i, j).SquareDistance(P);
            //-- On remplace la plus grande valeur ds Dist[.] par la val courante
            if (Dist3[0] < Dist3[1])
            {
              double z;
              z        = Dist3[0];
              Dist3[0] = Dist3[1];
              Dist3[1] = z;
              z        = u3[0];
              u3[0]    = u3[1];
              u3[1]    = z;
              z        = v3[0];
              v3[0]    = v3[1];
              v3[1]    = z;
            }
            if (Dist3[1] < Dist3[2])
            {
              double z;
              z        = Dist3[1];
              Dist3[1] = Dist3[2];
              Dist3[2] = z;
              z        = u3[1];
              u3[1]    = u3[2];
              u3[2]    = z;
              z        = v3[1];
              v3[1]    = v3[2];
              v3[2]    = z;
            }
            if (Dist3[0] < Dist3[1])
            {
              double z;
              z        = Dist3[0];
              Dist3[0] = Dist3[1];
              Dist3[1] = z;
              z        = u3[0];
              u3[0]    = u3[1];
              u3[1]    = z;
              z        = v3[0];
              v3[0]    = v3[1];
              v3[1]    = z;
            }
            //-- la plus grande valeur est dans Dist3[0]
            if (t < Dist3[0])
            {
              Dist3[0] = t;
              u3[0]    = U;
              v3[0]    = V;
            }
          }
        }
        //
        double U1_3 = (u3[0] + u3[1] + u3[2]) / 3.0;
        double V1_3 = (v3[0] + v3[1] + v3[2]) / 3.0;

        Dist3[0] = Dist3[1] = Dist3[2] = RealLast();
        for (U = U0, i = 0; i < SU2; i++, U += du2)
        {
          for (V = V0, j = 0; j < SV2; V += dv2, j++)
          {
            //-- On place les 3 meilleures valeurs dans Dist1,Dist2,Dist3
            double t = aIPD.xP2(i, j).SquareDistance(P);
            //-- On remplace la plus grande valeur ds Dist3[.] par la val courante
            if (Dist3[0] < Dist3[1])
            {
              double z;
              z        = Dist3[0];
              Dist3[0] = Dist3[1];
              Dist3[1] = z;
              z        = u3[0];
              u3[0]    = u3[1];
              u3[1]    = z;
              z        = v3[0];
              v3[0]    = v3[1];
              v3[1]    = z;
            }
            if (Dist3[1] < Dist3[2])
            {
              double z;
              z        = Dist3[1];
              Dist3[1] = Dist3[2];
              Dist3[2] = z;
              z        = u3[1];
              u3[1]    = u3[2];
              u3[2]    = z;
              z        = v3[1];
              v3[1]    = v3[2];
              v3[2]    = z;
            }
            if (Dist3[0] < Dist3[1])
            {
              double z;
              z        = Dist3[0];
              Dist3[0] = Dist3[1];
              Dist3[1] = z;
              z        = u3[0];
              u3[0]    = u3[1];
              u3[1]    = z;
              z        = v3[0];
              v3[0]    = v3[1];
              v3[1]    = z;
            }
            //-- la plus grande valeur est dans Dist3[0]
            if (t < Dist3[0])
            {
              Dist3[0] = t;
              u3[0]    = U;
              v3[0]    = V;
            }
          }
        }
        //
        double U2_3 = (u3[0] + u3[1] + u3[2]) / 3.0;
        double V2_3 = (v3[0] + v3[1] + v3[2]) / 3.0;
        //
        IntSurf_PntOn2S POn2S;
        POn2S.SetValue(P, U1_3, V1_3, U2_3, V2_3);
        LineOn2S->Add(POn2S);
      }
    }
  } while (ok);
}

//=================================================================================================

bool IsPointOnLine(const IntSurf_PntOn2S&             thePOn2S,
                   const occ::handle<IntPatch_WLine>& theWLine,
                   const double                       Deflection)
{
  bool   isOnLine    = false;
  double Deflection2 = Deflection * Deflection;
  double pu1, pu2, pv1, pv2;

  thePOn2S.Parameters(pu1, pv1, pu2, pv2);

  if ((!theWLine->IsOutSurf1Box(gp_Pnt2d(pu1, pv1)))
      && (!theWLine->IsOutSurf2Box(gp_Pnt2d(pu2, pv2))) && (!theWLine->IsOutBox(thePOn2S.Value())))
  {
    const int NbPntOn2SOnLine = theWLine->NbPnts();
    int       ll;

    for (ll = 1; ll < NbPntOn2SOnLine && !isOnLine; ll++)
    {
      const gp_Pnt& Pa     = theWLine->Point(ll).Value();
      const gp_Pnt& Pb     = theWLine->Point(ll + 1).Value();
      const gp_Pnt& PStart = thePOn2S.Value();
      const gp_Vec  AM(Pa, PStart);
      const gp_Vec  MB(PStart, Pb);
      const double  AMMB = AM.Dot(MB);

      if (AMMB > 0.0)
      {
        gp_Dir ABN(Pb.X() - Pa.X(), Pb.Y() - Pa.Y(), Pb.Z() - Pa.Z());
        double lan = ABN.X() * AM.X() + ABN.Y() * AM.Y() + ABN.Z() * AM.Z();
        gp_Vec AH(lan * ABN.X(), lan * ABN.Y(), lan * ABN.Z());
        gp_Vec HM(AM.X() - AH.X(), AM.Y() - AH.Y(), AM.Z() - AH.Z());
        double d = 0.0;

        if (HM.X() < Deflection)
        {
          d += HM.X() * HM.X();

          if (HM.Y() < Deflection)
          {
            d += HM.Y() * HM.Y();

            if (HM.Z() < Deflection)
            {
              d += HM.Z() * HM.Z();
            }
            else
            {
              d = Deflection2;
            }
          }
          else
          {
            d = Deflection2;
          }
        }
        else
        {
          d = Deflection2;
        }

        if (d < Deflection2)
        {
          isOnLine = true;
        }
      }
      else
      {
        double dab = Pa.SquareDistance(Pb);
        double dap = Pa.SquareDistance(PStart);

        if (dap < dab)
        {
          isOnLine = true;
        }
        else
        {
          double dbp = Pb.SquareDistance(PStart);

          if (dbp < dab)
          {
            isOnLine = true;
          }
        }
      }
    }
  }

  return isOnLine;
}

//=================================================================================================

void AddWLine(NCollection_Sequence<occ::handle<IntPatch_Line>>& theLines,
              const occ::handle<IntPatch_WLine>&                theWLine,
              const double                                      Deflection)
{
  int  i        = 1;
  int  aNbLines = theLines.Length();
  bool isToRemove;

  // Check each line of theLines if it is on theWLine.
  while (i <= aNbLines)
  {
    occ::handle<IntPatch_WLine> aWLine = occ::down_cast<IntPatch_WLine>(theLines.Value(i));

    isToRemove = false;

    if (!aWLine.IsNull())
    {
      // Check the middle point.
      int aMidIndex = (aWLine->NbPnts() + 1) / 2;

      if (aMidIndex > 0)
      {
        const IntSurf_PntOn2S& aPnt = aWLine->Point(aMidIndex);

        if (IsPointOnLine(aPnt, theWLine, Deflection))
        {
          // Middle point is on theWLine. Check vertices.
          isToRemove = true;

          int       j;
          const int aNbVtx = aWLine->NbVertex();

          for (j = 1; j <= aNbVtx; j++)
          {
            const IntPatch_Point& aPoint = aWLine->Vertex(j);

            if (!IsPointOnLine(aPoint.PntOn2S(), theWLine, Deflection))
            {
              isToRemove = false;
              break;
            }
          }
        }
      }
    }

    if (isToRemove)
    {
      theLines.Remove(i);
      aNbLines--;
    }
    else
    {
      i++;
    }
  }

  // Add theWLine to the sequence of lines.
  theLines.Append(theWLine);
}
