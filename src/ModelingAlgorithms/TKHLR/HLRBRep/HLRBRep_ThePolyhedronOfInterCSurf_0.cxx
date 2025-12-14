// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>

#include <Bnd_Array1OfBox.hxx>
#include <Bnd_Box.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <HLRBRep_SurfaceTool.hxx>
#include <Standard_OutOfRange.hxx>

#include "IntCurveSurface_PolyhedronUtils.pxx"

#define LONGUEUR_MINI_EDGE_TRIANGLE 1e-15

//==================================================================================================

HLRBRep_ThePolyhedronOfInterCSurf::HLRBRep_ThePolyhedronOfInterCSurf(
  const Standard_Address& Surface,
  const Standard_Integer  nbdU,
  const Standard_Integer  nbdV,
  const Standard_Real     u1,
  const Standard_Real     v1,
  const Standard_Real     u2,
  const Standard_Real     v2)
    : nbdeltaU((nbdU < 3) ? 3 : nbdU),
      nbdeltaV((nbdV < 3) ? 3 : nbdV),
      TheDeflection(Epsilon(100.)),
      C_MyPnts(NULL),
      C_MyU(NULL),
      C_MyV(NULL),
      C_MyIsOnBounds(NULL)
{
  Standard_Integer t = (nbdeltaU + 1) * (nbdeltaV + 1) + 1;
  C_MyPnts           = new gp_Pnt[t];
  C_MyU              = new Standard_Real[t];
  C_MyV              = new Standard_Real[t];
  C_MyIsOnBounds     = new Standard_Boolean[t];
  Init(Surface, u1, v1, u2, v2);
}

//==================================================================================================

HLRBRep_ThePolyhedronOfInterCSurf::HLRBRep_ThePolyhedronOfInterCSurf(
  const Standard_Address&     Surface,
  const TColStd_Array1OfReal& Upars,
  const TColStd_Array1OfReal& Vpars)
    : nbdeltaU(Upars.Length() - 1),
      nbdeltaV(Vpars.Length() - 1),
      TheDeflection(Epsilon(100.)),
      C_MyPnts(NULL),
      C_MyU(NULL),
      C_MyV(NULL),
      C_MyIsOnBounds(NULL)
{
  Standard_Integer t = (nbdeltaU + 1) * (nbdeltaV + 1) + 1;
  C_MyPnts           = new gp_Pnt[t];
  C_MyU              = new Standard_Real[t];
  C_MyV              = new Standard_Real[t];
  C_MyIsOnBounds     = new Standard_Boolean[t];
  Init(Surface, Upars, Vpars);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Destroy()
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  if (C_MyPnts)
    delete[] CMyPnts;
  Standard_Real* CMyU = (Standard_Real*)C_MyU;
  if (C_MyU)
    delete[] CMyU;
  Standard_Real* CMyV = (Standard_Real*)C_MyV;
  if (C_MyV)
    delete[] CMyV;
  Standard_Boolean* CMyIsOnBounds = (Standard_Boolean*)C_MyIsOnBounds;
  if (C_MyIsOnBounds)
    delete[] CMyIsOnBounds;

  C_MyPnts = C_MyU = C_MyV = C_MyIsOnBounds = NULL;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Init(const Standard_Address& Surface,
                                             const Standard_Real     U0,
                                             const Standard_Real     V0,
                                             const Standard_Real     U1,
                                             const Standard_Real     V1)
{
  IntCurveSurface_PolyhedronUtils::InitUniform<Standard_Address, HLRBRep_SurfaceTool>(
    Surface,
    U0,
    V0,
    U1,
    V1,
    nbdeltaU,
    nbdeltaV,
    (gp_Pnt*)C_MyPnts,
    (Standard_Real*)C_MyU,
    (Standard_Real*)C_MyV,
    (Standard_Boolean*)C_MyIsOnBounds,
    TheBnd);

  // Calculate triangle deflections
  Standard_Real    tol         = 0.0;
  Standard_Integer nbtriangles = NbTriangles();
  for (Standard_Integer i1 = 1; i1 <= nbtriangles; i1++)
  {
    Standard_Real tol1 = DeflectionOnTriangle(Surface, i1);
    if (tol1 > tol)
      tol = tol1;
  }

  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  // Compute border deflection
  TheBorderDeflection = RealFirst();
  Standard_Real aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, U0, V0, V1, Standard_True);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, U1, V0, V1, Standard_True);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, V0, U0, U1, Standard_False);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, V1, U0, U1, Standard_False);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Init(const Standard_Address&     Surface,
                                             const TColStd_Array1OfReal& Upars,
                                             const TColStd_Array1OfReal& Vpars)
{
  IntCurveSurface_PolyhedronUtils::InitWithParams<Standard_Address, HLRBRep_SurfaceTool>(
    Surface,
    Upars,
    Vpars,
    nbdeltaU,
    nbdeltaV,
    (gp_Pnt*)C_MyPnts,
    (Standard_Real*)C_MyU,
    (Standard_Real*)C_MyV,
    (Standard_Boolean*)C_MyIsOnBounds,
    TheBnd);

  // Calculate triangle deflections
  Standard_Real    tol         = 0.0;
  Standard_Integer nbtriangles = NbTriangles();
  for (Standard_Integer i1 = 1; i1 <= nbtriangles; i1++)
  {
    Standard_Real tol1 = DeflectionOnTriangle(Surface, i1);
    if (tol1 > tol)
      tol = tol1;
  }

  DeflectionOverEstimation(tol * 1.2);
  FillBounding();

  // Compute border deflection
  TheBorderDeflection     = RealFirst();
  Standard_Integer i0     = Upars.Lower();
  Standard_Integer j0     = Vpars.Lower();
  Standard_Real    U0     = Upars(i0);
  Standard_Real    V0     = Vpars(j0);
  Standard_Real    U1     = Upars(Upars.Upper());
  Standard_Real    V1     = Vpars(Vpars.Upper());
  Standard_Real    aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, U0, V0, V1, Standard_True);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, U1, V0, V1, Standard_True);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, V0, U0, U1, Standard_False);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;

  aDeflection = ComputeBorderDeflection(Surface, V1, U0, U1, Standard_False);
  if (aDeflection > TheBorderDeflection)
    TheBorderDeflection = aDeflection;
}

//==================================================================================================

Standard_Real HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOnTriangle(const Standard_Address& Surface,
                                                                      const Standard_Integer Triang) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);

  Standard_Real u1, v1, u2, v2, u3, v3;
  gp_Pnt        P1 = Point(i1, u1, v1);
  gp_Pnt        P2 = Point(i2, u2, v2);
  gp_Pnt        P3 = Point(i3, u3, v3);

  return IntCurveSurface_PolyhedronUtils::
    DeflectionOnTriangle<Standard_Address, HLRBRep_SurfaceTool>(Surface,
                                                                P1,
                                                                P2,
                                                                P3,
                                                                u1,
                                                                v1,
                                                                u2,
                                                                v2,
                                                                u3,
                                                                v3);
}

//==================================================================================================

Standard_Real HLRBRep_ThePolyhedronOfInterCSurf::ComputeBorderDeflection(
  const Standard_Address& Surface,
  const Standard_Real     Parameter,
  const Standard_Real     PMin,
  const Standard_Real     PMax,
  const Standard_Boolean  isUIso) const
{
  Standard_Integer aNbSamples = isUIso ? nbdeltaV : nbdeltaU;

  return IntCurveSurface_PolyhedronUtils::
    ComputeBorderDeflection<Standard_Address, HLRBRep_SurfaceTool>(Surface,
                                                                   Parameter,
                                                                   PMin,
                                                                   PMax,
                                                                   isUIso,
                                                                   aNbSamples);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Parameters(const Standard_Integer Index,
                                                   Standard_Real&         U,
                                                   Standard_Real&         V) const
{
  Standard_Real* CMyU = (Standard_Real*)C_MyU;
  U                   = CMyU[Index];
  Standard_Real* CMyV = (Standard_Real*)C_MyV;
  V                   = CMyV[Index];
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOverEstimation(const Standard_Real flec)
{
  if (flec < 0.0001)
  {
    TheDeflection = 0.0001;
    TheBnd.Enlarge(0.0001);
  }
  else
  {
    TheDeflection = flec;
    TheBnd.Enlarge(flec);
  }
}

//==================================================================================================

Standard_Real HLRBRep_ThePolyhedronOfInterCSurf::DeflectionOverEstimation() const
{
  return TheDeflection;
}

//==================================================================================================

const Bnd_Box& HLRBRep_ThePolyhedronOfInterCSurf::Bounding() const
{
  return TheBnd;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::FillBounding()
{
  TheComponentsBnd = new Bnd_HArray1OfBox(1, NbTriangles());
  Bnd_Box          Boite;
  Standard_Integer np1, np2, np3;
  Standard_Integer nbtriangles = NbTriangles();
  for (Standard_Integer iTri = 1; iTri <= nbtriangles; iTri++)
  {
    Triangle(iTri, np1, np2, np3);
    gp_Pnt p1(Point(np1));
    gp_Pnt p2(Point(np2));
    gp_Pnt p3(Point(np3));
    Boite.SetVoid();
    if (p1.SquareDistance(p2) > LONGUEUR_MINI_EDGE_TRIANGLE)
    {
      if (p1.SquareDistance(p3) > LONGUEUR_MINI_EDGE_TRIANGLE)
      {
        if (p2.SquareDistance(p3) > LONGUEUR_MINI_EDGE_TRIANGLE)
        {
          Boite.Add(p1);
          Boite.Add(p2);
          Boite.Add(p3);
          Boite.Enlarge(TheDeflection);
        }
      }
    }
    Boite.Enlarge(TheDeflection);
    TheComponentsBnd->SetValue(iTri, Boite);
  }
}

//==================================================================================================

const Handle(Bnd_HArray1OfBox)& HLRBRep_ThePolyhedronOfInterCSurf::ComponentsBounding() const
{
  return TheComponentsBnd;
}

//==================================================================================================

Standard_Integer HLRBRep_ThePolyhedronOfInterCSurf::NbTriangles() const
{
  return nbdeltaU * nbdeltaV * 2;
}

//==================================================================================================

Standard_Integer HLRBRep_ThePolyhedronOfInterCSurf::NbPoints() const
{
  return (nbdeltaU + 1) * (nbdeltaV + 1);
}

//==================================================================================================

Standard_Integer HLRBRep_ThePolyhedronOfInterCSurf::TriConnex(const Standard_Integer Triang,
                                                              const Standard_Integer Pivot,
                                                              const Standard_Integer Pedge,
                                                              Standard_Integer&      TriCon,
                                                              Standard_Integer&      OtherP) const
{
  Standard_Integer Pivotm1    = Pivot - 1;
  Standard_Integer nbdeltaVp1 = nbdeltaV + 1;
  Standard_Integer nbdeltaVm2 = nbdeltaV + nbdeltaV;

  Standard_Integer ligP = Pivotm1 / nbdeltaVp1;
  Standard_Integer colP = Pivotm1 - ligP * nbdeltaVp1;

  Standard_Integer ligE = 0, colE = 0, typE = 0;
  if (Pedge != 0)
  {
    ligE = (Pedge - 1) / nbdeltaVp1;
    colE = (Pedge - 1) - (ligE * nbdeltaVp1);
    if (ligP == ligE)
      typE = 1;
    else if (colP == colE)
      typE = 2;
    else
      typE = 3;
  }

  Standard_Integer linT = 0, colT = 0;
  Standard_Integer linO = 0, colO = 0;
  Standard_Integer t = 0, tt = 0;

  if (Triang != 0)
  {
    t    = (Triang - 1) / nbdeltaVm2;
    tt   = (Triang - 1) - t * nbdeltaVm2;
    linT = 1 + t;
    colT = 1 + tt;
    if (typE == 0)
    {
      if (ligP == linT)
      {
        ligE = ligP - 1;
        colE = colP - 1;
        typE = 3;
      }
      else
      {
        if (colT == ligP + ligP)
        {
          ligE = ligP;
          colE = colP - 1;
          typE = 1;
        }
        else
        {
          ligE = ligP + 1;
          colE = colP + 1;
          typE = 3;
        }
      }
    }
    switch (typE)
    {
      case 1:
        if (linT == ligP)
        {
          linT++;
          linO = ligP + 1;
          colO = (colP > colE) ? colP : colE;
        }
        else
        {
          linT--;
          linO = ligP - 1;
          colO = (colP < colE) ? colP : colE;
        }
        break;
      case 2:
        if (colT == (colP + colP))
        {
          colT++;
          linO = (ligP > ligE) ? ligP : ligE;
          colO = colP + 1;
        }
        else
        {
          colT--;
          linO = (ligP < ligE) ? ligP : ligE;
          colO = colP - 1;
        }
        break;
      case 3:
        if ((colT & 1) == 0)
        {
          colT--;
          linO = (ligP > ligE) ? ligP : ligE;
          colO = (colP < colE) ? colP : colE;
        }
        else
        {
          colT++;
          linO = (ligP < ligE) ? ligP : ligE;
          colO = (colP > colE) ? colP : colE;
        }
        break;
    }
  }
  else
  {
    if (Pedge == 0)
    {
      linT = (1 > ligP) ? 1 : ligP;
      colT = (1 > (colP + colP)) ? 1 : (colP + colP);
      if (ligP == 0)
        linO = ligP + 1;
      else
        linO = ligP - 1;
      colO = colP;
    }
    else
    {
      switch (typE)
      {
        case 1:
          linT = ligP + 1;
          colT = (colP > colE) ? colP : colE;
          colT += colT;
          linO = ligP + 1;
          colO = (colP > colE) ? colP : colE;
          break;
        case 2:
          linT = (ligP > ligE) ? ligP : ligE;
          colT = colP + colP;
          linO = (ligP < ligE) ? ligP : ligE;
          colO = colP - 1;
          break;
        case 3:
          linT = (ligP > ligE) ? ligP : ligE;
          colT = colP + colE;
          linO = (ligP > ligE) ? ligP : ligE;
          colO = (colP < colE) ? colP : colE;
          break;
      }
    }
  }

  TriCon = (linT - 1) * nbdeltaVm2 + colT;

  if (linT < 1)
  {
    linO = 0;
    colO = colP + colP - colE;
    if (colO < 0)
    {
      colO = 0;
      linO = 1;
    }
    else if (colO > nbdeltaV)
    {
      colO = nbdeltaV;
      linO = 1;
    }
    TriCon = 0;
  }
  else if (linT > nbdeltaU)
  {
    linO = nbdeltaU;
    colO = colP + colP - colE;
    if (colO < 0)
    {
      colO = 0;
      linO = nbdeltaU - 1;
    }
    else if (colO > nbdeltaV)
    {
      colO = nbdeltaV;
      linO = nbdeltaU - 1;
    }
    TriCon = 0;
  }

  if (colT < 1)
  {
    colO = 0;
    linO = ligP + ligP - ligE;
    if (linO < 0)
    {
      linO = 0;
      colO = 1;
    }
    else if (linO > nbdeltaU)
    {
      linO = nbdeltaU;
      colO = 1;
    }
    TriCon = 0;
  }
  else if (colT > nbdeltaV)
  {
    colO = nbdeltaV;
    linO = ligP + ligP - ligE;
    if (linO < 0)
    {
      linO = 0;
      colO = nbdeltaV - 1;
    }
    else if (linO > nbdeltaU)
    {
      linO = nbdeltaU;
      colO = nbdeltaV - 1;
    }
    TriCon = 0;
  }

  OtherP = linO * nbdeltaVp1 + colO + 1;

  return TriCon;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::PlaneEquation(const Standard_Integer Triang,
                                                      gp_XYZ&                NormalVector,
                                                      Standard_Real&         PolarDistance) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);

  gp_XYZ Pointi1(Point(i1).XYZ());
  gp_XYZ Pointi2(Point(i2).XYZ());
  gp_XYZ Pointi3(Point(i3).XYZ());

  gp_XYZ v1 = Pointi2 - Pointi1;
  gp_XYZ v2 = Pointi3 - Pointi2;
  gp_XYZ v3 = Pointi1 - Pointi3;

  if (v1.SquareModulus() <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    NormalVector.SetCoord(1.0, 0.0, 0.0);
    return;
  }
  if (v2.SquareModulus() <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    NormalVector.SetCoord(1.0, 0.0, 0.0);
    return;
  }
  if (v3.SquareModulus() <= LONGUEUR_MINI_EDGE_TRIANGLE)
  {
    NormalVector.SetCoord(1.0, 0.0, 0.0);
    return;
  }

  NormalVector           = (v1 ^ v2) + (v2 ^ v3) + (v3 ^ v1);
  Standard_Real aNormLen = NormalVector.Modulus();
  if (aNormLen < gp::Resolution())
  {
    PolarDistance = 0.;
  }
  else
  {
    NormalVector.Divide(aNormLen);
    PolarDistance = NormalVector * Point(i1).XYZ();
  }
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::Contain(const Standard_Integer Triang,
                                                            const gp_Pnt&          ThePnt) const
{
  Standard_Integer i1, i2, i3;
  Triangle(Triang, i1, i2, i3);
  gp_XYZ Pointi1(Point(i1).XYZ());
  gp_XYZ Pointi2(Point(i2).XYZ());
  gp_XYZ Pointi3(Point(i3).XYZ());

  gp_XYZ v1 = (Pointi2 - Pointi1) ^ (ThePnt.XYZ() - Pointi1);
  gp_XYZ v2 = (Pointi3 - Pointi2) ^ (ThePnt.XYZ() - Pointi2);
  gp_XYZ v3 = (Pointi1 - Pointi3) ^ (ThePnt.XYZ() - Pointi3);
  if (v1 * v2 >= 0. && v2 * v3 >= 0. && v3 * v1 >= 0.)
    return Standard_True;
  else
    return Standard_False;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Dump() const {}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Size(Standard_Integer& nbdu, Standard_Integer& nbdv) const
{
  nbdu = nbdeltaU;
  nbdv = nbdeltaV;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Triangle(const Standard_Integer Index,
                                                 Standard_Integer&      P1,
                                                 Standard_Integer&      P2,
                                                 Standard_Integer&      P3) const
{
  Standard_Integer line   = 1 + ((Index - 1) / (nbdeltaV * 2));
  Standard_Integer colon  = 1 + ((Index - 1) % (nbdeltaV * 2));
  Standard_Integer colpnt = (colon + 1) / 2;

  P1 = (line - 1) * (nbdeltaV + 1) + colpnt;
  P2 = line * (nbdeltaV + 1) + colpnt + ((colon - 1) % 2);
  P3 = (line - 1 + (colon % 2)) * (nbdeltaV + 1) + colpnt + 1;
}

//==================================================================================================

const gp_Pnt& HLRBRep_ThePolyhedronOfInterCSurf::Point(const Standard_Integer Index,
                                                       Standard_Real&         U,
                                                       Standard_Real&         V) const
{
  gp_Pnt*        CMyPnts = (gp_Pnt*)C_MyPnts;
  Standard_Real* CMyU    = (Standard_Real*)C_MyU;
  Standard_Real* CMyV    = (Standard_Real*)C_MyV;
  U                      = CMyU[Index];
  V                      = CMyV[Index];
  return CMyPnts[Index];
}

//==================================================================================================

const gp_Pnt& HLRBRep_ThePolyhedronOfInterCSurf::Point(const Standard_Integer Index) const
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  return CMyPnts[Index];
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Point(const gp_Pnt&,
                                              const Standard_Integer,
                                              const Standard_Integer,
                                              const Standard_Real,
                                              const Standard_Real)
{
  // Should not be called
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::Point(const Standard_Integer Index, gp_Pnt& P) const
{
  gp_Pnt* CMyPnts = (gp_Pnt*)C_MyPnts;
  P               = CMyPnts[Index];
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::IsOnBound(const Standard_Integer Index1,
                                                              const Standard_Integer Index2) const
{
  Standard_Boolean* CMyIsOnBounds = (Standard_Boolean*)C_MyIsOnBounds;
  Standard_Integer  aDiff         = std::abs(Index1 - Index2);

  if (aDiff != 1 && aDiff != nbdeltaV + 1)
    return Standard_False;

  for (Standard_Integer i = 0; i <= nbdeltaU; i++)
  {
    if ((Index1 == 1 + i * (nbdeltaV + 1)) && (Index2 == Index1 - 1))
      return Standard_False;

    if ((Index1 == (1 + i) * (nbdeltaV + 1)) && (Index2 == Index1 + 1))
      return Standard_False;
  }

  return (CMyIsOnBounds[Index1] && CMyIsOnBounds[Index2]);
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::UMinSingularity(const Standard_Boolean Sing)
{
  UMinSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::UMaxSingularity(const Standard_Boolean Sing)
{
  UMaxSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::VMinSingularity(const Standard_Boolean Sing)
{
  VMinSingular = Sing;
}

//==================================================================================================

void HLRBRep_ThePolyhedronOfInterCSurf::VMaxSingularity(const Standard_Boolean Sing)
{
  VMaxSingular = Sing;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasUMinSingularity() const
{
  return UMinSingular;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasUMaxSingularity() const
{
  return UMaxSingular;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasVMinSingularity() const
{
  return VMinSingular;
}

//==================================================================================================

Standard_Boolean HLRBRep_ThePolyhedronOfInterCSurf::HasVMaxSingularity() const
{
  return VMaxSingular;
}
