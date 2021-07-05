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

#ifndef _SelectMgr_TriangularFrustumSet_HeaderFile
#define _SelectMgr_TriangularFrustumSet_HeaderFile

#include <SelectMgr_TriangularFrustum.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>

typedef NCollection_List<Handle(SelectMgr_TriangularFrustum)> SelectMgr_TriangFrustums;

//! This class is used to handle polyline selection. The main principle of polyline selection
//! algorithm is to split the polygon defined by polyline onto triangles.
//! Than each of them is considered as a base for triangular frustum building.
//! In other words, each triangle vertex will be projected from 2d screen space to 3d world space onto near and far view frustum planes.
//! Thus, the projected triangles make up the bases of selecting frustum.
//! When the set of such frustums is created, the function determining
//! selection iterates through triangular frustum set and searches for overlap with any frustum.
class SelectMgr_TriangularFrustumSet : public SelectMgr_BaseFrustum
{
public:

  //! Auxiliary structure to define selection polyline
  struct SelectionPolyline
  {
    Handle(TColgp_HArray1OfPnt2d) Points;
  };

public:

  //! Constructor.
  SelectMgr_TriangularFrustumSet();

  //! Destructor.
  Standard_EXPORT virtual ~SelectMgr_TriangularFrustumSet();

  //! Initializes set of triangular frustums by polyline
  Standard_EXPORT void Init (const TColgp_Array1OfPnt2d& thePoints);

  //! Meshes polygon bounded by polyline. Than organizes a set of triangular frustums,
  //! where each triangle's projection onto near and far view frustum planes is considered as a frustum base
  //! NOTE: it should be called after Init() method
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;

  //! Returns FALSE (not applicable to this volume).
  virtual Standard_Boolean IsScalable() const Standard_OVERRIDE { return false; }

  //! Returns a copy of the frustum with all sub-volumes transformed according to the matrix given
  Standard_EXPORT virtual Handle(SelectMgr_BaseIntersector) ScaleAndTransform (const Standard_Integer theScale,
                                                                               const gp_GTrsf& theTrsf,
                                                                               const Handle(SelectMgr_FrustumBuilder)& theBuilder) const Standard_OVERRIDE;

public:

  Standard_EXPORT virtual Standard_Boolean OverlapsBox (const SelectMgr_Vec3& theMinPnt,
                                                        const SelectMgr_Vec3& theMaxPnt,
                                                        const SelectMgr_ViewClipRange& theClipRange,
                                                        SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OverlapsBox (const SelectMgr_Vec3& theMinPnt,
                                                        const SelectMgr_Vec3& theMaxPnt,
                                                        Standard_Boolean* theInside) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OverlapsPoint (const gp_Pnt& thePnt,
                                                          const SelectMgr_ViewClipRange& theClipRange,
                                                          SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Always returns FALSE (not applicable to this selector).
  virtual Standard_Boolean OverlapsPoint (const gp_Pnt& ) const Standard_OVERRIDE
  {
    return Standard_False;
  }

  Standard_EXPORT virtual Standard_Boolean OverlapsPolygon (const TColgp_Array1OfPnt& theArrayOfPnts,
                                                            Select3D_TypeOfSensitivity theSensType,
                                                            const SelectMgr_ViewClipRange& theClipRange,
                                                            SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OverlapsSegment (const gp_Pnt& thePnt1,
                                                            const gp_Pnt& thePnt2,
                                                            const SelectMgr_ViewClipRange& theClipRange,
                                                            SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean OverlapsTriangle (const gp_Pnt& thePnt1,
                                                             const gp_Pnt& thePnt2,
                                                             const gp_Pnt& thePnt3,
                                                             Select3D_TypeOfSensitivity theSensType,
                                                             const SelectMgr_ViewClipRange& theClipRange,
                                                             SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

public:

  //! Calculates the point on a view ray that was detected during the run of selection algo by given depth
  Standard_EXPORT virtual gp_Pnt DetectedPoint (const Standard_Real theDepth) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere (const gp_Pnt& theCenter,
                                                           const Standard_Real theRadius,
                                                           Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere (const gp_Pnt& theCenter,
                                                           const Standard_Real theRadius,
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses theBottomRad
  //! and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder (const Standard_Real theBottomRad,
                                                             const Standard_Real theTopRad,
                                                             const Standard_Real theHeight,
                                                             const gp_Trsf& theTrsf,
                                                             const SelectMgr_ViewClipRange& theClipRange,
                                                             SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses theBottomRad
  //! and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder (const Standard_Real theBottomRad,
                                                             const Standard_Real theTopRad,
                                                             const Standard_Real theHeight,
                                                             const gp_Trsf& theTrsf,
                                                             Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT virtual void GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const Standard_OVERRIDE;

  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the algorithm will
  //! mark both included and overlapped entities as matched
  Standard_EXPORT virtual void SetAllowOverlapDetection (const Standard_Boolean theIsToAllow);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  //! Checks whether the segment intersects with the boundary of the current volume selection
  Standard_EXPORT Standard_Boolean isIntersectBoundary (const gp_Pnt& thePnt1, const gp_Pnt& thePnt2) const;

  //! Checks whether the triangle intersects with a segment
  Standard_EXPORT Standard_Boolean segmentTriangleIntersection (const gp_Pnt &theOrig, const gp_Vec& theDir,
                                                                const gp_Pnt& theV1, const gp_Pnt& theV2, const gp_Pnt& theV3) const;

private:

  SelectMgr_TriangFrustums      myFrustums;          //!< set of triangular frustums
  SelectionPolyline             mySelPolyline;       //!< parameters of selection polyline (it is used to build triangle frustum set)
  TColgp_Array1OfPnt            myBoundaryPoints;    //!< boundary points
  Standard_Boolean              myToAllowOverlap;    //!< flag to detect only fully included sensitives or not
};

#endif // _SelectMgr_TriangularFrustumSet_HeaderFile
