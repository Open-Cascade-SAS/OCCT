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

#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomFill_BSplineCurves.hxx>
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
#include <Standard_Integer.hxx>

//=======================================================================
// function : Arrange
// purpose  : Internal Use Only
//            This function is used to prepare the Filling: The Curves
//            are arranged in this way:
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
//
//            In case a curve CCx is degenerated to start and end at
//            the same point, it is inserted before the curvature leaves
//            the point.
//=======================================================================
static bool Arrange(const occ::handle<Geom_BSplineCurve>& C1,
                    const occ::handle<Geom_BSplineCurve>& C2,
                    const occ::handle<Geom_BSplineCurve>& C3,
                    const occ::handle<Geom_BSplineCurve>& C4,
                    occ::handle<Geom_BSplineCurve>&       CC1,
                    occ::handle<Geom_BSplineCurve>&       CC2,
                    occ::handle<Geom_BSplineCurve>&       CC3,
                    occ::handle<Geom_BSplineCurve>&       CC4,
                    const double                          Tol)
{
  occ::handle<Geom_BSplineCurve> GC[4];
  occ::handle<Geom_BSplineCurve> Dummy;
  GC[0] = occ::down_cast<Geom_BSplineCurve>(C1->Copy());
  GC[1] = occ::down_cast<Geom_BSplineCurve>(C2->Copy());
  GC[2] = occ::down_cast<Geom_BSplineCurve>(C3->Copy());
  GC[3] = occ::down_cast<Geom_BSplineCurve>(C4->Copy());

  int  i, j;
  bool Trouve;

  for (i = 1; i <= 3; i++)
  {
    Trouve = false;

    // search for a degenerated curve = point, which would match first
    for (j = i; j <= 3 && !Trouve; j++)
    {
      if (GC[j]->StartPoint().Distance(GC[j]->EndPoint()) < Tol)
      {
        // this is a degenerated line, does it match the last endpoint?
        if (GC[j]->StartPoint().Distance(GC[i - 1]->EndPoint()) < Tol)
        {
          Dummy  = GC[i];
          GC[i]  = GC[j];
          GC[j]  = Dummy;
          Trouve = true;
        }
      }
    }

    // if no degenerated curve matched, try an ordinary one as next curve
    if (!Trouve)
    {
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
          GC[j]  = occ::down_cast<Geom_BSplineCurve>(GC[j]->Reversed());
          Dummy  = GC[i];
          GC[i]  = GC[j];
          GC[j]  = Dummy;
          Trouve = true;
        }
      }
    }

    // if still non matched -> error, the algorithm cannot finish
    if (!Trouve)
    {
      return false;
    }
  }

  CC1 = GC[0];
  CC2 = GC[1];
  CC3 = occ::down_cast<Geom_BSplineCurve>(GC[2]->Reversed());
  CC4 = occ::down_cast<Geom_BSplineCurve>(GC[3]->Reversed());

  return true;
}

//=================================================================================================

static int SetSameDistribution(occ::handle<Geom_BSplineCurve>& C1,
                               occ::handle<Geom_BSplineCurve>& C2)
{
  NCollection_Array1<gp_Pnt> P1(C1->Poles());
  NCollection_Array1<double> W1(1, C1->NbPoles());
  W1.Init(1.);
  if (const NCollection_Array1<double>* pW1 = C1->Weights())
    W1 = *pW1;
  NCollection_Array1<double> K1(C1->Knots());
  NCollection_Array1<int>    M1(C1->Multiplicities());

  NCollection_Array1<gp_Pnt> P2(C2->Poles());
  NCollection_Array1<double> W2(1, C2->NbPoles());
  W2.Init(1.);
  if (const NCollection_Array1<double>* pW2 = C2->Weights())
    W2 = *pW2;
  NCollection_Array1<double> K2(C2->Knots());
  NCollection_Array1<int>    M2(C2->Multiplicities());

  double K11 = K1(1);
  double K12 = K1(K1.Upper());
  double K21 = K2(1);
  double K22 = K2(K2.Upper());

  if ((K12 - K11) > (K22 - K21))
  {
    BSplCLib::Reparametrize(K11, K12, K2);
    C2->SetKnots(K2);
  }
  else if ((K12 - K11) < (K22 - K21))
  {
    BSplCLib::Reparametrize(K21, K22, K1);
    C1->SetKnots(K1);
  }
  else if (std::abs(K12 - K11) > Precision::PConfusion())
  {
    BSplCLib::Reparametrize(K11, K12, K2);
    C2->SetKnots(K2);
  }

  int NP, NK;
  if (BSplCLib::PrepareInsertKnots(C1->Degree(),
                                   false,
                                   K1,
                                   M1,
                                   K2,
                                   &M2,
                                   NP,
                                   NK,
                                   Precision::PConfusion(),
                                   false))
  {
    NCollection_Array1<gp_Pnt> NewP(1, NP);
    NCollection_Array1<double> NewW(1, NP);
    NCollection_Array1<double> NewK(1, NK);
    NCollection_Array1<int>    NewM(1, NK);
    BSplCLib::InsertKnots(C1->Degree(),
                          false,
                          P1,
                          &W1,
                          K1,
                          M1,
                          K2,
                          &M2,
                          NewP,
                          &NewW,
                          NewK,
                          NewM,
                          Precision::PConfusion(),
                          false);
    if (C1->IsRational())
    {
      C1 = new Geom_BSplineCurve(NewP, NewW, NewK, NewM, C1->Degree());
    }
    else
    {
      C1 = new Geom_BSplineCurve(NewP, NewK, NewM, C1->Degree());
    }
    BSplCLib::InsertKnots(C2->Degree(),
                          false,
                          P2,
                          &W2,
                          K2,
                          M2,
                          K1,
                          &M1,
                          NewP,
                          &NewW,
                          NewK,
                          NewM,
                          Precision::PConfusion(),
                          false);
    if (C2->IsRational())
    {
      C2 = new Geom_BSplineCurve(NewP, NewW, NewK, NewM, C2->Degree());
    }
    else
    {
      C2 = new Geom_BSplineCurve(NewP, NewK, NewM, C2->Degree());
    }
  }
  else
  {
    throw Standard_ConstructionError(" ");
  }

  return C1->NbPoles();
}

//=================================================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves() = default;

//=================================================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves(const occ::handle<Geom_BSplineCurve>& C1,
                                               const occ::handle<Geom_BSplineCurve>& C2,
                                               const occ::handle<Geom_BSplineCurve>& C3,
                                               const occ::handle<Geom_BSplineCurve>& C4,
                                               const GeomFill_FillingStyle           Type)
{
  Init(C1, C2, C3, C4, Type);
}

//=================================================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves(const occ::handle<Geom_BSplineCurve>& C1,
                                               const occ::handle<Geom_BSplineCurve>& C2,
                                               const occ::handle<Geom_BSplineCurve>& C3,
                                               const GeomFill_FillingStyle           Type)
{
  Init(C1, C2, C3, Type);
}

//=================================================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves(const occ::handle<Geom_BSplineCurve>& C1,
                                               const occ::handle<Geom_BSplineCurve>& C2,
                                               const GeomFill_FillingStyle           Type)
{
  Init(C1, C2, Type);
}

//=================================================================================================

void GeomFill_BSplineCurves::Init(const occ::handle<Geom_BSplineCurve>& C1,
                                  const occ::handle<Geom_BSplineCurve>& C2,
                                  const occ::handle<Geom_BSplineCurve>& C3,
                                  const occ::handle<Geom_BSplineCurve>& C4,
                                  const GeomFill_FillingStyle           Type)
{
  // On ordonne les courbes
  occ::handle<Geom_BSplineCurve> CC1, CC2, CC3, CC4;

  constexpr double Tol = Precision::Confusion();
#ifndef No_Exception
  bool IsOK =
#endif
    Arrange(C1, C2, C3, C4, CC1, CC2, CC3, CC4, Tol);

  Standard_ConstructionError_Raise_if(!IsOK, " GeomFill_BSplineCurves: Courbes non jointives");

  // Mise en conformite des degres
  int Deg1 = CC1->Degree();
  int Deg2 = CC2->Degree();
  int Deg3 = CC3->Degree();
  int Deg4 = CC4->Degree();
  int DegU = std::max(Deg1, Deg3);
  int DegV = std::max(Deg2, Deg4);
  if (Deg1 < DegU)
    CC1->IncreaseDegree(DegU);
  if (Deg2 < DegV)
    CC2->IncreaseDegree(DegV);
  if (Deg3 < DegU)
    CC3->IncreaseDegree(DegU);
  if (Deg4 < DegV)
    CC4->IncreaseDegree(DegV);

  // Mise en conformite des distributions de noeuds
  int NbUPoles = SetSameDistribution(CC1, CC3);
  int NbVPoles = SetSameDistribution(CC2, CC4);

  if (Type == GeomFill_CoonsStyle)
  {
    if (NbUPoles < 4 || NbVPoles < 4)
      throw Standard_ConstructionError("GeomFill_BSplineCurves: invalid filling style");
  }

  const NCollection_Array1<gp_Pnt>& P1 = CC1->Poles();
  const NCollection_Array1<gp_Pnt>& P2 = CC2->Poles();
  const NCollection_Array1<gp_Pnt>& P3 = CC3->Poles();
  const NCollection_Array1<gp_Pnt>& P4 = CC4->Poles();

  // Traitement des courbes rationelles
  bool isRat = (CC1->IsRational() || CC2->IsRational() || CC3->IsRational() || CC4->IsRational());

  NCollection_Array1<double> W1(1, NbUPoles);
  NCollection_Array1<double> W3(1, NbUPoles);
  NCollection_Array1<double> W2(1, NbVPoles);
  NCollection_Array1<double> W4(1, NbVPoles);
  W1.Init(1.);
  W2.Init(1.);
  W3.Init(1.);
  W4.Init(1.);
  if (isRat)
  {
    if (const NCollection_Array1<double>* pW1 = CC1->Weights())
      W1 = *pW1;
    if (const NCollection_Array1<double>* pW2 = CC2->Weights())
      W2 = *pW2;
    if (const NCollection_Array1<double>* pW3 = CC3->Weights())
      W3 = *pW3;
    if (const NCollection_Array1<double>* pW4 = CC4->Weights())
      W4 = *pW4;
  }

  GeomFill_Filling Caro;
  if (isRat)
  {
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

  NbUPoles = Caro.NbUPoles();
  NbVPoles = Caro.NbVPoles();
  NCollection_Array2<gp_Pnt> Poles(1, NbUPoles, 1, NbVPoles);

  // Creation de la surface
  const NCollection_Array1<double>& UKnots = CC1->Knots();
  const NCollection_Array1<int>&    UMults = CC1->Multiplicities();

  const NCollection_Array1<double>& VKnots = CC2->Knots();
  const NCollection_Array1<int>&    VMults = CC2->Multiplicities();

  Caro.Poles(Poles);

  if (Caro.isRational())
  {
    NCollection_Array2<double> Weights(1, NbUPoles, 1, NbVPoles);
    Caro.Weights(Weights);
    mySurface = new Geom_BSplineSurface(Poles,
                                        Weights,
                                        UKnots,
                                        VKnots,
                                        UMults,
                                        VMults,
                                        CC1->Degree(),
                                        CC2->Degree());
  }
  else
  {
    mySurface =
      new Geom_BSplineSurface(Poles, UKnots, VKnots, UMults, VMults, CC1->Degree(), CC2->Degree());
  }
}

//=================================================================================================

void GeomFill_BSplineCurves::Init(const occ::handle<Geom_BSplineCurve>& C1,
                                  const occ::handle<Geom_BSplineCurve>& C2,
                                  const occ::handle<Geom_BSplineCurve>& C3,
                                  const GeomFill_FillingStyle           Type)
{
  occ::handle<Geom_BSplineCurve> C4;
  NCollection_Array1<gp_Pnt>     Poles(1, 2);
  NCollection_Array1<double>     Knots(1, 2);
  NCollection_Array1<int>        Mults(1, 2);
  double                         Tol = Precision::Confusion();
  Tol                                = Tol * Tol;
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

  Knots(1) = C2->Knot(C2->FirstUKnotIndex());
  Knots(2) = C2->Knot(C2->LastUKnotIndex());
  Mults(1) = Mults(2) = 2;
  C4                  = new Geom_BSplineCurve(Poles, Knots, Mults, 1);
  Init(C1, C2, C3, C4, Type);
}

//=================================================================================================

void GeomFill_BSplineCurves::Init(const occ::handle<Geom_BSplineCurve>& C1,
                                  const occ::handle<Geom_BSplineCurve>& C2,
                                  const GeomFill_FillingStyle           Type)
{
  occ::handle<Geom_BSplineCurve> CC1 = occ::down_cast<Geom_BSplineCurve>(C1->Copy());
  occ::handle<Geom_BSplineCurve> CC2 = occ::down_cast<Geom_BSplineCurve>(C2->Copy());

  int Deg1 = CC1->Degree();
  int Deg2 = CC2->Degree();

  bool isRat = (CC1->IsRational() || CC2->IsRational());

  if (Type != GeomFill_CurvedStyle)
  {
    int DegU = std::max(Deg1, Deg2);

    if (CC1->Degree() < DegU)
      CC1->IncreaseDegree(DegU);
    if (CC2->Degree() < DegU)
      CC2->IncreaseDegree(DegU);

    // Mise en conformite des distributions de noeuds
    int                        NbPoles = SetSameDistribution(CC1, CC2);
    NCollection_Array2<gp_Pnt>        Poles(1, NbPoles, 1, 2);
    const NCollection_Array1<gp_Pnt>& P1 = CC1->Poles();
    const NCollection_Array1<gp_Pnt>& P2 = CC2->Poles();
    int i;
    for (i = 1; i <= NbPoles; i++)
    {
      Poles(i, 1) = P1(i);
      Poles(i, 2) = P2(i);
    }
    const NCollection_Array1<double>& UKnots = CC1->Knots();
    const NCollection_Array1<int>&    UMults = CC1->Multiplicities();
    //    int NbVKnots = 2;
    NCollection_Array1<double> VKnots(1, 2);
    NCollection_Array1<int>    VMults(1, 2);
    VKnots(1) = 0;
    VKnots(2) = 1;
    VMults(1) = 2;
    VMults(2) = 2;

    // Traitement des courbes rationelles
    if (isRat)
    {
      NCollection_Array2<double> Weights(1, NbPoles, 1, 2);
      NCollection_Array1<double> W1(1, NbPoles);
      NCollection_Array1<double> W2(1, NbPoles);
      W1.Init(1.);
      W2.Init(1.);

      if (const NCollection_Array1<double>* pW1 = CC1->Weights())
        W1 = *pW1;
      if (const NCollection_Array1<double>* pW2 = CC2->Weights())
        W2 = *pW2;
      for (i = 1; i <= NbPoles; i++)
      {
        Weights(i, 1) = W1(i);
        Weights(i, 2) = W2(i);
      }
      mySurface = new Geom_BSplineSurface(Poles,
                                          Weights,
                                          UKnots,
                                          VKnots,
                                          UMults,
                                          VMults,
                                          CC1->Degree(),
                                          1,
                                          CC1->IsPeriodic(),
                                          false);
    }
    else
    {
      mySurface = new Geom_BSplineSurface(Poles, UKnots, VKnots, UMults, VMults, CC1->Degree(), 1);
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
      throw Standard_OutOfRange("GeomFill_BSplineCurves: Courbes non jointives");

    int                               NbUPoles = CC1->NbPoles();
    int                               NbVPoles = CC2->NbPoles();
    const NCollection_Array1<gp_Pnt>& P1       = CC1->Poles();
    const NCollection_Array1<gp_Pnt>& P2       = CC2->Poles();

    const NCollection_Array1<double>& UKnots = CC1->Knots();
    const NCollection_Array1<int>&    UMults = CC1->Multiplicities();
    const NCollection_Array1<double>& VKnots = CC2->Knots();
    const NCollection_Array1<int>&    VMults = CC2->Multiplicities();

    NCollection_Array1<double> W1(1, NbUPoles);
    NCollection_Array1<double> W2(1, NbVPoles);
    W1.Init(1.);
    W2.Init(1.);

    GeomFill_Filling Caro;
    if (isRat)
    {
      if (const NCollection_Array1<double>* pW1 = CC1->Weights())
        W1 = *pW1;
      if (const NCollection_Array1<double>* pW2 = CC2->Weights())
        W2 = *pW2;
      Caro = GeomFill_Curved(P1, P2, W1, W2);
    }
    else
    {
      Caro = GeomFill_Curved(P1, P2);
    }

    NbUPoles = Caro.NbUPoles();
    NbVPoles = Caro.NbVPoles();
    NCollection_Array2<gp_Pnt> Poles(1, NbUPoles, 1, NbVPoles);

    Caro.Poles(Poles);

    if (Caro.isRational())
    {
      NCollection_Array2<double> Weights(1, NbUPoles, 1, NbVPoles);
      Caro.Weights(Weights);
      mySurface = new Geom_BSplineSurface(Poles,
                                          Weights,
                                          UKnots,
                                          VKnots,
                                          UMults,
                                          VMults,
                                          Deg1,
                                          Deg2,
                                          false,
                                          false);
    }
    else
    {
      mySurface =
        new Geom_BSplineSurface(Poles, UKnots, VKnots, UMults, VMults, Deg1, Deg2, false, false);
    }
  }
}
