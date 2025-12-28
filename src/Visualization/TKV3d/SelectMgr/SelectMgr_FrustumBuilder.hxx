// Created on: 2014-11-24
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_FrustumBuilder_HeaderFile
#define _SelectMgr_FrustumBuilder_HeaderFile

#include <Graphic3d_Camera.hxx>
#include <Precision.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>

//! The purpose of this class is to provide unified interface for building
//! selecting frustum depending on current camera projection and orientation
//! matrices, window size and viewport parameters.
class SelectMgr_FrustumBuilder : public Standard_Transient
{
public:
  //! Creates new frustum builder with empty matrices
  Standard_EXPORT SelectMgr_FrustumBuilder();

  //! Returns current camera
  const occ::handle<Graphic3d_Camera>& Camera() const { return myCamera; }

  //! Stores current camera
  Standard_EXPORT void SetCamera(const occ::handle<Graphic3d_Camera>& theCamera);

  //! Stores current window width and height
  Standard_EXPORT void SetWindowSize(const int theWidth, const int theHeight);

  //! Stores current viewport coordinates
  Standard_EXPORT void SetViewport(const double theX,
                                   const double theY,
                                   const double theWidth,
                                   const double theHeight);

  Standard_EXPORT void InvalidateViewport();

  Standard_EXPORT void WindowSize(int& theWidth, int& theHeight) const;

  //! Calculates signed distance between plane with equation
  //! theEq and point thePnt
  Standard_EXPORT double SignedPlanePntDist(const NCollection_Vec3<double>& theEq,
                                            const NCollection_Vec3<double>& thePnt) const;

  //! Projects 2d screen point onto view frustum plane:
  //! theZ = 0 - near plane,
  //! theZ = 1 - far plane
  Standard_EXPORT gp_Pnt ProjectPntOnViewPlane(const double& theX,
                                               const double& theY,
                                               const double& theZ) const;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_FrustumBuilder, Standard_Transient)

private:
  occ::handle<Graphic3d_Camera> myCamera;
  int                           myWidth;
  int                           myHeight;
  NCollection_Vec4<double>      myViewport;
  bool                          myIsViewportSet;
};

#endif // _SelectMgr_FrustumBuilder_HeaderFile
