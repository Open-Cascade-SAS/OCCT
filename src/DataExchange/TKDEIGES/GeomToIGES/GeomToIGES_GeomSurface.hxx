// Created on: 1994-11-17
// Created by: Marie Jose MARTZ
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

#ifndef _GeomToIGES_GeomSurface_HeaderFile
#define _GeomToIGES_GeomSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToIGES_GeomEntity.hxx>
class IGESData_IGESEntity;
class Geom_Surface;
class Geom_BoundedSurface;
class Geom_BSplineSurface;
class Geom_BezierSurface;
class Geom_RectangularTrimmedSurface;
class Geom_ElementarySurface;
class Geom_Plane;
class Geom_CylindricalSurface;
class Geom_ConicalSurface;
class Geom_SphericalSurface;
class Geom_ToroidalSurface;
class Geom_SweptSurface;
class Geom_SurfaceOfLinearExtrusion;
class Geom_SurfaceOfRevolution;
class Geom_OffsetSurface;

//! This class implements the transfer of the Surface Entity from Geom
//! To IGES. These can be:
//! . BoundedSurface
//! * BSplineSurface
//! * BezierSurface
//! * RectangularTrimmedSurface
//! . ElementarySurface
//! * Plane
//! * CylindricalSurface
//! * ConicalSurface
//! * SphericalSurface
//! * ToroidalSurface
//! . SweptSurface
//! * SurfaceOfLinearExtrusion
//! * SurfaceOfRevolution
//! . OffsetSurface
class GeomToIGES_GeomSurface : public GeomToIGES_GeomEntity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToIGES_GeomSurface();

  //! Creates a tool GeomSurface ready to run and sets its
  //! fields as GE's.
  Standard_EXPORT GeomToIGES_GeomSurface(const GeomToIGES_GeomEntity& GE);

  //! Transfer a GeometryEntity which answer True to the
  //! member : BRepToIGES::IsGeomSurface(Geometry). If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_Surface>& start,
    const double                     Udeb,
    const double                     Ufin,
    const double                     Vdeb,
    const double                     Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_BoundedSurface>& start,
    const double                            Udeb,
    const double                            Ufin,
    const double                            Vdeb,
    const double                            Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_BSplineSurface>& start,
    const double                            Udeb,
    const double                            Ufin,
    const double                            Vdeb,
    const double                            Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_BezierSurface>& start,
    const double                           Udeb,
    const double                           Ufin,
    const double                           Vdeb,
    const double                           Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_RectangularTrimmedSurface>& start,
    const double                                       Udeb,
    const double                                       Ufin,
    const double                                       Vdeb,
    const double                                       Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_ElementarySurface>& start,
    const double                               Udeb,
    const double                               Ufin,
    const double                               Vdeb,
    const double                               Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_Plane>& start,
    const double                   Udeb,
    const double                   Ufin,
    const double                   Vdeb,
    const double                   Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_CylindricalSurface>& start,
    const double                                Udeb,
    const double                                Ufin,
    const double                                Vdeb,
    const double                                Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_ConicalSurface>& start,
    const double                            Udeb,
    const double                            Ufin,
    const double                            Vdeb,
    const double                            Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_SphericalSurface>& start,
    const double                              Udeb,
    const double                              Ufin,
    const double                              Vdeb,
    const double                              Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_ToroidalSurface>& start,
    const double                             Udeb,
    const double                             Ufin,
    const double                             Vdeb,
    const double                             Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_SweptSurface>& start,
    const double                          Udeb,
    const double                          Ufin,
    const double                          Vdeb,
    const double                          Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_SurfaceOfLinearExtrusion>& start,
    const double                                      Udeb,
    const double                                      Ufin,
    const double                                      Vdeb,
    const double                                      Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_SurfaceOfRevolution>& start,
    const double                                 Udeb,
    const double                                 Ufin,
    const double                                 Vdeb,
    const double                                 Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSurface(
    const occ::handle<Geom_OffsetSurface>& start,
    const double                           Udeb,
    const double                           Ufin,
    const double                           Vdeb,
    const double                           Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferPlaneSurface(
    const occ::handle<Geom_Plane>& start,
    const double                   Udeb,
    const double                   Ufin,
    const double                   Vdeb,
    const double                   Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferCylindricalSurface(
    const occ::handle<Geom_CylindricalSurface>& start,
    const double                                Udeb,
    const double                                Ufin,
    const double                                Vdeb,
    const double                                Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferConicalSurface(
    const occ::handle<Geom_ConicalSurface>& start,
    const double                            Udeb,
    const double                            Ufin,
    const double                            Vdeb,
    const double                            Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferSphericalSurface(
    const occ::handle<Geom_SphericalSurface>& start,
    const double                              Udeb,
    const double                              Ufin,
    const double                              Vdeb,
    const double                              Vfin);

  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferToroidalSurface(
    const occ::handle<Geom_ToroidalSurface>& start,
    const double                             Udeb,
    const double                             Ufin,
    const double                             Vdeb,
    const double                             Vfin);

  //! Returns the value of "TheLength"
  Standard_EXPORT double Length() const;

  //! Returns Brep mode flag.
  Standard_EXPORT bool GetBRepMode() const;

  //! Sets BRep mode flag.
  Standard_EXPORT void SetBRepMode(const bool flag);

  //! Returns flag for writing elementary surfaces
  Standard_EXPORT bool GetAnalyticMode() const;

  //! Setst flag for writing elementary surfaces
  Standard_EXPORT void SetAnalyticMode(const bool flag);

private:
  double TheLength;
  bool   myBRepMode;
  bool   myAnalytic;
};

#endif // _GeomToIGES_GeomSurface_HeaderFile
