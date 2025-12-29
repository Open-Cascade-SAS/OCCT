// Copyright (c) 1995-1999 Matra Datavision
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

#include <AppParCurves_MultiCurve.hxx>
#include <AppParCurves_MultiPoint.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_Array1.hxx>

AppParCurves_MultiCurve::AppParCurves_MultiCurve() = default;

AppParCurves_MultiCurve::AppParCurves_MultiCurve(const int NbPol)
{
  tabPoint = new NCollection_HArray1<AppParCurves_MultiPoint>(1, NbPol);
}

AppParCurves_MultiCurve::AppParCurves_MultiCurve(
  const NCollection_Array1<AppParCurves_MultiPoint>& tabMU)
{
  tabPoint = new NCollection_HArray1<AppParCurves_MultiPoint>(1, tabMU.Length());
  int i, Lower = tabMU.Lower();
  for (i = 1; i <= tabMU.Length(); i++)
  {
    tabPoint->SetValue(i, tabMU.Value(Lower + i - 1));
  }
}

AppParCurves_MultiCurve::~AppParCurves_MultiCurve() = default;

int AppParCurves_MultiCurve::Dimension(const int Index) const
{
  int Lo = tabPoint->Lower();
  int nb = tabPoint->Value(Lo).NbPoints() + tabPoint->Value(Lo).NbPoints2d();
  if ((Index <= 0) || (Index > nb))
  {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Lo).Dimension(Index);
}

int AppParCurves_MultiCurve::NbCurves() const
{
  if (tabPoint.IsNull())
    return 0;
  AppParCurves_MultiPoint MP = tabPoint->Value(1);
  return MP.NbPoints() + MP.NbPoints2d();
}

int AppParCurves_MultiCurve::NbPoles() const
{
  if (tabPoint.IsNull())
    return 0;
  return tabPoint->Length();
}

int AppParCurves_MultiCurve::Degree() const
{
  return tabPoint->Length() - 1;
}

void AppParCurves_MultiCurve::SetNbPoles(const int nbPoles)
{
  tabPoint = new NCollection_HArray1<AppParCurves_MultiPoint>(1, nbPoles);
}

void AppParCurves_MultiCurve::SetValue(const int Index, const AppParCurves_MultiPoint& MPoint)
{

  if ((Index <= 0) || (Index > tabPoint->Length()))
  {
    throw Standard_OutOfRange();
  }
  tabPoint->SetValue(Index, MPoint);
}

void AppParCurves_MultiCurve::Curve(const int CuIndex, NCollection_Array1<gp_Pnt>& TabPnt) const
{
  if ((CuIndex <= 0))
  {
    throw Standard_OutOfRange();
  }
  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPnt(i) = tabPoint->Value(i).Point(CuIndex);
  }
}

void AppParCurves_MultiCurve::Curve(const int CuIndex, NCollection_Array1<gp_Pnt2d>& TabPnt2d) const
{
  if ((CuIndex <= 0))
  {
    throw Standard_OutOfRange();
  }
  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPnt2d(i) = tabPoint->Value(i).Point2d(CuIndex);
  }
}

const gp_Pnt& AppParCurves_MultiCurve::Pole(const int CuIndex, const int Nieme) const
{
  if ((CuIndex <= 0) && Nieme <= 0)
  {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Nieme).Point(CuIndex);
}

const gp_Pnt2d& AppParCurves_MultiCurve::Pole2d(const int CuIndex, const int Nieme) const
{
  if ((CuIndex <= 0) && Nieme <= 0)
  {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Nieme).Point2d(CuIndex);
}

const AppParCurves_MultiPoint& AppParCurves_MultiCurve::Value(const int Index) const
{
  if ((Index <= 0) || (Index > tabPoint->Length()))
  {
    throw Standard_OutOfRange();
  }
  return tabPoint->Value(Index);
}

void AppParCurves_MultiCurve::Transform(const int    CuIndex,
                                        const double x,
                                        const double dx,
                                        const double y,
                                        const double dy,
                                        const double z,
                                        const double dz)
{
  if (Dimension(CuIndex) != 3)
    throw Standard_OutOfRange();

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    (tabPoint->ChangeValue(i)).Transform(CuIndex, x, dx, y, dy, z, dz);
  }
}

void AppParCurves_MultiCurve::Transform2d(const int    CuIndex,
                                          const double x,
                                          const double dx,
                                          const double y,
                                          const double dy)
{
  if (Dimension(CuIndex) != 2)
    throw Standard_OutOfRange();

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    (tabPoint->ChangeValue(i)).Transform2d(CuIndex, x, dx, y, dy);
  }
}

void AppParCurves_MultiCurve::Value(const int CuIndex, const double U, gp_Pnt& Pt) const
{

  if (Dimension(CuIndex) != 3)
    throw Standard_OutOfRange();

  NCollection_Array1<gp_Pnt> TabPoles(1, tabPoint->Length());

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPoles(i) = tabPoint->Value(i).Point(CuIndex);
  }

  BSplCLib::D0(U, TabPoles, BSplCLib::NoWeights(), Pt);
}

void AppParCurves_MultiCurve::Value(const int CuIndex, const double U, gp_Pnt2d& Pt) const
{
  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt2d> TabPole(1, tabPoint->Length());

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPole(i) = tabPoint->Value(i).Point2d(CuIndex);
  }

  BSplCLib::D0(U, TabPole, BSplCLib::NoWeights(), Pt);
}

void AppParCurves_MultiCurve::D1(const int CuIndex, const double U, gp_Pnt& Pt, gp_Vec& V1) const
{

  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt> TabPole(1, tabPoint->Length());

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPole(i) = tabPoint->Value(i).Point(CuIndex);
  }

  BSplCLib::D1(U, TabPole, BSplCLib::NoWeights(), Pt, V1);
}

void AppParCurves_MultiCurve::D2(const int    CuIndex,
                                 const double U,
                                 gp_Pnt&      Pt,
                                 gp_Vec&      V1,
                                 gp_Vec&      V2) const
{

  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt> TabPole(1, tabPoint->Length());

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPole(i) = tabPoint->Value(i).Point(CuIndex);
  }

  BSplCLib::D2(U, TabPole, BSplCLib::NoWeights(), Pt, V1, V2);
}

void AppParCurves_MultiCurve::D1(const int    CuIndex,
                                 const double U,
                                 gp_Pnt2d&    Pt,
                                 gp_Vec2d&    V1) const
{

  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt2d> TabPole(1, tabPoint->Length());

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPole(i) = tabPoint->Value(i).Point2d(CuIndex);
  }

  BSplCLib::D1(U, TabPole, BSplCLib::NoWeights(), Pt, V1);
}

void AppParCurves_MultiCurve::D2(const int    CuIndex,
                                 const double U,
                                 gp_Pnt2d&    Pt,
                                 gp_Vec2d&    V1,
                                 gp_Vec2d&    V2) const
{

  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt2d> TabPole(1, tabPoint->Length());

  for (int i = 1; i <= tabPoint->Length(); i++)
  {
    TabPole(i) = tabPoint->Value(i).Point2d(CuIndex);
  }

  BSplCLib::D2(U, TabPole, BSplCLib::NoWeights(), Pt, V1, V2);
}

void AppParCurves_MultiCurve::Dump(Standard_OStream& o) const
{
  o << "AppParCurves_MultiCurve dump:" << std::endl;
  o << " It contains " << NbCurves() << " Bezier curves of degree " << tabPoint->Length() - 1
    << std::endl;
  o << " The poles are: " << std::endl;
  /*  for (int i = 1; i <= NbCurves(); i++) {
      o << " Curve No. " << i << std::endl;
      if (Dimension(i) == 3) {
        for (int j = 1; j <= tabPoint->Length(); j++) {
      o << " Pole No. " << j << ": " << std::endl;
      o << " Pole x = " << (tabPoint->Value(j)->Point(i)).X() << std::endl;
      o << " Pole y = " << (tabPoint->Value(j)->Point(i)).Y() << std::endl;
      o << " Pole z = " << (tabPoint->Value(j)->Point(i)).Z() << std::endl;
        }
      }
      else {
        for (int j = 1; j <= tabPoint->Length(); j++) {
      o << " Pole No. " << j << ": " << std::endl;
      o << " Pole x = " << (tabPoint->Value(j)->Point2d(i)).X() << std::endl;
      o << " Pole y = " << (tabPoint->Value(j)->Point2d(i)).Y() << std::endl;
        }
      }
    }
  */
}
