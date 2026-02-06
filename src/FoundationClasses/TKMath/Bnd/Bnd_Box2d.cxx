// Created on: 1991-10-30
// Created by: Modelisation
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

#include <Bnd_Box2d.hxx>

#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Macro.hxx>
#include <iostream>

namespace
{
// Precision constant for infinite bounds
constexpr double THE_BND_PRECISION_INFINITE = 1e+100;

// Precomputed unit direction vectors for bounding box transformations
constexpr gp_Dir2d THE_DIR_XMIN{gp_Dir2d::D::NX};
constexpr gp_Dir2d THE_DIR_XMAX{gp_Dir2d::D::X};
constexpr gp_Dir2d THE_DIR_YMIN{gp_Dir2d::D::NY};
constexpr gp_Dir2d THE_DIR_YMAX{gp_Dir2d::D::Y};
} // anonymous namespace

//=================================================================================================

void Bnd_Box2d::Update(const double x, const double y, const double X, const double Y)
{
  if (Flags & VoidMask)
  {
    Xmin = x;
    Ymin = y;
    Xmax = X;
    Ymax = Y;
    Flags &= ~VoidMask;
  }
  else
  {
    if (!(Flags & XminMask))
      Xmin = std::min(Xmin, x);
    if (!(Flags & XmaxMask))
      Xmax = std::max(Xmax, X);
    if (!(Flags & YminMask))
      Ymin = std::min(Ymin, y);
    if (!(Flags & YmaxMask))
      Ymax = std::max(Ymax, Y);
  }
}

//=================================================================================================

void Bnd_Box2d::Update(const double X, const double Y)
{
  if (Flags & VoidMask)
  {
    Xmin = X;
    Ymin = Y;
    Xmax = X;
    Ymax = Y;
    Flags &= ~VoidMask;
  }
  else
  {
    if (!(Flags & XminMask))
      Xmin = std::min(Xmin, X);
    if (!(Flags & XmaxMask))
      Xmax = std::max(Xmax, X);
    if (!(Flags & YminMask))
      Ymin = std::min(Ymin, Y);
    if (!(Flags & YmaxMask))
      Ymax = std::max(Ymax, Y);
  }
}

//=================================================================================================

void Bnd_Box2d::Get(double& x, double& y, double& Xm, double& Ym) const
{
  if (Flags & VoidMask)
    throw Standard_ConstructionError("Bnd_Box is void");

  x  = GetXMin();
  Xm = GetXMax();
  y  = GetYMin();
  Ym = GetYMax();
}

//=================================================================================================

Bnd_Box2d::Limits Bnd_Box2d::Get() const
{
  return {GetXMin(), GetXMax(), GetYMin(), GetYMax()};
}

//=================================================================================================

double Bnd_Box2d::GetXMin() const
{
  return (Flags & XminMask) ? -THE_BND_PRECISION_INFINITE : Xmin - Gap;
}

//=================================================================================================

double Bnd_Box2d::GetXMax() const
{
  return (Flags & XmaxMask) ? THE_BND_PRECISION_INFINITE : Xmax + Gap;
}

//=================================================================================================

double Bnd_Box2d::GetYMin() const
{
  return (Flags & YminMask) ? -THE_BND_PRECISION_INFINITE : Ymin - Gap;
}

//=================================================================================================

double Bnd_Box2d::GetYMax() const
{
  return (Flags & YmaxMask) ? THE_BND_PRECISION_INFINITE : Ymax + Gap;
}

//=================================================================================================

Bnd_Box2d Bnd_Box2d::Transformed(const gp_Trsf2d& T) const
{
  const gp_TrsfForm aF = T.Form();
  Bnd_Box2d         aNewBox(*this);
  if (IsVoid())
    return aNewBox;

  if (aF == gp_Identity)
  {
  }
  else if (aF == gp_Translation)
  {
    double aDX, aDY;
    (T.TranslationPart()).Coord(aDX, aDY);
    if (!(Flags & XminMask))
      aNewBox.Xmin += aDX;
    if (!(Flags & XmaxMask))
      aNewBox.Xmax += aDX;
    if (!(Flags & YminMask))
      aNewBox.Ymin += aDY;
    if (!(Flags & YmaxMask))
      aNewBox.Ymax += aDY;
  }
  else
  {
    gp_Pnt2d aP[4];
    bool     aVertex[4];
    aVertex[0] = true;
    aVertex[1] = true;
    aVertex[2] = true;
    aVertex[3] = true;
    gp_Dir2d aD[6];
    int      aNbDirs = 0;

    if (Flags & XminMask)
    {
      aD[aNbDirs++] = THE_DIR_XMIN;
      aVertex[0] = aVertex[2] = false;
    }
    if (Flags & XmaxMask)
    {
      aD[aNbDirs++] = THE_DIR_XMAX;
      aVertex[1] = aVertex[3] = false;
    }
    if (Flags & YminMask)
    {
      aD[aNbDirs++] = THE_DIR_YMIN;
      aVertex[0] = aVertex[1] = false;
    }
    if (Flags & YmaxMask)
    {
      aD[aNbDirs++] = THE_DIR_YMAX;
      aVertex[2] = aVertex[3] = false;
    }

    aNewBox.SetVoid();

    for (int i = 0; i < aNbDirs; i++)
    {
      aD[i].Transform(T);
      aNewBox.Add(aD[i]);
    }
    aP[0].SetCoord(Xmin, Ymin);
    aP[1].SetCoord(Xmax, Ymin);
    aP[2].SetCoord(Xmin, Ymax);
    aP[3].SetCoord(Xmax, Ymax);
    if (aVertex[0])
    {
      aP[0].Transform(T);
      aNewBox.Add(aP[0]);
    }
    if (aVertex[1])
    {
      aP[1].Transform(T);
      aNewBox.Add(aP[1]);
    }
    if (aVertex[2])
    {
      aP[2].Transform(T);
      aNewBox.Add(aP[2]);
    }
    if (aVertex[3])
    {
      aP[3].Transform(T);
      aNewBox.Add(aP[3]);
    }
    aNewBox.Gap = Gap;
  }
  return aNewBox;
}

//=================================================================================================

void Bnd_Box2d::Add(const Bnd_Box2d& Other)
{
  if (IsWhole())
    return;
  else if (Other.IsVoid())
    return;
  else if (Other.IsWhole())
    SetWhole();
  else if (IsVoid())
    (*this) = Other;
  else
  {
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
    Gap = std::max(Gap, Other.Gap);
  }
}

//=================================================================================================

void Bnd_Box2d::Add(const gp_Dir2d& D)
{
  double DX = D.X();
  double DY = D.Y();

  if (DX < -RealEpsilon())
    OpenXmin();
  else if (DX > RealEpsilon())
    OpenXmax();

  if (DY < -RealEpsilon())
    OpenYmin();
  else if (DY > RealEpsilon())
    OpenYmax();
}

//=================================================================================================

bool Bnd_Box2d::IsOut(const gp_Pnt2d& P) const
{
  if (IsWhole())
    return false;
  else if (IsVoid())
    return true;
  else
  {
    double X = P.X();
    double Y = P.Y();
    if (!(Flags & XminMask) && (X < (Xmin - Gap)))
      return true;
    else if (!(Flags & XmaxMask) && (X > (Xmax + Gap)))
      return true;
    else if (!(Flags & YminMask) && (Y < (Ymin - Gap)))
      return true;
    else if (!(Flags & YmaxMask) && (Y > (Ymax + Gap)))
      return true;
    else
      return false;
  }
}

//=================================================================================================

bool Bnd_Box2d::IsOut(const gp_Lin2d& theL) const
{
  if (IsWhole())
  {
    return false;
  }
  if (IsVoid())
  {
    return true;
  }
  double aXMin, aXMax, aYMin, aYMax;
  Get(aXMin, aYMin, aXMax, aYMax);

  gp_XY aCenter((aXMin + aXMax) / 2, (aYMin + aYMax) / 2);
  gp_XY aHeigh(std::abs(aXMax - aCenter.X()), std::abs(aYMax - aCenter.Y()));

  const double aProd[3] = {theL.Direction().XY() ^ (aCenter - theL.Location().XY()),
                           theL.Direction().X() * aHeigh.Y(),
                           theL.Direction().Y() * aHeigh.X()};
  bool         aStatus  = (std::abs(aProd[0]) > (std::abs(aProd[1]) + std::abs(aProd[2])));
  return aStatus;
}

//=================================================================================================

bool Bnd_Box2d::IsOut(const gp_Pnt2d& theP0, const gp_Pnt2d& theP1) const
{
  if (IsWhole())
  {
    return false;
  }
  if (IsVoid())
  {
    return true;
  }

  bool   aStatus = true;
  double aLocXMin, aLocXMax, aLocYMin, aLocYMax;
  Get(aLocXMin, aLocYMin, aLocXMax, aLocYMax);

  //// Intersect the line containing the segment.
  const gp_XY aSegDelta(theP1.XY() - theP0.XY());

  gp_XY aCenter((aLocXMin + aLocXMax) / 2, (aLocYMin + aLocYMax) / 2);
  gp_XY aHeigh(std::abs(aLocXMax - aCenter.X()), std::abs(aLocYMax - aCenter.Y()));

  const double aProd[3] = {aSegDelta ^ (aCenter - theP0.XY()),
                           aSegDelta.X() * aHeigh.Y(),
                           aSegDelta.Y() * aHeigh.X()};

  if ((std::abs(aProd[0]) <= (std::abs(aProd[1]) + std::abs(aProd[2]))))
  {
    // Intersection with line detected; check the segment as bounding box
    const gp_XY aHSeg(0.5 * aSegDelta.X(), 0.5 * aSegDelta.Y());
    const gp_XY aHSegAbs(std::abs(aHSeg.X()), std::abs(aHSeg.Y()));
    aStatus = ((std::abs((theP0.XY() + aHSeg - aCenter).X()) > (aHeigh + aHSegAbs).X())
               || (std::abs((theP0.XY() + aHSeg - aCenter).Y()) > (aHeigh + aHSegAbs).Y()));
  }
  return aStatus;
}

//=================================================================================================

bool Bnd_Box2d::IsOut(const Bnd_Box2d& Other) const
{
  // Fast path for non-open, non-void, non-whole boxes (most common case)
  if (!Flags && !Other.Flags)
  {
    const double aDelta = Other.Gap + Gap;
    if (Xmin - Other.Xmax > aDelta)
      return true;
    if (Other.Xmin - Xmax > aDelta)
      return true;
    if (Ymin - Other.Ymax > aDelta)
      return true;
    if (Other.Ymin - Ymax > aDelta)
      return true;
    return false;
  }

  // Handle special cases
  if (IsVoid() || Other.IsVoid())
    return true;
  if (IsWhole() || Other.IsWhole())
    return false;

  double OXmin, OXmax, OYmin, OYmax;
  Other.Get(OXmin, OYmin, OXmax, OYmax);
  if (!(Flags & XminMask) && (OXmax < (Xmin - Gap)))
    return true;
  if (!(Flags & XmaxMask) && (OXmin > (Xmax + Gap)))
    return true;
  if (!(Flags & YminMask) && (OYmax < (Ymin - Gap)))
    return true;
  if (!(Flags & YmaxMask) && (OYmin > (Ymax + Gap)))
    return true;
  return false;
}

//=================================================================================================

std::optional<gp_Pnt2d> Bnd_Box2d::Center() const
{
  if (IsVoid())
  {
    return std::nullopt;
  }
  return gp_Pnt2d(0.5 * (GetXMin() + GetXMax()),
                  0.5 * (GetYMin() + GetYMax()));
}

//=================================================================================================

double Bnd_Box2d::Distance(const Bnd_Box2d& theOther) const
{
  if (IsVoid() || theOther.IsVoid())
  {
    return 0.0;
  }

  double aXMin1, aYMin1, aXMax1, aYMax1;
  double aXMin2, aYMin2, aXMax2, aYMax2;

  Get(aXMin1, aYMin1, aXMax1, aYMax1);
  theOther.Get(aXMin2, aYMin2, aXMax2, aYMax2);

  // Compute squared distance per axis
  auto distAxis = [](const double theMin1,
                     const double theMax1,
                     const double theMin2,
                     const double theMax2) -> double
  {
    if (theMin1 > theMax2)
    {
      const double aD = theMin1 - theMax2;
      return aD * aD;
    }
    if (theMin2 > theMax1)
    {
      const double aD = theMin2 - theMax1;
      return aD * aD;
    }
    return 0.0;
  };

  const double aDx = distAxis(aXMin1, aXMax1, aXMin2, aXMax2);
  const double aDy = distAxis(aYMin1, aYMax1, aYMin2, aYMax2);

  return std::sqrt(aDx + aDy);
}

//=================================================================================================

void Bnd_Box2d::Dump() const
{
  std::cout << "Box2d : ";
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
  }
  std::cout << "\n Gap : " << Gap;
  std::cout << "\n";
}
