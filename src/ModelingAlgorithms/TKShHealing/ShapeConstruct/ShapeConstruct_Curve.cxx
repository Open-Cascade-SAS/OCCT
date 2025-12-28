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

#include <Approx_Curve2d.hxx>
#include <Approx_Curve3d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

// sln 29.12.2001 OCC90 : Method FixKnots was added
//=================================================================================================

bool ShapeConstruct_Curve::AdjustCurve(const occ::handle<Geom_Curve>& C3D,
                                                   const gp_Pnt&             P1,
                                                   const gp_Pnt&             P2,
                                                   const bool    take1,
                                                   const bool    take2) const
{
  if (!take1 && !take2)
    return true;

  if (C3D->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> BSPL = occ::down_cast<Geom_BSplineCurve>(C3D);
    if (take1)
      BSPL->SetPole(1, P1);
    if (take2)
      BSPL->SetPole(BSPL->NbPoles(), P2);
    return true;
  }

  if (C3D->IsKind(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Line> L3D = occ::down_cast<Geom_Line>(C3D);
    //   ATTENTION, P1 et P2 sont supposes tous deux pertinents ...
    gp_Vec        avec(P1, P2);
    gp_Dir        adir(avec);
    gp_Lin        alin(P1, adir);
    double theParam = ElCLib::Parameter(alin, L3D->Lin().Location());
    alin.SetLocation(ElCLib::Value(theParam, alin));
    L3D->SetLin(alin);
    return true;
  }

  return false;
}

//=================================================================================================

bool ShapeConstruct_Curve::AdjustCurveSegment(const occ::handle<Geom_Curve>& C3D,
                                                          const gp_Pnt&             P1,
                                                          const gp_Pnt&             P2,
                                                          const double       U1,
                                                          const double       U2) const
{
  if (C3D->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> BSPL = occ::down_cast<Geom_BSplineCurve>(C3D);
    //    Forcer l extremite c est bien
    //    Propager sur le reste, c est pas mal non plus
    if (U1 >= U2)
      return false;
    double UU1 = std::max(U1, BSPL->FirstParameter());
    double UU2 = std::min(U2, BSPL->LastParameter());
    BSPL->Segment(UU1, UU2);
    BSPL->SetPole(1, P1);
    BSPL->SetPole(BSPL->NbPoles(), P2);
    return true;
  }

  if (C3D->IsKind(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Line> L3D = occ::down_cast<Geom_Line>(C3D);
    //   ATTENTION, P1 et P2 sont supposes tous deux pertinents ...
    //   NB : on ne s aide pas de U1 et U2
    gp_Vec        avec(P1, P2);
    gp_Dir        adir(avec);
    gp_Lin        alin(P1, adir);
    double theParam = ElCLib::Parameter(alin, L3D->Lin().Location());
    alin.SetLocation(ElCLib::Value(theParam, alin));
    L3D->SetLin(alin);
    return true;
  }

  return false;
}

//=================================================================================================

bool ShapeConstruct_Curve::AdjustCurve2d(const occ::handle<Geom2d_Curve>& C2D,
                                                     const gp_Pnt2d&             P1,
                                                     const gp_Pnt2d&             P2,
                                                     const bool      take1,
                                                     const bool      take2) const
{
  if (!take1 && !take2)
    return true;

  if (C2D->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> BSPL = occ::down_cast<Geom2d_BSplineCurve>(C2D);
    if (take1)
      BSPL->SetPole(1, P1);
    if (take2)
      BSPL->SetPole(BSPL->NbPoles(), P2);
    return true;
  }

  if (C2D->IsKind(STANDARD_TYPE(Geom2d_Line)))
  {
    occ::handle<Geom2d_Line> L2D = occ::down_cast<Geom2d_Line>(C2D);
    //   ATTENTION, P1 et P2 sont supposes tous deux pertinents ...
    gp_Vec2d      avec(P1, P2);
    gp_Dir2d      adir(avec);
    gp_Lin2d      alin(P1, adir);
    double theParam = ElCLib::Parameter(alin, L2D->Lin2d().Location());
    alin.SetLocation(ElCLib::Value(theParam, alin));
    L2D->SetLin2d(alin);
    return true;
  }

  return false;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> ShapeConstruct_Curve::ConvertToBSpline(const occ::handle<Geom_Curve>& C,
                                                                 const double       first,
                                                                 const double       last,
                                                                 const double prec) const
{
  occ::handle<Geom_BSplineCurve> bspl;

  if (C->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    bspl = occ::down_cast<Geom_BSplineCurve>(C);
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_BezierCurve)) || C->IsKind(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Curve> tc = new Geom_TrimmedCurve(C, first, last);
    try
    {
      OCC_CATCH_SIGNALS
      bspl = GeomConvert::CurveToBSplineCurve(tc);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeConstruct_Curve::ConvertToBSpline(): Exception in GeomConvert: ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
    }
  }

  if (!bspl.IsNull())
  {
    // take segment if trim and range differ
    double    fbsp = bspl->FirstParameter(), lbsp = bspl->LastParameter();
    bool segment = false;
    if (first > fbsp + Precision::PConfusion())
    {
      fbsp    = first;
      segment = true;
    }
    if (last < lbsp - Precision::PConfusion())
    {
      lbsp    = last;
      segment = true;
    }
    if (!segment)
      return bspl;
    try
    {
      OCC_CATCH_SIGNALS
      bspl = occ::down_cast<Geom_BSplineCurve>(bspl->Copy());
      bspl->Segment(fbsp, lbsp);
      return bspl;
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeConstruct_Curve::ConvertToBSpline(): Exception in Segment: ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
    }
  }

  // Approx is used for conics and when ordinary methods fail
  occ::handle<Geom_Curve> newc = C;
  if (!bspl.IsNull())
  {
    newc = bspl;
    bspl.Nullify();
  }
  try
  {
    OCC_CATCH_SIGNALS
    Approx_Curve3d Conv(new GeomAdaptor_Curve(newc, first, last), prec, GeomAbs_C1, 9, 1000);
    if (Conv.IsDone() || Conv.HasResult())
      bspl = Conv.Curve();
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_Curve::ConvertToBSpline(): Exception in Approx_Curve3d: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return bspl;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> ShapeConstruct_Curve::ConvertToBSpline(const occ::handle<Geom2d_Curve>& C,
                                                                   const double first,
                                                                   const double last,
                                                                   const double prec) const
{
  occ::handle<Geom2d_BSplineCurve> bspl;

  if (C->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    bspl = occ::down_cast<Geom2d_BSplineCurve>(C);
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) || C->IsKind(STANDARD_TYPE(Geom2d_Line)))
  {
    occ::handle<Geom2d_Curve> tc = new Geom2d_TrimmedCurve(C, first, last);
    try
    {
      OCC_CATCH_SIGNALS
      bspl = Geom2dConvert::CurveToBSplineCurve(tc);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout
        << "Warning: ShapeConstruct_Curve::ConvertToBSpline(): Exception in Geom2dConvert: ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
    }
  }

  if (!bspl.IsNull())
  {
    // take segment if trim and range differ
    double    fbsp = bspl->FirstParameter(), lbsp = bspl->LastParameter();
    bool segment = false;
    if (first > fbsp + Precision::PConfusion())
    {
      fbsp    = first;
      segment = true;
    }
    if (last < lbsp - Precision::PConfusion())
    {
      lbsp    = last;
      segment = true;
    }
    if (!segment)
      return bspl;
    try
    {
      OCC_CATCH_SIGNALS
      bspl = occ::down_cast<Geom2d_BSplineCurve>(bspl->Copy());
      bspl->Segment(fbsp, lbsp);
      return bspl;
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeConstruct_Curve::ConvertToBSpline(): Exception in Segment: ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
    }
  }

  // Approx is used for conics and when ordinary methods fail
  occ::handle<Geom2d_Curve> newc = C;
  if (!bspl.IsNull())
  {
    newc = bspl;
    bspl.Nullify();
  }
  try
  {
    OCC_CATCH_SIGNALS
    Approx_Curve2d Conv(new Geom2dAdaptor_Curve(newc, first, last),
                        first,
                        last,
                        prec,
                        prec,
                        GeomAbs_C1,
                        9,
                        1000);
    if (Conv.IsDone() || Conv.HasResult())
      bspl = Conv.Curve();
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_Curve::ConvertToBSpline(): Exception in Approx_Curve3d: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return bspl;
}

//=================================================================================================

bool ShapeConstruct_Curve::FixKnots(occ::handle<NCollection_HArray1<double>>& knots)
{
  bool Fixed   = false;
  int nbKnots = knots->Length();
  double    knotVal = knots->Value(1);
  for (int i = 2; i <= nbKnots; i++)
  {
    double knotNext = knots->Value(i);
    if (knotNext - knotVal <= Epsilon(knotVal))
    {
      knotNext = knotVal + 2. * Epsilon(knotVal);
      knots->SetValue(i, knotNext);
      Fixed = true;
    }
    knotVal = knotNext;
  }
  return Fixed;
}

//=================================================================================================

bool ShapeConstruct_Curve::FixKnots(NCollection_Array1<double>& knots)
{
  bool Fixed   = false;
  int nbKnots = knots.Length();
  double    knotVal = knots.Value(1);
  for (int i = 2; i <= nbKnots; i++)
  {
    double knotNext = knots.Value(i);
    if (knotNext - knotVal <= Epsilon(knotVal))
    {
      knotNext = knotVal + 2. * Epsilon(knotVal);
      knots.SetValue(i, knotNext);
      Fixed = true;
    }
    knotVal = knotNext;
  }
  return Fixed;
}
