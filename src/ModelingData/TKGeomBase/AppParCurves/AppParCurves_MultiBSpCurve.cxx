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

#include <AppParCurves_MultiPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_OutOfRange.hxx>

//=================================================================================================

static int ComputeDegree(const NCollection_Array1<int>& mults, const int nbPoles)
{
  int i, sum = 0;
  for (i = mults.Lower(); i <= mults.Upper(); i++)
  {
    sum += mults(i);
  }
  return sum - nbPoles - 1;
}

//=================================================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve()
    : myDegree(0)
{
}

//=================================================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve(const int NbPol)
    : AppParCurves_MultiCurve(NbPol),
      myDegree(0)
{
}

//=================================================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve(
  const NCollection_Array1<AppParCurves_MultiPoint>& tabMU,
  const NCollection_Array1<double>&                  Knots,
  const NCollection_Array1<int>&                     Mults)
    : AppParCurves_MultiCurve(tabMU)
{
  myknots                 = new NCollection_HArray1<double>(Knots.Lower(), Knots.Upper());
  myknots->ChangeArray1() = Knots;
  mymults                 = new NCollection_HArray1<int>(Mults.Lower(), Mults.Upper());
  mymults->ChangeArray1() = Mults;
  myDegree                = ComputeDegree(Mults, NbPoles());
}

//=================================================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve(const AppParCurves_MultiCurve&    SC,
                                                       const NCollection_Array1<double>& Knots,
                                                       const NCollection_Array1<int>&    Mults)
    : AppParCurves_MultiCurve(SC)
{
  myknots                 = new NCollection_HArray1<double>(Knots.Lower(), Knots.Upper());
  myknots->ChangeArray1() = Knots;
  mymults                 = new NCollection_HArray1<int>(Mults.Lower(), Mults.Upper());
  mymults->ChangeArray1() = Mults;
  myDegree                = ComputeDegree(Mults, NbPoles());
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::SetKnots(const NCollection_Array1<double>& theKnots)
{
  myknots                 = new NCollection_HArray1<double>(theKnots.Lower(), theKnots.Upper());
  myknots->ChangeArray1() = theKnots;
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::SetMultiplicities(const NCollection_Array1<int>& theMults)
{
  mymults                 = new NCollection_HArray1<int>(theMults.Lower(), theMults.Upper());
  mymults->ChangeArray1() = theMults;
  myDegree                = ComputeDegree(theMults, NbPoles());
}

//=================================================================================================

const NCollection_Array1<double>& AppParCurves_MultiBSpCurve::Knots() const
{
  return myknots->Array1();
}

//=================================================================================================

const NCollection_Array1<int>& AppParCurves_MultiBSpCurve::Multiplicities() const
{
  return mymults->Array1();
}

//=================================================================================================

int AppParCurves_MultiBSpCurve::Degree() const
{
  return myDegree;
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::Value(const int CuIndex, const double U, gp_Pnt& Pt) const
{

  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt> TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D0(U,
               0,
               myDegree,
               false,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::Value(const int CuIndex, const double U, gp_Pnt2d& Pt) const
{

  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt2d> TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D0(U,
               0,
               myDegree,
               false,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D1(const int CuIndex, const double U, gp_Pnt& Pt, gp_Vec& V1) const
{
  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt> TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D1(U,
               0,
               myDegree,
               false,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D2(const int    CuIndex,
                                    const double U,
                                    gp_Pnt&      Pt,
                                    gp_Vec&      V1,
                                    gp_Vec&      V2) const
{
  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt> TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D2(U,
               0,
               myDegree,
               false,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1,
               V2);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D1(const int    CuIndex,
                                    const double U,
                                    gp_Pnt2d&    Pt,
                                    gp_Vec2d&    V1) const
{
  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt2d> TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D1(U,
               0,
               myDegree,
               false,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D2(const int    CuIndex,
                                    const double U,
                                    gp_Pnt2d&    Pt,
                                    gp_Vec2d&    V1,
                                    gp_Vec2d&    V2) const
{
  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  NCollection_Array1<gp_Pnt2d> TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D2(U,
               0,
               myDegree,
               false,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1,
               V2);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::Dump(Standard_OStream& o) const
{
  o << "AppParCurves_MultiBSpCurve dump:" << std::endl;
  o << " It contains " << NbCurves() << " BSpline curves " << std::endl;
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
