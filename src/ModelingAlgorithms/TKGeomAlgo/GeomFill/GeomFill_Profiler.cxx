// Created on: 1994-02-17
// Created by: Bruno DUMORTIER
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

#include <BSplCLib.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomFill_Profiler.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

static void UnifyByInsertingAllKnots(NCollection_Sequence<occ::handle<Geom_Curve>>& theCurves,
                                     const double                                   PTol)
{
  // inserting in the first curve the knot-vector of all the others.
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(theCurves(1));

  int i;
  for (i = 2; i <= theCurves.Length(); i++)
  {
    occ::handle<Geom_BSplineCurve> Ci = occ::down_cast<Geom_BSplineCurve>(theCurves(i));
    const NCollection_Array1<double>& Ki = Ci->Knots();
    const NCollection_Array1<int>&    Mi = Ci->Multiplicities();

    C->InsertKnots(Ki, Mi, PTol, false);
  }

  const NCollection_Array1<double>& NewKnots = C->Knots();
  const NCollection_Array1<int>&    NewMults = C->Multiplicities();
  for (i = 2; i <= theCurves.Length(); i++)
  {
    occ::handle<Geom_BSplineCurve> Ci = occ::down_cast<Geom_BSplineCurve>(theCurves(i));
    Ci->InsertKnots(NewKnots, NewMults, PTol, false);
  }

  // essai : tentative mise des poids sur chaque section a une moyenne 1
  for (i = 1; i <= theCurves.Length(); i++)
  {
    occ::handle<Geom_BSplineCurve> Ci = occ::down_cast<Geom_BSplineCurve>(theCurves(i));
    if (Ci->IsRational())
    {
      int    np    = Ci->NbPoles();
      double sigma = 0.;
      int    j;
      for (j = 1; j <= np; j++)
      {
        sigma += Ci->Weight(j);
      }
      sigma /= np;
      for (j = 1; j <= np; j++)
      {
        Ci->SetWeight(j, Ci->Weight(j) / sigma);
      }
    }
  }
  // fin de l essai
}

//=================================================================================================

static void UnifyBySettingMiddleKnots(NCollection_Sequence<occ::handle<Geom_Curve>>& theCurves)
{
  int i, j;

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(theCurves(1));

  int    NbKnots = C->NbKnots();
  double ULast   = C->Knot(C->LastUKnotIndex());
  double UFirst  = C->Knot(C->FirstUKnotIndex());

  // Set middle values of knots
  NCollection_Array1<double> NewKnots(1, NbKnots);
  NewKnots(1)       = UFirst;
  NewKnots(NbKnots) = ULast;
  for (j = 2; j < NbKnots; j++)
  {
    double aMidKnot = 0.;
    for (i = 1; i <= theCurves.Length(); i++)
    {
      occ::handle<Geom_BSplineCurve> Ctemp = occ::down_cast<Geom_BSplineCurve>(theCurves(i));
      aMidKnot += Ctemp->Knot(j);
    }
    aMidKnot /= theCurves.Length();
    NewKnots(j) = aMidKnot;
  }

  for (i = 1; i <= theCurves.Length(); i++)
  {
    occ::handle<Geom_BSplineCurve> Cres = occ::down_cast<Geom_BSplineCurve>(theCurves(i));
    Cres->SetKnots(NewKnots);
  }
}

//=================================================================================================

GeomFill_Profiler::GeomFill_Profiler()
{
  myIsDone     = false;
  myIsPeriodic = true;
}

//=======================================================================

GeomFill_Profiler::~GeomFill_Profiler() = default;

//=================================================================================================

void GeomFill_Profiler::AddCurve(const occ::handle<Geom_Curve>& Curve)
{
  occ::handle<Geom_Curve> C;
  //// modified by jgv, 19.01.05 for OCC7354 ////
  occ::handle<Geom_Curve> theCurve = Curve;
  if (theCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    theCurve = occ::down_cast<Geom_TrimmedCurve>(theCurve)->BasisCurve();
  if (theCurve->IsKind(STANDARD_TYPE(Geom_Conic)))
  {
    GeomConvert_ApproxCurve appr(Curve, Precision::Confusion(), GeomAbs_C1, 16, 14);
    if (appr.HasResult())
      C = appr.Curve();
  }
  if (C.IsNull())
    C = GeomConvert::CurveToBSplineCurve(Curve);
  /*
  if ( Curve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    C = occ::down_cast<Geom_Curve>(Curve->Copy());
  }
  else {
    C = GeomConvert::CurveToBSplineCurve(Curve,Convert_QuasiAngular);
  }
  */
  ///////////////////////////////////////////////

  mySequence.Append(C);

  if (myIsPeriodic && !C->IsPeriodic())
    myIsPeriodic = false;
}

//=================================================================================================

void GeomFill_Profiler::Perform(const double PTol)
{
  int i;
  //  int myDegree = 0, myNbPoles = 0;
  int                            myDegree = 0;
  occ::handle<Geom_BSplineCurve> C;
  double                         U1, U2, UFirst = 0, ULast = 0;
  double                         EcartMax = 0.;

  for (i = 1; i <= mySequence.Length(); i++)
  {
    C = occ::down_cast<Geom_BSplineCurve>(mySequence(i));

    // si non periodique, il faut deperiodiser toutes les courbes
    // on les segmente ensuite pour assurer K(1) et K(n) de multiplicite
    // degre + 1

    U2 = C->Knot(C->LastUKnotIndex());
    U1 = C->Knot(C->FirstUKnotIndex());

    if (!myIsPeriodic && C->IsPeriodic())
    {
      C->SetNotPeriodic();
      C->Segment(U1, U2);
    }

    // evaluate the max degree
    myDegree = std::max(myDegree, C->Degree());

    // Calcul de Max ( Ufin - Udeb) sur l ensemble des courbes.
    if ((U2 - U1) > EcartMax)
    {
      EcartMax = U2 - U1;
      UFirst   = U1;
      ULast    = U2;
    }
  }

  // increase the degree of the curves to my degree
  // reparametrize them in the range U1, U2.
  for (i = 1; i <= mySequence.Length(); i++)
  {
    C = occ::down_cast<Geom_BSplineCurve>(mySequence(i));

    C->IncreaseDegree(myDegree);

    NCollection_Array1<double> Knots(C->Knots());
    BSplCLib::Reparametrize(UFirst, ULast, Knots);
    C->SetKnots(Knots);
  }

  NCollection_Sequence<occ::handle<Geom_Curve>> theCurves;
  for (i = 1; i <= mySequence.Length(); i++)
    theCurves.Append(occ::down_cast<Geom_Curve>(mySequence(i)->Copy()));

  UnifyByInsertingAllKnots(theCurves, PTol);

  bool Unified    = true;
  int  theNbKnots = (occ::down_cast<Geom_BSplineCurve>(theCurves(1)))->NbKnots();
  for (i = 2; i <= theCurves.Length(); i++)
    if ((occ::down_cast<Geom_BSplineCurve>(theCurves(i)))->NbKnots() != theNbKnots)
    {
      Unified = false;
      break;
    }

  if (Unified)
    mySequence = theCurves;
  else
    UnifyBySettingMiddleKnots(mySequence);

  myIsDone = true;
}

//=================================================================================================

int GeomFill_Profiler::Degree() const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomFill_Profiler::Degree");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(1));
  return C->Degree();
}

//=================================================================================================

int GeomFill_Profiler::NbPoles() const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomFill_Profiler::Degree");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(1));
  return C->NbPoles();
}

//=================================================================================================

void GeomFill_Profiler::Poles(const int Index, NCollection_Array1<gp_Pnt>& Poles) const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomFill_Profiler::Degree");

  Standard_DomainError_Raise_if(Poles.Length() != NbPoles(), "GeomFill_Profiler::Poles");
  Standard_DomainError_Raise_if(Index < 1 || Index > mySequence.Length(),
                                "GeomFill_Profiler::Poles");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(Index));

  Poles = C->Poles();
}

//=================================================================================================

void GeomFill_Profiler::Weights(const int Index, NCollection_Array1<double>& Weights) const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomFill_Profiler::Degree");

  Standard_DomainError_Raise_if(Weights.Length() != NbPoles(), "GeomFill_Profiler::Weights");
  Standard_DomainError_Raise_if(Index < 1 || Index > mySequence.Length(),
                                "GeomFill_Profiler::Weights");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(Index));

  const NCollection_Array1<double>* aWPtr = C->Weights();
  if (aWPtr != nullptr)
    Weights = *aWPtr;
  else
    Weights.Init(1.0);
}

//=================================================================================================

int GeomFill_Profiler::NbKnots() const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomFill_Profiler::Degree");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(1));

  return C->NbKnots();
}

//=================================================================================================

void GeomFill_Profiler::KnotsAndMults(NCollection_Array1<double>& Knots,
                                      NCollection_Array1<int>&    Mults) const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomFill_Profiler::Degree");

#ifndef No_Exception
  int n = NbKnots();
#endif
  Standard_DomainError_Raise_if(Knots.Length() != n || Mults.Length() != n,
                                "GeomFill_Profiler::KnotsAndMults");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(mySequence(1));

  Knots = C->Knots();
  Mults = C->Multiplicities();
}
