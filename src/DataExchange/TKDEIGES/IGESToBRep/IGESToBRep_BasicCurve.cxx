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

//=======================================================================
// modified:
// 21.02.2002 skl
// 21.12.98 rln, gka S4054
//: k5 abv 25 Dec 98: PRO8803 1901: extending method of fixing Multi > Degree
// 28.12.98 dce S3767 New messaging system
// #61 rln 05.01.99
// #60 rln 29.12.98 PRO17015
//: l3 abv 11.01.99: CATIA01.igs: using less values for checking short lines
//%11 pdn 12.01.98 CTS22023 correcting used tolerances
// sln 29.12.2001 OCC90 : Method checkBSplineCurve and verification before creation of bspline
// curves were added
//=======================================================================

#include <ElCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <IGESConvGeom.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESToBRep_BasicCurve.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <MoniTool_Macros.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

//: 36
// S3767
//=======================================================================
// function : CheckBSplineCurve
// purpose  : Check coincidede knots Check whether knots are in ascending
//           order and difference between values of weights more than 1000.
//           Send corresponding messages. The function returns false
//           if curve can not be created, true otherwise.
//=======================================================================
static bool checkBSplineCurve(IGESToBRep_BasicCurve*                    theCurve,
                              const occ::handle<IGESGeom_BSplineCurve>& theBSplineCurve,
                              NCollection_Array1<double>&               CKnots,
                              const NCollection_Array1<double>&         CWeights)
{
  // check whether difference between values of weights more than 1000.
  if (!theBSplineCurve->IsPolynomial())
  {
    double aMinValue = CWeights.Value(CWeights.Lower());
    double aMaxValue = CWeights.Value(CWeights.Lower());
    for (int i = CWeights.Lower() + 1; i <= CWeights.Upper(); i++)
    {
      if (CWeights.Value(i) < aMinValue)
        aMinValue = CWeights.Value(i);
      if (CWeights.Value(i) > aMaxValue)
        aMaxValue = CWeights.Value(i);
    }
    if (aMaxValue - aMinValue > 1000)
    {
      Message_Msg msg1374("IGES_1374"); // WARNING - Difference between weights is too big.
      theCurve->SendWarning(theBSplineCurve, msg1374);
    }
  }

  bool aResult = true;

  // check whether knots are in ascending order.
  for (int i = CKnots.Lower(); i < CKnots.Upper(); i++)
    if (CKnots.Value(i + 1) < CKnots.Value(i))
    {
      Message_Msg msg1373("IGES_1373"); // FAIL - Knots are not in ascending order
      theCurve->SendFail(theBSplineCurve, msg1373);
      aResult = false;
    }
  // Fix coincided knots
  if (aResult)
    ShapeConstruct_Curve::FixKnots(CKnots);

  return aResult;
}

//=================================================================================================

IGESToBRep_BasicCurve::IGESToBRep_BasicCurve()

{
  SetModeTransfer(false);
}

//=================================================================================================

IGESToBRep_BasicCurve::IGESToBRep_BasicCurve(const IGESToBRep_CurveAndSurface& CS)
    : IGESToBRep_CurveAndSurface(CS)
{
}

//=================================================================================================

IGESToBRep_BasicCurve::IGESToBRep_BasicCurve(const double eps,
                                             const double epsCoeff,
                                             const double epsGeom,
                                             const bool   mode,
                                             const bool   modeapprox,
                                             const bool   optimized)
    : IGESToBRep_CurveAndSurface(eps, epsCoeff, epsGeom, mode, modeapprox, optimized)
{
}

//=================================================================================================

occ::handle<Geom_Curve> IGESToBRep_BasicCurve::TransferBasicCurve(
  const occ::handle<IGESData_IGESEntity>& start)
{
  occ::handle<Geom_Curve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  try
  { //: 36 by abv 11.12.97: Geom_BSplineCurve fails if somw weights are <=0
    OCC_CATCH_SIGNALS
    // S4054
    if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve)))
    {
      DeclareAndCast(IGESGeom_BSplineCurve, st126, start);
      res = TransferBSplineCurve(st126);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_Line)))
    {
      DeclareAndCast(IGESGeom_Line, st110, start);
      res = TransferLine(st110);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc)))
    {
      DeclareAndCast(IGESGeom_CircularArc, st100, start);
      res = TransferCircularArc(st100);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_ConicArc)))
    {
      DeclareAndCast(IGESGeom_ConicArc, st104, start);
      res = TransferConicArc(st104);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CopiousData)))
    {
      DeclareAndCast(IGESGeom_CopiousData, st106, start);
      res = TransferCopiousData(st106);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineCurve)))
    {
      DeclareAndCast(IGESGeom_SplineCurve, st112, start);
      res = TransferSplineCurve(st112);
    }
    else
    {
      // AddFail(start, "The IGESEntity is not a basic curve.");
      // This case can not occur
      return res;
    }

  } //: 36
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "\n** Exception in IGESToBRep_BasicCurve::TransferBasicCurve : ";
    anException.Print(std::cout);
#endif
    (void)anException;
  }
  if (res.IsNull())
  {
    // AddFail(start, "The IGESEntity cannot be transferred.");
    // The more specific function have ever add a fail message for this entity
  }
  else
    res->Scale(gp_Pnt(0, 0, 0), GetUnitFactor());
  return res;
}

//=================================================================================================

occ::handle<Geom2d_Curve> IGESToBRep_BasicCurve::Transfer2dBasicCurve(
  const occ::handle<IGESData_IGESEntity>& start)
{
  occ::handle<Geom2d_Curve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  try
  { //: h8 abv 15 Jul 98: BUC60291 43693: Bspline Multiplicity > Degree+1 -> exception
    OCC_CATCH_SIGNALS

    // S4054
    if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve)))
    {
      DeclareAndCast(IGESGeom_BSplineCurve, st126, start);
      res = Transfer2dBSplineCurve(st126);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_Line)))
    {
      DeclareAndCast(IGESGeom_Line, st110, start);
      res = Transfer2dLine(st110);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc)))
    {
      DeclareAndCast(IGESGeom_CircularArc, st100, start);
      res = Transfer2dCircularArc(st100);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_ConicArc)))
    {
      DeclareAndCast(IGESGeom_ConicArc, st104, start);
      res = Transfer2dConicArc(st104);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CopiousData)))
    {
      DeclareAndCast(IGESGeom_CopiousData, st106, start);
      res = Transfer2dCopiousData(st106);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineCurve)))
    {
      DeclareAndCast(IGESGeom_SplineCurve, st112, start);
      res = Transfer2dSplineCurve(st112);
    }
    else
    {
      // AddFail(start, "The IGESEntity is not a basic curve.");
      // This case can not occur
      return res;
    }
  } //: h8
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "\n** Exception in IGESToBRep_BasicCurve::Transfer2dBasicCurve : ";
    anException.Print(std::cout);
#endif
    (void)anException;
  }
  return res;
}

//=================================================================================================

//
// A,B,C,D,E,F are the coefficients recorded in IGES. a,b,c,d,e,f are used to
// simplify the equations of conversion. They are already used in Euclid.

occ::handle<Geom_Curve> IGESToBRep_BasicCurve::TransferConicArc(
  const occ::handle<IGESGeom_ConicArc>& st)
{
  occ::handle<Geom_Curve> res;
  if (st.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }
  // If the Conic is closed, the start and end points will be ignored.
  if (!st->ComputedFormNumber())
  {
    Message_Msg msg1155("IGES_1155");
    SendFail(st, msg1155);
    //    AddFail(st, "Coefficients do not define correctly a conic.");
    return res;
  }

  // Conic = ax2+bxy+cy2+dx+ey+f=0 in the plane z=ZT.
  double a, b, c, d, e, f, ZT;
  st->Equation(a, b, c, d, e, f);

  ZT = st->ZPlane();

  gp_Pnt center, startPoint, endPoint;
  gp_Dir mainAxis, normAxis;
  double minorRadius, majorRadius;

  if (!GetModeTransfer() && st->HasTransf())
  {

    st->TransformedDefinition(center, mainAxis, minorRadius, majorRadius);
    normAxis = st->TransformedAxis();

    startPoint = st->TransformedStartPoint();
    endPoint   = st->TransformedEndPoint();
  }
  else
  {
    st->Definition(center, mainAxis, minorRadius, majorRadius);
    normAxis = st->Axis();

    startPoint.SetCoord(st->StartPoint().X(), st->StartPoint().Y(), ZT);
    endPoint.SetCoord(st->EndPoint().X(), st->EndPoint().Y(), ZT);
  }
  gp_Ax2 frame(center, normAxis, mainAxis);
  double t1 = 0.0, t2 = 0.0;
  if (st->IsFromEllipse())
  {

    // #60 rln 29.12.98 PRO17015 reading back face#67: ellipse with big radii produces
    // small coefficients
    // The dimensions should be also obliged:
    //[a]=[b]=[c]=L^-2
    // if ( (std::abs(a-c) <= GetEpsGeom()) && (std::abs(b) < GetEpsCoeff()))
    constexpr double eps2 = Precision::PConfusion() * Precision::PConfusion();
    if ((std::abs(a - c) <= eps2) && (std::abs(b) < eps2))
    {

      //                          =================
      //                          ==  Circle 3D  ==
      //                          =================

      res = new Geom_Circle(frame, minorRadius);

      if (!st->IsClosed())
      {

        gp_Circ circ(frame, minorRadius);

        t1 = ElCLib::Parameter(circ, startPoint);
        t2 = ElCLib::Parameter(circ, endPoint);
        if (t1 > t2 && (t1 - t2) > Precision::Confusion())
          t2 += 2. * M_PI;
        if (std::abs(t1 - t2) <= Precision::Confusion())
        { // t1 = t2
          Message_Msg msg1160("IGES_1160");
          SendWarning(st, msg1160);
        }
        else
          res = new Geom_TrimmedCurve(res, t1, t2);
      }
      return res;
    }
    else
    {
      // This is a no circular ellipse which will be computed with
      // the hyperbola at the end of this member.
    }
  }
  else if (st->IsFromParabola())
  {

    //                         ===================
    //                         ==  Parabola 3D  ==
    //                         ===================

    double focal = minorRadius;
    // PTV 26.03.2002
    focal = focal / 2.;
    gp_Parab parab(frame, focal);

    res = new Geom_Parabola(frame, focal);

    t1 = ElCLib::Parameter(parab, startPoint);
    t2 = ElCLib::Parameter(parab, endPoint);
    if (std::abs(t1 - t2) <= Precision::Confusion())
    { // t1 = t2
      Message_Msg msg1160("IGES_1160");
      SendWarning(st, msg1160);
      // AddWarning(st, "The trim of the parabola is not correct.");
    }
    else
      // if t1 > t2, the course of the curve is going to be reversed.
      res = new Geom_TrimmedCurve(res, t1, t2);

    return res;
  }

  // Same computing for the ellipse and the hyperbola.

  //              =============================================
  //              ==  Hyperbola or a no circular Ellipse 3D. ==
  //              =============================================

  if (st->IsFromEllipse())
  {
    res = new Geom_Ellipse(frame, majorRadius, minorRadius);

    if (!st->IsClosed())
    {
      gp_Elips elips(frame, majorRadius, minorRadius);

      t1 = ElCLib::Parameter(elips, startPoint);
      t2 = ElCLib::Parameter(elips, endPoint);
      if (t2 < t1 && (t1 - t2) > Precision::Confusion())
        t2 += 2. * M_PI;
      if (std::abs(t1 - t2) <= Precision::Confusion())
      { // t1 = t2
        Message_Msg msg1160("IGES_1160");
        SendWarning(st, msg1160);
        // AddWarning(st, "The trim of the ellipse is not correct, the result will be a ellipse.");
      }
      else
        res = new Geom_TrimmedCurve(res, t1, t2);
    }
  }
  else
  {

    gp_Hypr hpr(frame, majorRadius, minorRadius);

    t1 = ElCLib::Parameter(hpr, startPoint);
    t2 = ElCLib::Parameter(hpr, endPoint);

    res = new Geom_Hyperbola(frame, majorRadius, minorRadius);

    // pdn taking PConfusion for parameters.
    if (std::abs(t1 - t2) <= Precision::PConfusion())
    { // t1 = t2
      Message_Msg msg1160("IGES_1160");
      SendWarning(st, msg1160);
    }
    else if (t1 > t2)
      res = new Geom_TrimmedCurve(res, t2, t1); // parameter inversion.
    else
      res = new Geom_TrimmedCurve(res, t1, t2);
  }

  return res;
}

//=======================================================================
// function : Transfer2dConicArc
// purpose  : Transfer 2d of a ConicArc to be used as a boundary of a Face
//=======================================================================

occ::handle<Geom2d_Curve> IGESToBRep_BasicCurve::Transfer2dConicArc(
  const occ::handle<IGESGeom_ConicArc>& st)
{
  occ::handle<Geom2d_Curve> res;
  if (st.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  if (!st->ComputedFormNumber())
  {
    Message_Msg msg1155("IGES_1155");
    SendFail(st, msg1155);
    return res;
  }

  double a, b, c, d, e, f;
  // Conic = ax2+bxy+cy2+dx+ey+f=0.
  st->Equation(a, b, c, d, e, f);

  gp_Pnt   center3d;
  gp_Dir   mainAxis3d;
  gp_Pnt2d startPoint, endPoint;
  double   minorRadius, majorRadius;

  SetEpsilon(1.E-03);
  if (!st->TransformedAxis().IsParallel /*#45 rln 23.11.98 IsEqual*/ (st->Axis(), GetEpsilon()))
  {
    SetModeTransfer(true);
    // Only not to use Trsf.
    // the normal axis is not parallel with Z axis.
    SendWarning(st, "The Trsf is not compatible with a transfer2d, it will not applied.");
  }

  if (!GetModeTransfer() && st->HasTransf())
  {

    st->TransformedDefinition(center3d, mainAxis3d, minorRadius, majorRadius);
    startPoint.SetCoord(st->TransformedStartPoint().X(), st->TransformedStartPoint().Y());
    endPoint.SetCoord(st->TransformedEndPoint().X(), st->TransformedEndPoint().Y());
  }
  else
  {
    st->Definition(center3d, mainAxis3d, minorRadius, majorRadius);
    startPoint = st->StartPoint();
    endPoint   = st->EndPoint();
  }

  gp_Pnt2d center(center3d.X(), center3d.Y());
  gp_Dir2d mainAxis(mainAxis3d.X(), mainAxis3d.Y());
  gp_Ax2d  frame(center, mainAxis);
  double   t1 = 0.0, t2 = 0.0;
  if (st->IsFromEllipse())
  {

    // #60 rln 29.12.98 PRO17015
    // if ( (std::abs(a-c) <= GetEpsGeom()) && (std::abs(b) < GetEpsCoeff()))
    constexpr double eps2 = Precision::PConfusion() * Precision::PConfusion();
    if ((std::abs(a - c) <= eps2) && (std::abs(b) < eps2))
    {

      //                          =================
      //                          ==  Circle 2D  ==
      //                          =================

      res = new Geom2d_Circle(frame, minorRadius);
      // #45 rln 23.11.98
      if (st->TransformedAxis().IsOpposite(st->Axis(), GetEpsilon()))
        res->Reverse();

      if (!st->IsClosed())
      {

        gp_Circ2d circ =
          occ::down_cast<Geom2d_Circle>(res)->Circ2d(); // #45 rln (frame, minorRadius);

        t1 = ElCLib::Parameter(circ, startPoint);
        t2 = ElCLib::Parameter(circ, endPoint);

        if (t2 < t1 && (t1 - t2) > Precision::PConfusion())
          t2 += 2. * M_PI;
        if (std::abs(t1 - t2) <= Precision::PConfusion())
        { // t1 = t2
          Message_Msg msg1160("IGES_1160");
          SendWarning(st, msg1160);
        }
        else
          res = new Geom2d_TrimmedCurve(res, t1, t2);
      }
      return res;
    }
    else
    {
      // This is a no circular ellipse, it will be computed with the hyperbola.
    }
  }
  else if (st->IsFromParabola())
  {

    //                         ===================
    //                         ==  Parabola 2D  ==
    //                         ===================

    double focal = minorRadius;
    // PTV 26.03.2002
    focal = focal / 2.;
    res   = new Geom2d_Parabola(frame, focal);
    // #45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite(st->Axis(), GetEpsilon()))
      res->Reverse();

    gp_Parab2d parab = occ::down_cast<Geom2d_Parabola>(res)->Parab2d(); // #45 rln (frame, focal);

    t1 = ElCLib::Parameter(parab, startPoint);
    t2 = ElCLib::Parameter(parab, endPoint);
    if (std::abs(t1 - t2) <= Precision::PConfusion())
    { // t1 = t2
      Message_Msg msg1160("IGES_1160");
      SendWarning(st, msg1160);
    }
    else if (t1 > t2)
      res = new Geom2d_TrimmedCurve(res, t2, t1); // parameter inversion.
    else
      res = new Geom2d_TrimmedCurve(res, t1, t2);
    return res;
  }

  /* Same computing for the ellipse2d and the hyperbola2d. */

  //              ============================================
  //              ==  Hyperbola or a no circular Ellipse 2D ==
  //              ============================================

  if (st->IsFromEllipse())
  {

    res = new Geom2d_Ellipse(frame, majorRadius, minorRadius);
    // #45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite(st->Axis(), GetEpsilon()))
      res->Reverse();

    if (!st->IsClosed())
    {

      // clang-format off
      gp_Elips2d elips = occ::down_cast<Geom2d_Ellipse>(res)->Elips2d();//#45 rln (frame, majorRadius, minorRadius);
      // clang-format on

      t1 = ElCLib::Parameter(elips, startPoint);
      t2 = ElCLib::Parameter(elips, endPoint);
      if (t2 < t1 && (t1 - t2) > Precision::PConfusion())
        t2 += 2. * M_PI;
      if (std::abs(t1 - t2) <= Precision::PConfusion())
      { // t1 = t2
        Message_Msg msg1160("IGES_1160");
        SendWarning(st, msg1160);
      }
      else
        res = new Geom2d_TrimmedCurve(res, t1, t2);
    }
  }
  else
  {

    res = new Geom2d_Hyperbola(frame, majorRadius, minorRadius);
    // #45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite(st->Axis(), GetEpsilon()))
      res->Reverse();

    // clang-format off
    gp_Hypr2d hpr = occ::down_cast<Geom2d_Hyperbola>(res)->Hypr2d();//#45 rln (frame, majorRadius, minorRadius);
    // clang-format on

    t1 = ElCLib::Parameter(hpr, startPoint);
    t2 = ElCLib::Parameter(hpr, endPoint);

    if (std::abs(t1 - t2) <= Precision::PConfusion())
    { // t1 = t2
      Message_Msg msg1160("IGES_1160");
      SendWarning(st, msg1160);
    }
    else if (t1 > t2)
      res = new Geom2d_TrimmedCurve(res, t2, t1); // parameter inversion.
    else
      res = new Geom2d_TrimmedCurve(res, t1, t2);
  }

  return res;
}

//=================================================================================================

occ::handle<Geom_Curve> IGESToBRep_BasicCurve::TransferCircularArc(
  const occ::handle<IGESGeom_CircularArc>& st)
{
  occ::handle<Geom_Curve> res;
  if (st.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  gp_Dir tNormAxis, tMainAxis;
  gp_Ax2 frame;
  gp_Pnt startPoint, endPoint;

  if (!GetModeTransfer() && st->HasTransf())
  {

    tNormAxis = st->TransformedAxis();

    gp_GTrsf loc = st->Location();
    loc.SetTranslationPart(gp_XYZ(0., 0., 0.));
    gp_XYZ mainAxis(1., 0., 0.);
    loc.Transforms(mainAxis);
    tMainAxis = gp_Dir(mainAxis);

    startPoint = st->TransformedStartPoint();
    endPoint   = st->TransformedEndPoint();

    frame = gp_Ax2(st->TransformedCenter(), tNormAxis, tMainAxis);
  }
  else
  {
    tNormAxis = st->Axis();
    tMainAxis.SetCoord(1., 0., 0.);

    double ZT = st->ZPlane();
    startPoint.SetCoord(st->StartPoint().X(), st->StartPoint().Y(), ZT);
    endPoint.SetCoord(st->EndPoint().X(), st->EndPoint().Y(), ZT);
    gp_Pnt centerPoint(st->Center().X(), st->Center().Y(), ZT);

    frame = gp_Ax2(centerPoint, tNormAxis, tMainAxis);
  }

  res = new Geom_Circle(frame, st->Radius());

  gp_Circ circ(frame, st->Radius());

  double t1 = 0.0, t2 = 0.0;

  t1 = ElCLib::Parameter(circ, startPoint);
  t2 = ElCLib::Parameter(circ, endPoint);

  if (st->IsClosed() && t1 >= GetEpsGeom())
    t2 = t1 + 2. * M_PI;
  if (!st->IsClosed() && fabs(t1 - t2) <= Precision::PConfusion())
  {
    // micro-arc
    // cky 27 Aout 1996 : t2-t1 vaut distance(start,end)/rayon
    t2 = t1 + startPoint.Distance(endPoint) / st->Radius();
  }
  if (!st->IsClosed() || t1 >= GetEpsGeom())
  {
    if (t2 < t1)
      t2 += 2. * M_PI;
    res = new Geom_TrimmedCurve(res, t1, t2);
  }

  return res;
}

//=================================================================================================

occ::handle<Geom2d_Curve> IGESToBRep_BasicCurve::Transfer2dCircularArc(
  const occ::handle<IGESGeom_CircularArc>& st)
{
  occ::handle<Geom2d_Curve> res;
  if (st.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  gp_XYZ center(st->Center().X(), st->Center().Y(), 0.);
  gp_XYZ mainAxis(1., 0., 0.);

  SetEpsilon(1.E-03);
  if (!st->TransformedAxis().IsParallel /*#45 rln 23.11.98 IsEqual*/ (st->Axis(), GetEpsilon()))
  {
    SetModeTransfer(true); // Only not to use Trsf
    Message_Msg msg1165("IGES_1165");
    // clang-format off
    SendWarning(st, msg1165); //"The Trsf is not compatible with a transfer2d, it will not applied."
    // clang-format on
  }

  if (!GetModeTransfer() && st->HasTransf())
  {
    gp_GTrsf loc = st->Location();
    loc.Transforms(center);
    loc.SetTranslationPart(gp_XYZ(0., 0., 0.));
    loc.Transforms(mainAxis);
  }
  gp_Pnt2d tCenter(center.X(), center.Y());
  gp_Dir2d tMainAxis(mainAxis.X(), mainAxis.Y());
  gp_Ax2d  frame(tCenter, tMainAxis);

  res = new Geom2d_Circle(frame, st->Radius());

  gp_Pnt2d startPoint, endPoint;
  if (!GetModeTransfer() && st->HasTransf())
  {
    startPoint.SetCoord(st->TransformedStartPoint().X(), st->TransformedStartPoint().Y());
    endPoint.SetCoord(st->TransformedEndPoint().X(), st->TransformedEndPoint().Y());
    // #45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite(st->Axis(), GetEpsilon()))
      res->Reverse();
  }
  else
  {
    startPoint = st->StartPoint();
    endPoint   = st->EndPoint();
  }

  gp_Circ2d circ = occ::down_cast<Geom2d_Circle>(res)->Circ2d(); // #45 rln (frame, st->Radius());

  double t1 = 0.0, t2 = 0.0;

  t1 = ElCLib::Parameter(circ, startPoint);
  t2 = ElCLib::Parameter(circ, endPoint);

  if (st->IsClosed() && t1 >= GetEpsGeom())
    t2 = t1 + 2. * M_PI;
  if (!st->IsClosed() && fabs(t1 - t2) <= Precision::PConfusion())
  {
    // micro-arc
    // cky 27 Aout 1996 : t2-t1 vaut distance(start,end)/rayon
    t2 = t1 + startPoint.Distance(endPoint) / st->Radius();
  }
  if (!st->IsClosed() || t1 >= GetEpsGeom())
  {
    if (t2 < t1)
      t2 += 2. * M_PI;
    res = new Geom2d_TrimmedCurve(res, t1, t2);
  }
  return res;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> IGESToBRep_BasicCurve::TransferSplineCurve(
  const occ::handle<IGESGeom_SplineCurve>& st)
{
  occ::handle<Geom_BSplineCurve> resconv;
  if (st.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return resconv;
  }

  double epscoef = GetEpsCoeff();
  double epsgeom = GetEpsGeom();

  int result = IGESConvGeom::SplineCurveFromIGES(st, epscoef, epsgeom, resconv);

  switch (result)
  {
    case 5: {
      Message_Msg msg246("XSTEP_246");
      SendFail(st, msg246);
      // less than on segment (no result produced)
      return resconv;
    }
    case 4: {
      Message_Msg msg1170("IGES_1170");
      SendFail(st, msg1170);
      // Polynomial equation is not correct ( no result produced
      return resconv;
    }
    case 3: {
      Message_Msg msg1175("IGES_1175");
      SendFail(st, msg1175);
      // Error during creation of control points ( no result produced)
      return resconv;
    }
    case 2: {
      Message_Msg msg1180("IGES_1180");
      SendFail(st, msg1180);
      // SplineType not processed (allowed : max 3) (no result produced)
      return resconv;
    }
    default:
      break;
  }

  //  Checking C2 and C1 continuity :
  //  ===============================
  IGESConvGeom::IncreaseCurveContinuity(resconv,
                                        std::min(Precision::Confusion(), epsgeom),
                                        GetContinuity());
  return resconv;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> IGESToBRep_BasicCurve::Transfer2dSplineCurve(
  const occ::handle<IGESGeom_SplineCurve>& st)
{
  occ::handle<Geom2d_BSplineCurve> res;
  if (st.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  // 3d transfer First
  // =================
  // The same Presision as in BSpline 2d is used
  double epsGeom = GetEpsGeom();
  SetEpsGeom(Precision::PConfusion());
  occ::handle<Geom_BSplineCurve> res3d = TransferSplineCurve(st);
  SetEpsGeom(epsGeom);
  if (res3d.IsNull())
    return res; // The transfer was not over the top.

  // 2d
  // ==
  int nbPoles = res3d->NbPoles();

  NCollection_Array1<gp_Pnt2d>  bspoles2d(1, nbPoles);
  const NCollection_Array1<double>& knots = res3d->Knots();
  const NCollection_Array1<int>&    multi = res3d->Multiplicities();

  for (int i = bspoles2d.Lower(); i <= bspoles2d.Upper(); i++)
    bspoles2d.SetValue(i, gp_Pnt2d(res3d->Pole(i).X(), res3d->Pole(i).Y()));

  res = new Geom2d_BSplineCurve(bspoles2d, knots, multi, res3d->Degree());
  return res;
}

//=================================================================================================

occ::handle<Geom_Curve> IGESToBRep_BasicCurve::TransferBSplineCurve(
  const occ::handle<IGESGeom_BSplineCurve>& start)

{
  occ::handle<Geom_BSplineCurve> BSplineRes;
  occ::handle<Geom_Curve>        res;

  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  int Degree = start->Degree();

  if (Degree <= 0 || Degree > Geom_BSplineCurve::MaxDegree())
  {
    Message_Msg msg1190("IGES_1190");
    SendFail(start, msg1190);
    // Improper degree either lower or equal to 0 or upper to MaxDegree
    return res;
  }

  //  Filling poles array :
  //  =====================

  int NbPoles    = start->NbPoles();
  int newNbPoles = NbPoles;

  if (NbPoles < 2)
  {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195);
    // Transfer aborted for a BSpline Curve : Number of poles lower than 2
    return res;
  }

  NCollection_Array1<gp_Pnt> Pole(1, NbPoles);
  int                        PoleIndex = Pole.Lower();
  int                        i; // szv#4:S4163:12Mar99 j unused

  if (!GetModeTransfer() && start->HasTransf())
    for (i = 0; i <= start->UpperIndex(); i++)
      Pole.SetValue(PoleIndex++, start->TransformedPole(i));
  else
    for (i = 0; i <= start->UpperIndex(); i++)
      Pole.SetValue(PoleIndex++, start->Pole(i));

  //  Filling knots & multiplicities arrays :
  //  ========================================

  int                        NbKnots = start->NbKnots();
  NCollection_Array1<double> TempKnot(1, NbKnots);
  NCollection_Array1<int>    TempMult(1, NbKnots);
  TempMult.Init(1);
  int KnotIndex = TempKnot.Lower();

  TempKnot.SetValue(KnotIndex, start->Knot(-Degree));

  //  If several identical IGES knots are encountered, corresponding
  //  multiplicity is increased
  //  ==============================================================

  for (i = 1 - Degree; i < NbKnots - Degree; i++)
  {

    double Knot1 = start->Knot(i);
    double Knot2 = start->Knot(i - 1);
    //    double ek    =  Epsilon(Knot1);

    if (std::abs(Knot1 - Knot2) <= Epsilon(Knot1))
      TempMult.SetValue(KnotIndex, TempMult.Value(KnotIndex) + 1);
    else
      TempKnot.SetValue(++KnotIndex, Knot1);
  }

  //  Final knots & multiplicities arrays are dimensioned so as to be fully
  //  filled
  //  =======================================================================

  NCollection_Array1<double> Knot(1, KnotIndex);
  NCollection_Array1<int>    Mult(1, KnotIndex);

  int SumOfMult = 0;

  NCollection_Sequence<int> SeqIndex;
  int                       DelIndex;
  int                       OldSumOfMult = 0;
  for (i = 1; i <= KnotIndex; i++)
  { //: k5 abv 25 Dec 98: cycle modified
    int aMult   = TempMult.Value(i);
    int maxMult = (i == 1 || i == KnotIndex ? Degree + 1 : Degree);
    if (aMult > maxMult)
    {
      Message_Msg       msg1200("IGES_1200"); // #61 rln 05.01.99
      const char* const vide("");
      msg1200.Arg(vide);
      msg1200.Arg(vide);
      msg1200.Arg(vide);
      SendWarning(start, msg1200); // Multiplicity > Degree (or Degree+1 at end); corrected
      for (DelIndex = OldSumOfMult + 1; aMult > maxMult; DelIndex++, aMult--)
      {
        newNbPoles--;
        SeqIndex.Append(DelIndex);
      }
    }
    OldSumOfMult += TempMult.Value(i);
    Knot.SetValue(i, TempKnot.Value(i));
    Mult.SetValue(i, aMult);
    SumOfMult += aMult;
  }

  // Update of the poles array during multiplicity correction
  NCollection_Array1<gp_Pnt> Poles(1, newNbPoles);
  NCollection_Sequence<int>  PoleInd;

  if (newNbPoles < NbPoles)
  {
    for (i = 1; i <= NbPoles; i++)
      PoleInd.Append(i);
    int Offset = 0;
    for (int itab = 1; itab <= SeqIndex.Length(); itab++)
    {
      DelIndex = SeqIndex.Value(itab) - Offset;
      PoleInd.Remove(DelIndex);
      Offset++;
    }
    int nbseq = PoleInd.Length();
    if (nbseq == newNbPoles)
    {
      int indj = 1;
      for (i = 1; i <= newNbPoles; i++)
      {
        Poles.SetValue(i, Pole.Value(PoleInd.Value(indj++)));
      }
    }
  }

  else
  {
    for (i = 1; i <= newNbPoles; i++)
    {
      Poles.SetValue(i, Pole.Value(i));
    }
  }

  if (!(SumOfMult == newNbPoles + Degree + 1))
  {
    Message_Msg       msg1210("IGES_1210");
    const char* const vide("");
    msg1210.Arg(vide);
    msg1210.Arg(vide);
    SendWarning(start, msg1210);
    // Sum of multiplicities not equal to the sum : Count of poles + Degree + 1
  }

  //  Output BSpline curve with the array of pole weights if any :
  //  ============================================================

  NCollection_Array1<double> Weight(1, newNbPoles);

  if (start->IsPolynomial())
  {
    //: 5    BSplineC = new Geom_BSplineCurve(Poles, Knot, Mult, Degree);
  }
  else
  {
    NCollection_Array1<double> PoleWeight(1, NbPoles);
    bool                       polynomial      = true;
    double                     WeightReference = start->Weight(0);
    int                        WeightIndex     = PoleWeight.Lower();

    for (i = 0; i <= start->UpperIndex(); i++)
    {
      polynomial =
        std::abs(start->Weight(i) - WeightReference) <= Epsilon(WeightReference) && polynomial;
      //: 39 by abv 15.12.97
      double weight = start->Weight(i);
      if (weight < Precision::PConfusion())
      {
        Message_Msg msg1215("IGES_1215");
        SendFail(start, msg1215);
        // Some weights are not positive
        return res;
      }
      PoleWeight.SetValue(WeightIndex++, weight);
      //: 39      PoleWeight.SetValue(WeightIndex++, start->Weight(i));
    }
    if (polynomial)
    {
      Message_Msg msg1220("IGES_1220");
      msg1220.Arg("curve");
      SendWarning(start, msg1220);
      // Rational curve is polynomial
    }
    // Update of the Weight array during multiplicity correction
    if (newNbPoles < NbPoles)
    {
      int indj = 1;
      for (i = 1; i <= newNbPoles; i++)
      {
        Weight.SetValue(i, PoleWeight.Value(PoleInd.Value(indj++)));
      }
    }
    else
    {
      for (i = 1; i <= newNbPoles; i++)
      {
        Weight.SetValue(i, PoleWeight.Value(i));
      }
    }
    //: 5    BSplineC = new Geom_BSplineCurve(Poles, Weight, Knot, Mult, Degree);
  }

  // If curve can not be created do nothing
  if (Poles.Size() < 2 || !checkBSplineCurve(this, start, Knot, Weight))
  {
    return BSplineRes;
  }

  {
    try
    {
      OCC_CATCH_SIGNALS
      if (start->IsPolynomial())
        BSplineRes = new Geom_BSplineCurve(Poles, Knot, Mult, Degree);
      else
        BSplineRes = new Geom_BSplineCurve(Poles, Weight, Knot, Mult, Degree);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "\n** Exception in IGESToBRep_BasicCurve::TransferBSplineCurve during creation "
                   "of Geom_BSplineCurve : ";
      anException.Print(std::cout);
#endif
      (void)anException;
      return BSplineRes;
    }
  }

  double First = BSplineRes->FirstParameter();
  double Last  = BSplineRes->LastParameter();
  double Udeb  = start->UMin();
  double Ufin  = start->UMax();
  //%11 pdn 12.01.98 CTS22023
  // if ( (Udeb-First) > Precision::PConfusion() || (Last-Ufin) > Precision::PConfusion() )
  //  BSplineRes->Segment(Udeb, Ufin);
  // res = BSplineRes;

  //  IGESConvGeom::IncreaseCurveContinuity (BSplineRes,Min(Precision::Confusion(),GetEpsGeom()),
  //  GetContinuity());

  // skl 21.02.2002 (exception in OCC133 and for file
  //                 "/dn04/OS/USINOR/UIdev/src/IsoLim/dat/igs/ps1002-v5.igs")
  occ::handle<Geom_BSplineCurve> BSplineRes2 = BSplineRes;
  if (((Udeb - First) > -Precision::PConfusion() && (Last - Ufin) > -Precision::PConfusion())
      && Udeb <= Ufin)
  {
    try
    {
      OCC_CATCH_SIGNALS
      if (std::abs(Ufin - Udeb) > Precision::PConfusion())
        BSplineRes->Segment(Udeb, Ufin);
      res = BSplineRes;
    }
    catch (Standard_Failure const&)
    {
      occ::handle<Geom_TrimmedCurve> gtc = new Geom_TrimmedCurve(BSplineRes2, Udeb, Ufin);
      res                                = gtc;
    }
  }
  else
    res = BSplineRes;

  return res;
}

//=================================================================================================

occ::handle<Geom2d_Curve> IGESToBRep_BasicCurve::Transfer2dBSplineCurve(
  const occ::handle<IGESGeom_BSplineCurve>& start)
{
  occ::handle<Geom2d_Curve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  occ::handle<Geom2d_BSplineCurve> BSplineC;
  occ::handle<Geom_BSplineCurve>   Bspline;
  bool                             IsTrimmed = false;
  double                           Deb = 0., Fin = 0.;

  //  3d transfer first :
  //  ===================
  //  double epsGeom = GetEpsGeom();
  //  SetEpsGeom(Precision::PConfusion());
  occ::handle<Geom_Curve> res3d = TransferBSplineCurve(start);
  //  SetEpsGeom(epsGeom);
  if (res3d.IsNull())
  {
    return res;
  }

  if (res3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    DeclareAndCast(Geom_TrimmedCurve, TrimC, res3d);
    occ::handle<Geom_Curve> BasicCurve = TrimC->BasisCurve();
    Deb                                = TrimC->FirstParameter();
    Fin                                = TrimC->LastParameter();
    IsTrimmed                          = true;
    if (BasicCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      DeclareAndCast(Geom_BSplineCurve, BSpline, BasicCurve);
      Bspline = BSpline;
    }
    else
    {
      return res;
    }
  }
  else if (res3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    DeclareAndCast(Geom_BSplineCurve, BSpline, res3d);
    Bspline = BSpline;
  }

  //  Creating 2d poles :
  //  ===================

  int                          NbPoles = Bspline->NbPoles();
  NCollection_Array1<gp_Pnt2d> Pole(1, NbPoles);

  for (int i = 1; i <= NbPoles; i++)
  {
    gp_Pnt2d aPole2d(Bspline->Pole(i).X(), Bspline->Pole(i).Y());
    Pole.SetValue(i, aPole2d);
  }

  //  Knots and multiplicities are the same :
  //  =======================================

  const NCollection_Array1<double>& Knot = Bspline->Knots();
  const NCollection_Array1<int>&    Mult = Bspline->Multiplicities();

  int Degree = Bspline->Degree();

  if (Bspline->IsRational())
  {
    const NCollection_Array1<double>* aWPtr = Bspline->Weights();
    if (aWPtr != nullptr)
    {
      BSplineC = new Geom2d_BSplineCurve(Pole, *aWPtr, Knot, Mult, Degree);
    }
    else
    {
      BSplineC = new Geom2d_BSplineCurve(Pole, Knot, Mult, Degree);
    }
  }
  else
    BSplineC = new Geom2d_BSplineCurve(Pole, Knot, Mult, Degree);

  res = BSplineC;

  // case where the Bspline is trimmed.
  if (IsTrimmed)
  {
    occ::handle<Geom2d_TrimmedCurve> TC = new Geom2d_TrimmedCurve(BSplineC, Deb, Fin, true);
    res                                 = TC;
  }

  return res;
}

//=================================================================================================

occ::handle<Geom_Curve> IGESToBRep_BasicCurve::TransferLine(const occ::handle<IGESGeom_Line>& start)
{
  occ::handle<Geom_Curve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  gp_Pnt Ps, Pe;

  if (!GetModeTransfer() && start->HasTransf())
  {
    Ps = start->TransformedStartPoint();
    Pe = start->TransformedEndPoint();
  }
  else
  {
    Ps = start->StartPoint();
    Pe = start->EndPoint();
  }

  // modification of 15/10/97: less severe test
  // many points confused at GetEpsGeom()*GetUnitFactor()
  if (!Ps.IsEqual(Pe, Precision::Confusion()))
  { //: l3 abv 11 Jan 99: GetEpsGeom()*GetUnitFactor()/10.)) {
    gp_Lin                 line(Ps, gp_Dir(gp_Vec(Ps, Pe)));
    double                 t1    = ElCLib::Parameter(line, Ps);
    double                 t2    = ElCLib::Parameter(line, Pe);
    occ::handle<Geom_Line> Gline = new Geom_Line(line);
    if (Precision::IsNegativeInfinite(t1))
      t1 = -Precision::Infinite();
    if (Precision::IsPositiveInfinite(t2))
      t2 = Precision::Infinite();
    res = new Geom_TrimmedCurve(Gline, t1, t2);
  }
  else
  {
    Message_Msg msg1225("IGES_1225");
    SendFail(start, msg1225);
    // StartPoint and EndPoint of the line are the same Point
  }

  return res;
}

//=================================================================================================

occ::handle<Geom2d_Curve> IGESToBRep_BasicCurve::Transfer2dLine(
  const occ::handle<IGESGeom_Line>& start)
{
  occ::handle<Geom2d_Curve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  gp_Pnt2d beg, end;

  if (!GetModeTransfer() && start->HasTransf())
  {
    beg.SetCoord(start->TransformedStartPoint().X(), start->TransformedStartPoint().Y());
    end.SetCoord(start->TransformedEndPoint().X(), start->TransformedEndPoint().Y());
  }
  else
  {
    beg.SetCoord(start->StartPoint().X(), start->StartPoint().Y());
    end.SetCoord(start->EndPoint().X(), start->EndPoint().Y());
  }

  if (!beg.IsEqual(end, Precision::PConfusion()))
  { //: l3 abv 11 Jan 99: GetEpsCoeff())) {
    gp_Lin2d                 line2d(beg, gp_Dir2d(gp_Vec2d(beg, end)));
    double                   t1      = ElCLib::Parameter(line2d, beg);
    double                   t2      = ElCLib::Parameter(line2d, end);
    occ::handle<Geom2d_Line> Gline2d = new Geom2d_Line(line2d);
    if (Precision::IsNegativeInfinite(t1))
      t1 = -Precision::Infinite();
    if (Precision::IsPositiveInfinite(t2))
      t2 = Precision::Infinite();
    res = new Geom2d_TrimmedCurve(Gline2d, t1, t2);
  }
  // added by rln 18/12/97 CSR# CTS18544 entity 25168 and 31273
  // generating fail the same as above
  else
  {
    Message_Msg msg1225("IGES_1225");
    SendFail(start, msg1225); // StartPoint and EndPoint of the 2d line are the same Point
  }
  return res;
}

//=================================================================================================

occ::handle<Geom_Transformation> IGESToBRep_BasicCurve::TransferTransformation(
  const occ::handle<IGESGeom_TransformationMatrix>& start)

{
  occ::handle<Geom_Transformation> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  gp_Trsf resultat;
  SetEpsilon(1.E-05);
  if (IGESData_ToolLocation::ConvertLocation(GetEpsilon(), start->Value(), resultat))
    res = new Geom_Transformation(resultat);
  else
  {
    Message_Msg msg1036("IGES_1036");
    SendFail(start, msg1036); // Transformation : not a similarity
  }
  return res;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> IGESToBRep_BasicCurve::TransferCopiousData(
  const occ::handle<IGESGeom_CopiousData>& start)

{
  occ::handle<Geom_BSplineCurve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  int FormNb = start->FormNumber();
  if (FormNb != 11 && FormNb != 12 && FormNb != 63)
  {
    Message_Msg msg1240("IGES_1240");
    SendWarning(start, msg1240);
    // "Copious Data : Form number is different from 11, 12 or 63 so the vector treatment is
    // skipped");
  }

  int NbPoints = start->NbPoints();
  if (NbPoints < 2)
  {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195); // Count of points lower than 2
    return res;
  }

  //  Filling array of poles :
  //  ========================

  NCollection_Array1<gp_Pnt> TempPole(1, NbPoints);
  int                        TempIndex = TempPole.Lower();

  if (!GetModeTransfer() && start->HasTransf())
  {
    TempPole.SetValue(TempIndex, start->TransformedPoint(1));
  }
  else
  {
    TempPole.SetValue(TempIndex, start->Point(1));
  }

  TempIndex++;
  int i; // svv Jan 10 2000 : porting on DEC
  for (i = 2; i <= NbPoints; i++)
  {
    gp_Pnt aPole;
    if (!GetModeTransfer() && start->HasTransf())
      aPole = start->TransformedPoint(i);
    else
      aPole = start->Point(i);
    // #2 pdn 7 May 1998 BUC50028
    //  delete GetUnitFactor()
    //   if (!aPole.IsEqual(TempPole(TempIndex-1),GetEpsGeom()))
    // S4054: some filter must be kept UKI60556 entity 7 (two equal points)
    if (!aPole.IsEqual(TempPole(TempIndex - 1), gp::Resolution()))
      TempPole.SetValue(TempIndex++, aPole);
  }

  NbPoints = TempIndex - TempPole.Lower();

  // #1 pdn 7 May 1998  BUC50028 entity 6307
  if (NbPoints == 1)
  {
    Message_Msg msg1235("IGES_1235");
    SendFail(start, msg1235);
    // The curve degenerates to a point");
    return res;
  }
  NCollection_Array1<gp_Pnt> Pole(1, NbPoints);

  TempIndex = TempPole.Lower();
  for (i = Pole.Lower(); i <= Pole.Upper(); i++)
    Pole.SetValue(i, TempPole.Value(TempIndex++));

  //  Filling array of knots :
  //  ========================

  NCollection_Array1<double> Knot(1, NbPoints);

  Knot.SetValue(Knot.Lower(), 0.0);

  for (i = Knot.Lower() + 1; i <= Knot.Upper(); i++)
  {
    gp_Pnt Pole1    = Pole.Value(i);
    gp_Pnt Pole2    = Pole.Value(i - 1);
    double KnotDist = Pole1.Distance(Pole2);
    Knot.SetValue(i, Knot.Value(i - 1) + KnotDist);
  }

  int Degree = 1;

  NCollection_Array1<int> Mult(1, NbPoints);
  Mult.Init(Degree);
  Mult.SetValue(Mult.Lower(), Degree + 1);
  Mult.SetValue(Mult.Upper(), Degree + 1);

  res = new Geom_BSplineCurve(Pole, Knot, Mult, Degree);

  IGESConvGeom::IncreaseCurveContinuity(res,
                                        std::max(GetEpsGeom() / 10., Precision::Confusion()),
                                        GetContinuity());
  return res;
}

//                    ================================
//                    ==  TRANSFER 2D Copious data  ==
//                    ================================

occ::handle<Geom2d_BSplineCurve> IGESToBRep_BasicCurve::Transfer2dCopiousData(
  const occ::handle<IGESGeom_CopiousData>& start)
{
  occ::handle<Geom2d_BSplineCurve> res;
  if (start.IsNull())
  {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  int FormNb = start->FormNumber();
  if (FormNb != 11 && FormNb != 12 && FormNb != 63)
  {
    Message_Msg msg1240("IGES_1240");
    SendWarning(start, msg1240);
    // "Copious Data : Form number is different from 11, 12 or 63 so the vector treatment is
    // skipped");
  }

  int NbPoints = start->NbPoints();
  if (NbPoints < 2)
  {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195); // Count of points lower than 2
    return res;
  }

  //  Filling array of poles :
  //  ========================

  NCollection_Array1<gp_Pnt2d> TempPole(1, NbPoints);
  int                          TempIndex = TempPole.Lower();

  if (!GetModeTransfer() && start->HasTransf())
    TempPole.SetValue(TempIndex,
                      gp_Pnt2d(start->TransformedPoint(1).X(), start->TransformedPoint(1).Y()));
  else
    TempPole.SetValue(TempIndex, gp_Pnt2d(start->Point(1).X(), start->Point(1).Y()));

  TempIndex++;
  int i; // svv Jan 10 2000 : porting on DEC
  for (i = 2; i <= NbPoints; i++)
  {
    gp_Pnt2d aPole;
    if (!GetModeTransfer() && start->HasTransf())
      aPole = gp_Pnt2d(start->TransformedPoint(i).X(), start->TransformedPoint(i).Y());
    else
      aPole = gp_Pnt2d(start->Point(i).X(), start->Point(i).Y());
    //    if (!aPole.IsEqual(TempPole(TempIndex-1), GetEpsCoeff())) //modified by rln 16/12/97 CSR#
    //    PRO11641 entity 46GetEpsGeom()*GetUnitFactor()
    // S4054: some filter must be kept UKI60556 entity 7 (two equal points)
    if (!aPole.IsEqual(TempPole(TempIndex - 1), gp::Resolution()))
      TempPole.SetValue(TempIndex++, aPole);
  }

  NbPoints = TempIndex - TempPole.Lower();
  // added by rln on 26/12/97 to avoid exception when creating Bspline from one point
  if (NbPoints == 1)
  {
    Message_Msg msg1235("IGES_1235");
    SendFail(start, msg1235);
    // The curve degenerates to a point");
    return res;
  }
  NCollection_Array1<gp_Pnt2d> Pole(1, NbPoints);

  TempIndex = TempPole.Lower();
  for (i = Pole.Lower(); i <= Pole.Upper(); i++)
    Pole.SetValue(i, TempPole.Value(TempIndex++));

  //  Filling array of knots :
  //  ========================

  NCollection_Array1<double> Knot(1, NbPoints);

  Knot.SetValue(Knot.Lower(), 0.0);

  for (i = Knot.Lower() + 1; i <= Knot.Upper(); i++)
  {
    gp_Pnt2d Pole1    = Pole.Value(i);
    gp_Pnt2d Pole2    = Pole.Value(i - 1);
    double   KnotDist = Pole1.Distance(Pole2);
    Knot.SetValue(i, Knot.Value(i - 1) + KnotDist);
  }

  const int Degree = 1;

  NCollection_Array1<int> Mult(1, NbPoints);
  Mult.Init(Degree);
  Mult.SetValue(Mult.Lower(), Degree + 1);
  Mult.SetValue(Mult.Upper(), Degree + 1);

  res = new Geom2d_BSplineCurve(Pole, Knot, Mult, Degree);

  double epsGeom        = GetEpsGeom();
  double anUVResolution = GetUVResolution();

  IGESConvGeom::IncreaseCurveContinuity(res,
                                        std::max(Precision::Confusion(), epsGeom * anUVResolution),
                                        GetContinuity());
  return res;
}
