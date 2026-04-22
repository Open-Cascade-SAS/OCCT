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
#include <Standard_NegativeValue.hxx>

#include <Aspect_GridDrawMode.hxx>
#include <Quantity_Color.hxx>
#include <gp_Pnt.hxx>

//! Render-only appearance parameters for a shader-rendered grid.
//!
//! Drives a screen-space quad intersected with a grid plane in the fragment
//! shader (see Graphic3d_ShaderManager::getGridProgram). This is a pure value
//! struct; it does NOT own any snap state. The authoritative grid geometry for
//! snap selection still lives on Aspect_RectangularGrid / Aspect_CircularGrid,
//! and V3d_{Rectangular,Circular}Grid::syncViews produces an Aspect_GridParams
//! from the grid on every display. If you introduce a new field here that has
//! a matching concept on Aspect_Grid, update syncViews too so both layers stay
//! in sync.
class Aspect_GridParams
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct with sensible defaults: grey lines on the plane origin with
  //! axis coloring enabled, 1/100 plane-unit spacing, overlay mode,
  //! unbounded in extent and radius.
  Aspect_GridParams()
      : myColor(Quantity_NOC_GRAY70),
        myAccentColor(Quantity_NOC_GRAY50),
        myOrigin(0.0, 0.0, 0.0),
        myScale(0.01),
        myScaleY(0.0),
        myAccentScaleX(0.0),
        myAccentScaleY(0.0),
        myAccentAngularScale(0.0),
        myLineThickness(0.01),
        myRotationAngle(0.0),
        mySizeX(0.0),
        mySizeY(0.0),
        myRadius(0.0),
        myZOffset(0.0),
        myAngleStart(0.0),
        myAngleEnd(0.0),
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

  //! Return accent color used by the classical V3d grid emulation path.
  const Quantity_Color& AccentColor() const { return myAccentColor; }

  //! Set accent color used by the classical V3d grid emulation path.
  void SetAccentColor(const Quantity_Color& theColor) { myAccentColor = theColor; }

  //! Return accent overlay scale along the plane X/radial direction.
  //! Zero disables the accent layer on that axis.
  double AccentScaleX() const { return myAccentScaleX; }

  //! Set accent overlay scale along the plane X/radial direction.
  void SetAccentScaleX(const double theScale) { myAccentScaleX = theScale; }

  //! Return accent overlay scale along the plane Y direction.
  //! Zero disables the accent layer on that axis.
  double AccentScaleY() const { return myAccentScaleY; }

  //! Set accent overlay scale along the plane Y direction.
  void SetAccentScaleY(const double theScale) { myAccentScaleY = theScale; }

  //! Return accent overlay angular scale for circular-grid spokes.
  //! Zero disables the angular accent layer.
  double AccentAngularScale() const { return myAccentAngularScale; }

  //! Set accent overlay angular scale for circular-grid spokes.
  void SetAccentAngularScale(const double theScale) { myAccentAngularScale = theScale; }

  //! Return major-grid scale factor along the plane X direction (cells per plane unit).
  double Scale() const { return myScale; }

  //! Set major-grid scale factor along the plane X direction (cells per plane unit).
  //! Must be non-negative; zero is a valid "unused" sentinel.
  void SetScale(const double theScale)
  {
    Standard_NegativeValue_Raise_if(theScale < 0.0, "invalid grid scale");
    myScale = theScale;
  }

  //! Return explicit Y-direction scale. When 0.0, renderer falls back to Scale() (isotropic).
  double ScaleY() const { return myScaleY; }

  //! Set explicit Y-direction scale. Pass 0.0 to mirror Scale() (isotropic, default).
  void SetScaleY(const double theScaleY)
  {
    Standard_NegativeValue_Raise_if(theScaleY < 0.0, "invalid grid Y-scale");
    myScaleY = theScaleY;
  }

  //! Effective Y-direction scale actually consumed by the renderer.
  double EffectiveScaleY() const { return myScaleY > 0.0 ? myScaleY : myScale; }

  //! Return line thickness in plane units (minimum pixel-space line width is derived from fwidth).
  double LineThickness() const { return myLineThickness; }

  //! Set line thickness in plane units.
  void SetLineThickness(const double theThickness)
  {
    Standard_NegativeValue_Raise_if(theThickness < 0.0, "invalid grid line thickness");
    myLineThickness = theThickness;
  }

  //! Return in-plane rotation angle (radians) applied to the grid axes around the plane normal.
  double RotationAngle() const { return myRotationAngle; }

  //! Set in-plane rotation angle (radians) applied to the grid axes around the plane normal.
  void SetRotationAngle(const double theAngle) { myRotationAngle = theAngle; }

  //! Return the angular subdivision count of the half-circle for circular grids.
  //! Zero means rectangular grid (default); any positive value switches the
  //! renderer to polar rings (Scale -> radial step) and spokes at pi/N rad.
  int AngularDivisions() const { return myAngularDivisions; }

  //! Set angular subdivision count (0 = rectangular grid, N>0 = circular with N spokes per 180
  //! deg).
  void SetAngularDivisions(const int theDivisions) { myAngularDivisions = theDivisions; }

  //! Return TRUE when the parameters describe a circular (polar) grid.
  bool IsCircular() const { return myAngularDivisions > 0; }

  //! Return rectangular bounded extent along plane X; 0.0 means unbounded.
  double SizeX() const { return mySizeX; }

  //! Set rectangular bounded extent along plane X; 0.0 means unbounded.
  void SetSizeX(const double theSize)
  {
    Standard_NegativeValue_Raise_if(theSize < 0.0, "invalid grid X-size");
    mySizeX = theSize;
  }

  //! Return rectangular bounded extent along plane Y; 0.0 means unbounded.
  double SizeY() const { return mySizeY; }

  //! Set rectangular bounded extent along plane Y; 0.0 means unbounded.
  void SetSizeY(const double theSize)
  {
    Standard_NegativeValue_Raise_if(theSize < 0.0, "invalid grid Y-size");
    mySizeY = theSize;
  }

  //! Return circular bounded radius; 0.0 means unbounded.
  double Radius() const { return myRadius; }

  //! Set circular bounded radius; 0.0 means unbounded.
  void SetRadius(const double theRadius)
  {
    Standard_NegativeValue_Raise_if(theRadius < 0.0, "invalid grid radius");
    myRadius = theRadius;
  }

  //! Return signed plane-normal offset applied at render time.
  double ZOffset() const { return myZOffset; }

  //! Set signed plane-normal offset applied at render time (display only;
  //! snap math stays on the unshifted plane).
  void SetZOffset(const double theOffset) { myZOffset = theOffset; }

  //! Return arc start angle (radians). Meaningful only when IsArc() is true.
  double AngleStart() const { return myAngleStart; }

  //! Return arc end angle (radians). Meaningful only when IsArc() is true.
  double AngleEnd() const { return myAngleEnd; }

  //! Restrict the circular grid to an angular wedge [start, end], walking CCW.
  //! Equal start and end (e.g. 0.0 and 0.0) returns to full-circle rendering.
  void SetArcRange(const double theStart, const double theEnd)
  {
    myAngleStart = theStart;
    myAngleEnd   = theEnd;
  }

  //! Return TRUE when the parameters describe a bounded rectangle or disc.
  bool IsBounded() const { return mySizeX > 0.0 || mySizeY > 0.0 || myRadius > 0.0; }

  //! Return TRUE when the circular grid is restricted to a sub-arc.
  bool IsArc() const { return myAngleStart != myAngleEnd; }

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
  Quantity_Color      myAccentColor;
  gp_Pnt              myOrigin;
  double              myScale;
  double              myScaleY;
  double              myAccentScaleX;
  double              myAccentScaleY;
  double              myAccentAngularScale;
  double              myLineThickness;
  double              myRotationAngle;
  double              mySizeX;
  double              mySizeY;
  double              myRadius;
  double              myZOffset;
  double              myAngleStart;
  double              myAngleEnd;
  int                 myAngularDivisions;
  Aspect_GridDrawMode myDrawMode;
  bool                myIsBackground;
  bool                myIsDrawAxis;
  bool                myIsInfinity;
};

#endif // _Aspect_GridParams_HeaderFile
