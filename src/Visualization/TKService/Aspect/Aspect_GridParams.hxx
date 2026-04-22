// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _Aspect_GridParams_HeaderFile
#define _Aspect_GridParams_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Aspect_GridDrawMode.hxx>
#include <Quantity_Color.hxx>
#include <gp_Pnt.hxx>

//! Appearance parameters for a shader-rendered infinite grid.
//! Used by Graphic3d_CView::GridDisplay to drive a screen-space quad
//! intersected with a grid plane in the fragment shader.
class Aspect_GridParams
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct with sensible defaults: grey lines on the plane origin with
  //! axis coloring enabled, 1/100 plane-unit spacing, overlay mode.
  Aspect_GridParams()
      : myColor(Quantity_NOC_GRAY70),
        myOrigin(0.0, 0.0, 0.0),
        myScale(0.01),
        myScaleY(0.0),
        myLineThickness(0.01),
        myRotationAngle(0.0),
        myAngularDivisions(0),
        myDrawMode(Aspect_GDM_Lines),
        myIsBackground(false),
        myIsDrawAxis(true),
        myIsInfinity(false)
  {
  }

  //! Return grid line color.
  const Quantity_Color& Color() const { return myColor; }

  //! Set grid line color.
  void SetColor(const Quantity_Color& theColor) { myColor = theColor; }

  //! Return local offset of the grid origin within the plane.
  const gp_Pnt& Origin() const { return myOrigin; }

  //! Set local offset of the grid origin within the plane.
  void SetOrigin(const gp_Pnt& theOrigin) { myOrigin = theOrigin; }

  //! Return major-grid scale factor along the plane X direction (cells per plane unit).
  double Scale() const { return myScale; }

  //! Set major-grid scale factor along the plane X direction (cells per plane unit).
  void SetScale(const double theScale) { myScale = theScale; }

  //! Return explicit Y-direction scale. When 0.0, renderer falls back to Scale() (isotropic).
  double ScaleY() const { return myScaleY; }

  //! Set explicit Y-direction scale. Pass 0.0 to mirror Scale() (isotropic, default).
  void SetScaleY(const double theScaleY) { myScaleY = theScaleY; }

  //! Effective Y-direction scale actually consumed by the renderer.
  double EffectiveScaleY() const { return myScaleY > 0.0 ? myScaleY : myScale; }

  //! Return line thickness in plane units (minimum pixel-space line width is derived from fwidth).
  double LineThickness() const { return myLineThickness; }

  //! Set line thickness in plane units.
  void SetLineThickness(const double theThickness) { myLineThickness = theThickness; }

  //! Return in-plane rotation angle (radians) applied to the grid axes around the plane normal.
  double RotationAngle() const { return myRotationAngle; }

  //! Set in-plane rotation angle (radians) applied to the grid axes around the plane normal.
  void SetRotationAngle(const double theAngle) { myRotationAngle = theAngle; }

  //! Return the angular subdivision count of the half-circle for circular grids.
  //! Zero means rectangular grid (default); any positive value switches the
  //! renderer to polar rings (Scale -> radial step) and spokes at pi/N rad.
  int AngularDivisions() const { return myAngularDivisions; }

  //! Set angular subdivision count (0 = rectangular grid, N>0 = circular with N spokes per 180 deg).
  void SetAngularDivisions(const int theDivisions) { myAngularDivisions = theDivisions; }

  //! Return TRUE when the parameters describe a circular (polar) grid.
  bool IsCircular() const { return myAngularDivisions > 0; }

  //! Return draw mode: lines, points at grid intersections, or none.
  Aspect_GridDrawMode DrawMode() const { return myDrawMode; }

  //! Set draw mode. Aspect_GDM_None suppresses rendering entirely; Points draws
  //! dots at grid-line intersections, Lines (default) draws the full grid.
  void SetDrawMode(const Aspect_GridDrawMode theMode) { myDrawMode = theMode; }

  //! Return TRUE if grid is drawn as a view-space background (behind all geometry).
  bool IsBackground() const { return myIsBackground; }

  //! Set background-mode rendering on/off.
  void SetIsBackground(const bool theIsBackground) { myIsBackground = theIsBackground; }

  //! Return TRUE if axis lines on the grid plane are drawn in red/green/blue.
  bool IsDrawAxis() const { return myIsDrawAxis; }

  //! Set axis coloring on/off.
  void SetIsDrawAxis(const bool theIsDrawAxis) { myIsDrawAxis = theIsDrawAxis; }

  //! Return TRUE if the scale adapts to camera zoom to keep an apparent grid density.
  bool IsInfinity() const { return myIsInfinity; }

  //! Set camera-adaptive scale on/off. When enabled, Scale() is ignored in favour
  //! of a derived value based on camera distance.
  void SetIsInfinity(const bool theIsInfinity) { myIsInfinity = theIsInfinity; }

private:
  Quantity_Color      myColor;
  gp_Pnt              myOrigin;
  double              myScale;
  double              myScaleY;
  double              myLineThickness;
  double              myRotationAngle;
  int                 myAngularDivisions;
  Aspect_GridDrawMode myDrawMode;
  bool                myIsBackground;
  bool                myIsDrawAxis;
  bool                myIsInfinity;
};

#endif // _Aspect_GridParams_HeaderFile
