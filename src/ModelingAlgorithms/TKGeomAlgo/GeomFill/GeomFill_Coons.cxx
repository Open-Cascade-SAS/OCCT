// Created on: 1993-09-29
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
#include <GeomFill_Coons.hxx>
#include <PLib.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//=================================================================================================

GeomFill_Coons::GeomFill_Coons() = default;

//=================================================================================================

GeomFill_Coons::GeomFill_Coons(const NCollection_Array1<gp_Pnt>& P1,
                               const NCollection_Array1<gp_Pnt>& P2,
                               const NCollection_Array1<gp_Pnt>& P3,
                               const NCollection_Array1<gp_Pnt>& P4)
{
  Init(P1, P2, P3, P4);
}

//=================================================================================================

GeomFill_Coons::GeomFill_Coons(const NCollection_Array1<gp_Pnt>& P1,
                               const NCollection_Array1<gp_Pnt>& P2,
                               const NCollection_Array1<gp_Pnt>& P3,
                               const NCollection_Array1<gp_Pnt>& P4,
                               const NCollection_Array1<double>& W1,
                               const NCollection_Array1<double>& W2,
                               const NCollection_Array1<double>& W3,
                               const NCollection_Array1<double>& W4)
{
  Init(P1, P2, P3, P4, W1, W2, W3, W4);
}

//=================================================================================================

void GeomFill_Coons::Init(const NCollection_Array1<gp_Pnt>& P1,
                          const NCollection_Array1<gp_Pnt>& P2,
                          const NCollection_Array1<gp_Pnt>& P3,
                          const NCollection_Array1<gp_Pnt>& P4)
{
  Standard_DomainError_Raise_if(P1.Length() != P3.Length() || P2.Length() != P4.Length(), " ");

  int NPolU = P1.Length();
  int NPolV = P2.Length();

  IsRational = false;

  myPoles = new NCollection_HArray2<gp_Pnt>(1, NPolU, 1, NPolV);

  // The boundaries are not modified
  int i, j, k;

  for (i = 1; i <= NPolU; i++)
  {
    myPoles->SetValue(i, 1, P1(i));
    myPoles->SetValue(i, NPolV, P3(i));
  }
  for (i = 1; i <= NPolV; i++)
  {
    myPoles->SetValue(1, i, P2(i));
    myPoles->SetValue(NPolU, i, P4(i));
  }

  // Calcul des coefficients multiplicateurs
  NCollection_Array1<gp_Pnt> Coef(1, 4);
  NCollection_Array1<gp_Pnt> Pole(1, 4);
  NCollection_Array1<gp_Pnt> CoefU(1, NPolU);
  NCollection_Array1<gp_Pnt> CoefV(1, NPolV);
  Coef(4) = gp_Pnt(2., -2., 0.);
  Coef(3) = gp_Pnt(-3., 3., 0.);
  Coef(2) = gp_Pnt(0., 0., 0.);
  Coef(1) = gp_Pnt(1., 0., 0.);
  PLib::CoefficientsPoles(Coef, PLib::NoWeights(), Pole, PLib::NoWeights());
  if (NPolU > 4)
  {
    BSplCLib::IncreaseDegree(NPolU - 1, Pole, BSplCLib::NoWeights(), CoefU, BSplCLib::NoWeights());
  }
  else
  {
    CoefU = Pole;
  }
  if (NPolV > 4)
  {
    BSplCLib::IncreaseDegree(NPolV - 1, Pole, BSplCLib::NoWeights(), CoefV, BSplCLib::NoWeights());
  }
  else
  {
    CoefV = Pole;
  }
  NCollection_Array1<double> FU(2, NPolU - 1);
  NCollection_Array1<double> GU(2, NPolU - 1);
  NCollection_Array1<double> FV(2, NPolV - 1);
  NCollection_Array1<double> GV(2, NPolV - 1);
  double                     Dummy;
  for (i = 2; i < NPolU; i++)
  {
    CoefU(i).Coord(FU(i), GU(i), Dummy);
  }
  for (i = 2; i < NPolV; i++)
  {
    CoefV(i).Coord(FV(i), GV(i), Dummy);
  }

  // Clacul des poles interieurs
  gp_Pnt P;
  for (j = 2; j < NPolV; j++)
  {
    for (i = 2; i < NPolU; i++)
    {
      for (k = 1; k <= 3; k++)
      {
        P.SetCoord(
          k,
          FV(j) * (myPoles->Value(i, 1)).Coord(k) + GV(j) * (myPoles->Value(i, NPolV)).Coord(k)
            + FU(i) * (myPoles->Value(1, j)).Coord(k) + GU(i) * (myPoles->Value(NPolU, j)).Coord(k)
            - FU(i) * FV(j) * (myPoles->Value(1, 1)).Coord(k)
            - FU(i) * GV(j) * (myPoles->Value(1, NPolV)).Coord(k)
            - GU(i) * FV(j) * (myPoles->Value(NPolU, 1)).Coord(k)
            - GU(i) * GV(j) * (myPoles->Value(NPolU, NPolV)).Coord(k));
      }
      myPoles->SetValue(i, j, P);
    }
  }
}

//=================================================================================================

void GeomFill_Coons::Init(const NCollection_Array1<gp_Pnt>& P1,
                          const NCollection_Array1<gp_Pnt>& P2,
                          const NCollection_Array1<gp_Pnt>& P3,
                          const NCollection_Array1<gp_Pnt>& P4,
                          const NCollection_Array1<double>& W1,
                          const NCollection_Array1<double>& W2,
                          const NCollection_Array1<double>& W3,
                          const NCollection_Array1<double>& W4)
{

  Standard_DomainError_Raise_if(W1.Length() != W3.Length() || W2.Length() != W4.Length(), " ");
  Standard_DomainError_Raise_if(W1.Length() != P1.Length() || W2.Length() != P2.Length()
                                  || W3.Length() != P3.Length() || W4.Length() != P4.Length(),
                                " ");
  Init(P1, P2, P3, P4);
  IsRational = true;

  int NPolU = W1.Length();
  int NPolV = W2.Length();

  // #ifdef OCCT_DEBUG
  double NU = NPolU - 1;
  double NV = NPolV - 1;
  // #endif
  myWeights = new NCollection_HArray2<double>(1, NPolU, 1, NPolV);
  // The boundaries are not modified
  int i, j;
  for (i = 1; i <= NPolU; i++)
  {
    myWeights->SetValue(i, 1, W1(i));
    myWeights->SetValue(i, NPolV, W3(i));
  }
  double PU, PU1, PV, PV1;

  for (j = 2; j <= NPolV - 1; j++)
  {
    PV  = (j - 1) / NV;
    PV1 = 1 - PV;
    myWeights->SetValue(1, j, W4(j));
    myWeights->SetValue(NPolU, j, W2(j));

    for (i = 2; i <= NPolU - 1; i++)
    {
      PU  = (i - 1) / NU;
      PU1 = 1 - PU;

      //      double W = 0.5 * ( PV1 * W1(i) + PV  * W3(i) +
      //			        PU  * W2(j) + PU1 * W4(j)  );
      double W =
        PV1 * W1(i) + PV * W3(i) + PU * W2(j) + PU1 * W4(j)
        - (PU1 * PV1 * W1(1) + PU * PV1 * W2(1) + PU * PV * W3(NPolU) + PU1 * PV * W4(NPolV));

      myWeights->SetValue(i, j, W);
    }
  }
}
