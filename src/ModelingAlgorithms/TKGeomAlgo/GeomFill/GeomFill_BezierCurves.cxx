// Created on: 1993-10-06
// Created by: Bruno DUMORTIER
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

#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomFill_BezierCurves.hxx>
#include <GeomFill_Coons.hxx>
#include <GeomFill_Curved.hxx>
#include <GeomFill_Filling.hxx>
#include <GeomFill_Stretch.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//=======================================================================
// function : SetSameWeights
// purpose  : Internal Use Only
//           This function uses the following property of Rational
//           BezierCurves
//              if Wi = Weight(i); Pi = Pole(i); n = NbPoles
//              with any a,b,c != 0,
//                                             i    n-i
//              The transformation : Wi = a * b  * c    doesn't modify
//              the geometry of the curve.
//              Only the length of the derivatives are changed.
//=======================================================================
static void SetSameWeights(NCollection_Array1<double>& W1,
                           NCollection_Array1<double>& W2,
                           NCollection_Array1<double>& W3,
                           NCollection_Array1<double>& W4)
{
  constexpr double Eps = Precision::Confusion();

  int NU = W1.Length();
  int NV = W2.Length();

  double A = (W1(1) * W2(1)) / (W1(NU) * W2(NV));
  double B = (W3(1) * W4(1)) / (W3(NU) * W4(NV));

  int    i;
  double Alfa = W1(NU) / W2(1);
  for (i = 1; i <= NV; i++)
  {
    W2(i) *= Alfa;
  }
  double Beta = W2(NV) / W3(NU);
  for (i = 1; i <= NU; i++)
  {
    W3(i) *= Beta;
  }
  double Gamma = W3(1) / W4(NV);
  for (i = 1; i <= NV; i++)
  {
    W4(i) *= Gamma;
  }

  if (std::abs(A - B) > Eps)
  {
    double w = std::pow(W1(1) / W4(1), 1. / (double)(NV - 1));
    double x = w;
    for (i = NV - 1; i >= 1; i--)
    {
      W4(i) *= x;
      x *= w;
    }
  }
}

//=======================================================================
// function : Arrange
// purpose  : Internal Use Only
//           This function is used to prepare the Filling: The Curves
//           are arranged in this way:
//
//                      CC3
//                  ----->-----
//                 |           |
//                 |           |
//                 |           |
//             CC4 ^           ^ CC2
//                 |           |
//                 |           |
//                  ----->-----
//                   CC1 = C1
//=======================================================================

static bool Arrange(const occ::handle<Geom_BezierCurve>& C1,
                    const occ::handle<Geom_BezierCurve>& C2,
                    const occ::handle<Geom_BezierCurve>& C3,
                    const occ::handle<Geom_BezierCurve>& C4,
                    occ::handle<Geom_BezierCurve>&       CC1,
                    occ::handle<Geom_BezierCurve>&       CC2,
                    occ::handle<Geom_BezierCurve>&       CC3,
                    occ::handle<Geom_BezierCurve>&       CC4,
                    const double                         Tol)
{
  occ::handle<Geom_BezierCurve> GC[4];
  occ::handle<Geom_BezierCurve> Dummy;
  GC[0] = occ::down_cast<Geom_BezierCurve>(C1->Copy());
  GC[1] = occ::down_cast<Geom_BezierCurve>(C2->Copy());
  GC[2] = occ::down_cast<Geom_BezierCurve>(C3->Copy());
  GC[3] = occ::down_cast<Geom_BezierCurve>(C4->Copy());

  int  i, j;
  bool Trouve;

  for (i = 1; i <= 3; i++)
  {
    Trouve = false;
    for (j = i; j <= 3 && !Trouve; j++)
    {
      if (GC[j]->StartPoint().Distance(GC[i - 1]->EndPoint()) < Tol)
      {
        Dummy  = GC[i];
        GC[i]  = GC[j];
        GC[j]  = Dummy;
        Trouve = true;
      }
      else if (GC[j]->EndPoint().Distance(GC[i - 1]->EndPoint()) < Tol)
      {
        GC[j]  = occ::down_cast<Geom_BezierCurve>(GC[j]->Reversed());
        Dummy  = GC[i];
        GC[i]  = GC[j];
        GC[j]  = Dummy;
        Trouve = true;
      }
    }
    if (!Trouve)
      return false;
  }

  CC1 = GC[0];
  CC2 = GC[1];
  CC3 = occ::down_cast<Geom_BezierCurve>(GC[2]->Reversed());
  CC4 = occ::down_cast<Geom_BezierCurve>(GC[3]->Reversed());

  return true;
}

//=================================================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves() = default;

//=================================================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves(const occ::handle<Geom_BezierCurve>& C1,
                                             const occ::handle<Geom_BezierCurve>& C2,
                                             const occ::handle<Geom_BezierCurve>& C3,
                                             const occ::handle<Geom_BezierCurve>& C4,
                                             const GeomFill_FillingStyle          Type)
{
  Init(C1, C2, C3, C4, Type);
}

//=================================================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves(const occ::handle<Geom_BezierCurve>& C1,
                                             const occ::handle<Geom_BezierCurve>& C2,
                                             const occ::handle<Geom_BezierCurve>& C3,
                                             const GeomFill_FillingStyle          Type)
{
  Init(C1, C2, C3, Type);
}

//=================================================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves(const occ::handle<Geom_BezierCurve>& C1,
                                             const occ::handle<Geom_BezierCurve>& C2,
                                             const GeomFill_FillingStyle          Type)
{
  Init(C1, C2, Type);
}

//=================================================================================================

void GeomFill_BezierCurves::Init(const occ::handle<Geom_BezierCurve>& C1,
                                 const occ::handle<Geom_BezierCurve>& C2,
                                 const occ::handle<Geom_BezierCurve>& C3,
                                 const occ::handle<Geom_BezierCurve>& C4,
                                 const GeomFill_FillingStyle          Type)
{
  // On ordonne les courbes
  occ::handle<Geom_BezierCurve> CC1, CC2, CC3, CC4;

  constexpr double Tol = Precision::Confusion();
#ifndef No_Exception
  bool IsOK =
#endif
    Arrange(C1, C2, C3, C4, CC1, CC2, CC3, CC4, Tol);

  Standard_ConstructionError_Raise_if(!IsOK, " GeomFill_BezierCurves: Courbes non jointives");

  // Mise en conformite des degres
  int DegU = std::max(CC1->Degree(), CC3->Degree());
  int DegV = std::max(CC2->Degree(), CC4->Degree());

  if (Type == GeomFill_CoonsStyle)
  {
    DegU = std::max(DegU, 3);
    DegV = std::max(DegV, 3);
  }

  if (CC1->Degree() < DegU)
    CC1->Increase(DegU);
  if (CC2->Degree() < DegV)
    CC2->Increase(DegV);
  if (CC3->Degree() < DegU)
    CC3->Increase(DegU);
  if (CC4->Degree() < DegV)
    CC4->Increase(DegV);

  const NCollection_Array1<gp_Pnt>& P1 = CC1->Poles();
  const NCollection_Array1<gp_Pnt>& P2 = CC2->Poles();
  const NCollection_Array1<gp_Pnt>& P3 = CC3->Poles();
  const NCollection_Array1<gp_Pnt>& P4 = CC4->Poles();

  // Traitement des courbes rationelles
  bool isRat = (CC1->IsRational() || CC2->IsRational() || CC3->IsRational() || CC4->IsRational());

  NCollection_Array1<double> W1(1, DegU + 1);
  NCollection_Array1<double> W3(1, DegU + 1);
  NCollection_Array1<double> W2(1, DegV + 1);
  NCollection_Array1<double> W4(1, DegV + 1);
  W1.Init(1.);
  W2.Init(1.);
  W3.Init(1.);
  W4.Init(1.);

  if (isRat)
  {
    if (CC1->IsRational())
    {
      if (const NCollection_Array1<double>* pW = CC1->Weights())
        W1 = *pW;
    }
    if (CC2->IsRational())
    {
      if (const NCollection_Array1<double>* pW = CC2->Weights())
        W2 = *pW;
    }
    if (CC3->IsRational())
    {
      if (const NCollection_Array1<double>* pW = CC3->Weights())
        W3 = *pW;
    }
    if (CC4->IsRational())
    {
      if (const NCollection_Array1<double>* pW = CC4->Weights())
        W4 = *pW;
    }
  }

  GeomFill_Filling Caro;
  if (isRat)
  {
    // Mise en conformite des poids aux coins.
    SetSameWeights(W1, W2, W3, W4);
    switch (Type)
    {
      case GeomFill_StretchStyle:
        Caro = GeomFill_Stretch(P1, P2, P3, P4, W1, W2, W3, W4);
        break;
      case GeomFill_CoonsStyle:
        Caro = GeomFill_Coons(P1, P4, P3, P2, W1, W4, W3, W2);
        break;
      case GeomFill_CurvedStyle:
        Caro = GeomFill_Curved(P1, P2, P3, P4, W1, W2, W3, W4);
        break;
    }
  }
  else
  {
    switch (Type)
    {
      case GeomFill_StretchStyle:
        Caro = GeomFill_Stretch(P1, P2, P3, P4);
        break;
      case GeomFill_CoonsStyle:
        Caro = GeomFill_Coons(P1, P4, P3, P2);
        break;
      case GeomFill_CurvedStyle:
        Caro = GeomFill_Curved(P1, P2, P3, P4);
        break;
    }
  }

  int                        NbUPoles = Caro.NbUPoles();
  int                        NbVPoles = Caro.NbVPoles();
  NCollection_Array2<gp_Pnt> Poles(1, NbUPoles, 1, NbVPoles);

  Caro.Poles(Poles);

  if (Caro.isRational())
  {
    NCollection_Array2<double> Weights(1, NbUPoles, 1, NbVPoles);
    Caro.Weights(Weights);
    mySurface = new Geom_BezierSurface(Poles, Weights);
  }
  else
  {
    mySurface = new Geom_BezierSurface(Poles);
  }
}

//=================================================================================================

void GeomFill_BezierCurves::Init(const occ::handle<Geom_BezierCurve>& C1,
                                 const occ::handle<Geom_BezierCurve>& C2,
                                 const occ::handle<Geom_BezierCurve>& C3,
                                 const GeomFill_FillingStyle          Type)
{
  occ::handle<Geom_BezierCurve> C4;
  NCollection_Array1<gp_Pnt>    Poles(1, 2);
  double                        Tol = Precision::Confusion();
  Tol                               = Tol * Tol;
  if (C1->StartPoint().SquareDistance(C2->StartPoint()) > Tol
      && C1->StartPoint().SquareDistance(C2->EndPoint()) > Tol)
    Poles(1) = C1->StartPoint();
  else
    Poles(1) = C1->EndPoint();

  if (C3->StartPoint().SquareDistance(C2->StartPoint()) > Tol
      && C3->StartPoint().SquareDistance(C2->EndPoint()) > Tol)
    Poles(2) = C3->StartPoint();
  else
    Poles(2) = C3->EndPoint();
  //  Poles(1) = C1->StartPoint();
  //  Poles(2) = C1->StartPoint();
  C4 = new Geom_BezierCurve(Poles);
  Init(C1, C2, C3, C4, Type);
}

//=================================================================================================

void GeomFill_BezierCurves::Init(const occ::handle<Geom_BezierCurve>& C1,
                                 const occ::handle<Geom_BezierCurve>& C2,
                                 const GeomFill_FillingStyle          Type)
{
  occ::handle<Geom_BezierCurve> CC1 = occ::down_cast<Geom_BezierCurve>(C1->Copy());
  occ::handle<Geom_BezierCurve> CC2 = occ::down_cast<Geom_BezierCurve>(C2->Copy());

  int Deg1 = CC1->Degree();
  int Deg2 = CC2->Degree();

  bool isRat = (CC1->IsRational() || CC2->IsRational());

  if (Type != GeomFill_CurvedStyle)
  {
    int DegU = std::max(Deg1, Deg2);

    if (CC1->Degree() < DegU)
      CC1->Increase(DegU);
    if (CC2->Degree() < DegU)
      CC2->Increase(DegU);

    NCollection_Array2<gp_Pnt>        Poles(1, DegU + 1, 1, 2);
    const NCollection_Array1<gp_Pnt>& P1 = CC1->Poles();
    const NCollection_Array1<gp_Pnt>& P2 = CC2->Poles();

    int i;
    for (i = 1; i <= DegU + 1; i++)
    {
      Poles(i, 1) = P1(i);
      Poles(i, 2) = P2(i);
    }
    if (isRat)
    {
      NCollection_Array1<double> W1(1, DegU + 1);
      NCollection_Array1<double> W2(1, DegU + 1);
      W1.Init(1.);
      W2.Init(1.);

      if (CC1->IsRational())
      {
        if (const NCollection_Array1<double>* pW = CC1->Weights())
          W1 = *pW;
      }
      if (CC2->IsRational())
      {
        if (const NCollection_Array1<double>* pW = CC2->Weights())
          W2 = *pW;
      }
      NCollection_Array2<double> Weights(1, DegU + 1, 1, 2);
      for (i = 1; i <= DegU + 1; i++)
      {
        Weights(i, 1) = W1(i);
        Weights(i, 2) = W2(i);
      }
      mySurface = new Geom_BezierSurface(Poles, Weights);
    }
    else
    {
      mySurface = new Geom_BezierSurface(Poles);
    }
  }
  else
  {
    constexpr double Eps  = Precision::Confusion();
    bool             IsOK = false;
    if (CC1->StartPoint().IsEqual(CC2->StartPoint(), Eps))
    {
      IsOK = true;
    }
    else if (CC1->StartPoint().IsEqual(CC2->EndPoint(), Eps))
    {
      CC2->Reverse();
      IsOK = true;
    }
    else if (CC1->EndPoint().IsEqual(CC2->StartPoint(), Eps))
    {
      C1->Reverse();
      IsOK = true;
    }
    else if (CC1->EndPoint().IsEqual(CC2->EndPoint(), Eps))
    {
      CC1->Reverse();
      CC2->Reverse();
      IsOK = true;
    }

    if (!IsOK)
      throw Standard_OutOfRange("GeomFill_BezierCurves: Courbes non jointives");

    const NCollection_Array1<gp_Pnt>& P1 = CC1->Poles();
    const NCollection_Array1<gp_Pnt>& P2 = CC2->Poles();

    NCollection_Array1<double> W1(1, Deg1 + 1);
    NCollection_Array1<double> W2(1, Deg2 + 1);
    W1.Init(1.);
    W2.Init(1.);

    GeomFill_Filling Caro;
    if (isRat)
    {
      if (CC1->IsRational())
      {
        if (const NCollection_Array1<double>* pW = CC1->Weights())
          W1 = *pW;
      }
      if (CC2->IsRational())
      {
        if (const NCollection_Array1<double>* pW = CC2->Weights())
          W2 = *pW;
      }
      Caro = GeomFill_Curved(P1, P2, W1, W2);
    }
    else
    {
      Caro = GeomFill_Curved(P1, P2);
    }

    int                        NbUPoles = Caro.NbUPoles();
    int                        NbVPoles = Caro.NbVPoles();
    NCollection_Array2<gp_Pnt> Poles(1, NbUPoles, 1, NbVPoles);

    Caro.Poles(Poles);

    if (Caro.isRational())
    {
      NCollection_Array2<double> Weights(1, NbUPoles, 1, NbVPoles);
      Caro.Weights(Weights);
      mySurface = new Geom_BezierSurface(Poles, Weights);
    }
    else
    {
      mySurface = new Geom_BezierSurface(Poles);
    }
  }
}
