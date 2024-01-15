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

#include <OSD_Chronometer.hxx>
#include <SelectMgr_BVHThreadPool.hxx>
#include <SelectMgr_IndexedDataMapOfOwnerCriterion.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_SelectableObjectSet.hxx>
#include <SelectMgr_StateOfSelection.hxx>
#include <SelectMgr_ToleranceMap.hxx>
#include <SelectMgr_TypeOfDepthTolerance.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Transient.hxx>
#include <StdSelect_TypeOfSelectionImage.hxx>
#include <TColStd_HArray1OfInteger.hxx>

class SelectMgr_SensitiveEntitySet;
class SelectMgr_EntityOwner;
class Select3D_SensitiveEntity;
class V3d_View;

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

  //! Constructs an empty selector object.
  Standard_EXPORT SelectMgr_ViewerSelector();

  //! Returns custom pixel tolerance value.
  Standard_Integer CustomPixelTolerance() const { return myTolerances.CustomTolerance(); }

  //! Sets the pixel tolerance <theTolerance>.
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer theTolerance);

  //! Returns the largest sensitivity of picking
  Standard_Real Sensitivity() const { return myTolerances.Tolerance(); }

  //! Returns the largest pixel tolerance.
  Standard_Integer PixelTolerance() const { return myTolerances.Tolerance(); }

  //! Sorts the detected entities by priority and distance.
  Standard_EXPORT virtual void SortResult() const;

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

  //! Empties all the tables, removes all selections...
  void Clear() { ClearPicked(); }

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

  //! Remove picked entities associated with specified object.
  Standard_EXPORT Standard_Boolean RemovePicked (const Handle(SelectMgr_SelectableObject)& theObject);

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

public:

  //! Picks the sensitive entity at the pixel coordinates of
  //! the mouse <theXPix> and <theYPix>. The selector looks for touched areas and owners.
  Standard_EXPORT void Pick (const Standard_Integer theXPix,
                             const Standard_Integer theYPix,
                             const Handle(V3d_View)& theView);

  //! Picks the sensitive entity according to the minimum
  //! and maximum pixel values <theXPMin>, <theYPMin>, <theXPMax>
  //! and <theYPMax> defining a 2D area for selection in the 3D view aView.
  Standard_EXPORT void Pick (const Standard_Integer theXPMin,
                             const Standard_Integer theYPMin,
                             const Standard_Integer theXPMax,
                             const Standard_Integer theYPMax,
                             const Handle(V3d_View)& theView);

  //! pick action - input pixel values for polyline selection for selection.
  Standard_EXPORT void Pick (const TColgp_Array1OfPnt2d& thePolyline,
                             const Handle(V3d_View)& theView);

  //! Picks the sensitive entity according to the input axis.
  //! This is geometric intersection 3D objects by axis
  //! (camera parameters are ignored and objects with transform persistance are skipped).
  Standard_EXPORT void Pick (const gp_Ax1& theAxis,
                             const Handle(V3d_View)& theView);

  //! Dump of detection results into image.
  //! This method performs axis picking for each pixel in the image
  //! and generates a color depending on picking results and selection image type.
  //! @param theImage       result image, should be initialized
  //! @param theView        3D view defining camera position
  //! @param theType        type of image to define
  //! @param thePickedIndex index of picked entity (1 means topmost)
  Standard_EXPORT Standard_Boolean ToPixMap (Image_PixMap&                        theImage,
                                             const Handle(V3d_View)&              theView,
                                             const StdSelect_TypeOfSelectionImage theType,
                                             const Standard_Integer               thePickedIndex = 1);

public:

  //! Displays sensitives in view <theView>.
  Standard_EXPORT void DisplaySensitive (const Handle(V3d_View)& theView);

  Standard_EXPORT void ClearSensitive (const Handle(V3d_View)& theView);

  Standard_EXPORT void DisplaySensitive (const Handle(SelectMgr_Selection)& theSel,
                                         const gp_Trsf& theTrsf,
                                         const Handle(V3d_View)& theView,
                                         const Standard_Boolean theToClearOthers = Standard_True);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

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

  //! Traverses BVH containing all added selectable objects and
  //! finds candidates for further search of overlap
  Standard_EXPORT void TraverseSensitives (const Standard_Integer theViewId = -1);

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
                                       const Graphic3d_Vec2i& theWinSize);

  //! Internal function that checks if a particular sensitive
  //! entity theEntity overlaps current selecting volume precisely
  Standard_EXPORT void checkOverlap (const Handle(Select3D_SensitiveEntity)& theEntity,
                                     const gp_GTrsf& theInversedTrsf,
                                     SelectMgr_SelectingVolumeManager& theMgr);

  //! Update z-layers order map.
  Standard_EXPORT void updateZLayers (const Handle(V3d_View)& theView);

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


private:

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

  mutable TColStd_Array1OfInteger              myIndexes;
  mutable Standard_Boolean                     myIsSorted;
  Standard_Boolean                             myIsLeftChildQueuedFirst;
  SelectMgr_MapOfObjectSensitives              myMapOfObjectSensitives;

  Graphic3d_SequenceOfStructure                myStructs; //!< list of debug presentations

};

DEFINE_STANDARD_HANDLE(SelectMgr_ViewerSelector, Standard_Transient)

#endif
