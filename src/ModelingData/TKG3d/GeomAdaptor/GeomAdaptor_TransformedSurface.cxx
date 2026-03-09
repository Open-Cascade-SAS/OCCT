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
#include <Geom_OffsetSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <Standard_NoSuchObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_TransformedSurface, Adaptor3d_Surface)

//=================================================================================================

GeomAdaptor_TransformedSurface::GeomAdaptor_TransformedSurface() = default;

//=================================================================================================

GeomAdaptor_TransformedSurface::GeomAdaptor_TransformedSurface(
  const occ::handle<Geom_Surface>& theSurface,
  const gp_Trsf&                   theTrsf)
    : mySurf(theSurface),
      myTrsf(theTrsf)
{
  invalidateTransformedCache();
}

//=================================================================================================

GeomAdaptor_TransformedSurface::GeomAdaptor_TransformedSurface(
  const occ::handle<Geom_Surface>& theSurface,
  const double                     theUFirst,
  const double                     theULast,
  const double                     theVFirst,
  const double                     theVLast,
  const gp_Trsf&                   theTrsf,
  const double                     theTolU,
  const double                     theTolV)
    : mySurf(theSurface, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV),
      myTrsf(theTrsf)
{
  invalidateTransformedCache();
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::ShallowCopy() const
{
  occ::handle<GeomAdaptor_TransformedSurface> aCopy = new GeomAdaptor_TransformedSurface();

  const occ::handle<Adaptor3d_Surface> aSurface = mySurf.ShallowCopy();
  const GeomAdaptor_Surface& aGeomSurface       = *occ::down_cast<GeomAdaptor_Surface>(aSurface);
  aCopy->mySurf                                 = aGeomSurface;
  aCopy->myTrsf                                 = myTrsf;
  aCopy->myTransformedAdaptor                   = myTransformedAdaptor;

  return aCopy;
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::Load(const occ::handle<Geom_Surface>& theSurface,
                                          const gp_Trsf&                   theTrsf)
{
  mySurf.Load(theSurface);
  myTrsf = theTrsf;
  invalidateTransformedCache();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::Load(const occ::handle<Geom_Surface>& theSurface,
                                          const double                     theUFirst,
                                          const double                     theULast,
                                          const double                     theVFirst,
                                          const double                     theVLast,
                                          const gp_Trsf&                   theTrsf,
                                          const double                     theTolU,
                                          const double                     theTolV)
{
  mySurf.Load(theSurface, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV);
  myTrsf = theTrsf;
  invalidateTransformedCache();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::SetTrsf(const gp_Trsf& theTrsf)
{
  myTrsf = theTrsf;
  invalidateTransformedCache();
}

//=================================================================================================

const occ::handle<Geom_Surface>& GeomAdaptor_TransformedSurface::GeomSurfaceTransformed() const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf.Surface();
  }

  ensureTransformedCache();
  return myTransformedAdaptor->Surface();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::invalidateTransformedCache()
{
  myTransformedAdaptor.reset();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::ensureTransformedCache() const
{
  if (myTrsf.Form() == gp_Identity || myTransformedAdaptor.has_value())
  {
    return;
  }
  initTransformedCache();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::initTransformedCache() const
{
  if (mySurf.Surface().IsNull())
  {
    myTransformedAdaptor.emplace();
    return;
  }

  const bool                isIdentity = myTrsf.Form() == gp_Identity;
  const gp_Trsf&            aTrsf      = myTrsf;
  occ::handle<Geom_Surface> aSurface;
  switch (mySurf.GetType())
  {
    case GeomAbs_Plane:
      aSurface = isIdentity
                   ? mySurf.Surface()
                   : occ::handle<Geom_Surface>(new Geom_Plane(mySurf.Plane().Transformed(aTrsf)));
      break;
    case GeomAbs_Cylinder:
      aSurface = isIdentity ? mySurf.Surface()
                            : occ::handle<Geom_Surface>(
                                new Geom_CylindricalSurface(mySurf.Cylinder().Transformed(aTrsf)));
      break;
    case GeomAbs_Cone:
      aSurface =
        isIdentity
          ? mySurf.Surface()
          : occ::handle<Geom_Surface>(new Geom_ConicalSurface(mySurf.Cone().Transformed(aTrsf)));
      break;
    case GeomAbs_Sphere:
      aSurface = isIdentity ? mySurf.Surface()
                            : occ::handle<Geom_Surface>(
                                new Geom_SphericalSurface(mySurf.Sphere().Transformed(aTrsf)));
      break;
    case GeomAbs_Torus:
      aSurface =
        isIdentity
          ? mySurf.Surface()
          : occ::handle<Geom_Surface>(new Geom_ToroidalSurface(mySurf.Torus().Transformed(aTrsf)));
      break;
    default:
      aSurface = isIdentity ? mySurf.Surface()
                            : occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(aTrsf));
      break;
  }

  myTransformedAdaptor.emplace(aSurface,
                               mySurf.FirstUParameter(),
                               mySurf.LastUParameter(),
                               mySurf.FirstVParameter(),
                               mySurf.LastVParameter(),
                               mySurf.ToleranceU(),
                               mySurf.ToleranceV());
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::UIntervals(NCollection_Array1<double>& theT,
                                                const GeomAbs_Shape         theS) const
{
  mySurf.UIntervals(theT, theS);
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::VIntervals(NCollection_Array1<double>& theT,
                                                const GeomAbs_Shape         theS) const
{
  mySurf.VIntervals(theT, theS);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::UTrim(const double theFirst,
                                                                     const double theLast,
                                                                     const double theTol) const
{
  return transformedAdaptor().UTrim(theFirst, theLast, theTol);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::VTrim(const double theFirst,
                                                                     const double theLast,
                                                                     const double theTol) const
{
  return transformedAdaptor().VTrim(theFirst, theLast, theTol);
}

//=================================================================================================

gp_Pnt GeomAdaptor_TransformedSurface::EvalD0(double theU, double theV) const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf.EvalD0(theU, theV);
  }
  return mySurf.EvalD0(theU, theV).Transformed(myTrsf);
}

//=================================================================================================

Geom_Surface::ResD1 GeomAdaptor_TransformedSurface::EvalD1(double theU, double theV) const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf.EvalD1(theU, theV);
  }
  Geom_Surface::ResD1 aRes = mySurf.EvalD1(theU, theV);
  aRes.Point.Transform(myTrsf);
  aRes.D1U.Transform(myTrsf);
  aRes.D1V.Transform(myTrsf);
  return aRes;
}

//=================================================================================================

Geom_Surface::ResD2 GeomAdaptor_TransformedSurface::EvalD2(double theU, double theV) const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf.EvalD2(theU, theV);
  }
  Geom_Surface::ResD2 aRes = mySurf.EvalD2(theU, theV);
  aRes.Point.Transform(myTrsf);
  aRes.D1U.Transform(myTrsf);
  aRes.D1V.Transform(myTrsf);
  aRes.D2U.Transform(myTrsf);
  aRes.D2V.Transform(myTrsf);
  aRes.D2UV.Transform(myTrsf);
  return aRes;
}

//=================================================================================================

Geom_Surface::ResD3 GeomAdaptor_TransformedSurface::EvalD3(double theU, double theV) const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf.EvalD3(theU, theV);
  }
  Geom_Surface::ResD3 aRes = mySurf.EvalD3(theU, theV);
  aRes.Point.Transform(myTrsf);
  aRes.D1U.Transform(myTrsf);
  aRes.D1V.Transform(myTrsf);
  aRes.D2U.Transform(myTrsf);
  aRes.D2V.Transform(myTrsf);
  aRes.D2UV.Transform(myTrsf);
  aRes.D3U.Transform(myTrsf);
  aRes.D3V.Transform(myTrsf);
  aRes.D3UUV.Transform(myTrsf);
  aRes.D3UVV.Transform(myTrsf);
  return aRes;
}

//=================================================================================================

gp_Vec GeomAdaptor_TransformedSurface::EvalDN(double theU, double theV, int theNu, int theNv) const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf.EvalDN(theU, theV, theNu, theNv);
  }
  return mySurf.EvalDN(theU, theV, theNu, theNv).Transformed(myTrsf);
}

//=================================================================================================

gp_Pln GeomAdaptor_TransformedSurface::Plane() const
{
  return transformedAdaptor().Plane();
}

//=================================================================================================

gp_Cylinder GeomAdaptor_TransformedSurface::Cylinder() const
{
  return transformedAdaptor().Cylinder();
}

//=================================================================================================

gp_Cone GeomAdaptor_TransformedSurface::Cone() const
{
  return transformedAdaptor().Cone();
}

//=================================================================================================

gp_Sphere GeomAdaptor_TransformedSurface::Sphere() const
{
  return transformedAdaptor().Sphere();
}

//=================================================================================================

gp_Torus GeomAdaptor_TransformedSurface::Torus() const
{
  return transformedAdaptor().Torus();
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_TransformedSurface::Bezier() const
{
  return transformedAdaptor().Bezier();
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_TransformedSurface::BSpline() const
{
  return transformedAdaptor().BSpline();
}

//=================================================================================================

gp_Ax1 GeomAdaptor_TransformedSurface::AxeOfRevolution() const
{
  return transformedAdaptor().AxeOfRevolution();
}

//=================================================================================================

gp_Dir GeomAdaptor_TransformedSurface::Direction() const
{
  return transformedAdaptor().Direction();
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_TransformedSurface::BasisCurve() const
{
  return transformedAdaptor().BasisCurve();
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::BasisSurface() const
{
  return transformedAdaptor().BasisSurface();
}

//=================================================================================================

double GeomAdaptor_TransformedSurface::OffsetValue() const
{
  return transformedAdaptor().OffsetValue();
}

//=================================================================================================

const GeomAdaptor_Surface& GeomAdaptor_TransformedSurface::AdaptorSurfaceTransformed() const
{
  return transformedAdaptor();
}

//=================================================================================================

const GeomAdaptor_Surface& GeomAdaptor_TransformedSurface::transformedAdaptor() const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf;
  }

  ensureTransformedCache();
  return *myTransformedAdaptor;
}
