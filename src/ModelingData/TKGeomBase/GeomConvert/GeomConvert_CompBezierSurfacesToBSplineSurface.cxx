// Created on: 1996-06-07
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <GeomConvert_CompBezierSurfacesToBSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

// ============================================================================
GeomConvert_CompBezierSurfacesToBSplineSurface::GeomConvert_CompBezierSurfacesToBSplineSurface(
  const NCollection_Array2<occ::handle<Geom_BezierSurface>>& Beziers)
// ============================================================================
{
  int ii;
  myDone = true;
  // Choix des noeuds
  myUKnots = new (NCollection_HArray1<double>)(1, Beziers.ColLength() + 1);
  for (ii = 0; ii < myUKnots->Length(); ii++)
  {
    myUKnots->SetValue(ii + 1, ii);
  }

  myVKnots = new (NCollection_HArray1<double>)(1, Beziers.RowLength() + 1);
  for (ii = 0; ii < myVKnots->Length(); ii++)
  {
    myVKnots->SetValue(ii + 1, ii);
  }

  // Calcul des Poles
  Perform(Beziers);
}

// ============================================================================
GeomConvert_CompBezierSurfacesToBSplineSurface::GeomConvert_CompBezierSurfacesToBSplineSurface(
  const NCollection_Array2<occ::handle<Geom_BezierSurface>>& Beziers,
  const double                                               Tolerance,
  const bool                                                 RemoveKnots)
// ============================================================================
{
  int                     ii, jj, multU = 0, multV, minus;
  bool                    Ok;
  gp_Vec                  vec;
  double                  V1, V2, V3, Ratio, L1, L2, Tol, val;
  gp_Pnt                  P1, P2, P3;
  occ::handle<Geom_Curve> FirstCurve, SecondCurve;

  myDone = true;

  // Choix des noeuds

  myUKnots = new (NCollection_HArray1<double>)(1, Beziers.ColLength() + 1);
  myVKnots = new (NCollection_HArray1<double>)(1, Beziers.RowLength() + 1);

  // --> en U
  myUKnots->SetValue(1, 0);
  jj         = myVKnots->Length() / 2;
  FirstCurve = Beziers(1, jj)->VIso(0.3);
  FirstCurve->D0(0, P1);
  FirstCurve->D0(0.5, P2);
  FirstCurve->D1(1, P3, vec);

  L1 = P1.Distance(P2) + P2.Distance(P3);
  myUKnots->SetValue(2, L1);

  V1 = vec.Magnitude();
  // if the parameterization is too strange we keep the pseudo-length
  if ((V1 > 1000 * L1) || (V1 < L1 * 1.e-3))
    V1 = L1;

  for (ii = 2; ii < myUKnots->Length(); ii++)
  {

    SecondCurve = Beziers(ii, jj)->VIso(0.3);
    SecondCurve->D1(0, P1, vec);
    V2 = vec.Magnitude();
    SecondCurve->D0(0.5, P2);
    SecondCurve->D1(1, P3, vec);
    V3 = vec.Magnitude();
    L2 = P1.Distance(P2) + P2.Distance(P3);

    // On calcul le ratio, en evitant les cas tordus...
    if ((V2 > 1000 * L2) || (V2 < L2 * 1.e-3))
      V2 = L2;
    if ((V3 > 1000 * L2) || (V3 < L2 * 1.e-3))
      V3 = L2;

    Ratio = 1;
    if ((V1 > Precision::Confusion()) && (V2 > Precision::Confusion()))
    {
      Ratio = V2 / V1;
    }
    if ((Ratio < Precision::Confusion()) || (Ratio > 1 / Precision::Confusion()))
    {
      Ratio = 1;
    }

    // On en deduit un  nouveau noeud
    val = myUKnots->Value(ii);
    myUKnots->SetValue(ii + 1, val + Ratio * (val - myUKnots->Value(ii - 1)));

    // Et c'est reparti, pour un tour
    L1         = L2;
    V1         = V3;
    FirstCurve = SecondCurve;
  }

  // --> en V
  myVKnots->SetValue(1, 0);
  ii         = myUKnots->Length() / 2;
  FirstCurve = Beziers(ii, 1)->UIso(0.3);
  FirstCurve->D0(0, P1);
  FirstCurve->D0(0.5, P2);
  FirstCurve->D1(1, P3, vec);

  L1 = P1.Distance(P2) + P2.Distance(P3);
  myVKnots->SetValue(2, L1);

  V1 = vec.Magnitude();
  // if the parameterization is too strange we keep the pseudo-length
  if ((V1 > 1000 * L1) || (V1 < L1 * 1.e-3))
    V1 = L1;

  for (jj = 2; jj < myVKnots->Length(); jj++)
  {

    SecondCurve = Beziers(ii, jj)->UIso(0.3);
    SecondCurve->D1(0, P1, vec);
    V2 = vec.Magnitude();
    SecondCurve->D0(0.5, P2);
    SecondCurve->D1(1, P3, vec);
    V3 = vec.Magnitude();
    L2 = P1.Distance(P2) + P2.Distance(P3);

    // On calcul le ratio, en evitant les cas tordus...
    if ((V2 > 1000 * L2) || (V2 < L2 * 1.e-3))
      V2 = L2;
    if ((V3 > 1000 * L2) || (V3 < L2 * 1.e-3))
      V3 = L2;

    Ratio = 1;
    if ((V1 > Precision::Confusion()) && (V2 > Precision::Confusion()))
    {
      Ratio = V2 / V1;
    }
    if ((Ratio < Precision::Confusion()) || (Ratio > 1 / Precision::Confusion()))
    {
      Ratio = 1;
    }

    // On en deduit un  nouveau noeud
    val = myVKnots->Value(jj);
    myVKnots->SetValue(jj + 1, val + Ratio * (val - myVKnots->Value(jj - 1)));

    // Et c'est reparti, pour un tour
    L1         = L2;
    V1         = V3;
    FirstCurve = SecondCurve;
  }

  // Calcul des Poles
  Perform(Beziers);

  // Reduction de la multiplicite
  occ::handle<Geom_BSplineSurface> Surface = new (Geom_BSplineSurface)(myPoles->Array2(),
                                                                       myUKnots->Array1(),
                                                                       myVKnots->Array1(),
                                                                       myUMults->Array1(),
                                                                       myVMults->Array1(),
                                                                       myUDegree,
                                                                       myVDegree);

  if (RemoveKnots)
    minus = 0;
  else
    minus = 1;

  for (ii = myUKnots->Length() - 1; ii > 1; ii--)
  {
    Ok    = true;
    Tol   = Tolerance / 2;
    multU = myUMults->Value(ii) - 1;
    for (; Ok && multU > minus; multU--, Tol /= 2)
    {
      Ok = Surface->RemoveUKnot(ii, multU, Tol);
    }
  }

  for (ii = myVKnots->Length() - 1; ii > 1; ii--)
  {
    Ok    = true;
    Tol   = Tolerance / 2;
    multV = myVMults->Value(ii) - 1;
    for (; Ok && multU > minus; multV--, Tol /= 2)
    {
      Ok = Surface->RemoveVKnot(ii, multV, Tol);
    }
  }

  // Les nouveaux champs sont arrivees ....
  myPoles  = new NCollection_HArray2<gp_Pnt>(Surface->Poles());
  myUMults = new NCollection_HArray1<int>(Surface->UMultiplicities());
  myVMults = new NCollection_HArray1<int>(Surface->VMultiplicities());
  myUKnots = new NCollection_HArray1<double>(Surface->UKnots());
  myVKnots = new NCollection_HArray1<double>(Surface->VKnots());
}

// ============================================================================
GeomConvert_CompBezierSurfacesToBSplineSurface::GeomConvert_CompBezierSurfacesToBSplineSurface(
  const NCollection_Array2<occ::handle<Geom_BezierSurface>>& Beziers,
  const NCollection_Array1<double>&                          UKnots,
  const NCollection_Array1<double>&                          VKnots,
  const GeomAbs_Shape                                        UContinuity,
  const GeomAbs_Shape                                        VContinuity,
  const double                                               Tolerance)
// ============================================================================
{
  int  decu = 0, decv = 0;
  bool Ok;

  myDone = true;

  // Recuperation des noeuds
  myUKnots                 = new (NCollection_HArray1<double>)(1, Beziers.ColLength() + 1);
  myUKnots->ChangeArray1() = UKnots;

  myVKnots                 = new (NCollection_HArray1<double>)(1, Beziers.RowLength() + 1);
  myVKnots->ChangeArray1() = VKnots;

  // Calcul des Poles
  Perform(Beziers);

  // Obtention des bonne continuitee

  switch (UContinuity)
  {
    case GeomAbs_C0:
      decu = 0;
      break;
    case GeomAbs_C1:
      decu = 1;
      break;
    case GeomAbs_C2:
      decu = 2;
      break;
    case GeomAbs_C3:
      decu = 3;
      break;
    default:
      throw Standard_ConstructionError("GeomConvert_CompBezierSurfacesToBSpl:: UContinuity error");
  }

  switch (VContinuity)
  {
    case GeomAbs_C0:
      decv = 0;
      break;
    case GeomAbs_C1:
      decv = 1;
      break;
    case GeomAbs_C2:
      decv = 2;
      break;
    case GeomAbs_C3:
      decv = 3;
      break;
    default:
      throw Standard_ConstructionError("GeomConvert_CompBezierSurfacesToBSpl:: VContinuity error");
  }

  if ((decu > 0) || (decv > 0))
  {

    int ii;
    int multU = myUDegree - decu;
    Standard_ConstructionError_Raise_if(
      ((multU <= 0) && (myUKnots->Length() > 2)),
      "GeomConvert_CompBezierSurfacesToBSpl:: UContinuity or Udeg error");

    int multV = myVDegree - decv;
    Standard_ConstructionError_Raise_if(
      ((multV <= 0) && (myVKnots->Length() > 2)),
      "GeomConvert_CompBezierSurfacesToBSpl:: VContinuity or Vdeg error");

    occ::handle<Geom_BSplineSurface> Surface = new (Geom_BSplineSurface)(myPoles->Array2(),
                                                                         myUKnots->Array1(),
                                                                         myVKnots->Array1(),
                                                                         myUMults->Array1(),
                                                                         myVMults->Array1(),
                                                                         myUDegree,
                                                                         myVDegree);

    if (decu > 0)
    {
      for (ii = 2; ii < myUKnots->Length(); ii++)
      {
        Ok = Surface->RemoveUKnot(ii, multU, Tolerance);
        if (!Ok)
        {
          myDone = Ok;
        }
      }
    }

    if (decv > 0)
    {
      for (ii = 2; ii < myVKnots->Length(); ii++)
      {
        Ok = Surface->RemoveVKnot(ii, multV, Tolerance);
        if (!Ok)
        {
          myDone = Ok;
        }
      }
    }

    // Les nouveaux champs sont arrivees ....
    myPoles  = new NCollection_HArray2<gp_Pnt>(Surface->Poles());
    myUMults = new NCollection_HArray1<int>(Surface->UMultiplicities());
    myVMults = new NCollection_HArray1<int>(Surface->VMultiplicities());
  }
}

// ================================================================================
void GeomConvert_CompBezierSurfacesToBSplineSurface::Perform(
  const NCollection_Array2<occ::handle<Geom_BezierSurface>>& Beziers)
// ================================================================================
{
  int IU, IV;

  // (1) Determination des degrees et si le resultat est rationnel.
  isrational = false;
  myUDegree  = 1;
  myVDegree  = 1;

  for (IU = Beziers.LowerRow(); IU <= Beziers.UpperRow(); IU++)
  {
    for (IV = Beziers.LowerCol(); IV <= Beziers.UpperCol(); IV++)
    {
      if (Beziers(IU, IV)->IsURational() || Beziers(IU, IV)->IsVRational())
      {
        isrational = true;
      }

      myUDegree = (Beziers(IU, IV)->UDegree() > myUDegree) ? Beziers(IU, IV)->UDegree() : myUDegree;

      myVDegree = (Beziers(IU, IV)->VDegree() > myVDegree) ? Beziers(IU, IV)->VDegree() : myVDegree;
    }
  }

  Standard_NotImplemented_Raise_if(isrational, "CompBezierSurfacesToBSpl : rational !");

  // (2) Boucle sur les carreaux  -----------------------------

  occ::handle<Geom_BezierSurface> Patch;
  int                             UIndex, VIndex, uindex, vindex, udeb, vdeb;
  int                             upol, vpol, ii;

  myPoles = new (NCollection_HArray2<gp_Pnt>)(
    1,
    (myUDegree + 1) * Beziers.ColLength() - myUKnots->Length() + 2,
    1,
    (myVDegree + 1) * Beziers.RowLength() - myVKnots->Length() + 2);

  for (IU = Beziers.LowerRow(); IU <= Beziers.UpperRow(); IU++)
  {
    UIndex = (IU - 1) * myUDegree + 1;
    for (IV = Beziers.LowerCol(); IV <= Beziers.UpperCol(); IV++)
    {

      Patch  = Beziers(IU, IV);
      VIndex = (IV - 1) * myVDegree + 1;

      // (2.1) Augmentation du degree
      Patch->Increase(myUDegree, myVDegree);

      // (2.2) Poles a recopier
      if (IU == 1)
      {
        udeb = 1;
      }
      else
      {
        udeb = 2;
      }
      if (IV == 1)
      {
        vdeb = 1;
      }
      else
      {
        vdeb = 2;
      }

      uindex = UIndex + udeb - 1;

      for (upol = udeb; upol <= myUDegree + 1; upol++, uindex++)
      {
        vindex = VIndex + vdeb - 1;
        for (vpol = vdeb; vpol <= myVDegree + 1; vpol++, vindex++)
        {
          myPoles->ChangeValue(uindex, vindex) = Patch->Pole(upol, vpol);
        }
      }

      // (2.3) Poles a sommer
      if (udeb == 2)
      {
        vindex = VIndex + vdeb - 1;
        for (vpol = vdeb; vpol <= myVDegree + 1; vpol++, vindex++)
        {
          myPoles->ChangeValue(UIndex, vindex).ChangeCoord() += Patch->Pole(1, vpol).Coord();
        }
      }

      if (vdeb == 2)
      {
        uindex = UIndex + udeb - 1;
        for (upol = udeb; upol <= myUDegree + 1; upol++, uindex++)
        {
          myPoles->ChangeValue(uindex, VIndex).ChangeCoord() += Patch->Pole(upol, 1).Coord();
        }
      }

      if (udeb == 2 && vdeb == 2)
      {
        myPoles->ChangeValue(UIndex, VIndex).ChangeCoord() += Patch->Pole(1, 1).Coord();
      }
    }
  }

  // (3) Elimination of redundancies
  // because in the previous loop we count:
  // - 2 times the poles associated with simple knots
  // - 4 times the poles associated with double knots (in U and V)

  // (3.1) Elimination in U
  for (UIndex = myUDegree + 1, ii = 2; ii < myUKnots->Length(); ii++, UIndex += myUDegree)
  {
    for (vpol = 1; vpol <= myPoles->UpperCol(); vpol++)
    {
      myPoles->ChangeValue(UIndex, vpol).ChangeCoord() *= 0.5;
    }
  }

  // (3.2) Elimination en V
  for (VIndex = myVDegree + 1, ii = 2; ii < myVKnots->Length(); ii++, VIndex += myVDegree)
  {
    for (upol = 1; upol <= myPoles->UpperRow(); upol++)
    {
      myPoles->ChangeValue(upol, VIndex).ChangeCoord() *= 0.5;
    }
  }

  // (4) Init des multiplicites
  myUMults = new (NCollection_HArray1<int>)(1, myUKnots->Length());
  myUMults->Init(myUDegree);
  myUMults->SetValue(1, myUDegree + 1);
  myUMults->SetValue(myUMults->Upper(), myUDegree + 1);

  myVMults = new (NCollection_HArray1<int>)(1, myVKnots->Length());
  myVMults->Init(myVDegree);
  myVMults->SetValue(1, myVDegree + 1);
  myVMults->SetValue(myVMults->Upper(), myVDegree + 1);
}

// ========================================================================
bool GeomConvert_CompBezierSurfacesToBSplineSurface::IsDone() const
// ========================================================================
{
  return myDone;
}
