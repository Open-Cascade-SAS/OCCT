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

#ifndef _Graphic3d_CullingTool_HeaderFile
#define _Graphic3d_CullingTool_HeaderFile

#include <Graphic3d_Camera.hxx>
#include <Graphic3d_Vec4.hxx>
#include <Graphic3d_WorldViewProjState.hxx>

//! Graphic3d_CullingTool class provides a possibility to store parameters of view volume,
//! such as its vertices and equations, and contains methods detecting if given AABB overlaps view volume.
class Graphic3d_CullingTool
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

  //! Auxiliary structure representing 3D plane.
  struct Plane
  {
    //! Creates default plane.
    Plane()
    : Origin (0.0, 0.0, 0.0),
      Normal (0.0, 0.0, 1.0) {}

    //! Creates plane with specific parameters.
    Plane (const Graphic3d_Vec3d& theOrigin,
           const Graphic3d_Vec3d& theNormal)
    : Origin (theOrigin),
      Normal (theNormal) {}

    Graphic3d_Vec3d Origin;
    Graphic3d_Vec3d Normal;
  };

public:

  //! Creates an empty selector object with parallel projection type by default.
  Standard_EXPORT Graphic3d_CullingTool();

  //! Retrieves view volume's planes equations and its vertices from projection and world-view matrices.
  //! @param theCamera [in] camera definition
  //! @param theModelWorld [in] optional object transformation for computing frustum in object local coordinate system
  Standard_EXPORT void SetViewVolume (const Handle(Graphic3d_Camera)& theCamera,
                                      const Graphic3d_Mat4d& theModelWorld = Graphic3d_Mat4d());

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
  //! @param theCtx    [in] culling properties
  //! @param theMinPnt [in] maximum point of AABB
  //! @param theMaxPnt [in] minimum point of AABB
  //! @param theIsInside [out] flag indicating if AABB is fully inside; initial value should be set to TRUE
  //! @return TRUE if AABB is completely outside of view frustum or culled by size/distance;
  //!         FALSE in case of partial or complete overlap (use theIsInside to distinguish)
  bool IsCulled (const CullingContext& theCtx,
                 const Graphic3d_Vec3d& theMinPnt,
                 const Graphic3d_Vec3d& theMaxPnt,
                 Standard_Boolean*      theIsInside = NULL) const
  {
    return IsOutFrustum(theMinPnt, theMaxPnt, theIsInside)
        || IsTooDistant(theCtx, theMinPnt, theMaxPnt, theIsInside)
        || IsTooSmall  (theCtx, theMinPnt, theMaxPnt);
  }

  //! Return the camera definition.
  const Handle(Graphic3d_Camera)& Camera() const { return myCamera; }

  //! Returns current projection matrix.
  const Graphic3d_Mat4d& ProjectionMatrix() const
  {
    return myProjectionMat;
  }

  //! Returns current world view transformation matrix.
  const Graphic3d_Mat4d& WorldViewMatrix() const
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

  //! Returns camera eye position.
  const Graphic3d_Vec3d& CameraEye() const { return myCamEye; }

  //! Returns camera direction.
  const Graphic3d_Vec3d& CameraDirection() const { return myCamDir; }

public:

  //! Calculates signed distance from plane to point.
  //! @param theNormal [in] the plane's normal.
  //! @param thePnt    [in]
  Standard_EXPORT Standard_Real SignedPlanePointDistance (const Graphic3d_Vec4d& theNormal,
                                                          const Graphic3d_Vec4d& thePnt);

  //! Detects if AABB overlaps view volume using separating axis theorem (SAT).
  //! @param theMinPnt   [in] maximum point of AABB
  //! @param theMaxPnt   [in] minimum point of AABB
  //! @param theIsInside [out] flag indicating if AABB is fully inside; initial value should be set to TRUE
  //! @return TRUE if AABB is completely outside of view frustum;
  //!         FALSE in case of partial or complete overlap (use theIsInside to distinguish)
  //! @sa SelectMgr_Frustum::hasOverlap()
  bool IsOutFrustum (const Graphic3d_Vec3d& theMinPnt,
                     const Graphic3d_Vec3d& theMaxPnt,
                     Standard_Boolean*      theIsInside = NULL) const
  {
    //     E1
    //    |_ E0
    //   /
    //    E2
    if (theMinPnt[0] > myMaxOrthoProjectionPts[0] // E0 test (x axis)
     || theMaxPnt[0] < myMinOrthoProjectionPts[0]
     || theMinPnt[1] > myMaxOrthoProjectionPts[1] // E1 test (y axis)
     || theMaxPnt[1] < myMinOrthoProjectionPts[1]
     || theMinPnt[2] > myMaxOrthoProjectionPts[2] // E2 test (z axis)
     || theMaxPnt[2] < myMinOrthoProjectionPts[2])
    {
      return true;
    }
    if (theIsInside != NULL
    && *theIsInside)
    {
      *theIsInside = theMinPnt[0] >= myMinOrthoProjectionPts[0] // E0 test (x axis)
                  && theMaxPnt[0] <= myMaxOrthoProjectionPts[0]
                  && theMinPnt[1] >= myMinOrthoProjectionPts[1] // E1 test (y axis)
                  && theMaxPnt[1] <= myMaxOrthoProjectionPts[1]
                  && theMinPnt[1] >= myMinOrthoProjectionPts[2] // E2 test (z axis)
                  && theMaxPnt[1] <= myMaxOrthoProjectionPts[2];
    }

    const Standard_Integer anIncFactor = myIsProjectionParallel ? 2 : 1;
    for (Standard_Integer aPlaneIter = 0; aPlaneIter < PlanesNB - 1; aPlaneIter += anIncFactor)
    {
      // frustum normals
      const Graphic3d_Vec3d& anAxis = myClipPlanes[aPlaneIter].Normal;
      const Graphic3d_Vec3d aPVertex (anAxis.x() > 0.0 ? theMaxPnt.x() : theMinPnt.x(),
                                      anAxis.y() > 0.0 ? theMaxPnt.y() : theMinPnt.y(),
                                      anAxis.z() > 0.0 ? theMaxPnt.z() : theMinPnt.z());
      const Standard_Real aPnt0 = aPVertex.Dot (anAxis);
      if (theIsInside == NULL
       && aPnt0 >= myMinClipProjectionPts[aPlaneIter]
       && aPnt0 <= myMaxClipProjectionPts[aPlaneIter])
      {
        continue;
      }
      
      const Graphic3d_Vec3d aNVertex (anAxis.x() > 0.0 ? theMinPnt.x() : theMaxPnt.x(),
                                      anAxis.y() > 0.0 ? theMinPnt.y() : theMaxPnt.y(),
                                      anAxis.z() > 0.0 ? theMinPnt.z() : theMaxPnt.z());
      const Standard_Real aPnt1 = aNVertex.Dot (anAxis);

      const Standard_Real aBoxProjMin = aPnt0 < aPnt1 ? aPnt0 : aPnt1;
      const Standard_Real aBoxProjMax = aPnt0 > aPnt1 ? aPnt0 : aPnt1;
      if (aBoxProjMin > myMaxClipProjectionPts[aPlaneIter]
       || aBoxProjMax < myMinClipProjectionPts[aPlaneIter])
      {
        return true;
      }

      if (theIsInside != NULL
      && *theIsInside)
      {
        *theIsInside = aBoxProjMin >= myMinClipProjectionPts[aPlaneIter]
                    && aBoxProjMax <= myMaxClipProjectionPts[aPlaneIter];
      }
    }
    return false;
  }

  //! Returns TRUE if given AABB should be discarded by distance culling criterion.
  //! @param theMinPnt   [in] maximum point of AABB
  //! @param theMaxPnt   [in] minimum point of AABB
  //! @param theIsInside [out] flag indicating if AABB is fully inside; initial value should be set to TRUE
  //! @return TRUE if AABB is completely behind culling distance;
  //!         FALSE in case of partial or complete overlap (use theIsInside to distinguish)
  bool IsTooDistant (const CullingContext&  theCtx,
                     const Graphic3d_Vec3d& theMinPnt,
                     const Graphic3d_Vec3d& theMaxPnt,
                     Standard_Boolean*      theIsInside = NULL) const
  {
    if (theCtx.DistCull <= 0.0)
    {
      return false;
    }

    // check distance to the bounding sphere as fast approximation
    const Graphic3d_Vec3d aSphereCenter = (theMinPnt + theMaxPnt) * 0.5;
    const Standard_Real   aSphereRadius = (theMaxPnt - theMinPnt).maxComp() * 0.5;
    const Standard_Real   aDistToCenter = (aSphereCenter - myCamEye).Modulus();
    if ((aDistToCenter - aSphereRadius) > theCtx.DistCull)
    {
      // clip if closest point is behind culling distance
      return true;
    }
    if (theIsInside != NULL
    && *theIsInside)
    {
      // check if farthest point is before culling distance
      *theIsInside = (aDistToCenter + aSphereRadius) <= theCtx.DistCull;
    }
    return false;
  }

  //! Returns TRUE if given AABB should be discarded by size culling criterion.
  bool IsTooSmall (const CullingContext&  theCtx,
                   const Graphic3d_Vec3d& theMinPnt,
                   const Graphic3d_Vec3d& theMaxPnt) const
  {
    if (theCtx.SizeCull2 <= 0.0)
    {
      return false;
    }

    const Standard_Real aBoxDiag2 = (theMaxPnt - theMinPnt).SquareModulus();
    if (myIsProjectionParallel)
    {
      return aBoxDiag2 < theCtx.SizeCull2;
    }

    // note that distances behind the Eye (aBndDist < 0) are not scaled correctly here,
    // but majority of such objects should be culled by frustum
    const Graphic3d_Vec3d aBndCenter = (theMinPnt + theMaxPnt) * 0.5;
    const Standard_Real   aBndDist   = (aBndCenter - myCamEye).Dot (myCamDir);
    return aBoxDiag2 < theCtx.SizeCull2 * aBndDist * aBndDist;
  }

protected:

  //! Enumerates planes of view volume.
  enum
  {
    Plane_Left,
    Plane_Right,
    Plane_Bottom,
    Plane_Top,
    Plane_Near,
    Plane_Far,
    PlanesNB
  };

protected:

  Plane                               myClipPlanes[PlanesNB]; //!< Planes
  NCollection_Array1<Graphic3d_Vec3d> myClipVerts;            //!< Vertices

  Handle(Graphic3d_Camera) myCamera; //!< camera definition

  // for caching clip points projections onto viewing area normals once per traverse
  // ORDER: LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
  Standard_Real myMaxClipProjectionPts[PlanesNB]; //!< Max view volume's vertices projections onto its normals
  Standard_Real myMinClipProjectionPts[PlanesNB]; //!< Min view volume's vertices projections onto its normals

  // for caching clip points projections onto AABB normals once per traverse
  // ORDER: E0, E1, E2
  Standard_Real myMaxOrthoProjectionPts[3]; //!< Max view volume's vertices projections onto normalized dimensions of AABB
  Standard_Real myMinOrthoProjectionPts[3]; //!< Min view volume's vertices projections onto normalized dimensions of AABB

  Standard_Boolean myIsProjectionParallel;

  Graphic3d_Mat4d myProjectionMat;
  Graphic3d_Mat4d myWorldViewMat;

  Standard_Integer myViewportWidth;
  Standard_Integer myViewportHeight;

  Graphic3d_WorldViewProjState myWorldViewProjState; //!< State of world view projection matrices.

  Graphic3d_Vec3d myCamEye;      //!< camera eye position for distance culling
  Graphic3d_Vec3d myCamDir;      //!< camera direction for size culling
  Standard_Real   myCamScale;    //!< camera scale for size culling
  Standard_Real   myPixelSize;   //!< pixel size for size culling

};

#endif // _Graphic3d_CullingTool_HeaderFile
