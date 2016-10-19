// Created on: 2014-05-22
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

#ifndef _SelectMgr_RectangularFrustum_HeaderFile
#define _SelectMgr_RectangularFrustum_HeaderFile

#include <SelectMgr_Frustum.hxx>
#include <SelectMgr_ViewClipRange.hxx>

//! This class contains representation of rectangular selecting frustum, created in case
//! of point and box selection, and algorithms for overlap detection between selecting
//! frustum and sensitive entities. The principle of frustum calculation:
//! - for point selection: on a near view frustum plane rectangular neighborhood of
//!                        user-picked point is created according to the pixel tolerance
//!                        given and then this rectangle is projected onto far view frustum
//!                        plane. This rectangles define the parallel bases of selecting frustum;
//! - for box selection: box points are projected onto near and far view frustum planes.
//!                      These 2 projected rectangles define parallel bases of selecting frustum.
//! Overlap detection tests are implemented according to the terms of separating axis
//! theorem (SAT).
class SelectMgr_RectangularFrustum : public SelectMgr_Frustum<4>
{
public:

  SelectMgr_RectangularFrustum() : myScale (1.0), myIsViewClipEnabled (Standard_True) {};

  //! Builds volume according to the point and given pixel tolerance
  Standard_EXPORT virtual void Build (const gp_Pnt2d& thePoint) Standard_OVERRIDE;

  //! Builds volume according to the selected rectangle
  Standard_EXPORT virtual void Build (const gp_Pnt2d& theMinPnt,
                                      const gp_Pnt2d& theMaxPnt) Standard_OVERRIDE;

  //! IMPORTANT: Scaling makes sense only for frustum built on a single point!
  //!            Note that this method does not perform any checks on type of the frustum.
  //! Returns a copy of the frustum resized according to the scale factor given
  //! and transforms it using the matrix given.
  //! There are no default parameters, but in case if:
  //!    - transformation only is needed: @theScaleFactor must be initialized as any negative value;
  //!    - scale only is needed: @theTrsf must be set to gp_Identity.
  Standard_EXPORT virtual Handle(SelectMgr_BaseFrustum) ScaleAndTransform (const Standard_Integer theScaleFactor,
                                                                           const gp_GTrsf& theTrsf) const Standard_OVERRIDE;


  // SAT Tests for different objects

  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                     const SelectMgr_Vec3& theBoxMax,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                     const SelectMgr_Vec3& theBoxMax,
                                                     Standard_Boolean*     theInside = NULL) Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt) Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean Overlaps (const TColgp_Array1OfPnt& theArrayOfPnts,
                                                     Select3D_TypeOfSensitivity theSensType,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                                     const gp_Pnt& thePnt2,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                                     const gp_Pnt& thePnt2,
                                                     const gp_Pnt& thePnt3,
                                                     Select3D_TypeOfSensitivity theSensType,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG
  Standard_EXPORT virtual Standard_Real DistToGeometryCenter (const gp_Pnt& theCOG) Standard_OVERRIDE;

  //! Calculates the point on a view ray that was detected during the run of selection algo by given depth
  Standard_EXPORT virtual gp_Pnt DetectedPoint (const Standard_Real theDepth) const Standard_OVERRIDE;

  //! Checks if the point of sensitive in which selection was detected belongs
  //! to the region defined by clipping planes
  Standard_EXPORT virtual Standard_Boolean IsClipped (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                      const Standard_Real theDepth) Standard_OVERRIDE;

  //! Valid for point selection only!
  //! Computes depth range for global (defined for the whole view) clipping planes.
  Standard_EXPORT virtual void SetViewClipping (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes) Standard_OVERRIDE;

  //! Set if view clipping plane is enabled or not.
  //! @return previous value of the flag
  virtual Standard_Boolean SetViewClippingEnabled (const Standard_Boolean theToEnable) Standard_OVERRIDE
  {
    Standard_Boolean aPrevValue = myIsViewClipEnabled;
    myIsViewClipEnabled = theToEnable;
    return aPrevValue;
  }

  //! A set of helper functions that return rectangular selecting frustum data
  inline const gp_Pnt* GetVertices() const { return myVertices; }

  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto near view frustum plane
  inline const gp_Pnt& GetNearPnt() const { return myNearPickedPnt; }

  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto far view frustum plane
  inline const gp_Pnt& GetFarPnt() const { return myFarPickedPnt; }

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT virtual void GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const Standard_OVERRIDE;

protected:

  Standard_EXPORT void segmentSegmentDistance (const gp_Pnt& theSegPnt1,
                                               const gp_Pnt& theSegPnt2,
                                               Standard_Real& theDepth);

  Standard_EXPORT void segmentPlaneIntersection (const gp_Vec& thePlane,
                                                 const gp_Pnt& thePntOnPlane,
                                                 Standard_Real& theDepth);

  //! Computes valid depth range for the given clipping planes
  Standard_EXPORT void computeClippingRange (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                             Standard_Real& theDepthMin,
                                             Standard_Real& theDepthMax);

  //! Returns false if theDepth must be clipped by current view clip range
  Standard_EXPORT Standard_Boolean isViewClippingOk (const Standard_Real theDepth) const;

private:

  void cacheVertexProjections (SelectMgr_RectangularFrustum* theFrustum) const;

private:
  enum { LeftTopNear, LeftTopFar,
         LeftBottomNear, LeftBottomFar,
         RightTopNear, RightTopFar,
         RightBottomNear, RightBottomFar };

private:

  gp_Pnt                  myNearPickedPnt;             //!< 3d projection of user-picked selection point onto near view plane
  gp_Pnt                  myFarPickedPnt;              //!< 3d projection of user-picked selection point onto far view plane
  gp_Vec                  myViewRayDir;
  gp_Pnt2d                myMousePos;                  //!< Mouse coordinates
  Standard_Real           myScale;                     //!< Scale factor of applied transformation, if there was any
  SelectMgr_ViewClipRange myViewClipRange;
  Standard_Boolean        myIsViewClipEnabled;         //!< view clipping enabled state

};

#endif // _SelectMgr_RectangularFrustum_HeaderFile
