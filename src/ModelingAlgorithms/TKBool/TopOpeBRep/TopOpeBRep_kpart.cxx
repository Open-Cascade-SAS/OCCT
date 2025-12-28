// Created on: 1994-02-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRep_GetcontextNEWKP();
#endif

// VP<STATIC_lastVPind> is the vp on which was computed the last CPI.
// if no CPI is computed yet, <STATIC_lastVPind> = 0.
static int STATIC_lastVPind;

#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

//-----------------------------------------------------------------------
static void FUNBREP_Periodize(const TopOpeBRep_LineInter&                   L,
                              const occ::handle<TopOpeBRepDS_Interference>& Ifound,
                              double&                                       PIfound,
                              double&                                       parline)
//-----------------------------------------------------------------------
{
  const TopOpeBRepDS_Transition& TIfound  = Ifound->Transition();
  TopAbs_Orientation             OTIfound = TIfound.Orientation(TopAbs_IN);

  if (L.IsPeriodic())
  {
    double f, l;
    L.Bounds(f, l);
    bool onf  = std::abs(parline - f) < Precision::PConfusion();
    bool onl  = std::abs(parline - l) < Precision::PConfusion();
    bool onfl = onf || onl;
    if (onfl)
    {
      if (OTIfound == TopAbs_FORWARD)
      {
        PIfound = f;
        parline = l;
      }
      else if (OTIfound == TopAbs_REVERSED)
      {
        PIfound = l;
        parline = f;
      }
      else
      {
        throw Standard_ProgramError("FUNBREP_Periodize");
      }
    } // onfl
    else
    {
      parline = PIfound = std::min(parline, PIfound);
    }
  } // IsPeriodic
  else
  {
    parline = PIfound = std::min(parline, PIfound);
  }
}

//-----------------------------------------------------------------------
static bool FUNBREP_HasSameGPoint(const TopOpeBRepDS_Point&                     DSP,
                                  const occ::handle<TopOpeBRepDS_Interference>& I,
                                  const TopOpeBRepDS_DataStructure&             BDS)
//-----------------------------------------------------------------------
{
  // returns <true> if <DSP> shares same geometric point with
  // <I> geometry.
  int  G      = I->Geometry();
  bool samegp = false;
  if (I->GeometryType() == TopOpeBRepDS_POINT)
  {
    const TopOpeBRepDS_Point& P = BDS.Point(G);
    samegp                      = DSP.IsEqual(P);
  }
  else if (I->GeometryType() == TopOpeBRepDS_VERTEX)
  {
    TopOpeBRepDS_Point P(BDS.Shape(G));
    samegp = DSP.IsEqual(P);
  }
  return samegp;
}

//-----------------------------------------------------------------------
static bool FUNBREP_SameUV(const TopOpeBRep_VPointInter& VP1,
                           const TopOpeBRep_VPointInter& VP2,
                           const int                     sind,
                           const double                  toluv)
//-----------------------------------------------------------------------
{
  double u1 = 0., v1 = 0., u2 = 0., v2 = 0.;
  if (sind == 1)
  {
    VP1.ParametersOnS1(u1, v1);
    VP2.ParametersOnS1(u2, v2);
  }
  if (sind == 2)
  {
    VP1.ParametersOnS2(u1, v1);
    VP2.ParametersOnS2(u2, v2);
  }
  gp_Pnt2d p1(u1, v1), p2(u2, v2);
  double   dist   = p1.Distance(p2);
  bool     sameuv = (dist < toluv);
  return sameuv;
}

//----------------------------------------------------------------------
bool FUNBREP_topowalki_new(const occ::handle<TopOpeBRepDS_Interference>&                   Ifound,
                           const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
                           const TopOpeBRep_LineInter&                                     L,
                           const TopOpeBRep_VPointInter&                                   VP,
                           const TopoDS_Shape&                                             E,
                           const bool                                                      samepar,
                           const bool                                                      couture,
                           TopOpeBRepDS_Transition& transLine)
//----------------------------------------------------------------------
{
  if (L.TypeLineCurve() != TopOpeBRep_WALKING)
  {
    throw Standard_ProgramError("FUNBREP_topowalki_new : line is not a walking");
  }

  bool CPIfound = !Ifound.IsNull();
  int  iVP      = VP.Index();
  int  iINON1, iINONn, nINON;
  L.VPBounds(iINON1, iINONn, nINON);
  bool onsort = (transLine.Orientation(TopAbs_OUT) == TopAbs_FORWARD);

  bool LITdefinie = DSCIL.IsEmpty() ? false : !DSCIL.Last()->Transition().IsUnknown();

  // dealing with INTERNAL/EXTERNAL <E> :
  // ------------------------------------
  TopAbs_Orientation Eori = E.Orientation();
  bool               EFR  = M_FORWARD(Eori) || M_REVERSED(Eori);
  if (!EFR)
  { // E INTERNAL ou EXTERNAL
    // we assume transition for first vp on non oriented edge to be FORWARD
    //                          last vp on non oriented edge to be REVERSED
    if (iVP == iINON1)
      transLine = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
    if (iVP == iINONn)
      transLine = TopOpeBRepDS_Transition(TopAbs_IN, TopAbs_OUT);
  }

  // no last defined transition :
  // ----------------------------
  if (!LITdefinie)
  {
    if (onsort)
      return false;
    if (iVP == iINONn)
      return false;
  }

  // has last defined transition :
  // -----------------------------
  bool keep = true;
  if (LITdefinie)
  {

    const occ::handle<TopOpeBRepDS_Interference>& I   = DSCIL.Last();
    TopOpeBRepDS_Transition                       LIT = I->Transition();
    bool LITonsort                                    = M_FORWARD(LIT.Orientation(TopAbs_OUT));
    bool LprecIsEntrant                               = !LITonsort;
    bool entrantsortant                               = LprecIsEntrant && onsort;

    if (EFR && !entrantsortant)
      keep = false;

    if (couture)
    {
      // vp describing the same geometric point on closing edge :
      // it is the same vp described on the FORWARD and REVERSED closing edge -> we keep it only
      // once INFO : on walking, vp line parameter is the vp index on the walking samepar =>
      // describes same CPI
      bool samevponcouture = samepar && keep;
      if (samevponcouture)
      {
        keep = false;
      }
    }

    { // kpart JYL 971204 for closing walking, first and current VP same parameter,
      //                  lastonentre && onsort,
      //                  first VP is first walking point (PIfound == 1.0),
      //                  current VP is the second one on walking and not the last.
      const occ::handle<TopOpeBRepDS_Interference>& anI           = DSCIL.First();
      const TopOpeBRepDS_Transition&                lasttransLine = anI->Transition();
      // xpu12-12-97 : line is built on only 2 vp,
      //  DSCIL->First() == DSCIL->Last()
      //  lasttransLine = DSCIL's first transition
      double PIfound    = TopOpeBRepDS_InterferenceTool::Parameter(Ifound);
      bool   fermee     = L.IsVClosed();
      bool   lastonsort = LITdefinie && M_FORWARD(lasttransLine.Orientation(TopAbs_OUT));

      bool kpartclosingwalki = (LITdefinie && !lastonsort);
      kpartclosingwalki      = kpartclosingwalki && CPIfound && samepar;
      kpartclosingwalki      = kpartclosingwalki && onsort;
      kpartclosingwalki      = kpartclosingwalki && fermee;
      kpartclosingwalki      = kpartclosingwalki && (PIfound == 1.0);
      kpartclosingwalki      = kpartclosingwalki && (iVP == iINON1 + 1) && (iVP != iINONn);
      if (kpartclosingwalki)
      {
        keep = false;
      }
    }
  } // LITdefinie

  return keep;
} // FUNBREP_topowalki_new

#ifdef OCCT_DEBUG
extern bool GLOBAL_bvpr;

Standard_EXPORT void debvpr2(void) {}
#endif

//----------------------------------------------------------------------
bool FUNBREP_topowalki(const occ::handle<TopOpeBRepDS_Interference>&                   Ifound,
                       const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
                       const TopOpeBRep_LineInter&                                     L,
                       const TopOpeBRep_VPointInter&                                   VP,
                       const TopOpeBRepDS_Transition& lasttransLine,
                       // const TopOpeBRepDS_DataStructure& BDS,
                       const TopOpeBRepDS_DataStructure&,
                       const TopoDS_Shape& E,
                       // const TopoDS_Shape& F,
                       const TopoDS_Shape&,
                       // const double toluv,
                       const double,
                       const bool CPIfound,
                       const bool samepar,
                       const bool couture,
                       // double& parline,
                       double&,
                       TopOpeBRepDS_Transition& transLine)
//----------------------------------------------------------------------
{
  if (L.TypeLineCurve() != TopOpeBRep_WALKING)
  {
    throw Standard_ProgramError("FUNBREP_topowalki : line is not a walking");
  }

#ifdef OCCT_DEBUG
  bool newkp = TopOpeBRep_GetcontextNEWKP();
  if (newkp)
  {
    bool keep = FUNBREP_topowalki_new(Ifound, DSCIL, L, VP, E, samepar, couture, transLine);
    return keep;
  }
#endif

  TopAbs_Orientation Eori = E.Orientation();
  int                iVP  = VP.Index();
  int                iINON1, iINONn, nINON;
  L.VPBounds(iINON1, iINONn, nINON);
  bool fermee      = L.IsVClosed();
  bool onsort      = (transLine.Orientation(TopAbs_OUT) == TopAbs_FORWARD);
  bool lastdefinie = !lasttransLine.IsUnknown();
  bool lastonsort  = false;
  bool lastinin    = false;

  if (lastdefinie)
  {
    lastonsort = (lasttransLine.Orientation(TopAbs_OUT) == TopAbs_FORWARD);
    lastinin   = (lasttransLine.Before() == TopAbs_IN);
    lastinin   = lastinin && (lasttransLine.After() == TopAbs_IN);
  }

  bool                                   LITdefinie = false;
  bool                                   LITonsort  = false;
  TopOpeBRepDS_Transition                LIT;
  occ::handle<TopOpeBRepDS_Interference> I;
  bool                                   nointerf = DSCIL.IsEmpty();
  if (!nointerf)
  {
    I          = DSCIL.Last();
    LIT        = I->Transition();
    LITdefinie = !LIT.IsUnknown();
    if (LITdefinie)
      LITonsort = (LIT.Orientation(TopAbs_OUT) == TopAbs_FORWARD);
  }

  bool keep = true;
  bool EFR  = (Eori == TopAbs_FORWARD || Eori == TopAbs_REVERSED);

  if (!LITdefinie)
  {
    if (onsort)
    {
      bool condCTS19305 = false;
      // start debug 971216 : CTS19305
      condCTS19305 = (!CPIfound);
      condCTS19305 = condCTS19305 && (!fermee);
      condCTS19305 = condCTS19305 && (!lastdefinie);
      condCTS19305 = condCTS19305 && (iVP == 1) && (iVP == iINON1);
      if (condCTS19305)
      {
        keep      = true;
        transLine = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
      }
      // end debug 971216 :
      else
        keep = false;
    }
    if (iVP == iINONn)
      keep = false;
    if (!EFR)
    { // E INTERNAL ou EXTERNAL
      if (iVP == iINON1)
      {
        transLine = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
      }
    }
  }

  if (keep && LITdefinie)
  {
    bool LprecIsEntrant = (LITdefinie && !LITonsort);
    bool entrantsortant = LprecIsEntrant && onsort;
    if (!EFR)
    {
      if (iVP == iINONn)
      {
        transLine = TopOpeBRepDS_Transition(TopAbs_IN, TopAbs_OUT);
      }
    }
    else
    {
      bool condCTS19305 = false;
      condCTS19305      = (!CPIfound);
      condCTS19305      = condCTS19305 && (!fermee);
      condCTS19305      = condCTS19305 && (lastdefinie && !lastonsort);
      condCTS19305      = condCTS19305 && (LITdefinie && !LITonsort);
      condCTS19305      = condCTS19305 && (iVP == iINONn);

      if (condCTS19305)
      {
        keep      = true;
        transLine = TopOpeBRepDS_Transition(TopAbs_IN, TopAbs_OUT);
      }
      else
      {
        if (!entrantsortant)
        {
          keep = false;
        }
      }
    }

    // vp describing the same geometric point on closing edge :
    // it is the same vp described on the FORWARD and REVERSED closing edge -> we keep it only once
    // INFO : on walking, vp line parameter is the vp index on the walking
    // samepar => describes same CPI

    bool samevponcouture = samepar && couture;
    if (keep && samevponcouture)
    {
      keep = false;
    }

    if (keep)
    {
#ifdef OCCT_DEBUG
      if (GLOBAL_bvpr)
        debvpr2();
#endif

      if (CPIfound && samepar)
      {
        double PIfound = TopOpeBRepDS_InterferenceTool::Parameter(Ifound);

        // 971204
        bool cond1 = true;
        cond1      = cond1 && (lastdefinie && !lastonsort);
        cond1      = cond1 && onsort;
        cond1      = cond1 && fermee;
        cond1      = cond1 && (CPIfound && samepar);
        cond1      = cond1 && (PIfound == 1.0);
        cond1      = cond1 && (iVP == iINON1 + 1) && (iVP != iINONn);
        if (cond1)
        {
          keep = false;
          return keep;
        }

        // PRO12107
        bool cond2 = true;
        cond2      = cond2 && (lastdefinie && !lastonsort);
        cond1      = cond1 && onsort;
        cond2      = cond2 && (!fermee);
        cond2      = cond2 && (CPIfound && samepar);
        cond2      = cond2 && (PIfound == 1.0);
        cond2      = cond2 && (iVP == iINON1 + 1) && (iVP != iINONn);
        if (cond2)
        {
          keep = false;
          return keep;
        }
      }
    } // keep

  } // keep && LITdefinie

  if (keep)
    STATIC_lastVPind = iVP;
  return keep;
} // FUNBREP_topowalki

//----------------------------------------------------------------------
bool FUNBREP_topogline_new(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
                           const TopOpeBRep_LineInter&                                     L,
                           const TopOpeBRep_VPointInter&                                   VP,
                           const TopOpeBRepDS_DataStructure&                               BDS,
                           const double                                                    toluv,
                           const bool                                                      samepar,
                           const bool                                                      couture,
                           double&                                                         parline,
                           TopOpeBRepDS_Transition& transLine)
//----------------------------------------------------------------------
{
  if (L.TypeLineCurve() == TopOpeBRep_WALKING)
  {
    throw Standard_ProgramError("FUNBREP_topogline_new : line is not a GLine");
  }

  int iVP = VP.Index();
  int iINON1, iINONn, nINON;
  L.VPBounds(iINON1, iINONn, nINON);

  // if VP<iVP> is on 3
  //            and VP on 1 has transition OUT/IN,
  //                VP on 2 has transition IN/OUT,
  // same VP will be kept.

  // the line is described with (VPmin,VPmax) boundaries.
  // if VP == VPmax, as we are only regarding IN/OUT transitions->ok
  // if VP == VPmin :
  bool dscilempty = DSCIL.IsEmpty();
  bool notkept    = !dscilempty && (iVP == 1);
  if (notkept)
    return false;

  // <transLine> : for unknown current transition
  // ------------
  // (see TopOpeBRep_FacesFiller::ProcessVPonR)
  // vpmin with transition UNKNOWN               => transLine-> OUT/IN
  // vpmin OUT/IN, vpmax with transition UNKNOWN => transLine-> IN/OUT
  TopOpeBRepDS_Transition                LIT;
  occ::handle<TopOpeBRepDS_Interference> I;
  bool                                   LITdefinie, LITonsort;
  LITdefinie = LITonsort = false;
  if (!dscilempty)
  {
    I          = DSCIL.Last();
    LIT        = I->Transition();
    LITdefinie = !LIT.IsUnknown();
    if (LITdefinie)
      LITonsort = M_FORWARD(LIT.Orientation(TopAbs_OUT));
  }
  bool trliunk   = transLine.IsUnknown();
  bool isfirstvp = (iVP == iINON1);
  bool islastvp  = (iVP == iINONn);
  if (trliunk)
  {
    if (isfirstvp)
      transLine = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
    if (islastvp && LITdefinie && !LITonsort)
      transLine = LIT.Complement();
  }

  bool onsort = M_FORWARD(transLine.Orientation(TopAbs_OUT));
  bool hasfp = L.HasFirstPoint(), haslp = L.HasLastPoint();
  bool hasfol = hasfp || haslp;
  bool keep   = true;

  // no last defined transition :
  // ----------------------------
  if (!LITdefinie)
  {
    if (onsort)
      keep = false;
    if (iVP == iINONn)
      keep = false;
  }

  // has last defined transition :
  // -----------------------------
  if (LITdefinie)
  {
    bool LprecIsEntrant = (LITdefinie && !LITonsort);
    bool entrantsortant = LprecIsEntrant && onsort;
    if (!entrantsortant)
      keep = false;

    bool sameparoncouture = samepar && couture;
    if (sameparoncouture && hasfol && keep)
    {

      // INFO : on geometric line, vp parameter on line is the point on
      //        curve's parameter.
      TopOpeBRepDS_Point pntVP  = TopOpeBRep_PointGeomTool::MakePoint(VP);
      bool               samegp = FUNBREP_HasSameGPoint(pntVP, I, BDS);

      // !fermee : same p3d && samepar        => same CPI
      // fermee  : (same p3d && samepar
      //           && sameUVon1 && sameUVon2) => same CPI
      bool fermee = L.IsVClosed();
      if (!fermee && samegp)
        keep = false;
      if (fermee && samegp)
      {
        // 2 vp describing a closing line describe the same 3dpoint
        // have same parameter, but do not describe the same uv
        // points on the closed surface (cobo121)
        const TopOpeBRep_VPointInter& lastVP    = L.VPoint(STATIC_lastVPind);
        bool                          sameUVon1 = FUNBREP_SameUV(VP, lastVP, 1, toluv);
        bool                          sameUVon2 = FUNBREP_SameUV(VP, lastVP, 2, toluv);
        keep                                    = !(sameUVon1 && sameUVon2);
      }
    }
    if (sameparoncouture && !hasfol)
    {

      // we have to parametrize the found interference (parameter PIfound)
      // and next interference (parline)
      const occ::handle<TopOpeBRepDS_Interference>& Ifound = DSCIL.First();
      double PIfound = TopOpeBRepDS_InterferenceTool::Parameter(Ifound);
      FUNBREP_Periodize(L, Ifound, PIfound, parline);
      TopOpeBRepDS_InterferenceTool::Parameter(Ifound, PIfound);
      transLine = LIT.Complement();
    }
  } // LITdefinie

  if (keep)
    STATIC_lastVPind = iVP;
  return keep;

} // FUNBREP_topogline_new

//----------------------------------------------------------------------
static bool FUNBREP_topogline(const occ::handle<TopOpeBRepDS_Interference>& Ifound,
                              const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
                              const TopOpeBRep_LineInter&                                     L,
                              const TopOpeBRep_VPointInter&                                   VP,
                              const TopOpeBRepDS_DataStructure&                               BDS,
                              const TopoDS_Shape&                                             E,
                              // const TopoDS_Shape& F,
                              const TopoDS_Shape&,
                              const double toluv,
                              // const bool CPIfound,
                              const bool,
                              const bool               samepar,
                              const bool               couture,
                              double&                  parline,
                              TopOpeBRepDS_Transition& transLine)
//----------------------------------------------------------------------
{
  if (L.TypeLineCurve() == TopOpeBRep_WALKING)
  {
    throw Standard_ProgramError("FUNBREP_topogline : line is not a GLine");
  }

#ifdef OCCT_DEBUG
  bool newkp = TopOpeBRep_GetcontextNEWKP();
  if (newkp)
  {
    bool keep =
      FUNBREP_topogline_new(DSCIL, L, VP, BDS, toluv, samepar, couture, parline, transLine);
    return keep;
  }
#endif

  TopAbs_Orientation Eori = E.Orientation();
  bool               EFR  = M_FORWARD(Eori) || M_REVERSED(Eori);
  int                iVP  = VP.Index();
  int                iINON1, iINONn, nINON;
  L.VPBounds(iINON1, iINONn, nINON);
  bool fermee = L.IsVClosed();

  if (!EFR)
  { // E INTERNAL ou EXTERNAL
    if (iVP == iINON1)
      transLine = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
    if (iVP == iINONn)
      transLine = TopOpeBRepDS_Transition(TopAbs_IN, TopAbs_OUT);
  }
  bool onsort = (transLine.Orientation(TopAbs_OUT) == TopAbs_FORWARD);

  bool                                   LITdefinie = false;
  bool                                   LITonsort  = false;
  TopOpeBRepDS_Transition                LIT;
  occ::handle<TopOpeBRepDS_Interference> I;
  bool                                   dscilempty = DSCIL.IsEmpty();

  // xpu : 28-05-97 : if VP<iVP> is on 3 and
  // VP on 1 has transition OUT/IN, VP on 2 has transition IN/OUT,
  // same VP will be kept.
  // the line is described with (VPmin,VPmax) boundaries.
  // if VP is VPmax, as we are only regarding IN/OUT transitions->ok
  // if VP is VPmin :
  bool newvvv = true; // xpu : 28-05-97
  if (newvvv)
  {
    bool notkept = !dscilempty && (iVP == 1);
    if (notkept)
      return false;
  } // 28-05-97 : xpu

  if (!dscilempty)
  {
    I          = DSCIL.Last();
    LIT        = I->Transition();
    LITdefinie = !LIT.IsUnknown();
    if (LITdefinie)
      LITonsort = (LIT.Orientation(TopAbs_OUT) == TopAbs_FORWARD);
  }

  // xpu : 21-05-97 (see TopOpeBRep_FacesFiller::ProcessVPonR)
  // vpmin with transition UNKNOWN =>transLine-> OUT/IN
  // vpmin OUT/IN, vpmax with transition UNKNOWN =>transLine-> IN/OUT
  bool trliunk   = transLine.IsUnknown();
  bool newvv     = true; // xpu : 21-05-97
  bool isfirstvp = (iVP == iINON1);
  bool islastvp  = (iVP == iINONn);
  if (newvv && trliunk)
  {
    if (isfirstvp)
    {
      transLine = TopOpeBRepDS_Transition(TopAbs_OUT, TopAbs_IN);
      onsort    = false;
    }
    if (islastvp)
      if (LITdefinie && !LITonsort)
      {
        transLine = LIT.Complement();
        onsort    = true;
      }
  } // 21-05-97 :xpu

  bool hasfp  = L.HasFirstPoint();
  bool haslp  = L.HasLastPoint();
  bool hasfol = hasfp || haslp;

  bool keep = true;

  if (!LITdefinie)
  {
    if (onsort)
      keep = false;
    if (iVP == iINONn)
      keep = false;
  }
  if (LITdefinie)
  {
    bool LprecIsEntrant = (LITdefinie && !LITonsort);
    bool entrantsortant = LprecIsEntrant && onsort;
    if (!entrantsortant)
      keep = false;

    bool sameparoncouture = samepar && couture;
    if (sameparoncouture && hasfol)
    {
      if (keep)
      {
        // INFO : on geometric line, vp parameter on line is the point on
        //        curve parameter.
        TopOpeBRepDS_Point            pntVP  = TopOpeBRep_PointGeomTool::MakePoint(VP);
        bool                          samegp = FUNBREP_HasSameGPoint(pntVP, I, BDS);
        const TopOpeBRep_VPointInter& lastVP = L.VPoint(STATIC_lastVPind);
        // if the line is not closed, same p3d and samepar represent
        // same CPI.else :
        if (fermee)
        {
          // 2 vp describing a closing line describe the same 3dpoint
          // have same parameter, but do not describe the same uv
          // points on the closed surface (cobo121)
          bool sameUVon1 = FUNBREP_SameUV(VP, lastVP, 1, toluv);
          bool sameUVon2 = FUNBREP_SameUV(VP, lastVP, 2, toluv);
          samegp         = samegp && sameUVon1 && sameUVon2;
        }
        if (samegp)
          keep = false;
      }
    }
    if (sameparoncouture && !hasfol)
    {
      // parametrize Ifound (parameter PIfound) and next interference (parline)
      double PIfound = TopOpeBRepDS_InterferenceTool::Parameter(Ifound);
      FUNBREP_Periodize(L, Ifound, PIfound, parline);
      TopOpeBRepDS_InterferenceTool::Parameter(Ifound, PIfound);
      if (LITdefinie)
        transLine = LIT.Complement();
    }
  }

  if (keep)
    STATIC_lastVPind = iVP;
  return keep;

} // end of FUNBREP_topogline

//-----------------------------------------------------------------------
static bool TopoParameter(const TopOpeBRep_LineInter&                   L,
                          const occ::handle<TopOpeBRepDS_Interference>& I,
                          const double                                  parline,
                          const bool                                    closingedge)
//-----------------------------------------------------------------------
{
  if (I.IsNull())
    return false;
  bool   samepar = false;
  double pCPI    = TopOpeBRepDS_InterferenceTool::Parameter(I);
  if (!closingedge)
    samepar = (std::abs(parline - pCPI) < Precision::PConfusion());
  else
  {
    // trouve et couture et courbe periodique :
    // on retourne vrai pour laisser a FUNBREP_topokpart
    // le soin de definir correctement les couples
    // (parametre, transition) pour les points confondus sur couture.
    bool perio = L.IsPeriodic();
    if (perio)
    {
      samepar = true;
    }
    else
    {
      samepar = (std::abs(parline - pCPI) < Precision::PConfusion());
    }
  }
  return samepar;
}

//----------------------------------------------------------------------
Standard_EXPORT bool FUNBREP_topokpart(
  const occ::handle<TopOpeBRepDS_Interference>&                   Ifound,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& DSCIL,
  const TopOpeBRep_LineInter&                                     L,
  const TopOpeBRep_VPointInter&                                   VP,
  const TopOpeBRepDS_DataStructure&                               BDS,
  const TopoDS_Shape&                                             E,
  const TopoDS_Shape&                                             F,
  const double                                                    toluv,
  double&                                                         parline,
  TopOpeBRepDS_Transition&                                        transLine)
//----------------------------------------------------------------------
{
  bool keep = true;

  bool CPIfound = !Ifound.IsNull();
  bool couture  = TopOpeBRepTool_ShapeTool::Closed(TopoDS::Edge(E), TopoDS::Face(F));
  // clang-format off
  bool samepar = false; // = True if current VPoint falls on an existing geometry with an equal parameter.
  // clang-format on
  if (!CPIfound)
    samepar = false;
  else
    samepar = CPIfound ? TopoParameter(L, Ifound, parline, couture) : false;

  TopOpeBRepDS_Transition lasttransLine;
  if (!DSCIL.IsEmpty())
    lasttransLine = DSCIL.Last()->Transition(); // xpu12-12-97

  // A line is valid if at least it has VPi1 and VPi2 with i1 < i2 and :
  // transition on line for VPi1 : OUT/IN and for VPi2 : IN/OUT.
  // When <VPj> is on j=i1/i2, we keep it (returning true).
  // Rmq :VP internal to the face is given with transition on line
  // OUT/IN or IN/OUT if it is on the beginning or on the end
  // of the line.

  if (L.TypeLineCurve() == TopOpeBRep_WALKING)
  {
    keep = FUNBREP_topowalki(Ifound,
                             DSCIL,
                             L,
                             VP,
                             lasttransLine,
                             BDS,
                             E,
                             F,
                             toluv,
                             CPIfound,
                             samepar,
                             couture,
                             parline,
                             transLine);
  }
  else
  {
    keep = FUNBREP_topogline(Ifound,
                             DSCIL,
                             L,
                             VP,
                             BDS,
                             E,
                             F,
                             toluv,
                             CPIfound,
                             samepar,
                             couture,
                             parline,
                             transLine);
  }
  return keep;
} // end of FUNBREP_tpokpart
