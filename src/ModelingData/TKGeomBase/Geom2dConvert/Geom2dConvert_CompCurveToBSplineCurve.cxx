// Created on: 1997-04-29
// Created by: Stagiaire Francois DUMONT
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

#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

Geom2dConvert_CompCurveToBSplineCurve::Geom2dConvert_CompCurveToBSplineCurve(
  const Convert_ParameterisationType theParameterisation)
    : myTol(Precision::Confusion()),
      myType(theParameterisation)
{
  //
}

//=================================================================================================

Geom2dConvert_CompCurveToBSplineCurve::Geom2dConvert_CompCurveToBSplineCurve(
  const occ::handle<Geom2d_BoundedCurve>& BasisCurve,
  const Convert_ParameterisationType Parameterisation)
    : myTol(Precision::Confusion()),
      myType(Parameterisation)
{
  occ::handle<Geom2d_BSplineCurve> Bs = occ::down_cast<Geom2d_BSplineCurve>(BasisCurve);
  if (!Bs.IsNull())
  {
    myCurve = occ::down_cast<Geom2d_BSplineCurve>(BasisCurve->Copy());
  }
  else
  {
    myCurve = Geom2dConvert::CurveToBSplineCurve(BasisCurve, myType);
  }
}

//=================================================================================================

bool Geom2dConvert_CompCurveToBSplineCurve::Add(
  const occ::handle<Geom2d_BoundedCurve>& NewCurve,
  const double                Tolerance,
  const bool             After)
{
  // conversion
  occ::handle<Geom2d_BSplineCurve> Bs = occ::down_cast<Geom2d_BSplineCurve>(NewCurve);
  if (!Bs.IsNull())
  {
    Bs = occ::down_cast<Geom2d_BSplineCurve>(NewCurve->Copy());
  }
  else
  {
    Bs = Geom2dConvert::CurveToBSplineCurve(NewCurve, myType);
  }
  if (myCurve.IsNull())
  {
    myCurve = Bs;
    return true;
  }

  myTol                      = Tolerance;
  const double aSqTol = Tolerance * Tolerance;

  int LBs = Bs->NbPoles(), LCb = myCurve->NbPoles();
  double    d1 = myCurve->Pole(1).SquareDistance(Bs->Pole(1));
  double    d2 = myCurve->Pole(1).SquareDistance(Bs->Pole(LBs));

  bool isBeforeReversed =
    (myCurve->Pole(1).SquareDistance(Bs->Pole(1)) < aSqTol) && (d1 < d2);
  bool isBefore =
    (myCurve->Pole(1).SquareDistance(Bs->Pole(LBs)) < aSqTol) || isBeforeReversed;

  d1 = myCurve->Pole(LCb).SquareDistance(Bs->Pole(1));
  d2 = myCurve->Pole(LCb).SquareDistance(Bs->Pole(LBs));

  bool isAfterReversed =
    (myCurve->Pole(LCb).SquareDistance(Bs->Pole(LBs)) < aSqTol) && (d2 < d1);
  bool isAfter =
    (myCurve->Pole(LCb).SquareDistance(Bs->Pole(1)) < aSqTol) || isAfterReversed;

  // myCurve and NewCurve together form a closed curve
  if (isBefore && isAfter)
  {
    if (After)
    {
      isBefore = false;
    }
    else
    {
      isAfter = false;
    }
  }
  if (isAfter)
  {
    if (isAfterReversed)
    {
      Bs->Reverse();
    }
    Add(myCurve, Bs, true);
    return true;
  }
  else if (isBefore)
  {
    if (isBeforeReversed)
    {
      Bs->Reverse();
    }
    Add(Bs, myCurve, false);
    return true;
  }

  return false;
}

//=================================================================================================

void Geom2dConvert_CompCurveToBSplineCurve::Add(occ::handle<Geom2d_BSplineCurve>& FirstCurve,
                                                occ::handle<Geom2d_BSplineCurve>& SecondCurve,
                                                const bool       After)
{
  // Harmonisation des degres.
  int Deg = std::max(FirstCurve->Degree(), SecondCurve->Degree());
  if (FirstCurve->Degree() < Deg)
  {
    FirstCurve->IncreaseDegree(Deg);
  }
  if (SecondCurve->Degree() < Deg)
  {
    SecondCurve->IncreaseDegree(Deg);
  }

  // Declarationd
  double           L1, L2, U_de_raccord;
  int        ii, jj;
  double           Ratio = 1, Ratio1, Ratio2, Delta1, Delta2;
  int        NbP1 = FirstCurve->NbPoles(), NbP2 = SecondCurve->NbPoles();
  int        NbK1 = FirstCurve->NbKnots(), NbK2 = SecondCurve->NbKnots();
  NCollection_Array1<double>    Noeuds(1, NbK1 + NbK2 - 1);
  NCollection_Array1<gp_Pnt2d>    Poles(1, NbP1 + NbP2 - 1);
  NCollection_Array1<double>    Poids(1, NbP1 + NbP2 - 1);
  NCollection_Array1<int> Mults(1, NbK1 + NbK2 - 1);

  // Ratio de reparametrisation (C1 si possible)
  L1 = FirstCurve->DN(FirstCurve->LastParameter(), 1).Magnitude();
  L2 = SecondCurve->DN(SecondCurve->FirstParameter(), 1).Magnitude();

  if ((L1 > Precision::Confusion()) && (L2 > Precision::Confusion()))
  {
    Ratio = L1 / L2;
  }
  if ((Ratio < Precision::Confusion()) || (Ratio > 1 / Precision::Confusion()))
  {
    Ratio = 1;
  }

  if (After)
  {
    // On ne bouge pas la premiere courbe
    Ratio1       = 1;
    Delta1       = 0;
    Ratio2       = 1 / Ratio;
    Delta2       = Ratio2 * SecondCurve->Knot(1) - FirstCurve->Knot(NbK1);
    U_de_raccord = FirstCurve->LastParameter();
  }
  else
  {
    // On ne bouge pas la seconde courbe
    Ratio1       = Ratio;
    Delta1       = Ratio1 * FirstCurve->Knot(NbK1) - SecondCurve->Knot(1);
    Ratio2       = 1;
    Delta2       = 0;
    U_de_raccord = SecondCurve->FirstParameter();
  }

  // Les Noeuds

  for (ii = 1; ii < NbK1; ii++)
  {
    Noeuds(ii) = Ratio1 * FirstCurve->Knot(ii) - Delta1;
    Mults(ii)  = FirstCurve->Multiplicity(ii);
  }
  Noeuds(NbK1) = U_de_raccord;
  Mults(NbK1)  = FirstCurve->Degree();
  for (ii = 2, jj = NbK1 + 1; ii <= NbK2; ii++, jj++)
  {
    Noeuds(jj) = Ratio2 * SecondCurve->Knot(ii) - Delta2;
    Mults(jj)  = SecondCurve->Multiplicity(ii);
  }
  Ratio = FirstCurve->Weight(NbP1);
  Ratio /= SecondCurve->Weight(1);
  // Les Poles et Poids
  for (ii = 1; ii < NbP1; ii++)
  {
    Poles(ii) = FirstCurve->Pole(ii);
    Poids(ii) = FirstCurve->Weight(ii);
  }
  for (ii = 1, jj = NbP1; ii <= NbP2; ii++, jj++)
  {
    Poles(jj) = SecondCurve->Pole(ii);
    //
    // attentiion les poids ne se raccord pas forcement C0
    // d'ou Ratio
    //
    Poids(jj) = Ratio * SecondCurve->Weight(ii);
  }

  // Creation de la BSpline
  myCurve = new (Geom2d_BSplineCurve)(Poles, Poids, Noeuds, Mults, Deg);

  // Reduction eventuelle de la multiplicite
  bool Ok = true;
  int M  = Mults(NbK1);
  while ((M > 0) && Ok)
  {
    M--;
    Ok = myCurve->RemoveKnot(NbK1, M, myTol);
  }
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Geom2dConvert_CompCurveToBSplineCurve::BSplineCurve() const
{
  return myCurve;
}

//=================================================================================================

void Geom2dConvert_CompCurveToBSplineCurve::Clear()
{
  myCurve.Nullify();
}
