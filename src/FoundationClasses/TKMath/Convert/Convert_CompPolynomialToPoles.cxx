// Created on: 1995-05-30
// Created by: Xavier BENVENISTE
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

// 19-06-96 : JPI : NbPoles doit utiliser  ColLength() au lieu de RowLength()
// 16-09-96 : PMN : On ne doit pas se soucier de la continuite lorsqu'il n'y
//                  qu'un seul segment(PRO5474).
// 11-12-96 : PMN : Respect de l'indicage des tableaux passer en arguments
//                  TrueIntervals et PolynomialIntervals (BUC40077)
// 15-04-97 : PMN : Constructeurs avec un seul segement ou differentes
//                  continuitees.

#define No_Standard_OutOfRange

#include <BSplCLib.hxx>
#include <Convert_CompPolynomialToPoles.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

Convert_CompPolynomialToPoles::Convert_CompPolynomialToPoles(
  const int                                       NumCurves,
  const int                                       Continuity,
  const int                                       Dimension,
  const int                                       MaxDegree,
  const occ::handle<NCollection_HArray1<int>>&    NumCoeffPerCurve,
  const occ::handle<NCollection_HArray1<double>>& Coefficients,
  const occ::handle<NCollection_HArray2<double>>& PolynomialIntervals,
  const occ::handle<NCollection_HArray1<double>>& TrueIntervals)
    : myDone(false)
{
  int ii, delta;
  if (NumCurves <= 0 || NumCoeffPerCurve.IsNull() || Coefficients.IsNull()
      || PolynomialIntervals.IsNull() || TrueIntervals.IsNull() || Continuity < 0 || MaxDegree <= 0
      || Dimension <= 0 || PolynomialIntervals->RowLength() != 2)
  {
    throw Standard_ConstructionError("Convert_CompPolynomialToPoles:bad arguments");
  }
  myDegree = 0;

  delta = NumCurves - 1;
  for (ii = NumCoeffPerCurve->Lower(); ii <= NumCoeffPerCurve->Lower() + delta; ii++)
  {
    myDegree = std::max(NumCoeffPerCurve->Value(ii) - 1, myDegree);
  }
  if ((Continuity > myDegree) && (NumCurves > 1))
  {
    throw Standard_ConstructionError("Convert_CompPolynomialToPoles:Continuity is too great");
  }
  //
  //  prepare output
  //
  int Tindex, multiplicities;

  myKnots = new NCollection_HArray1<double>(1, NumCurves + 1);
  for (ii = 1, Tindex = TrueIntervals->Lower(); ii <= NumCurves + 1; ii++, Tindex++)
  {
    myKnots->ChangeArray1().SetValue(ii, TrueIntervals->Value(Tindex));
  }

  multiplicities = myDegree - Continuity;
  myMults        = new NCollection_HArray1<int>(1, NumCurves + 1);
  for (ii = 2; ii < NumCurves + 1; ii++)
  {
    myMults->SetValue(ii, multiplicities);
  }
  myMults->SetValue(1, myDegree + 1);
  myMults->SetValue(NumCurves + 1, myDegree + 1);

  Perform(NumCurves,
          MaxDegree,
          Dimension,
          NumCoeffPerCurve->Array1(),
          Coefficients->Array1(),
          PolynomialIntervals->Array2(),
          TrueIntervals->Array1());
}

Convert_CompPolynomialToPoles::Convert_CompPolynomialToPoles(
  const int                         NumCurves,
  const int                         Dimension,
  const int                         MaxDegree,
  const NCollection_Array1<int>&    Continuity,
  const NCollection_Array1<int>&    NumCoeffPerCurve,
  const NCollection_Array1<double>& Coefficients,
  const NCollection_Array2<double>& PolynomialIntervals,
  const NCollection_Array1<double>& TrueIntervals)
    : myDone(false)
{
  int ii, delta;
  if (NumCurves <= 0 || MaxDegree <= 0 || Dimension <= 0 || PolynomialIntervals.RowLength() != 2)
  {
    throw Standard_ConstructionError("Convert_CompPolynomialToPoles:bad arguments");
  }
  myDegree = 0;

  delta = NumCurves - 1;
  for (ii = NumCoeffPerCurve.Lower(); ii <= NumCoeffPerCurve.Lower() + delta; ii++)
  {
    myDegree = std::max(NumCoeffPerCurve.Value(ii) - 1, myDegree);
  }
  //
  //  prepare output
  //
  int Tindex;

  myKnots = new NCollection_HArray1<double>(1, NumCurves + 1);
  for (ii = 1, Tindex = TrueIntervals.Lower(); ii <= NumCurves + 1; ii++, Tindex++)
  {
    myKnots->ChangeArray1().SetValue(ii, TrueIntervals.Value(Tindex));
  }

  myMults = new NCollection_HArray1<int>(1, NumCurves + 1);
  for (ii = 2; ii < NumCurves + 1; ii++)
  {
    if ((Continuity(ii) > myDegree) && (NumCurves > 1))
    {
      throw Standard_ConstructionError("Convert_CompPolynomialToPoles:Continuity is too great");
    }

    myMults->SetValue(ii, myDegree - Continuity(ii));
  }
  myMults->SetValue(1, myDegree + 1);
  myMults->SetValue(NumCurves + 1, myDegree + 1);

  // Calculs
  Perform(NumCurves,
          MaxDegree,
          Dimension,
          NumCoeffPerCurve,
          Coefficients,
          PolynomialIntervals,
          TrueIntervals);
}

Convert_CompPolynomialToPoles::Convert_CompPolynomialToPoles(
  const int                         Dimension,
  const int                         MaxDegree,
  const int                         Degree,
  const NCollection_Array1<double>& Coefficients,
  const NCollection_Array1<double>& PolynomialIntervals,
  const NCollection_Array1<double>& TrueIntervals)
    : myDegree(Degree),
      myDone(false)

{
  if (MaxDegree <= 0 || Dimension <= 0 || PolynomialIntervals.Length() != 2)
  {
    throw Standard_ConstructionError("Convert_CompPolynomialToPoles:bad arguments");
  }

  NCollection_Array2<double> ThePolynomialIntervals(1, 1, 1, 2);
  ThePolynomialIntervals.SetValue(1, 1, PolynomialIntervals(PolynomialIntervals.Lower()));
  ThePolynomialIntervals.SetValue(1, 2, PolynomialIntervals(PolynomialIntervals.Upper()));

  NCollection_Array1<int> NumCoeffPerCurve(1, 1);
  NumCoeffPerCurve(1) = Degree + 1;

  myKnots = new NCollection_HArray1<double>(1, 2);
  myKnots->ChangeArray1().SetValue(1, TrueIntervals.Value(TrueIntervals.Lower()));
  myKnots->ChangeArray1().SetValue(2, TrueIntervals.Value(TrueIntervals.Lower() + 1));

  myMults = new NCollection_HArray1<int>(1, 2);
  myMults->Init(myDegree + 1);

  // Calculs
  Perform(1,
          MaxDegree,
          Dimension,
          NumCoeffPerCurve,
          Coefficients,
          ThePolynomialIntervals,
          TrueIntervals);
}

void Convert_CompPolynomialToPoles::Perform(const int                         NumCurves,
                                            const int                         MaxDegree,
                                            const int                         Dimension,
                                            const NCollection_Array1<int>&    NumCoeffPerCurve,
                                            const NCollection_Array1<double>& Coefficients,
                                            const NCollection_Array2<double>& PolynomialIntervals,
                                            const NCollection_Array1<double>& TrueIntervals)
{
  int ii, num_flat_knots, index, Tindex, Pindex, coeff_index, inversion_problem, poles_index,
    num_poles;
  double normalized_value, *coefficient_array, *poles_array;

  num_flat_knots = 2 * myDegree + 2;
  for (ii = 2; ii < myMults->Length(); ii++)
  {
    num_flat_knots += myMults->Value(ii);
  }
  num_poles = num_flat_knots - myDegree - 1;

  myFlatKnots = new NCollection_HArray1<double>(1, num_flat_knots);
  BSplCLib::KnotSequence(myKnots->Array1(),
                         myMults->Array1(),
                         myDegree,
                         false,
                         myFlatKnots->ChangeArray1());

  NCollection_Array1<double> parameters(1, num_poles);
  BSplCLib::BuildSchoenbergPoints(myDegree, myFlatKnots->Array1(), parameters);
  myPoles     = new NCollection_HArray2<double>(1, num_poles, 1, Dimension);
  index       = 2;
  Tindex      = TrueIntervals.Lower() + 1;
  Pindex      = PolynomialIntervals.LowerRow();
  poles_array = (double*)&(myPoles->ChangeArray2()).Value(1, 1);

  NCollection_Array1<int> contact_array(1, num_poles);

  poles_index = 0;
  for (ii = 1; ii <= num_poles; ii++, poles_index += Dimension)
  {
    contact_array.SetValue(ii, 0);
    while (parameters.Value(ii) >= TrueIntervals(Tindex) && index <= NumCurves)
    {
      index++;
      Tindex++;
      Pindex++;
    }
    //
    // normalized value so that it fits the original intervals for
    // the polynomial definition of the curves
    //
    normalized_value = parameters.Value(ii) - TrueIntervals(Tindex - 1);
    normalized_value /= TrueIntervals(Tindex) - TrueIntervals(Tindex - 1);
    normalized_value =
      (1.0e0 - normalized_value) * PolynomialIntervals(Pindex, PolynomialIntervals.LowerCol())
      + normalized_value * PolynomialIntervals(Pindex, PolynomialIntervals.UpperCol());
    coeff_index =
      ((index - 2) * Dimension * (std::max(MaxDegree, myDegree) + 1)) + Coefficients.Lower();

    coefficient_array = (double*)&(Coefficients(coeff_index));
    int Deg           = NumCoeffPerCurve(NumCoeffPerCurve.Lower() + index - 2) - 1;

    PLib::NoDerivativeEvalPolynomial(normalized_value,
                                     Deg,
                                     Dimension,
                                     Deg * Dimension,
                                     coefficient_array[0],
                                     poles_array[poles_index]);
  }
  //
  // interpolation at schoenberg points should yield the desired
  // result
  //
  BSplCLib::Interpolate(myDegree,
                        myFlatKnots->Array1(),
                        parameters,
                        contact_array,
                        Dimension,
                        poles_array[0],
                        inversion_problem);
  if (inversion_problem != 0)
  {
    throw Standard_ConstructionError("Convert_CompPolynomialToPoles:inversion_problem");
  }
  myDone = true;
}

//=================================================================================================

int Convert_CompPolynomialToPoles::NbPoles() const
{
  if (myDone)
  {
    return myPoles->ColLength();
  }
  else
    return 0;
}

//=================================================================================================

void Convert_CompPolynomialToPoles::Poles(occ::handle<NCollection_HArray2<double>>& P) const
{
  if (myDone)
  {
    P = myPoles;
  }
}

//=================================================================================================

int Convert_CompPolynomialToPoles::NbKnots() const
{
  if (myDone)
  {
    return myKnots->Length();
  }
  else
    return 0;
}

//=================================================================================================

void Convert_CompPolynomialToPoles::Knots(occ::handle<NCollection_HArray1<double>>& K) const
{
  if (myDone)
  {
    K = myKnots;
  }
}

//=================================================================================================

void Convert_CompPolynomialToPoles::Multiplicities(occ::handle<NCollection_HArray1<int>>& M) const
{
  if (myDone)
  {
    M = myMults;
  }
}

//=================================================================================================

bool Convert_CompPolynomialToPoles::IsDone() const
{
  return myDone;
}

//=================================================================================================

int Convert_CompPolynomialToPoles::Degree() const
{
  if (myDone)
  {
    return myDegree;
  }
  return 0;
}
