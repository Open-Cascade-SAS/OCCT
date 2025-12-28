// Created on: 1993-06-21
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomToStep_MakeBoundedCurve.hxx>
#include <GeomToStep_MakeConic.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>

//=============================================================================
// Creation d' une Curve de prostep a partir d' une Curve de Geom
//=============================================================================
GeomToStep_MakeCurve::GeomToStep_MakeCurve(const occ::handle<Geom_Curve>& C,
                                           const StepData_Factors&        theLocalFactors)
{
  done = true;
  if (C->IsKind(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Line> L = occ::down_cast<Geom_Line>(C);
    GeomToStep_MakeLine    MkLine(L, theLocalFactors);
    theCurve = MkLine.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Conic)))
  {
    occ::handle<Geom_Conic> L = occ::down_cast<Geom_Conic>(C);
    GeomToStep_MakeConic    MkConic(L, theLocalFactors);
    theCurve = MkConic.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> T = occ::down_cast<Geom_TrimmedCurve>(C);
    occ::handle<Geom_Curve>        B = T->BasisCurve();
    //    TANT PIS, on passe la courbe de base ...
    if (B->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      occ::handle<Geom_BSplineCurve> BS = occ::down_cast<Geom_BSplineCurve>(B->Copy());
      BS->Segment(T->FirstParameter(), T->LastParameter());
      B = BS;
    }
    else if (B->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
    {
      occ::handle<Geom_BezierCurve> BZ = occ::down_cast<Geom_BezierCurve>(B->Copy());
      BZ->Segment(T->FirstParameter(), T->LastParameter());
      B = BZ;
    }
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "GeomToStep_MakeCurve, TrimmedCurve, BasisCurve is transferred not trimmed"
                << std::endl;
      std::cout << "BasisCurve Type : " << B->DynamicType()->Name() << std::endl;
#endif
    }
    GeomToStep_MakeCurve MkBasisC(B, theLocalFactors);
    theCurve = MkBasisC.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_BoundedCurve)))
  {
    occ::handle<Geom_BoundedCurve> L = occ::down_cast<Geom_BoundedCurve>(C);
    GeomToStep_MakeBoundedCurve    MkBoundedC(L, theLocalFactors);
    theCurve = MkBoundedC.Value();
  }
  else
    done = false;
}

//=============================================================================
// Creation d'une Curve de prostep a partir d' une Curve de Geom2d
//=============================================================================

GeomToStep_MakeCurve::GeomToStep_MakeCurve(const occ::handle<Geom2d_Curve>& C,
                                           const StepData_Factors&          theLocalFactors)
{
  done = true;
  if (C->IsKind(STANDARD_TYPE(Geom2d_Line)))
  {
    occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(C);
    GeomToStep_MakeLine      MkLine(L, theLocalFactors);
    theCurve = MkLine.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Conic)))
  {

    // ----------------------------------------------------------------------
    // A Circle of an Ellipse can be indirect. An indirect Axis in not
    // mappable onto STEP. Then to avoid changing the topology, the Circle
    // or the Ellipse are converted into BSpline Curves
    // ----------------------------------------------------------------------

    if (C->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    {
      occ::handle<Geom2d_Circle> theC2d = occ::down_cast<Geom2d_Circle>(C);
      gp_Circ2d                  C2d    = theC2d->Circ2d();
      if (!C2d.IsDirect())
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning : Circle converted to BSpline." << std::endl;
#endif
        occ::handle<Geom2d_BSplineCurve> aBSplineCurve2d =
          Geom2dConvert::CurveToBSplineCurve(theC2d);
        const occ::handle<Geom2d_BoundedCurve>& aBC2d = aBSplineCurve2d; // to avoid ambiguity
        GeomToStep_MakeBoundedCurve             MkBoundedC(aBC2d, theLocalFactors);
        theCurve = MkBoundedC.Value();
      }
      else
      {
        occ::handle<Geom2d_Conic> L = occ::down_cast<Geom2d_Conic>(C);
        GeomToStep_MakeConic      MkConic(L, theLocalFactors);
        theCurve = MkConic.Value();
      }
    }
    else if (C->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
    {
      occ::handle<Geom2d_Ellipse> theE2d = occ::down_cast<Geom2d_Ellipse>(C);
      gp_Elips2d                  E2d    = theE2d->Elips2d();
      if (!E2d.IsDirect())
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning : Ellipse converted to BSpline." << std::endl;
#endif
        occ::handle<Geom2d_BSplineCurve> aBSplineCurve2d =
          Geom2dConvert::CurveToBSplineCurve(theE2d);
        const occ::handle<Geom2d_BoundedCurve>& aBC2d = aBSplineCurve2d; // to avoid ambiguity
        GeomToStep_MakeBoundedCurve             MkBoundedC(aBC2d, theLocalFactors);
        theCurve = MkBoundedC.Value();
      }
      else
      {
        occ::handle<Geom2d_Conic> L = occ::down_cast<Geom2d_Conic>(C);
        GeomToStep_MakeConic      MkConic(L, theLocalFactors);
        theCurve = MkConic.Value();
      }
    }
    else
    {
      occ::handle<Geom2d_Conic> L = occ::down_cast<Geom2d_Conic>(C);
      GeomToStep_MakeConic      MkConic(L, theLocalFactors);
      theCurve = MkConic.Value();
    }
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve)))
  {
    occ::handle<Geom2d_BoundedCurve> L = occ::down_cast<Geom2d_BoundedCurve>(C);
    GeomToStep_MakeBoundedCurve      MkBoundedC(L, theLocalFactors);
    theCurve = MkBoundedC.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    GeomToStep_MakeCurve aMaker(occ::down_cast<Geom2d_TrimmedCurve>(C)->BasisCurve(),
                                theLocalFactors);
    theCurve = aMaker.Value();
  }
  else
    done = false;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_Curve>& GeomToStep_MakeCurve::Value() const
{
  StdFail_NotDone_Raise_if(!done, "GeomToStep_MakeCurve::Value() - no result");
  return theCurve;
}
