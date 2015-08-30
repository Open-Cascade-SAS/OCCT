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

#include <Standard_DefineHandle.hxx>

#include <Graphic3d_Camera.hxx>
#include <Precision.hxx>
#include <SelectMgr_VectorTypes.hxx>

//! The purpose of this class is to provide unified interface for building
//! selecting frustum depending on current camera projection and orientation
//! matrices, window size and viewport parameters.
class SelectMgr_FrustumBuilder : public Standard_Transient
{
public:
  //! Creates new frustum builder with empty matrices
  SelectMgr_FrustumBuilder();

  //! Stores current orientation matrix
  void SetOrientation (const Graphic3d_Mat4d& theOrientation);

  //! Stores current projection matrix
  void SetProjection (const Graphic3d_Mat4d& theProjection);

  //! Stores current window width and height
  void SetWindowSize (const Standard_Integer theWidth,
                      const Standard_Integer theHeight);

  //! Stores current viewport coordinates
  void SetViewport (const Standard_Real theX,
                    const Standard_Real theY,
                    const Standard_Real theWidth,
                    const Standard_Real theHeight);

  void InvalidateViewport();

  //! Calculates signed distance between plane with equation
  //! theEq and point thePnt
  Standard_Real SignedPlanePntDist (const SelectMgr_Vec3& theEq,
                                   const SelectMgr_Vec3& thePnt) const;

  //! Projects 2d screen point onto view frustum plane:
  //! theZ = 0 - near plane,
  //! theZ = 1 - far plane
  gp_Pnt ProjectPntOnViewPlane (const Standard_Real& theX,
                                const Standard_Real& theY,
                                const Standard_Real& theZ) const;

  DEFINE_STANDARD_RTTI(SelectMgr_FrustumBuilder)

private:

  //! Unprojects point from NDC coords to 3d world space
  gp_Pnt unProject (const gp_Pnt& thePnt) const;

private:

  Graphic3d_Mat4d                   myOrientation;
  Graphic3d_Mat4d                   myProjection;
  Standard_Integer                  myWidth;
  Standard_Integer                  myHeight;
  NCollection_Vec4<Standard_Real>   myViewport;
  Standard_Boolean                  myIsViewportSet;
};

DEFINE_STANDARD_HANDLE(SelectMgr_FrustumBuilder, Standard_Transient)

#endif // _SelectMgr_FrustumBuilder_HeaderFile
