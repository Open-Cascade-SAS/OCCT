// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _SelectMgr_BaseIntersector_HeaderFile
#define _SelectMgr_BaseIntersector_HeaderFile

#include <gp_GTrsf.hxx>
#include <NCollection_Mat4.hxx>
#include <Standard_TypeDef.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Vector.hxx>
#include <Select3D_TypeOfSensitivity.hxx>
#include <SelectBasics_PickResult.hxx>
#include <SelectMgr_SelectionType.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>

class Graphic3d_Camera;
class SelectMgr_FrustumBuilder;
class SelectMgr_ViewClipRange;

//! This class is an interface for different types of selecting intersector,
//! defining different selection types, like point, box or polyline
//! selection. It contains signatures of functions for detection of
//! overlap by sensitive entity and initializes some data for building
//! the selecting intersector
class SelectMgr_BaseIntersector : public Standard_Transient
{
public:
  //! Creates new empty selecting volume
  Standard_EXPORT SelectMgr_BaseIntersector();

  //! Destructor
  Standard_EXPORT ~SelectMgr_BaseIntersector() override;

  //! Builds intersector according to internal parameters
  virtual void Build() = 0;

  //! Returns selection type of this intersector
  SelectMgr_SelectionType GetSelectionType() const { return mySelectionType; }

public:
  //! Checks if it is possible to scale this intersector.
  virtual bool IsScalable() const = 0;

  //! Sets pixel tolerance.
  //! It makes sense only for scalable intersectors (built on a single point).
  //! This method does nothing for the base class.
  Standard_EXPORT virtual void SetPixelTolerance(const int theTol);

  //! Note that this method does not perform any checks on type of the frustum.
  //! @param[in] theScaleFactor  scale factor for new intersector or negative value if undefined;
  //!                            IMPORTANT: scaling makes sense only for scalable ::IsScalable()
  //!                            intersectors (built on a single point)!
  //! @param[in] theTrsf  transformation for new intersector or gp_Identity if undefined
  //! @param[in] theBuilder  an optional argument that represents corresponding settings for
  //! re-constructing transformed frustum from scratch;
  //!                        could be NULL if reconstruction is not expected furthermore
  //! @return a copy of the frustum resized according to the scale factor given and transforms it
  //! using the matrix given
  virtual occ::handle<SelectMgr_BaseIntersector> ScaleAndTransform(
    const int                                    theScaleFactor,
    const gp_GTrsf&                              theTrsf,
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const = 0;

  //! @param[in] theBuilder  argument that represents corresponding settings for re-constructing
  //! transformed frustum from scratch;
  //!                        should NOT be NULL.
  //! @return a copy of the frustum with the input builder assigned
  virtual occ::handle<SelectMgr_BaseIntersector> CopyWithBuilder(
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const = 0;

public:
  //! Return camera definition.
  const occ::handle<Graphic3d_Camera>& Camera() const { return myCamera; }

  //! Saves camera definition.
  Standard_EXPORT virtual void SetCamera(const occ::handle<Graphic3d_Camera>& theCamera);

  //! Returns current window size.
  //! This method doesn't set any output values for the base class.
  Standard_EXPORT virtual void WindowSize(int& theWidth, int& theHeight) const;

  //! Sets current window size.
  //! This method does nothing for the base class.
  Standard_EXPORT virtual void SetWindowSize(const int theWidth, const int theHeight);

  //! Sets viewport parameters.
  //! This method does nothing for the base class.
  Standard_EXPORT virtual void SetViewport(const double theX,
                                           const double theY,
                                           const double theWidth,
                                           const double theHeight);

  //! Returns near point of intersector.
  //! This method returns zero point for the base class.
  Standard_EXPORT virtual const gp_Pnt& GetNearPnt() const;

  //! Returns far point of intersector.
  //! This method returns zero point for the base class.
  Standard_EXPORT virtual const gp_Pnt& GetFarPnt() const;

  //! Returns direction ray of intersector.
  //! This method returns zero direction for the base class.
  Standard_EXPORT virtual const gp_Dir& GetViewRayDirection() const;

  //! Returns current mouse coordinates.
  //! This method returns infinite point for the base class.
  Standard_EXPORT virtual const gp_Pnt2d& GetMousePosition() const;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector.
  //! This method only clears input vector for the base class.
  virtual void GetPlanes(NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const
  {
    thePlaneEquations.Clear();
  }

public:
  //! SAT intersection test between defined volume and given axis-aligned box
  virtual bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                           const NCollection_Vec3<double>& theBoxMax,
                           const SelectMgr_ViewClipRange&  theClipRange,
                           SelectBasics_PickResult&        thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  virtual bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                           const NCollection_Vec3<double>& theBoxMax,
                           bool*                           theInside = nullptr) const = 0;

  //! Intersection test between defined volume and given point
  virtual bool OverlapsPoint(const gp_Pnt&                  thePnt,
                             const SelectMgr_ViewClipRange& theClipRange,
                             SelectBasics_PickResult&       thePickResult) const = 0;

  //! Intersection test between defined volume and given point
  //! Does not perform depth calculation, so this method is defined as helper function for inclusion
  //! test. Therefore, its implementation makes sense only for rectangular frustum with box
  //! selection mode activated.
  virtual bool OverlapsPoint(const gp_Pnt& thePnt) const = 0;

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  virtual bool OverlapsPolygon(const NCollection_Array1<gp_Pnt>& theArrayOfPnts,
                               Select3D_TypeOfSensitivity        theSensType,
                               const SelectMgr_ViewClipRange&    theClipRange,
                               SelectBasics_PickResult&          thePickResult) const = 0;

  //! Checks if line segment overlaps selecting frustum
  virtual bool OverlapsSegment(const gp_Pnt&                  thePnt1,
                               const gp_Pnt&                  thePnt2,
                               const SelectMgr_ViewClipRange& theClipRange,
                               SelectBasics_PickResult&       thePickResult) const = 0;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  virtual bool OverlapsTriangle(const gp_Pnt&                  thePnt1,
                                const gp_Pnt&                  thePnt2,
                                const gp_Pnt&                  thePnt3,
                                Select3D_TypeOfSensitivity     theSensType,
                                const SelectMgr_ViewClipRange& theClipRange,
                                SelectBasics_PickResult&       thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual bool OverlapsSphere(const gp_Pnt& theCenter,
                                              const double  theRadius,
                                              bool*         theInside = nullptr) const = 0;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual bool OverlapsSphere(const gp_Pnt&                  theCenter,
                                              const double                   theRadius,
                                              const SelectMgr_ViewClipRange& theClipRange,
                                              SelectBasics_PickResult& thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  virtual bool OverlapsCylinder(const double                   theBottomRad,
                                const double                   theTopRad,
                                const double                   theHeight,
                                const gp_Trsf&                 theTrsf,
                                const bool                     theIsHollow,
                                const SelectMgr_ViewClipRange& theClipRange,
                                SelectBasics_PickResult&       thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  virtual bool OverlapsCylinder(const double   theBottomRad,
                                const double   theTopRad,
                                const double   theHeight,
                                const gp_Trsf& theTrsf,
                                const bool     theIsHollow,
                                bool*          theInside = nullptr) const = 0;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  virtual bool OverlapsCircle(const double                   theBottomRad,
                              const gp_Trsf&                 theTrsf,
                              const bool                     theIsFilled,
                              const SelectMgr_ViewClipRange& theClipRange,
                              SelectBasics_PickResult&       thePickResult) const = 0;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  virtual bool OverlapsCircle(const double   theBottomRad,
                              const gp_Trsf& theTrsf,
                              const bool     theIsFilled,
                              bool*          theInside = nullptr) const = 0;

public:
  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG.
  //! It makes sense only for intersectors built on a single point.
  //! This method returns infinite value for the base class.
  Standard_EXPORT virtual double DistToGeometryCenter(const gp_Pnt& theCOG) const;

  //! Calculates the point on a view ray that was detected during the run of selection algo by given
  //! depth. It makes sense only for intersectors built on a single point. This method returns
  //! infinite point for the base class.
  Standard_EXPORT virtual gp_Pnt DetectedPoint(const double theDepth) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  //! Checks whether the ray that starts at the point theLoc and directs with the direction
  //! theRayDir intersects with the sphere with center at theCenter and radius TheRadius
  Standard_EXPORT virtual bool RaySphereIntersection(const gp_Pnt& theCenter,
                                                     const double  theRadius,
                                                     const gp_Pnt& theLoc,
                                                     const gp_Dir& theRayDir,
                                                     double&       theTimeEnter,
                                                     double&       theTimeLeave) const;

  //! Checks whether the ray that starts at the point theLoc and directs with the direction
  //! theRayDir intersects with the hollow cylinder (or cone)
  //! @param[in]  theBottomRadius the bottom cylinder radius
  //! @param[in]  theTopRadius    the top cylinder radius
  //! @param[in]  theHeight       the cylinder height
  //! @param[in]  theLoc          the location of the ray
  //! @param[in]  theRayDir       the ray direction
  //! @param[in]  theIsHollow     true if the cylinder is hollow
  //! @param[out] theTimeEnter    the entering the intersection
  //! @param[out] theTimeLeave    the leaving the intersection
  Standard_EXPORT virtual bool RayCylinderIntersection(const double  theBottomRadius,
                                                       const double  theTopRadius,
                                                       const double  theHeight,
                                                       const gp_Pnt& theLoc,
                                                       const gp_Dir& theRayDir,
                                                       const bool    theIsHollow,
                                                       double&       theTimeEnter,
                                                       double&       theTimeLeave) const;

  //! Checks whether the ray that starts at the point theLoc and directs with the direction
  //! theRayDir intersects with the circle
  //! @param[in]  theRadius   the circle radius
  //! @param[in]  theLoc      the location of the ray
  //! @param[in]  theRayDir   the ray direction
  //! @param[in]  theIsFilled true if it's a circle, false if it's a circle outline
  //! @param[out] theTime     the intersection
  Standard_EXPORT virtual bool RayCircleIntersection(const double  theRadius,
                                                     const gp_Pnt& theLoc,
                                                     const gp_Dir& theRayDir,
                                                     const bool    theIsFilled,
                                                     double&       theTime) const;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_BaseIntersector, Standard_Transient)

protected:
  // clang-format off
  occ::handle<Graphic3d_Camera> myCamera;        //!< camera definition (if builder isn't NULL it is the same as its camera)
  // clang-format on
  SelectMgr_SelectionType mySelectionType; //!< type of selection
};

#endif // _SelectMgr_BaseIntersector_HeaderFile
