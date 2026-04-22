// Created on: 1995-03-17
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Aspect_CircularGrid_HeaderFile
#define _Aspect_CircularGrid_HeaderFile

#include <Standard_Integer.hxx>
#include <Aspect_Grid.hxx>

class Aspect_CircularGrid : public Aspect_Grid
{
  DEFINE_STANDARD_RTTIEXT(Aspect_CircularGrid, Aspect_Grid)
public:
  //! creates a new grid. By default this grid is not
  //! active.
  Standard_EXPORT Aspect_CircularGrid(const double aRadiusStep,
                                      const int    aDivisionNumber,
                                      const double XOrigin        = 0,
                                      const double anYOrigin      = 0,
                                      const double aRotationAngle = 0);

  //! defines the x step of the grid.
  Standard_EXPORT void SetRadiusStep(const double aStep);

  //! defines the step of the grid.
  Standard_EXPORT void SetDivisionNumber(const int aNumber);

  Standard_EXPORT void SetGridValues(const double XOrigin,
                                     const double YOrigin,
                                     const double RadiusStep,
                                     const int    DivisionNumber,
                                     const double RotationAngle);

  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT void Compute(const double X,
                               const double Y,
                               double&      gridX,
                               double&      gridY) const override;

  //! returns the x step of the grid.
  Standard_EXPORT double RadiusStep() const;

  //! returns the x step of the grid.
  Standard_EXPORT int DivisionNumber() const;

  //! Set the circular grid radius (plane-local units). 0.0 (default) means
  //! unbounded - the shader draws the grid to the horizon.
  Standard_EXPORT void SetRadius(const double theRadius);

  //! Return the bounded radius. 0.0 means unbounded.
  double Radius() const { return myRadius; }

  //! Set signed offset along the plane normal for display only; snap math
  //! stays on the plane. Use a small negative value to avoid z-fighting with
  //! coplanar geometry.
  Standard_EXPORT void SetZOffset(const double theOffset);

  //! Return the display-time Z-offset along the plane normal.
  double ZOffset() const { return myZOffset; }

  //! Restrict the grid to an angular wedge, walking counter-clockwise from
  //! @p theStart to @p theEnd (radians, measured from the rotated plane X
  //! axis). Setting both values equal (e.g. both 0.0) returns to full-circle
  //! rendering - the sentinel used for unbounded.
  Standard_EXPORT void SetArcRange(const double theStart, const double theEnd);

  //! Return the arc start angle (radians). Meaningful only when IsArc() is true.
  double AngleStart() const { return myAngleStart; }

  //! Return the arc end angle (radians). Meaningful only when IsArc() is true.
  double AngleEnd() const { return myAngleEnd; }

  //! Return TRUE when the grid is restricted to an angular wedge.
  bool IsArc() const { return myIsArc; }

  Standard_EXPORT void Init() override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

private:
  double myRadiusStep;
  int    myDivisionNumber;
  double myAlpha;
  double myA1;
  double myB1;
  double myRadius;
  double myZOffset;
  double myAngleStart;
  double myAngleEnd;
  bool   myIsArc;
};

#endif // _Aspect_CircularGrid_HeaderFile
