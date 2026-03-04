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

#ifndef GeomBndLib_Plane_HeaderFile
#define GeomBndLib_Plane_HeaderFile

#include <Bnd_Box.hxx>
#include <ElSLib.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a 3D plane (Geom_Plane).
//! Handles infinite parameters by opening box sides based on the plane normal direction.
class GeomBndLib_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Plane(const occ::handle<Geom_Plane>& thePlane)
      : myGeom(thePlane)
  {
  }

  GeomBndLib_Plane(const GeomBndLib_Plane&)            = delete;
  GeomBndLib_Plane& operator=(const GeomBndLib_Plane&) = delete;
  GeomBndLib_Plane(GeomBndLib_Plane&&)                 = delete;
  GeomBndLib_Plane& operator=(GeomBndLib_Plane&&)      = delete;

  const occ::handle<Geom_Plane>& Geometry() const { return myGeom; }

  //! Add bounding box for full plane (infinite).
  void Add(double theTol, Bnd_Box& theBox) const
  {
    double aU1, aU2, aV1, aV2;
    myGeom->Bounds(aU1, aU2, aV1, aV2);
    Add(aU1, aU2, aV1, aV2, theTol, theBox);
  }

  //! Add bounding box for plane patch [theUMin, theUMax] x [theVMin, theVMax].
  void Add(double   theUMin,
           double   theUMax,
           double   theVMin,
           double   theVMax,
           double   theTol,
           Bnd_Box& theBox) const
  {
    const gp_Pln& aPln = myGeom->Pln();

    if (Precision::IsInfinite(theVMin) || Precision::IsInfinite(theVMax)
        || Precision::IsInfinite(theUMin) || Precision::IsInfinite(theUMax))
    {
      TreatInfinitePlane(aPln, theUMin, theUMax, theVMin, theVMax, theTol, theBox);
      return;
    }

    theBox.Add(ElSLib::Value(theUMin, theVMin, aPln));
    theBox.Add(ElSLib::Value(theUMin, theVMax, aPln));
    theBox.Add(ElSLib::Value(theUMax, theVMin, aPln));
    theBox.Add(ElSLib::Value(theUMax, theVMax, aPln));
    theBox.Enlarge(theTol);
  }

  //! For analytical surfaces, AddOptimal is same as Add.
  void AddOptimal(double   theUMin,
                  double   theUMax,
                  double   theVMin,
                  double   theVMax,
                  double   theTol,
                  Bnd_Box& theBox) const
  {
    Add(theUMin, theUMax, theVMin, theVMax, theTol, theBox);
  }

  //! AddOptimal for full surface.
  void AddOptimal(double theTol, Bnd_Box& theBox) const { Add(theTol, theBox); }

private:
  //! Compute barycenter point for infinite plane.
  static gp_Pnt BaryCenter(const gp_Pln& thePlane,
                            double        theUMin,
                            double        theUMax,
                            double        theVMin,
                            double        theVMax)
  {
    double aU, aV;
    const bool isU1Inf = Precision::IsInfinite(theUMin);
    const bool isU2Inf = Precision::IsInfinite(theUMax);
    const bool isV1Inf = Precision::IsInfinite(theVMin);
    const bool isV2Inf = Precision::IsInfinite(theVMax);

    if (isU1Inf && isU2Inf)
      aU = 0;
    else if (isU1Inf)
      aU = theUMax - 10.;
    else if (isU2Inf)
      aU = theUMin + 10.;
    else
      aU = (theUMin + theUMax) / 2.;

    if (isV1Inf && isV2Inf)
      aV = 0;
    else if (isV1Inf)
      aV = theVMax - 10.;
    else if (isV2Inf)
      aV = theVMin + 10.;
    else
      aV = (theVMin + theVMax) / 2.;

    return ElSLib::Value(aU, aV, thePlane);
  }

  //! Handle infinite plane by opening appropriate box sides.
  static void TreatInfinitePlane(const gp_Pln& thePlane,
                                 double        theUMin,
                                 double        theUMax,
                                 double        theVMin,
                                 double        theVMax,
                                 double        theTol,
                                 Bnd_Box&      theBox)
  {
    const gp_Dir&    aNorm        = thePlane.Axis().Direction();
    constexpr double anAngularTol = RealEpsilon();

    gp_Pnt aLocation = BaryCenter(thePlane, theUMin, theUMax, theVMin, theVMax);

    if (aNorm.IsParallel(gp::DX(), anAngularTol))
    {
      theBox.Add(aLocation);
      theBox.OpenYmin();
      theBox.OpenYmax();
      theBox.OpenZmin();
      theBox.OpenZmax();
    }
    else if (aNorm.IsParallel(gp::DY(), anAngularTol))
    {
      theBox.Add(aLocation);
      theBox.OpenXmin();
      theBox.OpenXmax();
      theBox.OpenZmin();
      theBox.OpenZmax();
    }
    else if (aNorm.IsParallel(gp::DZ(), anAngularTol))
    {
      theBox.Add(aLocation);
      theBox.OpenXmin();
      theBox.OpenXmax();
      theBox.OpenYmin();
      theBox.OpenYmax();
    }
    else
    {
      theBox.SetWhole();
      return;
    }

    theBox.Enlarge(theTol);
  }

  occ::handle<Geom_Plane> myGeom;
};

#endif // GeomBndLib_Plane_HeaderFile
