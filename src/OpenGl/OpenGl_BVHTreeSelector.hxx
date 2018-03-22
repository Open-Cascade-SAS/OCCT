// Created on: 2013-12-25
// Created by: Varvara POSKONINA
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

#ifndef _OpenGl_BVHTreeSelector_HeaderFile
#define _OpenGl_BVHTreeSelector_HeaderFile

#include <Graphic3d_Camera.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <OpenGl_Vec.hxx>

//! BVHTreeSelector class provides a possibility to store parameters of view volume,
//! such as its vertices and equations, and contains methods detecting if given AABB overlaps
//! view volume.
class OpenGl_BVHTreeSelector
{
public:
  //! Auxiliary structure holding non-persistent culling options.
  struct CullingContext
  {
    Standard_Real DistCull;  //!< culling distance
    Standard_Real SizeCull2; //!< squared culling size

    //! Empty constructor.
    CullingContext() : DistCull (-1.0), SizeCull2 (-1.0) {}
  };
public:

  //! Creates an empty selector object with parallel projection type by default.
  Standard_EXPORT OpenGl_BVHTreeSelector();

  //! Retrieves view volume's planes equations and its vertices from projection and world-view matrices.
  Standard_EXPORT void SetViewVolume (const Handle(Graphic3d_Camera)& theCamera);

  Standard_EXPORT void SetViewportSize (Standard_Integer theViewportWidth,
                                        Standard_Integer theViewportHeight,
                                        Standard_Real theResolutionRatio);

  //! Setup distance culling.
  Standard_EXPORT void SetCullingDistance (CullingContext& theCtx,
                                           Standard_Real theDistance) const;

  //! Setup size culling.
  Standard_EXPORT void SetCullingSize (CullingContext& theCtx,
                                       Standard_Real theSize) const;

  //! Caches view volume's vertices projections along its normals and AABBs dimensions.
  //! Must be called at the beginning of each BVH tree traverse loop.
  Standard_EXPORT void CacheClipPtsProjections();

  //! Checks whether given AABB should be entirely culled or not.
  //! @param theCtx   [in] culling properties
  //! @param theMinPt [in] maximum point of AABB
  //! @param theMaxPt [in] minimum point of AABB
  //! @return Standard_True, if AABB is in viewing area, Standard_False otherwise
  bool IsCulled (const CullingContext& theCtx,
                 const OpenGl_Vec3d& theMinPt,
                 const OpenGl_Vec3d& theMaxPt) const
  {
    return isFullOut   (theMinPt, theMaxPt)
        || isTooDistant(theCtx, theMinPt, theMaxPt)
        || isTooSmall  (theCtx, theMinPt, theMaxPt);
  }

  //! Return the camera definition.
  const Handle(Graphic3d_Camera)& Camera() const { return myCamera; }

  //! Returns current projection matrix.
  const OpenGl_Mat4d& ProjectionMatrix() const
  {
    return myProjectionMat;
  }

  //! Returns current world view transformation matrix.
  const OpenGl_Mat4d& WorldViewMatrix() const
  {
    return myWorldViewMat;
  }

  Standard_Integer ViewportWidth() const
  {
    return myViewportWidth;
  }

  Standard_Integer ViewportHeight() const
  {
    return myViewportHeight;
  }

  //! Returns state of current world view projection transformation matrices.
  const Graphic3d_WorldViewProjState& WorldViewProjState() const
  {
    return myWorldViewProjState;
  }

protected:

  //! Calculates signed distance from plane to point.
  //! @param theNormal [in] the plane's normal.
  //! @param thePnt    [in]
  Standard_EXPORT Standard_Real SignedPlanePointDistance (const OpenGl_Vec4d& theNormal,
                                                          const OpenGl_Vec4d& thePnt);

  //! Detects if AABB overlaps view volume using separating axis theorem (SAT).
  //! @param theMinPt [in] maximum point of AABB.
  //! @param theMaxPt [in] minimum point of AABB.
  //! @return FALSE, if AABB is in viewing area, TRUE otherwise.
  bool isFullOut (const OpenGl_Vec3d& theMinPt,
                  const OpenGl_Vec3d& theMaxPt) const
  {
    //     E1
    //    |_ E0
    //   /
    //    E2

    // E0 test
    if (theMinPt.x() > myMaxOrthoProjectionPts[0]
     || theMaxPt.x() < myMinOrthoProjectionPts[0])
    {
      return true;
    }

    // E1 test
    if (theMinPt.y() > myMaxOrthoProjectionPts[1]
     || theMaxPt.y() < myMinOrthoProjectionPts[1])
    {
      return true;
    }

    // E2 test
    if (theMinPt.z() > myMaxOrthoProjectionPts[2]
     || theMaxPt.z() < myMinOrthoProjectionPts[2])
    {
      return true;
    }

    Standard_Real aBoxProjMax = 0.0, aBoxProjMin = 0.0;
    const Standard_Integer anIncFactor = myIsProjectionParallel ? 2 : 1;
    for (Standard_Integer aPlaneIter = 0; aPlaneIter < 5; aPlaneIter += anIncFactor)
    {
      OpenGl_Vec4d aPlane = myClipPlanes[aPlaneIter];
      aBoxProjMax = (aPlane.x() > 0.0 ? (aPlane.x() * theMaxPt.x()) : aPlane.x() * theMinPt.x())
                  + (aPlane.y() > 0.0 ? (aPlane.y() * theMaxPt.y()) : aPlane.y() * theMinPt.y())
                  + (aPlane.z() > 0.0 ? (aPlane.z() * theMaxPt.z()) : aPlane.z() * theMinPt.z());
      if (aBoxProjMax > myMinClipProjectionPts[aPlaneIter]
       && aBoxProjMax < myMaxClipProjectionPts[aPlaneIter])
      {
        continue;
      }

      aBoxProjMin = (aPlane.x() < 0.0 ? aPlane.x() * theMaxPt.x() : aPlane.x() * theMinPt.x())
                  + (aPlane.y() < 0.0 ? aPlane.y() * theMaxPt.y() : aPlane.y() * theMinPt.y())
                  + (aPlane.z() < 0.0 ? aPlane.z() * theMaxPt.z() : aPlane.z() * theMinPt.z());
      if (aBoxProjMin > myMaxClipProjectionPts[aPlaneIter]
       || aBoxProjMax < myMinClipProjectionPts[aPlaneIter])
      {
        return true;
      }
    }
    return false;
  }

  //! Returns TRUE if given AABB should be discarded by distance culling criterion.
  bool isTooDistant (const CullingContext& theCtx,
                     const OpenGl_Vec3d& theMinPt,
                     const OpenGl_Vec3d& theMaxPt) const
  {
    if (theCtx.DistCull <= 0.0)
    {
      return false;
    }

    // check distance to the bounding sphere as fast approximation
    const Graphic3d_Vec3d aSphereCenter = (theMinPt + theMaxPt) * 0.5;
    const Standard_Real   aSphereRadius = (theMaxPt - theMinPt).maxComp() * 0.5;
    return (aSphereCenter - myCamEye).Modulus() - aSphereRadius > theCtx.DistCull;
  }

  //! Returns TRUE if given AABB should be discarded by size culling criterion.
  bool isTooSmall (const CullingContext& theCtx,
                   const OpenGl_Vec3d& theMinPt,
                   const OpenGl_Vec3d& theMaxPt) const
  {
    if (theCtx.SizeCull2 <= 0.0)
    {
      return false;
    }

    const Standard_Real aBoxDiag2 = (theMaxPt - theMinPt).SquareModulus();
    if (myIsProjectionParallel)
    {
      return aBoxDiag2 < theCtx.SizeCull2;
    }

    // note that distances behind the Eye (aBndDist < 0) are not scaled correctly here,
    // but majority of such objects should be culled by frustum
    const OpenGl_Vec3d  aBndCenter = (theMinPt + theMaxPt) * 0.5;
    const Standard_Real aBndDist   = (aBndCenter - myCamEye).Dot (myCamDir);
    return aBoxDiag2 < theCtx.SizeCull2 * aBndDist * aBndDist;
  }

protected:

  //! Enumerates planes of view volume.
  enum
  {
    Plane_Top,
    Plane_Bottom,
    Plane_Left,
    Plane_Right,
    Plane_Near,
    Plane_Far,
    PlanesNB
  };

  //! Enumerates vertices of view volume.
  enum
  {
    ClipVert_LeftTopNear,
    ClipVert_LeftBottomNear,
    ClipVert_RightTopNear,
    ClipVert_RightBottomNear,
    ClipVert_LeftTopFar,
    ClipVert_LeftBottomFar,
    ClipVert_RightTopFar,
    ClipVert_RightBottomFar,
    ClipVerticesNB
  };

protected:

  OpenGl_Vec4d myClipPlanes[PlanesNB];      //!< Plane equations
  OpenGl_Vec4d myClipVerts[ClipVerticesNB]; //!< Vertices

  Handle(Graphic3d_Camera) myCamera; //!< camera definition

  // for caching clip points projections onto viewing area normals once per traverse
  // ORDER: TOP, BOTTOM, LEFT, RIGHT, NEAR, FAR
  Standard_Real myMaxClipProjectionPts[PlanesNB]; //!< Max view volume's vertices projections onto its normals
  Standard_Real myMinClipProjectionPts[PlanesNB]; //!< Min view volume's vertices projections onto its normals

  // for caching clip points projections onto AABB normals once per traverse
  // ORDER: E0, E1, E2
  Standard_Real myMaxOrthoProjectionPts[3]; //!< Max view volume's vertices projections onto normalized dimensions of AABB
  Standard_Real myMinOrthoProjectionPts[3]; //!< Min view volume's vertices projections onto normalized dimensions of AABB

  Standard_Boolean myIsProjectionParallel;

  OpenGl_Mat4d myProjectionMat;
  OpenGl_Mat4d myWorldViewMat;

  Standard_Integer myViewportWidth;
  Standard_Integer myViewportHeight;

  Graphic3d_WorldViewProjState myWorldViewProjState; //!< State of world view projection matrices.

  Graphic3d_Vec3d myCamEye;      //!< camera eye position for distance culling
  Graphic3d_Vec3d myCamDir;      //!< camera direction for size culling
  Standard_Real   myCamScale;    //!< camera scale for size culling
  Standard_Real   myPixelSize;   //!< pixel size for size culling

};

#endif // _OpenGl_BVHTreeSelector_HeaderFile
