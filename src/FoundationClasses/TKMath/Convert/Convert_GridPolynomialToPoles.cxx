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
  const int                                       MaxUDegree,
  const int                                       MaxVDegree,
  const occ::handle<NCollection_HArray1<int>>&    NumCoeffPerSurface,
  const occ::handle<NCollection_HArray1<double>>& Coefficients,
  const occ::handle<NCollection_HArray1<double>>& PolynomialUIntervals,
  const occ::handle<NCollection_HArray1<double>>& PolynomialVIntervals)
    : myDone(false)
{
  // Les Controles
  if ((NumCoeffPerSurface->Lower() != 1) || (NumCoeffPerSurface->Upper() != 2))
  {
    throw Standard_DomainError("Convert : Wrong Coefficients");
  }
  if ((Coefficients->Lower() != 1)
      || (Coefficients->Upper() != 3 * (MaxUDegree + 1) * (MaxVDegree + 1)))
  {
    throw Standard_DomainError("Convert : Wrong Coefficients");
  }

  // Les Degres
  myUDegree = NumCoeffPerSurface->Value(1) - 1;
  myVDegree = NumCoeffPerSurface->Value(2) - 1;

  if (myUDegree > MaxUDegree)
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxUDegree");
  if (myVDegree > MaxVDegree)
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxVDegree");

  occ::handle<NCollection_HArray2<int>> NumCoeff = new (NCollection_HArray2<int>)(1, 1, 1, 2);
  NumCoeff->SetValue(1, 1, NumCoeffPerSurface->Value(1));
  NumCoeff->SetValue(1, 2, NumCoeffPerSurface->Value(2));

  Perform(0,
          0,
          MaxUDegree,
          MaxVDegree,
          NumCoeff,
          Coefficients,
          PolynomialUIntervals,
          PolynomialVIntervals,
          PolynomialUIntervals,
          PolynomialVIntervals);
}

Convert_GridPolynomialToPoles::Convert_GridPolynomialToPoles(
  const int                                       NbUSurfaces,
  const int                                       NbVSurfaces,
  const int                                       UContinuity,
  const int                                       VContinuity,
  const int                                       MaxUDegree,
  const int                                       MaxVDegree,
  const occ::handle<NCollection_HArray2<int>>&    NumCoeffPerSurface,
  const occ::handle<NCollection_HArray1<double>>& Coefficients,
  const occ::handle<NCollection_HArray1<double>>& PolynomialUIntervals,
  const occ::handle<NCollection_HArray1<double>>& PolynomialVIntervals,
  const occ::handle<NCollection_HArray1<double>>& TrueUIntervals,
  const occ::handle<NCollection_HArray1<double>>& TrueVIntervals)
    : myDone(false)
{
  int ii;
  int RealUDegree = std::max(MaxUDegree, 2 * UContinuity + 1);
  int RealVDegree = std::max(MaxVDegree, 2 * VContinuity + 1);
  myUDegree       = 0;
  myVDegree       = 0;

  // Les controles
  if ((NumCoeffPerSurface->LowerRow() != 1)
      || (NumCoeffPerSurface->UpperRow() != NbUSurfaces * NbVSurfaces)
      || (NumCoeffPerSurface->LowerCol() != 1) || (NumCoeffPerSurface->UpperCol() != 2))
  {
    throw Standard_DomainError("Convert : Wrong NumCoeffPerSurface");
  }

  if ((Coefficients->Lower() != 1)
      || (Coefficients->Upper()
          != 3 * NbUSurfaces * NbVSurfaces * (RealUDegree + 1) * (RealVDegree + 1)))
  {
    throw Standard_DomainError("Convert : Wrong Coefficients");
  }

  // Calcul des degree
  for (ii = 1; ii <= NbUSurfaces * NbVSurfaces; ii++)
  {
    if (NumCoeffPerSurface->Value(ii, 1) > myUDegree + 1)
      myUDegree = NumCoeffPerSurface->Value(ii, 1) - 1;
    if (NumCoeffPerSurface->Value(ii, 2) > myVDegree + 1)
      myVDegree = NumCoeffPerSurface->Value(ii, 2) - 1;
  }

  if (myUDegree > RealUDegree)
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxUDegree");
  if (myVDegree > RealVDegree)
    throw Standard_DomainError("Convert : Incoherence between NumCoeffPerSurface and MaxVDegree");

  Perform(UContinuity,
          VContinuity,
          RealUDegree,
          RealVDegree,
          NumCoeffPerSurface,
          Coefficients,
          PolynomialUIntervals,
          PolynomialVIntervals,
          TrueUIntervals,
          TrueVIntervals);
}

void Convert_GridPolynomialToPoles::Perform(
  const int                                       UContinuity,
  const int                                       VContinuity,
  const int                                       MaxUDegree,
  const int                                       MaxVDegree,
  const occ::handle<NCollection_HArray2<int>>&    NumCoeffPerSurface,
  const occ::handle<NCollection_HArray1<double>>& Coefficients,
  const occ::handle<NCollection_HArray1<double>>& PolynomialUIntervals,
  const occ::handle<NCollection_HArray1<double>>& PolynomialVIntervals,
  const occ::handle<NCollection_HArray1<double>>& TrueUIntervals,
  const occ::handle<NCollection_HArray1<double>>& TrueVIntervals)
{
  // (1) Construction des Tables monodimensionnelles ----------------------------
  occ::handle<NCollection_HArray1<double>> UParameters, VParameters;
  myUKnots                 = new (NCollection_HArray1<double>)(1, TrueUIntervals->Length());
  myUKnots->ChangeArray1() = TrueUIntervals->Array1();
  myVKnots                 = new (NCollection_HArray1<double>)(1, TrueVIntervals->Length());
  myVKnots->ChangeArray1() = TrueVIntervals->Array1();

  BuildArray(myUDegree, myUKnots, UContinuity, myUFlatKnots, myUMults, UParameters);

  BuildArray(myVDegree, myVKnots, VContinuity, myVFlatKnots, myVMults, VParameters);

  // (2) Digitalisation -------------------------------------------------------

  int    ii, jj, Uindex = 0, Vindex = 0;
  int    Patch_Indice = 0;
  double NValue, UValue, VValue;
  int    dimension = 3 * (myVDegree + 1);
  int    SizPatch  = 3 * (MaxUDegree + 1) * (MaxVDegree + 1);
  myPoles = new (NCollection_HArray2<gp_Pnt>)(1, UParameters->Length(), 1, VParameters->Length());

  NCollection_Array1<double> Patch(1, (myUDegree + 1) * dimension);
  NCollection_Array1<double> Point(1, 3);
  double*                    Coeffs = (double*)&Patch.ChangeValue(1);
  double*                    Digit  = (double*)&Point.ChangeValue(1);

  for (ii = 1, Uindex = 1; ii <= UParameters->Length(); ii++)
  {

    while (UParameters->Value(ii) > TrueUIntervals->Value(Uindex + 1)
           && Uindex < myUKnots->Length() - 1)
    {
      Uindex++;
    }

    NValue = (UParameters->Value(ii) - TrueUIntervals->Value(Uindex))
             / (TrueUIntervals->Value(Uindex + 1) - TrueUIntervals->Value(Uindex));
    UValue =
      (1 - NValue) * PolynomialUIntervals->Value(1) + NValue * PolynomialUIntervals->Value(2);

    for (jj = 1, Vindex = 1; jj <= VParameters->Length(); jj++)
    {

      while (VParameters->Value(jj) > TrueVIntervals->Value(Vindex + 1)
             && Vindex < myVKnots->Length() - 1)
      {
        Vindex++;
      }

      NValue = (VParameters->Value(jj) - TrueVIntervals->Value(Vindex))
               / (TrueVIntervals->Value(Vindex + 1) - TrueVIntervals->Value(Vindex));
      VValue =
        (1 - NValue) * PolynomialVIntervals->Value(1) + NValue * PolynomialVIntervals->Value(2);

      // (2.1) Extraction du bon Patch
      if (Patch_Indice != Uindex + (myUKnots->Length() - 1) * (Vindex - 1))
      {
        int k1, k2, pos, ll = 1;
        Patch_Indice = Uindex + (myUKnots->Length() - 1) * (Vindex - 1);
        for (k1 = 1; k1 <= NumCoeffPerSurface->Value(Patch_Indice, 1); k1++)
        {
          pos = SizPatch * (Patch_Indice - 1) + 3 * (MaxVDegree + 1) * (k1 - 1) + 1;
          for (k2 = 1; k2 <= NumCoeffPerSurface->Value(Patch_Indice, 2); k2++, pos += 3)
          {
            Patch(ll)     = Coefficients->Value(pos);
            Patch(ll + 1) = Coefficients->Value(pos + 1);
            Patch(ll + 2) = Coefficients->Value(pos + 2);
            ll += 3;
          }
        }
      }

      // (2.2) Positionnement en UValue,VValue
      PLib::EvalPoly2Var(UValue,
                         VValue,
                         0,
                         0,
                         NumCoeffPerSurface->Value(Patch_Indice, 1) - 1,
                         NumCoeffPerSurface->Value(Patch_Indice, 2) - 1,
                         3,
                         Coeffs[0],
                         Digit[0]);

      myPoles->SetValue(ii, jj, gp_Pnt(Digit[0], Digit[1], Digit[2]));
    }
  }

  // (3)Interpolation --------------------------------------------------------------

  int InversionProblem;
  BSplSLib::Interpolate(myUDegree,
                        myVDegree,
                        myUFlatKnots->Array1(),
                        myVFlatKnots->Array1(),
                        UParameters->Array1(),
                        VParameters->Array1(),
                        myPoles->ChangeArray2(),
                        InversionProblem);
  myDone = (InversionProblem == 0);
}

void Convert_GridPolynomialToPoles::BuildArray(
  const int                                       Degree,
  const occ::handle<NCollection_HArray1<double>>& Knots,
  const int                                       Continuity,
  occ::handle<NCollection_HArray1<double>>&       FlatKnots,
  occ::handle<NCollection_HArray1<int>>&          Mults,
  occ::handle<NCollection_HArray1<double>>&       Parameters) const
{
  int NumCurves = Knots->Length() - 1;

  // Calcul des Multiplicites
  int ii;
  int multiplicities = Degree - Continuity;
  Mults              = new (NCollection_HArray1<int>)(1, Knots->Length());

  for (ii = 2; ii < Knots->Length(); ii++)
  {
    Mults->SetValue(ii, multiplicities);
  }
  Mults->SetValue(1, Degree + 1);
  Mults->SetValue(NumCurves + 1, Degree + 1);

  // Calcul des Noeuds Plats
  int num_flat_knots = multiplicities * (NumCurves - 1) + 2 * Degree + 2;
  FlatKnots          = new NCollection_HArray1<double>(1, num_flat_knots);

  BSplCLib::KnotSequence(Knots->Array1(),
                         Mults->Array1(),
                         Degree,
                         false,
                         FlatKnots->ChangeArray1());

  // Calcul du nombre de Poles
  int num_poles = num_flat_knots - Degree - 1;

  // Cacul des parametres d'interpolation
  Parameters = new (NCollection_HArray1<double>)(1, num_poles);
  BSplCLib::BuildSchoenbergPoints(Degree, FlatKnots->Array1(), Parameters->ChangeArray1());
}

int Convert_GridPolynomialToPoles::NbUPoles() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myPoles->ColLength();
}

int Convert_GridPolynomialToPoles::NbVPoles() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myPoles->RowLength();
}

const occ::handle<NCollection_HArray2<gp_Pnt>>& Convert_GridPolynomialToPoles::Poles() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myPoles;
}

int Convert_GridPolynomialToPoles::UDegree() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUDegree;
}

int Convert_GridPolynomialToPoles::VDegree() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVDegree;
}

int Convert_GridPolynomialToPoles::NbUKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUKnots->Length();
}

int Convert_GridPolynomialToPoles::NbVKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVKnots->Length();
}

const occ::handle<NCollection_HArray1<double>>& Convert_GridPolynomialToPoles::UKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUKnots;
}

const occ::handle<NCollection_HArray1<double>>& Convert_GridPolynomialToPoles::VKnots() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVKnots;
}

const occ::handle<NCollection_HArray1<int>>& Convert_GridPolynomialToPoles::UMultiplicities() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myUMults;
}

const occ::handle<NCollection_HArray1<int>>& Convert_GridPolynomialToPoles::VMultiplicities() const
{
  StdFail_NotDone_Raise_if(!myDone, "GridPolynomialToPoles");
  return myVMults;
}

bool Convert_GridPolynomialToPoles::IsDone() const
{
  return myDone;
}
