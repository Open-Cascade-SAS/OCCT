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

#include <GeomAdaptor_TransformedSurface.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_TransformedSurface, Adaptor3d_Surface)

//==================================================================================================

GeomAdaptor_TransformedSurface::GeomAdaptor_TransformedSurface() {}

//==================================================================================================

GeomAdaptor_TransformedSurface::GeomAdaptor_TransformedSurface(
  const occ::handle<Geom_Surface>& theSurface,
  const gp_Trsf&              theTrsf)
    : mySurf(theSurface),
      myTrsf(theTrsf)
{
}

//==================================================================================================

GeomAdaptor_TransformedSurface::GeomAdaptor_TransformedSurface(
  const occ::handle<Geom_Surface>& theSurface,
  const double         theUFirst,
  const double         theULast,
  const double         theVFirst,
  const double         theVLast,
  const gp_Trsf&              theTrsf,
  const double         theTolU,
  const double         theTolV)
    : mySurf(theSurface, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV),
      myTrsf(theTrsf)
{
}

//==================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::ShallowCopy() const
{
  occ::handle<GeomAdaptor_TransformedSurface> aCopy = new GeomAdaptor_TransformedSurface();

  const occ::handle<Adaptor3d_Surface> aSurface     = mySurf.ShallowCopy();
  const GeomAdaptor_Surface&      aGeomSurface = *occ::down_cast<GeomAdaptor_Surface>(aSurface);
  aCopy->mySurf                                = aGeomSurface;
  aCopy->myTrsf                                = myTrsf;

  return aCopy;
}

//==================================================================================================

void GeomAdaptor_TransformedSurface::UIntervals(NCollection_Array1<double>& theT,
                                                const GeomAbs_Shape   theS) const
{
  mySurf.UIntervals(theT, theS);
}

//==================================================================================================

void GeomAdaptor_TransformedSurface::VIntervals(NCollection_Array1<double>& theT,
                                                const GeomAbs_Shape   theS) const
{
  mySurf.VIntervals(theT, theS);
}

//==================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::UTrim(const double theFirst,
                                                                const double theLast,
                                                                const double theTol) const
{
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(myTrsf)));
  return HS->UTrim(theFirst, theLast, theTol);
}

//==================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::VTrim(const double theFirst,
                                                                const double theLast,
                                                                const double theTol) const
{
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(myTrsf)));
  return HS->VTrim(theFirst, theLast, theTol);
}

//==================================================================================================

gp_Pnt GeomAdaptor_TransformedSurface::Value(const double theU,
                                             const double theV) const
{
  return mySurf.Value(theU, theV).Transformed(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedSurface::D0(const double theU,
                                        const double theV,
                                        gp_Pnt&             theP) const
{
  mySurf.D0(theU, theV, theP);
  theP.Transform(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedSurface::D1(const double theU,
                                        const double theV,
                                        gp_Pnt&             theP,
                                        gp_Vec&             theD1U,
                                        gp_Vec&             theD1V) const
{
  mySurf.D1(theU, theV, theP, theD1U, theD1V);
  theP.Transform(myTrsf);
  theD1U.Transform(myTrsf);
  theD1V.Transform(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedSurface::D2(const double theU,
                                        const double theV,
                                        gp_Pnt&             theP,
                                        gp_Vec&             theD1U,
                                        gp_Vec&             theD1V,
                                        gp_Vec&             theD2U,
                                        gp_Vec&             theD2V,
                                        gp_Vec&             theD2UV) const
{
  mySurf.D2(theU, theV, theP, theD1U, theD1V, theD2U, theD2V, theD2UV);
  theP.Transform(myTrsf);
  theD1U.Transform(myTrsf);
  theD1V.Transform(myTrsf);
  theD2U.Transform(myTrsf);
  theD2V.Transform(myTrsf);
  theD2UV.Transform(myTrsf);
}

//==================================================================================================

void GeomAdaptor_TransformedSurface::D3(const double theU,
                                        const double theV,
                                        gp_Pnt&             theP,
                                        gp_Vec&             theD1U,
                                        gp_Vec&             theD1V,
                                        gp_Vec&             theD2U,
                                        gp_Vec&             theD2V,
                                        gp_Vec&             theD2UV,
                                        gp_Vec&             theD3U,
                                        gp_Vec&             theD3V,
                                        gp_Vec&             theD3UUV,
                                        gp_Vec&             theD3UVV) const
{
  mySurf.D3(theU,
            theV,
            theP,
            theD1U,
            theD1V,
            theD2U,
            theD2V,
            theD2UV,
            theD3U,
            theD3V,
            theD3UUV,
            theD3UVV);
  theP.Transform(myTrsf);
  theD1U.Transform(myTrsf);
  theD1V.Transform(myTrsf);
  theD2U.Transform(myTrsf);
  theD2V.Transform(myTrsf);
  theD2UV.Transform(myTrsf);
  theD3U.Transform(myTrsf);
  theD3V.Transform(myTrsf);
  theD3UUV.Transform(myTrsf);
  theD3UVV.Transform(myTrsf);
}

//==================================================================================================

gp_Vec GeomAdaptor_TransformedSurface::DN(const double    theU,
                                          const double    theV,
                                          const int theNu,
                                          const int theNv) const
{
  return mySurf.DN(theU, theV, theNu, theNv).Transformed(myTrsf);
}

//==================================================================================================

gp_Pln GeomAdaptor_TransformedSurface::Plane() const
{
  return mySurf.Plane().Transformed(myTrsf);
}

//==================================================================================================

gp_Cylinder GeomAdaptor_TransformedSurface::Cylinder() const
{
  return mySurf.Cylinder().Transformed(myTrsf);
}

//==================================================================================================

gp_Cone GeomAdaptor_TransformedSurface::Cone() const
{
  return mySurf.Cone().Transformed(myTrsf);
}

//==================================================================================================

gp_Sphere GeomAdaptor_TransformedSurface::Sphere() const
{
  return mySurf.Sphere().Transformed(myTrsf);
}

//==================================================================================================

gp_Torus GeomAdaptor_TransformedSurface::Torus() const
{
  return mySurf.Torus().Transformed(myTrsf);
}

//==================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_TransformedSurface::Bezier() const
{
  return occ::down_cast<Geom_BezierSurface>(mySurf.Bezier()->Transformed(myTrsf));
}

//==================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_TransformedSurface::BSpline() const
{
  return occ::down_cast<Geom_BSplineSurface>(mySurf.BSpline()->Transformed(myTrsf));
}

//==================================================================================================

gp_Ax1 GeomAdaptor_TransformedSurface::AxeOfRevolution() const
{
  return mySurf.AxeOfRevolution().Transformed(myTrsf);
}

//==================================================================================================

gp_Dir GeomAdaptor_TransformedSurface::Direction() const
{
  return mySurf.Direction().Transformed(myTrsf);
}

//==================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_TransformedSurface::BasisCurve() const
{
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(myTrsf)));
  return HS->BasisCurve();
}

//==================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::BasisSurface() const
{
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(myTrsf)));
  return HS->BasisSurface();
}

//==================================================================================================

double GeomAdaptor_TransformedSurface::OffsetValue() const
{
  return mySurf.OffsetValue();
}
