// Created on: 1996-12-18
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_InteractiveContext_HeaderFile
#define _AIS_InteractiveContext_HeaderFile

#include <AIS_DataMapOfILC.hxx>
#include <AIS_DataMapOfIOStatus.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_DisplayStatus.hxx>
#include <AIS_ClearMode.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_Selection.hxx>
#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>
#include <AIS_TypeOfIso.hxx>
#include <Aspect_TypeOfFacingModel.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_TypeOfHighlight.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <SelectMgr_PickingStrategy.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Quantity_Color.hxx>

class SelectMgr_SelectionManager;
class V3d_Viewer;
class AIS_InteractiveObject;
class SelectMgr_OrFilter;
class V3d_View;
class AIS_LocalContext;
class TopLoc_Location;
class TCollection_ExtendedString;
class Prs3d_LineAspect;
class Prs3d_BasicAspect;
class SelectMgr_EntityOwner;
class SelectMgr_Filter;
class TCollection_AsciiString;

//! The Interactive Context allows you to manage graphic behavior and selection of Interactive Objects in one or more viewers.
//! Class methods make this highly transparent.
//! It is essential to remember that an Interactive Object which is already known by the Interactive Context must be modified using Context methods.
//! You can only directly call the methods available for an Interactive Object if it has not been loaded into an Interactive Context.
//!
//! Each selectable object must specify the selection mode that is
//! responsible for selection of object as a whole (global selection mode).
//! Interactive context itself supports decomposed object selection with selection filters support.
//! By default, global selection mode is equal to 0, but it might be redefined if needed.
class AIS_InteractiveContext : public Standard_Transient
{
  friend class AIS_LocalContext;
  DEFINE_STANDARD_RTTIEXT(AIS_InteractiveContext, Standard_Transient)
public: //! @name object display management

  //! Constructs the interactive context object defined by the principal viewer MainViewer.
  Standard_EXPORT AIS_InteractiveContext(const Handle(V3d_Viewer)& MainViewer);

  //! Destructor.
  Standard_EXPORT virtual ~AIS_InteractiveContext();

  //! Returns the display status of the entity anIobj.
  //! This will be one of the following:
  //! - AIS_DS_Displayed displayed in main viewer
  //! - AIS_DS_Erased    hidden in main viewer
  //! - AIS_DS_Temporary temporarily displayed
  //! - AIS_DS_None      nowhere displayed.
  Standard_EXPORT AIS_DisplayStatus DisplayStatus (const Handle(AIS_InteractiveObject)& anIobj) const;

  //! Returns the status of the Interactive Context for the view of the Interactive Object.
  Standard_EXPORT void Status (const Handle(AIS_InteractiveObject)& anObj, TCollection_ExtendedString& astatus) const;

  //! Returns true if Object is displayed in the interactive context.
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(AIS_InteractiveObject)& anIobj) const;

  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Integer aMode) const;

  //! Enable or disable automatic activation of default selection mode while displaying the object.
  void SetAutoActivateSelection (const Standard_Boolean theIsAuto) { myIsAutoActivateSelMode = theIsAuto; }

  //! Manages displaying the new object should also automatically activate default selection mode; TRUE by default.
  Standard_Boolean GetAutoActivateSelection() const { return myIsAutoActivateSelMode; }

  //! Displays the object in this Context using default Display Mode.
  //! This will be the object's default display mode, if there is one. Otherwise, it will be the context mode.
  //! The Interactive Object's default selection mode is activated if GetAutoActivateSelection() is TRUE. In general, this is 0.
  Standard_EXPORT void Display (const Handle(AIS_InteractiveObject)& theIObj,
                                const Standard_Boolean               theToUpdateViewer);

  //! Sets status, display mode and selection mode for specified Object
  //! If theSelectionMode equals -1, theIObj will not be activated: it will be displayed but will not be selectable.
  Standard_EXPORT void Display (const Handle(AIS_InteractiveObject)& theIObj,
                                const Standard_Integer               theDispMode,
                                const Standard_Integer               theSelectionMode,
                                const Standard_Boolean               theToUpdateViewer,
                                const Standard_Boolean               theToAllowDecomposition = Standard_True,
                                const AIS_DisplayStatus              theDispStatus = AIS_DS_None);

  //! Allows you to load the Interactive Object with a given selection mode,
  //! and/or with the desired decomposition option, whether the object is visualized or not.
  //! If AllowDecomp = Standard_True and, if the interactive object is of the "Shape" type,
  //! these "standard" selection modes will be automatically activated as a function of the modes present in the Local Context.
  //! The loaded objects will be selectable but displayable in highlighting only when detected by the Selector.
  //! This method is available only when Local Contexts are open.
  Standard_EXPORT void Load (const Handle(AIS_InteractiveObject)& aniobj, const Standard_Integer SelectionMode = -1, const Standard_Boolean AllowDecomp = Standard_False);

  //! Hides the object. The object's presentations are simply flagged as invisible and therefore excluded from redrawing.
  //! To show hidden objects, use Display().
  Standard_EXPORT void Erase (const Handle(AIS_InteractiveObject)& theIObj,
                              const Standard_Boolean               theToUpdateViewer);
  
  //! Hides all objects. The object's presentations are simply flagged as invisible and therefore excluded from redrawing.
  //! To show all hidden objects, use DisplayAll().
  Standard_EXPORT void EraseAll (const Standard_Boolean theToUpdateViewer);
  
  //! Displays all hidden objects.
  Standard_EXPORT void DisplayAll (const Standard_Boolean theToUpdateViewer);

  //! Hides selected objects. The object's presentations are simply flagged as invisible and therefore excluded from redrawing.
  //! To show hidden objects, use Display().
  Standard_EXPORT void EraseSelected (const Standard_Boolean theToUpdateViewer);

  //! Displays current objects.
  Standard_EXPORT void DisplaySelected (const Standard_Boolean theToUpdateViewer);
  
  //! Empties the graphic presentation of the mode indexed by aMode.
  //! Warning! Removes theIObj. theIObj is still active if it was previously activated.
  Standard_EXPORT void ClearPrs (const Handle(AIS_InteractiveObject)& theIObj,
                                 const Standard_Integer               theMode,
                                 const Standard_Boolean               theToUpdateViewer);

  //! Removes Object from every viewer.
  Standard_EXPORT void Remove (const Handle(AIS_InteractiveObject)& theIObj,
                               const Standard_Boolean               theToUpdateViewer);

  //! Removes all the objects from all opened Local Contexts and from the Neutral Point.
  Standard_EXPORT void RemoveAll (const Standard_Boolean theToUpdateViewer);

  //! Recomputes the seen parts presentation of the Object.
  //! If theAllModes equals true, all presentations are present in the object even if unseen.
  Standard_EXPORT void Redisplay (const Handle(AIS_InteractiveObject)& theIObj,
                                  const Standard_Boolean               theToUpdateViewer,
                                  const Standard_Boolean               theAllModes = Standard_False);

  //! Recomputes the Prs/Selection of displayed objects of a given type and a given signature.
  //! if signature = -1  doesn't take signature criterion.
  Standard_EXPORT void Redisplay (const AIS_KindOfInteractive theTypeOfObject,
                                  const Standard_Integer      theSignature,
                                  const Standard_Boolean      theToUpdateViewer);

  //! Recomputes the displayed presentations, flags the others.
  //! Doesn't update presentations.
  Standard_EXPORT void RecomputePrsOnly (const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateViewer,
                                         const Standard_Boolean               theAllModes = Standard_False);

  //! Recomputes the active selections, flags the others.
  //! Doesn't update presentations.
  Standard_EXPORT void RecomputeSelectionOnly (const Handle(AIS_InteractiveObject)& anIObj);

  //! Updates displayed interactive object by checking and recomputing its flagged as "to be recomputed" presentation and selection structures.
  //! This method does not force any recomputation on its own.
  //! The method recomputes selections even if they are loaded without activation in particular selector.
  Standard_EXPORT void Update (const Handle(AIS_InteractiveObject)& theIObj,
                               const Standard_Boolean               theUpdateViewer);

public: //! @name highlighting management

  //! Returns highlight style settings.
  const Handle(Prs3d_Drawer)& HighlightStyle (const Prs3d_TypeOfHighlight theStyleType) const { return myStyles[theStyleType]; }

  //! Setup highlight style settings.
  void SetHighlightStyle (const Prs3d_TypeOfHighlight theStyleType,
                          const Handle(Prs3d_Drawer)& theStyle) { myStyles[theStyleType] = theStyle; }

  //! Returns current dynamic highlight style settings.
  //! By default:
  //!   - the color of dynamic highlight is Quantity_NOC_CYAN1;
  //!   - the presentation for dynamic highlight is completely opaque;
  //!   - the type of highlight is Aspect_TOHM_COLOR.
  const Handle(Prs3d_Drawer)& HighlightStyle() const
  {
    return myStyles[Prs3d_TypeOfHighlight_Dynamic];
  }

  //! Setup the style of dynamic highlighting.
  void SetHighlightStyle (const Handle(Prs3d_Drawer)& theStyle) { myStyles[Prs3d_TypeOfHighlight_Dynamic] = theStyle; }

  //! Returns current selection style settings.
  //! By default:
  //!   - the color of selection is Quantity_NOC_GRAY80;
  //!   - the presentation for selection is completely opaque;
  //!   - the type of highlight is Aspect_TOHM_COLOR.
  const Handle(Prs3d_Drawer)& SelectionStyle() const
  {
    return myStyles[Prs3d_TypeOfHighlight_Selected];
  }

  //! Setup the style of selection highlighting.
  void SetSelectionStyle (const Handle(Prs3d_Drawer)& theStyle) { myStyles[Prs3d_TypeOfHighlight_Selected] = theStyle; }

  //! Returns highlight style of the object if it is marked as highlighted via global status
  //! @param theObj [in] the object to check
  Standard_EXPORT Standard_Boolean HighlightStyle (const Handle(AIS_InteractiveObject)& theObj,
                                                   Handle(Prs3d_Drawer)& theStyle) const;

  //! Returns highlight style of the owner if it is selected
  //! @param theOwner [in] the owner to check
  Standard_EXPORT Standard_Boolean HighlightStyle (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                   Handle(Prs3d_Drawer)& theStyle) const;

  //! Returns true if the object is marked as highlighted via its global status
  //! @param theObj [in] the object to check
  Standard_EXPORT Standard_Boolean IsHilighted (const Handle(AIS_InteractiveObject)& theObj) const;

  //! Returns true if the owner is marked as selected
  //! @param theOwner [in] the owner to check
  Standard_EXPORT Standard_Boolean IsHilighted (const Handle(SelectMgr_EntityOwner)& theOwner) const;

  //! Updates the display in the viewer to take dynamic detection into account.
  //! On dynamic detection by the mouse cursor, sensitive primitives are highlighted.
  //! The highlight color of entities detected by mouse movement is white by default.
  Standard_DEPRECATED("Deprecated method Hilight()")
  void Hilight (const Handle(AIS_InteractiveObject)& theObj,
                const Standard_Boolean               theIsToUpdateViewer)
  {
    return HilightWithColor (theObj, myStyles[Prs3d_TypeOfHighlight_Dynamic], theIsToUpdateViewer);
  }

  //! Changes the color of all the lines of the object in view.
  Standard_EXPORT void HilightWithColor (const Handle(AIS_InteractiveObject)& theObj,
                                         const Handle(Prs3d_Drawer)&          theStyle,
                                         const Standard_Boolean               theToUpdateViewer);

  //! Removes hilighting from the Object.
  Standard_EXPORT void Unhilight (const Handle(AIS_InteractiveObject)& theIObj,
                                  const Standard_Boolean               theToUpdateViewer);

public: //! @name object presence management (View affinity, Layer, Priority)

  //! Returns the display priority of the Object.
  Standard_EXPORT Standard_Integer DisplayPriority (const Handle(AIS_InteractiveObject)& theIObj) const;

  //! Sets the display priority of the seen parts presentation of the Object.
  Standard_EXPORT void SetDisplayPriority (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Integer thePriority);

  //! Get Z layer id set for displayed interactive object.
  Standard_EXPORT Standard_Integer GetZLayer (const Handle(AIS_InteractiveObject)& theIObj) const;

  //! Set Z layer id for interactive object.
  //! The Z layers can be used to display temporarily presentations of some object in front of the other objects in the scene.
  //! The ids for Z layers are generated by V3d_Viewer.
  Standard_EXPORT void SetZLayer (const Handle(AIS_InteractiveObject)& theIObj, const Standard_Integer theLayerId);

  //! Setup object visibility in specified view.
  //! Has no effect if object is not displayed in this context.
  Standard_EXPORT void SetViewAffinity (const Handle(AIS_InteractiveObject)& theIObj, const Handle(V3d_View)& theView, const Standard_Boolean theIsVisible);

public: //! @name Display Mode management

  //! Returns the Display Mode setting to be used by default.
  Standard_Integer DisplayMode() const { return myDefaultDrawer->DisplayMode(); }

  //! Sets the display mode of seen Interactive Objects (which have no overridden Display Mode).
  Standard_EXPORT void SetDisplayMode (const Standard_Integer theMode,
                                       const Standard_Boolean theToUpdateViewer);

  //! Sets the display mode of seen Interactive Objects.
  //! theMode provides the display mode index of the entity theIObj.
  Standard_EXPORT void SetDisplayMode (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Standard_Integer               theMode,
                                       const Standard_Boolean               theToUpdateViewer);

  //! Unsets the display mode of seen Interactive Objects.
  Standard_EXPORT void UnsetDisplayMode (const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateViewer);

public: //! @name object local transformation management

  //! Puts the location on the initial graphic representation and the selection for the Object.
  Standard_EXPORT void SetLocation (const Handle(AIS_InteractiveObject)& theObject, const TopLoc_Location& theLocation);

  //! Puts the Object back into its initial position.
  Standard_EXPORT void ResetLocation (const Handle(AIS_InteractiveObject)& theObject);

  //! Returns true if the Object has a location.
  Standard_EXPORT Standard_Boolean HasLocation (const Handle(AIS_InteractiveObject)& theObject) const;

  //! Returns the location of the Object.
  Standard_EXPORT TopLoc_Location Location (const Handle(AIS_InteractiveObject)& theObject) const;

  //! Sets transform persistence.
  Standard_EXPORT void SetTransformPersistence (const Handle(AIS_InteractiveObject)& theObject,
                                                const Handle(Graphic3d_TransformPers)& theTrsfPers);

  Standard_DEPRECATED("This method is deprecated - SetTransformPersistence() taking Graphic3d_TransformPers should be called instead")
  void SetTransformPersistence (const Handle(AIS_InteractiveObject)& theObj,
                                const Graphic3d_TransModeFlags&      theFlag,
                                const gp_Pnt&                        thePoint = gp_Pnt (0.0, 0.0, 0.0))
  {
    SetTransformPersistence (theObj, Graphic3d_TransformPers::FromDeprecatedParams (theFlag, thePoint));
  }

public: //! @name mouse picking logic (detection and dynamic highlighting of entities under cursor)

  //! Setup pixel tolerance for MoveTo() operation.
  //! @sa MoveTo().
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer thePrecision = 2);

  //! Returns the pixel tolerance, default is 2.
  //! Pixel Tolerance extends sensitivity within MoveTo() operation (picking by point)
  //! and can be adjusted by application based on user input precision (e.g. screen pixel density, input device precision, etc.).
  Standard_EXPORT Standard_Integer PixelTolerance() const;

  //! Allows to manage sensitivity of a particular selection of interactive object theObject
  //! and changes previous sensitivity value of all sensitive entities in selection with theMode
  //! to the given theNewSensitivity.
  Standard_EXPORT void SetSelectionSensitivity (const Handle(AIS_InteractiveObject)& theObject,
                                                const Standard_Integer theMode,
                                                const Standard_Integer theNewSensitivity);

  //! Relays mouse position in pixels theXPix and theYPix to the interactive context selectors.
  //! This is done by the view theView passing this position to the main viewer and updating it.
  //! Functions in both Neutral Point and local contexts.
  //! If theToRedrawOnUpdate is set to false, callee should call RedrawImmediate() to highlight detected object.
  //! @sa PickingStrategy()
  Standard_EXPORT AIS_StatusOfDetection MoveTo (const Standard_Integer  theXPix,
                                                const Standard_Integer  theYPix,
                                                const Handle(V3d_View)& theView,
                                                const Standard_Boolean  theToRedrawOnUpdate);

  //! Returns true if there is a mouse-detected entity in context.
  //! @sa DetectedOwner()/HasNextDetected()/HilightPreviousDetected()/HilightNextDetected().
  Standard_EXPORT Standard_Boolean HasDetected() const;

  //! Returns the owner of the detected sensitive primitive which is currently dynamically highlighted.
  //! WARNING! This method is irrelevant to InitDetected()/MoreDetected()/NextDetected().
  //! @sa HasDetected()/HasNextDetected()/HilightPreviousDetected()/HilightNextDetected().
  Standard_EXPORT Handle(SelectMgr_EntityOwner) DetectedOwner() const;

  //! Returns the interactive objects last detected in context.
  //! In general this is just a wrapper for Handle(AIS_InteractiveObject)::DownCast(DetectedOwner()->Selectable()).
  //! @sa DetectedOwner()
  Standard_EXPORT Handle(AIS_InteractiveObject) DetectedInteractive() const;

  //! Returns true if there is a detected shape in local context.
  //! @sa HasDetected()/DetectedShape()
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Boolean HasDetectedShape() const;

  //! Returns the shape detected in local context.
  //! @sa DetectedOwner()
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT const TopoDS_Shape& DetectedShape() const;
  
  //! returns True if other entities were detected in the last mouse detection
  //! @sa HilightPreviousDetected()/HilightNextDetected().
  Standard_EXPORT Standard_Boolean HasNextDetected() const;

  //! If more than 1 object is detected by the selector, only the "best" owner is hilighted at the mouse position.
  //! This Method allows the user to hilight one after another the other detected entities.
  //! If The method select is called, the selected entity will be the hilighted one!
  //! WARNING: Loop Method. When all the detected entities have been hilighted, the next call will hilight the first one again.
  //! @return the Rank of hilighted entity
  //! @sa HasNextDetected()/HilightPreviousDetected().
  Standard_EXPORT Standard_Integer HilightNextDetected (const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate = Standard_True);

  //! Same as previous methods in reverse direction.
  //! @sa HasNextDetected()/HilightNextDetected().
  Standard_EXPORT Standard_Integer HilightPreviousDetected (const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate = Standard_True);

public: //! @name iteration through detected entities

  //! Initialization for iteration through mouse-detected objects in
  //! interactive context or in local context if it is opened.
  //! @sa DetectedCurrentOwner()/MoreDetected()/NextDetected().
  Standard_EXPORT void InitDetected();

  //! Return TRUE if there is more mouse-detected objects after the current one
  //! during iteration through mouse-detected interactive objects.
  //! @sa DetectedCurrentOwner()/InitDetected()/NextDetected().
  Standard_EXPORT Standard_Boolean MoreDetected() const;

  //! Gets next current object during iteration through mouse-detected interactive objects.
  //! @sa DetectedCurrentOwner()/InitDetected()/MoreDetected().
  Standard_EXPORT void NextDetected();

  //! Returns the owner from detected list pointed by current iterator position.
  //! WARNING! This method is irrelevant to DetectedOwner() which returns last picked Owner regardless of iterator position!
  //! @sa InitDetected()/MoreDetected()/NextDetected().
  Standard_EXPORT Handle(SelectMgr_EntityOwner) DetectedCurrentOwner() const;

public: //! @name Selection management

  //! Sets the graphic basic aspect to the current presentation of ALL selected objects.
  Standard_EXPORT void SetSelectedAspect (const Handle(Prs3d_BasicAspect)& theAspect,
                                          const Standard_Boolean           theToUpdateViewer);

  //! Adds object in the selection.
  Standard_EXPORT AIS_StatusOfPick AddSelect (const Handle(SelectMgr_EntityOwner)& theObject);

  //! Adds object in the selection.
  AIS_StatusOfPick AddSelect (const Handle(AIS_InteractiveObject)& theObject)
  {
    return AddSelect (theObject->GlobalSelOwner());
  }

  //! Selects everything found in the bounding rectangle defined by the pixel minima and maxima, XPMin, YPMin, XPMax, and YPMax in the view.
  //! The objects detected are passed to the main viewer, which is then updated.
  Standard_EXPORT AIS_StatusOfPick Select (const Standard_Integer  theXPMin,
                                           const Standard_Integer  theYPMin,
                                           const Standard_Integer  theXPMax,
                                           const Standard_Integer  theYPMax,
                                           const Handle(V3d_View)& theView,
                                           const Standard_Boolean  theToUpdateViewer);
  
  //! polyline selection; clears the previous picked list
  Standard_EXPORT AIS_StatusOfPick Select (const TColgp_Array1OfPnt2d& thePolyline,
                                           const Handle(V3d_View)&     theView,
                                           const Standard_Boolean      theToUpdateViewer);

  //! Stores and hilights the previous detected; Unhilights the previous picked.
  //! @sa MoveTo().
  Standard_EXPORT AIS_StatusOfPick Select (const Standard_Boolean theToUpdateViewer);

  //! Adds the last detected to the list of previous picked.
  //! If the last detected was already declared as picked, removes it from the Picked List.
  //! @sa MoveTo().
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const Standard_Boolean theToUpdateViewer);

  //! Adds the last detected to the list of previous picked.
  //! If the last detected was already declared as picked, removes it from the Picked List.
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const TColgp_Array1OfPnt2d& thePolyline,
                                                const Handle(V3d_View)&     theView,
                                                const Standard_Boolean      theToUpdateViewer);

  //! Rectangle of selection; adds new detected entities into the picked list,
  //! removes the detected entities that were already stored.
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const Standard_Integer  theXPMin,
                                                const Standard_Integer  theYPMin,
                                                const Standard_Integer  theXPMax,
                                                const Standard_Integer  theYPMax,
                                                const Handle(V3d_View)& theView,
                                                const Standard_Boolean  theToUpdateViewer);

  //! Fits the view correspondingly to the bounds of selected objects.
  //! Infinite objects are ignored if infinite state of AIS_InteractiveObject is set to true.
  Standard_EXPORT void FitSelected (const Handle(V3d_View)& theView,
                                    const Standard_Real     theMargin,
                                    const Standard_Boolean  theToUpdate);

  //! Fits the view correspondingly to the bounds of selected objects.
  //! Infinite objects are ignored if infinite state of AIS_InteractiveObject is set to true.
  Standard_EXPORT void FitSelected (const Handle(V3d_View)& theView);

  //! Specify whether selected object must be hilighted when mouse cursor is moved above it (in MoveTo method).
  //! By default this value is false and selected object is not hilighted in this case.
  void SetToHilightSelected (const Standard_Boolean toHilight) { myToHilightSelected = toHilight; }

  //! Return value specified whether selected object must be hilighted when mouse cursor is moved above it
  Standard_Boolean ToHilightSelected() const { return myToHilightSelected; }

  //! Unhighlights previously selected owners and marks them as not selected.
  //! Marks owner given as selected and highlights it.
  //! Performs selection filters check.
  Standard_EXPORT void SetSelected (const Handle(SelectMgr_EntityOwner)& theOwners,
                                    const Standard_Boolean               theToUpdateViewer);

  //! Puts the interactive object aniObj in the list of selected objects.
  //! Performs selection filters check.
  Standard_EXPORT void SetSelected (const Handle(AIS_InteractiveObject)& theObject,
                                    const Standard_Boolean               theToUpdateViewer);

  //! Allows to highlight or unhighlight the owner given depending on its selection status
  Standard_EXPORT void AddOrRemoveSelected (const Handle(AIS_InteractiveObject)& theObject,
                                            const Standard_Boolean               theToUpdateViewer);

  //! Highlights selected objects.
  Standard_EXPORT void HilightSelected (const Standard_Boolean theToUpdateViewer);

  //! Removes highlighting from selected objects.
  Standard_EXPORT void UnhilightSelected (const Standard_Boolean theToUpdateViewer);

  //! Updates the list of selected objects:
  //! i.e. highlights the newely selected ones and unhighlights previously selected objects.
  //! @sa HilightSelected().
  Standard_EXPORT void UpdateSelected (const Standard_Boolean theToUpdateViewer);

  //! Empties previous selected objects in order to get the selected objects detected by the selector using UpdateSelected.
  Standard_EXPORT void ClearSelected (const Standard_Boolean theToUpdateViewer);
  
  //! No right to Add a selected Shape (Internal Management of shape Selection).
  //! A Previous selected shape may only be removed.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void AddOrRemoveSelected (const TopoDS_Shape&    theShape,
                                            const Standard_Boolean theToUpdateViewer);

  //! Allows to highlight or unhighlight the owner given depending on its selection status
  Standard_EXPORT void AddOrRemoveSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                                            const Standard_Boolean               theToUpdateViewer);

  //! Returns true is the owner given is selected
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(SelectMgr_EntityOwner)& theOwner) const;

  //! Returns true is the object given is selected
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(AIS_InteractiveObject)& theObj) const;

  //! Returns the first selected object in the list of current selected.
  Standard_EXPORT Handle(AIS_InteractiveObject) FirstSelectedObject();

  //! Count a number of selected entities using InitSelected()+MoreSelected()+NextSelected() iterator.
  //! @sa SelectedOwner()/InitSelected()/MoreSelected()/NextSelected().
  Standard_EXPORT Standard_Integer NbSelected();

  //! Initializes a scan of the selected objects.
  //! @sa SelectedOwner()/MoreSelected()/NextSelected().
  Standard_EXPORT void InitSelected();

  //! Returns true if there is another object found by the scan of the list of selected objects.
  //! @sa SelectedOwner()/InitSelected()/NextSelected().
  Standard_EXPORT Standard_Boolean MoreSelected() const;

  //! Continues the scan to the next object in the list of selected objects.
  //! @sa SelectedOwner()/InitSelected()/MoreSelected().
  Standard_EXPORT void NextSelected();

  //! Returns the owner of the selected entity.
  //! @sa InitSelected()/MoreSelected()/NextSelected().
  Standard_EXPORT Handle(SelectMgr_EntityOwner) SelectedOwner() const;

  //! Return Handle(AIS_InteractiveObject)::DownCast (SelectedOwner()->Selectable()).
  //! @sa SelectedOwner().
  Standard_EXPORT Handle(AIS_InteractiveObject) SelectedInteractive() const;

  //! Returns TRUE if the interactive context has a shape selected.
  //! @sa SelectedShape().
  Standard_EXPORT Standard_Boolean HasSelectedShape() const;

  //! Returns the selected shape.
  //! Basically it is just a shape returned stored by StdSelect_BRepOwner with graphic transformation being applied:
  //! @code
  //!   const Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast (SelectedOwner());
  //!   TopoDS_Shape aSelShape     = aBRepOwner->Shape();
  //!   TopoDS_Shape aLocatedShape = aSelShape.Located (aBRepOwner->Location() * aSelShape.Location());
  //! @endcode
  //! @sa SelectedOwner()/HasSelectedShape().
  Standard_EXPORT TopoDS_Shape SelectedShape() const;

  //! Returns SelectedInteractive()->HasOwner().
  //! @sa SelectedOwner().
  Standard_EXPORT Standard_Boolean HasApplicative() const;

  //! Returns SelectedInteractive()->GetOwner().
  //! @sa SelectedOwner().
  Standard_EXPORT Handle(Standard_Transient) Applicative() const;

public: //! @name immediate mode rendering

  //! initializes the list of presentations to be displayed
  //! returns False if no local context is opened.
  Standard_EXPORT Standard_Boolean BeginImmediateDraw();
  
  //! returns True if <anIObj> has been stored in the list.
  Standard_EXPORT Standard_Boolean ImmediateAdd (const Handle(AIS_InteractiveObject)& theObj, const Standard_Integer theMode = 0);
  
  //! returns True if the immediate display has been done.
  Standard_EXPORT Standard_Boolean EndImmediateDraw (const Handle(V3d_View)& theView);

  //! Uses the First Active View of Main Viewer!
  //! returns True if the immediate display has been done.
  Standard_EXPORT Standard_Boolean EndImmediateDraw();

  Standard_EXPORT Standard_Boolean IsImmediateModeOn() const;

  //! Redraws immediate structures in all views of the viewer given taking into account its visibility.
  Standard_EXPORT void RedrawImmediate (const Handle(V3d_Viewer)& theViewer);

public: //! @name management of active Selection Modes

  //! Activates the selection mode aMode whose index is given, for the given interactive entity anIobj.
  Standard_EXPORT void Activate (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Integer aMode = 0, const Standard_Boolean theIsForce = Standard_False);

  //! Activates the given selection mode for the all displayed objects.
  Standard_EXPORT void Activate (const Standard_Integer theMode,
                                 const Standard_Boolean theIsForce = Standard_False);
  
  //! Deactivates all the activated selection modes of an object.
  Standard_EXPORT void Deactivate (const Handle(AIS_InteractiveObject)& anIObj);

  //! Deactivates all the activated selection modes of the interactive object anIobj with a given selection mode aMode.
  Standard_EXPORT void Deactivate (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Integer aMode);

  //! Deactivates the given selection mode for all displayed objects.
  Standard_EXPORT void Deactivate (const Standard_Integer theMode);

  //! Deactivates all the activated selection mode at all displayed objects.
  Standard_EXPORT void Deactivate();

  //! Returns the list of activated selection modes in an open context.
  Standard_EXPORT void ActivatedModes (const Handle(AIS_InteractiveObject)& anIobj, TColStd_ListOfInteger& theList) const;

  //! Returns a collection containing all entity owners created for the interactive object in specified selection mode (in all active modes if the Mode == -1)
  Standard_EXPORT void EntityOwners (Handle(SelectMgr_IndexedMapOfOwner)& theOwners,
                                     const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Integer theMode = -1) const;

public: //! @name Selection Filters management

  //! Returns the list of filters active in a local context.
  Standard_EXPORT const SelectMgr_ListOfFilter& Filters() const;

  //! Allows you to add the filter.
  Standard_EXPORT void AddFilter (const Handle(SelectMgr_Filter)& theFilter);

  //! Removes a filter from context.
  Standard_EXPORT void RemoveFilter (const Handle(SelectMgr_Filter)& theFilter);

  //! Remove all filters from context.
  Standard_EXPORT void RemoveFilters();

  //! Return picking strategy; SelectMgr_PickingStrategy_FirstAcceptable by default.
  //! @sa MoveTo()/Filters()
  SelectMgr_PickingStrategy PickingStrategy() const { return myPickingStrategy; }

  //! Setup picking strategy - which entities detected by picking line will be accepted, considering Selection Filters.
  //! By default (SelectMgr_PickingStrategy_FirstAcceptable), Selection Filters reduce the list of entities
  //! so that the context accepts topmost in remaining.
  //!
  //! This means that entities behind non-selectable (by filters) parts can be picked by user.
  //! If this behavior is undesirable, and user wants that non-selectable (by filters) parts
  //! should remain an obstacle for picking, SelectMgr_PickingStrategy_OnlyTopmost can be set instead.
  //!
  //! Notice, that since Selection Manager operates only objects registered in it,
  //! SelectMgr_PickingStrategy_OnlyTopmost will NOT prevent picking entities behind
  //! visible by unregistered in Selection Manager presentations (e.g. deactivated).
  //! Hence, SelectMgr_PickingStrategy_OnlyTopmost changes behavior only with Selection Filters enabled.
  void SetPickingStrategy (const SelectMgr_PickingStrategy theStrategy)
  {
    myPickingStrategy = theStrategy;
  }

public: //! @name common properties

  //! Returns the default attribute manager.
  //! This contains all the color and line attributes which can be used by interactive objects which do not have their own attributes.
  const Handle(Prs3d_Drawer)& DefaultDrawer() const { return myDefaultDrawer; }

  //! Returns the current viewer.
  const Handle(V3d_Viewer)& CurrentViewer() const { return myMainVwr; }

  const Handle(SelectMgr_SelectionManager)& SelectionManager() const { return mgrSelector; }

  const Handle(PrsMgr_PresentationManager3d)& MainPrsMgr() const { return myMainPM; }

  const Handle(StdSelect_ViewerSelector3d)& MainSelector() const { return myMainSel; }

  //! Updates the current viewer.
  Standard_EXPORT void UpdateCurrentViewer();

  //! Returns the list of displayed objects of a particular Type WhichKind and Signature WhichSignature.
  //! By Default, WhichSignature equals -1. This means that there is a check on type only.
  Standard_EXPORT void DisplayedObjects (AIS_ListOfInteractive& aListOfIO, const Standard_Boolean OnlyFromNeutral = Standard_False) const;

  //! gives the list of displayed objects of a particular Type and signature.
  //! by Default, <WhichSignature> = -1 means control only on <WhichKind>.
  Standard_EXPORT void DisplayedObjects (const AIS_KindOfInteractive WhichKind, const Standard_Integer WhichSignature, AIS_ListOfInteractive& aListOfIO, const Standard_Boolean OnlyFromNeutral = Standard_False) const;

  //! Returns the list theListOfIO of erased objects (hidden objects) particular Type WhichKind and Signature WhichSignature.
  //! By Default, WhichSignature equals 1. This means that there is a check on type only.
  Standard_EXPORT void ErasedObjects (AIS_ListOfInteractive& theListOfIO) const;

  //! gives the list of erased objects (hidden objects)
  //! Type and signature by Default, <WhichSignature> = -1 means control only on <WhichKind>.
  Standard_EXPORT void ErasedObjects (const AIS_KindOfInteractive WhichKind, const Standard_Integer WhichSignature, AIS_ListOfInteractive& theListOfIO) const;

  //! Returns the list theListOfIO of objects with indicated display status particular Type WhichKind and Signature WhichSignature.
  //! By Default, WhichSignature equals 1. This means that there is a check on type only.
  Standard_EXPORT void ObjectsByDisplayStatus (const AIS_DisplayStatus theStatus, AIS_ListOfInteractive& theListOfIO) const;

  //! gives the list of objects with indicated display status
  //! Type and signature by Default, <WhichSignature> = -1 means control only on <WhichKind>.
  Standard_EXPORT void ObjectsByDisplayStatus (const AIS_KindOfInteractive WhichKind, const Standard_Integer WhichSignature, const AIS_DisplayStatus theStatus, AIS_ListOfInteractive& theListOfIO) const;
  
  //! fills <aListOfIO> with objects of a particular Type and Signature with no consideration of display status.
  //! by Default, <WhichSignature> = -1 means control only on <WhichKind>.
  //! if <WhichKind> = AIS_KOI_None and <WhichSignature> = -1, all the objects are put into the list.
  Standard_EXPORT void ObjectsInside (AIS_ListOfInteractive& aListOfIO, const AIS_KindOfInteractive WhichKind = AIS_KOI_None, const Standard_Integer WhichSignature = -1) const;

  //! Rebuilds 1st level of BVH selection forcibly
  Standard_EXPORT void RebuildSelectionStructs();

  //! Disconnects theObjToDisconnect from theAssembly and removes dependent selection structures
  Standard_EXPORT void Disconnect (const Handle(AIS_InteractiveObject)& theAssembly, const Handle(AIS_InteractiveObject)& theObjToDisconnect = NULL);

  //! Query objects visible or hidden in specified view due to affinity mask.
  Standard_EXPORT void ObjectsForView (AIS_ListOfInteractive& theListOfIO, const Handle(V3d_View)& theView, const Standard_Boolean theIsVisibleInView, const AIS_DisplayStatus theStatus = AIS_DS_None) const;

  //! Clears all the structures which don't belong to objects displayed at neutral point
  //! only effective when no Local Context is opened...
  //! returns the number of removed  structures from the viewers.
  Standard_EXPORT Standard_Integer PurgeDisplay();

public: //! @name debug visualization

  //! Visualization of sensitives - for debugging purposes!
  Standard_EXPORT void DisplayActiveSensitive (const Handle(V3d_View)& aView);

  //! Clear visualization of sensitives.
  Standard_EXPORT void ClearActiveSensitive (const Handle(V3d_View)& aView);

  //! Visualization of sensitives - for debugging purposes!
  Standard_EXPORT void DisplayActiveSensitive (const Handle(AIS_InteractiveObject)& anObject, const Handle(V3d_View)& aView);

public: //! @name common object display attributes

  //! Sets the graphic attributes of the interactive object, such as visualization mode, color, and material.
  Standard_EXPORT void SetLocalAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                           const Handle(Prs3d_Drawer)&          theDrawer,
                                           const Standard_Boolean               theToUpdateViewer);

  //! Removes the settings for local attributes of the Object and returns to defaults.
  Standard_EXPORT void UnsetLocalAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Boolean               theToUpdateViewer);

  //! change the current facing model apply on polygons for SetColor(), SetTransparency(), SetMaterial() methods default facing model is Aspect_TOFM_TWO_SIDE.
  //! This mean that attributes is applying both on the front and back face.
  Standard_EXPORT void SetCurrentFacingModel (const Handle(AIS_InteractiveObject)& aniobj, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);

  //! Returns true if a view of the Interactive Object has color.
  Standard_EXPORT Standard_Boolean HasColor (const Handle(AIS_InteractiveObject)& aniobj) const;

  //! Returns the color of the Object in the interactive context.
  Standard_EXPORT void Color (const Handle(AIS_InteractiveObject)& aniobj, Quantity_Color& acolor) const;

  //! Sets the color of the selected entity.
  Standard_EXPORT void SetColor (const Handle(AIS_InteractiveObject)& theIObj,
                                 const Quantity_Color&                theColor,
                                 const Standard_Boolean               theToUpdateViewer);

  //! Removes the color selection for the selected entity.
  Standard_EXPORT void UnsetColor (const Handle(AIS_InteractiveObject)& theIObj,
                                   const Standard_Boolean               theToUpdateViewer);

  //! Returns the width of the Interactive Object in the interactive context.
  Standard_EXPORT virtual Standard_Real Width (const Handle(AIS_InteractiveObject)& aniobj) const;

  //! Sets the width of the Object.
  Standard_EXPORT virtual void SetWidth (const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Real                  theValue,
                                         const Standard_Boolean               theToUpdateViewer);

  //! Removes the width setting of the Object.
  Standard_EXPORT virtual void UnsetWidth (const Handle(AIS_InteractiveObject)& theIObj,
                                           const Standard_Boolean               theToUpdateViewer);

  //! Provides the type of material setting for the view of the Object.
  Standard_EXPORT void SetMaterial (const Handle(AIS_InteractiveObject)& theIObj,
                                    const Graphic3d_MaterialAspect&      theMaterial,
                                    const Standard_Boolean               theToUpdateViewer);

  //! Removes the type of material setting for viewing the Object.
  Standard_EXPORT void UnsetMaterial (const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Boolean               theToUpdateViewer);
  
  //! Provides the transparency settings for viewing the Object.
  //! The transparency value aValue may be between 0.0, opaque, and 1.0, fully transparent.
  Standard_EXPORT void SetTransparency (const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Real                  theValue,
                                        const Standard_Boolean               theToUpdateViewer);

  //! Removes the transparency settings for viewing the Object.
  Standard_EXPORT void UnsetTransparency (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Boolean               theToUpdateViewer);

  //! Sets up polygon offsets for the given AIS_InteractiveObject.
  //! It simply calls AIS_InteractiveObject::SetPolygonOffsets().
  Standard_EXPORT void SetPolygonOffsets (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Integer               theMode,
                                          const Standard_ShortReal             theFactor,
                                          const Standard_ShortReal             theUnits,
                                          const Standard_Boolean               theToUpdateViewer);
  
  //! Simply calls AIS_InteractiveObject::HasPolygonOffsets().
  Standard_EXPORT Standard_Boolean HasPolygonOffsets (const Handle(AIS_InteractiveObject)& anObj) const;

  //! Retrieves current polygon offsets settings for Object.
  Standard_EXPORT void PolygonOffsets (const Handle(AIS_InteractiveObject)& anObj, Standard_Integer& aMode, Standard_ShortReal& aFactor, Standard_ShortReal& aUnits) const;

public: //! @name trihedron display attributes

  //! Sets the size aSize of the trihedron.
  //! Is used to change the default value 100 mm for display of trihedra.
  //! Use of this function in one of your own interactive objects requires a call to the Compute function of the new class.
  //! This will recalculate the presentation for every trihedron displayed.
  Standard_EXPORT void SetTrihedronSize (const Standard_Real    theSize,
                                         const Standard_Boolean theToUpdateViewer);

  //! returns the current value of trihedron size.
  Standard_EXPORT Standard_Real TrihedronSize() const;

public: //! @name plane display attributes

  //! Sets the plane size defined by the length in the X direction XSize and that in the Y direction YSize.
  Standard_EXPORT void SetPlaneSize (const Standard_Real    theSizeX,
                                     const Standard_Real    theSizeY,
                                     const Standard_Boolean theToUpdateViewer);

  //! Sets the plane size aSize.
  Standard_EXPORT void SetPlaneSize (const Standard_Real    theSize,
                                     const Standard_Boolean theToUpdateViewer);

  //! Returns true if the length in the X direction XSize is the same as that in the Y direction YSize.
  Standard_EXPORT Standard_Boolean PlaneSize (Standard_Real& XSize, Standard_Real& YSize) const;

public: //! @name tessellation deviation properties for automatic triangulation

  //! Sets the deviation coefficient theCoefficient.
  //! Drawings of curves or patches are made with respect to a maximal chordal deviation.
  //! A Deviation coefficient is used in the shading display mode.
  //! The shape is seen decomposed into triangles.
  //! These are used to calculate reflection of light from the surface of the object.
  //! The triangles are formed from chords of the curves in the shape.
  //! The deviation coefficient theCoefficient gives the highest value of the angle with which a chord can deviate from a tangent to a curve.
  //! If this limit is reached, a new triangle is begun.
  //! This deviation is absolute and is set through the method: SetMaximalChordialDeviation.
  //! The default value is 0.001.
  //! In drawing shapes, however, you are allowed to ask for a relative deviation.
  //! This deviation will be: SizeOfObject * DeviationCoefficient.
  Standard_EXPORT void SetDeviationCoefficient (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Real                  theCoefficient,
                                                const Standard_Boolean               theToUpdateViewer);

  Standard_EXPORT void SetDeviationAngle (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Real                  theAngle,
                                          const Standard_Boolean               theToUpdateViewer);
  
  //! Calls the AIS_Shape SetAngleAndDeviation to set both Angle and Deviation coefficients
  Standard_EXPORT void SetAngleAndDeviation (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Real                  theAngle,
                                             const Standard_Boolean               theToUpdateViewer);
  

  //! Sets the deviation coefficient aCoefficient for removal of hidden lines created by different viewpoints in different presentations.
  //! The Default value is 0.02.
  Standard_EXPORT void SetHLRDeviationCoefficient (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Real                  theCoefficient,
                                                   const Standard_Boolean               theToUpdateViewer);
  
  Standard_EXPORT void SetHLRDeviationAngle (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Real                  theAngle,
                                             const Standard_Boolean               theToUpdateViewer);
  
  //! Computes a HLRAngle and a HLRDeviationCoefficient by means of the angle anAngle
  //! and sets the corresponding methods in the default drawing tool with these values.
  Standard_EXPORT void SetHLRAngleAndDeviation (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Real                  theAngle,
                                                const Standard_Boolean               theToUpdateViewer);

  //! Sets the deviation coefficient theCoefficient.
  //! Drawings of curves or patches are made with respect to a maximal chordal deviation.
  //! A Deviation coefficient is used in the shading display mode.
  //! The shape is seen decomposed into triangles.
  //! These are used to calculate reflection of light from the surface of the object.
  //! The triangles are formed from chords of the curves in the shape.
  //! The deviation coefficient theCoefficient gives the highest value of the angle with which a chord can deviate from a tangent to a curve.
  //! If this limit is reached, a new triangle is begun.
  //! This deviation is absolute and is set through the method: SetMaximalChordialDeviation.
  //! The default value is 0.001.
  //! In drawing shapes, however, you are allowed to ask for a relative deviation.
  //! This deviation will be: SizeOfObject * DeviationCoefficient.
  Standard_EXPORT void SetDeviationCoefficient (const Standard_Real theCoefficient);
  
  //! Returns the deviation coefficient.
  //! Drawings of curves or patches are made with respect to a maximal chordal deviation.
  //! A Deviation coefficient is used in the shading display mode.
  //! The shape is seen decomposed into triangles.
  //! These are used to calculate reflection of light from the surface of the object.
  //! The triangles are formed from chords of the curves in the shape.
  //! The deviation coefficient gives the highest value of the angle with which a chord can deviate from a tangent to a curve.
  //! If this limit is reached, a new triangle is begun.
  //! This deviation is absolute and is set through Prs3d_Drawer::SetMaximalChordialDeviation.
  //! The default value is 0.001.
  //! In drawing shapes, however, you are allowed to ask for a relative deviation.
  //! This deviation will be: SizeOfObject * DeviationCoefficient.
  Standard_EXPORT Standard_Real DeviationCoefficient() const;

  //! default 12 degrees
  Standard_EXPORT void SetDeviationAngle (const Standard_Real anAngle);

  Standard_EXPORT Standard_Real DeviationAngle() const;
  
  //! Sets the deviation coefficient aCoefficient for removal of hidden lines created by different viewpoints in different presentations.
  //! The Default value is 0.02.
  Standard_EXPORT void SetHLRDeviationCoefficient (const Standard_Real aCoefficient);

  //! Returns the real number value of the hidden line removal deviation coefficient.
  //! A Deviation coefficient is used in the shading display mode.
  //! The shape is seen decomposed into triangles.
  //! These are used to calculate reflection of light from the surface of the object.
  //! The triangles are formed from chords of the curves in the shape.
  //! The deviation coefficient give the highest value of the angle with which a chord can deviate from a tangent to a curve.
  //! If this limit is reached, a new triangle is begun.
  //! To find the hidden lines, hidden line display mode entails recalculation of the view at each different projector perspective.
  //! Because hidden lines entail calculations of more than usual complexity to decompose them into these triangles,
  //! a deviation coefficient allowing greater tolerance is used.
  //! This increases efficiency in calculation.
  //! The Default value is 0.02.
  Standard_EXPORT Standard_Real HLRDeviationCoefficient() const;

  //! Sets the HLR angle.
  Standard_EXPORT void SetHLRAngle (const Standard_Real theAngle);

  //! Returns the real number value of the deviation angle in hidden line removal views in this interactive context.
  //! The default value is 20*PI/180.
  Standard_EXPORT Standard_Real HLRAngle() const;

  //! compute with theAngle a HLRAngle and a HLRDeviationCoefficient
  //! and set them in myHLRAngle and in myHLRDeviationCoefficient of myDefaultDrawer;
  //! theAngle is in radian; ( 1 deg < angle in deg < 20 deg)
  Standard_EXPORT void SetHLRAngleAndDeviation (const Standard_Real theAngle);

public: //! @name HLR (Hidden Line Removal) display attributes

  //! Initializes hidden line aspect in the default drawing tool, or Drawer.
  //! The default values are:
  //! Color: Quantity_NOC_YELLOW
  //! Type of line: Aspect_TOL_DASH
  //! Width: 1.
  Standard_EXPORT Handle(Prs3d_LineAspect) HiddenLineAspect() const;

  //! Sets the hidden line aspect anAspect.
  //! Aspect defines display attributes for hidden lines in HLR projections.
  Standard_EXPORT void SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& anAspect) const;

  //! returns Standard_True if the hidden lines are to be drawn.
  //! By default the hidden lines are not drawn.
  Standard_EXPORT Standard_Boolean DrawHiddenLine() const;

  Standard_EXPORT void EnableDrawHiddenLine() const;

  Standard_EXPORT void DisableDrawHiddenLine() const;

public: //! @name iso-line display attributes

  //! Sets the number of U and V isoparameters displayed.
  Standard_EXPORT void SetIsoNumber (const Standard_Integer NbIsos, const AIS_TypeOfIso WhichIsos = AIS_TOI_Both);
  
  //! Returns the number of U and V isoparameters displayed.
  Standard_EXPORT Standard_Integer IsoNumber (const AIS_TypeOfIso WhichIsos = AIS_TOI_Both);
  
  //! Returns True if drawing isoparameters on planes is enabled.
  Standard_EXPORT void IsoOnPlane (const Standard_Boolean SwitchOn);
  
  //! Returns True if drawing isoparameters on planes is enabled.
  //! if <forUIsos> = False,
  Standard_EXPORT Standard_Boolean IsoOnPlane() const;

  //! Enables or disables on-triangulation build for isolines for a particular object.
  //! In case if on-triangulation builder is disabled, default on-plane builder will compute isolines for the object given.
  Standard_EXPORT void IsoOnTriangulation (const Standard_Boolean theIsEnabled,
                                           const Handle(AIS_InteractiveObject)& theObject);

  //! Enables or disables on-triangulation build for isolines for default drawer.
  //! In case if on-triangulation builder is disabled, default on-plane builder will compute isolines for the object given.
  Standard_EXPORT void IsoOnTriangulation (const Standard_Boolean theToSwitchOn);

  //! Returns true if drawing isolines on triangulation algorithm is enabled.
  Standard_EXPORT Standard_Boolean IsoOnTriangulation() const;

public: //! @name Local Context management (deprecated)

  //! Returns true if there is an open context.
  Standard_Boolean HasOpenedContext() const { return myCurLocalIndex != 0; }

  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Integer HighestIndex() const;

  //! For advanced usage! You should use other (non-internal) methods of class AIS_InteractiveContext without trying to obtain an instance of AIS_LocalContext.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Handle(AIS_LocalContext) LocalContext() const { return myCurLocalIndex > 0 ? myLocalContexts (myCurLocalIndex) : Handle(AIS_LocalContext)(); }

  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Handle(StdSelect_ViewerSelector3d) LocalSelector() const;

  //! Opens local contexts and specifies how this is to be done.
  //! The options listed above function in the following manner:
  //! - UseDisplayedObjects - allows you to load or not load the interactive objects visualized at Neutral Point in the local context which you open.
  //!   If false, the local context is empty after being opened. If true, the objects at Neutral Point are loaded by their default selection mode.
  //! - AllowShapeDecomposition - AIS_Shape allows or prevents decomposition in standard shape location mode of objects at Neutral Point which are type-"privileged".
  //!   This Flag is only taken into account when UseDisplayedObjects is true.
  //! - AcceptEraseOfObjects - authorises other local contexts to erase the interactive objects present in this context. This option is rarely used.
  //! - BothViewers - Has no use currently defined.
  //! This method returns the index of the created local context.
  //! It should be kept and used to close the context.
  //! Opening a local context allows you to prepare an environment for temporary presentations and selections which will disappear once the local context is closed.
  //! You can open several local contexts, but only the last one will be active.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Integer OpenLocalContext (const Standard_Boolean UseDisplayedObjects = Standard_True, const Standard_Boolean AllowShapeDecomposition = Standard_True, const Standard_Boolean AcceptEraseOfObjects = Standard_False, const Standard_Boolean BothViewers = Standard_False);

  //! Allows you to close local contexts. For greater security, you should close the context with the index Index given on opening.
  //! When you close a local context, the one before, which is still on the stack, reactivates.
  //! If none is left, you return to Neutral Point.
  //! If a local context is open and if updateviewer equals Standard_False, the presentation of the Interactive Object activates the selection mode;
  //! the object is displayed but no viewer will be updated.
  //! Warning
  //! When the index isn't specified, the current context is closed.
  //! This option can be dangerous, as other Interactive Functions can open local contexts without necessarily warning the user.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void CloseLocalContext (const Standard_Integer theIndex = -1,
                                          const Standard_Boolean theToUpdateViewer = Standard_True);

  //! returns -1 if no opened local context.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Integer IndexOfCurrentLocal() const;

  //! Allows you to close all local contexts at one go and return to Neutral Point.
  //! If a local context is open and if updateviewer equals Standard_False, the presentation of the Interactive Object activates the selection mode;
  //! the object is displayed but no viewer will be updated.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void CloseAllContexts (const Standard_Boolean theToUpdateViewer);

  //! to be used only with no opened local context..
  //! displays and activates objects in their original state before local contexts were opened...
  Standard_EXPORT void ResetOriginalState (const Standard_Boolean theToUpdateViewer);
  
  //! clears Objects/Filters/Activated Modes list in the current opened local context.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void ClearLocalContext (const AIS_ClearMode TheMode = AIS_CM_All);

  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void UseDisplayedObjects();

  //! when a local Context is opened, one is able to use/not use the displayed objects at neutral point at anytime.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void NotUseDisplayedObjects();

  //! to be Used only with opened local context and
  //! if <anIobj> is of type shape...
  //! if <aStatus> = True <anIobj> will be sensitive to
  //! shape selection modes activation.
  //! = False, <anIobj> will not be sensitive any more.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void SetShapeDecomposition (const Handle(AIS_InteractiveObject)& anIobj, const Standard_Boolean aStatus);
  

  //! Sets the temporary graphic attributes of the entity anObj.
  //! These are provided by the attribute manager aDrawer and are valid for a particular local context only.
  //! If a local context is open and if updateviewer equals Standard_False, the presentation of the Interactive Object activates the selection mode;
  //! the object is displayed but no viewer will be updated.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void SetTemporaryAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                               const Handle(Prs3d_Drawer)&          theDrawer,
                                               const Standard_Boolean               theToUpdateViewer);

  //! Provides an alternative to the Display methods when activating specific selection modes.
  //! This has the effect of activating the corresponding selection mode aStandardActivation for all objects
  //! in Local Context which accept decomposition into sub-shapes.
  //! Every new Object which has been loaded into the interactive context and which answers these decomposition criteria
  //! is automatically activated according to these modes.
  //! Warning
  //! If you have opened a local context by loading an object with the default options (<AllowShapeDecomposition >= Standard_True),
  //! all objects of the "Shape" type are also activated with the same modes.
  //! You can act on the state of these "Standard" objects by using SetShapeDecomposition(Status).
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void ActivateStandardMode (const TopAbs_ShapeEnum aStandardActivation);

  //! Provides an alternative to the Display methods when deactivating specific selection modes.
  //! This has the effect of deactivating the corresponding selection mode aStandardActivation for all objects
  //! in Local Context which accept decomposition into sub-shapes.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void DeactivateStandardMode (const TopAbs_ShapeEnum aStandardActivation);

  //! Returns the list of activated standard selection modes available in a local context.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT const TColStd_ListOfInteger& ActivatedStandardModes() const;

  //! returns if possible, the first local context where the object is seen
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Boolean IsInLocal (const Handle(AIS_InteractiveObject)& anObject, Standard_Integer& TheIndex) const;

  //! Sets the highlighting status aStatus of detected and selected entities.
  //! Whether you are in Neutral Point or local context, this is automatically managed by the Interactive Context.
  //! This function allows you to disconnect the automatic mode.
  Standard_EXPORT void SetAutomaticHilight (const Standard_Boolean aStatus);

  //! Returns true if the automatic highlight mode is active in an open context.
  Standard_EXPORT Standard_Boolean AutomaticHilight() const;

  //! Changes the status of a temporary object.
  //! It will be kept at the neutral point, i.e. put in the list of displayed objects along with its temporary attributes.
  //! These include display mode and selection mode, for example.
  //! Returns true if done.
  //! inWhichLocal gives the local context in which anIObj is displayed.
  //! By default, the index -1 refers to the last Local Context opened.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Boolean KeepTemporary (const Handle(AIS_InteractiveObject)& anIObj, const Standard_Integer InWhichLocal = -1);

public:

  //! Updates the view of the current object in open context.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void SetCurrentObject (const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateViewer);

  //! Allows to add or remove the object given to the list of current and highlight/unhighlight it correspondingly.
  //! Is valid for global context only; for local context use method AddOrRemoveSelected.
  //! Since this method makes sence only for neutral point selection of a whole object,
  //! if 0 selection of the object is empty this method simply does nothing.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void AddOrRemoveCurrentObject (const Handle(AIS_InteractiveObject)& theObj,
                                                 const Standard_Boolean               theIsToUpdateViewer);

  //! Updates the list of current objects, i.e. hilights new current objects, removes hilighting from former current objects.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void UpdateCurrent();

  //! Returns true if there is a non-null interactive object in Neutral Point.
  //! Objects selected when there is no open local context are called current objects;
  //! those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Boolean IsCurrent (const Handle(AIS_InteractiveObject)& theObject) const;

  //! Initializes a scan of the current selected objects in Neutral Point.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void InitCurrent();

  //! Returns true if there is another object found by the scan of the list of current objects.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Boolean MoreCurrent() const;
  
  //! Continues the scan to the next object in the list of current objects.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void NextCurrent();

  //! Returns the current interactive object.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Handle(AIS_InteractiveObject) Current() const;

  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Standard_Integer NbCurrents();

  //! Highlights current objects.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void HilightCurrents (const Standard_Boolean theToUpdateViewer);

  //! Removes highlighting from current objects.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void UnhilightCurrents (const Standard_Boolean theToUpdateViewer);

  //! Empties previous current objects in order to get the current objects detected by the selector using UpdateCurrent.
  //! Objects selected when there is no open local context are called current objects; those selected in open local context, selected objects.
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT void ClearCurrents (const Standard_Boolean theToUpdateViewer);

  //! @return current mouse-detected shape or empty (null) shape, if current interactive object
  //! is not a shape (AIS_Shape) or there is no current mouse-detected interactive object at all.
  //! @sa DetectedCurrentOwner()/InitDetected()/MoreDetected()/NextDetected().
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT const TopoDS_Shape& DetectedCurrentShape() const;
  
  //! @return current mouse-detected interactive object or null object, if there is no currently detected interactives
  //! @sa DetectedCurrentOwner()/InitDetected()/MoreDetected()/NextDetected().
  Standard_DEPRECATED ("Local Context is deprecated - local selection should be used without Local Context")
  Standard_EXPORT Handle(AIS_InteractiveObject) DetectedCurrentObject() const;

public: //! @name sub-intensity management (deprecated)

  //! Sub-intensity allows temporary highlighting of particular objects with specified color in a manner of selection highlight,
  //! but without actual selection (e.g., global status and owner's selection state will not be updated).
  //! The method returns the color of such highlighting.
  //! By default, it is Quantity_NOC_GRAY40.
  const Quantity_Color& SubIntensityColor() const
  {
    return myStyles[Prs3d_TypeOfHighlight_SubIntensity]->Color();
  }

  //! Sub-intensity allows temporary highlighting of particular objects with specified color in a manner of selection highlight,
  //! but without actual selection (e.g., global status and owner's selection state will not be updated).
  //! The method sets up the color for such highlighting.
  //! By default, this is Quantity_NOC_GRAY40.
  void SetSubIntensityColor (const Quantity_Color& theColor)
  {
    myStyles[Prs3d_TypeOfHighlight_SubIntensity]->SetColor (theColor);
  }

  //! Highlights, and removes highlights from, the displayed object which is displayed at Neutral Point with subintensity color.
  //! Available only for active local context.
  //! There is no effect if there is no local context.
  //! If a local context is open, the presentation of the Interactive Object activates the selection mode.
  Standard_EXPORT void SubIntensityOn (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Standard_Boolean               theToUpdateViewer);

  //! Removes the subintensity option for the entity.
  //! If a local context is open, the presentation of the Interactive Object activates the selection mode.
  Standard_EXPORT void SubIntensityOff (const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Boolean               theToUpdateViewer);

  //! hilights/unhilights displayed objects which are displayed at neutral state with subintensity color.
  //! Available only for active local context.
  //! No effect if no local context.
  Standard_EXPORT void SubIntensityOn (const Standard_Boolean theToUpdateViewer);

  //! Removes subintensity option for all objects.
  Standard_EXPORT void SubIntensityOff (const Standard_Boolean theToUpdateViewer);

protected: //! @name internal methods

  Standard_EXPORT void GetDefModes (const Handle(AIS_InteractiveObject)& anIobj, Standard_Integer& Dmode, Standard_Integer& HiMod, Standard_Integer& SelMode) const;
  
  Standard_EXPORT void EraseGlobal (const Handle(AIS_InteractiveObject)& theIObj,
                                    const Standard_Boolean               theToUpdateViewer);
  
  Standard_EXPORT void ClearGlobal (const Handle(AIS_InteractiveObject)& theIObj,
                                    const Standard_Boolean               theToUpdateViewer);
  
  Standard_EXPORT void ClearGlobalPrs (const Handle(AIS_InteractiveObject)& theObj,
                                       const Standard_Integer               theMode,
                                       const Standard_Boolean               theToUpdateViewer);
  
  Standard_EXPORT void InitAttributes();
  
  Standard_EXPORT Standard_Integer PurgeViewer (const Handle(V3d_Viewer)& Vwr);
  
  //! UNKNOWN
  Standard_EXPORT void redisplayPrsModes (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Boolean               theToUpdateViewer);
  
  //! UNKNOWN
  Standard_EXPORT void redisplayPrsRecModes (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Boolean               theToUpdateViewer);

  //! Helper function to unhighlight all entity owners currently highlighted with seleciton color.
  Standard_EXPORT void unhighlightOwners (const Handle(AIS_InteractiveObject)& theObject);

  //! Helper function that highlights the owner given with <theStyle> without
  //! performing AutoHighlight checks, e.g. is used for dynamic highlight.
  Standard_EXPORT void highlightWithColor (const Handle(SelectMgr_EntityOwner)& theOwner,
                                           const Handle(V3d_Viewer)& theViewer = NULL);

  //! Helper function that highlights the owner given with <theStyle> with check
  //! for AutoHighlight, e.g. is used for selection.
  Standard_EXPORT void highlightSelected (const Handle(SelectMgr_EntityOwner)& theOwner);

  //! Helper function that highlights global owner of the object given with <theStyle> with check
  //! for AutoHighlight, e.g. is used for selection.
  //! If global owner is null, it simply highlights the whole object
  Standard_EXPORT void highlightGlobal (const Handle(AIS_InteractiveObject)& theObj,
                                        const Handle(Prs3d_Drawer)& theStyle,
                                        const Standard_Integer theDispMode) const;

  //! Helper function that unhighlights all owners that are stored in current AIS_Selection.
  //! The function updates global status and selection state of owner and interactive object.
  //! If the parameter <theIsToHilightSubIntensity> is set to true, interactive objects with sub-intensity
  //! switched on in AIS_GlobalStatus will be highlighted with context's sub-intensity color.
  Standard_EXPORT void unhighlightSelected (const Standard_Boolean theIsToHilightSubIntensity = Standard_False);

  //! Helper function that unhighlights global selection owner of given interactive.
  //! The function does not perform any updates of global or owner status
  Standard_EXPORT void unhighlightGlobal (const Handle(AIS_InteractiveObject)& theObj) const;

  //! Helper function that turns on sub-intensity in global status and highlights
  //! given objects with sub-intensity color
  //! @param theObject [in] the object. If NULL is given, than sub-intensity will be turned on for
  //! all inveractive objects of the context
  //! @param theDispMode [in] display mode. If -1 is given, sub-intensity will be turned on for
  //! all display modes in global status's list of modes
  //! @param theIsDisplayedOnly [in] is true if sub-intensity should be applied only to objects with
  //! status AIS_DS_Displayed
  Standard_EXPORT void turnOnSubintensity (const Handle(AIS_InteractiveObject)& theObject = NULL,
                                           const Standard_Integer theDispMode = -1,
                                           const Standard_Boolean theIsDisplayedOnly = Standard_True) const;

  //! Helper function that highlights the object with sub-intensity color without any checks
  //! @param theObject [in] the object that will be highlighted
  //! @param theMode [in] display mode
  Standard_EXPORT void highlightWithSubintensity (const Handle(AIS_InteractiveObject)& theObject,
                                                  const Standard_Integer theMode) const;

  //! Helper function that highlights the owner with sub-intensity color without any checks
  //! @param theOwner [in] the owner that will be highlighted
  //! @param theMode [in] display mode
  Standard_EXPORT void highlightWithSubintensity (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                  const Standard_Integer theMode) const;

  //! Helper function that returns correct dynamic highlight style for the object:
  //! if custom style is defined via object's highlight drawer, it will be used. Otherwise,
  //! dynamic highlight style of interactive context will be returned.
  //! @param theObj [in] the object to check
  const Handle(Prs3d_Drawer)& getHiStyle (const Handle(AIS_InteractiveObject)& theObj,
                                          const Handle(SelectMgr_EntityOwner)& theOwner) const
  {
    const Handle(Prs3d_Drawer)& aHiDrawer = theObj->DynamicHilightAttributes();
    if (!aHiDrawer.IsNull())
    {
      return aHiDrawer;
    }

    return myStyles[!theOwner.IsNull() && theOwner->ComesFromDecomposition() ? Prs3d_TypeOfHighlight_LocalDynamic : Prs3d_TypeOfHighlight_Dynamic];
  }

  //! Helper function that returns correct selection style for the object:
  //! if custom style is defined via object's highlight drawer, it will be used. Otherwise,
  //! selection style of interactive context will be returned.
  //! @param theObj [in] the object to check
  const Handle(Prs3d_Drawer)& getSelStyle (const Handle(AIS_InteractiveObject)& theObj,
                                           const Handle(SelectMgr_EntityOwner)& theOwner) const
  {
    const Handle(Prs3d_Drawer)& aHiDrawer = theObj->HilightAttributes();
    if (!aHiDrawer.IsNull())
    {
      return aHiDrawer;
    }

    return myStyles[!theOwner.IsNull() && theOwner->ComesFromDecomposition() ? Prs3d_TypeOfHighlight_LocalSelected : Prs3d_TypeOfHighlight_Selected];
  }

  //! Assign the context to the object or throw exception if object was already assigned to another context.
  void setContextToObject (const Handle(AIS_InteractiveObject)& theObj)
  {
    if (theObj->HasInteractiveContext())
    {
      if (theObj->myCTXPtr != this)
      {
        throw Standard_ProgramError("AIS_InteractiveContext - object has been already displayed in another context!");
      }
    }
    else
    {
      theObj->SetContext (this);
    }
  }

  //! Return display mode for highlighting.
  Standard_Integer getHilightMode (const Handle(AIS_InteractiveObject)& theObj,
                                   const Handle(Prs3d_Drawer)& theStyle,
                                   const Standard_Integer theDispMode) const
  {
    if (!theStyle.IsNull()
     &&  theStyle->DisplayMode() != -1
     &&  theObj->AcceptDisplayMode (theStyle->DisplayMode()))
    {
      return theStyle->DisplayMode();
    }
    else if (theDispMode != -1)
    {
      return theDispMode;
    }
    else if (theObj->HasDisplayMode())
    {
      return theObj->DisplayMode();
    }
    return myDefaultDrawer->DisplayMode();
  }

  //! Removes dynamic highlight draw
  void clearDynamicHighlight() const
  {
    if (myLastinMain.IsNull())
      return;

    if (myLastinMain->IsAutoHilight())
    {
      myMainPM->ClearImmediateDraw();
    }
    else
    {
      myLastinMain->Selectable()->ClearDynamicHighlight (myMainPM);
    }
  }

protected: //! @name internal fields

  AIS_DataMapOfIOStatus myObjects;
  Handle(SelectMgr_SelectionManager) mgrSelector;
  Handle(PrsMgr_PresentationManager3d) myMainPM;
  Handle(V3d_Viewer) myMainVwr;
  Handle(StdSelect_ViewerSelector3d) myMainSel;
  Handle(SelectMgr_EntityOwner) myLastPicked;
  Handle(SelectMgr_EntityOwner) myLastinMain;
  Standard_Boolean myWasLastMain;
  Standard_Boolean myToHilightSelected;
  Handle(AIS_Selection) mySelection;
  Handle(SelectMgr_OrFilter) myFilters;
  Handle(Prs3d_Drawer) myDefaultDrawer;
  Handle(Prs3d_Drawer) myStyles[Prs3d_TypeOfHighlight_NB];
  AIS_DataMapOfILC myLocalContexts;
  Standard_Integer myCurLocalIndex;
  TColStd_SequenceOfInteger myDetectedSeq;
  Standard_Integer myCurDetected;
  Standard_Integer myCurHighlighted;
  SelectMgr_PickingStrategy myPickingStrategy; //!< picking strategy to be applied within MoveTo()
  Standard_Boolean myIsAutoActivateSelMode;

};

DEFINE_STANDARD_HANDLE(AIS_InteractiveContext, Standard_Transient)

#endif // _AIS_InteractiveContext_HeaderFile
