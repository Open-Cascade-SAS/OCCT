// Created on: 1998-11-12
// Created by: data exchange team
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

#include <ShapeUpgrade.hxx>

#include <BSplCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

/*
// Debug state= True / False.
static bool Dbg=false;
void ShapeUpgrade::SetDebug(const bool State)
{
  Dbg=State;
}
bool ShapeUpgrade::Debug()
{
  return Dbg;
}
*/
//=================================================================================================

bool ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve(
  const occ::handle<Geom_BSplineCurve>&                               BS,
  occ::handle<NCollection_HSequence<occ::handle<Geom_BoundedCurve>>>& seqBS)
{
  if (BS.IsNull() || (BS->IsCN(1)))
    return false;

  seqBS = new NCollection_HSequence<occ::handle<Geom_BoundedCurve>>;
  BS->SetNotPeriodic(); // to have equation NbPoles = NbKnots with Multiplicities - degree - 1

  int                              deg = BS->Degree();
  const NCollection_Array1<gp_Pnt>& Poles = BS->Poles();
  NCollection_Array1<double>       Weights(1, BS->NbPoles());
  if (BS->IsRational())
    Weights = *BS->Weights();
  else
    Weights.Init(1.);
  const NCollection_Array1<double>& Knots       = BS->Knots();
  const NCollection_Array1<int>&    Mults       = BS->Multiplicities();
  const NCollection_Array1<double>& KnotSequence = BS->KnotSequence();

  int StartKnotIndex, EndKnotIndex, j;

  StartKnotIndex = BS->FirstUKnotIndex();
  for (EndKnotIndex = StartKnotIndex + 1; EndKnotIndex <= BS->LastUKnotIndex(); EndKnotIndex++)
  {
    if ((Mults(EndKnotIndex) < deg) && (EndKnotIndex < BS->LastUKnotIndex()))
      continue;

    int StartFlatIndex = BSplCLib::FlatIndex(deg, StartKnotIndex, Mults, false);
    //    StartFlatIndex += Mults (StartKnotIndex) - 1;
    int EndFlatIndex = BSplCLib::FlatIndex(deg, EndKnotIndex, Mults, false);
    EndFlatIndex -= Mults(EndKnotIndex) - 1;

    NCollection_Array1<double> TempKnots(1, Knots.Length());
    NCollection_Array1<int>    TempMults(1, Knots.Length());
    TempMults.Init(1);
    int TempKnotIndex        = 1;
    TempKnots(TempKnotIndex) = KnotSequence(StartFlatIndex - deg);

    for (j = StartFlatIndex - deg + 1; j <= EndFlatIndex + deg; j++)
      if (std::abs(KnotSequence(j) - KnotSequence(j - 1)) <= gp::Resolution())
        TempMults(TempKnotIndex)++;
      else
        TempKnots(++TempKnotIndex) = KnotSequence(j);

    int TempStartIndex = 1, TempEndIndex = TempKnotIndex;
    if (TempMults(TempStartIndex) == 1)
      TempMults(++TempStartIndex)++;
    if (TempMults(TempEndIndex) == 1)
      TempMults(--TempEndIndex)++;

    int                        NewNbKnots = TempEndIndex - TempStartIndex + 1;
    NCollection_Array1<int>    newMults(1, NewNbKnots);
    NCollection_Array1<double> newKnots(1, NewNbKnots);
    for (j = 1; j <= NewNbKnots; j++)
    {
      newMults(j) = TempMults(j + TempStartIndex - 1);
      newKnots(j) = TempKnots(j + TempStartIndex - 1);
    }

    int                        NewNbPoles = BSplCLib::NbPoles(deg, false, newMults);
    NCollection_Array1<gp_Pnt> newPoles(1, NewNbPoles);
    NCollection_Array1<double> newWeights(1, NewNbPoles);
    // clang-format off
    int PoleIndex = StartFlatIndex - deg;//Index of starting pole when splitting B-Spline is an index of starting knot
    // clang-format on
    for (j = 1; j <= NewNbPoles; j++)
    {
      newWeights(j) = Weights(j + PoleIndex - 1);
      newPoles(j)   = Poles(j + PoleIndex - 1);
    }

    occ::handle<Geom_BSplineCurve> newC =
      new Geom_BSplineCurve(newPoles, newWeights, newKnots, newMults, deg);
    seqBS->Append(newC);

    StartKnotIndex = EndKnotIndex;
  }
  return true;
}

//=================================================================================================

static occ::handle<Geom_BSplineCurve> BSplineCurve2dTo3d(const occ::handle<Geom2d_BSplineCurve>& BS)
{
  int                                deg     = BS->Degree();
  int                                NbPoles = BS->NbPoles();
  const NCollection_Array1<gp_Pnt2d>& Poles2d = BS->Poles();
  NCollection_Array1<double>         Weights(1, NbPoles);
  if (BS->IsRational())
    Weights = *BS->Weights();
  else
    Weights.Init(1);
  const NCollection_Array1<double>& Knots = BS->Knots();
  const NCollection_Array1<int>&    Mults = BS->Multiplicities();

  NCollection_Array1<gp_Pnt> Poles3d(1, NbPoles);
  for (int i = 1; i <= NbPoles; i++)
    Poles3d(i) = gp_Pnt(Poles2d(i).X(), Poles2d(i).Y(), 0);

  occ::handle<Geom_BSplineCurve> BS3d =
    new Geom_BSplineCurve(Poles3d, Weights, Knots, Mults, deg, BS->IsPeriodic());
  return BS3d;
}

static occ::handle<Geom2d_BSplineCurve> BSplineCurve3dTo2d(const occ::handle<Geom_BSplineCurve>& BS)
{
  int                              deg     = BS->Degree();
  int                              NbPoles = BS->NbPoles();
  const NCollection_Array1<gp_Pnt>& Poles3d = BS->Poles();
  NCollection_Array1<double>       Weights(1, NbPoles);
  if (BS->IsRational())
    Weights = *BS->Weights();
  else
    Weights.Init(1);
  const NCollection_Array1<double>& Knots = BS->Knots();
  const NCollection_Array1<int>&    Mults = BS->Multiplicities();

  NCollection_Array1<gp_Pnt2d> Poles2d(1, NbPoles);
  for (int i = 1; i <= NbPoles; i++)
    Poles2d(i) = gp_Pnt2d(Poles3d(i).X(), Poles3d(i).Y());

  occ::handle<Geom2d_BSplineCurve> BS2d =
    new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults, deg, BS->IsPeriodic());
  return BS2d;
}

bool ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve(
  const occ::handle<Geom2d_BSplineCurve>&                               BS,
  occ::handle<NCollection_HSequence<occ::handle<Geom2d_BoundedCurve>>>& seqBS)
{
  if (BS.IsNull() || (BS->IsCN(1)))
    return false;

  occ::handle<Geom_BSplineCurve>                                     BS3d = BSplineCurve2dTo3d(BS);
  occ::handle<NCollection_HSequence<occ::handle<Geom_BoundedCurve>>> seqBS3d;
  bool result = C0BSplineToSequenceOfC1BSplineCurve(BS3d, seqBS3d);
  if (result)
  {
    seqBS = new NCollection_HSequence<occ::handle<Geom2d_BoundedCurve>>;
    for (int i = 1; i <= seqBS3d->Length(); i++)
      seqBS->Append(BSplineCurve3dTo2d(occ::down_cast<Geom_BSplineCurve>(seqBS3d->Value(i))));
  }
  return result;
}
