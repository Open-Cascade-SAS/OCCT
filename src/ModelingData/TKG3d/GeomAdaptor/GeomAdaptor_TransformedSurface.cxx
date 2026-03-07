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
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
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
  aCopy->myTransformedSurface                   = myTransformedSurface;

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
  ensureTransformedCache();
  return myTransformedSurface;
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::invalidateTransformedCache()
{
  myTransformedSurface.Nullify();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::ensureTransformedCache() const
{
  if (!myTransformedSurface.IsNull() || mySurf.Surface().IsNull())
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
    myTransformedSurface.Nullify();
    return;
  }

  const bool     isIdentity = myTrsf.Form() == gp_Identity;
  const gp_Trsf& aTrsf      = myTrsf;
  switch (mySurf.GetType())
  {
    case GeomAbs_Plane:
      myTransformedSurface =
        isIdentity ? mySurf.Surface()
                   : occ::handle<Geom_Surface>(new Geom_Plane(mySurf.Plane().Transformed(aTrsf)));
      break;
    case GeomAbs_Cylinder:
      myTransformedSurface = isIdentity ? mySurf.Surface()
                                        : occ::handle<Geom_Surface>(new Geom_CylindricalSurface(
                                            mySurf.Cylinder().Transformed(aTrsf)));
      break;
    case GeomAbs_Cone:
      myTransformedSurface =
        isIdentity
          ? mySurf.Surface()
          : occ::handle<Geom_Surface>(new Geom_ConicalSurface(mySurf.Cone().Transformed(aTrsf)));
      break;
    case GeomAbs_Sphere:
      myTransformedSurface = isIdentity ? mySurf.Surface()
                                        : occ::handle<Geom_Surface>(new Geom_SphericalSurface(
                                            mySurf.Sphere().Transformed(aTrsf)));
      break;
    case GeomAbs_Torus:
      myTransformedSurface =
        isIdentity
          ? mySurf.Surface()
          : occ::handle<Geom_Surface>(new Geom_ToroidalSurface(mySurf.Torus().Transformed(aTrsf)));
      break;
    default:
      myTransformedSurface = isIdentity
                               ? mySurf.Surface()
                               : occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(aTrsf));
      break;
  }
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
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_Plane || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Plane");
  }
  const occ::handle<Geom_Plane> aPlane = occ::down_cast<Geom_Plane>(myTransformedSurface);
  if (aPlane.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Plane");
  }
  return aPlane->Pln();
}

//=================================================================================================

gp_Cylinder GeomAdaptor_TransformedSurface::Cylinder() const
{
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_Cylinder || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Cylinder");
  }
  const occ::handle<Geom_CylindricalSurface> aCylinder =
    occ::down_cast<Geom_CylindricalSurface>(myTransformedSurface);
  if (aCylinder.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Cylinder");
  }
  return aCylinder->Cylinder();
}

//=================================================================================================

gp_Cone GeomAdaptor_TransformedSurface::Cone() const
{
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_Cone || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Cone");
  }
  const occ::handle<Geom_ConicalSurface> aCone =
    occ::down_cast<Geom_ConicalSurface>(myTransformedSurface);
  if (aCone.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Cone");
  }
  return aCone->Cone();
}

//=================================================================================================

gp_Sphere GeomAdaptor_TransformedSurface::Sphere() const
{
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_Sphere || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Sphere");
  }
  const occ::handle<Geom_SphericalSurface> aSphere =
    occ::down_cast<Geom_SphericalSurface>(myTransformedSurface);
  if (aSphere.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Sphere");
  }
  return aSphere->Sphere();
}

//=================================================================================================

gp_Torus GeomAdaptor_TransformedSurface::Torus() const
{
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_Torus || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Torus");
  }
  const occ::handle<Geom_ToroidalSurface> aTorus =
    occ::down_cast<Geom_ToroidalSurface>(myTransformedSurface);
  if (aTorus.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Torus");
  }
  return aTorus->Torus();
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_TransformedSurface::Bezier() const
{
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_BezierSurface || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Bezier");
  }
  const occ::handle<Geom_BezierSurface> aBezier =
    occ::down_cast<Geom_BezierSurface>(myTransformedSurface);
  if (aBezier.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Bezier");
  }
  return aBezier;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_TransformedSurface::BSpline() const
{
  ensureTransformedCache();
  if (mySurf.GetType() != GeomAbs_BSplineSurface || myTransformedSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::BSpline");
  }
  const occ::handle<Geom_BSplineSurface> aBSpline =
    occ::down_cast<Geom_BSplineSurface>(myTransformedSurface);
  if (aBSpline.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::BSpline");
  }
  return aBSpline;
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

GeomAdaptor_Surface GeomAdaptor_TransformedSurface::transformedAdaptor() const
{
  if (myTrsf.Form() == gp_Identity)
  {
    return mySurf;
  }

  ensureTransformedCache();
  return GeomAdaptor_Surface(myTransformedSurface,
                             mySurf.FirstUParameter(),
                             mySurf.LastUParameter(),
                             mySurf.FirstVParameter(),
                             mySurf.LastVParameter(),
                             mySurf.ToleranceU(),
                             mySurf.ToleranceV());
}
