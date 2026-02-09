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

#include <array>

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

Geom_BezierSurface::Geom_BezierSurface(const Geom_BezierSurface& theOther)
    : myPoles(theOther.myPoles),
      myWeights((theOther.myURational || theOther.myVRational)
                  ? NCollection_Array2<double>(theOther.myWeights)
                  : BSplSLib::UnitWeights(theOther.myPoles.ColLength(),
                                          theOther.myPoles.RowLength())),
      myURational(theOther.myURational),
      myVRational(theOther.myVRational),
      myUMaxDerivInv(theOther.myUMaxDerivInv),
      myVMaxDerivInv(theOther.myVMaxDerivInv),
      myMaxDerivInvOk(false)
{
}

//=================================================================================================

Geom_BezierSurface::Geom_BezierSurface(const NCollection_Array2<gp_Pnt>& SurfacePoles)
    : myMaxDerivInvOk(false)
{
  int NbUPoles = SurfacePoles.ColLength();
  int NbVPoles = SurfacePoles.RowLength();
  if (NbUPoles < 2 || NbUPoles > MaxDegree() + 1 || NbVPoles < 2 || NbVPoles > MaxDegree() + 1)
  {
    throw Standard_ConstructionError();
  }

  myURational = false;
  myVRational = false;

  // init non rational
  init(SurfacePoles, nullptr);
}

//=================================================================================================

Geom_BezierSurface::Geom_BezierSurface(const NCollection_Array2<gp_Pnt>& SurfacePoles,
                                       const NCollection_Array2<double>& PoleWeights)
    : myMaxDerivInvOk(false)
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
  myURational = false;
  myVRational = false;
  J           = PoleWeights.LowerCol();
  while (!myVRational && J <= PoleWeights.UpperCol())
  {
    I = PoleWeights.LowerRow();
    while (!myVRational && I <= PoleWeights.UpperRow() - 1)
    {
      myVRational = (std::abs(PoleWeights(I, J) - PoleWeights(I + 1, J))
                     > Epsilon(std::abs(PoleWeights(I, J))));
      I++;
    }
    J++;
  }
  I = PoleWeights.LowerRow();
  while (!myURational && I <= PoleWeights.UpperRow())
  {
    J = PoleWeights.LowerCol();
    while (!myURational && J <= PoleWeights.UpperCol() - 1)
    {
      myURational = (std::abs(PoleWeights(I, J) - PoleWeights(I, J + 1))
                     > Epsilon(std::abs(PoleWeights(I, J))));
      J++;
    }
    I++;
  }

  if (myURational || myVRational)
  {
    init(SurfacePoles, &PoleWeights);
  }
  else
  {
    init(SurfacePoles, nullptr);
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
  int LR = myPoles.LowerRow();
  int UR = myPoles.UpperRow();
  int LC = myPoles.LowerCol();
  int UC = myPoles.UpperCol();

  NCollection_Array2<gp_Pnt> npoles(LC, UC, LR, UR);
  NCollection_Array2<double> nweights;
  bool                       rat = (myURational || myVRational);
  if (rat)
  {
    nweights.Resize(LC, UC, LR, UR, false);
  }

  for (int i = LC; i <= UC; i++)
  {
    for (int j = LR; j <= UR; j++)
    {
      npoles(i, j) = myPoles(j, i);
      if (rat)
      {
        nweights(i, j) = myWeights(j, i);
      }
    }
  }
  myPoles = std::move(npoles);
  if (rat)
  {
    myWeights = std::move(nweights);
  }
  else
  {
    myWeights = BSplSLib::UnitWeights(UC - LC + 1, UR - LR + 1);
  }

  std::swap(myURational, myVRational);
  myMaxDerivInvOk = false;
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

    if (myURational || myVRational)
    {
      NCollection_Array2<double> nweights(1, UDeg + 1, 1, VDegree() + 1);

      BSplSLib::IncreaseDegree(true,
                               oldUDeg,
                               UDeg,
                               false,
                               myPoles,
                               &myWeights,
                               UKnots(),
                               UMultiplicities(),
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
      myWeights = std::move(nweights);
    }
    else
    {
      BSplSLib::IncreaseDegree(true,
                               oldUDeg,
                               UDeg,
                               false,
                               myPoles,
                               BSplSLib::NoWeights(),
                               UKnots(),
                               UMultiplicities(),
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
    }
    myPoles = std::move(npoles);
  }
  if (IncVDeg > 0)
  {
    NCollection_Array2<gp_Pnt> npoles(1, UDeg + 1, 1, VDeg + 1);
    double                     aKnotsBuf[2];
    int                        aMultsBuf[2];
    NCollection_Array1<double> nknots(aKnotsBuf[0], 1, 2);
    NCollection_Array1<int>    nmults(aMultsBuf[0], 1, 2);

    if (myURational || myVRational)
    {
      NCollection_Array2<double> nweights(1, UDeg + 1, 1, VDeg + 1);

      BSplSLib::IncreaseDegree(false,
                               oldVDeg,
                               VDeg,
                               false,
                               myPoles,
                               &myWeights,
                               UKnots(),
                               VMultiplicities(),
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
      myWeights = std::move(nweights);
    }
    else
    {
      BSplSLib::IncreaseDegree(false,
                               oldVDeg,
                               VDeg,
                               false,
                               myPoles,
                               BSplSLib::NoWeights(),
                               UKnots(),
                               VMultiplicities(),
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
    }
    myPoles = std::move(npoles);
  }
  if (myURational || myVRational)
  {
    Rational(myWeights, myURational, myVRational);
    if (!(myURational || myVRational))
    {
      myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
    }
  }
  else
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColAfter(const int                         VIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myPoles.ColLength())
  {
    throw Standard_ConstructionError();
  }

  int                        NbUPoles = myPoles.ColLength();
  int                        NbVPoles = myPoles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles, 1, NbVPoles + 1);

  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles, 1, NbVPoles + 1);

    NCollection_Array1<double> CWeights(1, NbUPoles);
    CWeights.Init(1.);

    AddRatPoleCol(myPoles, myWeights, CPoles, CWeights, VIndex, npoles, nweights);
    myWeights = std::move(nweights);
  }
  else
  {
    AddPoleCol(myPoles, CPoles, VIndex, npoles);
    myWeights = BSplSLib::UnitWeights(NbUPoles, NbVPoles + 1);
  }
  myPoles         = std::move(npoles);
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleColAfter(const int                         VIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles,
                                            const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myPoles.ColLength() || CPoleWeights.Length() != CPoles.Length())
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

  int NbUPoles = myPoles.ColLength();
  int NbVPoles = myPoles.RowLength();

  // Ensure weights are an owned copy for rational insertion
  if (!(myURational || myVRational))
  {
    myWeights = NCollection_Array2<double>(myWeights);
  }

  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles, 1, NbVPoles + 1);
  NCollection_Array2<double> nweights(1, NbUPoles, 1, NbVPoles + 1);

  AddRatPoleCol(myPoles, myWeights, CPoles, CPoleWeights, VIndex, npoles, nweights);

  myPoles   = std::move(npoles);
  myWeights = std::move(nweights);

  Rational(myWeights, myURational, myVRational);
  if (!(myURational || myVRational))
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  }
  myMaxDerivInvOk = false;
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
  if (UIndex < 1 || UIndex > myPoles.ColLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myPoles.RowLength())
  {
    throw Standard_ConstructionError();
  }

  int                        NbUPoles = myPoles.ColLength();
  int                        NbVPoles = myPoles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles + 1, 1, NbVPoles);

  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles + 1, 1, NbVPoles);

    NCollection_Array1<double> CWeights(1, NbVPoles);
    CWeights.Init(1.0);

    AddRatPoleRow(myPoles, myWeights, CPoles, CWeights, UIndex, npoles, nweights);
    myWeights = std::move(nweights);
  }
  else
  {
    AddPoleRow(myPoles, CPoles, UIndex, npoles);
    myWeights = BSplSLib::UnitWeights(NbUPoles + 1, NbVPoles);
  }
  myPoles         = std::move(npoles);
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::InsertPoleRowAfter(const int                         UIndex,
                                            const NCollection_Array1<gp_Pnt>& CPoles,
                                            const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
    throw Standard_OutOfRange();
  if (CPoles.Length() != myPoles.RowLength() || CPoleWeights.Length() != CPoles.Length())
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

  int NbUPoles = myPoles.ColLength();
  int NbVPoles = myPoles.RowLength();

  // Ensure weights are an owned copy for rational insertion
  if (!(myURational || myVRational))
  {
    myWeights = NCollection_Array2<double>(myWeights);
  }

  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles + 1, 1, NbVPoles);
  NCollection_Array2<double> nweights(1, NbUPoles + 1, 1, NbVPoles);

  AddRatPoleRow(myPoles, myWeights, CPoles, CPoleWeights, UIndex, npoles, nweights);

  myPoles   = std::move(npoles);
  myWeights = std::move(nweights);

  Rational(myWeights, myURational, myVRational);
  if (!(myURational || myVRational))
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  }
  myMaxDerivInvOk = false;
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
  if (VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();
  if (myPoles.RowLength() <= 2)
    throw Standard_ConstructionError();

  int                        NbUPoles = myPoles.ColLength();
  int                        NbVPoles = myPoles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles, 1, NbVPoles - 1);

  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles, 1, NbVPoles - 1);

    DeleteRatPoleCol(myPoles, myWeights, VIndex, npoles, nweights);
    Rational(nweights, myURational, myVRational);
    if (myURational || myVRational)
      myWeights = std::move(nweights);
    else
      myWeights = BSplSLib::UnitWeights(NbUPoles, NbVPoles - 1);
  }
  else
  {
    DeletePoleCol(myPoles, VIndex, npoles);
    myWeights = BSplSLib::UnitWeights(NbUPoles, NbVPoles - 1);
  }
  myPoles         = std::move(npoles);
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::RemovePoleRow(const int UIndex)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
    throw Standard_OutOfRange();
  if (myPoles.ColLength() <= 2)
    throw Standard_ConstructionError();

  int                        NbUPoles = myPoles.ColLength();
  int                        NbVPoles = myPoles.RowLength();
  NCollection_Array2<gp_Pnt> npoles(1, NbUPoles - 1, 1, NbVPoles);

  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, NbUPoles - 1, 1, NbVPoles);

    DeleteRatPoleRow(myPoles, myWeights, UIndex, npoles, nweights);

    Rational(nweights, myURational, myVRational);
    if (myURational || myVRational)
      myWeights = std::move(nweights);
    else
      myWeights = BSplSLib::UnitWeights(NbUPoles - 1, NbVPoles);
  }
  else
  {
    DeletePoleRow(myPoles, UIndex, npoles);
    myWeights = BSplSLib::UnitWeights(NbUPoles - 1, NbVPoles);
  }
  myPoles         = std::move(npoles);
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::Segment(const double U1, const double U2, const double V1, const double V2)
{
  const bool rat = (myURational || myVRational);

  const int aMinDegree = UDegree() <= VDegree() ? UDegree() : VDegree();
  const int aMaxDegree = UDegree() > VDegree() ? UDegree() : VDegree();

  NCollection_Array2<gp_Pnt> aCoefs(1, aMaxDegree + 1, 1, aMinDegree + 1);
  NCollection_Array2<double> aWCoefs;
  if (rat)
    aWCoefs.Resize(1, aMaxDegree + 1, 1, aMinDegree + 1, false);

  const double uparameter_11  = 0.5;
  const double uspanlenght_11 = 0.5;
  const double vparameter_11  = 0.5;
  const double vspanlenght_11 = 0.5;

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
                         UKnotSequence(),
                         VKnotSequence(),
                         myPoles,
                         &myWeights,
                         aCoefs,
                         &aWCoefs);
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
                         UKnotSequence(),
                         VKnotSequence(),
                         myPoles,
                         BSplSLib::NoWeights(),
                         aCoefs,
                         BSplSLib::NoWeights());
  }

  // When udeg <= vdeg, u and v are interchanged in the coefficients,
  // so everything needs to be transposed.
  if (UDegree() <= VDegree())
  {
    NCollection_Array2<gp_Pnt> aTransposed(1, UDegree() + 1, 1, VDegree() + 1);
    for (int ii = 1; ii <= UDegree() + 1; ii++)
      for (int jj = 1; jj <= VDegree() + 1; jj++)
        aTransposed(ii, jj) = aCoefs(jj, ii);
    aCoefs = std::move(aTransposed);

    if (rat)
    {
      NCollection_Array2<double> aWTransposed(1, UDegree() + 1, 1, VDegree() + 1);
      for (int ii = 1; ii <= UDegree() + 1; ii++)
        for (int jj = 1; jj <= VDegree() + 1; jj++)
          aWTransposed(ii, jj) = aWCoefs(jj, ii);
      aWCoefs = std::move(aWTransposed);
    }
  }

  // Trim in the canonical basis and update Poles and Coefficients.
  const double ufirst = 2 * (U1 - 0.5), ulast = 2 * (U2 - 0.5), vfirst = 2 * (V1 - 0.5),
               vlast = 2 * (V2 - 0.5);
  if (rat)
  {
    PLib::UTrimming(ufirst, ulast, aCoefs, &aWCoefs);
    PLib::VTrimming(vfirst, vlast, aCoefs, &aWCoefs);
    PLib::CoefficientsPoles(aCoefs, &aWCoefs, myPoles, &myWeights);
  }
  else
  {
    PLib::UTrimming(ufirst, ulast, aCoefs, PLib::NoWeights2());
    PLib::VTrimming(vfirst, vlast, aCoefs, PLib::NoWeights2());
    PLib::CoefficientsPoles(aCoefs, PLib::NoWeights2(), myPoles, PLib::NoWeights2());
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::SetPole(const int UIndex, const int VIndex, const gp_Pnt& P)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();

  myPoles(UIndex, VIndex) = P;
  myMaxDerivInvOk         = false;
}

//=================================================================================================

void Geom_BezierSurface::SetPole(const int     UIndex,
                                 const int     VIndex,
                                 const gp_Pnt& P,
                                 const double  Weight)
{

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierSurface::SetPole");
  if (UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange("Geom_BezierSurface::SetPole");

  myPoles(UIndex, VIndex) = P;

  SetWeight(UIndex, VIndex, Weight);
}

//=================================================================================================

void Geom_BezierSurface::SetPoleCol(const int                         VIndex,
                                    const NCollection_Array1<gp_Pnt>& CPoles,
                                    const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myPoles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myPoles.ColLength() || CPoleWeights.Lower() != CPoles.Lower()
      || CPoleWeights.Upper() != CPoles.Upper())
  {
    throw Standard_ConstructionError();
  }

  int I;
  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myPoles(I, VIndex) = CPoles(I);
  }
  SetWeightCol(VIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BezierSurface::SetPoleCol(const int VIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myPoles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myPoles.ColLength())
  {
    throw Standard_ConstructionError();
  }
  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myPoles(I, VIndex) = CPoles(I);
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::SetPoleRow(const int UIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myPoles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myPoles.RowLength())
    throw Standard_ConstructionError();

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myPoles(UIndex, I) = CPoles(I);
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::SetPoleRow(const int                         UIndex,
                                    const NCollection_Array1<gp_Pnt>& CPoles,
                                    const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
    throw Standard_OutOfRange();

  if (CPoles.Lower() < 1 || CPoles.Lower() > myPoles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myPoles.RowLength() || CPoleWeights.Lower() != CPoles.Lower()
      || CPoleWeights.Upper() != CPoles.Upper())
  {
    throw Standard_ConstructionError();
  }

  int I;

  for (I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myPoles(UIndex, I) = CPoles(I);
  }

  SetWeightRow(UIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BezierSurface::SetWeight(const int UIndex, const int VIndex, const double Weight)
{
  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeight");

  if (UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();

  // compute new rationality
  bool wasrat = (myURational || myVRational);
  if (!wasrat)
  {
    // a weight of 1. does not turn to rational
    if (std::abs(Weight - 1.) <= gp::Resolution())
      return;

    // owned copy from non-owning view
    myWeights = NCollection_Array2<double>(myWeights);
  }

  if (std::abs(Weight - myWeights(UIndex, VIndex)) > gp::Resolution())
  {
    myWeights(UIndex, VIndex) = Weight;
    Rational(myWeights, myURational, myVRational);
    if (!(myURational || myVRational))
    {
      myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
    }
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::SetWeightCol(const int                         VIndex,
                                      const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
    throw Standard_OutOfRange();

  if (CPoleWeights.Length() != myPoles.ColLength())
  {
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeightCol");
  }

  int I;
  // compute new rationality
  bool wasrat = (myURational || myVRational);
  if (!wasrat)
  {
    // owned copy from non-owning view
    myWeights = NCollection_Array2<double>(myWeights);
  }

  I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
    myWeights(I, VIndex) = CPoleWeights(I);
    I++;
  }

  Rational(myWeights, myURational, myVRational);
  if (!(myURational || myVRational))
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::SetWeightRow(const int                         UIndex,
                                      const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
    throw Standard_OutOfRange("Geom_BezierSurface::SetWeightRow");
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > myPoles.RowLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > myPoles.RowLength())
  {
    throw Standard_ConstructionError("Geom_BezierSurface::SetWeightRow");
  }

  int I;
  // compute new rationality
  bool wasrat = (myURational || myVRational);
  if (!wasrat)
  {
    // owned copy from non-owning view
    myWeights = NCollection_Array2<double>(myWeights);
  }

  I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
    myWeights(UIndex, I) = CPoleWeights(I);
    I++;
  }

  Rational(myWeights, myURational, myVRational);
  if (!(myURational || myVRational))
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::UReverse()
{
  gp_Pnt Pol;
  int    Row, Col;
  int    NbUPoles = myPoles.ColLength();
  int    NbVPoles = myPoles.RowLength();
  if (myURational || myVRational)
  {
    double W;
    for (Col = 1; Col <= NbVPoles; Col++)
    {
      for (Row = 1; Row <= NbUPoles / 2; Row++)
      {
        W                                  = myWeights(Row, Col);
        myWeights(Row, Col)                = myWeights(NbUPoles - Row + 1, Col);
        myWeights(NbUPoles - Row + 1, Col) = W;
        Pol                                = myPoles(Row, Col);
        myPoles(Row, Col)                  = myPoles(NbUPoles - Row + 1, Col);
        myPoles(NbUPoles - Row + 1, Col)   = Pol;
      }
    }
  }
  else
  {
    for (Col = 1; Col <= NbVPoles; Col++)
    {
      for (Row = 1; Row <= NbUPoles / 2; Row++)
      {
        Pol                              = myPoles(Row, Col);
        myPoles(Row, Col)                = myPoles(NbUPoles - Row + 1, Col);
        myPoles(NbUPoles - Row + 1, Col) = Pol;
      }
    }
  }
  myMaxDerivInvOk = false;
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
  int    NbUPoles = myPoles.ColLength();
  int    NbVPoles = myPoles.RowLength();
  if (myURational || myVRational)
  {
    double W;
    for (Row = 1; Row <= NbUPoles; Row++)
    {
      for (Col = 1; Col <= NbVPoles / 2; Col++)
      {
        W                                  = myWeights(Row, Col);
        myWeights(Row, Col)                = myWeights(Row, NbVPoles - Col + 1);
        myWeights(Row, NbVPoles - Col + 1) = W;
        Pol                                = myPoles(Row, Col);
        myPoles(Row, Col)                  = myPoles(Row, NbVPoles - Col + 1);
        myPoles(Row, NbVPoles - Col + 1)   = Pol;
      }
    }
  }
  else
  {
    for (Row = 1; Row <= NbUPoles; Row++)
    {
      for (Col = 1; Col <= NbVPoles / 2; Col++)
      {
        Pol                              = myPoles(Row, Col);
        myPoles(Row, Col)                = myPoles(Row, NbVPoles - Col + 1);
        myPoles(Row, NbVPoles - Col + 1) = Pol;
      }
    }
  }
  myMaxDerivInvOk = false;
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
  if (myURational || myVRational)
  {
    BSplSLib::D0(U,
                 V,
                 1,
                 1,
                 myPoles,
                 &myWeights,
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
                 myPoles,
                 BSplSLib::NoWeights(),
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
  if (myURational || myVRational)
  {
    BSplSLib::D1(U,
                 V,
                 1,
                 1,
                 myPoles,
                 &myWeights,
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
                 myPoles,
                 BSplSLib::NoWeights(),
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
  if (myURational || myVRational)
  {
    //-- ATTENTION a l'ORDRE d'appel ds BSPLSLIB
    BSplSLib::D2(U,
                 V,
                 1,
                 1,
                 myPoles,
                 &myWeights,
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
                 myPoles,
                 BSplSLib::NoWeights(),
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
  if (myURational || myVRational)
  {
    BSplSLib::D3(U,
                 V,
                 0,
                 0,
                 myPoles,
                 &myWeights,
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
                 myPoles,
                 BSplSLib::NoWeights(),
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
  if (myURational || myVRational)
  {
    BSplSLib::DN(U,
                 V,
                 Nu,
                 Nv,
                 0,
                 0,
                 myPoles,
                 &myWeights,
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
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
                 myPoles,
                 BSplSLib::NoWeights(),
                 UKnots(),
                 UKnots(),
                 &UMultiplicities(),
                 &VMultiplicities(),
                 (myPoles.ColLength() - 1),
                 (myPoles.RowLength() - 1),
                 myURational,
                 myVRational,
                 false,
                 false,
                 Derivative);
  }
  return Derivative;
}

//=================================================================================================

int Geom_BezierSurface::NbUPoles() const
{
  return myPoles.ColLength();
}

//=================================================================================================

int Geom_BezierSurface::NbVPoles() const
{
  return myPoles.RowLength();
}

//=================================================================================================

const gp_Pnt& Geom_BezierSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1
                                 || VIndex > myPoles.RowLength(),
                               " ");
  return myPoles(UIndex, VIndex);
}

//=================================================================================================

void Geom_BezierSurface::Poles(NCollection_Array2<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.RowLength() != myPoles.RowLength()
                                     || P.ColLength() != myPoles.ColLength(),
                                   " ");
  P = myPoles;
}

//=================================================================================================

int Geom_BezierSurface::UDegree() const
{
  return (myPoles.ColLength() - 1);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BezierSurface::UIso(const double U) const
{
  occ::handle<Geom_BezierCurve> UIsoCurve;
  NCollection_Array1<gp_Pnt>    VCurvePoles(1, myPoles.RowLength());
  if (myURational || myVRational)
  {
    NCollection_Array1<double> VCurveWeights(1, myPoles.RowLength());
    BSplSLib::Iso(U,
                  true,
                  myPoles,
                  &myWeights,
                  UKnots(),
                  &UMultiplicities(),
                  (myPoles.ColLength() - 1),
                  false,
                  VCurvePoles,
                  &VCurveWeights);
    if (myURational)
      UIsoCurve = new Geom_BezierCurve(VCurvePoles, VCurveWeights);
    else
      UIsoCurve = new Geom_BezierCurve(VCurvePoles);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  myPoles,
                  BSplSLib::NoWeights(),
                  UKnots(),
                  &UMultiplicities(),
                  (myPoles.ColLength() - 1),
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
  return (myPoles.RowLength() - 1);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BezierSurface::VIso(const double V) const
{
  occ::handle<Geom_BezierCurve> VIsoCurve;
  NCollection_Array1<gp_Pnt>    VCurvePoles(1, myPoles.ColLength());
  if (myVRational || myURational)
  {
    NCollection_Array1<double> VCurveWeights(1, myPoles.ColLength());
    BSplSLib::Iso(V,
                  false,
                  myPoles,
                  &myWeights,
                  UKnots(),
                  &VMultiplicities(),
                  (myPoles.RowLength() - 1),
                  false,
                  VCurvePoles,
                  &VCurveWeights);
    if (myVRational)
      VIsoCurve = new Geom_BezierCurve(VCurvePoles, VCurveWeights);
    else
      VIsoCurve = new Geom_BezierCurve(VCurvePoles);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  myPoles,
                  BSplSLib::NoWeights(),
                  UKnots(),
                  &VMultiplicities(),
                  (myPoles.RowLength() - 1),
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
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1
                                 || VIndex > myPoles.RowLength(),
                               " ");

  if (myURational || myVRational)
    return myWeights(UIndex, VIndex);
  else
    return 1;
}

//=================================================================================================

void Geom_BezierSurface::Weights(NCollection_Array2<double>& W) const
{
  Standard_DimensionError_Raise_if(W.RowLength() != myPoles.RowLength()
                                     || W.ColLength() != myPoles.ColLength(),
                                   " ");
  if (myURational || myVRational)
    W = myWeights;
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
  return myURational;
}

//=================================================================================================

bool Geom_BezierSurface::IsVRational() const
{
  return myVRational;
}

//=================================================================================================

void Geom_BezierSurface::Transform(const gp_Trsf& T)
{
  for (int I = 1; I <= myPoles.ColLength(); I++)
  {
    for (int J = 1; J <= myPoles.RowLength(); J++)
    {
      myPoles(I, J).Transform(T);
    }
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

bool Geom_BezierSurface::IsUClosed() const
{
  bool Closed = true;
  int  Lower  = myPoles.LowerRow();
  int  Upper  = myPoles.UpperRow();
  int  j      = myPoles.LowerCol();

  while (Closed && j <= myPoles.UpperCol())
  {
    Closed = (myPoles(Lower, j).Distance(myPoles(Upper, j)) <= Precision::Confusion());
    j++;
  }
  return Closed;
}

//=================================================================================================

bool Geom_BezierSurface::IsVClosed() const
{
  bool Closed = true;
  int  Lower  = myPoles.LowerCol();
  int  Upper  = myPoles.UpperCol();
  int  i      = myPoles.LowerRow();
  while (Closed && i <= myPoles.UpperRow())
  {
    Closed = (myPoles(i, Lower).Distance(myPoles(i, Upper)) <= Precision::Confusion());
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
  if (!myMaxDerivInvOk)
  {
    if (myURational || myVRational)
    {
      BSplSLib::Resolution(myPoles,
                           &myWeights,
                           UKnots(),
                           UKnots(),
                           UMultiplicities(),
                           VMultiplicities(),
                           (myPoles.ColLength() - 1),
                           (myPoles.RowLength() - 1),
                           myURational,
                           myVRational,
                           false,
                           false,
                           1.,
                           myUMaxDerivInv,
                           myVMaxDerivInv);
    }
    else
    {
      BSplSLib::Resolution(myPoles,
                           BSplSLib::NoWeights(),
                           UKnots(),
                           UKnots(),
                           UMultiplicities(),
                           VMultiplicities(),
                           (myPoles.ColLength() - 1),
                           (myPoles.RowLength() - 1),
                           myURational,
                           myVRational,
                           false,
                           false,
                           1.,
                           myUMaxDerivInv,
                           myVMaxDerivInv);
    }
    myMaxDerivInvOk = true;
  }
  UTolerance = Tolerance3D * myUMaxDerivInv;
  VTolerance = Tolerance3D * myVMaxDerivInv;
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_BezierSurface::Copy() const
{
  return new Geom_BezierSurface(*this);
}

//=================================================================================================

void Geom_BezierSurface::init(const NCollection_Array2<gp_Pnt>& thePoles,
                              const NCollection_Array2<double>* theWeights)
{
  int NbUPoles = thePoles.ColLength();
  int NbVPoles = thePoles.RowLength();

  myPoles.Resize(1, NbUPoles, 1, NbVPoles, false);
  myPoles = thePoles;

  if (theWeights != nullptr)
  {
    myWeights.Resize(1, NbUPoles, 1, NbVPoles, false);
    myWeights = *theWeights;
    Rational(myWeights, myURational, myVRational);
    if (!(myURational || myVRational))
    {
      myWeights = BSplSLib::UnitWeights(NbUPoles, NbVPoles);
    }
  }
  else
  {
    myURational = false;
    myVRational = false;
    myWeights   = BSplSLib::UnitWeights(NbUPoles, NbVPoles);
  }

  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BezierSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myURational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVRational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())
  if (myURational || myVRational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWeights.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUMaxDerivInv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVMaxDerivInv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDerivInvOk)
}

//=================================================================================================

const NCollection_Array1<double>& Geom_BezierSurface::UKnots() const
{
  static const double                     THE_DATA[2] = {0.0, 1.0};
  static const NCollection_Array1<double> THE_KNOTS(THE_DATA[0], 1, 2);
  return THE_KNOTS;
}

//=================================================================================================

const NCollection_Array1<double>& Geom_BezierSurface::VKnots() const
{
  return UKnots();
}

//=================================================================================================

const NCollection_Array1<int>& Geom_BezierSurface::UMultiplicities() const
{
  Standard_ProgramError_Raise_if(myPoles.IsEmpty(), "Geom_BezierSurface: empty poles");
  static constexpr int THE_MAX_SIZE = BSplCLib::MaxDegree() + 1;
  static const auto    THE_DATA     = []() {
    std::array<std::array<int, 2>, THE_MAX_SIZE> anArr;
    for (int i = 0; i < THE_MAX_SIZE; ++i)
      anArr[i] = {i + 1, i + 1};
    return anArr;
  }();
  static const auto THE_MULTS = []() {
    std::array<NCollection_Array1<int>, THE_MAX_SIZE> anArr;
    for (int i = 0; i < THE_MAX_SIZE; ++i)
      anArr[i] = NCollection_Array1<int>(THE_DATA[i][0], 1, 2);
    return anArr;
  }();
  return THE_MULTS[myPoles.ColLength() - 1];
}

//=================================================================================================

const NCollection_Array1<int>& Geom_BezierSurface::VMultiplicities() const
{
  Standard_ProgramError_Raise_if(myPoles.IsEmpty(), "Geom_BezierSurface: empty poles");
  static constexpr int THE_MAX_SIZE = BSplCLib::MaxDegree() + 1;
  static const auto    THE_DATA     = []() {
    std::array<std::array<int, 2>, THE_MAX_SIZE> anArr;
    for (int i = 0; i < THE_MAX_SIZE; ++i)
      anArr[i] = {i + 1, i + 1};
    return anArr;
  }();
  static const auto THE_MULTS = []() {
    std::array<NCollection_Array1<int>, THE_MAX_SIZE> anArr;
    for (int i = 0; i < THE_MAX_SIZE; ++i)
      anArr[i] = NCollection_Array1<int>(THE_DATA[i][0], 1, 2);
    return anArr;
  }();
  return THE_MULTS[myPoles.RowLength() - 1];
}

//=================================================================================================

const NCollection_Array1<double>& Geom_BezierSurface::UKnotSequence() const
{
  Standard_ProgramError_Raise_if(myPoles.IsEmpty(), "Geom_BezierSurface: empty poles");
  static constexpr int THE_MAX_SIZE = BSplCLib::MaxDegree() + 1;
  static const auto    THE_FKNOTS   = []() {
    std::array<NCollection_Array1<double>, THE_MAX_SIZE> anArr;
    for (int i = 1; i <= BSplCLib::MaxDegree(); ++i)
      anArr[i] = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(i), 1, 2 * (i + 1));
    return anArr;
  }();
  return THE_FKNOTS[myPoles.ColLength() - 1];
}

//=================================================================================================

const NCollection_Array1<double>& Geom_BezierSurface::VKnotSequence() const
{
  Standard_ProgramError_Raise_if(myPoles.IsEmpty(), "Geom_BezierSurface: empty poles");
  static constexpr int THE_MAX_SIZE = BSplCLib::MaxDegree() + 1;
  static const auto    THE_FKNOTS   = []() {
    std::array<NCollection_Array1<double>, THE_MAX_SIZE> anArr;
    for (int i = 1; i <= BSplCLib::MaxDegree(); ++i)
      anArr[i] = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(i), 1, 2 * (i + 1));
    return anArr;
  }();
  return THE_FKNOTS[myPoles.RowLength() - 1];
}
