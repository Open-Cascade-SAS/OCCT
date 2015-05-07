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

//=======================================================================
// function : SelectMgr_SelectingVolumeManager
// purpose  : Creates instances of all available selecting volume types
//=======================================================================
SelectMgr_SelectingVolumeManager::SelectMgr_SelectingVolumeManager (Standard_Boolean theToAllocateFrustums)
{
  myActiveSelectionType = Unknown;
  myToAllowOverlap = Standard_False;

  if (theToAllocateFrustums)
  {
    mySelectingVolumes[Frustum] = new SelectMgr_RectangularFrustum();
    mySelectingVolumes[FrustumSet] = new SelectMgr_TriangularFrustumSet();
  }
}

//=======================================================================
// function : Transform
// purpose  : Returns a copy of active frustum transformed according to the matrix given
//=======================================================================
SelectMgr_SelectingVolumeManager SelectMgr_SelectingVolumeManager::Transform (const gp_Trsf& theTrsf)
{
  SelectMgr_SelectingVolumeManager aMgr (Standard_False);

  if (myActiveSelectionType == Unknown)
    return aMgr;

  aMgr.myActiveSelectionType = myActiveSelectionType;

  aMgr.mySelectingVolumes[myActiveSelectionType / 2] = mySelectingVolumes[myActiveSelectionType / 2]->Transform (theTrsf);
  aMgr.myToAllowOverlap = myToAllowOverlap;

  return aMgr;
}

//=======================================================================
// function : Scale
// purpose  : IMPORTANT: Makes sense only for point selection!
//            Returns a copy of the frustum resized according to the scale factor given
//=======================================================================
SelectMgr_SelectingVolumeManager SelectMgr_SelectingVolumeManager::Scale (const Standard_Real theScaleFactor)
{
  if (myActiveSelectionType != Point)
    return SelectMgr_SelectingVolumeManager (Standard_False);

  SelectMgr_SelectingVolumeManager aMgr (Standard_False);

  aMgr.myActiveSelectionType = Point;

  aMgr.mySelectingVolumes[Point] = mySelectingVolumes[Point]->Scale (theScaleFactor);

  return aMgr;
}

//=======================================================================
// function : GetActiveSelectionType
// purpose  :
//=======================================================================
Standard_Integer SelectMgr_SelectingVolumeManager::GetActiveSelectionType() const
{
  return myActiveSelectionType;
}

//=======================================================================
// function : SetActiveSelectionType
// purpose  :
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetActiveSelectionType (const SelectionType& theType)
{
  myActiveSelectionType = theType;
}

//=======================================================================
// function : SetCamera
// purpose  : Updates camera projection and orientation matrices in all
//            selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetCamera (const Handle(Graphic3d_Camera) theCamera)
{
  for (Standard_Integer anIdx = 0; anIdx < VolumeTypesNb; ++anIdx)
  {
    mySelectingVolumes[anIdx]->SetCamera (theCamera);
  }
}

//=======================================================================
// function : SetCamera
// purpose  : Updates camera projection and orientation matrices in all
//            selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetCamera (const Graphic3d_Mat4d& theProjection,
                                                  const Graphic3d_Mat4d& theOrientation,
                                                  const Standard_Boolean theIsOrthographic)
{
  for (Standard_Integer anIdx = 0; anIdx < VolumeTypesNb; ++anIdx)
  {
    mySelectingVolumes[anIdx]->SetCamera (theProjection, theOrientation, theIsOrthographic);
  }
}

//=======================================================================
// function : SetCamera
// purpose  : Updates viewport in all selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetViewport (const Standard_Real theX,
                                                    const Standard_Real theY,
                                                    const Standard_Real theWidth,
                                                    const Standard_Real theHeight)
{
  for (Standard_Integer anIdx = 0; anIdx < VolumeTypesNb; ++anIdx)
  {
    mySelectingVolumes[anIdx]->SetViewport (theX, theY, theWidth, theHeight);
  }
}

//=======================================================================
// function : SetWindowSize
// purpose  : Updates window size in all selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetWindowSize (const Standard_Integer theWidth,
                                                      const Standard_Integer theHeight)
{
  for (Standard_Integer anIdx = 0; anIdx < VolumeTypesNb; ++anIdx)
  {
    mySelectingVolumes[anIdx]->SetWindowSize (theWidth, theHeight);
  }
}

//=======================================================================
// function : SetPixelTolerance
// purpose  : Updates pixel tolerance in all selecting volumes
//=======================================================================
void SelectMgr_SelectingVolumeManager::SetPixelTolerance (const Standard_Real theTolerance)
{
  for (Standard_Integer anIdx = 0; anIdx < VolumeTypesNb; ++anIdx)
  {
    mySelectingVolumes[anIdx]->SetPixelTolerance (theTolerance);
  }
}

//=======================================================================
// function : BuildSelectingVolume
// purpose  : Builds rectangular selecting frustum for point selection
//=======================================================================
void SelectMgr_SelectingVolumeManager::BuildSelectingVolume (const gp_Pnt2d& thePoint)
{
  if (myActiveSelectionType != Point)
    return;

  mySelectingVolumes[Frustum]->Build (thePoint);
}

//=======================================================================
// function : BuildSelectingVolume
// purpose  : Builds rectangular selecting frustum for box selection
//=======================================================================
void SelectMgr_SelectingVolumeManager::BuildSelectingVolume (const gp_Pnt2d& theMinPt,
                                                             const gp_Pnt2d& theMaxPt)
{
  if (myActiveSelectionType != Box)
    return;

  mySelectingVolumes[Frustum]->Build (theMinPt, theMaxPt);
}

//=======================================================================
// function : BuildSelectingVolume
// purpose  : Builds set of triangular selecting frustums for polyline
//            selection
//=======================================================================
void SelectMgr_SelectingVolumeManager::BuildSelectingVolume (const TColgp_Array1OfPnt2d& thePoints)
{
  if (myActiveSelectionType != Polyline)
    return;

  mySelectingVolumes[FrustumSet]->Build (thePoints);
}

//=======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::Overlaps (const BVH_Box<Standard_Real, 3>& theBndBox,
                                                             Standard_Real& theDepth)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->Overlaps (theBndBox, theDepth);
}

//=======================================================================
// function : Overlaps
// purpose  : Intersection test between defined volume and given point
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                             const SelectMgr_Vec3& theBoxMax,
                                                             Standard_Boolean*     theInside)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->Overlaps (theBoxMin, theBoxMax, theInside);
}

//=======================================================================
// function : Overlaps
// purpose  : Intersection test between defined volume and given point
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::Overlaps (const gp_Pnt& thePt,
                                                             Standard_Real& theDepth)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->Overlaps (thePt,
                                                                  theDepth);
}

//=======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            ordered set of points, representing line segments. The test
//            may be considered of interior part or boundary line defined
//            by segments depending on given sensitivity type
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::Overlaps (const Handle(TColgp_HArray1OfPnt)& theArrayOfPts,
                                                             Standard_Integer theSensType,
                                                             Standard_Real& theDepth)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->Overlaps (theArrayOfPts,
                                                                  (Select3D_TypeOfSensitivity)theSensType,
                                                                  theDepth);
}

//=======================================================================
// function : Overlaps
// purpose  : Checks if line segment overlaps selecting volume
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::Overlaps (const gp_Pnt& thePt1,
                                                             const gp_Pnt& thePt2,
                                                             Standard_Real& theDepth)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->Overlaps (thePt1, thePt2, theDepth);
}

//=======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            triangle. The test may be considered of interior part or
//            boundary line defined by triangle vertices depending on
//            given sensitivity type
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::Overlaps (const gp_Pnt& thePt1,
                                                             const gp_Pnt& thePt2,
                                                             const gp_Pnt& thePt3,
                                                             Standard_Integer theSensType,
                                                             Standard_Real& theDepth)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->Overlaps (thePt1,
                                                                  thePt2,
                                                                  thePt3,
                                                                  (Select3D_TypeOfSensitivity)theSensType,
                                                                  theDepth);
}

//=======================================================================
// function : DistToGeometryCenter
// purpose  : Measures distance between 3d projection of user-picked
//            screen point and given point theCOG
//=======================================================================
Standard_Real SelectMgr_SelectingVolumeManager::DistToGeometryCenter (const gp_Pnt& theCOG)
{
  if (myActiveSelectionType == Unknown)
    return Standard_False;

  return mySelectingVolumes[myActiveSelectionType / 2]->DistToGeometryCenter (theCOG);
}

// =======================================================================
// function : DetectedPoint
// purpose  : Calculates the point on a view ray that was detected during
//            the run of selection algo by given depth. Is valid for point
//            selection only
// =======================================================================
NCollection_Vec3<Standard_Real> SelectMgr_SelectingVolumeManager::DetectedPoint (const Standard_Real theDepth) const
{
  if (myActiveSelectionType != Point)
    return NCollection_Vec3<Standard_Real> (RealLast());

  return mySelectingVolumes[Frustum]->DetectedPoint (theDepth);
}

//=======================================================================
// function : IsClipped
// purpose  : Checks if the point of sensitive in which selection was
//            detected belongs to the region defined by clipping planes
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::IsClipped (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                              const Standard_Real& theDepth)
{
  if (myActiveSelectionType == Point)
    return Standard_False;

  return mySelectingVolumes[Frustum]->IsClipped (thePlanes, theDepth);
}

//=======================================================================
// function : AllowOverlapDetection
// purpose  : If theIsToAllow is false, only fully included sensitives will
//            be detected, otherwise the algorithm will mark both included
//            and overlapped entities as matched
//=======================================================================
void SelectMgr_SelectingVolumeManager::AllowOverlapDetection (const Standard_Boolean theIsToAllow)
{
  myToAllowOverlap = theIsToAllow;
}

//=======================================================================
// function : IsOverlapAllowed
// purpose  :
//=======================================================================
Standard_Boolean SelectMgr_SelectingVolumeManager::IsOverlapAllowed() const
{
  return myActiveSelectionType != Box || myToAllowOverlap;
}
