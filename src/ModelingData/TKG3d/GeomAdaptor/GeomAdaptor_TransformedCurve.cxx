// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomAdaptor_TransformedCurve.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Standard_NoSuchObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_TransformedCurve, Adaptor3d_Curve)

//==================================================================================================

GeomAdaptor_TransformedCurve::GeomAdaptor_TransformedCurve() = default;

//==================================================================================================

GeomAdaptor_TransformedCurve::GeomAdaptor_TransformedCurve(
  const occ::handle<Geom_Curve>& theCurve,
  const gp_Trsf&                 theTrsf)
    : myCurve(theCurve),
      myTrsf(theTrsf)
{
}

//==================================================================================================

GeomAdaptor_TransformedCurve::GeomAdaptor_TransformedCurve(
  const occ::handle<Geom_Curve>& theCurve,
  const double                   theFirst,
  const double                   theLast,
  const gp_Trsf&                 theTrsf)
    : myCurve(theCurve, theFirst, theLast),
      myTrsf(theTrsf)
{
}

//==================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_TransformedCurve::ShallowCopy() const
{
  occ::handle<GeomAdaptor_TransformedCurve> aCopy = new GeomAdaptor_TransformedCurve();

  const occ::handle<Adaptor3d_Curve> aCurve     = myCurve.ShallowCopy();
  const GeomAdaptor_Curve&           aGeomCurve = *occ::down_cast<GeomAdaptor_Curve>(aCurve);
  aCopy->myCurve                                = aGeomCurve;

  if (!myConSurf.IsNull())
  {
    aCopy->myConSurf = occ::down_cast<Adaptor3d_CurveOnSurface>(myConSurf->ShallowCopy());
  }
  aCopy->myTrsf = myTrsf;

  return aCopy;
}

//==================================================================================================

void GeomAdaptor_TransformedCurve::Intervals(NCollection_Array1<double>& theT,
                                             const GeomAbs_Shape         theS) const
{
  if (myConSurf.IsNull())
    myCurve.Intervals(theT, theS);
  else
    myConSurf->Intervals(theT, theS);
}

//==================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_TransformedCurve::Trim(const double theFirst,
                                                                const double theLast,
                                                                const double theTol) const
{
  occ::handle<GeomAdaptor_TransformedCurve> aCopy = new GeomAdaptor_TransformedCurve();
  if (myConSurf.IsNull())
  {
    aCopy->myCurve.Load(myCurve.Curve(), theFirst, theLast);
  }
  else
  {
    aCopy->myConSurf =
      occ::down_cast<Adaptor3d_CurveOnSurface>(myConSurf->Trim(theFirst, theLast, theTol));
  }
  aCopy->myTrsf = myTrsf;
  return aCopy;
}

//==================================================================================================

gp_Pnt GeomAdaptor_TransformedCurve::Value(const double theU) const
{
  gp_Pnt aP;
  if (myConSurf.IsNull())
    aP = myCurve.Value(theU);
  else
    aP = myConSurf->Value(theU);
  aP.Transform(myTrsf);
  return aP;
}

//==================================================================================================

void GeomAdaptor_TransformedCurve::D0(const double theU, gp_Pnt& theP) const
{
  if (myConSurf.IsNull())
    myCurve.D0(theU, theP);
  else
    myConSurf->D0(theU, theP);
  theP.Transform(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedCurve::D1(const double theU, gp_Pnt& theP, gp_Vec& theV) const
{
  if (myConSurf.IsNull())
    myCurve.D1(theU, theP, theV);
  else
    myConSurf->D1(theU, theP, theV);
  theP.Transform(myTrsf);
  theV.Transform(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedCurve::D2(const double theU,
                                      gp_Pnt&      theP,
                                      gp_Vec&      theV1,
                                      gp_Vec&      theV2) const
{
  if (myConSurf.IsNull())
    myCurve.D2(theU, theP, theV1, theV2);
  else
    myConSurf->D2(theU, theP, theV1, theV2);
  theP.Transform(myTrsf);
  theV1.Transform(myTrsf);
  theV2.Transform(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedCurve::D3(const double theU,
                                      gp_Pnt&      theP,
                                      gp_Vec&      theV1,
                                      gp_Vec&      theV2,
                                      gp_Vec&      theV3) const
{
  if (myConSurf.IsNull())
    myCurve.D3(theU, theP, theV1, theV2, theV3);
  else
    myConSurf->D3(theU, theP, theV1, theV2, theV3);
  theP.Transform(myTrsf);
  theV1.Transform(myTrsf);
  theV2.Transform(myTrsf);
  theV3.Transform(myTrsf);
}

//==================================================================================================

gp_Vec GeomAdaptor_TransformedCurve::DN(const double theU, const int theN) const
{
  gp_Vec aV;
  if (myConSurf.IsNull())
    aV = myCurve.DN(theU, theN);
  else
    aV = myConSurf->DN(theU, theN);
  aV.Transform(myTrsf);
  return aV;
}

//==================================================================================================

gp_Lin GeomAdaptor_TransformedCurve::Line() const
{
  gp_Lin aL;
  if (myConSurf.IsNull())
    aL = myCurve.Line();
  else
    aL = myConSurf->Line();
  aL.Transform(myTrsf);
  return aL;
}

//==================================================================================================

gp_Circ GeomAdaptor_TransformedCurve::Circle() const
{
  gp_Circ aC;
  if (myConSurf.IsNull())
    aC = myCurve.Circle();
  else
    aC = myConSurf->Circle();
  aC.Transform(myTrsf);
  return aC;
}

//==================================================================================================

gp_Elips GeomAdaptor_TransformedCurve::Ellipse() const
{
  gp_Elips aE;
  if (myConSurf.IsNull())
    aE = myCurve.Ellipse();
  else
    aE = myConSurf->Ellipse();
  aE.Transform(myTrsf);
  return aE;
}

//==================================================================================================

gp_Hypr GeomAdaptor_TransformedCurve::Hyperbola() const
{
  gp_Hypr aH;
  if (myConSurf.IsNull())
    aH = myCurve.Hyperbola();
  else
    aH = myConSurf->Hyperbola();
  aH.Transform(myTrsf);
  return aH;
}

//==================================================================================================

gp_Parab GeomAdaptor_TransformedCurve::Parabola() const
{
  gp_Parab aP;
  if (myConSurf.IsNull())
    aP = myCurve.Parabola();
  else
    aP = myConSurf->Parabola();
  aP.Transform(myTrsf);
  return aP;
}

//==================================================================================================

occ::handle<Geom_BezierCurve> GeomAdaptor_TransformedCurve::Bezier() const
{
  occ::handle<Geom_BezierCurve> aBC;
  if (myConSurf.IsNull())
    aBC = myCurve.Bezier();
  else
    aBC = myConSurf->Bezier();
  return myTrsf.Form() == gp_Identity ? aBC
                                      : occ::down_cast<Geom_BezierCurve>(aBC->Transformed(myTrsf));
}

//==================================================================================================

occ::handle<Geom_BSplineCurve> GeomAdaptor_TransformedCurve::BSpline() const
{
  occ::handle<Geom_BSplineCurve> aBS;
  if (myConSurf.IsNull())
    aBS = myCurve.BSpline();
  else
    aBS = myConSurf->BSpline();
  return myTrsf.Form() == gp_Identity ? aBS
                                      : occ::down_cast<Geom_BSplineCurve>(aBS->Transformed(myTrsf));
}

//==================================================================================================

occ::handle<Geom_OffsetCurve> GeomAdaptor_TransformedCurve::OffsetCurve() const
{
  if (!Is3DCurve() || myCurve.GetType() != GeomAbs_OffsetCurve)
    throw Standard_NoSuchObject("GeomAdaptor_TransformedCurve::OffsetCurve");

  occ::handle<Geom_OffsetCurve> anOffC = myCurve.OffsetCurve();
  return myTrsf.Form() == gp_Identity
           ? anOffC
           : occ::down_cast<Geom_OffsetCurve>(anOffC->Transformed(myTrsf));
}
