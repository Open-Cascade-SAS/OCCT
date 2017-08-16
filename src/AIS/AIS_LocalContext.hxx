// Created on: 1996-09-04
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

#ifndef _AIS_LocalContext_HeaderFile
#define _AIS_LocalContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <AIS_DataMapOfSelStat.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <AIS_SequenceOfInteractive.hxx>
#include <Standard_Transient.hxx>
#include <AIS_ClearMode.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Real.hxx>
#include <AIS_Selection.hxx>
class AIS_InteractiveContext;
class SelectMgr_SelectionManager;
class SelectMgr_OrFilter;
class SelectMgr_Filter;
class TCollection_AsciiString;
class AIS_InteractiveObject;
class V3d_View;
class V3d_Viewer;
class TopoDS_Shape;
class SelectMgr_EntityOwner;
class Standard_Transient;
class AIS_LocalStatus;
class SelectMgr_SelectableObject;


class AIS_LocalContext;
DEFINE_STANDARD_HANDLE(AIS_LocalContext, Standard_Transient)

//! Defines a specific context  for selection.
//! It becomes possible to:
//! +  Load  InteractiveObjects  with   a mode   to be
//! activated +  associate InteractiveObjects   with a
//! set of  temporary   selectable Objects....   +   +
//! activate StandardMode  of selection  for  Entities
//! inheriting  BasicShape  from  AIS (Selection    Of
//! vertices, edges,   wires,faces...  + Add   Filters
//! acting on detected owners of sensitive primitives
//!
//! -     automatically    highlight   shapes   and
//! InteractiveObjects  (highlight of  detected shape +
//! highlight of detected selectable...
class AIS_LocalContext : public Standard_Transient
{

public:

  
  Standard_EXPORT AIS_LocalContext();
  
  //! Constructor By Default, the  displayed objects are
  //! automatically loaded.
  Standard_EXPORT AIS_LocalContext(const Handle(AIS_InteractiveContext)& aCtx, const Standard_Integer anIndex, const Standard_Boolean LoadDisplayed = Standard_True, const Standard_Boolean AcceptStandardModes = Standard_True, const Standard_Boolean AcceptErase = Standard_False, const Standard_Boolean UseBothViewers = Standard_False);
  
  //! authorize or not others contexts to erase
  //! temporary displayed objects here;
    void AcceptErase (const Standard_Boolean aStatus);
  
    Standard_Boolean AcceptErase() const;
  
  Standard_EXPORT void SetContext (const Handle(AIS_InteractiveContext)& aCtx);
  
    const Handle(AIS_Selection) Selection() const;
  
  Standard_EXPORT void Terminate (const Standard_Boolean updateviewer = Standard_True);
  
  //! returns true if done...
  Standard_EXPORT Standard_Boolean Display (const Handle(AIS_InteractiveObject)& anInteractive, const Standard_Integer DisplayMode = 0, const Standard_Boolean AllowShapeDecomposition = Standard_True, const Standard_Integer ActivationMode = 0);
  
  //! loads <anInteractive> with nodisplay...
  //! returns true if done
  Standard_EXPORT Standard_Boolean Load (const Handle(AIS_InteractiveObject)& anInteractive, const Standard_Boolean AllowShapeDecomposition = Standard_True, const Standard_Integer ActivationMode = 0);
  
  //! returns true if done...
  Standard_EXPORT Standard_Boolean Erase (const Handle(AIS_InteractiveObject)& anInteractive);
  
  Standard_EXPORT Standard_Boolean Remove (const Handle(AIS_InteractiveObject)& aSelectable);
  
  Standard_EXPORT Standard_Boolean ClearPrs (const Handle(AIS_InteractiveObject)& anInteractive, const Standard_Integer aMode);
  
  //! allows  or  forbids   the   shape  decomposition  into
  //! Activated Standard   Mode  for   <aStoredObject>
  //! does nothing if the object doesn't inherits
  //! BasicShape from AIS
  Standard_EXPORT void SetShapeDecomposition (const Handle(AIS_InteractiveObject)& aStoredObject, const Standard_Boolean aStatus);
  
  //! according to <atype>  , clears the  different parts of
  //! the selector (filters, modeof activation, objects...)
  Standard_EXPORT void Clear (const AIS_ClearMode atype = AIS_CM_All);
  
  //! optional : activation of a mode which is not 0 for a selectable...
  Standard_EXPORT void ActivateMode (const Handle(AIS_InteractiveObject)& aSelectable, const Standard_Integer aMode);
  
  Standard_EXPORT void DeactivateMode (const Handle(AIS_InteractiveObject)& aSelectable, const Standard_Integer aMode);
  
  Standard_EXPORT void Deactivate (const Handle(AIS_InteractiveObject)& aSelectable);
  
  //! decomposition of shapes into <aType>
  Standard_EXPORT void ActivateStandardMode (const TopAbs_ShapeEnum aType);
  
  Standard_EXPORT void DeactivateStandardMode (const TopAbs_ShapeEnum aType);
  
    const TColStd_ListOfInteger& StandardModes() const;
  
  Standard_EXPORT void AddFilter (const Handle(SelectMgr_Filter)& aFilter);
  
  Standard_EXPORT void RemoveFilter (const Handle(SelectMgr_Filter)& aFilter);
  
    const SelectMgr_ListOfFilter& ListOfFilter() const;
  
    const Handle(SelectMgr_OrFilter)& Filter() const;
  
  //! if <aStatus> = True , the shapes or subshapes detected
  //! by the selector will be automatically hilighted in the
  //! main viewer.
  //! Else the user has to manage the detected shape outside the
  //! Shape Selector....
    void SetAutomaticHilight (const Standard_Boolean aStatus);
  
    Standard_Boolean AutomaticHilight() const;
  
  Standard_EXPORT AIS_StatusOfDetection MoveTo (const Standard_Integer theXpix, const Standard_Integer theYpix, const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate);
  
  //! returns True if more than one entity
  //! was detected at the last Mouse position.
    Standard_Boolean HasNextDetected() const;
  
  //! returns True if  last detected. the next detected will
  //! be first one (endless loop)
  Standard_EXPORT Standard_Integer HilightNextDetected (const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate);
  
  Standard_EXPORT Standard_Integer HilightPreviousDetected (const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate);
  
  //! returns True if something was done...
  Standard_EXPORT Standard_Boolean UnhilightLastDetected (const Handle(V3d_Viewer)& theViewer);

  //! returns True if something was done...
  Standard_EXPORT Standard_Boolean UnhilightLastDetected (const Handle(V3d_View)& theView);
  
  //! returns the number of selected
  Standard_EXPORT AIS_StatusOfPick AddSelect (const Handle(SelectMgr_EntityOwner)& theObject);

  Standard_EXPORT AIS_StatusOfPick Select (const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT AIS_StatusOfPick Select (const Standard_Integer XPMin, const Standard_Integer YPMin, const Standard_Integer XPMax, const Standard_Integer YPMax, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const Standard_Integer XPMin, const Standard_Integer YPMin, const Standard_Integer XPMax, const Standard_Integer YPMax, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT AIS_StatusOfPick Select (const TColgp_Array1OfPnt2d& Polyline, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT AIS_StatusOfPick ShiftSelect (const TColgp_Array1OfPnt2d& Polyline, const Handle(V3d_View)& aView, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void HilightPicked (const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void UnhilightPicked (const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void UpdateSelected (const Standard_Boolean updateviewer = Standard_True);
  
  //! Part of advanced selection highlighting mechanism.
  //! If no owners belonging to anobj are selected, calls anobj->ClearSelected(),
  //! otherwise calls anobj->HilightSelected(). This method can be used to avoid
  //! redrawing the whole selection belonging to several Selectable Objects.
  Standard_EXPORT void UpdateSelected (const Handle(AIS_InteractiveObject)& anobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! useful  to  update selection with objects  coming from
  //! Collector or stack
  Standard_EXPORT void SetSelected (const Handle(AIS_InteractiveObject)& anobj, const Standard_Boolean updateviewer = Standard_True);
  
  //! useful  to  update selection with objects  coming from
  //! Collector or stack
  Standard_EXPORT void AddOrRemoveSelected (const Handle(AIS_InteractiveObject)& anobj, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void AddOrRemoveSelected (const TopoDS_Shape& aShape, const Standard_Boolean updateviewer = Standard_True);
  
  Standard_EXPORT void AddOrRemoveSelected (const Handle(SelectMgr_EntityOwner)& theOwner, const Standard_Boolean toUpdateViewer = Standard_True);
  
  //! Clears local context selection.
  //! @param toUpdateViewer [in] if TRUE the viewer will be updated.
  Standard_EXPORT void ClearSelected (const Standard_Boolean toUpdateViewer = Standard_True);
  
  //! Clears outdated selection and detection of owners for the
  //! interactive object. Use this method if selection structures
  //! of the interactive object have changed. The method unhilights
  //! and removes outdated entity owners from lists of selected
  //! and detected owners.
  //! @param theIO [in] the interactive object.
  //! @param toClearDeactivated [in] pass TRUE to treat deactivated
  //! entity owners as 'outdated' when clearing the selection.
  Standard_EXPORT void ClearOutdatedSelection (const Handle(AIS_InteractiveObject)& theIO, const Standard_Boolean toClearDeactivated);
  
    Standard_Boolean HasDetected() const;
  

  //! Initialization for iteration through mouse-detected objects in local context.
  Standard_EXPORT void InitDetected();
  

  //! @return true if there is more mouse-detected objects after the current one
  //! during iteration through mouse-detected interactive objects.
  Standard_EXPORT Standard_Boolean MoreDetected() const;
  

  //! Gets next current object during iteration through mouse-detected
  //! interactive objects.
  Standard_EXPORT void NextDetected();

  //! @return current mouse-detected Owner or null object if there is no current detected.
  Standard_EXPORT Handle(SelectMgr_EntityOwner) DetectedCurrentOwner() const;

  //! @return current mouse-detected shape or empty (null) shape, if current interactive object
  //! is not a shape (AIS_Shape) or there is no current mouse-detected interactive object at all.
  Standard_EXPORT const TopoDS_Shape& DetectedCurrentShape() const;

  //! @return current mouse-detected interactive object or null object if there is no current detected.
  Standard_EXPORT Handle(AIS_InteractiveObject) DetectedCurrentObject() const;
  
  Standard_EXPORT Standard_Boolean HasDetectedShape() const;
  
  Standard_EXPORT const TopoDS_Shape& DetectedShape() const;
  
  Standard_EXPORT Handle(AIS_InteractiveObject) DetectedInteractive() const;
  
  Standard_EXPORT Handle(SelectMgr_EntityOwner) DetectedOwner() const;
  
  Standard_EXPORT void InitSelected();
  
  Standard_EXPORT Standard_Boolean MoreSelected() const;
  
  Standard_EXPORT void NextSelected();
  
  //! returns TRUE if the detected entity is a shape
  //! coming from a Decomposition of an element.
  Standard_EXPORT Standard_Boolean HasShape() const;
  
  //! returns true if current selection is not empty
  //! and the owner of selected object contains a shape.
  //! This method does not take into account decomposition
  //! status of detected shape.
  Standard_EXPORT Standard_Boolean HasSelectedShape() const;
  
  Standard_EXPORT TopoDS_Shape SelectedShape() const;
  
  Standard_EXPORT Handle(SelectMgr_EntityOwner) SelectedOwner() const;
  
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(AIS_InteractiveObject)& aniobj) const;
  
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(SelectMgr_EntityOwner)& anOwner) const;
  
  Standard_EXPORT Handle(AIS_InteractiveObject) SelectedInteractive() const;
  
  //! returns TRUE if an interactive element
  //! was associated with the current picked entity.
  Standard_EXPORT Standard_Boolean HasApplicative() const;
  
  Standard_EXPORT const Handle(Standard_Transient)& SelectedApplicative() const;
  
  Standard_EXPORT void SetDisplayPriority (const Handle(AIS_InteractiveObject)& anObject, const Standard_Integer Prior);
  
  Standard_EXPORT Standard_Integer DisplayedObjects (TColStd_MapOfTransient& theMapToFill) const;
  
  Standard_EXPORT Standard_Boolean IsIn (const Handle(AIS_InteractiveObject)& anObject) const;
  
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(AIS_InteractiveObject)& anObject) const;
  
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(AIS_InteractiveObject)& anObject, const Standard_Integer aMode) const;
  
  Standard_EXPORT const TColStd_ListOfInteger& SelectionModes (const Handle(AIS_InteractiveObject)& anObject) const;
  
  Standard_EXPORT void SubIntensityOn (const Handle(AIS_InteractiveObject)& anObject);
  
  Standard_EXPORT void SubIntensityOff (const Handle(AIS_InteractiveObject)& anObject);
  
  Standard_EXPORT void Hilight (const Handle(AIS_InteractiveObject)& anObject);
  
  Standard_EXPORT void Hilight (const Handle(AIS_InteractiveObject)& theObj,
                                const Handle(Prs3d_Drawer)& theStyle);
  
  Standard_EXPORT void Unhilight (const Handle(AIS_InteractiveObject)& anObject);
  
  Standard_EXPORT Standard_Boolean IsHilighted (const Handle(AIS_InteractiveObject)& anObject) const;
  
  Standard_EXPORT Standard_Boolean HighlightStyle (const Handle(AIS_InteractiveObject)& theObject,
                                                   Handle(Prs3d_Drawer)& theStyle) const;
  
  //! Define the current selection sensitivity for
  //! this local context according to the view size.
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer aPrecision = 2);
  
  //! Returns the pixel tolerance.
  Standard_EXPORT Standard_Integer PixelTolerance() const;

  //! Allows to manage sensitivity of a particular selection of interactive object theObject
  //! and changes previous sensitivity value of all sensitive entities in selection with theMode
  //! to the given theNewSensitivity.
  Standard_EXPORT void SetSelectionSensitivity (const Handle(AIS_InteractiveObject)& theObject,
                                                const Standard_Integer theMode,
                                                const Standard_Integer theNewSensitivity);

  //! Resets the transient list of presentations previously displayed in immediate mode
  //! and begins accumulation of new list by following AddToImmediateList()/Color()/Highlight() calls.
  Standard_EXPORT Standard_Boolean BeginImmediateDraw();
  
  //! Resets the transient list of presentations previously displayed in immediate mode.
  Standard_EXPORT virtual void ClearImmediateDraw();
  
  //! Stores presentation theMode of object theObj in the transient list of presentations to be displayed in immediate mode.
  //! Will be taken in account in EndImmediateDraw method.
  Standard_EXPORT Standard_Boolean ImmediateAdd (const Handle(AIS_InteractiveObject)& theObj, const Standard_Integer theMode = 0);
  
  //! Allows rapid drawing of the view theView by avoiding an update of the whole background.
  Standard_EXPORT Standard_Boolean EndImmediateDraw (const Handle(V3d_Viewer)& theViewer);
  
  //! Returns true if Presentation Manager is accumulating transient list of presentations to be displayed in immediate mode.
  Standard_EXPORT Standard_Boolean IsImmediateModeOn() const;
  
  Standard_EXPORT TCollection_AsciiString Status() const;
  
  Standard_EXPORT const Handle(AIS_LocalStatus)& Status (const Handle(AIS_InteractiveObject)& anObject) const;
  
  Standard_EXPORT void LoadContextObjects();
  
  Standard_EXPORT void UnloadContextObjects();
  
  Standard_EXPORT void DisplaySensitive (const Handle(V3d_View)& aView);
  
  Standard_EXPORT void ClearSensitive (const Handle(V3d_View)& aView);
  
    const Handle(StdSelect_ViewerSelector3d)& MainSelector() const;
  
  Standard_EXPORT Handle(SelectMgr_EntityOwner) FindSelectedOwnerFromIO (const Handle(AIS_InteractiveObject)& anIObj) const;
  
  Standard_EXPORT Handle(SelectMgr_EntityOwner) FindSelectedOwnerFromShape (const TopoDS_Shape& aShape) const;

  //! Iterates through all interactive objects of local context and activates selection modes
  //! stored in local status
  Standard_EXPORT void RestoreActivatedModes() const;

  DEFINE_STANDARD_RTTIEXT(AIS_LocalContext,Standard_Transient)

private:

  
  Standard_EXPORT void Process (const Handle(SelectMgr_SelectableObject)& anObject);
  
  Standard_EXPORT void Process();
  
  Standard_EXPORT void ActivateStandardModes (const Handle(SelectMgr_SelectableObject)& anObject);
  
  Standard_EXPORT void manageDetected (const Handle(SelectMgr_EntityOwner)& thePickOwner, const Handle(V3d_View)& theView, const Standard_Boolean theToRedrawImmediate);
  
  //! returns 0  if the detected entity was Not FilterOK...
    Standard_Integer DetectedIndex();
  
  Standard_EXPORT void Hilight (const Handle(SelectMgr_EntityOwner)& Own, const Handle(V3d_View)& aview);
  
  Standard_EXPORT void Unhilight (const Handle(SelectMgr_EntityOwner)& Ownr, const Handle(V3d_View)& aview);
  
  Standard_EXPORT void ClearObjects();
  
  Standard_EXPORT void ClearDetected();
  
  Standard_EXPORT Standard_Boolean IsDecompositionOn() const;
  
  Standard_EXPORT Standard_Boolean IsShape (const Standard_Integer anIndex) const;
  
  Standard_EXPORT Standard_Boolean IsValidForSelection (const Handle(AIS_InteractiveObject)& anIObj) const;
  
    Standard_Boolean IsValidIndex (const Standard_Integer anIndex) const;
  
  Standard_EXPORT Standard_Boolean ComesFromDecomposition (const Standard_Integer aPickedIndex) const;
  
  Standard_EXPORT Standard_Boolean HasFilters (const TopAbs_ShapeEnum aType) const;

  Handle(AIS_InteractiveContext) myCTX;
  Standard_Boolean myLoadDisplayed;
  Standard_Boolean myAcceptStdMode;
  Standard_Boolean myAcceptErase;
  Handle(SelectMgr_SelectionManager) mySM;
  Handle(StdSelect_ViewerSelector3d) myMainVS;
  Handle(PrsMgr_PresentationManager3d) myMainPM;
  Handle(StdSelect_ViewerSelector3d) myCollVS;
  AIS_DataMapOfSelStat myActiveObjects;
  Handle(SelectMgr_OrFilter) myFilters;
  TColStd_ListOfInteger myListOfStandardMode;
  Handle(SelectMgr_Filter) myStdFilters[9];
  Standard_Boolean myAutoHilight;
  Handle(SelectMgr_IndexedMapOfOwner) myMapOfOwner;
  Handle(AIS_Selection) mySelection;
  Standard_Integer mylastindex;
  Standard_Integer mylastgood;
  Standard_Integer myCurrentOwner;
  TColStd_SequenceOfInteger myDetectedSeq;
  Standard_Integer myCurDetected;
  Standard_Integer myAISCurDetected;
  Handle(Prs3d_Drawer) mySubintStyle;

};


#include <AIS_LocalContext.lxx>





#endif // _AIS_LocalContext_HeaderFile
