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
#include <Standard_Stream.hxx>

namespace
{
// Precision constant for infinite bounds
constexpr Standard_Real THE_BND_PRECISION_INFINITE = 1e+100;

// Precomputed unit direction vectors for bounding box transformations
constexpr gp_Dir2d THE_DIR_XMIN{gp_Dir2d::D::NX};
constexpr gp_Dir2d THE_DIR_XMAX{gp_Dir2d::D::X};
constexpr gp_Dir2d THE_DIR_YMIN{gp_Dir2d::D::NY};
constexpr gp_Dir2d THE_DIR_YMAX{gp_Dir2d::D::Y};
} // anonymous namespace

//=================================================================================================

void Bnd_Box2d::Update(const Standard_Real x,
                       const Standard_Real y,
                       const Standard_Real X,
                       const Standard_Real Y)
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

void Bnd_Box2d::Update(const Standard_Real X, const Standard_Real Y)
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

void Bnd_Box2d::Get(Standard_Real& x, Standard_Real& y, Standard_Real& Xm, Standard_Real& Ym) const
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

Standard_Real Bnd_Box2d::GetXMin() const
{
  return (Flags & XminMask) ? -THE_BND_PRECISION_INFINITE : Xmin - Gap;
}

//=================================================================================================

Standard_Real Bnd_Box2d::GetXMax() const
{
  return (Flags & XmaxMask) ? THE_BND_PRECISION_INFINITE : Xmax + Gap;
}

//=================================================================================================

Standard_Real Bnd_Box2d::GetYMin() const
{
  return (Flags & YminMask) ? -THE_BND_PRECISION_INFINITE : Ymin - Gap;
}

//=================================================================================================

Standard_Real Bnd_Box2d::GetYMax() const
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
    Standard_Real aDX, aDY;
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
    gp_Pnt2d         aP[4];
    Standard_Boolean aVertex[4];
    aVertex[0] = Standard_True;
    aVertex[1] = Standard_True;
    aVertex[2] = Standard_True;
    aVertex[3] = Standard_True;
    gp_Dir2d         aD[6];
    Standard_Integer aNbDirs = 0;

    if (Flags & XminMask)
    {
      aD[aNbDirs++] = THE_DIR_XMIN;
      aVertex[0] = aVertex[2] = Standard_False;
    }
    if (Flags & XmaxMask)
    {
      aD[aNbDirs++] = THE_DIR_XMAX;
      aVertex[1] = aVertex[3] = Standard_False;
    }
    if (Flags & YminMask)
    {
      aD[aNbDirs++] = THE_DIR_YMIN;
      aVertex[0] = aVertex[1] = Standard_False;
    }
    if (Flags & YmaxMask)
    {
      aD[aNbDirs++] = THE_DIR_YMAX;
      aVertex[2] = aVertex[3] = Standard_False;
    }

    aNewBox.SetVoid();

    for (Standard_Integer i = 0; i < aNbDirs; i++)
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
  Standard_Real DX = D.X();
  Standard_Real DY = D.Y();

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

Standard_Boolean Bnd_Box2d::IsOut(const gp_Pnt2d& P) const
{
  if (IsWhole())
    return Standard_False;
  else if (IsVoid())
    return Standard_True;
  else
  {
    Standard_Real X = P.X();
    Standard_Real Y = P.Y();
    if (!(Flags & XminMask) && (X < (Xmin - Gap)))
      return Standard_True;
    else if (!(Flags & XmaxMask) && (X > (Xmax + Gap)))
      return Standard_True;
    else if (!(Flags & YminMask) && (Y < (Ymin - Gap)))
      return Standard_True;
    else if (!(Flags & YmaxMask) && (Y > (Ymax + Gap)))
      return Standard_True;
    else
      return Standard_False;
  }
}

//=================================================================================================

Standard_Boolean Bnd_Box2d::IsOut(const gp_Lin2d& theL) const
{
  if (IsWhole())
  {
    return Standard_False;
  }
  if (IsVoid())
  {
    return Standard_True;
  }
  Standard_Real aXMin, aXMax, aYMin, aYMax;
  Get(aXMin, aYMin, aXMax, aYMax);

  gp_XY aCenter((aXMin + aXMax) / 2, (aYMin + aYMax) / 2);
  gp_XY aHeigh(std::abs(aXMax - aCenter.X()), std::abs(aYMax - aCenter.Y()));

  const Standard_Real aProd[3] = {theL.Direction().XY() ^ (aCenter - theL.Location().XY()),
                                  theL.Direction().X() * aHeigh.Y(),
                                  theL.Direction().Y() * aHeigh.X()};
  Standard_Boolean    aStatus  = (std::abs(aProd[0]) > (std::abs(aProd[1]) + std::abs(aProd[2])));
  return aStatus;
}

//=================================================================================================

Standard_Boolean Bnd_Box2d::IsOut(const gp_Pnt2d& theP0, const gp_Pnt2d& theP1) const
{
  if (IsWhole())
  {
    return Standard_False;
  }
  if (IsVoid())
  {
    return Standard_True;
  }

  Standard_Boolean aStatus = Standard_True;
  Standard_Real    aLocXMin, aLocXMax, aLocYMin, aLocYMax;
  Get(aLocXMin, aLocYMin, aLocXMax, aLocYMax);

  //// Intersect the line containing the segment.
  const gp_XY aSegDelta(theP1.XY() - theP0.XY());

  gp_XY aCenter((aLocXMin + aLocXMax) / 2, (aLocYMin + aLocYMax) / 2);
  gp_XY aHeigh(std::abs(aLocXMax - aCenter.X()), std::abs(aLocYMax - aCenter.Y()));

  const Standard_Real aProd[3] = {aSegDelta ^ (aCenter - theP0.XY()),
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

Standard_Boolean Bnd_Box2d::IsOut(const Bnd_Box2d& Other) const
{
  if (IsWhole())
    return Standard_False;
  else if (IsVoid())
    return Standard_True;
  else if (Other.IsWhole())
    return Standard_False;
  else if (Other.IsVoid())
    return Standard_True;
  else
  {
    Standard_Real OXmin, OXmax, OYmin, OYmax;
    Other.Get(OXmin, OYmin, OXmax, OYmax);
    if (!(Flags & XminMask) && (OXmax < (Xmin - Gap)))
      return Standard_True;
    else if (!(Flags & XmaxMask) && (OXmin > (Xmax + Gap)))
      return Standard_True;
    else if (!(Flags & YminMask) && (OYmax < (Ymin - Gap)))
      return Standard_True;
    else if (!(Flags & YmaxMask) && (OYmin > (Ymax + Gap)))
      return Standard_True;
  }
  return Standard_False;
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
