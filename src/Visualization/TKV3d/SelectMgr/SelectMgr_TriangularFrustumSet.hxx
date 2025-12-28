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
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! This class is used to handle polyline selection. The main principle of polyline selection
//! algorithm is to split the polygon defined by polyline onto triangles.
//! Than each of them is considered as a base for triangular frustum building.
//! In other words, each triangle vertex will be projected from 2d screen space to 3d world space
//! onto near and far view frustum planes. Thus, the projected triangles make up the bases of
//! selecting frustum. When the set of such frustums is created, the function determining selection
//! iterates through triangular frustum set and searches for overlap with any frustum.
class SelectMgr_TriangularFrustumSet : public SelectMgr_BaseFrustum
{
public:
  //! Auxiliary structure to define selection polyline
  struct SelectionPolyline
  {
    occ::handle<NCollection_HArray1<gp_Pnt2d>> Points;
  };

public:
  //! Constructor.
  SelectMgr_TriangularFrustumSet();

  //! Destructor.
  Standard_EXPORT ~SelectMgr_TriangularFrustumSet() override;

  //! Initializes set of triangular frustums by polyline
  Standard_EXPORT void Init(const NCollection_Array1<gp_Pnt2d>& thePoints);

  //! Meshes polygon bounded by polyline. Than organizes a set of triangular frustums,
  //! where each triangle's projection onto near and far view frustum planes is considered as a
  //! frustum base NOTE: it should be called after Init() method
  Standard_EXPORT void Build() override;

  //! Returns FALSE (not applicable to this volume).
  bool IsScalable() const override { return false; }

  //! Returns a copy of the frustum with all sub-volumes transformed according to the matrix given
  Standard_EXPORT occ::handle<SelectMgr_BaseIntersector> ScaleAndTransform(
    const int                                    theScale,
    const gp_GTrsf&                              theTrsf,
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const override;

  //! Returns a copy of the frustum using the given frustum builder configuration.
  //! Returned frustum should be re-constructed before being used.
  //! @param[in] theBuilder  argument that represents corresponding settings for re-constructing
  //! transformed frustum from scratch;
  //!                        should NOT be NULL.
  //! @return a copy of the frustum with the input builder assigned
  Standard_EXPORT occ::handle<SelectMgr_BaseIntersector> CopyWithBuilder(
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const override;

public:
  Standard_EXPORT bool OverlapsBox(const NCollection_Vec3<double>& theMinPnt,
                                           const NCollection_Vec3<double>& theMaxPnt,
                                           const SelectMgr_ViewClipRange&  theClipRange,
                                           SelectBasics_PickResult& thePickResult) const override;

  Standard_EXPORT bool OverlapsBox(const NCollection_Vec3<double>& theMinPnt,
                                           const NCollection_Vec3<double>& theMaxPnt,
                                           bool* theInside) const override;

  Standard_EXPORT bool OverlapsPoint(const gp_Pnt&                  thePnt,
                                             const SelectMgr_ViewClipRange& theClipRange,
                                             SelectBasics_PickResult& thePickResult) const override;

  //! Always returns FALSE (not applicable to this selector).
  bool OverlapsPoint(const gp_Pnt&) const override { return false; }

  Standard_EXPORT bool OverlapsPolygon(
    const NCollection_Array1<gp_Pnt>& theArrayOfPnts,
    Select3D_TypeOfSensitivity        theSensType,
    const SelectMgr_ViewClipRange&    theClipRange,
    SelectBasics_PickResult&          thePickResult) const override;

  Standard_EXPORT bool OverlapsSegment(
    const gp_Pnt&                  thePnt1,
    const gp_Pnt&                  thePnt2,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  Standard_EXPORT bool OverlapsTriangle(
    const gp_Pnt&                  thePnt1,
    const gp_Pnt&                  thePnt2,
    const gp_Pnt&                  thePnt3,
    Select3D_TypeOfSensitivity     theSensType,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

public:
  //! Calculates the point on a view ray that was detected during the run of selection algo by given
  //! depth
  Standard_EXPORT gp_Pnt DetectedPoint(const double theDepth) const override;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT bool OverlapsSphere(const gp_Pnt& theCenter,
                                              const double  theRadius,
                                              bool*         theInside = nullptr) const override;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT bool OverlapsSphere(
    const gp_Pnt&                  theCenter,
    const double                   theRadius,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT bool OverlapsCylinder(
    const double                   theBottomRad,
    const double                   theTopRad,
    const double                   theHeight,
    const gp_Trsf&                 theTrsf,
    const bool                     theIsHollow,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT bool OverlapsCylinder(const double   theBottomRad,
                                                const double   theTopRad,
                                                const double   theHeight,
                                                const gp_Trsf& theTrsf,
                                                const bool     theIsHollow,
                                                bool*          theInside = nullptr) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT bool OverlapsCircle(
    const double                   theBottomRad,
    const gp_Trsf&                 theTrsf,
    const bool                     theIsFilled,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT bool OverlapsCircle(const double   theBottomRad,
                                              const gp_Trsf& theTrsf,
                                              const bool     theIsFilled,
                                              bool*          theInside = nullptr) const override;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT void GetPlanes(
    NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const override;

  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the
  //! algorithm will mark both included and overlapped entities as matched
  Standard_EXPORT virtual void SetAllowOverlapDetection(const bool theIsToAllow);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

private:
  //! Checks whether the segment intersects with the boundary of the current volume selection
  Standard_EXPORT bool isIntersectBoundary(const gp_Pnt& thePnt1, const gp_Pnt& thePnt2) const;

  //! Checks whether the circle intersects with the boundary of the current volume selection
  Standard_EXPORT bool isIntersectBoundary(const double   theRadius,
                                           const gp_Trsf& theTrsf,
                                           const bool     theIsFilled) const;

  //! Checks whether the triangle intersects with a segment
  Standard_EXPORT static bool segmentTriangleIntersection(const gp_Pnt& theOrig,
                                                          const gp_Vec& theDir,
                                                          const gp_Pnt& theV1,
                                                          const gp_Pnt& theV2,
                                                          const gp_Pnt& theV3);

  Standard_EXPORT static bool segmentSegmentIntersection(const gp_Pnt& theStartPnt1,
                                                         const gp_Pnt& theEndPnt1,
                                                         const gp_Pnt& theStartPnt2,
                                                         const gp_Pnt& theEndPnt2);

  Standard_EXPORT static bool pointInTriangle(const gp_Pnt& thePnt,
                                              const gp_Pnt& theV1,
                                              const gp_Pnt& theV2,
                                              const gp_Pnt& theV3);

private:
  NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>
    myFrustums; //!< set of triangular frustums
  // clang-format off
  SelectionPolyline             mySelPolyline;       //!< parameters of selection polyline (it is used to build triangle frustum set)
  NCollection_Array1<gp_Pnt>            myBoundaryPoints;    //!< boundary points
                                                     //!       1_____2
                                                     //!      /|     |\ .
                                                     //!    4/_|_____|_\3
                                                     //!    | 5|_____|6 |
                                                     //!    | /       \ |
                                                     //!   8|/_________\|7
  bool              myToAllowOverlap;    //!< flag to detect only fully included sensitives or not
  // clang-format on
};

#endif // _SelectMgr_TriangularFrustumSet_HeaderFile
