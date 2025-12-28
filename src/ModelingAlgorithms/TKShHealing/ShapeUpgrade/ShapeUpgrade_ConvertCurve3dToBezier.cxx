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

#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_BSplineCurveToBezierCurve.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_ConvertCurve3dToBezier.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ConvertCurve3dToBezier, ShapeUpgrade_SplitCurve3d)

ShapeUpgrade_ConvertCurve3dToBezier::ShapeUpgrade_ConvertCurve3dToBezier()
{
  mySegments    = new NCollection_HSequence<occ::handle<Geom_Curve>>;
  mySplitParams = new NCollection_HSequence<double>;
  myLineMode    = true;
  myCircleMode  = true;
  myConicMode   = true;
}

//=================================================================================================

void ShapeUpgrade_ConvertCurve3dToBezier::Compute()
{
  mySegments->Clear();
  mySplitParams->Clear();
  constexpr double precision = Precision::PConfusion();
  double           First     = mySplitValues->Value(1);
  double           Last      = mySplitValues->Value(mySplitValues->Length());
  if (myCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve>      tmp      = occ::down_cast<Geom_TrimmedCurve>(myCurve);
    occ::handle<Geom_Curve>             BasCurve = tmp->BasisCurve();
    ShapeUpgrade_ConvertCurve3dToBezier converter;
    converter.Init(BasCurve, First, Last);
    converter.SetSplitValues(mySplitValues);
    converter.Compute();
    mySplitValues->ChangeSequence() = converter.SplitValues()->Sequence();
    myNbCurves                      = mySplitValues->Length() - 1;
    myStatus |= converter.myStatus;
    mySegments->ChangeSequence()    = converter.Segments()->Sequence();
    mySplitParams->ChangeSequence() = converter.SplitParams()->Sequence();
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
  {
    occ::handle<Geom_BezierCurve> bezier = occ::down_cast<Geom_BezierCurve>(myCurve);
    myNbCurves                           = mySplitValues->Length() - 1;
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    if (First < precision && Last > 1 - precision)
    {
      mySegments->Append(bezier);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
    }
    else
    {
      occ::handle<Geom_BezierCurve> besNew = occ::down_cast<Geom_BezierCurve>(bezier->Copy());
      besNew->Segment(First, Last);
      mySegments->Append(besNew);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    }
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(myCurve);
    myNbCurves                   = mySplitValues->Length() - 1;
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    if (!myLineMode)
    {
      mySegments->Append(aLine);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
      return;
    }
    NCollection_Array1<gp_Pnt> poles(1, 2);
    poles(1)                             = aLine->Value(First);
    poles(2)                             = aLine->Value(Last);
    occ::handle<Geom_BezierCurve> bezier = new Geom_BezierCurve(poles);
    mySegments->Append(bezier);
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return;
  }
  else if ((myCurve->IsKind(STANDARD_TYPE(Geom_Conic)) && !myConicMode)
           || (myCurve->IsKind(STANDARD_TYPE(Geom_Circle)) && !myCircleMode))
  {
    myNbCurves = mySplitValues->Length() - 1;
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    mySegments->Append(myCurve);
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
    return;
  }
  else
  {
    occ::handle<Geom_BSplineCurve> aBSpline;
    double                         Shift = 0.;
    if (myCurve->IsKind(STANDARD_TYPE(Geom_Conic)))
    {
      // clang-format off
      occ::handle<Geom_Curve> tcurve = new Geom_TrimmedCurve(myCurve,First,Last); //protection against parabols ets
      // clang-format on
      GeomConvert_ApproxCurve approx(tcurve, Precision::Approximation(), GeomAbs_C1, 100, 6);
      if (approx.HasResult())
        aBSpline = approx.Curve();
      else
      {
        occ::handle<Geom_TrimmedCurve> t3d = new Geom_TrimmedCurve(myCurve, First, Last);
        aBSpline = GeomConvert::CurveToBSplineCurve(t3d, Convert_QuasiAngular);
      }
      Shift = First - aBSpline->FirstParameter();
      First = aBSpline->FirstParameter();
      Last  = aBSpline->LastParameter();
    }
    else if (!myCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      aBSpline = GeomConvert::CurveToBSplineCurve(myCurve, Convert_QuasiAngular);
    }
    else
      aBSpline = occ::down_cast<Geom_BSplineCurve>(myCurve);

    double bf = aBSpline->FirstParameter();
    double bl = aBSpline->LastParameter();
    if (std::abs(First - bf) < precision)
      First = bf;
    if (std::abs(Last - bl) < precision)
      Last = bl;
    if (First < bf)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: The range of the edge exceeds the curve domain" << std::endl;
#endif
      First = bf;
      mySplitValues->SetValue(1, First);
    }
    if (Last > bl)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: The range of the edge exceeds the curve domain" << std::endl;
#endif
      Last = bl;
      mySplitValues->SetValue(mySplitValues->Length(), Last);
    }

    GeomConvert_BSplineCurveToBezierCurve tool(aBSpline, First, Last, precision);
    int                                   nbArcs = tool.NbArcs();
    NCollection_Array1<double>            knots(1, nbArcs + 1);
    tool.Knots(knots);
    mySplitParams->Append(First + Shift);
    int j; // svv Jan 10 2000 : porting on DEC
    for (j = 1; j <= nbArcs; j++)
    {
      double nextKnot = knots(j + 1) + Shift;
      if (nextKnot - mySplitParams->Value(mySplitParams->Length()) > precision)
      {
        mySegments->Append(tool.Arc(j));
        mySplitParams->Append(knots(j + 1) + Shift);
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

void ShapeUpgrade_ConvertCurve3dToBezier::Build(const bool /*Segment*/)
{
  constexpr double prec = Precision::PConfusion();
  int              nb   = mySplitValues->Length();
  myResultingCurves     = new NCollection_HArray1<occ::handle<Geom_Curve>>(1, nb - 1);
  double prevPar        = 0.;
  int    j              = 2;
  for (int i = 2; i <= nb; i++)
  {
    double par = mySplitValues->Value(i);
    for (; j <= mySplitParams->Length(); j++)
      if (mySplitParams->Value(j) + prec > par)
        break;
      else
        prevPar = 0.;

    occ::handle<Geom_Curve> crv = occ::down_cast<Geom_Curve>(mySegments->Value(j - 1)->Copy());
    if (crv->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
    {
      occ::handle<Geom_BezierCurve> bes    = occ::down_cast<Geom_BezierCurve>(crv);
      double                        uFact  = mySplitParams->Value(j) - mySplitParams->Value(j - 1);
      double                        pp     = mySplitValues->Value(i - 1);
      double                        length = (par - pp) / uFact;
      bes->Segment(prevPar, prevPar + length);
      prevPar += length;
      myResultingCurves->SetValue(i - 1, bes);
    }
    else
      myResultingCurves->SetValue(i - 1, crv);
  }
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<Geom_Curve>>> ShapeUpgrade_ConvertCurve3dToBezier::
  Segments() const
{
  return mySegments;
}

//=================================================================================================

occ::handle<NCollection_HSequence<double>> ShapeUpgrade_ConvertCurve3dToBezier::SplitParams() const
{
  return mySplitParams;
}
