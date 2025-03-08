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

#include <GeomFill_Curved.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

//=================================================================================================

GeomFill_Curved::GeomFill_Curved() {}

//=================================================================================================

GeomFill_Curved::GeomFill_Curved(const TColgp_Array1OfPnt& P1,
                                 const TColgp_Array1OfPnt& P2,
                                 const TColgp_Array1OfPnt& P3,
                                 const TColgp_Array1OfPnt& P4)
{
  Init(P1, P2, P3, P4);
}

//=================================================================================================

GeomFill_Curved::GeomFill_Curved(const TColgp_Array1OfPnt&   P1,
                                 const TColgp_Array1OfPnt&   P2,
                                 const TColgp_Array1OfPnt&   P3,
                                 const TColgp_Array1OfPnt&   P4,
                                 const TColStd_Array1OfReal& W1,
                                 const TColStd_Array1OfReal& W2,
                                 const TColStd_Array1OfReal& W3,
                                 const TColStd_Array1OfReal& W4)
{
  Init(P1, P2, P3, P4, W1, W2, W3, W4);
}

//=================================================================================================

GeomFill_Curved::GeomFill_Curved(const TColgp_Array1OfPnt& P1, const TColgp_Array1OfPnt& P2)
{
  Init(P1, P2);
}

//=================================================================================================

GeomFill_Curved::GeomFill_Curved(const TColgp_Array1OfPnt&   P1,
                                 const TColgp_Array1OfPnt&   P2,
                                 const TColStd_Array1OfReal& W1,
                                 const TColStd_Array1OfReal& W2)
{
  Init(P1, P2, W1, W2);
}

//=================================================================================================

void GeomFill_Curved::Init(const TColgp_Array1OfPnt& P1,
                           const TColgp_Array1OfPnt& P2,
                           const TColgp_Array1OfPnt& P3,
                           const TColgp_Array1OfPnt& P4)
{
  Standard_DomainError_Raise_if(P1.Length() != P3.Length() || P2.Length() != P4.Length(), " ");

  Standard_Integer NPolU = P1.Length();
  Standard_Integer NPolV = P2.Length();

  IsRational = Standard_False;

  Standard_Real NU = NPolU - 1;
  Standard_Real NV = NPolV - 1;
  myPoles          = new TColgp_HArray2OfPnt(1, NPolU, 1, NPolV);

  // The boundaries are not modified
  Standard_Integer i, j, k;
  for (i = 1; i <= NPolU; i++)
  {
    myPoles->SetValue(i, 1, P1(i));
    myPoles->SetValue(i, NPolV, P3(i));
  }
  Standard_Real PU, PU1, PV, PV1;

  for (j = 2; j <= NPolV - 1; j++)
  {
    PV  = (j - 1) / NV;
    PV1 = 1 - PV;
    PV /= 2.;
    PV1 /= 2.;
    myPoles->SetValue(1, j, P4(j));
    myPoles->SetValue(NPolU, j, P2(j));

    for (i = 2; i <= NPolU - 1; i++)
    {
      PU  = (i - 1) / NU;
      PU1 = 1 - PU;
      PU /= 2.;
      PU1 /= 2.;
      gp_Pnt P;
      for (k = 1; k <= 3; k++)
      {
        P.SetCoord(k,
                   PV1 * P1(i).Coord(k) + PV * P3(i).Coord(k) + PU * P2(j).Coord(k)
                     + PU1 * P4(j).Coord(k));
      }
      myPoles->SetValue(i, j, P);
    }
  }
}

//=================================================================================================

void GeomFill_Curved::Init(const TColgp_Array1OfPnt&   P1,
                           const TColgp_Array1OfPnt&   P2,
                           const TColgp_Array1OfPnt&   P3,
                           const TColgp_Array1OfPnt&   P4,
                           const TColStd_Array1OfReal& W1,
                           const TColStd_Array1OfReal& W2,
                           const TColStd_Array1OfReal& W3,
                           const TColStd_Array1OfReal& W4)
{
  Standard_DomainError_Raise_if(W1.Length() != W3.Length() || W2.Length() != W4.Length(), " ");
  Standard_DomainError_Raise_if(W1.Length() != P1.Length() || W2.Length() != P2.Length()
                                  || W3.Length() != P3.Length() || W4.Length() != P4.Length(),
                                " ");

  Init(P1, P2, P3, P4);
  IsRational = Standard_True;

  Standard_Integer NPolU = W1.Length();
  Standard_Integer NPolV = W2.Length();

  Standard_Real NU = NPolU - 1;
  Standard_Real NV = NPolV - 1;
  myWeights        = new TColStd_HArray2OfReal(1, NPolU, 1, NPolV);

  // The boundaries are not modified
  Standard_Integer i, j;
  for (i = 1; i <= NPolU; i++)
  {
    myWeights->SetValue(i, 1, W1(i));
    myWeights->SetValue(i, NPolV, W3(i));
  }
  Standard_Real PU, PU1, PV, PV1;

  for (j = 2; j <= NPolV - 1; j++)
  {
    PV  = (j - 1) / NV;
    PV1 = 1 - PV;
    PV /= 2.;
    PV1 /= 2.;
    myWeights->SetValue(1, j, W4(j));
    myWeights->SetValue(NPolU, j, W2(j));

    for (i = 2; i <= NPolU - 1; i++)
    {
      PU  = (i - 1) / NU;
      PU1 = 1 - PU;
      PU /= 2.;
      PU1 /= 2.;
      Standard_Real W = PV1 * W1(i) + PV * W3(i) + PU * W2(j) + PU1 * W4(j);
      myWeights->SetValue(i, j, W);
    }
  }
}

//=================================================================================================

void GeomFill_Curved::Init(const TColgp_Array1OfPnt& P1, const TColgp_Array1OfPnt& P2)
{
  Standard_Integer NPolU = P1.Length();
  Standard_Integer NPolV = P2.Length();

  IsRational = Standard_False;

  myPoles = new TColgp_HArray2OfPnt(1, NPolU, 1, NPolV);

  Standard_Integer i, j;

  for (j = 1; j <= NPolV; j++)
  {
    gp_Vec Tra(P2(1), P2(j));
    for (i = 1; i <= NPolU; i++)
    {
      myPoles->SetValue(i, j, P1(i).Translated(Tra));
    }
  }
}

//=================================================================================================

void GeomFill_Curved::Init(const TColgp_Array1OfPnt&   P1,
                           const TColgp_Array1OfPnt&   P2,
                           const TColStd_Array1OfReal& W1,
                           const TColStd_Array1OfReal& W2)
{
  Init(P1, P2);
  IsRational = Standard_True;

  // Initialisation des poids.
  Standard_Integer NPolU = W1.Length();
  Standard_Integer NPolV = W2.Length();

  myWeights = new TColStd_HArray2OfReal(1, NPolU, 1, NPolV);
  for (Standard_Integer j = 1; j <= NPolV; j++)
  {
    Standard_Real Factor = W2(j) / W1(1);
    for (Standard_Integer i = 1; i <= NPolU; i++)
    {
      myWeights->SetValue(i, j, W1(i) * Factor);
    }
  }
}
