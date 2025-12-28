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

#include <SelectMgr_SelectingVolumeManager.hxx>

#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <SelectMgr_AxisIntersector.hxx>
#include <SelectMgr_RectangularFrustum.hxx>
#include <SelectMgr_TriangularFrustumSet.hxx>

#include <BVH_Tools.hxx>
#include <Standard_Dump.hxx>

//=======================================================================
// function : SelectMgr_SelectingVolumeManager
// purpose  : Creates instances of all available selecting volume types
//=======================================================================
SelectMgr_SelectingVolumeManager::SelectMgr_SelectingVolumeManager()
    : myActiveSelectingVolume(nullptr),
      myToAllowOverlap(false)
{
}

//=======================================================================
// function : ScaleAndTransform
// purpose  : IMPORTANT: Scaling makes sense only for frustum built on a single point!
//            Note that this method does not perform any checks on type of the frustum.
//
//            Returns a copy of the frustum resized according to the scale factor given
//            and transforms it using the matrix given.
//            There are no default parameters, but in case if:
//                - transformation only is needed: @theScaleFactor must be initialized
//                  as any negative value;
//                - scale only is needed: @theTrsf must be set to gp_Identity.
//            Builder is an optional argument that represents corresponding settings for
//            re-constructing transformed frustum from scratch. Can be null if reconstruction
//            is not needed furthermore in the code.
//=======================================================================
SelectMgr_SelectingVolumeManager SelectMgr_SelectingVolumeManager::ScaleAndTransform(
  const int                                    theScaleFactor,
  const gp_GTrsf&                              theTrsf,
  const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const
{
  SelectMgr_SelectingVolumeManager aMgr;
  if (myActiveSelectingVolume.IsNull())
  {
    return aMgr;
  }

  aMgr.myActiveSelectingVolume =
    myActiveSelectingVolume->ScaleAndTransform(theScaleFactor, theTrsf, theBuilder);
  aMgr.myToAllowOverlap   = myToAllowOverlap;
  aMgr.myViewClipPlanes   = myViewClipPlanes;
  aMgr.myObjectClipPlanes = myObjectClipPlanes;
  aMgr.myViewClipRange    = myViewClipRange;

  return aMgr;
}

//=======================================================================
// function : CopyWithBuilder
// purpose  : Returns a copy of the selecting volume manager and its active frustum re-constructed
// using the passed builder.
//            Builder is an argument that represents corresponding settings for re-constructing
//            transformed frustum from scratch.
//=======================================================================
SelectMgr_SelectingVolumeManager SelectMgr_SelectingVolumeManager::CopyWithBuilder(
  const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const
{
  SelectMgr_SelectingVolumeManager aMgr;
  aMgr.myToAllowOverlap   = myToAllowOverlap;
  aMgr.myViewClipPlanes   = myViewClipPlanes;
  aMgr.myObjectClipPlanes = myObjectClipPlanes;
  aMgr.myViewClipRange    = myViewClipRange;
  if (!myActiveSelectingVolume.IsNull())
  {
    aMgr.myActiveSelectingVolume = myActiveSelectingVolume->CopyWithBuilder(theBuilder);
    aMgr.BuildSelectingVolume();
  }

  return aMgr;
}

//=================================================================================================

int SelectMgr_SelectingVolumeManager::GetActiveSelectionType() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return SelectMgr_SelectionType_Unknown;
  }
  return myActiveSelectingVolume->GetSelectionType();
}

//=================================================================================================

const occ::handle<Graphic3d_Camera>& SelectMgr_SelectingVolumeManager::Camera() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    static const occ::handle<Graphic3d_Camera> anEmptyCamera;
    return anEmptyCamera;
  }
  return myActiveSelectingVolume->Camera();
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::SetCamera(const occ::handle<Graphic3d_Camera>& theCamera)
{
  Standard_ASSERT_RAISE(!myActiveSelectingVolume.IsNull(),
                        "SelectMgr_SelectingVolumeManager::SetCamera() should be called after "
                        "initialization of selection volume ");
  myActiveSelectingVolume->SetCamera(theCamera);
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::WindowSize(int& theWidth, int& theHeight) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return;
  }
  myActiveSelectingVolume->WindowSize(theWidth, theHeight);
}

//=======================================================================
// function : SetWindowSize
// purpose  : Updates window size in all selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetWindowSize(const int theWidth, const int theHeight)
{
  Standard_ASSERT_RAISE(!myActiveSelectingVolume.IsNull(),
                        "SelectMgr_SelectingVolumeManager::SetWindowSize() should be called after "
                        "initialization of selection volume ");
  myActiveSelectingVolume->SetWindowSize(theWidth, theHeight);
}

//=======================================================================
// function : SetCamera
// purpose  : Updates viewport in all selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetViewport(const double theX,
                                                   const double theY,
                                                   const double theWidth,
                                                   const double theHeight)
{
  Standard_ASSERT_RAISE(!myActiveSelectingVolume.IsNull(),
                        "SelectMgr_SelectingVolumeManager::SetViewport() should be called after "
                        "initialization of selection volume ");
  myActiveSelectingVolume->SetViewport(theX, theY, theWidth, theHeight);
}

//=======================================================================
// function : SetPixelTolerance
// purpose  : Updates pixel tolerance in all selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetPixelTolerance(const int theTolerance)
{
  Standard_ASSERT_RAISE(!myActiveSelectingVolume.IsNull(),
                        "SelectMgr_SelectingVolumeManager::SetPixelTolerance() should be called "
                        "after initialization of selection volume ");
  myActiveSelectingVolume->SetPixelTolerance(theTolerance);
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::InitPointSelectingVolume(const gp_Pnt2d& thePoint)
{
  occ::handle<SelectMgr_RectangularFrustum> aPntVolume =
    occ::down_cast<SelectMgr_RectangularFrustum>(myActiveSelectingVolume);
  if (aPntVolume.IsNull())
  {
    aPntVolume = new SelectMgr_RectangularFrustum();
  }
  aPntVolume->Init(thePoint);
  myActiveSelectingVolume = aPntVolume;
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::InitBoxSelectingVolume(const gp_Pnt2d& theMinPt,
                                                              const gp_Pnt2d& theMaxPt)
{
  occ::handle<SelectMgr_RectangularFrustum> aBoxVolume =
    occ::down_cast<SelectMgr_RectangularFrustum>(myActiveSelectingVolume);
  if (aBoxVolume.IsNull())
  {
    aBoxVolume = new SelectMgr_RectangularFrustum();
  }
  aBoxVolume->Init(theMinPt, theMaxPt);
  myActiveSelectingVolume = aBoxVolume;
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::InitPolylineSelectingVolume(
  const NCollection_Array1<gp_Pnt2d>& thePoints)
{
  occ::handle<SelectMgr_TriangularFrustumSet> aPolylineVolume =
    occ::down_cast<SelectMgr_TriangularFrustumSet>(myActiveSelectingVolume);
  if (aPolylineVolume.IsNull())
  {
    aPolylineVolume = new SelectMgr_TriangularFrustumSet();
  }
  aPolylineVolume->Init(thePoints);
  myActiveSelectingVolume = aPolylineVolume;
  aPolylineVolume->SetAllowOverlapDetection(IsOverlapAllowed());
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::InitAxisSelectingVolume(const gp_Ax1& theAxis)
{
  occ::handle<SelectMgr_AxisIntersector> anAxisVolume =
    occ::down_cast<SelectMgr_AxisIntersector>(myActiveSelectingVolume);
  if (anAxisVolume.IsNull())
  {
    anAxisVolume = new SelectMgr_AxisIntersector();
  }
  anAxisVolume->Init(theAxis);
  myActiveSelectingVolume = anAxisVolume;
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::InitSelectingVolume(
  const occ::handle<SelectMgr_BaseIntersector>& theVolume)
{
  myActiveSelectingVolume = theVolume;
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::BuildSelectingVolume()
{
  Standard_ASSERT_RAISE(!myActiveSelectingVolume.IsNull(),
                        "SelectMgr_SelectingVolumeManager::BuildSelectingVolume() should be called "
                        "after initialization of active selection volume.");
  myActiveSelectingVolume->Build();
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::BuildSelectingVolume(const gp_Pnt2d& thePoint)
{
  InitPointSelectingVolume(thePoint);
  myActiveSelectingVolume->Build();
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::BuildSelectingVolume(const gp_Pnt2d& theMinPt,
                                                            const gp_Pnt2d& theMaxPt)
{
  InitBoxSelectingVolume(theMinPt, theMaxPt);
  myActiveSelectingVolume->Build();
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::BuildSelectingVolume(
  const NCollection_Array1<gp_Pnt2d>& thePoints)
{
  InitPolylineSelectingVolume(thePoints);
  myActiveSelectingVolume->Build();
}

//=======================================================================
// function : OverlapsBox
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                                   const NCollection_Vec3<double>& theBoxMax,
                                                   SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsBox(theBoxMin, theBoxMax, myViewClipRange, thePickResult);
}

//=======================================================================
// function : OverlapsBox
// purpose  : Intersection test between defined volume and given point
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                                   const NCollection_Vec3<double>& theBoxMax,
                                                   bool*                           theInside) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsBox(theBoxMin, theBoxMax, theInside);
}

//=======================================================================
// function : OverlapsPoint
// purpose  : Intersection test between defined volume and given point
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsPoint(const gp_Pnt&            thePnt,
                                                     SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsPoint(thePnt, myViewClipRange, thePickResult);
}

//=======================================================================
// function : OverlapsPoint
// purpose  : Intersection test between defined volume and given point
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsPoint(const gp_Pnt& thePnt) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsPoint(thePnt);
}

//=======================================================================
// function : OverlapsPolygon
// purpose  : SAT intersection test between defined volume and given
//            ordered set of points, representing line segments. The test
//            may be considered of interior part or boundary line defined
//            by segments depending on given sensitivity type
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsPolygon(
  const NCollection_Array1<gp_Pnt>& theArrayOfPnts,
  int                               theSensType,
  SelectBasics_PickResult&          thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsPolygon(theArrayOfPnts,
                                                  (Select3D_TypeOfSensitivity)theSensType,
                                                  myViewClipRange,
                                                  thePickResult);
}

//=======================================================================
// function : OverlapsSegment
// purpose  : Checks if line segment overlaps selecting volume
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsSegment(const gp_Pnt&            thePt1,
                                                       const gp_Pnt&            thePt2,
                                                       SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsSegment(thePt1, thePt2, myViewClipRange, thePickResult);
}

//=======================================================================
// function : OverlapsTriangle
// purpose  : SAT intersection test between defined volume and given
//            triangle. The test may be considered of interior part or
//            boundary line defined by triangle vertices depending on
//            given sensitivity type
//=======================================================================
bool SelectMgr_SelectingVolumeManager::OverlapsTriangle(
  const gp_Pnt&            thePt1,
  const gp_Pnt&            thePt2,
  const gp_Pnt&            thePt3,
  int                      theSensType,
  SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }

  return myActiveSelectingVolume->OverlapsTriangle(thePt1,
                                                   thePt2,
                                                   thePt3,
                                                   (Select3D_TypeOfSensitivity)theSensType,
                                                   myViewClipRange,
                                                   thePickResult);
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::OverlapsSphere(const gp_Pnt&            theCenter,
                                                      const double             theRadius,
                                                      SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume->OverlapsSphere(theCenter,
                                                 theRadius,
                                                 myViewClipRange,
                                                 thePickResult);
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::OverlapsSphere(const gp_Pnt& theCenter,
                                                      const double  theRadius,
                                                      bool*         theInside) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume->OverlapsSphere(theCenter, theRadius, theInside);
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::OverlapsCylinder(
  const double             theBottomRad,
  const double             theTopRad,
  const double             theHeight,
  const gp_Trsf&           theTrsf,
  const bool               theIsHollow,
  SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume->OverlapsCylinder(theBottomRad,
                                                   theTopRad,
                                                   theHeight,
                                                   theTrsf,
                                                   theIsHollow,
                                                   myViewClipRange,
                                                   thePickResult);
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::OverlapsCylinder(const double   theBottomRad,
                                                        const double   theTopRad,
                                                        const double   theHeight,
                                                        const gp_Trsf& theTrsf,
                                                        const bool     theIsHollow,
                                                        bool*          theInside) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume
    ->OverlapsCylinder(theBottomRad, theTopRad, theHeight, theTrsf, theIsHollow, theInside);
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::OverlapsCircle(const double             theRadius,
                                                      const gp_Trsf&           theTrsf,
                                                      const bool               theIsFilled,
                                                      SelectBasics_PickResult& thePickResult) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume->OverlapsCircle(theRadius,
                                                 theTrsf,
                                                 theIsFilled,
                                                 myViewClipRange,
                                                 thePickResult);
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::OverlapsCircle(const double   theRadius,
                                                      const gp_Trsf& theTrsf,
                                                      const bool     theIsFilled,
                                                      bool*          theInside) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume->OverlapsCircle(theRadius, theTrsf, theIsFilled, theInside);
}

//=======================================================================
// function : DistToGeometryCenter
// purpose  : Measures distance between 3d projection of user-picked
//            screen point and given point theCOG
//=======================================================================
double SelectMgr_SelectingVolumeManager::DistToGeometryCenter(const gp_Pnt& theCOG) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return RealLast();
  }
  return myActiveSelectingVolume->DistToGeometryCenter(theCOG);
}

// =======================================================================
// function : DetectedPoint
// purpose  : Calculates the point on a view ray that was detected during
//            the run of selection algo by given depth. Is valid for point
//            selection only
// =======================================================================
gp_Pnt SelectMgr_SelectingVolumeManager::DetectedPoint(const double theDepth) const
{
  Standard_ASSERT_RAISE(!myActiveSelectingVolume.IsNull(),
                        "SelectMgr_SelectingVolumeManager::DetectedPoint() should be called after "
                        "initialization of selection volume");
  return myActiveSelectingVolume->DetectedPoint(theDepth);
}

//=======================================================================
// function : AllowOverlapDetection
// purpose  : If theIsToAllow is false, only fully included sensitives will
//            be detected, otherwise the algorithm will mark both included
//            and overlapped entities as matched
//=======================================================================
void SelectMgr_SelectingVolumeManager::AllowOverlapDetection(const bool theIsToAllow)
{
  myToAllowOverlap = theIsToAllow;
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::IsOverlapAllowed() const
{
  return myToAllowOverlap || GetActiveSelectionType() == SelectMgr_SelectionType_Point;
}

//=================================================================================================

const gp_Pnt* SelectMgr_SelectingVolumeManager::GetVertices() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return nullptr;
  }
  const SelectMgr_RectangularFrustum* aRectFrustum =
    static_cast<const SelectMgr_RectangularFrustum*>(myActiveSelectingVolume.get());
  if (aRectFrustum == nullptr)
  {
    return nullptr;
  }
  return aRectFrustum->GetVertices();
}

//=================================================================================================

gp_Pnt SelectMgr_SelectingVolumeManager::GetNearPickedPnt() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return gp_Pnt();
  }
  return myActiveSelectingVolume->GetNearPnt();
}

//=================================================================================================

gp_Pnt SelectMgr_SelectingVolumeManager::GetFarPickedPnt() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return gp_Pnt(RealLast(), RealLast(), RealLast());
  }
  return myActiveSelectingVolume->GetFarPnt();
}

//=================================================================================================

gp_Dir SelectMgr_SelectingVolumeManager::GetViewRayDirection() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return gp_Dir();
  }
  return myActiveSelectingVolume->GetViewRayDirection();
}

//=================================================================================================

bool SelectMgr_SelectingVolumeManager::IsScalableActiveVolume() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return false;
  }
  return myActiveSelectingVolume->IsScalable();
}

//=================================================================================================

gp_Pnt2d SelectMgr_SelectingVolumeManager::GetMousePosition() const
{
  if (myActiveSelectingVolume.IsNull())
  {
    return gp_Pnt2d(RealLast(), RealLast());
  }
  return myActiveSelectingVolume->GetMousePosition();
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::GetPlanes(
  NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const
{
  if (myActiveSelectingVolume.IsNull())
  {
    thePlaneEquations.Clear();
    return;
  }
  return myActiveSelectingVolume->GetPlanes(thePlaneEquations);
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::SetViewClipping(
  const occ::handle<Graphic3d_SequenceOfHClipPlane>& theViewPlanes,
  const occ::handle<Graphic3d_SequenceOfHClipPlane>& theObjPlanes,
  const SelectMgr_SelectingVolumeManager*            theWorldSelMgr)
{
  myViewClipPlanes   = theViewPlanes;
  myObjectClipPlanes = theObjPlanes;
  if (GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    return;
  }

  const SelectMgr_SelectingVolumeManager* aWorldSelMgr =
    theWorldSelMgr != nullptr ? theWorldSelMgr : this;
  myViewClipRange.SetVoid();
  if (!theViewPlanes.IsNull() && !theViewPlanes->IsEmpty())
  {
    myViewClipRange.AddClippingPlanes(
      *theViewPlanes,
      gp_Ax1(aWorldSelMgr->myActiveSelectingVolume->GetNearPnt(),
             aWorldSelMgr->myActiveSelectingVolume->GetViewRayDirection()));
  }
  if (!theObjPlanes.IsNull() && !theObjPlanes->IsEmpty())
  {
    myViewClipRange.AddClippingPlanes(
      *theObjPlanes,
      gp_Ax1(aWorldSelMgr->myActiveSelectingVolume->GetNearPnt(),
             aWorldSelMgr->myActiveSelectingVolume->GetViewRayDirection()));
  }
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::SetViewClipping(
  const SelectMgr_SelectingVolumeManager& theOther)
{
  myViewClipPlanes   = theOther.myViewClipPlanes;
  myObjectClipPlanes = theOther.myObjectClipPlanes;
  myViewClipRange    = theOther.myViewClipRange;
}

//=================================================================================================

void SelectMgr_SelectingVolumeManager::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, SelectMgr_SelectingVolumeManager)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myActiveSelectingVolume.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myViewClipPlanes.get())
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myObjectClipPlanes.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myViewClipRange)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToAllowOverlap)
}
