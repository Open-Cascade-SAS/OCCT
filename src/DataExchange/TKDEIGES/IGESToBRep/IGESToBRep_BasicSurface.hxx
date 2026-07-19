// Created on: 1994-03-14
// Created by: Frederic UNTEREINER
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESToBRep_BasicSurface_HeaderFile
#define _IGESToBRep_BasicSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESToBRep_CurveAndSurface.hxx>
class Geom_Surface;
class IGESData_IGESEntity;
class Geom_Plane;
class IGESSolid_PlaneSurface;
class Geom_CylindricalSurface;
class IGESSolid_CylindricalSurface;
class Geom_ConicalSurface;
class IGESSolid_ConicalSurface;
class Geom_SphericalSurface;
class IGESSolid_SphericalSurface;
class Geom_ToroidalSurface;
class IGESSolid_ToroidalSurface;
class Geom_BSplineSurface;
class IGESGeom_SplineSurface;
class IGESGeom_BSplineSurface;

//! Provides methods to transfer basic geometric surface entities
//! from IGES to CASCADE.
//! These can be:
//! * Spline surface
//! * BSpline surface
class IGESToBRep_BasicSurface : public IGESToBRep_CurveAndSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a tool BasicSurface ready to run, with
  //! epsilons set to 1.E-04, TheModeTopo to True, the
  //! optimization of the continuity to False.
  Standard_EXPORT IGESToBRep_BasicSurface();

  //! Creates a tool BasicSurface ready to run and sets its
  //! fields as CS's.
  Standard_EXPORT IGESToBRep_BasicSurface(const IGESToBRep_CurveAndSurface& CS);

  //! Creates a tool BasicSurface ready to run.
  Standard_EXPORT IGESToBRep_BasicSurface(const double eps,
                                          const double epsGeom,
                                          const double epsCoeff,
                                          const bool   mode,
                                          const bool   modeapprox,
                                          const bool   optimized);

  //! Returns Surface from Geom if the last transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_Surface> TransferBasicSurface(
    const occ::handle<IGESData_IGESEntity>& start);

  //! Returns Plane from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_Plane> TransferPlaneSurface(
    const occ::handle<IGESSolid_PlaneSurface>& start);

  //! Returns CylindricalSurface from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_CylindricalSurface> TransferRigthCylindricalSurface(
    const occ::handle<IGESSolid_CylindricalSurface>& start);

  //! Returns ConicalSurface from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_ConicalSurface> TransferRigthConicalSurface(
    const occ::handle<IGESSolid_ConicalSurface>& start);

  //! Returns SphericalSurface from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_SphericalSurface> TransferSphericalSurface(
    const occ::handle<IGESSolid_SphericalSurface>& start);

  //! Returns SphericalSurface from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_ToroidalSurface> TransferToroidalSurface(
    const occ::handle<IGESSolid_ToroidalSurface>& start);

  //! Returns BSplineSurface from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_BSplineSurface> TransferSplineSurface(
    const occ::handle<IGESGeom_SplineSurface>& start);

  //! Returns BSplineSurface from Geom if the transfer has succeeded.
  Standard_EXPORT occ::handle<Geom_BSplineSurface> TransferBSplineSurface(
    const occ::handle<IGESGeom_BSplineSurface>& start);
};

#endif // _IGESToBRep_BasicSurface_HeaderFile
