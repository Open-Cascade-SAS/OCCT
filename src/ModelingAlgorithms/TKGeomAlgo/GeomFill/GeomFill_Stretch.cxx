// Created on: 1993-09-28
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

#include <GeomFill_Stretch.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//=================================================================================================

GeomFill_Stretch::GeomFill_Stretch() {}

//=================================================================================================

GeomFill_Stretch::GeomFill_Stretch(const NCollection_Array1<gp_Pnt>& P1,
                                   const NCollection_Array1<gp_Pnt>& P2,
                                   const NCollection_Array1<gp_Pnt>& P3,
                                   const NCollection_Array1<gp_Pnt>& P4)
{
  Init(P1, P2, P3, P4);
}

//=================================================================================================

GeomFill_Stretch::GeomFill_Stretch(const NCollection_Array1<gp_Pnt>&   P1,
                                   const NCollection_Array1<gp_Pnt>&   P2,
                                   const NCollection_Array1<gp_Pnt>&   P3,
                                   const NCollection_Array1<gp_Pnt>&   P4,
                                   const NCollection_Array1<double>& W1,
                                   const NCollection_Array1<double>& W2,
                                   const NCollection_Array1<double>& W3,
                                   const NCollection_Array1<double>& W4)
{
  Init(P1, P2, P3, P4, W1, W2, W3, W4);
}

//=================================================================================================

void GeomFill_Stretch::Init(const NCollection_Array1<gp_Pnt>& P1,
                            const NCollection_Array1<gp_Pnt>& P2,
                            const NCollection_Array1<gp_Pnt>& P3,
                            const NCollection_Array1<gp_Pnt>& P4)
{
  Standard_DomainError_Raise_if(P1.Length() != P3.Length() || P2.Length() != P4.Length(), " ");

  int NPolU = P1.Length();
  int NPolV = P2.Length();

  IsRational = false;

  double NU = NPolU - 1;
  double NV = NPolV - 1;
  myPoles          = new NCollection_HArray2<gp_Pnt>(1, NPolU, 1, NPolV);

  // The boundaries are not modified
  int i, j, k;
  for (i = 1; i <= NPolU; i++)
  {
    myPoles->SetValue(i, 1, P1(i));
    myPoles->SetValue(i, NPolV, P3(i));
  }
  double PU, PU1, PV, PV1;

  for (j = 2; j <= NPolV - 1; j++)
  {
    PV  = (j - 1) / NV;
    PV1 = 1 - PV;
    myPoles->SetValue(1, j, P4(j));
    myPoles->SetValue(NPolU, j, P2(j));

    for (i = 2; i <= NPolU - 1; i++)
    {
      PU  = (i - 1) / NU;
      PU1 = 1 - PU;

      gp_Pnt P;
      for (k = 1; k <= 3; k++)
      {
        P.SetCoord(k,
                   PV1 * P1(i).Coord(k) + PV * P3(i).Coord(k) + PU * P2(j).Coord(k)
                     + PU1 * P4(j).Coord(k)
                     - (PU1 * PV1 * P1(1).Coord(k) + PU * PV1 * P2(1).Coord(k)
                        + PU * PV * P3(NPolU).Coord(k) + PU1 * PV * P4(NPolV).Coord(k)));
      }
      myPoles->SetValue(i, j, P);
    }
  }
}

//=================================================================================================

void GeomFill_Stretch::Init(const NCollection_Array1<gp_Pnt>&   P1,
                            const NCollection_Array1<gp_Pnt>&   P2,
                            const NCollection_Array1<gp_Pnt>&   P3,
                            const NCollection_Array1<gp_Pnt>&   P4,
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

  double NU = NPolU - 1;
  double NV = NPolV - 1;
  myWeights        = new NCollection_HArray2<double>(1, NPolU, 1, NPolV);

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

      double W = 0.5 * (PV1 * W1(i) + PV * W3(i) + PU * W2(j) + PU1 * W4(j));

      //      double W = PV1 * W1(i) + PV  * W3(i) +
      //	                PU  * W2(j) + PU1 * W4(j) -
      //	              ( PU1 * PV1 * W1(1)     +
      //	                PU  * PV1 * W2(1)     +
      //	                PU  * PV  * W3(NPolU) +
      //	                PU1 * PV  * W4(NPolV)   );
      myWeights->SetValue(i, j, W);
    }
  }
}
