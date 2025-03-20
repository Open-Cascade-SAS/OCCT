// Created on: 1999-05-21
// Created by: Pavel DURANDIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeUpgrade_ConvertSurfaceToBezierBasis_HeaderFile
#define _ShapeUpgrade_ConvertSurfaceToBezierBasis_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ShapeUpgrade_SplitSurface.hxx>
class ShapeExtend_CompositeSurface;

class ShapeUpgrade_ConvertSurfaceToBezierBasis;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_ConvertSurfaceToBezierBasis, ShapeUpgrade_SplitSurface)

//! Converts a plane, bspline surface, surface of revolution, surface
//! of extrusion, offset surface to grid of bezier basis surface (
//! bezier surface,
//! surface of revolution based on bezier curve,
//! offset surface based on any previous type).
class ShapeUpgrade_ConvertSurfaceToBezierBasis : public ShapeUpgrade_SplitSurface
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_ConvertSurfaceToBezierBasis();

  //! Splits a list of beziers computed by Compute method according
  //! the split values and splitting parameters.
  Standard_EXPORT virtual void Build(const Standard_Boolean Segment) Standard_OVERRIDE;

  //! Converts surface into a grid of bezier based surfaces, and
  //! stores this grid.
  Standard_EXPORT virtual void Compute(const Standard_Boolean Segment) Standard_OVERRIDE;

  //! Returns the grid of bezier based surfaces correspondent to
  //! original surface.
  Standard_EXPORT Handle(ShapeExtend_CompositeSurface) Segments() const;

  //! Sets mode for conversion Geom_Plane to Bezier
  void SetPlaneMode(const Standard_Boolean mode);

  //! Returns the Geom_Pline conversion mode.
  Standard_Boolean GetPlaneMode() const;

  //! Sets mode for conversion Geom_SurfaceOfRevolution to Bezier
  void SetRevolutionMode(const Standard_Boolean mode);

  //! Returns the Geom_SurfaceOfRevolution conversion mode.
  Standard_Boolean GetRevolutionMode() const;

  //! Sets mode for conversion Geom_SurfaceOfLinearExtrusion to Bezier
  void SetExtrusionMode(const Standard_Boolean mode);

  //! Returns the Geom_SurfaceOfLinearExtrusion conversion mode.
  Standard_Boolean GetExtrusionMode() const;

  //! Sets mode for conversion Geom_BSplineSurface to Bezier
  void SetBSplineMode(const Standard_Boolean mode);

  //! Returns the Geom_BSplineSurface conversion mode.
  Standard_Boolean GetBSplineMode() const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_ConvertSurfaceToBezierBasis, ShapeUpgrade_SplitSurface)

protected:
private:
  Handle(ShapeExtend_CompositeSurface) mySegments;
  Standard_Boolean                     myPlaneMode;
  Standard_Boolean                     myRevolutionMode;
  Standard_Boolean                     myExtrusionMode;
  Standard_Boolean                     myBSplineMode;
};

#include <ShapeUpgrade_ConvertSurfaceToBezierBasis.lxx>

#endif // _ShapeUpgrade_ConvertSurfaceToBezierBasis_HeaderFile
