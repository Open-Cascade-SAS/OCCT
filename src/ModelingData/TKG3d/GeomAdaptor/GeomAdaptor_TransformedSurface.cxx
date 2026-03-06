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
  initTransformedCache();
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
  initTransformedCache();
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::ShallowCopy() const
{
  occ::handle<GeomAdaptor_TransformedSurface> aCopy = new GeomAdaptor_TransformedSurface();

  const occ::handle<Adaptor3d_Surface> aSurface = mySurf.ShallowCopy();
  const GeomAdaptor_Surface& aGeomSurface       = *occ::down_cast<GeomAdaptor_Surface>(aSurface);
  aCopy->mySurf                                 = aGeomSurface;
  aCopy->myTrsf                                 = myTrsf;
  aCopy->myTransformedData                      = myTransformedData;

  return aCopy;
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::Load(const occ::handle<Geom_Surface>& theSurface,
                                          const gp_Trsf&                   theTrsf)
{
  mySurf.Load(theSurface);
  myTrsf = theTrsf;
  initTransformedCache();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::Load(const occ::handle<Geom_Surface>& theSurface,
                                          const double                     theUFirst,
                                          const double                     theULast,
                                          const double                     theVFirst,
                                          const double                     theVLast,
                                          const double                     theTolU,
                                          const double                     theTolV,
                                          const gp_Trsf&                   theTrsf)
{
  mySurf.Load(theSurface, theUFirst, theULast, theVFirst, theVLast, theTolU, theTolV);
  myTrsf = theTrsf;
  initTransformedCache();
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::SetTrsf(const gp_Trsf& theTrsf)
{
  myTrsf = theTrsf;
  initTransformedCache();
}

//=================================================================================================

const occ::handle<Geom_Surface>& GeomAdaptor_TransformedSurface::GeomSurfaceTransformed() const
{
  if (!myTransformedData.Surface.IsNull())
  {
    return myTransformedData.Surface;
  }

  switch (mySurf.GetType())
  {
    case GeomAbs_Plane:
      myTransformedData.Surface = new Geom_Plane(std::get<gp_Pln>(myTransformedData.Primitive));
      break;
    case GeomAbs_Cylinder:
      myTransformedData.Surface =
        new Geom_CylindricalSurface(std::get<gp_Cylinder>(myTransformedData.Primitive));
      break;
    case GeomAbs_Cone:
      myTransformedData.Surface =
        new Geom_ConicalSurface(std::get<gp_Cone>(myTransformedData.Primitive));
      break;
    case GeomAbs_Sphere:
      myTransformedData.Surface =
        new Geom_SphericalSurface(std::get<gp_Sphere>(myTransformedData.Primitive));
      break;
    case GeomAbs_Torus:
      myTransformedData.Surface =
        new Geom_ToroidalSurface(std::get<gp_Torus>(myTransformedData.Primitive));
      break;
    default:
      break;
  }

  return myTransformedData.Surface;
}

//=================================================================================================

void GeomAdaptor_TransformedSurface::initTransformedCache()
{
  myTransformedData = TransformedSurfaceData();
  if (mySurf.Surface().IsNull())
  {
    return;
  }

  const bool     isIdentity = myTrsf.Form() == gp_Identity;
  const gp_Trsf& aTrsf      = myTrsf;
  switch (mySurf.GetType())
  {
    case GeomAbs_Plane:
      myTransformedData.Primitive =
        isIdentity ? TransformedSurfaceData::PrimitiveData(mySurf.Plane())
                   : TransformedSurfaceData::PrimitiveData(mySurf.Plane().Transformed(aTrsf));
      break;
    case GeomAbs_Cylinder:
      myTransformedData.Primitive =
        isIdentity ? TransformedSurfaceData::PrimitiveData(mySurf.Cylinder())
                   : TransformedSurfaceData::PrimitiveData(mySurf.Cylinder().Transformed(aTrsf));
      break;
    case GeomAbs_Cone:
      myTransformedData.Primitive =
        isIdentity ? TransformedSurfaceData::PrimitiveData(mySurf.Cone())
                   : TransformedSurfaceData::PrimitiveData(mySurf.Cone().Transformed(aTrsf));
      break;
    case GeomAbs_Sphere:
      myTransformedData.Primitive =
        isIdentity ? TransformedSurfaceData::PrimitiveData(mySurf.Sphere())
                   : TransformedSurfaceData::PrimitiveData(mySurf.Sphere().Transformed(aTrsf));
      break;
    case GeomAbs_Torus:
      myTransformedData.Primitive =
        isIdentity ? TransformedSurfaceData::PrimitiveData(mySurf.Torus())
                   : TransformedSurfaceData::PrimitiveData(mySurf.Torus().Transformed(aTrsf));
      break;
    case GeomAbs_BezierSurface:
      myTransformedData.Surface =
        isIdentity ? mySurf.Surface()
                   : occ::down_cast<Geom_BezierSurface>(mySurf.Bezier()->Transformed(aTrsf));
      break;
    case GeomAbs_BSplineSurface:
      myTransformedData.Surface =
        isIdentity ? mySurf.Surface()
                   : occ::down_cast<Geom_BSplineSurface>(mySurf.BSpline()->Transformed(aTrsf));
      break;
    default:
      myTransformedData.Surface =
        isIdentity ? mySurf.Surface()
                   : occ::down_cast<Geom_Surface>(mySurf.Surface()->Transformed(aTrsf));
      break;
  }

  if (isIdentity)
  {
    if (mySurf.GetType() == GeomAbs_SurfaceOfRevolution)
    {
      myTransformedData.Axis       = mySurf.AxeOfRevolution();
      myTransformedData.BasisCurve = mySurf.BasisCurve();
    }
    else if (mySurf.GetType() == GeomAbs_SurfaceOfExtrusion)
    {
      myTransformedData.Direction  = mySurf.Direction();
      myTransformedData.BasisCurve = mySurf.BasisCurve();
    }
    else if (mySurf.GetType() == GeomAbs_OffsetSurface)
    {
      myTransformedData.BasisSurface = mySurf.BasisSurface();
      myTransformedData.OffsetValue  = mySurf.OffsetValue();
    }
  }
  else if (!myTransformedData.Surface.IsNull())
  {
    GeomAdaptor_Surface aTransformedSurf(myTransformedData.Surface);
    if (aTransformedSurf.GetType() == GeomAbs_SurfaceOfRevolution)
    {
      myTransformedData.Axis       = aTransformedSurf.AxeOfRevolution();
      myTransformedData.BasisCurve = aTransformedSurf.BasisCurve();
    }
    else if (aTransformedSurf.GetType() == GeomAbs_SurfaceOfExtrusion)
    {
      myTransformedData.Direction  = aTransformedSurf.Direction();
      myTransformedData.BasisCurve = aTransformedSurf.BasisCurve();
    }
    else if (aTransformedSurf.GetType() == GeomAbs_OffsetSurface)
    {
      myTransformedData.BasisSurface = aTransformedSurf.BasisSurface();
      myTransformedData.OffsetValue  = aTransformedSurf.OffsetValue();
    }
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
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(GeomSurfaceTransformed());
  return HS->UTrim(theFirst, theLast, theTol);
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::VTrim(const double theFirst,
                                                                     const double theLast,
                                                                     const double theTol) const
{
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(GeomSurfaceTransformed());
  return HS->VTrim(theFirst, theLast, theTol);
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
  return std::get<gp_Pln>(myTransformedData.Primitive);
}

//=================================================================================================

gp_Cylinder GeomAdaptor_TransformedSurface::Cylinder() const
{
  return std::get<gp_Cylinder>(myTransformedData.Primitive);
}

//=================================================================================================

gp_Cone GeomAdaptor_TransformedSurface::Cone() const
{
  return std::get<gp_Cone>(myTransformedData.Primitive);
}

//=================================================================================================

gp_Sphere GeomAdaptor_TransformedSurface::Sphere() const
{
  return std::get<gp_Sphere>(myTransformedData.Primitive);
}

//=================================================================================================

gp_Torus GeomAdaptor_TransformedSurface::Torus() const
{
  return std::get<gp_Torus>(myTransformedData.Primitive);
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_TransformedSurface::Bezier() const
{
  if (myTransformedData.Surface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Bezier");
  }
  return occ::down_cast<Geom_BezierSurface>(myTransformedData.Surface);
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_TransformedSurface::BSpline() const
{
  if (myTransformedData.Surface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::BSpline");
  }
  return occ::down_cast<Geom_BSplineSurface>(myTransformedData.Surface);
}

//=================================================================================================

gp_Ax1 GeomAdaptor_TransformedSurface::AxeOfRevolution() const
{
  if (!myTransformedData.Axis.has_value())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::AxeOfRevolution");
  }
  return myTransformedData.Axis.value();
}

//=================================================================================================

gp_Dir GeomAdaptor_TransformedSurface::Direction() const
{
  if (!myTransformedData.Direction.has_value())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::Direction");
  }
  return myTransformedData.Direction.value();
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_TransformedSurface::BasisCurve() const
{
  if (myTransformedData.BasisCurve.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::BasisCurve");
  }
  return myTransformedData.BasisCurve;
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_TransformedSurface::BasisSurface() const
{
  if (myTransformedData.BasisSurface.IsNull())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::BasisSurface");
  }
  return myTransformedData.BasisSurface;
}

//=================================================================================================

double GeomAdaptor_TransformedSurface::OffsetValue() const
{
  if (!myTransformedData.OffsetValue.has_value())
  {
    throw Standard_NoSuchObject("GeomAdaptor_TransformedSurface::OffsetValue");
  }
  return myTransformedData.OffsetValue.value();
}
