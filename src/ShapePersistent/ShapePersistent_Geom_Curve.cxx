// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Standard_NullObject.hxx>

#include <ShapePersistent_Geom_Curve.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>


Handle(Geom_Curve) ShapePersistent_Geom_Curve::pBezier::Import() const
{
  if (myPoles.IsNull())
    return NULL;

  if (myRational)
  {
    if (myWeights.IsNull())
      return NULL;
    return new Geom_BezierCurve (*myPoles->Array(), *myWeights->Array());
  }
  else
    return new Geom_BezierCurve (*myPoles->Array());
}

Handle(Geom_Curve) ShapePersistent_Geom_Curve::pBSpline::Import() const
{
  if (myPoles.IsNull() || myKnots.IsNull() || myMultiplicities.IsNull())
    return NULL;

  if (myRational)
  {
    if (myWeights.IsNull())
      return NULL;

    return new Geom_BSplineCurve (*myPoles->Array(),
                                  *myWeights->Array(),
                                  *myKnots->Array(),
                                  *myMultiplicities->Array(),
                                  mySpineDegree,
                                  myPeriodic);
  }
  else
    return new Geom_BSplineCurve (*myPoles->Array(),
                                  *myKnots->Array(),
                                  *myMultiplicities->Array(),
                                  mySpineDegree,
                                  myPeriodic);
}

Handle(Geom_Curve) ShapePersistent_Geom_Curve::pTrimmed::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_TrimmedCurve (myBasisCurve->Import(), myFirstU, myLastU);
}

Handle(Geom_Curve) ShapePersistent_Geom_Curve::pOffset::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_OffsetCurve
    (myBasisCurve->Import(), myOffsetValue, myOffsetDirection);
}

//=======================================================================
// Line
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom::Curve,
                                                Geom_Line,
                                                gp_Ax1>
  ::PName() const { return "PGeom_Line"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::Curve,
                                    Geom_Line,
                                    gp_Ax1>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_Line) aMyGeom =
    Handle(Geom_Line)::DownCast(myTransient);
  write(theWriteData, aMyGeom->Position());
}

Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_Line)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Line) aPT = new Line;
      aPT->myTransient = theCurve;
      aPC = aPT;
    }
  }
  return aPC;
}

//=======================================================================
// Conic
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::Curve,
                                                  gp_Ax2>
  ::PName() const { return "PGeom_Conic"; }

//=======================================================================
// Circle
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                                Geom_Circle,
                                                gp_Circ>
  ::PName() const { return "PGeom_Circle"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                    Geom_Circle,
                                    gp_Circ>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_Circle) aMyGeom =
    Handle(Geom_Circle)::DownCast(myTransient);
  theWriteData << aMyGeom->Circ();
}

Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_Circle)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Circle) aPT = new Circle;
      aPT->myTransient = theCurve;
      aPC = aPT;
    }
  }
  return aPC;
}

//=======================================================================
// Ellipse
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                                Geom_Ellipse,
                                                gp_Elips>
  ::PName() const { return "PGeom_Ellipse"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                    Geom_Ellipse,
                                    gp_Elips>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_Ellipse) aMyGeom =
    Handle(Geom_Ellipse)::DownCast(myTransient);
  theWriteData << aMyGeom->Elips();
}

Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_Ellipse)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Ellipse) aPT = new Ellipse;
      aPT->myTransient = theCurve;
      aPC = aPT;
    }
  }
  return aPC;
}

//=======================================================================
// Hyperbola
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                                Geom_Hyperbola,
                                                gp_Hypr>
  ::PName() const { return "PGeom_Hyperbola"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                    Geom_Hyperbola,
                                    gp_Hypr>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_Hyperbola) aMyGeom =
    Handle(Geom_Hyperbola)::DownCast(myTransient);
  theWriteData << aMyGeom->Hypr();
}

Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_Hyperbola)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Hyperbola) aPT = new Hyperbola;
      aPT->myTransient = theCurve;
      aPC = aPT;
    }
  }
  return aPC;
}

//=======================================================================
// Parabola
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                                Geom_Parabola,
                                                gp_Parab>
  ::PName() const { return "PGeom_Parabola"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom_Curve::Conic,
                                    Geom_Parabola,
                                    gp_Parab>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom_Parabola) aMyGeom =
    Handle(Geom_Parabola)::DownCast(myTransient);
  theWriteData << aMyGeom->Parab();
}

Handle(ShapePersistent_Geom::Curve) 
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_Parabola)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Parabola) aPT = new Parabola;
      aPT->myTransient = theCurve;
      aPC = aPT;
    }
  }
  return aPC;
}

//=======================================================================
// BezierCurve
//=======================================================================
Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_BezierCurve)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Bezier) aPBC = new Bezier;
      Handle(pBezier) aPpBC = new pBezier;
      aPpBC->myRational = theCurve->IsRational();
      aPpBC->myPoles = StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt>("PColgp_HArray1OfPnt", theCurve->Poles());
      if (theCurve->IsRational()) {
        aPpBC->myWeights = StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(*theCurve->Weights());
      }
      aPBC->myPersistent = aPpBC;
      aPC = aPBC;
    }
  }
  return aPC;
}

//=======================================================================
// BSplineCurve
//=======================================================================
Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_BSplineCurve)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(BSpline) aPBSC = new BSpline;
      Handle(pBSpline) aPpBSC = new pBSpline;
      aPpBSC->myRational = theCurve->IsRational();
      aPpBSC->myPeriodic = theCurve->IsPeriodic();
      aPpBSC->mySpineDegree = theCurve->Degree();
      aPpBSC->myPoles = StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt>("PColgp_HArray1OfPnt", theCurve->Poles());
      if (theCurve->IsRational()) {
        aPpBSC->myWeights = StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(*theCurve->Weights());
      }
      aPpBSC->myKnots = StdLPersistent_HArray1::Translate<TColStd_HArray1OfReal>(theCurve->Knots());
      aPpBSC->myMultiplicities = StdLPersistent_HArray1::Translate<TColStd_HArray1OfInteger>(theCurve->Multiplicities());
      aPBSC->myPersistent = aPpBSC;
      aPC = aPBSC;
    }
  }
  return aPC;
}

//=======================================================================
// TrimmedCurve
//=======================================================================
Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_TrimmedCurve)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Trimmed) aPTC = new Trimmed;
      Handle(pTrimmed) aPpTC = new pTrimmed;
      aPpTC->myFirstU = theCurve->FirstParameter();
      aPpTC->myLastU = theCurve->LastParameter();
      aPpTC->myBasisCurve = ShapePersistent_Geom::Translate(theCurve->BasisCurve(), theMap);
      aPTC->myPersistent = aPpTC;
      aPC = aPTC;
    }
  }
  return aPC;
}

//=======================================================================
// OffsetCurve
//=======================================================================
Handle(ShapePersistent_Geom::Curve)
ShapePersistent_Geom_Curve::Translate(const Handle(Geom_OffsetCurve)& theCurve,
                                      StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Offset) aPOC = new Offset;
      Handle(pOffset) aPpOC = new pOffset;
      aPpOC->myOffsetDirection = theCurve->Direction();
      aPpOC->myOffsetValue = theCurve->Offset();
      aPpOC->myBasisCurve = ShapePersistent_Geom::Translate(theCurve->BasisCurve(), theMap);
      aPOC->myPersistent = aPpOC;
      aPC = aPOC;
    }
  }
  return aPC;
}
