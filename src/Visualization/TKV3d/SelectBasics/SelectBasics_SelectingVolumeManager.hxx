// Created on: 2014-08-21
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

#ifndef _SelectBasics_SelectingVolumeManager_HeaderFile
#define _SelectBasics_SelectingVolumeManager_HeaderFile

#include <BVH_Box.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <SelectBasics_PickResult.hxx>
#include <SelectMgr_SelectionType.hxx>

class gp_Pnt;

//! This class provides an interface for selecting volume manager,
//! which is responsible for all overlap detection methods and
//! calculation of minimum depth, distance to center of geometry
//! and detected closest point on entity.
class SelectBasics_SelectingVolumeManager
{
public:
  //! Empty constructor.
  SelectBasics_SelectingVolumeManager();

  //! Destructor.
  Standard_EXPORT virtual ~SelectBasics_SelectingVolumeManager();

  //! Return selection type.
  virtual int GetActiveSelectionType() const = 0;

public:
  //! Returns true if selecting volume is overlapped by box theBox
  virtual bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                           const NCollection_Vec3<double>& theBoxMax,
                           SelectBasics_PickResult&        thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box with minimum
  //! corner at point theMinPt and maximum at point theMaxPt
  virtual bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                           const NCollection_Vec3<double>& theBoxMax,
                           bool*                           theInside = NULL) const = 0;

  //! Returns true if selecting volume is overlapped by point thePnt
  virtual bool OverlapsPoint(const gp_Pnt&            thePnt,
                             SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by point thePnt.
  //! Does not perform depth calculation, so this method is defined as
  //! helper function for inclusion test.
  virtual bool OverlapsPoint(const gp_Pnt& thePnt) const = 0;

  //! Returns true if selecting volume is overlapped by planar convex polygon, which points
  //! are stored in theArrayOfPts, taking into account sensitivity type theSensType
  virtual bool OverlapsPolygon(const NCollection_Array1<gp_Pnt>& theArrayOfPts,
                               int                               theSensType,
                               SelectBasics_PickResult&          thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by line segment with start point at thePt1
  //! and end point at thePt2
  virtual bool OverlapsSegment(const gp_Pnt&            thePt1,
                               const gp_Pnt&            thePt2,
                               SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by triangle with vertices thePt1,
  //! thePt2 and thePt3, taking into account sensitivity type theSensType
  virtual bool OverlapsTriangle(const gp_Pnt&            thePt1,
                                const gp_Pnt&            thePt2,
                                const gp_Pnt&            thePt3,
                                int                      theSensType,
                                SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  virtual bool OverlapsSphere(const gp_Pnt&            theCenter,
                              const double             theRadius,
                              SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  virtual bool OverlapsSphere(const gp_Pnt& theCenter,
                              const double  theRadius,
                              bool*         theInside = NULL) const = 0;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight, the boolean theIsHollow and transformation to
  //! apply theTrsf.
  virtual bool OverlapsCylinder(const double             theBottomRad,
                                const double             theTopRad,
                                const double             theHeight,
                                const gp_Trsf&           theTrsf,
                                const bool               theIsHollow,
                                SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight, the boolean theIsHollow and transformation to
  //! apply theTrsf.
  virtual bool OverlapsCylinder(const double   theBottomRad,
                                const double   theTopRad,
                                const double   theHeight,
                                const gp_Trsf& theTrsf,
                                const bool     theIsHollow,
                                bool*          theInside = NULL) const = 0;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! the boolean theIsFilled, and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  virtual bool OverlapsCircle(const double             theRadius,
                              const gp_Trsf&           theTrsf,
                              const bool               theIsFilled,
                              SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! the boolean theIsFilled, and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  virtual bool OverlapsCircle(const double   theRadius,
                              const gp_Trsf& theTrsf,
                              const bool     theIsFilled,
                              bool*          theInside = NULL) const = 0;

public:
  //! Calculates distance from 3d projection of user-defined selection point
  //! to the given point theCOG
  virtual double DistToGeometryCenter(const gp_Pnt& theCOG) const = 0;

  //! Return 3D point corresponding to specified depth within picking ray.
  virtual gp_Pnt DetectedPoint(const double theDepth) const = 0;

  //! Returns flag indicating if partial overlapping of entities is allowed or should be rejected.
  virtual bool IsOverlapAllowed() const = 0;

  //! Valid only for point and rectangular selection.
  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto near view frustum plane
  virtual gp_Pnt GetNearPickedPnt() const = 0;

  //! Valid only for point and rectangular selection.
  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto far view frustum plane
  virtual gp_Pnt GetFarPickedPnt() const = 0;

  //! Valid only for point and rectangular selection.
  //! Returns view ray direction
  virtual gp_Dir GetViewRayDirection() const = 0;

  //! Checks if it is possible to scale current active selecting volume
  virtual bool IsScalableActiveVolume() const = 0;

  //! Returns mouse coordinates for Point selection mode.
  //! @return infinite point in case of unsupport of mouse position for this active selection
  //! volume.
  virtual gp_Pnt2d GetMousePosition() const = 0;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  virtual void GetPlanes(NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const = 0;

  //! Dumps the content of me into the stream
  virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const
  {
    (void)theOStream;
    (void)theDepth;
  }

public:
  Standard_DEPRECATED("Deprecated alias for OverlapsBox()")
  bool Overlaps(const NCollection_Vec3<double>& theBoxMin,
                const NCollection_Vec3<double>& theBoxMax,
                SelectBasics_PickResult&        thePickResult) const
  {
    return OverlapsBox(theBoxMin, theBoxMax, thePickResult);
  }

  Standard_DEPRECATED("Deprecated alias for OverlapsBox()")
  bool Overlaps(const NCollection_Vec3<double>& theBoxMin,
                const NCollection_Vec3<double>& theBoxMax,
                bool*                           theInside = NULL) const
  {
    return OverlapsBox(theBoxMin, theBoxMax, theInside);
  }

  Standard_DEPRECATED("Deprecated alias for OverlapsPoint()")
  bool Overlaps(const gp_Pnt& thePnt, SelectBasics_PickResult& thePickResult) const
  {
    return OverlapsPoint(thePnt, thePickResult);
  }

  Standard_DEPRECATED("Deprecated alias for OverlapsPoint()")
  bool Overlaps(const gp_Pnt& thePnt) const { return OverlapsPoint(thePnt); }

  Standard_DEPRECATED("Deprecated alias for OverlapsPolygon()")
  Standard_EXPORT bool Overlaps(const occ::handle<NCollection_HArray1<gp_Pnt>>& theArrayOfPts,
                                int                                             theSensType,
                                SelectBasics_PickResult& thePickResult) const;

  Standard_DEPRECATED("Deprecated alias for OverlapsPolygon()")
  bool Overlaps(const NCollection_Array1<gp_Pnt>& theArrayOfPts,
                int                               theSensType,
                SelectBasics_PickResult&          thePickResult) const
  {
    return OverlapsPolygon(theArrayOfPts, theSensType, thePickResult);
  }

  Standard_DEPRECATED("Deprecated alias for OverlapsSegment()")
  bool Overlaps(const gp_Pnt&            thePnt1,
                const gp_Pnt&            thePnt2,
                SelectBasics_PickResult& thePickResult) const
  {
    return OverlapsSegment(thePnt1, thePnt2, thePickResult);
  }

  Standard_DEPRECATED("Deprecated alias for OverlapsTriangle()")
  bool Overlaps(const gp_Pnt&            thePnt1,
                const gp_Pnt&            thePnt2,
                const gp_Pnt&            thePnt3,
                int                      theSensType,
                SelectBasics_PickResult& thePickResult) const
  {
    return OverlapsTriangle(thePnt1, thePnt2, thePnt3, theSensType, thePickResult);
  }

//! Deprecated static const class members aren't supported for Visual Studio prior to 2015 (vc14)
//! and GCC >= 5.0 and GCC < 6.3 (due to bug when warning was raised without member usage).
#if (!defined(_MSC_VER) || (_MSC_VER >= 1900))                                                     \
  && (!defined(__GNUC__) || (__GNUC__ != 5 && (__GNUC__ != 6 || __GNUC_MINOR__ >= 3)))
  Standard_DEPRECATED("Deprecated alias - SelectMgr_SelectionType should be used instead")
  static const SelectMgr_SelectionType Point = SelectMgr_SelectionType_Point;

  Standard_DEPRECATED("Deprecated alias - SelectMgr_SelectionType should be used instead")
  static const SelectMgr_SelectionType Box = SelectMgr_SelectionType_Box;

  Standard_DEPRECATED("Deprecated alias - SelectMgr_SelectionType should be used instead")
  static const SelectMgr_SelectionType Polyline = SelectMgr_SelectionType_Polyline;

  Standard_DEPRECATED("Deprecated alias - SelectMgr_SelectionType should be used instead")
  static const SelectMgr_SelectionType Unknown = SelectMgr_SelectionType_Unknown;
#endif
};

#endif // _SelectBasics_SelectingVolumeManager_HeaderFile
