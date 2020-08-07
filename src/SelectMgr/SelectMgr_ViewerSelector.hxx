// Created on: 1995-02-15
// Created by: Roberc Coublanc
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _SelectMgr_ViewerSelector_HeaderFile
#define _SelectMgr_ViewerSelector_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Chronometer.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Select3D_BVHBuilder3d.hxx>
#include <SelectMgr_IndexedDataMapOfOwnerCriterion.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_SelectableObjectSet.hxx>
#include <SelectMgr_StateOfSelection.hxx>
#include <SelectMgr_ToleranceMap.hxx>
#include <SelectMgr_TypeOfDepthTolerance.hxx>
#include <Standard_OStream.hxx>
#include <SelectMgr_BVHThreadPool.hxx>

class SelectMgr_SelectionManager;
class SelectMgr_SensitiveEntitySet;
class SelectMgr_EntityOwner;
class Select3D_SensitiveEntity;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif
typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_SensitiveEntitySet) > SelectMgr_MapOfObjectSensitives;
typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_SensitiveEntitySet) >::Iterator SelectMgr_MapOfObjectSensitivesIterator;

typedef NCollection_DataMap<Standard_Integer, SelectMgr_SelectingVolumeManager> SelectMgr_FrustumCache;

//! A framework to define finding, sorting the sensitive
//! primitives in a view. Services are also provided to
//! define the return of the owners of those primitives
//! selected. The primitives are sorted by criteria such
//! as priority of the primitive or its depth in the view
//! relative to that of other primitives.
//! Note that in 3D, the inheriting framework
//! StdSelect_ViewerSelector3d   is only to be used
//! if you do not want to use the services provided by
//! AIS.
//! Two tools are available to find and select objects
//! found at a given position in the view. If you want to
//! select the owners of all the objects detected at
//! point x,y,z you use the Init - More - Next - Picked
//! loop. If, on the other hand, you want to select only
//! one object detected at that point, you use the Init -
//! More - OnePicked loop. In this iteration, More is
//! used to see if an object was picked and
//! OnePicked, to get the object closest to the pick position.
//! Viewer selectors are driven by
//! SelectMgr_SelectionManager, and manipulate
//! the SelectMgr_Selection objects given to them by
//! the selection manager.
//!
//! Tolerances are applied to the entities in the following way:
//! 1. tolerance value stored in mytolerance will be used to calculate initial
//!    selecting frustum, which will be applied for intersection testing during
//!    BVH traverse;
//! 2. if tolerance of sensitive entity is less than mytolerance, the frustum for
//!    intersection detection will be resized according to its sensitivity.
class SelectMgr_ViewerSelector : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_ViewerSelector, Standard_Transient)
  friend class SelectMgr_SelectionManager;
public:

  //! Empties all the tables, removes all selections...
  Standard_EXPORT void Clear();

  //! Returns custom pixel tolerance value.
  Standard_Integer CustomPixelTolerance() const { return myTolerances.CustomTolerance(); }

  //! Sets the pixel tolerance <theTolerance>.
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer theTolerance);

  //! Returns the largest sensitivity of picking
  Standard_Real Sensitivity() const { return myTolerances.Tolerance(); }

  //! Returns the largest pixel tolerance.
  Standard_Integer PixelTolerance() const { return myTolerances.Tolerance(); }

  //! Sorts the detected entites by priority and distance.
  Standard_EXPORT virtual void SortResult();

  //! Returns the picked element with the highest priority,
  //! and which is the closest to the last successful mouse position.
  Handle(SelectMgr_EntityOwner) OnePicked() const
  {
    return mystored.IsEmpty()
         ? Handle(SelectMgr_EntityOwner)()
         : Picked (1);
  }

  //! Return the flag determining precedence of picked depth (distance from eye to entity) over entity priority in sorted results; TRUE by default.
  //! When flag is TRUE, priority will be considered only if entities have the same depth  within the tolerance.
  //! When flag is FALSE, entities with higher priority will be in front regardless of their depth (like x-ray).
  bool ToPickClosest() const { return myToPreferClosest; }

  //! Set flag determining precedence of picked depth over entity priority in sorted results.
  void SetPickClosest (bool theToPreferClosest) { myToPreferClosest = theToPreferClosest; }

  //! Return the type of tolerance for considering two entities having a similar depth (distance from eye to entity);
  //! SelectMgr_TypeOfDepthTolerance_SensitivityFactor by default.
  SelectMgr_TypeOfDepthTolerance DepthToleranceType() const { return myDepthTolType; }

  //! Return the tolerance for considering two entities having a similar depth (distance from eye to entity).
  Standard_Real DepthTolerance() const { return myDepthTolerance; }

  //! Set the tolerance for considering two entities having a similar depth (distance from eye to entity).
  //! @param theType [in] type of tolerance value
  //! @param theTolerance [in] tolerance value in 3D scale (SelectMgr_TypeOfDepthTolerance_Uniform)
  //!                          or in pixels (SelectMgr_TypeOfDepthTolerance_UniformPixels);
  //!                          value is ignored in case of SelectMgr_TypeOfDepthTolerance_SensitivityFactor
  void SetDepthTolerance (SelectMgr_TypeOfDepthTolerance theType,
                          Standard_Real theTolerance)
  {
    myDepthTolType   = theType;
    myDepthTolerance = theTolerance;
  }

  //! Returns the number of detected owners.
  Standard_Integer NbPicked() const { return mystored.Extent(); }

  //! Clears picking results.
  Standard_EXPORT void ClearPicked();

  //! Returns the entity Owner for the object picked at specified position.
  //! @param theRank rank of detected object within range 1...NbPicked()
  Standard_EXPORT Handle(SelectMgr_EntityOwner) Picked (const Standard_Integer theRank) const;

  //! Returns the Entity for the object picked at specified position.
  //! @param theRank rank of detected object within range 1...NbPicked()
  Standard_EXPORT const SelectMgr_SortCriterion& PickedData (const Standard_Integer theRank) const;

  //! Returns the Entity for the object picked at specified position.
  //! @param theRank rank of detected object within range 1...NbPicked()
  const Handle(Select3D_SensitiveEntity)& PickedEntity (const Standard_Integer theRank) const { return PickedData (theRank).Entity; }

  //! Returns the 3D point (intersection of picking axis with the object nearest to eye)
  //! for the object picked at specified position.
  //! @param theRank rank of detected object within range 1...NbPicked()
  gp_Pnt PickedPoint (const Standard_Integer theRank) const { return PickedData (theRank).Point; }

  Standard_EXPORT Standard_Boolean Contains (const Handle(SelectMgr_SelectableObject)& theObject) const;

  //! Returns the default builder used to construct BVH of entity set.
  const Handle(Select3D_BVHBuilder3d) EntitySetBuilder() { return myEntitySetBuilder; }

  //! Sets the default builder used to construct BVH of entity set.
  //! The new builder will be also assigned for already defined objects, but computed BVH trees will not be invalidated.
  Standard_EXPORT void SetEntitySetBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder);

  //! Returns the list of selection modes ModeList found in
  //! this selector for the selectable object aSelectableObject.
  //! Returns true if aSelectableObject is referenced inside
  //! this selector; returns false if the object is not present
  //! in this selector.
  Standard_EXPORT Standard_Boolean Modes (const Handle(SelectMgr_SelectableObject)& theSelectableObject,
                                          TColStd_ListOfInteger& theModeList,
                                          const SelectMgr_StateOfSelection theWantedState = SelectMgr_SOS_Any) const;

  //! Returns true if the selectable object
  //! aSelectableObject having the selection mode aMode
  //! is active in this selector.
  Standard_EXPORT Standard_Boolean IsActive (const Handle(SelectMgr_SelectableObject)& theSelectableObject,
                                             const Standard_Integer theMode) const;

  //! Returns true if the selectable object
  //! aSelectableObject having the selection mode aMode
  //! is in this selector.
  Standard_EXPORT Standard_Boolean IsInside (const Handle(SelectMgr_SelectableObject)& theSelectableObject,
                                             const Standard_Integer theMode) const;

  //! Returns the selection status Status of the selection aSelection.
  Standard_EXPORT SelectMgr_StateOfSelection Status (const Handle(SelectMgr_Selection)& theSelection) const;

  Standard_EXPORT TCollection_AsciiString Status (const Handle(SelectMgr_SelectableObject)& theSelectableObject) const;

  //! Returns the list of active entity owners
  Standard_EXPORT void ActiveOwners (NCollection_List<Handle(SelectMgr_EntityOwner)>& theOwners) const;

  //! Adds new object to the map of selectable objects
  Standard_EXPORT void AddSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Adds new selection to the object and builds its BVH tree
  Standard_EXPORT void AddSelectionToObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                             const Handle(SelectMgr_Selection)& theSelection);

  //! Moves existing object from set of not transform persistence objects
  //! to set of transform persistence objects (or vice versa).
  Standard_EXPORT void MoveSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Removes selectable object from map of selectable ones
  Standard_EXPORT void RemoveSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Removes selection of the object and marks its BVH tree for rebuild
  Standard_EXPORT void RemoveSelectionOfObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                                const Handle(SelectMgr_Selection)& theSelection);

  //! Marks BVH of selectable objects for rebuild. Parameter theIsForce set as true
  //! guarantees that 1st level BVH for the viewer selector will be rebuilt during this call
  Standard_EXPORT void RebuildObjectsTree (const Standard_Boolean theIsForce = Standard_False);

  //! Marks BVH of sensitive entities of particular selectable object for rebuild. Parameter
  //! theIsForce set as true guarantees that 2nd level BVH for the object given will be
  //! rebuilt during this call
  Standard_EXPORT void RebuildSensitivesTree (const Handle(SelectMgr_SelectableObject)& theObject,
                                              const Standard_Boolean theIsForce = Standard_False);

  //! Returns instance of selecting volume manager of the viewer selector
  SelectMgr_SelectingVolumeManager& GetManager() { return mySelectingVolumeMgr; }

  //! Return map of selectable objects.
  const SelectMgr_SelectableObjectSet& SelectableObjects() const { return mySelectableObjects; }

  //! Marks all added sensitive entities of all objects as non-selectable
  Standard_EXPORT void ResetSelectionActivationStatus();

  //! Is used for rectangular selection only
  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the algorithm will
  //! mark both included and overlapped entities as matched
  Standard_EXPORT void AllowOverlapDetection (const Standard_Boolean theIsToAllow);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

public:

  //! Begins an iteration scanning for the owners detected at a position in the view.
  Standard_DEPRECATED("Deprecated method Init()")
  void Init() { initPicked(); }

  //! Continues the interation scanning for the owners detected at a position in the view,
  //! or continues the iteration scanning for the owner closest to the position in the view.
  Standard_DEPRECATED("Deprecated method More()")
  Standard_Boolean More() { return morePicked(); }

  //! Returns the next owner found in the iteration. This is
  //! a scan for the owners detected at a position in the view.
  Standard_DEPRECATED("Deprecated method Next()")
  void Next() { nextPicked(); }

  //! Returns the current selected entity detected by the selector;
  Standard_DEPRECATED("Deprecated method Picked()")
  Standard_EXPORT Handle(SelectMgr_EntityOwner) Picked() const;

  //! Initializes internal iterator for stored detected sensitive entities
  Standard_DEPRECATED("Deprecated method InitDetected()")
  void InitDetected() { initPicked(); }

  //! Makes a step along the map of detected sensitive entities and their owners
  Standard_DEPRECATED("Deprecated method NextDetected()")
  void NextDetected() { nextPicked(); }

  //! Returns true if iterator of map of detected sensitive entities has reached its end
  Standard_DEPRECATED("Deprecated method MoreDetected()")
  Standard_Boolean MoreDetected() { return morePicked(); }

  //! Returns sensitive entity that was detected during the previous run of selection algorithm
  Standard_DEPRECATED("Deprecated method DetectedEntity() should be replaced by DetectedEntity(int)")
  Standard_EXPORT const Handle(Select3D_SensitiveEntity)& DetectedEntity() const;

public:

  //! Enables/disables building BVH for sensitives in separate threads
  Standard_EXPORT void SetToPrebuildBVH(Standard_Boolean theToPrebuild, Standard_Integer theThreadsNum = -1);

  //! Queues a sensitive entity to build its BVH
  Standard_EXPORT void QueueBVHBuild(const Handle(Select3D_SensitiveEntity)& theEntity);

  //! Waits BVH threads finished building
  Standard_EXPORT void WaitForBVHBuild();

  //! Returns TRUE if building BVH for sensitives in separate threads is enabled
  Standard_Boolean ToPrebuildBVH() const
  {
    return myToPrebuildBVH;
  }

protected:

  Standard_EXPORT SelectMgr_ViewerSelector();

  //! Traverses BVH containing all added selectable objects and
  //! finds candidates for further search of overlap
  Standard_EXPORT void TraverseSensitives();

  //! Internal function that checks if there is possible overlap between some entity of selectable object theObject and
  //! current selecting volume.
  //! @param theObject [in] the selectable object for traversal.
  //! @param theMgr [in] the (un)transformed copy of the selecting volume manager representing active selection frustum.
  //! @param theCamera, theProjectionMat, theWorldViewMat [in] the source camera and matrices for theMgr given.
  //! @param theViewportWidth, theViewportHeight [in] viewport (window) dimensions for evaluating 
  //!        object's transformation persistence.
  Standard_EXPORT void traverseObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                       const SelectMgr_SelectingVolumeManager& theMgr,
                                       const Handle(Graphic3d_Camera)& theCamera,
                                       const Graphic3d_Mat4d& theProjectionMat,
                                       const Graphic3d_Mat4d& theWorldViewMat,
                                       const Standard_Integer theViewportWidth,
                                       const Standard_Integer theViewportHeight);

  //! Internal function that checks if a particular sensitive
  //! entity theEntity overlaps current selecting volume precisely
  Standard_EXPORT void checkOverlap (const Handle(Select3D_SensitiveEntity)& theEntity,
                                     const gp_GTrsf& theInversedTrsf,
                                     SelectMgr_SelectingVolumeManager& theMgr);

private:

  //! Checks if the entity given requires to scale current selecting frustum
  Standard_Boolean isToScaleFrustum (const Handle(Select3D_SensitiveEntity)& theEntity);

  //! In case if custom tolerance is set, this method will return sum of entity sensitivity and
  //! custom tolerance. Otherwise, pure entity sensitivity factor will be returned.
  Standard_Integer sensitivity (const Handle(Select3D_SensitiveEntity)& theEntity) const;

  void Activate (const Handle(SelectMgr_Selection)& theSelection);

  void Deactivate (const Handle(SelectMgr_Selection)& theSelection);

  //! removes a Selection from the Selector
  void Remove (const Handle(SelectMgr_Selection)& aSelection);

  //! Internal function that checks if a current selecting frustum needs to be scaled and transformed for the entity and performs necessary calculations.
  void computeFrustum (const Handle(Select3D_SensitiveEntity)& theEnt,
                       const SelectMgr_SelectingVolumeManager& theMgrGlobal,
                       const SelectMgr_SelectingVolumeManager& theMgrObject,
                       const gp_GTrsf& theInvTrsf,
                       SelectMgr_FrustumCache& theCachedMgrs,
                       SelectMgr_SelectingVolumeManager& theResMgr);


private: // implementation of deprecated methods

  //! Initializes internal iterator for stored detected sensitive entities
  void initPicked() { myCurRank = 1; }

  //! Makes a step along the map of detected sensitive entities and their owners
  void nextPicked() { ++myCurRank; }

  //! Returns true if iterator of map of detected sensitive entities has reached its end
  Standard_Boolean morePicked() const
  {
    if (mystored.Extent() == 0) return Standard_False;
    if (myCurRank == 0) return Standard_False;
    return myCurRank <= myIndexes->Length();
  }

  //! Compute 3d position for detected entity.
  void updatePoint3d (SelectMgr_SortCriterion& theCriterion,
                      const SelectBasics_PickResult& thePickResult,
                      const Handle(Select3D_SensitiveEntity)& theEntity,
                      const gp_GTrsf& theInversedTrsf,
                      const SelectMgr_SelectingVolumeManager& theMgr) const;

protected:

  Standard_Real                                 myDepthTolerance;
  SelectMgr_TypeOfDepthTolerance                myDepthTolType;
  Standard_Boolean                              myToPreferClosest;
  Standard_Boolean                              myToUpdateTolerance;
  SelectMgr_IndexedDataMapOfOwnerCriterion      mystored;
  SelectMgr_SelectingVolumeManager              mySelectingVolumeMgr;
  mutable SelectMgr_SelectableObjectSet         mySelectableObjects;
  SelectMgr_ToleranceMap                        myTolerances;
  NCollection_DataMap<Graphic3d_ZLayerId, Standard_Integer> myZLayerOrderMap;
  Handle(Select3D_BVHBuilder3d)                 myEntitySetBuilder;
  gp_Pnt                                        myCameraEye;
  gp_Dir                                        myCameraDir;
  Standard_Real                                 myCameraScale;

  Standard_Boolean                              myToPrebuildBVH;
  Handle(SelectMgr_BVHThreadPool)               myBVHThreadPool;

  Handle(TColStd_HArray1OfInteger)             myIndexes;
  Standard_Integer                             myCurRank;
  Standard_Boolean                             myIsLeftChildQueuedFirst;
  SelectMgr_MapOfObjectSensitives              myMapOfObjectSensitives;

};

DEFINE_STANDARD_HANDLE(SelectMgr_ViewerSelector, Standard_Transient)

#endif
