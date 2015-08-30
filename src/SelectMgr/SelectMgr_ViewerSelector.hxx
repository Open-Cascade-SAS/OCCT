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

// Modified by  ...
//              ROB JAN/07/98 : Improve Storage of detected entities
//              AGV OCT/23/03 : Optimize the method SortResult() (OCC4201)

#ifndef _SelectMgr_ViewerSelector_HeaderFile
#define _SelectMgr_ViewerSelector_HeaderFile

#include <MMgt_TShared.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Chronometer.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Handle_TColStd_HArray1OfInteger.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <SelectMgr_IndexedDataMapOfOwnerCriterion.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_SelectableObjectSet.hxx>
#include <Handle_SelectMgr_EntityOwner.hxx>
#include <SelectMgr_StateOfSelection.hxx>
#include <Standard_OStream.hxx>
#include <Handle_SelectBasics_SensitiveEntity.hxx>

class TColStd_HArray1OfInteger;
class SelectMgr_SelectionManager;
class SelectMgr_Selection;
class SelectMgr_SensitiveEntitySet;
class SelectMgr_EntityOwner;
class TColStd_ListOfInteger;
class TCollection_AsciiString;
class SelectBasics_SensitiveEntity;
class SelectMgr_SelectableObjectSet;

typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), NCollection_Handle<SelectMgr_SensitiveEntitySet> > SelectMgr_MapOfObjectSensitives;
typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), NCollection_Handle<SelectMgr_SensitiveEntitySet> >::Iterator SelectMgr_MapOfObjectSensitivesIterator;

typedef NCollection_DataMap<Handle(SelectMgr_EntityOwner), Standard_Integer> SelectMgr_MapOfOwnerDetectedEntities;
typedef NCollection_DataMap<Handle(SelectMgr_EntityOwner), Standard_Integer>::Iterator SelectMgr_MapOfOwnerDetectedEntitiesIterator;

typedef NCollection_DataMap<Standard_Integer, SelectMgr_SelectingVolumeManager> SelectMgr_FrustumCache;

//! An internal class for calculation of current largest tolerance value which will be applied
//! for creation of selecting frustum by default. Each time the selection set is deactivated,
//! maximum tolerance value will be recalculated. If a user enables custom precision using
//! StdSelect_ViewerSelector3d::SetPixelTolerance, it will be applied to all sensitive entities
//! without any checks.
class SelectMgr_ToleranceMap
{
public:

  //! Sets tolerance values to -1.0
  SelectMgr_ToleranceMap();

  Standard_EXPORT ~SelectMgr_ToleranceMap();

  //! Adds the value given to map, checks if the current tolerance value
  //! should be replaced by theTolerance
  Standard_EXPORT void Add (const Standard_Integer& theTolerance);

  //! Decrements a counter of the tolerance given, checks if the current tolerance value
  //! should be recalculated
  Standard_EXPORT void Decrement (const Standard_Integer& theTolerance);

  //! Returns a current tolerance that must be applied
  Standard_EXPORT Standard_Integer Tolerance() const;

  //! Sets tolerance to the given one and disables adaptive checks
  Standard_EXPORT void SetCustomTolerance (const Standard_Integer theTolerance);

  //! Unsets a custom tolerance and enables adaptive checks
  Standard_EXPORT void ResetDefaults();

private:
  NCollection_DataMap<Standard_Integer, Standard_Integer> myTolerances;
  Standard_Integer                                        myLargestKey;
  Standard_Integer                                        myCustomTolerance;
};

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
class SelectMgr_ViewerSelector : public MMgt_TShared
{
public:

  //! Empties all the tables, removes all selections...
  Standard_EXPORT void Clear();

  //! returns the Sensitivity of picking
  Standard_Integer Sensitivity() const;

  //! Sorts the detected entites by priority and distance.
  //!          to be redefined if other criterion are used...
  Standard_EXPORT void SortResult();

  //! Begins an iteration scanning for the owners detected at a position in the view.
  void Init();

  //!  Continues the interation scanning for the owners
  //!   detected at a position in the view, or
  //! -   continues the iteration scanning for the owner
  //!   closest to the position in the view.
  Standard_EXPORT Standard_Boolean More();

  //! Returns the next owner found in the iteration. This is
  //! a scan for the owners detected at a position in the view.
  void Next();

  //! Returns the current selected entity detected by the selector;
  Standard_EXPORT Handle_SelectMgr_EntityOwner Picked() const;

  //! Returns the picked element with the highest priority,
  //! and which is the closest to the last successful mouse position.
  Standard_EXPORT Handle_SelectMgr_EntityOwner OnePicked();

  //! Set preference of selecting one object for OnePicked() method:
  //! - If True, objects with less depth (distance fron the view plane) are
  //!   preferred regardless of priority (priority is used then to choose among
  //!   objects with similar depth),
  //! - If False, objects with higher priority are preferred regardless of the
  //!   depth which is used to choose among objects of the same priority.
  void SetPickClosest (const Standard_Boolean preferClosest);

  //! Returns the number of owners found at a position in
  //! the view by the Init - More - Next - Picked iteration.
  Standard_EXPORT Standard_Integer NbPicked() const;

  //! Returns the  entity which is at rank <aRank>
  //!          in the list of stored ones.
  Standard_EXPORT Handle_SelectMgr_EntityOwner Picked (const Standard_Integer aRank) const;

  Standard_EXPORT Standard_Boolean Contains (const Handle(SelectMgr_SelectableObject)& theObject) const;

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
  Standard_EXPORT void ActiveOwners (NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners) const;

  //! Adds new object to the map of selectable objects
  Standard_EXPORT void AddSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Adds new selection to the object and builds its BVH tree
  Standard_EXPORT void AddSelectionToObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                             const Handle(SelectMgr_Selection)& theSelection);

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

  //! Initializes internal iterator for stored detected sensitive entities
  Standard_EXPORT void InitDetected();

  //! Makes a step along the map of detected sensitive entities and their owners
  Standard_EXPORT void NextDetected();

  //! Returns true if iterator of map of detected sensitive entities has reached
  //! its end
  Standard_EXPORT Standard_Boolean MoreDetected();

  //! Returns sensitive entity that was detected during the previous run of
  //! selection algorithm
  Standard_EXPORT const Handle(SelectBasics_SensitiveEntity)& DetectedEntity() const;

  //! Returns instance of selecting volume manager of the viewer selector
  Standard_EXPORT SelectMgr_SelectingVolumeManager& GetManager();

  friend class SelectMgr_SelectionManager;

  DEFINE_STANDARD_RTTI(SelectMgr_ViewerSelector)

protected:

  Standard_EXPORT SelectMgr_ViewerSelector();

  //! Traverses BVH containing all added selectable objects and
  //! finds candidates for further search of overlap
  Standard_EXPORT void TraverseSensitives();

  //! Returns True if the owner provides clipping by depth
  //! for its sensitives. Override this method to tell the selector
  //! to use the DepthClipping method for the owner.
  //! Default implementation returns False for every owner.
  //! @param theOwner [in] the onwer to check.
  //! @return True if owner provides depth limits for sensitive clipping.
  Standard_EXPORT virtual Standard_Boolean HasDepthClipping (const Handle(SelectMgr_EntityOwner)& theOwner) const;

  //! Internal function that checks if there is possible overlap
  //! between some entity of selectable object theObject and
  //! current selecting volume
  void traverseObject (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Internal function that checks if a particular sensitive
  //! entity theEntity overlaps current selecting volume precisely
  void checkOverlap (const Handle(SelectBasics_SensitiveEntity)& theEntity,
                     const Standard_Integer theEntityIdx,
                     SelectMgr_SelectingVolumeManager& theMgr);

  //! Marks all added sensitive entities of all objects as non-selectable
  void resetSelectionActivationStatus();

  //! Checks if the entity given requires to scale current selecting frustum
  Standard_Boolean isToScaleFrustum (const Handle(SelectBasics_SensitiveEntity)& theEntity);

private:

  void Activate (const Handle(SelectMgr_Selection)& theSelection);

  void Deactivate (const Handle(SelectMgr_Selection)& theSelection);

  //! removes a Selection from the Selector
  void Remove (const Handle(SelectMgr_Selection)& aSelection);

  //! Internal function that checks if a current selecting frustum
  //! needs to be scaled and transformed for the entity and performs
  //! necessary calculations
  void computeFrustum (const Handle(SelectBasics_SensitiveEntity)& theEnt,
                       const gp_Trsf&                              theInvTrsf,
                       SelectMgr_FrustumCache&                     theCachedMgrs,
                       SelectMgr_SelectingVolumeManager&           theResMgr);

protected:

  Standard_Boolean preferclosest;
  Standard_Boolean myToUpdateTolerance;
  SelectMgr_IndexedDataMapOfOwnerCriterion mystored;
  SelectMgr_SelectingVolumeManager mySelectingVolumeMgr;
  mutable NCollection_Handle<SelectMgr_SelectableObjectSet> mySelectableObjects;
  SelectMgr_ToleranceMap myTolerances;

private:

  Handle_TColStd_HArray1OfInteger              myIndexes;
  Standard_Integer                             myCurRank;
  Standard_Boolean                             myIsLeftChildQueuedFirst;
  Standard_Integer                             myEntityIdx;
  SelectMgr_MapOfObjectSensitives              myMapOfObjectSensitives;
  SelectMgr_MapOfOwnerDetectedEntities         myMapOfDetected;
  SelectMgr_MapOfOwnerDetectedEntitiesIterator myDetectedIter;
};

DEFINE_STANDARD_HANDLE(SelectMgr_ViewerSelector, MMgt_TShared)

#include <SelectMgr_ViewerSelector.lxx>

#endif
