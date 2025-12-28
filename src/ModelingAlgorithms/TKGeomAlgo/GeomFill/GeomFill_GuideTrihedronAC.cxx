// Created by: Stephanie HUMEAU
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

// Created:	Tue Jun 23 15:39:24 1998

#include <Adaptor3d_Curve.hxx>
#include <Approx_CurvlinFunc.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_GuideTrihedronAC.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_GuideTrihedronAC, GeomFill_TrihedronWithGuide)

//=================================================================================================

GeomFill_GuideTrihedronAC::GeomFill_GuideTrihedronAC(const occ::handle<Adaptor3d_Curve>& guide)
{
  myCurve.Nullify();
  myGuide   = guide;
  myTrimG   = guide;
  myGuideAC = new (Approx_CurvlinFunc)(myGuide, 1.e-7);
  Lguide    = myGuideAC->GetLength();
  UTol = STol = Precision::PConfusion();
  Orig1       = 0; // origines pour le cas path multi-edges
  Orig2       = 1;
}

//=======================================================================
// function : Guide
// purpose  : calculation of trihedron
//=======================================================================

occ::handle<Adaptor3d_Curve> GeomFill_GuideTrihedronAC::Guide() const
{
  return myGuide;
}

//=======================================================================
// function : D0
// purpose  : calculation of trihedron
//=======================================================================
bool GeomFill_GuideTrihedronAC::D0(const double Param,
                                   gp_Vec&      Tangent,
                                   gp_Vec&      Normal,
                                   gp_Vec&      BiNormal)
{
  double s     = myCurveAC->GetSParameter(Param); // abscisse curviligne <=> Param
  double OrigG = Orig1 + s * (Orig2 - Orig1);     // abscisse curv sur le guide (cas multi-edges)
  double tG    = myGuideAC->GetUParameter(*myGuide, OrigG, 1); // param <=> s sur theGuide

  gp_Pnt P, PG;
  gp_Vec To, B;
  myTrimmed->D1(Param, P, To); // point et derivee au parametre Param sur myCurve
  myTrimG->D0(tG, PG);         // point au parametre tG sur myGuide
  myCurPointOnGuide = PG;

  gp_Vec n(P, PG); // vecteur definissant la normale

  Normal   = n.Normalized();
  B        = To.Crossed(Normal);
  BiNormal = B / B.Magnitude();
  Tangent  = Normal.Crossed(BiNormal);
  Tangent.Normalize();

  return true;
}

//=======================================================================
// function : D1
// purpose  : calculation of trihedron and first derivative
//=======================================================================
bool GeomFill_GuideTrihedronAC::D1(const double Param,
                                   gp_Vec&      Tangent,
                                   gp_Vec&      DTangent,
                                   gp_Vec&      Normal,
                                   gp_Vec&      DNormal,
                                   gp_Vec&      BiNormal,
                                   gp_Vec&      DBiNormal)
{
  // triedre
  double s, OrigG, tG, dtg;
  // abscisse curviligne <=> Param
  s = myCurveAC->GetSParameter(Param);
  // parametre <=> s sur theGuide
  OrigG = Orig1 + s * (Orig2 - Orig1);
  // parametre <=> s sur  theGuide
  tG = myGuideAC->GetUParameter(*myGuide, OrigG, 1);

  gp_Pnt P, PG;
  gp_Vec To, DTo, TG, B, BPrim;

  myTrimmed->D2(Param, P, To, DTo);
  myTrimG->D1(tG, PG, TG);
  myCurPointOnGuide = PG;

  gp_Vec n(P, PG), dn;
  double Norm = n.Magnitude();
  if (Norm < 1.e-12)
  {
    Norm = 1;
#ifdef OCCT_DEBUG
    std::cout << "GuideTrihedronAC : Normal indefinie" << std::endl;
#endif
  }

  n /= Norm;
  // derivee de n par rapport a Param
  dtg = (Orig2 - Orig1) * (To.Magnitude() / TG.Magnitude()) * (Lguide / L);
  dn.SetLinearForm(dtg, TG, -1, To);
  dn /= Norm;

  // triedre
  Normal       = n;
  B            = To.Crossed(Normal);
  double NormB = B.Magnitude();
  B /= NormB;

  BiNormal = B;

  Tangent = Normal.Crossed(BiNormal);
  Tangent.Normalize();

  // derivee premiere
  DNormal.SetLinearForm(-(n.Dot(dn)), n, dn);

  BPrim.SetLinearForm(DTo.Crossed(Normal), To.Crossed(DNormal));

  DBiNormal.SetLinearForm(-(B.Dot(BPrim)), B, BPrim);
  DBiNormal /= NormB;

  DTangent.SetLinearForm(Normal.Crossed(DBiNormal), DNormal.Crossed(BiNormal));

  return true;
}

//=======================================================================
// function : D2
// purpose  : calculation of trihedron and derivatives
//=======================================================================
bool GeomFill_GuideTrihedronAC::D2(const double Param,
                                   gp_Vec&      Tangent,
                                   gp_Vec&      DTangent,
                                   gp_Vec&      D2Tangent,
                                   gp_Vec&      Normal,
                                   gp_Vec&      DNormal,
                                   gp_Vec&      D2Normal,
                                   gp_Vec&      BiNormal,
                                   gp_Vec&      DBiNormal,
                                   gp_Vec&      D2BiNormal)
{
  // abscisse curviligne <=> Param
  double s = myCurveAC->GetSParameter(Param);
  // parametre <=> s sur theGuide
  double OrigG = Orig1 + s * (Orig2 - Orig1);
  double tG    = myGuideAC->GetUParameter(*myGuide, OrigG, 1);

  gp_Pnt P, PG;
  gp_Vec TG, DTG;
  //  gp_Vec To,DTo,D2To,B;
  gp_Vec To, DTo, D2To;

  myTrimmed->D3(Param, P, To, DTo, D2To);
  myTrimG->D2(tG, PG, TG, DTG);
  myCurPointOnGuide = PG;

  double NTo  = To.Magnitude();
  double N2To = To.SquareMagnitude();
  double NTG  = TG.Magnitude();
  double N2Tp = TG.SquareMagnitude();
  double d2tp_dt2, dtg_dt;
  dtg_dt = (Orig2 - Orig1) * (NTo / NTG) * (Lguide / L);

  gp_Vec n(P, PG); // vecteur definissant la normale
  double Norm = n.Magnitude(), ndn;
  // derivee de n par rapport a Param
  gp_Vec dn, d2n;
  dn.SetLinearForm(dtg_dt, TG, -1, To);

  // derivee seconde de tG par rapport a Param
  d2tp_dt2 = (Orig2 - Orig1) * (Lguide / L)
             * (DTo.Dot(To) / (NTo * NTG) - N2To * TG * DTG * (Lguide / L) / (N2Tp * N2Tp));
  // derivee seconde de n par rapport a Param
  d2n.SetLinearForm(dtg_dt * dtg_dt, DTG, d2tp_dt2, TG, -1, DTo);

  if (Norm > 1.e-9)
  {
    n /= Norm;
    dn /= Norm;
    d2n /= Norm;
  }
  // triedre
  Normal = n;

  gp_Vec TN, DTN, D2TN;
  TN = To.Crossed(Normal);

  double Norma = TN.Magnitude();
  if (Norma > 1.e-9)
    TN /= Norma;

  BiNormal = TN;

  Tangent = Normal.Crossed(BiNormal);
  //  Tangent.Normalize();

  // derivee premiere du triedre
  //  gp_Vec DTN = DTo.Crossed(Normal);
  //  gp_Vec TDN = To.Crossed(DNormal);
  //  gp_Vec DT = DTN + TDN;

  ndn = n.Dot(dn);
  DNormal.SetLinearForm(-ndn, n, dn);

  DTN.SetLinearForm(DTo.Crossed(Normal), To.Crossed(DNormal));
  DTN /= Norma;
  double TNDTN = TN.Dot(DTN);

  DBiNormal.SetLinearForm(-TNDTN, TN, DTN);

  DTangent.SetLinearForm(Normal.Crossed(DBiNormal), DNormal.Crossed(BiNormal));

// derivee seconde du triedre
#ifdef OCCT_DEBUG
  gp_Vec DTDN = DTo.Crossed(DNormal);
  (void)DTDN;
#endif
  double TN2 = TN.SquareMagnitude();

  D2Normal.SetLinearForm(-2 * ndn, dn, 3 * ndn * ndn - (dn.SquareMagnitude() + n.Dot(d2n)), n, d2n);

  D2TN.SetLinearForm(1, D2To.Crossed(Normal), 2, DTo.Crossed(DNormal), To.Crossed(D2Normal));
  D2TN /= Norma;

  D2BiNormal.SetLinearForm(-2 * TNDTN, DTN, 3 * TNDTN * TNDTN - (TN2 + TN.Dot(D2TN)), TN, D2TN);

  D2Tangent.SetLinearForm(1,
                          D2Normal.Crossed(BiNormal),
                          2,
                          DNormal.Crossed(DBiNormal),
                          Normal.Crossed(D2BiNormal));

  //  return true;
  return false;
}

//=================================================================================================

occ::handle<GeomFill_TrihedronLaw> GeomFill_GuideTrihedronAC::Copy() const
{
  occ::handle<GeomFill_GuideTrihedronAC> copy = new (GeomFill_GuideTrihedronAC)(myGuide);
  copy->SetCurve(myCurve);
  copy->Origine(Orig1, Orig2);
  return copy;
}

//=================================================================================================

bool GeomFill_GuideTrihedronAC::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  myCurve   = C;
  myTrimmed = C;
  if (!myCurve.IsNull())
  {
    myCurveAC = new (Approx_CurvlinFunc)(C, 1.e-7);
    L         = myCurveAC->GetLength();
    //    CorrectOrient(myGuide);
  }
  return true;
}

//=================================================================================================

int GeomFill_GuideTrihedronAC::NbIntervals(const GeomAbs_Shape S) const
{
  int Nb;
  Nb = myCurveAC->NbIntervals(S);
  NCollection_Array1<double> DiscC(1, Nb + 1);
  myCurveAC->Intervals(DiscC, S);
  Nb = myGuideAC->NbIntervals(S);
  NCollection_Array1<double> DiscG(1, Nb + 1);
  myGuideAC->Intervals(DiscG, S);

  NCollection_Sequence<double> Seq;
  GeomLib::FuseIntervals(DiscC, DiscG, Seq);

  return Seq.Length() - 1;
}

//=================================================================================================

void GeomFill_GuideTrihedronAC::Intervals(NCollection_Array1<double>& TT,
                                          const GeomAbs_Shape         S) const
{
  int Nb, ii;
  Nb = myCurveAC->NbIntervals(S);
  NCollection_Array1<double> DiscC(1, Nb + 1);
  myCurveAC->Intervals(DiscC, S);
  Nb = myGuideAC->NbIntervals(S);
  NCollection_Array1<double> DiscG(1, Nb + 1);
  myGuideAC->Intervals(DiscG, S);

  NCollection_Sequence<double> Seq;
  GeomLib::FuseIntervals(DiscC, DiscG, Seq);
  Nb = Seq.Length();

  for (ii = 1; ii <= Nb; ii++)
  {
    TT(ii) = myCurveAC->GetUParameter(*myCurve, Seq(ii), 1);
  }
}

//=================================================================================================

void GeomFill_GuideTrihedronAC::SetInterval(const double First, const double Last)
{
  myTrimmed = myCurve->Trim(First, Last, UTol);
  double Sf, Sl, U;

  Sf = myCurveAC->GetSParameter(First);
  Sl = myCurveAC->GetSParameter(Last);
  //  if (Sl>1) Sl=1;
  //  myCurveAC->Trim(Sf, Sl, UTol);

  U       = Orig1 + Sf * (Orig2 - Orig1);
  Sf      = myGuideAC->GetUParameter(*myGuide, U, 1);
  U       = Orig1 + Sl * (Orig2 - Orig1);
  Sl      = myGuideAC->GetUParameter(*myGuide, U, 1);
  myTrimG = myGuide->Trim(Sf, Sl, UTol);
}

//=================================================================================================

void GeomFill_GuideTrihedronAC::GetAverageLaw(gp_Vec& ATangent, gp_Vec& ANormal, gp_Vec& ABiNormal)
{
  int    ii;
  double t, Delta = (myCurve->LastParameter() - myCurve->FirstParameter()) / 20.001;

  ATangent.SetCoord(0., 0., 0.);
  ANormal.SetCoord(0., 0., 0.);
  ABiNormal.SetCoord(0., 0., 0.);
  gp_Vec T, N, B;

  for (ii = 1; ii <= 20; ii++)
  {
    t = myCurve->FirstParameter() + (ii - 1) * Delta;
    D0(t, T, N, B);
    ATangent += T;
    ANormal += N;
    ABiNormal += B;
  }
  ATangent /= 20;
  ANormal /= 20;
  ABiNormal /= 20;
}

//=================================================================================================

bool GeomFill_GuideTrihedronAC::IsConstant() const
{
  return false;
}

//=================================================================================================

bool GeomFill_GuideTrihedronAC::IsOnlyBy3dCurve() const
{
  return false;
}

//=================================================================================================

void GeomFill_GuideTrihedronAC::Origine(const double OrACR1, const double OrACR2)
{
  Orig1 = OrACR1;
  Orig2 = OrACR2;
}
