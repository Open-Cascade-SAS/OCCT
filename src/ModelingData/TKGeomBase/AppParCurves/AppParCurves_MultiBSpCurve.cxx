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

#include <AppParCurves_HArray1OfMultiPoint.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

//=================================================================================================

static Standard_Integer ComputeDegree(const TColStd_Array1OfInteger& mults,
                                      const Standard_Integer         nbPoles)
{
  Standard_Integer i, sum = 0;
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

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve(const Standard_Integer NbPol)
    : AppParCurves_MultiCurve(NbPol),
      myDegree(0)
{
}

//=================================================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve(const AppParCurves_Array1OfMultiPoint& tabMU,
                                                       const TColStd_Array1OfReal&            Knots,
                                                       const TColStd_Array1OfInteger&         Mults)
    : AppParCurves_MultiCurve(tabMU)
{
  myknots                 = new TColStd_HArray1OfReal(Knots.Lower(), Knots.Upper());
  myknots->ChangeArray1() = Knots;
  mymults                 = new TColStd_HArray1OfInteger(Mults.Lower(), Mults.Upper());
  mymults->ChangeArray1() = Mults;
  myDegree                = ComputeDegree(Mults, NbPoles());
}

//=================================================================================================

AppParCurves_MultiBSpCurve::AppParCurves_MultiBSpCurve(const AppParCurves_MultiCurve& SC,
                                                       const TColStd_Array1OfReal&    Knots,
                                                       const TColStd_Array1OfInteger& Mults)
    : AppParCurves_MultiCurve(SC)
{
  myknots                 = new TColStd_HArray1OfReal(Knots.Lower(), Knots.Upper());
  myknots->ChangeArray1() = Knots;
  mymults                 = new TColStd_HArray1OfInteger(Mults.Lower(), Mults.Upper());
  mymults->ChangeArray1() = Mults;
  myDegree                = ComputeDegree(Mults, NbPoles());
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::SetKnots(const TColStd_Array1OfReal& theKnots)
{
  myknots                 = new TColStd_HArray1OfReal(theKnots.Lower(), theKnots.Upper());
  myknots->ChangeArray1() = theKnots;
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::SetMultiplicities(const TColStd_Array1OfInteger& theMults)
{
  mymults                 = new TColStd_HArray1OfInteger(theMults.Lower(), theMults.Upper());
  mymults->ChangeArray1() = theMults;
  myDegree                = ComputeDegree(theMults, NbPoles());
}

//=================================================================================================

const TColStd_Array1OfReal& AppParCurves_MultiBSpCurve::Knots() const
{
  return myknots->Array1();
}

//=================================================================================================

const TColStd_Array1OfInteger& AppParCurves_MultiBSpCurve::Multiplicities() const
{
  return mymults->Array1();
}

//=================================================================================================

Standard_Integer AppParCurves_MultiBSpCurve::Degree() const
{
  return myDegree;
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::Value(const Standard_Integer CuIndex,
                                       const Standard_Real    U,
                                       gp_Pnt&                Pt) const
{

  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D0(U,
               0,
               myDegree,
               Standard_False,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::Value(const Standard_Integer CuIndex,
                                       const Standard_Real    U,
                                       gp_Pnt2d&              Pt) const
{

  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt2d TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D0(U,
               0,
               myDegree,
               Standard_False,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D1(const Standard_Integer CuIndex,
                                    const Standard_Real    U,
                                    gp_Pnt&                Pt,
                                    gp_Vec&                V1) const
{
  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D1(U,
               0,
               myDegree,
               Standard_False,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D2(const Standard_Integer CuIndex,
                                    const Standard_Real    U,
                                    gp_Pnt&                Pt,
                                    gp_Vec&                V1,
                                    gp_Vec&                V2) const
{
  if (Dimension(CuIndex) != 3)
  {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D2(U,
               0,
               myDegree,
               Standard_False,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1,
               V2);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D1(const Standard_Integer CuIndex,
                                    const Standard_Real    U,
                                    gp_Pnt2d&              Pt,
                                    gp_Vec2d&              V1) const
{
  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt2d TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D1(U,
               0,
               myDegree,
               Standard_False,
               TabPoles,
               BSplCLib::NoWeights(),
               myknots->Array1(),
               &mymults->Array1(),
               Pt,
               V1);
}

//=================================================================================================

void AppParCurves_MultiBSpCurve::D2(const Standard_Integer CuIndex,
                                    const Standard_Real    U,
                                    gp_Pnt2d&              Pt,
                                    gp_Vec2d&              V1,
                                    gp_Vec2d&              V2) const
{
  if (Dimension(CuIndex) != 2)
  {
    throw Standard_OutOfRange();
  }

  TColgp_Array1OfPnt2d TabPoles(1, tabPoint->Length());
  Curve(CuIndex, TabPoles);

  BSplCLib::D2(U,
               0,
               myDegree,
               Standard_False,
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
  /*  for (Standard_Integer i = 1; i <= NbCurves(); i++) {
      o << " Curve No. " << i << std::endl;
      if (Dimension(i) == 3) {
        for (Standard_Integer j = 1; j <= tabPoint->Length(); j++) {
      o << " Pole No. " << j << ": " << std::endl;
      o << " Pole x = " << (tabPoint->Value(j)->Point(i)).X() << std::endl;
      o << " Pole y = " << (tabPoint->Value(j)->Point(i)).Y() << std::endl;
      o << " Pole z = " << (tabPoint->Value(j)->Point(i)).Z() << std::endl;
        }
      }
      else {
        for (Standard_Integer j = 1; j <= tabPoint->Length(); j++) {
      o << " Pole No. " << j << ": " << std::endl;
      o << " Pole x = " << (tabPoint->Value(j)->Point2d(i)).X() << std::endl;
      o << " Pole y = " << (tabPoint->Value(j)->Point2d(i)).Y() << std::endl;
        }
      }
    }
  */
}
