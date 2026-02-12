// Created on: 1991-03-08
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Bnd_Box.hxx>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Macro.hxx>
#include <iostream>

namespace
{
// Precision constant for infinite bounds
constexpr double THE_BND_PRECISION_INFINITE = 1e+100;

// Precomputed unit direction vectors for bounding box transformations
constexpr gp_Dir THE_DIR_XMIN{gp_Dir::D::NX};
constexpr gp_Dir THE_DIR_XMAX{gp_Dir::D::X};
constexpr gp_Dir THE_DIR_YMIN{gp_Dir::D::NY};
constexpr gp_Dir THE_DIR_YMAX{gp_Dir::D::Y};
constexpr gp_Dir THE_DIR_ZMIN{gp_Dir::D::NZ};
constexpr gp_Dir THE_DIR_ZMAX{gp_Dir::D::Z};

// Computes minimum squared distance between two 1D intervals
inline double DistMini2Box(const double theR1Min,
                           const double theR1Max,
                           const double theR2Min,
                           const double theR2Max) noexcept
{
  const double aR1 = Square(theR1Min - theR2Max);
  const double aR2 = Square(theR1Max - theR2Min);
  return std::min(aR1, aR2);
}

// Computes squared distance in one dimension, returns 0 if intervals overlap
inline double DistanceInDimension(const double theMin1,
                                  const double theMax1,
                                  const double theMin2,
                                  const double theMax2) noexcept
{
  // Check if intervals overlap
  if ((theMin1 <= theMin2 && theMin2 <= theMax1) || (theMin2 <= theMin1 && theMin1 <= theMax2))
    return 0.0;
  return DistMini2Box(theMin1, theMax1, theMin2, theMax2);
}

// Tests if a 2D segment is outside a 2D box
bool IsSegmentOut(const double theX1,
                  const double theY1,
                  const double theX2,
                  const double theY2,
                  const double theXs1,
                  const double theYs1,
                  const double theXs2,
                  const double theYs2) noexcept
{
  constexpr double anEps  = RealSmall();
  const double     aXsMin = std::min(theXs1, theXs2);
  const double     aXsMax = std::max(theXs1, theXs2);
  const double     aYsMin = std::min(theYs1, theYs2);
  const double     aYsMax = std::max(theYs1, theYs2);

  if (aYsMax - aYsMin < anEps && (theY1 - theYs1 < anEps && theYs1 - theY2 < anEps)
      && ((aXsMin - theX1 < anEps && theX1 - aXsMax < anEps)
          || (aXsMin - theX2 < anEps && theX2 - aXsMax < anEps)
          || (theX1 - theXs1 < anEps && theXs1 - theX2 < anEps)))
    return false;
  if (aXsMax - aXsMin < anEps && (theX1 - theXs1 < anEps && theXs1 - theX2 < anEps)
      && ((aYsMin - theY1 < anEps && theY1 - aYsMax < anEps)
          || (aYsMin - theY2 < anEps && theY2 - aYsMax < anEps)
          || (theY1 - theYs1 < anEps && theYs1 - theY2 < anEps)))
    return false;

  if ((theXs1 < theX1 && theXs2 < theX1) || (theXs1 > theX2 && theXs2 > theX2)
      || (theYs1 < theY1 && theYs2 < theY1) || (theYs1 > theY2 && theYs2 > theY2))
    return true;

  if (std::abs(theXs2 - theXs1) > anEps)
  {
    const double aYa =
      (std::min(theX1, theX2) - theXs1) * (theYs2 - theYs1) / (theXs2 - theXs1) + theYs1;
    const double aYb =
      (std::max(theX1, theX2) - theXs1) * (theYs2 - theYs1) / (theXs2 - theXs1) + theYs1;
    if ((aYa < theY1 && aYb < theY1) || (aYa > theY2 && aYb > theY2))
      return true;
  }
  else if (std::abs(theYs2 - theYs1) > anEps)
  {
    const double aXa =
      (std::min(theY1, theY2) - theYs1) * (theXs2 - theXs1) / (theYs2 - theYs1) + theXs1;
    const double aXb =
      (std::max(theY1, theY2) - theYs1) * (theXs2 - theXs1) / (theYs2 - theYs1) + theXs1;
    if ((aXa < theX1 && aXb < theX1) || (aXa > theX2 && aXb > theX2))
      return true;
  }
  else
    return true;

  return false;
}
} // anonymous namespace

//=================================================================================================

void Bnd_Box::Set(const gp_Pnt& P)
{
  SetVoid();
  Add(P);
}

//=================================================================================================

void Bnd_Box::Set(const gp_Pnt& P, const gp_Dir& D)
{
  SetVoid();
  Add(P, D);
}

//=================================================================================================

void Bnd_Box::Update(const double x,
                     const double y,
                     const double z,
                     const double X,
                     const double Y,
                     const double Z)
{
  if (IsVoid())
  {
    Xmin = x;
    Ymin = y;
    Zmin = z;
    Xmax = X;
    Ymax = Y;
    Zmax = Z;
    Flags &= ~VoidMask;
  }
  else
  {
    Xmin = std::min(Xmin, x);
    Xmax = std::max(Xmax, X);
    Ymin = std::min(Ymin, y);
    Ymax = std::max(Ymax, Y);
    Zmin = std::min(Zmin, z);
    Zmax = std::max(Zmax, Z);
  }
}

//=================================================================================================

void Bnd_Box::Update(const double X, const double Y, const double Z)
{
  if (IsVoid())
  {
    Xmin = X;
    Ymin = Y;
    Zmin = Z;
    Xmax = X;
    Ymax = Y;
    Zmax = Z;
    Flags &= ~VoidMask;
  }
  else
  {
    Xmin = std::min(Xmin, X);
    Xmax = std::max(Xmax, X);
    Ymin = std::min(Ymin, Y);
    Ymax = std::max(Ymax, Y);
    Zmin = std::min(Zmin, Z);
    Zmax = std::max(Zmax, Z);
  }
}

//=================================================================================================

void Bnd_Box::Get(double& theXmin,
                  double& theYmin,
                  double& theZmin,
                  double& theXmax,
                  double& theYmax,
                  double& theZmax) const
{
  if (IsVoid())
  {
    throw Standard_ConstructionError("Bnd_Box is void");
  }

  theXmin = GetXMin();
  theXmax = GetXMax();
  theYmin = GetYMin();
  theYmax = GetYMax();
  theZmin = GetZMin();
  theZmax = GetZMax();
}

//=================================================================================================

Bnd_Box::Limits Bnd_Box::Get() const
{
  return {GetXMin(), GetXMax(), GetYMin(), GetYMax(), GetZMin(), GetZMax()};
}

//=================================================================================================

double Bnd_Box::GetXMin() const
{
  return IsOpenXmin() ? -THE_BND_PRECISION_INFINITE : Xmin - Gap;
}

//=================================================================================================

double Bnd_Box::GetXMax() const
{
  return IsOpenXmax() ? THE_BND_PRECISION_INFINITE : Xmax + Gap;
}

//=================================================================================================

double Bnd_Box::GetYMin() const
{
  return IsOpenYmin() ? -THE_BND_PRECISION_INFINITE : Ymin - Gap;
}

//=================================================================================================

double Bnd_Box::GetYMax() const
{
  return IsOpenYmax() ? THE_BND_PRECISION_INFINITE : Ymax + Gap;
}

//=================================================================================================

double Bnd_Box::GetZMin() const
{
  return IsOpenZmin() ? -THE_BND_PRECISION_INFINITE : Zmin - Gap;
}

//=================================================================================================

double Bnd_Box::GetZMax() const
{
  return IsOpenZmax() ? THE_BND_PRECISION_INFINITE : Zmax + Gap;
}

//=================================================================================================

gp_Pnt Bnd_Box::CornerMin() const
{
  if (IsVoid())
  {
    throw Standard_ConstructionError("Bnd_Box is void");
  }
  return gp_Pnt(IsOpenXmin() ? -THE_BND_PRECISION_INFINITE : Xmin - Gap,
                IsOpenYmin() ? -THE_BND_PRECISION_INFINITE : Ymin - Gap,
                IsOpenZmin() ? -THE_BND_PRECISION_INFINITE : Zmin - Gap);
}

//=================================================================================================

gp_Pnt Bnd_Box::CornerMax() const
{
  if (IsVoid())
  {
    throw Standard_ConstructionError("Bnd_Box is void");
  }
  return gp_Pnt(IsOpenXmax() ? THE_BND_PRECISION_INFINITE : Xmax + Gap,
                IsOpenYmax() ? THE_BND_PRECISION_INFINITE : Ymax + Gap,
                IsOpenZmax() ? THE_BND_PRECISION_INFINITE : Zmax + Gap);
}

//=================================================================================================

std::optional<gp_Pnt> Bnd_Box::Center() const
{
  if (IsVoid())
  {
    return std::nullopt;
  }
  return gp_Pnt(0.5 * (GetXMin() + GetXMax()),
                0.5 * (GetYMin() + GetYMax()),
                0.5 * (GetZMin() + GetZMax()));
}

//=================================================================================================

bool Bnd_Box::IsXThin(const double tol) const
{
  if (IsWhole())
    return false;
  if (IsVoid())
    return true;
  if (IsOpenXmin())
    return false;
  if (IsOpenXmax())
    return false;
  if (Xmax - Xmin < tol)
    return true;
  return false;
}

//=================================================================================================

bool Bnd_Box::IsYThin(const double tol) const
{
  if (IsWhole())
    return false;
  if (IsVoid())
    return true;
  if (IsOpenYmin())
    return false;
  if (IsOpenYmax())
    return false;
  if (Ymax - Ymin < tol)
    return true;
  return false;
}

//=================================================================================================

bool Bnd_Box::IsZThin(const double tol) const
{
  if (IsWhole())
    return false;
  if (IsVoid())
    return true;
  if (IsOpenZmin())
    return false;
  if (IsOpenZmax())
    return false;
  if (Zmax - Zmin < tol)
    return true;
  return false;
}

//=================================================================================================

bool Bnd_Box::IsThin(const double tol) const
{
  if (IsWhole())
    return false;
  if (IsVoid())
    return true;
  if (IsOpenXmin() || IsOpenXmax() || Xmax - Xmin >= tol)
    return false;
  if (IsOpenYmin() || IsOpenYmax() || Ymax - Ymin >= tol)
    return false;
  if (IsOpenZmin() || IsOpenZmax() || Zmax - Zmin >= tol)
    return false;
  return true;
}

//=================================================================================================

Bnd_Box Bnd_Box::Transformed(const gp_Trsf& T) const
{
  if (IsVoid())
  {
    return Bnd_Box();
  }
  else if (T.Form() == gp_Identity)
  {
    return *this;
  }
  else if (T.Form() == gp_Translation)
  {
    if (!HasFinitePart())
    {
      return *this;
    }

    const gp_XYZ& aDelta = T.TranslationPart();
    Bnd_Box       aNewBox(*this);
    aNewBox.Xmin += aDelta.X();
    aNewBox.Xmax += aDelta.X();
    aNewBox.Ymin += aDelta.Y();
    aNewBox.Ymax += aDelta.Y();
    aNewBox.Zmin += aDelta.Z();
    aNewBox.Zmax += aDelta.Z();
    return aNewBox;
  }

  Bnd_Box aNewBox;
  if (HasFinitePart())
  {
    gp_Pnt aCorners[8] = {
      gp_Pnt(Xmin, Ymin, Zmin),
      gp_Pnt(Xmax, Ymin, Zmin),
      gp_Pnt(Xmin, Ymax, Zmin),
      gp_Pnt(Xmax, Ymax, Zmin),
      gp_Pnt(Xmin, Ymin, Zmax),
      gp_Pnt(Xmax, Ymin, Zmax),
      gp_Pnt(Xmin, Ymax, Zmax),
      gp_Pnt(Xmax, Ymax, Zmax),
    };
    for (int aCornerIter = 0; aCornerIter < 8; ++aCornerIter)
    {
      aCorners[aCornerIter].Transform(T);
      aNewBox.Add(aCorners[aCornerIter]);
    }
  }
  aNewBox.Gap = Gap;
  if (!IsOpen())
  {
    return aNewBox;
  }

  gp_Dir aDirs[6];
  int    aNbDirs = 0;
  if (IsOpenXmin())
  {
    aDirs[aNbDirs++] = THE_DIR_XMIN;
  }
  if (IsOpenXmax())
  {
    aDirs[aNbDirs++] = THE_DIR_XMAX;
  }
  if (IsOpenYmin())
  {
    aDirs[aNbDirs++] = THE_DIR_YMIN;
  }
  if (IsOpenYmax())
  {
    aDirs[aNbDirs++] = THE_DIR_YMAX;
  }
  if (IsOpenZmin())
  {
    aDirs[aNbDirs++] = THE_DIR_ZMIN;
  }
  if (IsOpenZmax())
  {
    aDirs[aNbDirs++] = THE_DIR_ZMAX;
  }

  for (int aDirIter = 0; aDirIter < aNbDirs; ++aDirIter)
  {
    aDirs[aDirIter].Transform(T);
    aNewBox.Add(aDirs[aDirIter]);
  }

  return aNewBox;
}

//=================================================================================================

void Bnd_Box::Add(const Bnd_Box& Other)
{
  if (Other.IsVoid())
  {
    return;
  }
  else if (IsVoid())
  {
    *this = Other;
    return;
  }

  Gap = std::max(Gap, Other.Gap);

  if (IsWhole())
  {
    return;
  }
  else if (Other.IsWhole())
  {
    SetWhole();
    return;
  }

  if (!IsOpenXmin())
  {
    if (Other.IsOpenXmin())
      OpenXmin();
    else if (Xmin > Other.Xmin)
      Xmin = Other.Xmin;
  }
  if (!IsOpenXmax())
  {
    if (Other.IsOpenXmax())
      OpenXmax();
    else if (Xmax < Other.Xmax)
      Xmax = Other.Xmax;
  }
  if (!IsOpenYmin())
  {
    if (Other.IsOpenYmin())
      OpenYmin();
    else if (Ymin > Other.Ymin)
      Ymin = Other.Ymin;
  }
  if (!IsOpenYmax())
  {
    if (Other.IsOpenYmax())
      OpenYmax();
    else if (Ymax < Other.Ymax)
      Ymax = Other.Ymax;
  }
  if (!IsOpenZmin())
  {
    if (Other.IsOpenZmin())
      OpenZmin();
    else if (Zmin > Other.Zmin)
      Zmin = Other.Zmin;
  }
  if (!IsOpenZmax())
  {
    if (Other.IsOpenZmax())
      OpenZmax();
    else if (Zmax < Other.Zmax)
      Zmax = Other.Zmax;
  }
}

//=================================================================================================

void Bnd_Box::Add(const gp_Pnt& P)
{
  double X, Y, Z;
  P.Coord(X, Y, Z);
  Update(X, Y, Z);
}

//=================================================================================================

void Bnd_Box::Add(const gp_Pnt& P, const gp_Dir& D)
{
  Add(P);
  Add(D);
}

//=================================================================================================

void Bnd_Box::Add(const gp_Dir& D)
{
  double DX, DY, DZ;
  D.Coord(DX, DY, DZ);

  if (DX < -RealEpsilon())
    OpenXmin();
  else if (DX > RealEpsilon())
    OpenXmax();

  if (DY < -RealEpsilon())
    OpenYmin();
  else if (DY > RealEpsilon())
    OpenYmax();

  if (DZ < -RealEpsilon())
    OpenZmin();
  else if (DZ > RealEpsilon())
    OpenZmax();
}

//=================================================================================================

bool Bnd_Box::IsOut(const gp_Pnt& P) const
{
  if (IsWhole())
    return false;
  else if (IsVoid())
    return true;
  else
  {
    double X, Y, Z;
    P.Coord(X, Y, Z);
    if (!IsOpenXmin() && (X < (Xmin - Gap)))
      return true;
    else if (!IsOpenXmax() && (X > (Xmax + Gap)))
      return true;
    else if (!IsOpenYmin() && (Y < (Ymin - Gap)))
      return true;
    else if (!IsOpenYmax() && (Y > (Ymax + Gap)))
      return true;
    else if (!IsOpenZmin() && (Z < (Zmin - Gap)))
      return true;
    else if (!IsOpenZmax() && (Z > (Zmax + Gap)))
      return true;
    else
      return false;
  }
}

//=================================================================================================

bool Bnd_Box::IsOut(const gp_Pln& P) const
{
  if (IsWhole())
    return false;
  else if (IsVoid())
    return true;
  else
  {
    double A, B, C, D;
    P.Coefficients(A, B, C, D);
    const double aXmin = GetXMin();
    const double aXmax = GetXMax();
    const double aYmin = GetYMin();
    const double aYmax = GetYMax();
    const double aZmin = GetZMin();
    const double aZmax = GetZMax();
    double       d     = A * aXmin + B * aYmin + C * aZmin + D;
    bool         plus  = d > 0;
    if (plus != ((A * aXmin + B * aYmin + C * aZmax + D) > 0))
      return false;
    if (plus != ((A * aXmin + B * aYmax + C * aZmin + D) > 0))
      return false;
    if (plus != ((A * aXmin + B * aYmax + C * aZmax + D) > 0))
      return false;
    if (plus != ((A * aXmax + B * aYmin + C * aZmin + D) > 0))
      return false;
    if (plus != ((A * aXmax + B * aYmin + C * aZmax + D) > 0))
      return false;
    if (plus != ((A * aXmax + B * aYmax + C * aZmin + D) > 0))
      return false;
    return plus == ((A * aXmax + B * aYmax + C * aZmax + D) > 0);
  }
}

//=================================================================================================

bool Bnd_Box::IsOut(const gp_Lin& L) const
{
  if (IsWhole())
    return false;
  else if (IsVoid())
    return true;
  else
  {
    double xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
    double parmin, parmax, par1, par2;
    bool   xToSet, yToSet;
    double myXmin, myYmin, myZmin, myXmax, myYmax, myZmax;
    Get(myXmin, myYmin, myZmin, myXmax, myYmax, myZmax);

    const double aDirX = std::abs(L.Direction().XYZ().X());
    const double aDirY = std::abs(L.Direction().XYZ().Y());
    const double aDirZ = std::abs(L.Direction().XYZ().Z());

    if (aDirX > 0.)
    {
      par1   = (myXmin - L.Location().XYZ().X()) / L.Direction().XYZ().X();
      par2   = (myXmax - L.Location().XYZ().X()) / L.Direction().XYZ().X();
      parmin = std::min(par1, par2);
      parmax = std::max(par1, par2);
      xToSet = true;
    }
    else
    {
      if (L.Location().XYZ().X() < myXmin || myXmax < L.Location().XYZ().X())
      {
        return true;
      }
      xmin   = L.Location().XYZ().X();
      xmax   = L.Location().XYZ().X();
      parmin = -THE_BND_PRECISION_INFINITE;
      parmax = THE_BND_PRECISION_INFINITE;
      xToSet = false;
    }

    if (aDirY > 0.)
    {
      par1 = (myYmin - L.Location().XYZ().Y()) / L.Direction().XYZ().Y();
      par2 = (myYmax - L.Location().XYZ().Y()) / L.Direction().XYZ().Y();
      // Check if parameter ranges from this axis are disjoint (early exit)
      if (parmax < std::min(par1, par2) || parmin > std::max(par1, par2))
        return true;

      parmin = std::max(parmin, std::min(par1, par2));
      parmax = std::min(parmax, std::max(par1, par2));
      yToSet = true;
    }
    else
    {
      if (L.Location().XYZ().Y() < myYmin || myYmax < L.Location().XYZ().Y())
      {
        return true;
      }
      ymin   = L.Location().XYZ().Y();
      ymax   = L.Location().XYZ().Y();
      yToSet = false;
    }

    if (aDirZ > 0.)
    {
      par1 = (myZmin - L.Location().XYZ().Z()) / L.Direction().XYZ().Z();
      par2 = (myZmax - L.Location().XYZ().Z()) / L.Direction().XYZ().Z();
      // Check if parameter ranges from this axis are disjoint (early exit)
      if (parmax < std::min(par1, par2) || parmin > std::max(par1, par2))
        return true;

      parmin = std::max(parmin, std::min(par1, par2));
      parmax = std::min(parmax, std::max(par1, par2));
      par1   = L.Location().XYZ().Z() + parmin * L.Direction().XYZ().Z();
      par2   = L.Location().XYZ().Z() + parmax * L.Direction().XYZ().Z();
      zmin   = std::min(par1, par2);
      zmax   = std::max(par1, par2);
    }
    else
    {
      if (L.Location().XYZ().Z() < myZmin || myZmax < L.Location().XYZ().Z())
        return true;
      zmin = L.Location().XYZ().Z();
      zmax = L.Location().XYZ().Z();
    }
    if (zmax < myZmin || myZmax < zmin)
      return true;

    if (xToSet)
    {
      par1 = L.Location().XYZ().X() + parmin * L.Direction().XYZ().X();
      par2 = L.Location().XYZ().X() + parmax * L.Direction().XYZ().X();
      xmin = std::min(par1, par2);
      xmax = std::max(par1, par2);
    }
    if (xmax < myXmin || myXmax < xmin)
      return true;

    if (yToSet)
    {
      par1 = L.Location().XYZ().Y() + parmin * L.Direction().XYZ().Y();
      par2 = L.Location().XYZ().Y() + parmax * L.Direction().XYZ().Y();
      ymin = std::min(par1, par2);
      ymax = std::max(par1, par2);
    }
    if (ymax < myYmin || myYmax < ymin)
      return true;
  }
  return false;
}

//=================================================================================================

bool Bnd_Box::IsOut(const Bnd_Box& Other) const
{
  // Fast path for non-open boxes with early exit
  if (!Flags && !Other.Flags)
  {
    const double aDelta = Other.Gap + Gap;
    // Early exit on first separating axis found
    if (Xmin - Other.Xmax > aDelta)
      return true;
    if (Other.Xmin - Xmax > aDelta)
      return true;
    if (Ymin - Other.Ymax > aDelta)
      return true;
    if (Other.Ymin - Ymax > aDelta)
      return true;
    if (Zmin - Other.Zmax > aDelta)
      return true;
    if (Other.Zmin - Zmax > aDelta)
      return true;
    return false;
  }

  // Handle special cases
  if (IsVoid() || Other.IsVoid())
    return true;
  if (IsWhole() || Other.IsWhole())
    return false;

  const double aDelta = Other.Gap + Gap;

  // Check each axis with early exit
  if (!IsOpenXmin() && !Other.IsOpenXmax() && Xmin - Other.Xmax > aDelta)
    return true;
  if (!IsOpenXmax() && !Other.IsOpenXmin() && Other.Xmin - Xmax > aDelta)
    return true;

  if (!IsOpenYmin() && !Other.IsOpenYmax() && Ymin - Other.Ymax > aDelta)
    return true;
  if (!IsOpenYmax() && !Other.IsOpenYmin() && Other.Ymin - Ymax > aDelta)
    return true;

  if (!IsOpenZmin() && !Other.IsOpenZmax() && Zmin - Other.Zmax > aDelta)
    return true;
  if (!IsOpenZmax() && !Other.IsOpenZmin() && Other.Zmin - Zmax > aDelta)
    return true;

  return false;
}

//=================================================================================================

bool Bnd_Box::IsOut(const Bnd_Box& Other, const gp_Trsf& T) const
{
  return IsOut(Other.Transformed(T));
}

//=================================================================================================

bool Bnd_Box::IsOut(const gp_Trsf& T1, const Bnd_Box& Other, const gp_Trsf& T2) const
{
  return Transformed(T1).IsOut(Other.Transformed(T2));
}

//=================================================================================================

bool Bnd_Box::IsOut(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Dir& D) const
{

  if (IsWhole())
    return false;
  else if (IsVoid())
    return true;

  constexpr double eps = RealSmall();
  double           myXmin, myYmin, myZmin, myXmax, myYmax, myZmax;
  Get(myXmin, myYmin, myZmin, myXmax, myYmax, myZmax);

  if (std::abs(D.X()) < eps && std::abs(D.Y()) < eps)
    return IsSegmentOut(myXmin, myYmin, myXmax, myYmax, P1.X(), P1.Y(), P2.X(), P2.Y());

  if (std::abs(D.X()) < eps && std::abs(D.Z()) < eps)
    return IsSegmentOut(myXmin, myZmin, myXmax, myZmax, P1.X(), P1.Z(), P2.X(), P2.Z());

  if (std::abs(D.Y()) < eps && std::abs(D.Z()) < eps)
    return IsSegmentOut(myYmin, myZmin, myYmax, myZmax, P1.Y(), P1.Z(), P2.Y(), P2.Z());

  if (std::abs(D.X()) < eps)
  {
    if (!IsSegmentOut(myXmin,
                      myZmin,
                      myXmax,
                      myZmax,
                      P1.X(),
                      (myYmin - P1.Y()) * D.Z() / D.Y() + P1.Z(),
                      P2.X(),
                      (myYmin - P2.Y()) * D.Z() / D.Y() + P2.Z()))
      return false;

    if (!IsSegmentOut(myXmin,
                      myZmin,
                      myXmax,
                      myZmax,
                      P1.X(),
                      (myYmax - P1.Y()) * D.Z() / D.Y() + P1.Z(),
                      P2.X(),
                      (myYmax - P2.Y()) * D.Z() / D.Y() + P2.Z()))
      return false;

    if (!IsSegmentOut(myXmin,
                      myYmin,
                      myXmax,
                      myYmax,
                      P1.X(),
                      (myZmin - P1.Z()) * D.Y() / D.Z() + P1.Y(),
                      P2.X(),
                      (myZmin - P2.Z()) * D.Y() / D.Z() + P2.Y()))
      return false;

    if (!IsSegmentOut(myXmin,
                      myYmin,
                      myXmax,
                      myYmax,
                      P1.X(),
                      (myZmax - P1.Z()) * D.Y() / D.Z() + P1.Y(),
                      P2.X(),
                      (myZmax - P2.Z()) * D.Y() / D.Z() + P2.Y()))
      return false;

    return true;
  } // if(D.X() == 0)

  if (std::abs(D.Y()) < eps)
  {
    if (!IsSegmentOut(myYmin,
                      myZmin,
                      myYmax,
                      myZmax,
                      P1.Y(),
                      (myXmin - P1.X()) * D.Z() / D.X() + P1.Z(),
                      P2.Y(),
                      (myXmin - P2.X()) * D.Z() / D.X() + P2.Z()))
      return false;

    if (!IsSegmentOut(myYmin,
                      myZmin,
                      myYmax,
                      myZmax,
                      P1.Y(),
                      (myXmax - P1.X()) * D.Z() / D.X() + P1.Z(),
                      P2.Y(),
                      (myXmax - P2.X()) * D.Z() / D.X() + P2.Z()))
      return false;

    if (!IsSegmentOut(myYmin,
                      myXmin,
                      myYmax,
                      myXmax,
                      P1.Y(),
                      (myZmin - P1.Z()) * D.X() / D.Z() + P1.X(),
                      P2.Y(),
                      (myZmin - P2.Z()) * D.X() / D.Z() + P2.X()))
      return false;

    if (!IsSegmentOut(myYmin,
                      myXmin,
                      myYmax,
                      myXmax,
                      P1.Y(),
                      (myZmax - P1.Z()) * D.X() / D.Z() + P1.X(),
                      P2.Y(),
                      (myZmax - P2.Z()) * D.X() / D.Z() + P2.X()))
      return false;

    return true;
  } // if(D.Y() == 0)

  if (std::abs(D.Z()) < eps)
  {
    if (!IsSegmentOut(myZmin,
                      myXmin,
                      myZmax,
                      myXmax,
                      P1.Z(),
                      (myYmax - P1.Y()) * D.X() / D.Y() + P1.X(),
                      P2.Z(),
                      (myYmax - P2.Y()) * D.X() / D.Y() + P2.X()))
      return false;

    if (!IsSegmentOut(myZmin,
                      myXmin,
                      myZmax,
                      myXmax,
                      P1.Z(),
                      (myYmin - P1.Y()) * D.X() / D.Y() + P1.X(),
                      P2.Z(),
                      (myYmin - P2.Y()) * D.X() / D.Y() + P2.X()))
      return false;

    if (!IsSegmentOut(myZmin,
                      myYmin,
                      myZmax,
                      myYmax,
                      P1.Z(),
                      (myXmax - P1.X()) * D.Y() / D.X() + P1.Y(),
                      P2.Z(),
                      (myXmax - P2.X()) * D.Y() / D.X() + P2.Y()))
      return false;

    if (!IsSegmentOut(myZmin,
                      myYmin,
                      myZmax,
                      myYmax,
                      P1.Z(),
                      (myXmin - P1.X()) * D.Y() / D.X() + P1.Y(),
                      P2.Z(),
                      (myXmin - P2.X()) * D.Y() / D.X() + P2.Y()))
      return false;

    return true;
  } // if(D.Z() == 0)

  if (!IsSegmentOut(myXmin,
                    myZmin,
                    myXmax,
                    myZmax,
                    (myYmin - P1.Y()) / D.Y() * D.X() + P1.X(),
                    (myYmin - P1.Y()) / D.Y() * D.Z() + P1.Z(),
                    (myYmin - P2.Y()) / D.Y() * D.X() + P2.X(),
                    (myYmin - P2.Y()) / D.Y() * D.Z() + P2.Z()))
    return false;

  if (!IsSegmentOut(myXmin,
                    myZmin,
                    myXmax,
                    myZmax,
                    (myYmax - P1.Y()) / D.Y() * D.X() + P1.X(),
                    (myYmax - P1.Y()) / D.Y() * D.Z() + P1.Z(),
                    (myYmax - P2.Y()) / D.Y() * D.X() + P2.X(),
                    (myYmax - P2.Y()) / D.Y() * D.Z() + P2.Z()))
    return false;

  if (!IsSegmentOut(myXmin,
                    myYmin,
                    myXmax,
                    myYmax,
                    (myZmin - P1.Z()) / D.Z() * D.X() + P1.X(),
                    (myZmin - P1.Z()) / D.Z() * D.Y() + P1.Y(),
                    (myZmin - P2.Z()) / D.Z() * D.X() + P2.X(),
                    (myZmin - P2.Z()) / D.Z() * D.Y() + P2.Y()))
    return false;

  if (!IsSegmentOut(myXmin,
                    myYmin,
                    myXmax,
                    myYmax,
                    (myZmax - P1.Z()) / D.Z() * D.X() + P1.X(),
                    (myZmax - P1.Z()) / D.Z() * D.Y() + P1.Y(),
                    (myZmax - P2.Z()) / D.Z() * D.X() + P2.X(),
                    (myZmax - P2.Z()) / D.Z() * D.Y() + P2.Y()))
    return false;

  if (!IsSegmentOut(myZmin,
                    myYmin,
                    myZmax,
                    myYmax,
                    (myXmin - P1.X()) / D.X() * D.Z() + P1.Z(),
                    (myXmin - P1.X()) / D.X() * D.Y() + P1.Y(),
                    (myXmin - P2.X()) / D.X() * D.Z() + P2.Z(),
                    (myXmin - P2.X()) / D.X() * D.Y() + P2.Y()))
    return false;

  if (!IsSegmentOut(myZmin,
                    myYmin,
                    myZmax,
                    myYmax,
                    (myXmax - P1.X()) / D.X() * D.Z() + P1.Z(),
                    (myXmax - P1.X()) / D.X() * D.Y() + P1.Y(),
                    (myXmax - P2.X()) / D.X() * D.Z() + P2.Z(),
                    (myXmax - P2.X()) / D.X() * D.Y() + P2.Y()))
    return false;

  return true;
}

//=================================================================================================

double Bnd_Box::Distance(const Bnd_Box& Other) const
{
  if (IsVoid() || Other.IsVoid())
  {
    return 0.0;
  }

  double aXMinB1, aYMinB1, aZMinB1, aXMaxB1, aYMaxB1, aZMaxB1;
  double aXMinB2, aYMinB2, aZMinB2, aXMaxB2, aYMaxB2, aZMaxB2;

  Get(aXMinB1, aYMinB1, aZMinB1, aXMaxB1, aYMaxB1, aZMaxB1);
  Other.Get(aXMinB2, aYMinB2, aZMinB2, aXMaxB2, aYMaxB2, aZMaxB2);

  const double aDistX = DistanceInDimension(aXMinB1, aXMaxB1, aXMinB2, aXMaxB2);
  const double aDistY = DistanceInDimension(aYMinB1, aYMaxB1, aYMinB2, aYMaxB2);
  const double aDistZ = DistanceInDimension(aZMinB1, aZMaxB1, aZMinB2, aZMaxB2);

  return std::sqrt(aDistX + aDistY + aDistZ);
}

//=================================================================================================

void Bnd_Box::Dump() const
{
  std::cout << "Box3D : ";
  if (IsVoid())
    std::cout << "Void";
  else if (IsWhole())
    std::cout << "Whole";
  else
  {
    std::cout << "\n Xmin : ";
    if (IsOpenXmin())
      std::cout << "Infinite";
    else
      std::cout << Xmin;
    std::cout << "\n Xmax : ";
    if (IsOpenXmax())
      std::cout << "Infinite";
    else
      std::cout << Xmax;
    std::cout << "\n Ymin : ";
    if (IsOpenYmin())
      std::cout << "Infinite";
    else
      std::cout << Ymin;
    std::cout << "\n Ymax : ";
    if (IsOpenYmax())
      std::cout << "Infinite";
    else
      std::cout << Ymax;
    std::cout << "\n Zmin : ";
    if (IsOpenZmin())
      std::cout << "Infinite";
    else
      std::cout << Zmin;
    std::cout << "\n Zmax : ";
    if (IsOpenZmax())
      std::cout << "Infinite";
    else
      std::cout << Zmax;
  }
  std::cout << "\n Gap : " << Gap;
  std::cout << "\n";
}

//=================================================================================================

void Bnd_Box::DumpJson(Standard_OStream& theOStream, int) const
{
  OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "CornerMin", 3, Xmin, Ymin, Zmin);
  OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "CornerMax", 3, Xmax, Ymax, Zmax);
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Gap);
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Flags)
}

//=================================================================================================

bool Bnd_Box::InitFromJson(const Standard_SStream& theSStream, int& theStreamPos)
{
  int aPos = theStreamPos;

  TCollection_AsciiString aStreamStr = Standard_Dump::Text(theSStream);
  OCCT_INIT_VECTOR_CLASS(aStreamStr, "CornerMin", aPos, 3, &Xmin, &Ymin, &Zmin)
  OCCT_INIT_VECTOR_CLASS(aStreamStr, "CornerMax", aPos, 3, &Xmax, &Ymax, &Zmax)

  OCCT_INIT_FIELD_VALUE_REAL(aStreamStr, aPos, Gap);
  OCCT_INIT_FIELD_VALUE_INTEGER(aStreamStr, aPos, Flags);

  theStreamPos = aPos;
  return true;
}
