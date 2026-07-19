// Created: 1998-06-03
//
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS,
// furnished in accordance with the terms and conditions of the contract
// and with the inclusion of this copyright notice.
// This file or any part thereof may not be provided or otherwise
// made available to any third party.
//
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef _GeomConvert_SurfToAnaSurf_HeaderFile
#define _GeomConvert_SurfToAnaSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <GeomConvert_ConvType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class Geom_Surface;
class Geom_SurfaceOfRevolution;
class Geom_Circle;

//! Converts a surface to the analytical form with given
//! precision. Conversion is done only the surface is bspline
//! of bezier and this can be approximated by some analytical
//! surface with that precision.
class GeomConvert_SurfToAnaSurf
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomConvert_SurfToAnaSurf();

  Standard_EXPORT GeomConvert_SurfToAnaSurf(const occ::handle<Geom_Surface>& S);

  Standard_EXPORT void Init(const occ::handle<Geom_Surface>& S);

  void SetConvType(const GeomConvert_ConvType theConvType = GeomConvert_Simplest)
  {
    myConvType = theConvType;
  }

  void SetTarget(const GeomAbs_SurfaceType theSurfType = GeomAbs_Plane) { myTarget = theSurfType; }

  //! Returns maximal deviation of converted surface from the original
  //! one computed by last call to ConvertToAnalytical
  double Gap() const { return myGap; }

  //! Tries to convert the Surface to an Analytic form
  //! Returns the result
  //! In case of failure, returns a Null Handle
  //!
  Standard_EXPORT occ::handle<Geom_Surface> ConvertToAnalytical(const double InitialToler);
  Standard_EXPORT occ::handle<Geom_Surface> ConvertToAnalytical(const double InitialToler,
                                                                const double Umin,
                                                                const double Umax,
                                                                const double Vmin,
                                                                const double Vmax);

  //! Returns true if surfaces is same with the given tolerance
  Standard_EXPORT static bool IsSame(const occ::handle<Geom_Surface>& S1,
                                     const occ::handle<Geom_Surface>& S2,
                                     const double                     tol);

  //! Returns true, if surface is canonical
  Standard_EXPORT static bool IsCanonical(const occ::handle<Geom_Surface>& S);

private:
  //! static method for checking surface of revolution
  //! To avoid two-parts cone-like surface
  static void CheckVTrimForRevSurf(const occ::handle<Geom_SurfaceOfRevolution>& aRevSurf,
                                   double&                                      V1,
                                   double&                                      V2);

  //! static method to try create cylindrical or conical surface
  static occ::handle<Geom_Surface> TryCylinerCone(const occ::handle<Geom_Surface>& theSurf,
                                                  const bool                       theVCase,
                                                  const occ::handle<Geom_Curve>&   theUmidiso,
                                                  const occ::handle<Geom_Curve>&   theVmidiso,
                                                  const double                     theU1,
                                                  const double                     theU2,
                                                  const double                     theV1,
                                                  const double                     theV2,
                                                  const double                     theToler);

  //! static method to try create cylinrical surface using least square method
  static bool GetCylByLS(const occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints,
                         const double                                    theTol,
                         gp_Ax3&                                         thePos,
                         double&                                         theR,
                         double&                                         theGap);

  //! static method to try create cylinrical surface based on its Gauss field
  static occ::handle<Geom_Surface> TryCylinderByGaussField(const occ::handle<Geom_Surface>& theSurf,
                                                           const double                     theU1,
                                                           const double                     theU2,
                                                           const double                     theV1,
                                                           const double                     theV2,
                                                           const double theToler,
                                                           const int    theNbU         = 20,
                                                           const int    theNbV         = 20,
                                                           const bool   theLeastSquare = false);

  //! static method to try create toroidal surface.
  //! In case <isTryUMajor> = true try to use V isoline radius as minor radaius.
  static occ::handle<Geom_Surface> TryTorusSphere(const occ::handle<Geom_Surface>& theSurf,
                                                  const occ::handle<Geom_Circle>&  circle,
                                                  const occ::handle<Geom_Circle>&  otherCircle,
                                                  const double                     Param1,
                                                  const double                     Param2,
                                                  const double                     aParam1ToCrv,
                                                  const double                     aParam2ToCrv,
                                                  const double                     toler,
                                                  const bool                       isTryUMajor);

  static double ComputeGap(const occ::handle<Geom_Surface>& theSurf,
                           const double                     theU1,
                           const double                     theU2,
                           const double                     theV1,
                           const double                     theV2,
                           const occ::handle<Geom_Surface>& theNewSurf,
                           const double                     theTol = RealLast());

private:
  occ::handle<Geom_Surface> mySurf;
  double                    myGap;
  GeomConvert_ConvType      myConvType;
  GeomAbs_SurfaceType       myTarget;
};

#endif // _GeomConvert_SurfToAnaSurf_HeaderFile
