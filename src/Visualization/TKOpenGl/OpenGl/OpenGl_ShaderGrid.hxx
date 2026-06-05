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

#ifndef _OpenGl_ShaderGrid_HeaderFile
#define _OpenGl_ShaderGrid_HeaderFile

#include <Aspect_GridParams.hxx>
#include <Bnd_Box.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_Vertex.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec2.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <gp_Ax3.hxx>

//! State and geometry model of the OpenGl shader-rendered grid.
class OpenGl_ShaderGrid
{
public:
  //! Return TRUE if the grid is currently shown.
  bool IsShown() const { return myIsShown; }

  //! Return TRUE if the grid is rendered as a background.
  bool IsBackground() const { return myParams.IsBackground(); }

  //! Return current parameters.
  const Aspect_GridParams& Params() const { return myParams; }

  //! Store grid state. Returns FALSE when parameters cannot produce a shader grid.
  bool Display(const Aspect_GridParams&             theParams,
               const gp_Ax3&                        thePlane,
               const occ::handle<Graphic3d_Camera>& theCamera,
               const occ::handle<OpenGl_Context>&   theContext);

  //! Clear grid state.
  void Erase();

  //! Add helper bounds required by camera Z fitting.
  void AddZFitBounds(Bnd_Box& theGraphicBox, const occ::handle<Graphic3d_Camera>& theCamera) const;

  //! Return snapped point for the grid under the window pixel.
  bool Echo(const occ::handle<Graphic3d_Camera>& theCamera,
            const int                            theWidth,
            const int                            theHeight,
            const int                            theX,
            const int                            theY,
            Graphic3d_Vertex&                    thePoint,
            Graphic3d_Vertex&                    theDisplayPoint) const;

  //! Return snapped point for an arbitrary world point.
  bool SnapPoint(const occ::handle<Graphic3d_Camera>& theCamera,
                 const Graphic3d_Vertex&              thePoint,
                 Graphic3d_Vertex&                    theGridPoint) const;

  //! Upload shader uniforms for current grid state.
  void SetUniforms(const occ::handle<OpenGl_Context>&       theContext,
                   const occ::handle<OpenGl_ShaderProgram>& theProgram,
                   const occ::handle<Graphic3d_Camera>&     theCamera,
                   const NCollection_Mat4<float>&           theWorldView) const;

  //! Return worldview matrix to use for drawing.
  NCollection_Mat4<float> DrawWorldView(const NCollection_Mat4<float>& theCurrentWorldView) const;

private:
  //! Compute grid plane frame.
  void frame(gp_Pnt& theOrigin, gp_XYZ& theX, gp_XYZ& theY, gp_XYZ& theN) const;

  //! Compute effective scales for the current camera.
  void effectiveScale(const occ::handle<Graphic3d_Camera>& theCamera,
                      double&                              theScaleX,
                      double&                              theScaleY) const;

  //! Return TRUE if local coordinates are inside configured grid domain.
  bool isPointInBounds(const double theLocalX, const double theLocalY) const;

  //! Add local point to box.
  static void addLocalPoint(Bnd_Box&      theBox,
                            const gp_Pnt& theOrigin,
                            const gp_XYZ& theX,
                            const gp_XYZ& theY,
                            const double  theLocalX,
                            const double  theLocalY);

  //! Add configured finite bounds to the box.
  void addFiniteBounds(Bnd_Box& theBox) const;

  //! Add view footprint bounds to the box.
  void addViewFootprintBounds(Bnd_Box&                             theBox,
                              const occ::handle<Graphic3d_Camera>& theCamera) const;

  //! Intersect camera ray at NDC point with the grid plane.
  bool planeLocalHit(const occ::handle<Graphic3d_Camera>& theCamera,
                     const double                         theNdcX,
                     const double                         theNdcY,
                     double&                              theLocalX,
                     double&                              theLocalY,
                     gp_XYZ*                              theHit            = nullptr,
                     const bool                           theToRejectBehind = true) const;

  //! Return stable visible reference point in local coordinates.
  bool referenceLocal(const occ::handle<Graphic3d_Camera>& theCamera,
                      double&                              theLocalX,
                      double&                              theLocalY) const;

  //! Return local shift snapped to a grid phase.
  static double snappedLocalShift(const double theLocal, const double theScale);

  //! Return TRUE if previous and new grid definitions share the same background anchor frame.
  bool hasSameAnchorFrame(const Aspect_GridParams& theParams, const gp_Ax3& thePlane) const;

  //! Accept echo candidate if it is visible and closer to the requested pixel.
  bool acceptEchoCandidate(const occ::handle<Graphic3d_Camera>& theCamera,
                           const int                            theWidth,
                           const int                            theHeight,
                           const int                            theX,
                           const int                            theY,
                           const gp_Pnt&                        theGridOrigin,
                           const gp_XYZ&                        theGridX,
                           const gp_XYZ&                        theGridY,
                           const double                         theLocalX,
                           const double                         theLocalY,
                           gp_XYZ&                              theBestSnapped,
                           double&                              theBestDist2,
                           bool&                                theHasBestPoint) const;

  //! Convert point to current draw view coordinates.
  static NCollection_Vec3<float> viewPoint(const NCollection_Mat4<float>& theWorldView,
                                           const gp_Pnt&                  thePoint);

  //! Convert direction to current draw view coordinates.
  static NCollection_Vec3<float> viewDirection(const NCollection_Mat4<float>& theWorldView,
                                               const gp_XYZ&                  theDirection);

  //! Return display-safe echo marker point.
  static bool echoDisplayPoint(const occ::handle<Graphic3d_Camera>& theCamera,
                               const gp_XYZ&                        theSnapped,
                               gp_Pnt&                              theDisplayPoint);

  //! Return TRUE if angle belongs to arc.
  static bool angleInArc(const double theStart, const double theEnd, const double theAngle);

private:
  Aspect_GridParams       myParams;
  gp_Ax3                  myPlane;
  NCollection_Mat4<float> myRefViewMatrix;
  bool                    myIsShown = false;
};

#endif // _OpenGl_ShaderGrid_HeaderFile
