// Created on: 1996-07-08
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

// Modified:	Fri Oct  3 14:58:05 1997
//    by:	Joelle CHAUVET
//              Condition d'extraction corrigee
//              + positionnement par EvalPoly2Var

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <PLib.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

Convert_GridPolynomialToPoles::Convert_GridPolynomialToPoles(
  const int                                       theMaxUDegree,
  const int                                       theMaxVDegree,
  const occ::handle<NCollection_HArray1<int>>&    theNumCoeff,
  const occ::handle<NCollection_HArray1<double>>& theCoefficients,
  const occ::handle<NCollection_HArray1<double>>& thePolynomialUIntervals,
  const occ::handle<NCollection_HArray1<double>>& thePolynomialVIntervals)
    : Convert_GridPolynomialToPoles(theMaxUDegree,
                                    theMaxVDegree,
                                    theNumCoeff->Array1(),
                                    theCoefficients->Array1(),
                                    thePolynomialUIntervals->Array1(),
                                    thePolynomialVIntervals->Array1())
{
}

Convert_GridPolynomialToPoles::Convert_GridPolynomialToPoles(
  const int                                       theNbUSurfaces,
  const int                                       theNbVSurfaces,
  const int                                       theUContinuity,
  const int                                       theVContinuity,
  const int                                       theMaxUDegree,
  const int                                       theMaxVDegree,
  const occ::handle<NCollection_HArray2<int>>&    theNumCoeffPerSurface,
  const occ::handle<NCollection_HArray1<double>>& theCoefficients,
  const occ::handle<NCollection_HArray1<double>>& thePolynomialUIntervals,
  const occ::handle<NCollection_HArray1<double>>& thePolynomialVIntervals,
  const occ::handle<NCollection_HArray1<double>>& theTrueUIntervals,
  const occ::handle<NCollection_HArray1<double>>& theTrueVIntervals)
    : Convert_GridPolynomialToPoles(theNbUSurfaces,
                                    theNbVSurfaces,
                                    theUContinuity,
                                    theVContinuity,
                                    theMaxUDegree,
                                    theMaxVDegree,
                                    theNumCoeffPerSurface->Array2(),
                                    theCoefficients->Array1(),
                                    thePolynomialUIntervals->Array1(),
                                    thePolynomialVIntervals->Array1(),
                                    theTrueUIntervals->Array1(),
                                    theTrueVIntervals->Array1())
{
}

Convert_GridPolynomialToPoles::Convert_GridPolynomialToPoles(
  const int                         theMaxUDegree,
  const int                         theMaxVDegree,
  const NCollection_Array1<int>&    theNumCoeff,
  const NCollection_Array1<double>& theCoefficients,
  const NCollection_Array1<double>& thePolynomialUIntervals,
  const NCollection_Array1<double>& thePolynomialVIntervals)
    : myUDegree(0),
      myVDegree(0),
      myDone(false)
{
  if (theNumCoeff.Lower() != 1 || theNumCoeff.Upper() != 2)
  {
    throw Standard_DomainError("Convert : Wrong Coefficients");
  }
  if (theCoefficients.Lower() != 1
      || theCoefficients.Upper() != 3 * (theMaxUDegree + 1) * (theMaxVDegree + 1))
  {
    throw Standard_DomainError("Convert : Wrong Coefficients");
  }

  myUDegree = theNumCoeff.Value(1) - 1;
  myVDegree = theNumCoeff.Value(2) - 1;

  if (myUDegree > theMaxUDegree)
  {
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxUDegree");
  }
  if (myVDegree > theMaxVDegree)
  {
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxVDegree");
  }

  NCollection_Array2<int> aNumCoeff2D(1, 1, 1, 2);
  aNumCoeff2D.SetValue(1, 1, theNumCoeff.Value(1));
  aNumCoeff2D.SetValue(1, 2, theNumCoeff.Value(2));

  Perform(0,
          0,
          theMaxUDegree,
          theMaxVDegree,
          aNumCoeff2D,
          theCoefficients,
          thePolynomialUIntervals,
          thePolynomialVIntervals,
          thePolynomialUIntervals,
          thePolynomialVIntervals);
}

Convert_GridPolynomialToPoles::Convert_GridPolynomialToPoles(
  const int                         theNbUSurfaces,
  const int                         theNbVSurfaces,
  const int                         theUContinuity,
  const int                         theVContinuity,
  const int                         theMaxUDegree,
  const int                         theMaxVDegree,
  const NCollection_Array2<int>&    theNumCoeffPerSurface,
  const NCollection_Array1<double>& theCoefficients,
  const NCollection_Array1<double>& thePolynomialUIntervals,
  const NCollection_Array1<double>& thePolynomialVIntervals,
  const NCollection_Array1<double>& theTrueUIntervals,
  const NCollection_Array1<double>& theTrueVIntervals)
    : myUDegree(0),
      myVDegree(0),
      myDone(false)
{
  const int aRealUDegree = std::max(theMaxUDegree, 2 * theUContinuity + 1);
  const int aRealVDegree = std::max(theMaxVDegree, 2 * theVContinuity + 1);

  if (theNumCoeffPerSurface.LowerRow() != 1
      || theNumCoeffPerSurface.UpperRow() != theNbUSurfaces * theNbVSurfaces
      || theNumCoeffPerSurface.LowerCol() != 1 || theNumCoeffPerSurface.UpperCol() != 2)
  {
    throw Standard_DomainError("Convert : Wrong NumCoeffPerSurface");
  }

  if (theCoefficients.Lower() != 1
      || theCoefficients.Upper()
           != 3 * theNbUSurfaces * theNbVSurfaces * (aRealUDegree + 1) * (aRealVDegree + 1))
  {
    throw Standard_DomainError("Convert : Wrong Coefficients");
  }

  // Compute actual degrees from the supplied coefficient counts.
  for (int ii = 1; ii <= theNbUSurfaces * theNbVSurfaces; ++ii)
  {
    if (theNumCoeffPerSurface.Value(ii, 1) > myUDegree + 1)
    {
      myUDegree = theNumCoeffPerSurface.Value(ii, 1) - 1;
    }
    if (theNumCoeffPerSurface.Value(ii, 2) > myVDegree + 1)
    {
      myVDegree = theNumCoeffPerSurface.Value(ii, 2) - 1;
    }
  }

  if (myUDegree > aRealUDegree)
  {
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxUDegree");
  }
  if (myVDegree > aRealVDegree)
  {
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxVDegree");
  }

  Perform(theUContinuity,
          theVContinuity,
          aRealUDegree,
          aRealVDegree,
          theNumCoeffPerSurface,
          theCoefficients,
          thePolynomialUIntervals,
          thePolynomialVIntervals,
          theTrueUIntervals,
          theTrueVIntervals);
}

void Convert_GridPolynomialToPoles::Perform(
  const int                         theUContinuity,
  const int                         theVContinuity,
  const int                         theMaxUDegree,
  const int                         theMaxVDegree,
  const NCollection_Array2<int>&    theNumCoeffPerSurface,
  const NCollection_Array1<double>& theCoefficients,
  const NCollection_Array1<double>& thePolynomialUIntervals,
  const NCollection_Array1<double>& thePolynomialVIntervals,
  const NCollection_Array1<double>& theTrueUIntervals,
  const NCollection_Array1<double>& theTrueVIntervals)
{
  // (1) Build monodimensional knot/multiplicity/parameter tables.
  NCollection_Array1<double> aUParameters;
  NCollection_Array1<double> aVParameters;
  myUKnots = NCollection_Array1<double>(theTrueUIntervals);
  myVKnots = NCollection_Array1<double>(theTrueVIntervals);

  BuildArray(myUDegree, myUKnots, theUContinuity, myUFlatKnots, myUMults, aUParameters);
  BuildArray(myVDegree, myVKnots, theVContinuity, myVFlatKnots, myVMults, aVParameters);

  // (2) Digitalisation
  const int aDimension = 3 * (myVDegree + 1);
  const int aSizPatch  = 3 * (theMaxUDegree + 1) * (theMaxVDegree + 1);
  myPoles = NCollection_Array2<gp_Pnt>(1, aUParameters.Length(), 1, aVParameters.Length());

  NCollection_Array1<double> aPatch(1, (myUDegree + 1) * aDimension);
  NCollection_Array1<double> aPoint(1, 3);
  double* const              aCoeffs = &aPatch.ChangeValue(1);
  double* const              aDigit  = &aPoint.ChangeValue(1);

  int aPatchIndice = 0;
  int aUIndex      = 1;
  for (int ii = 1; ii <= aUParameters.Length(); ++ii)
  {
    while (aUParameters.Value(ii) > theTrueUIntervals.Value(aUIndex + 1)
           && aUIndex < myUKnots.Length() - 1)
    {
      ++aUIndex;
    }
    double aNValue = (aUParameters.Value(ii) - theTrueUIntervals.Value(aUIndex))
                     / (theTrueUIntervals.Value(aUIndex + 1) - theTrueUIntervals.Value(aUIndex));
    const double aUValue = (1.0 - aNValue) * thePolynomialUIntervals.Value(1)
                           + aNValue * thePolynomialUIntervals.Value(2);

    int aVIndex = 1;
    for (int jj = 1; jj <= aVParameters.Length(); ++jj)
    {
      while (aVParameters.Value(jj) > theTrueVIntervals.Value(aVIndex + 1)
             && aVIndex < myVKnots.Length() - 1)
      {
        ++aVIndex;
      }
      aNValue = (aVParameters.Value(jj) - theTrueVIntervals.Value(aVIndex))
                / (theTrueVIntervals.Value(aVIndex + 1) - theTrueVIntervals.Value(aVIndex));
      const double aVValue = (1.0 - aNValue) * thePolynomialVIntervals.Value(1)
                             + aNValue * thePolynomialVIntervals.Value(2);

      // (2.1) Extract the active patch coefficients.
      if (aPatchIndice != aUIndex + (myUKnots.Length() - 1) * (aVIndex - 1))
      {
        aPatchIndice = aUIndex + (myUKnots.Length() - 1) * (aVIndex - 1);
        int ll       = 1;
        for (int k1 = 1; k1 <= theNumCoeffPerSurface.Value(aPatchIndice, 1); ++k1)
        {
          int pos = aSizPatch * (aPatchIndice - 1) + 3 * (theMaxVDegree + 1) * (k1 - 1) + 1;
          for (int k2 = 1; k2 <= theNumCoeffPerSurface.Value(aPatchIndice, 2); ++k2, pos += 3)
          {
            aPatch(ll)     = theCoefficients.Value(pos);
            aPatch(ll + 1) = theCoefficients.Value(pos + 1);
            aPatch(ll + 2) = theCoefficients.Value(pos + 2);
            ll += 3;
          }
        }
      }

      // (2.2) Evaluate at (aUValue, aVValue).
      PLib::EvalPoly2Var(aUValue,
                         aVValue,
                         0,
                         0,
                         theNumCoeffPerSurface.Value(aPatchIndice, 1) - 1,
                         theNumCoeffPerSurface.Value(aPatchIndice, 2) - 1,
                         3,
                         aCoeffs[0],
                         aDigit[0]);

      myPoles.SetValue(ii, jj, gp_Pnt(aDigit[0], aDigit[1], aDigit[2]));
    }
  }

  // (3)Interpolation --------------------------------------------------------------

  int InversionProblem;
  BSplSLib::Interpolate(myUDegree,
                        myVDegree,
                        myUFlatKnots,
                        myVFlatKnots,
                        aUParameters,
                        aVParameters,
                        myPoles,
                        InversionProblem);
  myDone = (InversionProblem == 0);
}

void Convert_GridPolynomialToPoles::BuildArray(const int                         Degree,
                                               const NCollection_Array1<double>& Knots,
                                               const int                         Continuity,
                                               NCollection_Array1<double>&       FlatKnots,
                                               NCollection_Array1<int>&          Mults,
                                               NCollection_Array1<double>&       Parameters) const
{
  const int NumCurves = Knots.Length() - 1;

  // Calcul des Multiplicites
  const int multiplicities = Degree - Continuity;
  Mults                    = NCollection_Array1<int>(1, Knots.Length());

  for (int ii = 2; ii < Knots.Length(); ii++)
  {
    Mults.SetValue(ii, multiplicities);
  }
  Mults.SetValue(1, Degree + 1);
  Mults.SetValue(NumCurves + 1, Degree + 1);

  // Calcul des Noeuds Plats
  const int num_flat_knots = multiplicities * (NumCurves - 1) + 2 * Degree + 2;
  FlatKnots                = NCollection_Array1<double>(1, num_flat_knots);

  BSplCLib::KnotSequence(Knots, Mults, Degree, false, FlatKnots);

  // Calcul du nombre de Poles
  const int num_poles = num_flat_knots - Degree - 1;

  // Cacul des parametres d'interpolation
  Parameters = NCollection_Array1<double>(1, num_poles);
  BSplCLib::BuildSchoenbergPoints(Degree, FlatKnots, Parameters);
}

//=================================================================================================

int Convert_GridPolynomialToPoles::NbUPoles() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myPoles.ColLength();
}

//=================================================================================================

int Convert_GridPolynomialToPoles::NbVPoles() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myPoles.RowLength();
}

//=================================================================================================

const NCollection_Array2<gp_Pnt>& Convert_GridPolynomialToPoles::Poles() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myPoles;
}

//=================================================================================================

int Convert_GridPolynomialToPoles::UDegree() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUDegree;
}

//=================================================================================================

int Convert_GridPolynomialToPoles::VDegree() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVDegree;
}

//=================================================================================================

int Convert_GridPolynomialToPoles::NbUKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUKnots.Length();
}

//=================================================================================================

int Convert_GridPolynomialToPoles::NbVKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVKnots.Length();
}

//=================================================================================================

const NCollection_Array1<double>& Convert_GridPolynomialToPoles::UKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUKnots;
}

//=================================================================================================

const NCollection_Array1<double>& Convert_GridPolynomialToPoles::VKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVKnots;
}

//=================================================================================================

const NCollection_Array1<int>& Convert_GridPolynomialToPoles::UMultiplicities() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUMults;
}

//=================================================================================================

const NCollection_Array1<int>& Convert_GridPolynomialToPoles::VMultiplicities() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVMults;
}

//=================================================================================================

bool Convert_GridPolynomialToPoles::IsDone() const
{
  return myDone;
}
