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

#include <ShapePersistent_Geom2d_Curve.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>


Handle(Geom2d_Curve) ShapePersistent_Geom2d_Curve::pBezier::Import() const
{
  if (myPoles.IsNull())
    return NULL;

  if (myRational)
  {
    if (myWeights.IsNull())
      return NULL;
    return new Geom2d_BezierCurve (*myPoles->Array(), *myWeights->Array());
  }
  else
    return new Geom2d_BezierCurve (*myPoles->Array());
}

Handle(Geom2d_Curve) ShapePersistent_Geom2d_Curve::pBSpline::Import() const
{
  if (myPoles.IsNull() || myKnots.IsNull() || myMultiplicities.IsNull())
    return NULL;

  if (myRational)
  {
    if (myWeights.IsNull())
      return NULL;

    return new Geom2d_BSplineCurve (*myPoles->Array(),
                                    *myWeights->Array(),
                                    *myKnots->Array(),
                                    *myMultiplicities->Array(),
                                    mySpineDegree,
                                    myPeriodic);
  }
  else
    return new Geom2d_BSplineCurve (*myPoles->Array(),
                                    *myKnots->Array(),
                                    *myMultiplicities->Array(),
                                    mySpineDegree,
                                    myPeriodic);
}

Handle(Geom2d_Curve) ShapePersistent_Geom2d_Curve::pTrimmed::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom2d_TrimmedCurve (myBasisCurve->Import(), myFirstU, myLastU);
}

Handle(Geom2d_Curve) ShapePersistent_Geom2d_Curve::pOffset::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom2d_OffsetCurve (myBasisCurve->Import(), myOffsetValue);
}

//=======================================================================
// Line
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d::Curve,
                                                        Geom2d_Line,
                                                        gp_Ax2d>
  ::PName() const { return "PGeom2d_Line"; }

template<>
void ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d::Curve,
                                            Geom2d_Line,
                                            gp_Ax2d>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_Line) aMyGeom =
    Handle(Geom2d_Line)::DownCast(myTransient);
  write(theWriteData, aMyGeom->Position());
}

Handle(ShapePersistent_Geom2d::Curve) 
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_Line)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
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
Standard_CString ShapePersistent_Geom2d_Curve::subBase_gp<ShapePersistent_Geom2d::Curve,
                                                          gp_Ax22d>
  ::PName() const { return "PGeom2d_Conic"; }

//=======================================================================
// Circle
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                                        Geom2d_Circle,
                                                        gp_Circ2d>
  ::PName() const { return "PGeom2d_Circle"; }

template<>
void ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                            Geom2d_Circle,
                                            gp_Circ2d>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_Circle) aMyGeom =
    Handle(Geom2d_Circle)::DownCast(myTransient);
  theWriteData << aMyGeom->Circ2d();
}

Handle(ShapePersistent_Geom2d::Curve) 
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_Circle)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
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
Standard_CString ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                                        Geom2d_Ellipse,
                                                        gp_Elips2d>
  ::PName() const { return "PGeom2d_Ellipse"; }

template<>
void ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                            Geom2d_Ellipse,
                                            gp_Elips2d>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_Ellipse) aMyGeom =
    Handle(Geom2d_Ellipse)::DownCast(myTransient);
  theWriteData << aMyGeom->Elips2d();
}

Handle(ShapePersistent_Geom2d::Curve) 
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_Ellipse)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
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
Standard_CString ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                                        Geom2d_Hyperbola,
                                                        gp_Hypr2d>
  ::PName() const { return "PGeom2d_Hyperbola"; }

template<>
void ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                            Geom2d_Hyperbola,
                                            gp_Hypr2d>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_Hyperbola) aMyGeom =
    Handle(Geom2d_Hyperbola)::DownCast(myTransient);
  theWriteData << aMyGeom->Hypr2d();
}

Handle(ShapePersistent_Geom2d::Curve) 
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_Hyperbola)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
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
Standard_CString ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                                        Geom2d_Parabola,
                                                        gp_Parab2d>
  ::PName() const { return "PGeom2d_Hyperbola"; }

template<>
void ShapePersistent_Geom2d_Curve::instance<ShapePersistent_Geom2d_Curve::Conic,
                                            Geom2d_Parabola,
                                            gp_Parab2d>
  ::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_Parabola) aMyGeom =
    Handle(Geom2d_Parabola)::DownCast(myTransient);
  theWriteData << aMyGeom->Parab2d();
}

Handle(ShapePersistent_Geom2d::Curve) 
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_Parabola)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
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
Handle(ShapePersistent_Geom2d::Curve)
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_BezierCurve)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Bezier) aPBC = new Bezier;
      Handle(pBezier) aPpBC = new pBezier;
      aPpBC->myRational = theCurve->IsRational();
      aPpBC->myPoles = StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt2d>("PColgp_HArray1OfPnt2d", theCurve->Poles());
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
Handle(ShapePersistent_Geom2d::Curve)
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_BSplineCurve)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(BSpline) aPBSC = new BSpline;
      Handle(pBSpline) aPpBSC = new pBSpline;
      aPpBSC->myRational = theCurve->IsRational();
      aPpBSC->myPeriodic = theCurve->IsPeriodic();
      aPpBSC->mySpineDegree = theCurve->Degree();
      aPpBSC->myPoles = StdLPersistent_HArray1::Translate<TColgp_HArray1OfPnt2d>("PColgp_HArray1OfPnt2d", theCurve->Poles());
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
Handle(ShapePersistent_Geom2d::Curve)
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_TrimmedCurve)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC = new Trimmed;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Trimmed) aPTC = new Trimmed;
      Handle(pTrimmed) aPpTC = new pTrimmed;
      aPpTC->myFirstU = theCurve->FirstParameter();
      aPpTC->myLastU = theCurve->LastParameter();
      aPpTC->myBasisCurve = ShapePersistent_Geom2d::Translate(theCurve->BasisCurve(), theMap);
      aPTC->myPersistent = aPpTC;
      aPC = aPTC;
    }
  }
  return aPC;
}

//=======================================================================
// OffsetCurve
//=======================================================================
Handle(ShapePersistent_Geom2d::Curve)
ShapePersistent_Geom2d_Curve::Translate(const Handle(Geom2d_OffsetCurve)& theCurve,
                                        StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC = new Offset;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
    else {
      Handle(Offset) aPOC = new Offset;
      Handle(pOffset) aPpOC = new pOffset;
      aPpOC->myOffsetValue = theCurve->Offset();
      aPpOC->myBasisCurve = ShapePersistent_Geom2d::Translate(theCurve->BasisCurve(), theMap);
      aPOC->myPersistent = aPpOC;
      aPC = aPOC;
    }
  }
  return aPC;
}
