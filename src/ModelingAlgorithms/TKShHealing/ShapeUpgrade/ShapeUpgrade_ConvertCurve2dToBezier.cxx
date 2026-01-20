// Created on: 1999-05-13
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2dConvert_BSplineCurveToBezierCurve.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <ShapeCustom_Curve2d.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_ConvertCurve2dToBezier.hxx>
#include <Standard_Type.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ConvertCurve2dToBezier, ShapeUpgrade_SplitCurve2d)

ShapeUpgrade_ConvertCurve2dToBezier::ShapeUpgrade_ConvertCurve2dToBezier()
{
  mySegments    = new NCollection_HSequence<occ::handle<Geom2d_Curve>>;
  mySplitParams = new NCollection_HSequence<double>;
}

static occ::handle<Geom2d_BezierCurve> MakeBezier2d(const occ::handle<Geom2d_Curve>& theCurve2d,
                                               const double         theFirst,
                                               const double         theLast)
{
  NCollection_Array1<gp_Pnt2d> poles(1, 2);
  poles(1)                          = theCurve2d->Value(theFirst);
  poles(2)                          = theCurve2d->Value(theLast);
  occ::handle<Geom2d_BezierCurve> bezier = new Geom2d_BezierCurve(poles);
  return bezier;
}

//=================================================================================================

void ShapeUpgrade_ConvertCurve2dToBezier::Compute()
{
  mySegments->Clear();
  mySplitParams->Clear();
  constexpr double precision = Precision::PConfusion();
  double           First     = mySplitValues->Value(1);
  double           Last      = mySplitValues->Value(mySplitValues->Length());

  // PTV Try to create line2d from myCurve
  if (myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))
      || myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
  {
    // static function`s code getted from ShapeConvert
    double       tmpF, tmpL, aDeviation;
    occ::handle<Geom2d_Line> aTmpLine2d =
      ShapeCustom_Curve2d::ConvertToLine2d(myCurve,
                                           First,
                                           Last,
                                           Precision::Approximation(),
                                           tmpF,
                                           tmpL,
                                           aDeviation);
    if (!aTmpLine2d.IsNull() && (aDeviation <= Precision::Approximation()))
    {
      occ::handle<Geom2d_BezierCurve> bezier = MakeBezier2d(aTmpLine2d, tmpF, tmpL);
      mySegments->Append(bezier);
      mySplitParams->Append(First);
      mySplitParams->Append(Last);
      myNbCurves = mySplitValues->Length() - 1;
      myStatus   = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      return;
    }
  }

  if (myCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve>         tmp      = occ::down_cast<Geom2d_TrimmedCurve>(myCurve);
    occ::handle<Geom2d_Curve>                BasCurve = tmp->BasisCurve();
    ShapeUpgrade_ConvertCurve2dToBezier converter;
    // converter.Init(BasCurve,Max(First,BasCurve->FirstParameter()),Min(Last,BasCurve->LastParameter()));
    // //???
    converter.Init(BasCurve, First, Last);
    converter.SetSplitValues(mySplitValues);
    converter.Compute();
    mySplitValues->Clear();
    mySplitValues->ChangeSequence() = converter.SplitValues()->Sequence();
    myNbCurves                      = mySplitValues->Length() - 1;
    myStatus |= converter.myStatus;
    mySegments->ChangeSequence()    = converter.Segments()->Sequence();
    mySplitParams->ChangeSequence() = converter.SplitParams()->Sequence();
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
  {
    occ::handle<Geom2d_BezierCurve> bezier = occ::down_cast<Geom2d_BezierCurve>(myCurve);
    myNbCurves                        = mySplitValues->Length() - 1;
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    if (First < precision && Last > 1 - precision)
    {
      mySegments->Append(bezier);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
    }
    else
    {
      occ::handle<Geom2d_BezierCurve> besNew = occ::down_cast<Geom2d_BezierCurve>(bezier->Copy());
      besNew->Segment(First, Last);
      mySegments->Append(besNew);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    }
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_Line)))
  {
    occ::handle<Geom2d_Line>        aLine2d = occ::down_cast<Geom2d_Line>(myCurve);
    occ::handle<Geom2d_BezierCurve> bezier  = MakeBezier2d(aLine2d, First, Last);
    mySegments->Append(bezier);
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    myNbCurves = mySplitValues->Length() - 1;
    myStatus   = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return;
  }
  else
  {
    occ::handle<Geom2d_BSplineCurve> aBSpline2d;
    double               Shift = 0.;
    if (myCurve->IsKind(STANDARD_TYPE(Geom2d_Conic)))
    {
      // clang-format off
      occ::handle<Geom2d_Curve> tcurve = new Geom2d_TrimmedCurve(myCurve,First,Last); //protection against parabols ets
      // clang-format on
      Geom2dConvert_ApproxCurve approx(tcurve, Precision::Approximation(), GeomAbs_C1, 100, 6);
      if (approx.HasResult())
        aBSpline2d = approx.Curve();
      else
        aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve, Convert_QuasiAngular);

      Shift = First - aBSpline2d->FirstParameter();
      First = aBSpline2d->FirstParameter();
      Last  = aBSpline2d->LastParameter();
    }
    else if (!myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(myCurve, Convert_QuasiAngular);
    }
    else
      aBSpline2d = occ::down_cast<Geom2d_BSplineCurve>(myCurve);

    double bf = aBSpline2d->FirstParameter();
    double bl = aBSpline2d->LastParameter();
    if (std::abs(First - bf) < precision)
      First = bf;
    if (std::abs(Last - bl) < precision)
      Last = bl;
    if (First < bf)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: The range of the edge exceeds the pcurve domain" << std::endl;
#endif
      First = bf;
      mySplitValues->SetValue(1, First);
    }
    if (Last > bl)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: The range of the edge exceeds the pcurve domain" << std::endl;
#endif
      Last = bl;
      mySplitValues->SetValue(mySplitValues->Length(), Last);
    }

    // PTV 20.12.2001 Try to simplify BSpline Curve
    ShapeCustom_Curve2d::SimplifyBSpline2d(aBSpline2d, Precision::Approximation());

    Geom2dConvert_BSplineCurveToBezierCurve tool(aBSpline2d, First, Last, precision);
    int                        nbArcs = tool.NbArcs();
    NCollection_Array1<double>                    knots(1, nbArcs + 1);
    tool.Knots(knots);
    mySplitParams->Append(First + Shift);
    int j; // svv Jan 10 2000 : porting on DEC

    double newFirst = First + Shift;
    double newLast  = First + Shift;

    for (j = 1; j <= nbArcs; j++)
    {
      double nextKnot = knots(j + 1) + Shift;
      if (nextKnot - mySplitParams->Value(mySplitParams->Length()) > precision)
      {
        occ::handle<Geom2d_Curve> aCrv2d = tool.Arc(j);
        if (aCrv2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
        {
          newFirst = newLast;
          newLast  = nextKnot;
          double       tmpF, tmpL, aDeviation;
          occ::handle<Geom2d_Line> aTmpLine2d =
            ShapeCustom_Curve2d::ConvertToLine2d(aCrv2d,
                                                 newFirst,
                                                 newLast,
                                                 Precision::Approximation(),
                                                 tmpF,
                                                 tmpL,
                                                 aDeviation);
          if (!aTmpLine2d.IsNull() && (aDeviation <= Precision::Approximation()))
          {
            occ::handle<Geom2d_BezierCurve> bezier = MakeBezier2d(aBSpline2d, newFirst, newLast);
            mySegments->Append(bezier);
            mySplitParams->Append(newLast);
            continue;
          }
        }
        mySegments->Append(aCrv2d);
        mySplitParams->Append(nextKnot);
      }
    }

    First = mySplitValues->Value(1);
    for (j = 2; j <= mySplitValues->Length(); j++)
    {
      Last = mySplitValues->Value(j);
      for (int i = 2; i <= nbArcs + 1; i++)
      {
        double valknot = knots(i) + Shift;
        if (valknot <= First + precision)
          continue;
        if (valknot >= Last - precision)
          break;
        mySplitValues->InsertBefore(j++, valknot);
      }
      First = Last;
    }
    myNbCurves = mySplitValues->Length() - 1;
  }
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
}

//=================================================================================================

void ShapeUpgrade_ConvertCurve2dToBezier::Build(const bool /*Segment*/)
{
  constexpr double prec = Precision::PConfusion();
  int        nb   = mySplitValues->Length();
  myResultingCurves            = new NCollection_HArray1<occ::handle<Geom2d_Curve>>(1, nb - 1);
  double    prevPar     = 0.;
  int j           = 2;
  for (int i = 2; i <= nb; i++)
  {
    double par = mySplitValues->Value(i);
    for (; j <= mySplitParams->Length(); j++)
      if (mySplitParams->Value(j) + prec > par)
        break;
      else
        prevPar = 0.;

    occ::handle<Geom2d_BezierCurve> bes =
      occ::down_cast<Geom2d_BezierCurve>(mySegments->Value(j - 1)->Copy());
    double uFact  = mySplitParams->Value(j) - mySplitParams->Value(j - 1);
    double pp     = mySplitValues->Value(i - 1);
    double length = (par - pp) / uFact;
    bes->Segment(prevPar, prevPar + length);
    prevPar += length;
    myResultingCurves->SetValue(i - 1, bes);
  }
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Geom2d_Curve>>> ShapeUpgrade_ConvertCurve2dToBezier::Segments() const
{
  return mySegments;
}

//=================================================================================================

occ::handle<NCollection_HSequence<double>> ShapeUpgrade_ConvertCurve2dToBezier::SplitParams() const
{
  return mySplitParams;
}
