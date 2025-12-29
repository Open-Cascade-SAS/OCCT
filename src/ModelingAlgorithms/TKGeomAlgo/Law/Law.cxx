// Created on: 1995-11-20
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Law.hxx>

#include <Adaptor3d_Curve.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <Law_BSpFunc.hxx>
#include <Law_BSpline.hxx>
#include <Law_Interpolate.hxx>
#include <Law_Linear.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

occ::handle<Law_BSpFunc> Law::MixBnd(const occ::handle<Law_Linear>& Lin)
{
  double f, l;
  Lin->Bounds(f, l);
  NCollection_Array1<double> Knots(1, 4);
  NCollection_Array1<int>    Mults(1, 4);
  Knots(1) = f;
  Knots(4) = l;
  Knots(2) = 0.75 * f + 0.25 * l;
  Knots(3) = 0.25 * f + 0.75 * l;
  Mults(1) = Mults(4) = 4;
  Mults(2) = Mults(3)                          = 1;
  occ::handle<NCollection_HArray1<double>> pol = Law::MixBnd(3, Knots, Mults, Lin);
  occ::handle<Law_BSpline>                 bs  = new Law_BSpline(pol->Array1(), Knots, Mults, 3);
  occ::handle<Law_BSpFunc>                 bsf = new Law_BSpFunc();
  bsf->SetCurve(bs);
  return bsf;
}

occ::handle<NCollection_HArray1<double>> Law::MixBnd(const int                         Degree,
                                                     const NCollection_Array1<double>& Knots,
                                                     const NCollection_Array1<int>&    Mults,
                                                     const occ::handle<Law_Linear>&    Lin)
{
  int nbpol = 0, nbfk = 0, i, j, k = 0;
  for (i = Mults.Lower(); i <= Mults.Upper(); i++)
  {
    nbfk += Mults(i);
  }
  NCollection_Array1<double> fk(1, nbfk);
  for (i = Mults.Lower(); i <= Mults.Upper(); i++)
  {
    for (j = 1; j <= Mults(i); j++)
    {
      fk(++k) = Knots(i);
    }
  }
  nbpol = nbfk - Degree - 1;
  NCollection_Array1<double> par(1, nbpol);
  BSplCLib::BuildSchoenbergPoints(Degree, fk, par);
  occ::handle<NCollection_HArray1<double>> res = new NCollection_HArray1<double>(1, nbpol);
  NCollection_Array1<double>&              pol = res->ChangeArray1();
  for (i = 1; i <= nbpol; i++)
  {
    pol(i) = Lin->Value(par(i));
  }
  NCollection_Array1<int> ord(1, nbpol);
  ord.Init(0);
  BSplCLib::Interpolate(Degree, fk, par, ord, 1, pol(1), i);
  if (nbpol >= 4)
  {
    pol(2)         = pol(1);
    pol(nbpol - 1) = pol(nbpol);
  }
  return res;
}

static double eval1(const double p,
                    const double first,
                    const double last,
                    const double piv,
                    const bool   nulr)
{
  if ((nulr && p >= piv) || (!nulr && p <= piv))
    return 0.;
  else if (nulr)
  {
    double a = piv - first;
    a *= a;
    a        = 1. / a;
    double b = p - first;
    a *= b;
    b = piv - p;
    a *= b;
    a *= b;
    return a;
  }
  else
  {
    double a = last - piv;
    a *= a;
    a        = 1. / a;
    double b = last - p;
    a *= b;
    b = p - piv;
    a *= b;
    a *= b;
    return a;
  }
}

occ::handle<NCollection_HArray1<double>> Law::MixTgt(const int                         Degree,
                                                     const NCollection_Array1<double>& Knots,
                                                     const NCollection_Array1<int>&    Mults,
                                                     const bool NulOnTheRight,
                                                     const int  Index)
{
  double first = Knots(Knots.Lower());
  double last  = Knots(Knots.Upper());
  double piv   = Knots(Index);
  int    nbpol = 0, nbfk = 0, i, j, k = 0;
  for (i = Mults.Lower(); i <= Mults.Upper(); i++)
  {
    nbfk += Mults(i);
  }
  NCollection_Array1<double> fk(1, nbfk);
  for (i = Mults.Lower(); i <= Mults.Upper(); i++)
  {
    for (j = 1; j <= Mults(i); j++)
    {
      fk(++k) = Knots(i);
    }
  }
  nbpol = nbfk - Degree - 1;
  NCollection_Array1<double> par(1, nbpol);
  BSplCLib::BuildSchoenbergPoints(Degree, fk, par);
  occ::handle<NCollection_HArray1<double>> res = new NCollection_HArray1<double>(1, nbpol);
  NCollection_Array1<double>&              pol = res->ChangeArray1();
  for (i = 1; i <= nbpol; i++)
  {
    pol(i) = eval1(par(i), first, last, piv, NulOnTheRight);
  }
  NCollection_Array1<int> ord(1, nbpol);
  ord.Init(0);
  BSplCLib::Interpolate(Degree, fk, par, ord, 1, pol(1), i);
  return res;
}

occ::handle<Law_BSpline> Law::Reparametrize(const Adaptor3d_Curve& Curve,
                                            const double           First,
                                            const double           Last,
                                            const bool             HasDF,
                                            const bool             HasDL,
                                            const double           DFirst,
                                            const double           DLast,
                                            const bool             Rev,
                                            const int              NbPoints)
{
  // On evalue la longeur approximative de la courbe.

  int    i;
  double DDFirst = DFirst, DDLast = DLast;
  if (HasDF && Rev)
    DDFirst = -DFirst;
  if (HasDL && Rev)
    DDLast = -DLast;
  NCollection_Array1<double> cumdist(1, 2 * NbPoints);
  NCollection_Array1<double> ucourbe(1, 2 * NbPoints);
  gp_Pnt                     P1, P2;
  double                     U1 = Curve.FirstParameter();
  double                     U2 = Curve.LastParameter();
  double                     U, DU, Length = 0.;
  if (!Rev)
  {
    P1 = Curve.Value(U1);
    U  = U1;
    DU = (U2 - U1) / (2 * NbPoints - 1);
  }
  else
  {
    P1 = Curve.Value(U2);
    U  = U2;
    DU = (U1 - U2) / (2 * NbPoints - 1);
  }
  for (i = 1; i <= 2 * NbPoints; i++)
  {
    P2 = Curve.Value(U);
    Length += P2.Distance(P1);
    cumdist(i) = Length;
    ucourbe(i) = U;
    U += DU;
    P1 = P2;
  }
  if (Rev)
    ucourbe(2 * NbPoints) = U1;
  else
    ucourbe(2 * NbPoints) = U2;

  occ::handle<NCollection_HArray1<double>> point = new NCollection_HArray1<double>(1, NbPoints);
  occ::handle<NCollection_HArray1<double>> param = new NCollection_HArray1<double>(1, NbPoints);

  double DCorde = Length / (NbPoints - 1);
  double Corde  = DCorde;
  int    Index  = 1;
  double Alpha;
  double fac = 1. / (NbPoints - 1);

  point->SetValue(1, ucourbe(1));
  param->SetValue(1, First);
  point->SetValue(NbPoints, ucourbe(2 * NbPoints));
  param->SetValue(NbPoints, Last);

  for (i = 2; i < NbPoints; i++)
  {
    while (cumdist(Index) < Corde)
      Index++;

    Alpha = (Corde - cumdist(Index - 1)) / (cumdist(Index) - cumdist(Index - 1));
    U     = ucourbe(Index - 1) + Alpha * (ucourbe(Index) - ucourbe(Index - 1));
    point->SetValue(i, U);
    param->SetValue(i, ((NbPoints - i) * First + (i - 1) * Last) * fac);
    Corde = i * DCorde;
  }
  Law_Interpolate inter(point, param, false, 1.e-9);
  if (HasDF || HasDL)
  {
    NCollection_Array1<double>             tgt(1, NbPoints);
    occ::handle<NCollection_HArray1<bool>> flag = new NCollection_HArray1<bool>(1, NbPoints);
    flag->ChangeArray1().Init(false);
    if (HasDF)
    {
      flag->SetValue(1, true);
      tgt.SetValue(1, DDFirst);
    }
    if (HasDL)
    {
      flag->SetValue(NbPoints, true);
      tgt.SetValue(NbPoints, DDLast);
    }
    inter.Load(tgt, flag);
  }
  inter.Perform();
  if (!inter.IsDone())
    throw Standard_Failure("Law::Reparametrize echec interpolation");
  occ::handle<Law_BSpline> bs = inter.Curve();
  return bs;
}

static double eval2(const double p,
                    //			   const double        first,
                    const double,
                    //			   const double        last,
                    const double,
                    const double                    mil,
                    const bool                      hasfirst,
                    const bool                      haslast,
                    const occ::handle<Law_BSpline>& bs1,
                    const occ::handle<Law_BSpline>& bs2)
{
  if (hasfirst && p < mil)
    return bs1->Value(p);
  else if (haslast && p > mil)
    return bs2->Value(p);
  else
    return 1.;
}

occ::handle<Law_BSpline> Law::Scale(const double First,
                                    const double Last,
                                    const bool   HasF,
                                    const bool   HasL,
                                    const double VFirst,
                                    const double VLast)
{
  int                        i;
  double                     Milieu = 0.5 * (First + Last);
  NCollection_Array1<double> knot(1, 3);
  NCollection_Array1<double> fknot(1, 10);
  NCollection_Array1<int>    mult(1, 3);
  knot(1)  = First;
  knot(2)  = Milieu;
  knot(3)  = Last;
  fknot(1) = fknot(2) = fknot(3) = fknot(4) = First;
  fknot(10) = fknot(9) = fknot(8) = fknot(7) = Last;
  fknot(5) = fknot(6) = Milieu;
  mult(1)             = 4;
  mult(3)             = 4;
  mult(2)             = 2;

  NCollection_Array1<double> pbs(1, 4);
  NCollection_Array1<double> kbs(1, 2);
  NCollection_Array1<int>    mbs(1, 2);
  mbs(1) = mbs(2) = 4;
  occ::handle<Law_BSpline> bs1, bs2;
  if (HasF)
  {
    pbs(1) = pbs(2) = VFirst;
    pbs(3) = pbs(4) = 1.;
    kbs(1)          = First;
    kbs(2)          = Milieu;
    bs1             = new Law_BSpline(pbs, kbs, mbs, 3);
  }
  if (HasL)
  {
    pbs(1) = pbs(2) = 1.;
    pbs(3) = pbs(4) = VLast;
    kbs(1)          = Milieu;
    kbs(2)          = Last;
    bs2             = new Law_BSpline(pbs, kbs, mbs, 3);
  }

  NCollection_Array1<double> pol(1, 6);
  NCollection_Array1<double> par(1, 6);
  BSplCLib::BuildSchoenbergPoints(3, fknot, par);
  for (i = 1; i <= 6; i++)
  {
    pol(i) = eval2(par(i), First, Last, Milieu, HasF, HasL, bs1, bs2);
  }
  NCollection_Array1<int> ord(1, 6);
  ord.Init(0);
  BSplCLib::Interpolate(3, fknot, par, ord, 1, pol(1), i);
  bs1 = new Law_BSpline(pol, knot, mult, 3);
  return bs1;
}

occ::handle<Law_BSpline> Law::ScaleCub(const double First,
                                       const double Last,
                                       const bool   HasF,
                                       const bool   HasL,
                                       const double VFirst,
                                       const double VLast)
{
  // int i;
  double                     Milieu = 0.5 * (First + Last);
  NCollection_Array1<double> pol(1, 5);
  NCollection_Array1<double> knot(1, 3);
  NCollection_Array1<int>    mult(1, 3);
  knot(1) = First;
  knot(2) = Milieu;
  knot(3) = Last;
  mult(1) = 4;
  mult(3) = 4;
  mult(2) = 1;

  occ::handle<Law_BSpline> bs;
  if (HasF)
  {
    pol(1) = pol(2) = VFirst;
  }
  else
  {
    pol(1) = pol(2) = 1.;
  }
  if (HasL)
  {
    pol(4) = pol(5) = VLast;
  }
  else
  {
    pol(4) = pol(5) = 1.;
  }

  pol(3) = 1.;

  bs = new Law_BSpline(pol, knot, mult, 3);
  return bs;
}
