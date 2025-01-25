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

//-- #include <Precision.hxx> Precision::Infinite() -> 1e+100
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
    if (!(Flags & XminMask) && (x < Xmin))
      Xmin = x;
    if (!(Flags & XmaxMask) && (X > Xmax))
      Xmax = X;
    if (!(Flags & YminMask) && (y < Ymin))
      Ymin = y;
    if (!(Flags & YmaxMask) && (Y > Ymax))
      Ymax = Y;
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
    if (!(Flags & XminMask) && (X < Xmin))
      Xmin = X;
    else if (!(Flags & XmaxMask) && (X > Xmax))
      Xmax = X;
    if (!(Flags & YminMask) && (Y < Ymin))
      Ymin = Y;
    else if (!(Flags & YmaxMask) && (Y > Ymax))
      Ymax = Y;
  }
}

//=================================================================================================

void Bnd_Box2d::Get(Standard_Real& x, Standard_Real& y, Standard_Real& Xm, Standard_Real& Ym) const
{
  if (Flags & VoidMask)
    throw Standard_ConstructionError("Bnd_Box is void");
  Standard_Real pinf = 1e+100; //-- Precision::Infinite();
  if (Flags & XminMask)
    x = -pinf;
  else
    x = Xmin - Gap;
  if (Flags & XmaxMask)
    Xm = pinf;
  else
    Xm = Xmax + Gap;
  if (Flags & YminMask)
    y = -pinf;
  else
    y = Ymin - Gap;
  if (Flags & YmaxMask)
    Ym = pinf;
  else
    Ym = Ymax + Gap;
}

//=================================================================================================

Bnd_Box2d Bnd_Box2d::Transformed(const gp_Trsf2d& T) const
{
  gp_TrsfForm F = T.Form();
  Bnd_Box2d   newb(*this);
  if (IsVoid())
    return newb;

  if (F == gp_Identity)
  {
  }
  else if (F == gp_Translation)
  {
    Standard_Real DX, DY;
    (T.TranslationPart()).Coord(DX, DY);
    if (!(Flags & XminMask))
      newb.Xmin += DX;
    if (!(Flags & XmaxMask))
      newb.Xmax += DX;
    if (!(Flags & YminMask))
      newb.Ymin += DY;
    if (!(Flags & YmaxMask))
      newb.Ymax += DY;
  }
  else
  {
    gp_Pnt2d         P[4];
    Standard_Boolean Vertex[4];
    Standard_Integer i;
    Vertex[0] = Standard_True;
    Vertex[1] = Standard_True;
    Vertex[2] = Standard_True;
    Vertex[3] = Standard_True;
    gp_Dir2d D[6];
    //    Standard_Integer vertices = 0;
    Standard_Integer directions = 0;

    if (Flags & XminMask)
    {
      D[directions].SetCoord(-1., 0.);
      directions++;
      Vertex[0] = Vertex[2] = Standard_False;
    }
    if (Flags & XmaxMask)
    {
      D[directions].SetCoord(1., 0.);
      directions++;
      Vertex[1] = Vertex[3] = Standard_False;
    }
    if (Flags & YminMask)
    {
      D[directions].SetCoord(0., -1.);
      directions++;
      Vertex[0] = Vertex[1] = Standard_False;
    }
    if (Flags & YmaxMask)
    {
      D[directions].SetCoord(0., 1.);
      directions++;
      Vertex[2] = Vertex[3] = Standard_False;
    }

    newb.SetVoid();

    for (i = 0; i < directions; i++)
    {
      D[i].Transform(T);
      newb.Add(D[i]);
    }
    P[0].SetCoord(Xmin, Ymin);
    P[1].SetCoord(Xmax, Ymin);
    P[2].SetCoord(Xmin, Ymax);
    P[3].SetCoord(Xmax, Ymax);
    if (Vertex[0])
    {
      P[0].Transform(T);
      newb.Add(P[0]);
    }
    if (Vertex[1])
    {
      P[1].Transform(T);
      newb.Add(P[1]);
    }
    if (Vertex[2])
    {
      P[2].Transform(T);
      newb.Add(P[2]);
    }
    if (Vertex[3])
    {
      P[3].Transform(T);
      newb.Add(P[3]);
    }
    newb.Gap = Gap;
  }
  return newb;
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
    Gap = Max(Gap, Other.Gap);
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
  gp_XY aHeigh(Abs(aXMax - aCenter.X()), Abs(aYMax - aCenter.Y()));

  const Standard_Real aProd[3] = {theL.Direction().XY() ^ (aCenter - theL.Location().XY()),
                                  theL.Direction().X() * aHeigh.Y(),
                                  theL.Direction().Y() * aHeigh.X()};
  Standard_Boolean    aStatus  = (Abs(aProd[0]) > (Abs(aProd[1]) + Abs(aProd[2])));
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
  gp_XY aHeigh(Abs(aLocXMax - aCenter.X()), Abs(aLocYMax - aCenter.Y()));

  const Standard_Real aProd[3] = {aSegDelta ^ (aCenter - theP0.XY()),
                                  aSegDelta.X() * aHeigh.Y(),
                                  aSegDelta.Y() * aHeigh.X()};

  if ((Abs(aProd[0]) <= (Abs(aProd[1]) + Abs(aProd[2]))))
  {
    // Intersection with line detected; check the segment as bounding box
    const gp_XY aHSeg(0.5 * aSegDelta.X(), 0.5 * aSegDelta.Y());
    const gp_XY aHSegAbs(Abs(aHSeg.X()), Abs(aHSeg.Y()));
    aStatus = ((Abs((theP0.XY() + aHSeg - aCenter).X()) > (aHeigh + aHSegAbs).X())
               || (Abs((theP0.XY() + aHSeg - aCenter).Y()) > (aHeigh + aHSegAbs).Y()));
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
