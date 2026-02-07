// Created on: 1993-03-09
// Created by: JCV
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

// Passage en classe persistante - 23/01/91
// Modif suite a la deuxieme revue de projet toolkit Geometry -23/01/91
//  pmn : 21/10/95 ; Correction de la methode segment (PRO5853)
//  pmn : 31-Dec-96; Bonne gestion des poids (bug PRO4622)
//  xab : 07-Jul-97; le cache est instable en degree 21
//        a partir du degree 15 on ne l'utilise plus
//  RBD : 15/10/98 ; Le cache est desormais defini sur [-1,1] (pro15537).
//  pmn : 10/12/98 ; Update de la methode segment (suite a la modif de cache).

#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <BSplCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_HArray2.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BezierSurface, Geom_BoundedSurface)

namespace
{
static const double THE_BEZIER_KNOTS[2] = {0.0, 1.0};
static const int    THE_BEZIER_MULTS[26][2] = {
  {1, 1},   {2, 2},   {3, 3},   {4, 4},   {5, 5},   {6, 6},   {7, 7},
  {8, 8},   {9, 9},   {10, 10}, {11, 11}, {12, 12}, {13, 13}, {14, 14},
  {15, 15}, {16, 16}, {17, 17}, {18, 18}, {19, 19}, {20, 20}, {21, 21},
  {22, 22}, {23, 23}, {24, 24}, {25, 25}, {26, 26}};
} // namespace

//=======================================================================
// function : Rational
// purpose  : check rationality of an array of weights
//=======================================================================
static void Rational(const NCollection_Array2<double>& Weights, bool& Urational, bool& Vrational)
{
  int I, J;
  J         = Weights.LowerCol();
  Vrational = false;
  while (!Vrational && J <= Weights.UpperCol())
  {
    I = Weights.LowerRow();
    while (!Vrational && I <= Weights.UpperRow() - 1)
    {
      Vrational = (std::abs(Weights(I, J) - Weights(I + 1, J)) > Epsilon(std::abs(Weights(I, J))));
      I++;
    }
    J++;
  }

  I         = Weights.LowerRow();
  Urational = false;
  while (!Urational && I <= Weights.UpperRow())
  {
    J = Weights.LowerCol();
    while (!Urational && J <= Weights.UpperCol() - 1)
    {
      Urational = (std::abs(Weights(I, J) - Weights(I, J + 1)) > Epsilon(std::abs(Weights(I, J))));
      J++;
    }
    I++;
  }
}

//=================================================================================================

static void AddPoleCol(const NCollection_Array2<gp_Pnt>& Poles,
                       const NCollection_Array1<gp_Pnt>& PoleCol,
                       const int                         AfterIndex,
                       NCollection_Array2<gp_Pnt>&       NewPoles)
{
  int InsertIndex = AfterIndex + NewPoles.LowerCol();
  int Offset      = NewPoles.LowerRow() - PoleCol.Lower();
  int ColIndex    = NewPoles.LowerCol();
  int RowIndex;
  while (ColIndex < InsertIndex)
  {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow())
    {
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex, ColIndex);
      RowIndex++;
    }
    ColIndex++;
  }
  RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow())
  {
    NewPoles(RowIndex, ColIndex) = PoleCol(RowIndex - Offset);
    RowIndex++;
  }
  ColIndex++;
  while (ColIndex <= NewPoles.UpperCol())
  {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow())
    {
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex, ColIndex - 1);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=================================================================================================

static void AddRatPoleCol(const NCollection_Array2<gp_Pnt>& Poles,
                          const NCollection_Array2<double>& Weights,
                          const NCollection_Array1<gp_Pnt>& PoleCol,
                          const NCollection_Array1<double>& PoleWeightCol,
                          const int                         AfterIndex,
                          NCollection_Array2<gp_Pnt>&       NewPoles,
                          NCollection_Array2<double>&       NewWeights)
{
  int InsertIndex = AfterIndex + NewPoles.LowerCol();
  int OffsetPol   = NewPoles.LowerRow() - PoleCol.Lower();
  int OffsetW     = NewWeights.LowerRow() - PoleWeightCol.Lower();

  int ColIndex = NewPoles.LowerCol();
  int RowIndex;
  while (ColIndex < InsertIndex)
  {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow())
    {
      NewPoles(RowIndex, ColIndex)   = Poles(RowIndex, ColIndex);
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex, ColIndex);
      RowIndex++;
    }
    ColIndex++;
  }
  RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow())
  {
    NewPoles(RowIndex, ColIndex)   = PoleCol(RowIndex - OffsetPol);
    NewWeights(RowIndex, ColIndex) = PoleWeightCol(RowIndex - OffsetW);
    RowIndex++;
  }
  ColIndex++;
  while (ColIndex <= NewPoles.UpperCol())
  {
    RowIndex = NewPoles.LowerRow();
    while (RowIndex <= NewPoles.UpperRow())
    {
      NewPoles(RowIndex, ColIndex)   = Poles(RowIndex, ColIndex - 1);
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex, ColIndex - 1);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=================================================================================================

static void AddPoleRow(const NCollection_Array2<gp_Pnt>& Poles,
                       const NCollection_Array1<gp_Pnt>& PoleRow,
                       const int                         AfterIndex,
                       NCollection_Array2<gp_Pnt>&       NewPoles)
{
  int InsertIndex = AfterIndex + NewPoles.LowerRow();
  int Offset      = NewPoles.LowerCol() - PoleRow.Lower();
  int RowIndex    = NewPoles.LowerRow();
  int ColIndex;
  while (RowIndex < InsertIndex)
  {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol())
    {
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
  ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol())
  {
    NewPoles(RowIndex, ColIndex) = PoleRow(ColIndex - Offset);
    ColIndex++;
  }
  RowIndex++;
  while (RowIndex <= NewPoles.UpperRow())
  {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol())
    {
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex - 1, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=================================================================================================

static void AddRatPoleRow(const NCollection_Array2<gp_Pnt>& Poles,
                          const NCollection_Array2<double>& Weights,
                          const NCollection_Array1<gp_Pnt>& PoleRow,
                          const NCollection_Array1<double>& PoleWeightRow,
                          const int                         AfterIndex,
                          NCollection_Array2<gp_Pnt>&       NewPoles,
                          NCollection_Array2<double>&       NewWeights)
{
  int InsertIndex = AfterIndex + NewPoles.LowerRow();
  int OffsetPol   = NewPoles.LowerCol() - PoleRow.Lower();
  int OffsetW     = NewWeights.LowerCol() - PoleWeightRow.Lower();

  int ColIndex;
  int RowIndex = NewPoles.LowerRow();
  while (RowIndex < InsertIndex)
  {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol())
    {
      NewPoles(RowIndex, ColIndex)   = Poles(RowIndex, ColIndex);
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
  ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol())
  {
    NewPoles(RowIndex, ColIndex)   = PoleRow(ColIndex - OffsetPol);
    NewWeights(RowIndex, ColIndex) = PoleWeightRow(ColIndex - OffsetW);
    ColIndex++;
  }
  RowIndex++;
  while (RowIndex <= NewPoles.UpperRow())
  {
    ColIndex = NewPoles.LowerCol();
    while (ColIndex <= NewPoles.UpperCol())
    {
      NewPoles(RowIndex, ColIndex)   = Poles(RowIndex - 1, ColIndex);
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex - 1, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=================================================================================================

static void DeletePoleCol(const NCollection_Array2<gp_Pnt>& Poles,
                          const int                         Index,
                          NCollection_Array2<gp_Pnt>&       NewPoles)
{
  int Offset = 0;
  int RowIndex;
  int ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol())
  {
    RowIndex = NewPoles.LowerRow();
    if (ColIndex == Index)
      Offset = 1;
    while (RowIndex <= NewPoles.UpperRow())
    {
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex, ColIndex + Offset);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=================================================================================================

static void DeleteRatPoleCol(const NCollection_Array2<gp_Pnt>& Poles,
                             const NCollection_Array2<double>& Weights,
                             const int                         Index,
                             NCollection_Array2<gp_Pnt>&       NewPoles,
                             NCollection_Array2<double>&       NewWeights)
{
  int Offset = 0;
  int RowIndex;
  int ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol())
  {
    RowIndex = NewPoles.LowerRow();
    if (ColIndex == Index)
      Offset = 1;
    while (RowIndex <= NewPoles.UpperRow())
    {
      NewPoles(RowIndex, ColIndex)   = Poles(RowIndex, ColIndex + Offset);
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex, ColIndex + Offset);
      RowIndex++;
    }
    ColIndex++;
  }
}

//=================================================================================================

static void DeletePoleRow(const NCollection_Array2<gp_Pnt>& Poles,
                          const int                         Index,
                          NCollection_Array2<gp_Pnt>&       NewPoles)
{
  int Offset = 0;
  int ColIndex;
  int RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow())
  {
    ColIndex = NewPoles.LowerCol();
    if (RowIndex == Index)
      Offset = 1;
    while (ColIndex <= NewPoles.UpperCol())
    {
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex + Offset, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=================================================================================================

static void DeleteRatPoleRow(const NCollection_Array2<gp_Pnt>& Poles,
                             const NCollection_Array2<double>& Weights,
                             const int                         Index,
                             NCollection_Array2<gp_Pnt>&       NewPoles,
                             NCollection_Array2<double>&       NewWeights)
{
  int Offset = 0;
  int ColIndex;
  int RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow())
  {
    ColIndex = NewPoles.LowerCol();
    if (RowIndex == Index)
      Offset = 1;
    while (ColIndex <= NewPoles.UpperCol())
    {
      NewPoles(RowIndex, ColIndex)   = Poles(RowIndex + Offset, ColIndex);
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex + Offset, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}

//=================================================================================================

void Geom_BezierSurface::updateUKnots()
{
  const int aDeg = myData.UDegree;
  // Non-owning wrappers around static data — zero allocation
  myData.UKnots     = NCollection_Array1<double>(THE_BEZIER_KNOTS[0], 1, 2);
  myData.UMults     = NCollection_Array1<int>(THE_BEZIER_MULTS[aDeg][0], 1, 2);
  myData.UFlatKnots = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(aDeg), 1, 2 * (aDeg + 1));
  myData.IsUPeriodic = false;
}

//=================================================================================================

void Geom_BezierSurface::updateVKnots()
{
  const int aDeg = myData.VDegree;
  // Non-owning wrappers around static data — zero allocation
  myData.VKnots     = NCollection_Array1<double>(THE_BEZIER_KNOTS[0], 1, 2);
  myData.VMults     = NCollection_Array1<int>(THE_BEZIER_MULTS[aDeg][0], 1, 2);
  myData.VFlatKnots = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(aDeg), 1, 2 * (aDeg + 1));
  myData.IsVPeriodic = false;
}

//=================================================================================================

Geom_BezierSurface::Geom_BezierSurface(const Geom_BezierSurface& theOther)
    : myData(theOther.myData),
      urational(theOther.urational),
      vrational(theOther.vrational),
      umaxderivinv(theOther.umaxderivinv),
      vmaxderivinv(theOther.vmaxderivinv),
      maxderivinvok(false)
{
  // Restore non-owning knot/mult/flatknot references to static data
  updateUKnots();
  updateVKnots();
}

//=================================================================================================

Geom_BezierSurface::Geom_BezierSurface(const NCollection_Array2<gp_Pnt>& SurfacePoles)
    : maxderivinvok(false)
{
  int NbUPoles = SurfacePoles.ColLength();
  int NbVPoles = SurfacePoles.RowLength();
  if (NbUPoles < 2 || NbUPoles > MaxDegree() + 1 || NbVPoles < 2 || NbVPoles > MaxDegree() + 1)
  {
    throw Standard_ConstructionError();
  }

  urational = false;
  vrational = false;

  // Init non rational
  Init(SurfacePoles, nullptr);
}

//=================================================================================================

Geom_BezierSurface::Geom_BezierSurface(const NCollection_Array2<gp_Pnt>& SurfacePoles,
                                       const NCollection_Array2<double>& PoleWeights)
    : maxderivinvok(false)
{
  int NbUPoles = SurfacePoles.ColLength();
  int NbVPoles = SurfacePoles.RowLength();
  if (NbUPoles < 2 || NbUPoles > MaxDegree() + 1 || NbVPoles < 2 || NbVPoles > MaxDegree() + 1
      || NbVPoles != PoleWeights.RowLength() || NbUPoles != PoleWeights.ColLength())
  {
    throw Standard_ConstructionError();
  }

  int Col = PoleWeights.LowerCol();
  while (Col <= PoleWeights.UpperCol())
  {
    int Row = PoleWeights.LowerRow();
    while (Row <= PoleWeights.UpperRow())
    {
      if (PoleWeights(Row, Col) <= gp::Resolution())
      {
        throw Standard_ConstructionError();
      }
      Row++;
    }
    Col++;
  }

  int I, J;
  urational = false;
  vrational = false;
  J         = PoleWeights.LowerCol();
  while (!vrational && J <= PoleWeights.UpperCol())
  {
    I = PoleWeights.LowerRow();
    while (!vrational && I <= PoleWeights.UpperRow() - 1)
    {
      vrational = (std::abs(PoleWeights(I, J) - PoleWeights(I + 1, J))
                   > Epsilon(std::abs(PoleWeights(I, J))));
      I++;
    }
    J++;
  }
  I = PoleWeights.LowerRow();
  while (!urational && I <= PoleWeights.UpperRow())
  {
    J = PoleWeights.LowerCol();
    while (!urational && J <= PoleWeights.UpperCol() - 1)
    {
      urational = (std::abs(PoleWeights(I, J) - PoleWeights(I, J + 1))
                   > Epsilon(std::abs(PoleWeights(I, J))));
      J++;
    }
    I++;
  }

  if (urational || vrational)
  {
    Init(SurfacePoles, &PoleWeights);
  }
  else
  {
    Init(SurfacePoles, nullptr);
  }
}

//=================================================================================================

int Geom_BezierSurface::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom_BezierSurface::ExchangeUV()
{
  int LR = myData.Poles.LowerRow();
  int UR = myData.Poles.UpperRow();
  int LC = myData.Poles.LowerCol();
  int UC = myData.Poles.UpperCol();

  NCollection_Array2<gp_Pnt> npoles(LC, UC, LR, UR);
  NCollection_Array2<double> nweights;
  bool                       rat = (urational || vrational);
  if (rat)
  {
    nweights.Resize(LC, UC, LR, UR, false);
  }

  for (int i = LC; i <= UC; i++)
  {
    for (int j = LR; j <= UR; j++)
    {
      npoles(i, j) = myData.Poles(j, i);
      if (rat)
      {
        nweights(i, j) = myData.Weights(j, i);
      }
    }
  }
  myData.Poles = std::move(npoles);
  if (rat)
  {
    myData.Weights = std::move(nweights);
  }

  std::swap(urational, vrational);
  std::swap(myData.UDegree, myData.VDegree);
  updateUKnots();
  updateVKnots();
}

//=================================================================================================

void Geom_BezierSurface::Increase(const int UDeg, const int VDeg)
{
  if (UDeg < UDegree() || UDeg > Geom_BezierSurface::MaxDegree() || VDeg < VDegree()
      || VDeg > Geom_BezierSurface::MaxDegree())
  {
    throw Standard_ConstructionError();
  }

  int oldUDeg = UDegree();
  int oldVDeg = VDegree();
  int IncUDeg = UDeg - oldUDeg;
  int IncVDeg = VDeg - oldVDeg;
  if (IncUDeg == 0 && IncVDeg == 0)
    return;

  if (IncUDeg > 0)
  {
    NCollection_Array2<gp_Pnt> npoles(1, UDeg + 1, 1, oldVDeg + 1);
    double                     aKnotsBuf[2];
    int                        aMultsBuf[2];
    NCollection_Array1<double> nknots(aKnotsBuf[0], 1, 2);
    NCollection_Array1<int>    nmults(aMultsBuf[0], 1, 2);

    if (urational || vrational)
    {
      NCollection_Array2<double> nweights(1, UDeg + 1, 1, VDegree() + 1);

      BSplSLib::IncreaseDegree(true,
                               oldUDeg,
                               UDeg,
                               false,
                               myData.Poles,
                               &myData.Weights,
                               myData.UKnots,
                               myData.UMults,
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
      myData.Weights = std::move(nweights);
    }
    else
    {
      BSplSLib::IncreaseDegree(true,
                               oldUDeg,
                               UDeg,
                               false,
                               myData.Poles,
                               BSplSLib::NoWeights(),
                               myData.UKnots,
                               myData.UMults,
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
    }
    myData.Poles = std::move(npoles);
  }
  if (IncVDeg > 0)
  {
    NCollection_Array2<gp_Pnt> npoles(1, UDeg + 1, 1, VDeg + 1);
    double                     aKnotsBuf[2];
    int                        aMultsBuf[2];
    NCollection_Array1<double> nknots(aKnotsBuf[0], 1, 2);
    NCollection_Array1<int>    nmults(aMultsBuf[0], 1, 2);

    if (urational || vrational)
    {
      NCollection_Array2<double> nweights(1, UDeg + 1, 1, VDeg + 1);

      BSplSLib::IncreaseDegree(false,
                               oldVDeg,
                               VDeg,
                               false,
                               myData.Poles,
                               &myData.Weights,
                               myData.VKnots,
                               myData.VMults,
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
      myData.Weights = std::move(nweights);
    }
    else
    {
      BSplSLib::IncreaseDegree(false,
                               oldVDeg,
                               VDeg,
                               false,
                               myData.Poles,
                               BSplSLib::NoWeights(),
                               myData.VKnots,
                               myData.VMults,
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
    }
    myData.Poles = std::move(npoles);
  }
  Init(myData.Poles, (urational || vrational) ? &myData.Weights : nullptr);
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColAfter(const int                         VIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myData.Poles.ColLength())
  {
    throw Standard_ConstructionError();
  }

  int NbUPoles = myData.Poles.ColLength();
  int NbVPoles = myData.Poles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles, 1, NbVPoles + 1);

  if (urational || vrational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles, 1, NbVPoles + 1);

    NCollection_Array1<double> CWeights(1, NbUPoles);
    CWeights.Init(1.);

    AddRatPoleCol(myData.Poles,
                  myData.Weights,
                  CPoles,
                  CWeights,
                  VIndex,
                  npoles,
                  nweights);
    myData.Weights = std::move(nweights);
  }
  else
  {
    AddPoleCol(myData.Poles, CPoles, VIndex, npoles);
  }
  myData.Poles = std::move(npoles);
  myData.VDegree = myData.Poles.RowLength() - 1;
  updateVKnots();
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColAfter(const int                         VIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles,
                                            const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myData.Poles.ColLength() || CPoleWeights.Length() != CPoles.Length())
  {
    throw Standard_ConstructionError();
  }
  int Index = CPoleWeights.Lower();
  while (Index <= CPoleWeights.Upper())
  {
    if (CPoleWeights(Index) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
    Index++;
  }

  int NbUPoles = myData.Poles.ColLength();
  int NbVPoles = myData.Poles.RowLength();

  // Ensure weights exist for rational insertion
  if (!(urational || vrational))
  {
    myData.Weights.Resize(1, NbUPoles, 1, NbVPoles, false);
    myData.Weights.Init(1.0);
  }

  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles, 1, NbVPoles + 1);
  NCollection_Array2<double> nweights(1, NbUPoles, 1, NbVPoles + 1);

  AddRatPoleCol(myData.Poles,
                myData.Weights,
                CPoles,
                CPoleWeights,
                VIndex,
                npoles,
                nweights);

  myData.Poles = std::move(npoles);
  myData.Weights = std::move(nweights);

  Rational(myData.Weights, urational, vrational);
  myData.VDegree = myData.Poles.RowLength() - 1;
  updateVKnots();
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColBefore(const int                         VIndex,
                                             const NCollection_Array1<gp_Pnt>& CPoles)
{
  InsertPoleColAfter(VIndex - 1, CPoles);
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColBefore(const int                         VIndex,
                                             const NCollection_Array1<gp_Pnt>& CPoles,
                                             const NCollection_Array1<double>& CPoleWeights)
{
  InsertPoleColAfter(VIndex - 1, CPoles, CPoleWeights);
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleRowAfter(const int                         UIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (UIndex < 1 || UIndex > myData.Poles.ColLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myData.Poles.RowLength())
  {
    throw Standard_ConstructionError();
  }

  int NbUPoles = myData.Poles.ColLength();
  int NbVPoles = myData.Poles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles + 1, 1, NbVPoles);

  if (urational || vrational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles + 1, 1, NbVPoles);

    NCollection_Array1<double> CWeights(1, NbVPoles);
    CWeights.Init(1.0);

    AddRatPoleRow(myData.Poles,
                  myData.Weights,
                  CPoles,
                  CWeights,
                  UIndex,
                  npoles,
                  nweights);
    myData.Weights = std::move(nweights);
  }
  else
  {
    AddPoleRow(myData.Poles, CPoles, UIndex, npoles);
  }
  myData.Poles = std::move(npoles);
  myData.UDegree = myData.Poles.ColLength() - 1;
  updateUKnots();
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleRowAfter(const int                         UIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles,
                                            const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myData.Poles.ColLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myData.Poles.RowLength() || CPoleWeights.Length() != CPoles.Length())
  {
    throw Standard_ConstructionError();
  }
  int Index = CPoleWeights.Lower();
  while (Index <= CPoleWeights.Upper())
  {
    if (CPoleWeights(Index) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
    Index++;
  }

  int NbUPoles = myData.Poles.ColLength();
  int NbVPoles = myData.Poles.RowLength();

  // Ensure weights exist for rational insertion
  if (!(urational || vrational))
  {
    myData.Weights.Resize(1, NbUPoles, 1, NbVPoles, false);
    myData.Weights.Init(1.0);
  }

  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles + 1, 1, NbVPoles);
  NCollection_Array2<double> nweights(1, NbUPoles + 1, 1, NbVPoles);

  AddRatPoleRow(myData.Poles,
                myData.Weights,
                CPoles,
                CPoleWeights,
                UIndex,
                npoles,
                nweights);

  myData.Poles = std::move(npoles);
  myData.Weights = std::move(nweights);

  Rational(myData.Weights, urational, vrational);
  myData.UDegree = myData.Poles.ColLength() - 1;
  updateUKnots();
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleRowBefore(const int                         UIndex,
                                             const NCollection_Array1<gp_Pnt>& CPoles)
{
  InsertPoleRowAfter(UIndex - 1, CPoles);
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleRowBefore(const int                         UIndex,
                                             const NCollection_Array1<gp_Pnt>& CPoles,
                                             const NCollection_Array1<double>& CPoleWeights)
{
  InsertPoleRowAfter(UIndex - 1, CPoles, CPoleWeights);
}

//=================================================================================================

void Geom_BezierSurface::RemovePoleCol(const int VIndex)
{
  if (VIndex < 1 || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange();
  if (myData.Poles.RowLength() <= 2)
    throw Standard_ConstructionError();

  int NbUPoles = myData.Poles.ColLength();
  int NbVPoles = myData.Poles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles, 1, NbVPoles - 1);

  if (urational || vrational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles, 1, NbVPoles - 1);

    DeleteRatPoleCol(myData.Poles,
                     myData.Weights,
                     VIndex,
                     npoles,
                     nweights);
    Rational(nweights, urational, vrational);
    if (urational || vrational)
      myData.Weights = std::move(nweights);
  }
  else
  {
    DeletePoleCol(myData.Poles, VIndex, npoles);
  }
  myData.Poles = std::move(npoles);
  myData.VDegree = myData.Poles.RowLength() - 1;
  updateVKnots();
}

//=================================================================================================

void Geom_BezierSurface::RemovePoleRow(const int UIndex)
{
  if (UIndex < 1 || UIndex > myData.Poles.ColLength())
    throw Standard_OutOfRange();
  if (myData.Poles.ColLength() <= 2)
    throw Standard_ConstructionError();

  int NbUPoles = myData.Poles.ColLength();
  int NbVPoles = myData.Poles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles - 1, 1, NbVPoles);

  if (urational || vrational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles - 1, 1, NbVPoles);

    DeleteRatPoleRow(myData.Poles,
                     myData.Weights,
                     UIndex,
                     npoles,
                     nweights);

    Rational(nweights, urational, vrational);
    if (urational || vrational)
      myData.Weights = std::move(nweights);
  }
  else
  {
    DeletePoleRow(myData.Poles, UIndex, npoles);
  }
  myData.Poles = std::move(npoles);
  myData.UDegree = myData.Poles.ColLength() - 1;
  updateUKnots();
}

//=================================================================================================

void Geom_BezierSurface::Segment(const double U1, const double U2, const double V1, const double V2)
{
  bool rat = (urational || vrational);

  int aMinDegree = UDegree() <= VDegree() ? UDegree() : VDegree();
  int aMaxDegree = UDegree() > VDegree() ? UDegree() : VDegree();

  occ::handle<NCollection_HArray2<gp_Pnt>> Coefs =
    new NCollection_HArray2<gp_Pnt>(1, aMaxDegree + 1, 1, aMinDegree + 1);
  occ::handle<NCollection_HArray2<double>> WCoefs;
  if (rat)
    WCoefs = new NCollection_HArray2<double>(1, aMaxDegree + 1, 1, aMinDegree + 1);

  double uparameter_11  = 0.5;
  double uspanlenght_11 = 0.5;
  double vparameter_11  = 0.5;
  double vspanlenght_11 = 0.5;

  if (rat)
  {
    BSplSLib::BuildCache(uparameter_11,
                         vparameter_11,
                         uspanlenght_11,
                         vspanlenght_11,
                         false,
                         false,
                         UDegree(),
                         VDegree(),
                         0,
                         0,
                         myData.UFlatKnots,
                         myData.VFlatKnots,
                         myData.Poles,
                         &myData.Weights,
                         Coefs->ChangeArray2(),
                         &WCoefs->ChangeArray2());
  }
  else
  {
    BSplSLib::BuildCache(uparameter_11,
                         vparameter_11,
                         uspanlenght_11,
                         vspanlenght_11,
                         false,
                         false,
                         UDegree(),
                         VDegree(),
                         0,
                         0,
                         myData.UFlatKnots,
                         myData.VFlatKnots,
                         myData.Poles,
                         BSplSLib::NoWeights(),
                         Coefs->ChangeArray2(),
                         BSplSLib::NoWeights());
  }

  // Attention si udeg <= vdeg u et v sont intervertis
  // dans les coeffs, il faut donc tout transposer.
  if (UDegree() <= VDegree())
  {
    occ::handle<NCollection_HArray2<gp_Pnt>> coeffs  = Coefs;
    occ::handle<NCollection_HArray2<double>> wcoeffs = WCoefs;
    int                                      ii, jj;
    Coefs = new (NCollection_HArray2<gp_Pnt>)(1, UDegree() + 1, 1, VDegree() + 1);
    if (rat)
    {
      WCoefs = new (NCollection_HArray2<double>)(1, UDegree() + 1, 1, VDegree() + 1);
    }
    for (ii = 1; ii <= UDegree() + 1; ii++)
      for (jj = 1; jj <= VDegree() + 1; jj++)
      {
        Coefs->SetValue(ii, jj, coeffs->Value(jj, ii));
        if (rat)
          WCoefs->SetValue(ii, jj, wcoeffs->Value(jj, ii));
      }
  }

  // Trim dans la base cannonique et Update des Poles et Coeffs

  // PMN : tranfo sur les parametres
  double ufirst = 2 * (U1 - 0.5), ulast = 2 * (U2 - 0.5), vfirst = 2 * (V1 - 0.5),
         vlast = 2 * (V2 - 0.5);
  if (rat)
  {
    PLib::UTrimming(ufirst, ulast, Coefs->ChangeArray2(), &WCoefs->ChangeArray2());
    PLib::VTrimming(vfirst, vlast, Coefs->ChangeArray2(), &WCoefs->ChangeArray2());
    PLib::CoefficientsPoles(Coefs->Array2(),
                            &WCoefs->Array2(),
                            myData.Poles,
                            &myData.Weights);
  }
  else
  {
    PLib::UTrimming(ufirst, ulast, Coefs->ChangeArray2(), PLib::NoWeights2());
    PLib::VTrimming(vfirst, vlast, Coefs->ChangeArray2(), PLib::NoWeights2());
    PLib::CoefficientsPoles(Coefs->Array2(),
                            PLib::NoWeights2(),
                            myData.Poles,
                            PLib::NoWeights2());
  }
}

//=================================================================================================

void Geom_BezierSurface::SetPole(const int UIndex, const int VIndex, const gp_Pnt& P)
{
  if (UIndex < 1 || UIndex > myData.Poles.ColLength() || VIndex < 1
      || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange();

  myData.Poles(UIndex, VIndex) = P;
}

//=================================================================================================

void Geom_BezierSurface::SetPole(const int     UIndex,
                                 const int     VIndex,
                                 const gp_Pnt& P,
                                 const double  Weight)
{

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierSurface::SetPole");
  if (UIndex < 1 || UIndex > myData.Poles.ColLength() || VIndex < 1
      || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange("Geom_BezierSurface::SetPole");

  myData.Poles(UIndex, VIndex) = P;

  SetWeight(UIndex, VIndex, Weight);
}

//=================================================================================================

void Geom_BezierSurface::SetPoleCol(const int                         VIndex,
                                    const NCollection_Array1<gp_Pnt>& CPoles,
                                    const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myData.Poles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myData.Poles.ColLength() || CPoleWeights.Lower() != CPoles.Lower()
      || CPoleWeights.Upper() != CPoles.Upper())
  {
    throw Standard_ConstructionError();
  }

  int I;
  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myData.Poles(I, VIndex) = CPoles(I);
  }
  SetWeightCol(VIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BezierSurface::SetPoleCol(const int VIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > myData.Poles.RowLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myData.Poles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myData.Poles.ColLength())
  {
    throw Standard_ConstructionError();
  }
  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myData.Poles(I, VIndex) = CPoles(I);
  }
}

//=================================================================================================

void Geom_BezierSurface::SetPoleRow(const int UIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (UIndex < 1 || UIndex > myData.Poles.ColLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myData.Poles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myData.Poles.RowLength())
    throw Standard_ConstructionError();

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myData.Poles(UIndex, I) = CPoles(I);
  }
}

//=================================================================================================

void Geom_BezierSurface::SetPoleRow(const int                         UIndex,
                                    const NCollection_Array1<gp_Pnt>& CPoles,
                                    const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myData.Poles.ColLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myData.Poles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myData.Poles.RowLength() || CPoleWeights.Lower() != CPoles.Lower()
      || CPoleWeights.Upper() != CPoles.Upper())
  {
    throw Standard_ConstructionError();
  }

  int I;

  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myData.Poles(UIndex, I) = CPoles(I);
  }

  SetWeightRow(UIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BezierSurface::SetWeight(const int UIndex, const int VIndex, const double Weight)
{
  // compute new rationality
  bool wasrat = (urational || vrational);
  if (!wasrat)
  {
    // a weight of 1. does not turn to rational
    if (std::abs(Weight - 1.) <= gp::Resolution())
      return;

    // set weights of 1.
    myData.Weights.Resize(1, myData.Poles.ColLength(), 1, myData.Poles.RowLength(), false);
    myData.Weights.Init(1.);
  }

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeight");

  if (UIndex < 1 || UIndex > myData.Weights.ColLength() || VIndex < 1
      || VIndex > myData.Weights.RowLength())
    throw Standard_OutOfRange();

  if (std::abs(Weight - myData.Weights(UIndex, VIndex)) > gp::Resolution())
  {
    myData.Weights(UIndex, VIndex) = Weight;
    Rational(myData.Weights, urational, vrational);
    if (!(urational || vrational))
    {
      myData.Weights = NCollection_Array2<double>();
    }
  }
}

//=================================================================================================

void Geom_BezierSurface::SetWeightCol(const int                         VIndex,
                                      const NCollection_Array1<double>& CPoleWeights)
{
  int I;
  // compute new rationality
  bool wasrat = (urational || vrational);
  if (!wasrat)
  {
    // set weights of 1.
    myData.Weights.Resize(1, myData.Poles.ColLength(), 1, myData.Poles.RowLength(), false);
    myData.Weights.Init(1.);
  }

  if (VIndex < 1 || VIndex > myData.Weights.RowLength())
    throw Standard_OutOfRange();

  if (CPoleWeights.Length() != myData.Weights.ColLength())
  {
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeightCol");
  }

  I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
    myData.Weights(I, VIndex) = CPoleWeights(I);
    I++;
  }

  Rational(myData.Weights, urational, vrational);
  if (!(urational || vrational))
  {
    myData.Weights = NCollection_Array2<double>();
  }
}

//=================================================================================================

void Geom_BezierSurface::SetWeightRow(const int                         UIndex,
                                      const NCollection_Array1<double>& CPoleWeights)
{
  int I;
  // compute new rationality
  bool wasrat = (urational || vrational);
  if (!wasrat)
  {
    // set weights of 1.
    myData.Weights.Resize(1, myData.Poles.ColLength(), 1, myData.Poles.RowLength(), false);
    myData.Weights.Init(1.);
  }

  if (UIndex < 1 || UIndex > myData.Weights.ColLength())
    throw Standard_OutOfRange("Geom_BezierSurface::SetWeightRow");
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > myData.Weights.RowLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > myData.Weights.RowLength())
  {
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeightRow");
  }

  I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
    myData.Weights(UIndex, I) = CPoleWeights(I);
    I++;
  }

  Rational(myData.Weights, urational, vrational);
  if (!(urational || vrational))
  {
    myData.Weights = NCollection_Array2<double>();
  }
}

//=================================================================================================

void Geom_BezierSurface::UReverse()
{
  gp_Pnt Pol;
  int    Row, Col;
  int    NbUPoles = myData.Poles.ColLength();
  int    NbVPoles = myData.Poles.RowLength();
  if (urational || vrational)
  {
    double W;
    for (Col = 1; Col <= NbVPoles; Col++)
    {
      for (Row = 1; Row <= NbUPoles / 2; Row++)
      {
        W                                       = myData.Weights(Row, Col);
        myData.Weights(Row, Col)                = myData.Weights(NbUPoles - Row + 1, Col);
        myData.Weights(NbUPoles - Row + 1, Col) = W;
        Pol                                     = myData.Poles(Row, Col);
        myData.Poles(Row, Col)                   = myData.Poles(NbUPoles - Row + 1, Col);
        myData.Poles(NbUPoles - Row + 1, Col)    = Pol;
      }
    }
  }
  else
  {
    for (Col = 1; Col <= NbVPoles; Col++)
    {
      for (Row = 1; Row <= NbUPoles / 2; Row++)
      {
        Pol                                   = myData.Poles(Row, Col);
        myData.Poles(Row, Col)                = myData.Poles(NbUPoles - Row + 1, Col);
        myData.Poles(NbUPoles - Row + 1, Col) = Pol;
      }
    }
  }
}

//=================================================================================================

double Geom_BezierSurface::UReversedParameter(const double U) const
{
  return (1. - U);
}

//=================================================================================================

void Geom_BezierSurface::VReverse()
{
  gp_Pnt Pol;
  int    Row, Col;
  int    NbUPoles = myData.Poles.ColLength();
  int    NbVPoles = myData.Poles.RowLength();
  if (urational || vrational)
  {
    double W;
    for (Row = 1; Row <= NbUPoles; Row++)
    {
      for (Col = 1; Col <= NbVPoles / 2; Col++)
      {
        W                                       = myData.Weights(Row, Col);
        myData.Weights(Row, Col)                = myData.Weights(Row, NbVPoles - Col + 1);
        myData.Weights(Row, NbVPoles - Col + 1) = W;
        Pol                                     = myData.Poles(Row, Col);
        myData.Poles(Row, Col)                   = myData.Poles(Row, NbVPoles - Col + 1);
        myData.Poles(Row, NbVPoles - Col + 1)    = Pol;
      }
    }
  }
  else
  {
    for (Row = 1; Row <= NbUPoles; Row++)
    {
      for (Col = 1; Col <= NbVPoles / 2; Col++)
      {
        Pol                                   = myData.Poles(Row, Col);
        myData.Poles(Row, Col)                = myData.Poles(Row, NbVPoles - Col + 1);
        myData.Poles(Row, NbVPoles - Col + 1) = Pol;
      }
    }
  }
}

//=================================================================================================

double Geom_BezierSurface::VReversedParameter(const double V) const
{
  return (1. - V);
}

//=================================================================================================

void Geom_BezierSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = 0.0;
  U2 = 1.0;
  V1 = 0.0;
  V2 = 1.0;
}

//=================================================================================================

GeomAbs_Shape Geom_BezierSurface::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

void Geom_BezierSurface::D0(const double U, const double V, gp_Pnt& P) const
{
  if (urational || vrational)
  {
    BSplSLib::D0(U,
                 V,
                 1,
                 1,
                 myData.Poles,
                 &myData.Weights,
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P);
  }
  else
  {
    BSplSLib::D0(U,
                 V,
                 1,
                 1,
                 myData.Poles,
                 BSplSLib::NoWeights(),
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P);
  }
}

//=================================================================================================

void Geom_BezierSurface::D1(const double U,
                            const double V,
                            gp_Pnt&      P,
                            gp_Vec&      D1U,
                            gp_Vec&      D1V) const
{
  if (urational || vrational)
  {
    BSplSLib::D1(U,
                 V,
                 1,
                 1,
                 myData.Poles,
                 &myData.Weights,
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P,
                 D1U,
                 D1V);
  }
  else
  {
    BSplSLib::D1(U,
                 V,
                 1,
                 1,
                 myData.Poles,
                 BSplSLib::NoWeights(),
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P,
                 D1U,
                 D1V);
  }
}

//=================================================================================================

void Geom_BezierSurface::D2(const double U,
                            const double V,
                            gp_Pnt&      P,
                            gp_Vec&      D1U,
                            gp_Vec&      D1V,
                            gp_Vec&      D2U,
                            gp_Vec&      D2V,
                            gp_Vec&      D2UV) const
{
  if (urational || vrational)
  {
    //-- ATTENTION a l'ORDRE d'appel ds BSPLSLIB
    BSplSLib::D2(U,
                 V,
                 1,
                 1,
                 myData.Poles,
                 &myData.Weights,
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P,
                 D1U,
                 D1V,
                 D2U,
                 D2V,
                 D2UV);
  }
  else
  {
    //-- ATTENTION a l'ORDRE d'appel ds BSPLSLIB
    BSplSLib::D2(U,
                 V,
                 1,
                 1,
                 myData.Poles,
                 BSplSLib::NoWeights(),
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P,
                 D1U,
                 D1V,
                 D2U,
                 D2V,
                 D2UV);
  }
}

//=================================================================================================

void Geom_BezierSurface::D3(const double U,
                            const double V,
                            gp_Pnt&      P,
                            gp_Vec&      D1U,
                            gp_Vec&      D1V,
                            gp_Vec&      D2U,
                            gp_Vec&      D2V,
                            gp_Vec&      D2UV,
                            gp_Vec&      D3U,
                            gp_Vec&      D3V,
                            gp_Vec&      D3UUV,
                            gp_Vec&      D3UVV) const
{
  if (urational || vrational)
  {
    BSplSLib::D3(U,
                 V,
                 0,
                 0,
                 myData.Poles,
                 &myData.Weights,
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P,
                 D1U,
                 D1V,
                 D2U,
                 D2V,
                 D2UV,
                 D3U,
                 D3V,
                 D3UUV,
                 D3UVV);
  }
  else
  {
    BSplSLib::D3(U,
                 V,
                 0,
                 0,
                 myData.Poles,
                 BSplSLib::NoWeights(),
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 P,
                 D1U,
                 D1V,
                 D2U,
                 D2V,
                 D2UV,
                 D3U,
                 D3V,
                 D3UUV,
                 D3UVV);
  }
}

//=================================================================================================

gp_Vec Geom_BezierSurface::DN(const double U, const double V, const int Nu, const int Nv) const
{
  Standard_RangeError_Raise_if(Nu + Nv < 1 || Nv < 0 || Nu < 0, " ");
  gp_Vec Derivative;
  if (urational || vrational)
  {
    BSplSLib::DN(U,
                 V,
                 Nu,
                 Nv,
                 0,
                 0,
                 myData.Poles,
                 &myData.Weights,
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 Derivative);
  }
  else
  {
    BSplSLib::DN(U,
                 V,
                 Nu,
                 Nv,
                 0,
                 0,
                 myData.Poles,
                 BSplSLib::NoWeights(),
                 myData.UKnots,
                 myData.VKnots,
                 &myData.UMults,
                 &myData.VMults,
                 myData.UDegree,
                 myData.VDegree,
                 urational,
                 vrational,
                 false,
                 false,
                 Derivative);
  }
  return Derivative;
}

//=================================================================================================

int Geom_BezierSurface::NbUPoles() const
{
  return myData.Poles.ColLength();
}

//=================================================================================================

int Geom_BezierSurface::NbVPoles() const
{
  return myData.Poles.RowLength();
}

//=================================================================================================

const gp_Pnt& Geom_BezierSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myData.Poles.ColLength() || VIndex < 1
                                 || VIndex > myData.Poles.RowLength(),
                               " ");
  return myData.Poles(UIndex, VIndex);
}

//=================================================================================================

void Geom_BezierSurface::Poles(NCollection_Array2<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.RowLength() != myData.Poles.RowLength()
                                     || P.ColLength() != myData.Poles.ColLength(),
                                   " ");
  P = myData.Poles;
}

//=================================================================================================

int Geom_BezierSurface::UDegree() const
{
  return myData.UDegree;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BezierSurface::UIso(const double U) const
{
  occ::handle<Geom_BezierCurve>     UIsoCurve;
  NCollection_Array1<gp_Pnt>        VCurvePoles(1, myData.Poles.RowLength());
  if (urational || vrational)
  {
    NCollection_Array1<double> VCurveWeights(1, myData.Poles.RowLength());
    BSplSLib::Iso(U,
                  true,
                  myData.Poles,
                  &myData.Weights,
                  myData.UKnots,
                  &myData.UMults,
                  myData.UDegree,
                  false,
                  VCurvePoles,
                  &VCurveWeights);
    if (urational)
      UIsoCurve = new Geom_BezierCurve(VCurvePoles, VCurveWeights);
    else
      UIsoCurve = new Geom_BezierCurve(VCurvePoles);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  myData.Poles,
                  BSplSLib::NoWeights(),
                  myData.UKnots,
                  &myData.UMults,
                  myData.UDegree,
                  false,
                  VCurvePoles,
                  PLib::NoWeights());
    UIsoCurve = new Geom_BezierCurve(VCurvePoles);
  }
  return UIsoCurve;
}

//=================================================================================================

int Geom_BezierSurface::VDegree() const
{
  return myData.VDegree;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BezierSurface::VIso(const double V) const
{
  occ::handle<Geom_BezierCurve>     VIsoCurve;
  NCollection_Array1<gp_Pnt>        VCurvePoles(1, myData.Poles.ColLength());
  if (vrational || urational)
  {
    NCollection_Array1<double> VCurveWeights(1, myData.Poles.ColLength());
    BSplSLib::Iso(V,
                  false,
                  myData.Poles,
                  &myData.Weights,
                  myData.VKnots,
                  &myData.VMults,
                  myData.VDegree,
                  false,
                  VCurvePoles,
                  &VCurveWeights);
    if (vrational)
      VIsoCurve = new Geom_BezierCurve(VCurvePoles, VCurveWeights);
    else
      VIsoCurve = new Geom_BezierCurve(VCurvePoles);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  myData.Poles,
                  BSplSLib::NoWeights(),
                  myData.VKnots,
                  &myData.VMults,
                  myData.VDegree,
                  false,
                  VCurvePoles,
                  PLib::NoWeights());
    VIsoCurve = new Geom_BezierCurve(VCurvePoles);
  }
  return VIsoCurve;
}

//=================================================================================================

double Geom_BezierSurface::Weight(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myData.Poles.ColLength() || VIndex < 1
                                 || VIndex > myData.Poles.RowLength(),
                               " ");

  if (urational || vrational)
    return myData.Weights(UIndex, VIndex);
  else
    return 1;
}

//=================================================================================================

void Geom_BezierSurface::Weights(NCollection_Array2<double>& W) const
{
  Standard_DimensionError_Raise_if(W.RowLength() != myData.Poles.RowLength()
                                     || W.ColLength() != myData.Poles.ColLength(),
                                   " ");
  if (urational || vrational)
    W = myData.Weights;
  else
    W.Init(1.);
}

//=================================================================================================

bool Geom_BezierSurface::IsCNu(const int) const
{
  return true;
}

//=================================================================================================

bool Geom_BezierSurface::IsCNv(const int) const
{
  return true;
}

//=================================================================================================

bool Geom_BezierSurface::IsURational() const
{
  return urational;
}

//=================================================================================================

bool Geom_BezierSurface::IsVRational() const
{
  return vrational;
}

//=================================================================================================

void Geom_BezierSurface::Transform(const gp_Trsf& T)
{
  for (int I = 1; I <= myData.Poles.ColLength(); I++)
  {
    for (int J = 1; J <= myData.Poles.RowLength(); J++)
    {
      myData.Poles(I, J).Transform(T);
    }
  }
}

//=================================================================================================

bool Geom_BezierSurface::IsUClosed() const
{
  bool Closed = true;
  int  Lower  = myData.Poles.LowerRow();
  int  Upper  = myData.Poles.UpperRow();
  int  j      = myData.Poles.LowerCol();

  while (Closed && j <= myData.Poles.UpperCol())
  {
    Closed = (myData.Poles(Lower, j).Distance(myData.Poles(Upper, j)) <= Precision::Confusion());
    j++;
  }
  return Closed;
}

//=================================================================================================

bool Geom_BezierSurface::IsVClosed() const
{
  bool Closed = true;
  int  Lower  = myData.Poles.LowerCol();
  int  Upper  = myData.Poles.UpperCol();
  int  i      = myData.Poles.LowerRow();
  while (Closed && i <= myData.Poles.UpperRow())
  {
    Closed = (myData.Poles(i, Lower).Distance(myData.Poles(i, Upper)) <= Precision::Confusion());
    i++;
  }
  return Closed;
}

//=================================================================================================

bool Geom_BezierSurface::IsUPeriodic() const
{
  return false;
}

//=================================================================================================

bool Geom_BezierSurface::IsVPeriodic() const
{
  return false;
}

//=================================================================================================

void Geom_BezierSurface::Resolution(const double Tolerance3D,
                                    double&      UTolerance,
                                    double&      VTolerance)
{
  if (!maxderivinvok)
  {
    if (urational || vrational)
    {
      BSplSLib::Resolution(myData.Poles,
                           &myData.Weights,
                           myData.UKnots,
                           myData.VKnots,
                           myData.UMults,
                           myData.VMults,
                           myData.UDegree,
                           myData.VDegree,
                           urational,
                           vrational,
                           false,
                           false,
                           1.,
                           umaxderivinv,
                           vmaxderivinv);
    }
    else
    {
      BSplSLib::Resolution(myData.Poles,
                           BSplSLib::NoWeights(),
                           myData.UKnots,
                           myData.VKnots,
                           myData.UMults,
                           myData.VMults,
                           myData.UDegree,
                           myData.VDegree,
                           urational,
                           vrational,
                           false,
                           false,
                           1.,
                           umaxderivinv,
                           vmaxderivinv);
    }
    maxderivinvok = true;
  }
  UTolerance = Tolerance3D * umaxderivinv;
  VTolerance = Tolerance3D * vmaxderivinv;
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_BezierSurface::Copy() const
{
  return new Geom_BezierSurface(*this);
}

//=================================================================================================

void Geom_BezierSurface::Init(const NCollection_Array2<gp_Pnt>& thePoles,
                              const NCollection_Array2<double>* theWeights)
{
  int NbUPoles = thePoles.ColLength();
  int NbVPoles = thePoles.RowLength();

  myData.Poles.Resize(1, NbUPoles, 1, NbVPoles, false);
  myData.Poles = thePoles;

  myData.UDegree = NbUPoles - 1;
  myData.VDegree = NbVPoles - 1;

  if (urational || vrational)
  {
    myData.Weights.Resize(1, NbUPoles, 1, NbVPoles, false);
    if (theWeights != nullptr)
      myData.Weights = *theWeights;
    else
      myData.Weights.Init(1.0);
  }
  else
  {
    myData.Weights = NCollection_Array2<double>();
  }

  updateUKnots();
  updateVKnots();

  maxderivinvok = false;
}

//=================================================================================================

void Geom_BezierSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, urational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vrational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Poles.Size())
  if (urational || vrational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Weights.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, umaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vmaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
