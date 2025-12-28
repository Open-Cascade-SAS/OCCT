// Created by: GG
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

#include <V3d.hxx>

#include <Aspect_Grid.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

namespace
{
static const char* V3d_Table_PrintTypeOfOrientation[26] = {
  "XPOS",         "YPOS",         "ZPOS",         "XNEG",         "YNEG",         "ZNEG",
  "XPOSYPOS",     "XPOSZPOS",     "XPOSZPOS",     "XNEGYNEG",     "XNEGYPOS",     "XNEGZNEG",
  "XNEGZPOS",     "YNEGZNEG",     "YNEGZPOS",     "XPOSYNEG",     "XPOSZNEG",     "YPOSZNEG",
  "XPOSYPOSZPOS", "XPOSYNEGZPOS", "XPOSYPOSZNEG", "XNEGYPOSZPOS", "XPOSYNEGZNEG", "XNEGYPOSZNEG",
  "XNEGYNEGZPOS", "XNEGYNEGZNEG"};
}

void V3d::ArrowOfRadius(const occ::handle<Graphic3d_Group>& garrow,
                        const double            X0,
                        const double            Y0,
                        const double            Z0,
                        const double            Dx,
                        const double            Dy,
                        const double            Dz,
                        const double            Alpha,
                        const double            Lng)
{
  double          Xc, Yc, Zc, Xi, Yi, Zi, Xj, Yj, Zj;
  double          Xn, Yn, Zn, X, Y, Z, X1 = 0., Y1 = 0., Z1 = 0., Norme;
  const int NbPoints = 10;

  //      Centre du cercle base de la fleche :
  Xc = X0 - Dx * Lng;
  Yc = Y0 - Dy * Lng;
  Zc = Z0 - Dz * Lng;

  //      Construction d'un repere i,j pour le cercle:
  Xn = 0., Yn = 0., Zn = 0.;

  if (std::abs(Dx) <= std::abs(Dy) && std::abs(Dx) <= std::abs(Dz))
    Xn = 1.;
  else if (std::abs(Dy) <= std::abs(Dz) && std::abs(Dy) <= std::abs(Dx))
    Yn = 1.;
  else
    Zn = 1.;
  Xi = Dy * Zn - Dz * Yn;
  Yi = Dz * Xn - Dx * Zn;
  Zi = Dx * Yn - Dy * Xn;

  Norme = std::sqrt(Xi * Xi + Yi * Yi + Zi * Zi);
  Xi    = Xi / Norme;
  Yi    = Yi / Norme;
  Zi    = Zi / Norme;

  Xj = Dy * Zi - Dz * Yi;
  Yj = Dz * Xi - Dx * Zi;
  Zj = Dx * Yi - Dy * Xi;

  occ::handle<Graphic3d_ArrayOfPolylines> aPrims =
    new Graphic3d_ArrayOfPolylines(3 * NbPoints, NbPoints);

  int    i;
  const double Tg = std::tan(Alpha);
  for (i = 1; i <= NbPoints; i++)
  {
    const double cosinus = std::cos(2. * M_PI / NbPoints * (i - 1));
    const double sinus   = std::sin(2. * M_PI / NbPoints * (i - 1));

    X = Xc + (cosinus * Xi + sinus * Xj) * Lng * Tg;
    Y = Yc + (cosinus * Yi + sinus * Yj) * Lng * Tg;
    Z = Zc + (cosinus * Zi + sinus * Zj) * Lng * Tg;

    if (i == 1)
    {
      X1 = X, Y1 = Y, Z1 = Z;
    }
    else
      aPrims->AddVertex(X, Y, Z);
    aPrims->AddBound(3);
    aPrims->AddVertex(X0, Y0, Z0);
    aPrims->AddVertex(X, Y, Z);
  }
  aPrims->AddVertex(X1, Y1, Z1);

  garrow->AddPrimitiveArray(aPrims);
}

void V3d::CircleInPlane(const occ::handle<Graphic3d_Group>& gcircle,
                        const double            X0,
                        const double            Y0,
                        const double            Z0,
                        const double            DX,
                        const double            DY,
                        const double            DZ,
                        const double            Rayon)
{
  double Norme = std::sqrt(DX * DX + DY * DY + DZ * DZ);
  if (Norme >= 0.0001)
  {
    double VX, VY, VZ, X, Y, Z, Xn, Yn, Zn, Xi, Yi, Zi, Xj, Yj, Zj;

    VX = DX / Norme;
    VY = DY / Norme;
    VZ = DZ / Norme;

    // Construction of marker i,j for the circle:
    Xn = 0., Yn = 0., Zn = 0.;
    if (std::abs(VX) <= std::abs(VY) && std::abs(VX) <= std::abs(VZ))
      Xn = 1.;
    else if (std::abs(VY) <= std::abs(VZ) && std::abs(VY) <= std::abs(VX))
      Yn = 1.;
    else
      Zn = 1.;
    Xi = VY * Zn - VZ * Yn;
    Yi = VZ * Xn - VX * Zn;
    Zi = VX * Yn - VY * Xn;

    Norme = std::sqrt(Xi * Xi + Yi * Yi + Zi * Zi);
    Xi    = Xi / Norme;
    Yi    = Yi / Norme;
    Zi    = Zi / Norme;

    Xj = VY * Zi - VZ * Yi;
    Yj = VZ * Xi - VX * Zi;
    Zj = VX * Yi - VY * Xi;

    const int             NFACES = 30;
    occ::handle<Graphic3d_ArrayOfPolylines> aPrims = new Graphic3d_ArrayOfPolylines(NFACES + 1);

    int    i      = 0;
    double       Alpha  = 0.;
    const double Dalpha = 2. * M_PI / NFACES;
    for (; i <= NFACES; i++, Alpha += Dalpha)
    {
      const double cosinus = std::cos(Alpha);
      const double sinus   = std::sin(Alpha);

      X = X0 + (cosinus * Xi + sinus * Xj) * Rayon;
      Y = Y0 + (cosinus * Yi + sinus * Yj) * Rayon;
      Z = Z0 + (cosinus * Zi + sinus * Zj) * Rayon;

      aPrims->AddVertex(X, Y, Z);
    }
    gcircle->AddPrimitiveArray(aPrims);
  }
}

void V3d::SwitchViewsinWindow(const occ::handle<V3d_View>& aPreviousView,
                              const occ::handle<V3d_View>& aNextView)
{
  aPreviousView->Viewer()->SetViewOff(aPreviousView);
  if (!aNextView->IfWindow())
    aNextView->SetWindow(aPreviousView->Window());
  aNextView->Viewer()->SetViewOn(aNextView);
}

//=================================================================================================

const char* V3d::TypeOfOrientationToString(V3d_TypeOfOrientation theType)
{
  return V3d_Table_PrintTypeOfOrientation[theType];
}

//=================================================================================================

bool V3d::TypeOfOrientationFromString(const char*       theTypeString,
                                                  V3d_TypeOfOrientation& theType)
{
  TCollection_AsciiString aName(theTypeString);
  aName.UpperCase();
  for (int aTypeIter = 0; aTypeIter <= V3d_XnegYnegZneg; ++aTypeIter)
  {
    const char* aTypeName = V3d_Table_PrintTypeOfOrientation[aTypeIter];
    if (aName == aTypeName)
    {
      theType = V3d_TypeOfOrientation(aTypeIter);
      return true;
    }
  }
  return false;
}
