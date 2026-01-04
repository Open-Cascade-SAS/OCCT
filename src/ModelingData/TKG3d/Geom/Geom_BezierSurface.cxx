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
#include "Geom_BezierSurfaceCache.pxx"
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
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
      NewPoles(RowIndex, ColIndex) = Poles(RowIndex, ColIndex - 1);
      RowIndex++;
      NewWeights(RowIndex, ColIndex) = Weights(RowIndex, ColIndex - 1);
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

Geom_BezierSurface::Geom_BezierSurface(const Geom_BezierSurface& theOther)
    : urational(theOther.urational),
      vrational(theOther.vrational),
      umaxderivinv(theOther.umaxderivinv),
      vmaxderivinv(theOther.vmaxderivinv),
      maxderivinvok(false)
{
  // Deep copy all data arrays without validation
  poles                 = new NCollection_HArray2<gp_Pnt>(theOther.poles->LowerRow(),
                                          theOther.poles->UpperRow(),
                                          theOther.poles->LowerCol(),
                                          theOther.poles->UpperCol());
  poles->ChangeArray2() = theOther.poles->Array2();

  if (!theOther.weights.IsNull())
  {
    weights                 = new NCollection_HArray2<double>(theOther.weights->LowerRow(),
                                              theOther.weights->UpperRow(),
                                              theOther.weights->LowerCol(),
                                              theOther.weights->UpperCol());
    weights->ChangeArray2() = theOther.weights->Array2();
  }
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

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, NbUPoles, 1, NbVPoles);

  urational = false;
  vrational = false;

  npoles->ChangeArray2() = SurfacePoles;

  // Init non rational
  Init(npoles, occ::handle<NCollection_HArray2<double>>());
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

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, NbUPoles, 1, NbVPoles);
  npoles->ChangeArray2() = SurfacePoles;

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

  occ::handle<NCollection_HArray2<double>> nweights;
  if (urational || vrational)
  {
    nweights                 = new NCollection_HArray2<double>(1, NbUPoles, 1, NbVPoles);
    nweights->ChangeArray2() = PoleWeights;
  }

  // Init
  Init(npoles, nweights);
}

//=================================================================================================

Geom_BezierSurface::Geom_BezierSurface(const occ::handle<NCollection_HArray2<gp_Pnt>>& SurfacePoles,
                                       const occ::handle<NCollection_HArray2<double>>& PoleWeights,
                                       const bool                                      IsURational,
                                       const bool                                      IsVRational)
    : maxderivinvok(false)
{
  urational    = IsURational;
  vrational    = IsVRational;
  int NbUPoles = SurfacePoles->ColLength();
  int NbVPoles = SurfacePoles->RowLength();

  poles                 = new NCollection_HArray2<gp_Pnt>(1, NbUPoles, 1, NbVPoles);
  poles->ChangeArray2() = SurfacePoles->Array2();

  if (urational || vrational)
  {
    weights                 = new NCollection_HArray2<double>(1, NbUPoles, 1, NbVPoles);
    weights->ChangeArray2() = PoleWeights->Array2();
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
  int LC = poles->LowerCol();
  int UC = poles->UpperCol();
  int LR = poles->LowerRow();
  int UR = poles->UpperRow();

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles = new NCollection_HArray2<gp_Pnt>(LC, UC, LR, UR);
  occ::handle<NCollection_HArray2<double>> nweights;
  if (!weights.IsNull())
  {
    nweights = new NCollection_HArray2<double>(LC, UC, LR, UR);
  }

  const NCollection_Array2<gp_Pnt>& spoles   = poles->Array2();
  const NCollection_Array2<double>* sweights = !weights.IsNull() ? &weights->Array2() : nullptr;

  NCollection_Array2<gp_Pnt>& snpoles   = npoles->ChangeArray2();
  NCollection_Array2<double>* snweights = !nweights.IsNull() ? &nweights->ChangeArray2() : nullptr;
  for (int i = LC; i <= UC; i++)
  {
    for (int j = LR; j <= UR; j++)
    {
      snpoles(i, j) = spoles(j, i);
      if (snweights != nullptr)
      {
        snweights->ChangeValue(i, j) = sweights->Value(j, i);
      }
    }
  }
  poles   = npoles;
  weights = nweights;

  std::swap(urational, vrational);

  invalidateCache();
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

  NCollection_Array1<double> biduknots(1, 2);
  biduknots(1) = 0.;
  biduknots(2) = 1.;
  NCollection_Array1<int> bidumults(1, 2);
  bidumults.Init(UDegree() + 1);
  NCollection_Array1<double> bidvknots(1, 2);
  bidvknots(1) = 0.;
  bidvknots(2) = 1.;
  NCollection_Array1<int> bidvmults(1, 2);
  bidvmults.Init(VDegree() + 1);
  occ::handle<NCollection_HArray2<gp_Pnt>> npoles;
  occ::handle<NCollection_HArray2<double>> nweights;

  if (IncUDeg > 0)
  {
    npoles = new NCollection_HArray2<gp_Pnt>(1, UDeg + 1, 1, oldVDeg + 1);

    if (urational || vrational)
    {
      nweights = new NCollection_HArray2<double>(1, UDeg + 1, 1, VDegree() + 1);

      BSplSLib::IncreaseDegree(true,
                               oldUDeg,
                               UDeg,
                               false,
                               poles->Array2(),
                               &weights->Array2(),
                               biduknots,
                               bidumults,
                               npoles->ChangeArray2(),
                               &nweights->ChangeArray2(),
                               biduknots,
                               bidumults);
      weights = nweights;
    }
    else
    {
      BSplSLib::IncreaseDegree(true,
                               oldUDeg,
                               UDeg,
                               false,
                               poles->Array2(),
                               BSplSLib::NoWeights(),
                               biduknots,
                               bidumults,
                               npoles->ChangeArray2(),
                               BSplSLib::NoWeights(),
                               biduknots,
                               bidumults);
    }
    poles = npoles;
  }
  if (IncVDeg > 0)
  {
    npoles = new NCollection_HArray2<gp_Pnt>(1, UDeg + 1, 1, VDeg + 1);

    if (urational || vrational)
    {
      nweights = new NCollection_HArray2<double>(1, UDeg + 1, 1, VDeg + 1);

      BSplSLib::IncreaseDegree(false,
                               oldVDeg,
                               VDeg,
                               false,
                               poles->Array2(),
                               &weights->Array2(),
                               bidvknots,
                               bidvmults,
                               npoles->ChangeArray2(),
                               &nweights->ChangeArray2(),
                               bidvknots,
                               bidvmults);
      weights = nweights;
    }
    else
    {
      BSplSLib::IncreaseDegree(false,
                               oldVDeg,
                               VDeg,
                               false,
                               poles->Array2(),
                               BSplSLib::NoWeights(),
                               bidvknots,
                               bidvmults,
                               npoles->ChangeArray2(),
                               BSplSLib::NoWeights(),
                               bidvknots,
                               bidvmults);
    }
    poles = npoles;
  }
  Init(npoles, nweights);
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColAfter(const int                         VIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles)
{
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  if (VIndex < 1 || VIndex > Poles.RowLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.ColLength())
  {
    throw Standard_ConstructionError();
  }

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, poles->ColLength(), 1, poles->RowLength() + 1);

  occ::handle<NCollection_HArray2<double>> nweights;

  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength() + 1);

    NCollection_Array1<double> CWeights(nweights->LowerRow(), nweights->UpperRow());
    CWeights.Init(1.);

    AddRatPoleCol(poles->Array2(),
                  weights->Array2(),
                  CPoles,
                  CWeights,
                  VIndex,
                  npoles->ChangeArray2(),
                  nweights->ChangeArray2());
  }
  else
  {
    AddPoleCol(poles->Array2(), CPoles, VIndex, npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColAfter(const int                         VIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles,
                                            const NCollection_Array1<double>& CPoleWeights)
{
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  if (VIndex < 1 || VIndex > Poles.RowLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.ColLength() || CPoleWeights.Length() != CPoles.Length())
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

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, poles->ColLength(), 1, poles->RowLength() + 1);

  occ::handle<NCollection_HArray2<double>> nweights =
    new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength() + 1);

  AddRatPoleCol(poles->Array2(),
                weights->Array2(),
                CPoles,
                CPoleWeights,
                VIndex,
                npoles->ChangeArray2(),
                nweights->ChangeArray2());

  poles   = npoles;
  weights = nweights;

  Rational(weights->Array2(), urational, vrational);
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
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  if (UIndex < 1 || UIndex > Poles.ColLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.RowLength())
  {
    throw Standard_ConstructionError();
  }

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, poles->ColLength() + 1, 1, poles->RowLength());

  occ::handle<NCollection_HArray2<double>> nweights;

  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, poles->ColLength() + 1, 1, poles->RowLength());

    NCollection_Array1<double> CWeights(nweights->LowerCol(), nweights->UpperCol());
    CWeights.Init(1.0);

    AddRatPoleRow(poles->Array2(),
                  weights->Array2(),
                  CPoles,
                  CWeights,
                  UIndex,
                  npoles->ChangeArray2(),
                  nweights->ChangeArray2());
  }
  else
  {
    AddPoleRow(poles->Array2(), CPoles, UIndex, npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleRowAfter(const int                         UIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles,
                                            const NCollection_Array1<double>& CPoleWeights)
{
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  if (UIndex < 1 || UIndex > Poles.ColLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != Poles.RowLength() || CPoleWeights.Length() != CPoles.Length())
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

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, poles->ColLength() + 1, 1, poles->RowLength());

  occ::handle<NCollection_HArray2<double>> nweights =
    new NCollection_HArray2<double>(1, poles->ColLength() + 1, 1, poles->RowLength());

  AddRatPoleCol(poles->Array2(),
                weights->Array2(),
                CPoles,
                CPoleWeights,
                UIndex,
                npoles->ChangeArray2(),
                nweights->ChangeArray2());

  poles   = npoles;
  weights = nweights;

  Rational(weights->Array2(), urational, vrational);
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
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  if (VIndex < 1 || VIndex > Poles.RowLength())
    throw Standard_OutOfRange();
  if (Poles.RowLength() <= 2)
    throw Standard_ConstructionError();

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, poles->ColLength(), 1, poles->RowLength() - 1);

  occ::handle<NCollection_HArray2<double>> nweights;

  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength() - 1);

    DeleteRatPoleCol(poles->Array2(),
                     weights->Array2(),
                     VIndex,
                     npoles->ChangeArray2(),
                     nweights->ChangeArray2());
    // Mise a jour de la rationalite
    Rational(nweights->Array2(), urational, vrational);
  }
  else
  {
    DeletePoleCol(poles->Array2(), VIndex, npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=================================================================================================

void Geom_BezierSurface::RemovePoleRow(const int UIndex)
{
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  if (UIndex < 1 || UIndex > Poles.ColLength())
    throw Standard_OutOfRange();
  if (Poles.ColLength() <= 2)
    throw Standard_ConstructionError();

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, poles->ColLength() - 1, 1, poles->RowLength());

  occ::handle<NCollection_HArray2<double>> nweights;

  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, poles->ColLength() - 1, 1, poles->RowLength());

    DeleteRatPoleRow(poles->Array2(),
                     weights->Array2(),
                     UIndex,
                     npoles->ChangeArray2(),
                     nweights->ChangeArray2());

    // Mise a jour de la rationalite
    Rational(nweights->Array2(), urational, vrational);
  }
  else
  {
    DeletePoleRow(poles->Array2(), UIndex, npoles->ChangeArray2());
  }
  poles   = npoles;
  weights = nweights;
}

//=================================================================================================

void Geom_BezierSurface::Segment(const double U1, const double U2, const double V1, const double V2)
{
  bool                                     rat = (urational || vrational);
  occ::handle<NCollection_HArray2<gp_Pnt>> Coefs;
  occ::handle<NCollection_HArray2<double>> WCoefs;

  int aMinDegree = UDegree() <= VDegree() ? UDegree() : VDegree();
  int aMaxDegree = UDegree() > VDegree() ? UDegree() : VDegree();
  Coefs          = new NCollection_HArray2<gp_Pnt>(1, aMaxDegree + 1, 1, aMinDegree + 1);
  if (rat)
    WCoefs = new NCollection_HArray2<double>(1, aMaxDegree + 1, 1, aMinDegree + 1);

  NCollection_Array1<double> biduflatknots(BSplCLib::FlatBezierKnots(UDegree()),
                                           1,
                                           2 * (UDegree() + 1));
  NCollection_Array1<double> bidvflatknots(BSplCLib::FlatBezierKnots(VDegree()),
                                           1,
                                           2 * (VDegree() + 1));

  double uparameter_11  = 0.5;
  double uspanlenght_11 = 0.5;
  double vparameter_11  = 0.5;
  double vspanlenght_11 = 0.5;

  if (urational || vrational)
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
                         biduflatknots,
                         bidvflatknots,
                         poles->Array2(),
                         &weights->Array2(),
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
                         biduflatknots,
                         bidvflatknots,
                         poles->Array2(),
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
                            poles->ChangeArray2(),
                            &weights->ChangeArray2());
  }
  else
  {
    PLib::UTrimming(ufirst, ulast, Coefs->ChangeArray2(), PLib::NoWeights2());
    PLib::VTrimming(vfirst, vlast, Coefs->ChangeArray2(), PLib::NoWeights2());
    PLib::CoefficientsPoles(Coefs->Array2(),
                            PLib::NoWeights2(),
                            poles->ChangeArray2(),
                            PLib::NoWeights2());
  }

  invalidateCache();
}

//=================================================================================================

void Geom_BezierSurface::SetPole(const int UIndex, const int VIndex, const gp_Pnt& P)
{
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (UIndex < 1 || UIndex > Poles.ColLength() || VIndex < 1 || VIndex > Poles.RowLength())
    throw Standard_OutOfRange();

  Poles(UIndex, VIndex) = P;

  invalidateCache();
}

//=================================================================================================

void Geom_BezierSurface::SetPole(const int     UIndex,
                                 const int     VIndex,
                                 const gp_Pnt& P,
                                 const double  Weight)
{

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierSurface::SetPole");
  if (UIndex < 1 || UIndex > poles->ColLength() || VIndex < 1 || VIndex > poles->RowLength())
    throw Standard_OutOfRange("Geom_BezierSurface::SetPole");

  poles->SetValue(UIndex, VIndex, P);

  SetWeight(UIndex, VIndex, Weight); // L'update des coeff est fait la dedans
}

//=================================================================================================

void Geom_BezierSurface::SetPoleCol(const int                         VIndex,
                                    const NCollection_Array1<gp_Pnt>& CPoles,
                                    const NCollection_Array1<double>& CPoleWeights)
{
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (VIndex < 1 || VIndex > Poles.RowLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > Poles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > Poles.ColLength() || CPoleWeights.Lower() != CPoles.Lower()
      || CPoleWeights.Upper() != CPoles.Upper())
  {
    throw Standard_ConstructionError();
  }

  int I;
  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    Poles(I, VIndex) = CPoles(I);
  }
  SetWeightCol(VIndex, CPoleWeights); // Avec l'update
}

//=================================================================================================

void Geom_BezierSurface::SetPoleCol(const int VIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (VIndex < 1 || VIndex > Poles.RowLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > Poles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > Poles.ColLength())
  {
    throw Standard_ConstructionError();
  }
  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    Poles(I, VIndex) = CPoles(I);
  }

  invalidateCache();
}

//=================================================================================================

void Geom_BezierSurface::SetPoleRow(const int UIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (UIndex < 1 || UIndex > Poles.ColLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > Poles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > Poles.RowLength())
    throw Standard_ConstructionError();

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    Poles(UIndex, I) = CPoles(I);
  }

  invalidateCache();
}

//=================================================================================================

void Geom_BezierSurface::SetPoleRow(const int                         UIndex,
                                    const NCollection_Array1<gp_Pnt>& CPoles,
                                    const NCollection_Array1<double>& CPoleWeights)
{
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (UIndex < 1 || UIndex > Poles.ColLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > Poles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > Poles.RowLength() || CPoleWeights.Lower() != CPoles.Lower()
      || CPoleWeights.Upper() != CPoles.Upper())
  {
    throw Standard_ConstructionError();
  }

  int I;

  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    Poles(UIndex, I) = CPoles(I);
  }

  SetWeightRow(UIndex, CPoleWeights); // Avec l'update
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
    weights = new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength(), 1.);
  }

  NCollection_Array2<double>& Weights = weights->ChangeArray2();
  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeight");

  if (UIndex < 1 || UIndex > Weights.ColLength() || VIndex < 1 || VIndex > Weights.RowLength())
    throw Standard_OutOfRange();

  if (std::abs(Weight - Weights(UIndex, VIndex)) > gp::Resolution())
  {
    Weights(UIndex, VIndex) = Weight;
    Rational(Weights, urational, vrational);
  }

  // is it turning into non rational
  if (wasrat && !(urational || vrational))
    weights.Nullify();

  invalidateCache();
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
    weights = new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength(), 1.);
  }

  NCollection_Array2<double>& Weights = weights->ChangeArray2();
  if (VIndex < 1 || VIndex > Weights.RowLength())
    throw Standard_OutOfRange();

  if (CPoleWeights.Length() != Weights.ColLength())
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
    Weights(I, VIndex) = CPoleWeights(I);
    I++;
  }

  Rational(Weights, urational, vrational);

  // is it turning into non rational
  if (wasrat && !(urational || vrational))
    weights.Nullify();

  invalidateCache();
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
    weights = new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength(), 1.);
  }

  NCollection_Array2<double>& Weights = weights->ChangeArray2();
  if (UIndex < 1 || UIndex > Weights.ColLength())
    throw Standard_OutOfRange("Geom_BezierSurface::SetWeightRow");
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > Weights.RowLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > Weights.RowLength())
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
    Weights(UIndex, I) = CPoleWeights(I);
    I++;
  }

  Rational(Weights, urational, vrational);

  // is it turning into non rational
  if (wasrat && !(urational || vrational))
    weights.Nullify();

  invalidateCache();
}

//=================================================================================================

void Geom_BezierSurface::UReverse()
{
  gp_Pnt                      Pol;
  int                         Row, Col;
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (urational || vrational)
  {
    NCollection_Array2<double>& Weights = weights->ChangeArray2();
    double                      W;
    for (Col = 1; Col <= Poles.RowLength(); Col++)
    {
      for (Row = 1; Row <= std::trunc(Poles.ColLength() / 2); Row++)
      {
        W                                         = Weights(Row, Col);
        Weights(Row, Col)                         = Weights(Poles.ColLength() - Row + 1, Col);
        Weights(Poles.ColLength() - Row + 1, Col) = W;
        Pol                                       = Poles(Row, Col);
        Poles(Row, Col)                           = Poles(Poles.ColLength() - Row + 1, Col);
        Poles(Poles.ColLength() - Row + 1, Col)   = Pol;
      }
    }
  }
  else
  {
    for (Col = 1; Col <= Poles.RowLength(); Col++)
    {
      for (Row = 1; Row <= std::trunc(Poles.ColLength() / 2); Row++)
      {
        Pol                                     = Poles(Row, Col);
        Poles(Row, Col)                         = Poles(Poles.ColLength() - Row + 1, Col);
        Poles(Poles.ColLength() - Row + 1, Col) = Pol;
      }
    }
  }

  invalidateCache();
}

//=================================================================================================

double Geom_BezierSurface::UReversedParameter(const double U) const
{
  return (1. - U);
}

//=================================================================================================

void Geom_BezierSurface::VReverse()
{
  gp_Pnt                      Pol;
  int                         Row, Col;
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();
  if (urational || vrational)
  {
    NCollection_Array2<double>& Weights = weights->ChangeArray2();
    double                      W;
    for (Row = 1; Row <= Poles.ColLength(); Row++)
    {
      for (Col = 1; Col <= std::trunc(Poles.RowLength() / 2); Col++)
      {
        W                                         = Weights(Row, Col);
        Weights(Row, Col)                         = Weights(Row, Poles.RowLength() - Col + 1);
        Weights(Row, Poles.RowLength() - Col + 1) = W;
        Pol                                       = Poles(Row, Col);
        Poles(Row, Col)                           = Poles(Row, Poles.RowLength() - Col + 1);
        Poles(Row, Poles.RowLength() - Col + 1)   = Pol;
      }
    }
  }
  else
  {
    for (Row = 1; Row <= Poles.ColLength(); Row++)
    {
      for (Col = 1; Col <= std::trunc(Poles.RowLength() / 2); Col++)
      {
        Pol                                     = Poles(Row, Col);
        Poles(Row, Col)                         = Poles(Row, Poles.RowLength() - Col + 1);
        Poles(Row, Poles.RowLength() - Col + 1) = Pol;
      }
    }
  }

  invalidateCache();
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
  Geom_BezierSurfaceCache& aCache = ensureCache();
  aCache.Build(poles->Array2(), Weights());
  aCache.D0(U, V, P);
}

//=================================================================================================

void Geom_BezierSurface::D1(const double U,
                            const double V,
                            gp_Pnt&      P,
                            gp_Vec&      D1U,
                            gp_Vec&      D1V) const
{
  Geom_BezierSurfaceCache& aCache = ensureCache();
  aCache.Build(poles->Array2(), Weights());
  aCache.D1(U, V, P, D1U, D1V);
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
  Geom_BezierSurfaceCache& aCache = ensureCache();
  aCache.Build(poles->Array2(), Weights());
  aCache.D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
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
  NCollection_Array1<double> biduknots(1, 2);
  biduknots(1) = 0.;
  biduknots(2) = 1.;
  NCollection_Array1<int> bidumults(1, 2);
  bidumults.Init(UDegree() + 1);
  NCollection_Array1<double> bidvknots(1, 2);
  bidvknots(1) = 0.;
  bidvknots(2) = 1.;
  NCollection_Array1<int> bidvmults(1, 2);
  bidvmults.Init(VDegree() + 1);
  if (urational || vrational)
  {
    BSplSLib::D3(U,
                 V,
                 0,
                 0,
                 poles->Array2(),
                 &weights->Array2(),
                 biduknots,
                 bidvknots,
                 &bidumults,
                 &bidvmults,
                 UDegree(),
                 VDegree(),
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
                 poles->Array2(),
                 BSplSLib::NoWeights(),
                 biduknots,
                 bidvknots,
                 &bidumults,
                 &bidvmults,
                 UDegree(),
                 VDegree(),
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
  gp_Vec                     Derivative;
  NCollection_Array1<double> biduknots(1, 2);
  biduknots(1) = 0.;
  biduknots(2) = 1.;
  NCollection_Array1<int> bidumults(1, 2);
  bidumults.Init(UDegree() + 1);
  NCollection_Array1<double> bidvknots(1, 2);
  bidvknots(1) = 0.;
  bidvknots(2) = 1.;
  NCollection_Array1<int> bidvmults(1, 2);
  bidvmults.Init(VDegree() + 1);
  if (urational || vrational)
  {
    BSplSLib::DN(U,
                 V,
                 Nu,
                 Nv,
                 0,
                 0,
                 poles->Array2(),
                 &weights->Array2(),
                 biduknots,
                 bidvknots,
                 &bidumults,
                 &bidvmults,
                 UDegree(),
                 VDegree(),
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
                 poles->Array2(),
                 BSplSLib::NoWeights(),
                 biduknots,
                 bidvknots,
                 &bidumults,
                 &bidvmults,
                 UDegree(),
                 VDegree(),
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
  return poles->ColLength();
}

//=================================================================================================

int Geom_BezierSurface::NbVPoles() const
{
  return poles->RowLength();
}

//=================================================================================================

const gp_Pnt& Geom_BezierSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > poles->ColLength() || VIndex < 1
                                 || VIndex > poles->RowLength(),
                               " ");
  return poles->Value(UIndex + poles->LowerRow() - 1, VIndex + poles->LowerCol() - 1);
}

//=================================================================================================

void Geom_BezierSurface::Poles(NCollection_Array2<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.RowLength() != poles->RowLength()
                                     || P.ColLength() != poles->ColLength(),
                                   " ");
  P = poles->Array2();
}

//=================================================================================================

int Geom_BezierSurface::UDegree() const
{
  return poles->ColLength() - 1;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BezierSurface::UIso(const double U) const
{
  NCollection_Array1<double> biduknots(1, 2);
  biduknots(1) = 0.;
  biduknots(2) = 1.;
  NCollection_Array1<int> bidumults(1, 2);
  bidumults.Init(UDegree() + 1);

  occ::handle<Geom_BezierCurve>     UIsoCurve;
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  NCollection_Array1<gp_Pnt>        VCurvePoles(Poles.LowerCol(), Poles.UpperCol());
  if (urational || vrational)
  {
    const NCollection_Array2<double>& Weights = weights->Array2();
    NCollection_Array1<double>        VCurveWeights(Weights.LowerCol(), Weights.UpperCol());
    BSplSLib::Iso(U,
                  true,
                  Poles,
                  &Weights,
                  biduknots,
                  &bidumults,
                  UDegree(),
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
                  Poles,
                  BSplSLib::NoWeights(),
                  biduknots,
                  &bidumults,
                  UDegree(),
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
  return poles->RowLength() - 1;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BezierSurface::VIso(const double V) const
{
  NCollection_Array1<double> bidvknots(1, 2);
  bidvknots(1) = 0.;
  bidvknots(2) = 1.;
  NCollection_Array1<int> bidvmults(1, 2);
  bidvmults.Init(VDegree() + 1);

  occ::handle<Geom_BezierCurve>     VIsoCurve;
  const NCollection_Array2<gp_Pnt>& Poles = poles->Array2();
  NCollection_Array1<gp_Pnt>        VCurvePoles(Poles.LowerRow(), Poles.UpperRow());
  if (vrational || urational)
  {
    const NCollection_Array2<double>& Weights = weights->Array2();
    NCollection_Array1<double>        VCurveWeights(Weights.LowerRow(), Weights.UpperRow());
    BSplSLib::Iso(V,
                  false,
                  Poles,
                  &Weights,
                  bidvknots,
                  &bidvmults,
                  VDegree(),
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
                  Poles,
                  BSplSLib::NoWeights(),
                  bidvknots,
                  &bidvmults,
                  VDegree(),
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
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > weights->ColLength() || VIndex < 1
                                 || VIndex > weights->RowLength(),
                               " ");

  if (urational || vrational)
    return weights->Value(UIndex, VIndex);
  else
    return 1;
}

//=================================================================================================

void Geom_BezierSurface::Weights(NCollection_Array2<double>& W) const
{
  Standard_DimensionError_Raise_if(W.RowLength() != weights->RowLength()
                                     || W.ColLength() != weights->ColLength(),
                                   " ");
  if (urational || vrational)
    W = weights->Array2();
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
  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();

  for (int I = 1; I <= Poles.ColLength(); I++)
  {

    for (int J = 1; J <= Poles.RowLength(); J++)
    {
      Poles(I, J).Transform(T);
    }
  }

  invalidateCache();
}

//=================================================================================================

bool Geom_BezierSurface::IsUClosed() const
{
  const NCollection_Array2<gp_Pnt>& Poles  = poles->Array2();
  bool                              Closed = true;
  int                               Lower  = Poles.LowerRow();
  int                               Upper  = Poles.UpperRow();
  int                               Length = Poles.RowLength();
  int                               j      = Poles.LowerCol();

  while (Closed && j <= Length)
  {
    Closed = (Poles(Lower, j).Distance(Poles(Upper, j)) <= Precision::Confusion());
    j++;
  }
  return Closed;
}

//=================================================================================================

bool Geom_BezierSurface::IsVClosed() const
{
  const NCollection_Array2<gp_Pnt>& Poles  = poles->Array2();
  bool                              Closed = true;
  int                               Lower  = Poles.LowerCol();
  int                               Upper  = Poles.UpperCol();
  int                               Length = Poles.ColLength();
  int                               i      = Poles.LowerRow();
  while (Closed && i <= Length)
  {
    Closed = (Poles(i, Lower).Distance(Poles(i, Upper)) <= Precision::Confusion());
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
    NCollection_Array1<double> biduknots(1, 2);
    biduknots(1) = 0.;
    biduknots(2) = 1.;
    NCollection_Array1<int> bidumults(1, 2);
    bidumults.Init(UDegree() + 1);
    NCollection_Array1<double> bidvknots(1, 2);
    bidvknots(1) = 0.;
    bidvknots(2) = 1.;
    NCollection_Array1<int> bidvmults(1, 2);
    bidvmults.Init(VDegree() + 1);
    if (urational || vrational)
    {
      BSplSLib::Resolution(poles->Array2(),
                           &weights->Array2(),
                           biduknots,
                           bidvknots,
                           bidumults,
                           bidvmults,
                           UDegree(),
                           VDegree(),
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
      BSplSLib::Resolution(poles->Array2(),
                           BSplSLib::NoWeights(),
                           biduknots,
                           bidvknots,
                           bidumults,
                           bidvmults,
                           UDegree(),
                           VDegree(),
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

void Geom_BezierSurface::Init(const occ::handle<NCollection_HArray2<gp_Pnt>>& Poles,
                              const occ::handle<NCollection_HArray2<double>>& Weights)
{
  // set fields
  poles = Poles;
  if (urational || vrational)
    weights = Weights;
  else
    weights.Nullify();

  // Invalidate cache since structure changed
  invalidateCache();
}

//=================================================================================================

Geom_BezierSurfaceCache& Geom_BezierSurface::ensureCache() const
{
  if (!myCache)
  {
    myCache = std::make_unique<Geom_BezierSurfaceCache>(UDegree(), VDegree(), urational || vrational);
  }
  return *myCache;
}

//=================================================================================================

void Geom_BezierSurface::invalidateCache() const
{
  myCache.reset();
}

//=================================================================================================

void Geom_BezierSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, urational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vrational)
  if (!poles.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, poles->Size())
  if (!weights.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, weights->Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, umaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vmaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
