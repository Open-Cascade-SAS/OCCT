// Created on: 1995-05-23
// Created by: Robert COUBLANC
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

#ifndef _SelectMgr_EntityOwner_HeaderFile
#define _SelectMgr_EntityOwner_HeaderFile

#include <AIS_SelectionScheme.hxx>
#include <Aspect_VKey.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <TopLoc_Location.hxx>

class V3d_Viewer;

//! A framework to define classes of owners of sensitive primitives.
//! The owner is the link between application and selection data structures.
//! For the application to make its own objects selectable, it must define owner classes inheriting
//! this framework.
class SelectMgr_EntityOwner : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_EntityOwner, Standard_Transient)
public:
  //! Initializes the selection priority aPriority.
  Standard_EXPORT SelectMgr_EntityOwner(const int aPriority = 0);

  //! Constructs a framework with the selectable object
  //! anSO being attributed the selection priority aPriority.
  Standard_EXPORT SelectMgr_EntityOwner(const occ::handle<SelectMgr_SelectableObject>& aSO,
                                        const int aPriority = 0);

  //! Constructs a framework from existing one
  //! anSO being attributed the selection priority aPriority.
  Standard_EXPORT SelectMgr_EntityOwner(const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                        const int                                 aPriority = 0);

  //! Return selection priority (within range [0-9]) for results with the same depth; 0 by default.
  //! Example - selection of shapes:
  //! the owners are selectable objects (presentations) a user can give vertex priority [3], edges
  //! [2] faces [1] shape [0], so that if during selection one vertex one edge and one face are
  //! simultaneously detected, the vertex will only be hilighted.
  int Priority() const { return mypriority; }

  //! Sets the selectable priority of the owner within range [0-9].
  void SetPriority(int thePriority) { mypriority = thePriority; }

  //! Returns true if there is a selectable object to serve as an owner.
  bool HasSelectable() const { return mySelectable != nullptr; }

  //! Returns a selectable object detected in the working context.
  virtual occ::handle<SelectMgr_SelectableObject> Selectable() const { return mySelectable; }

  //! Sets the selectable object.
  virtual void SetSelectable(const occ::handle<SelectMgr_SelectableObject>& theSelObj)
  {
    mySelectable = theSelObj.get();
  }

  //! Handle mouse button click event.
  //! Does nothing by default and returns FALSE.
  //! @param thePoint      mouse cursor position
  //! @param theButton     clicked button
  //! @param theModifiers  key modifiers
  //! @param theIsDoubleClick flag indicating double mouse click
  //! @return TRUE if object handled click
  //! For all selection schemes, allowing to select an object,
  //! it's available
  virtual bool HandleMouseClick(const NCollection_Vec2<int>& thePoint,
                                Aspect_VKeyMouse             theButton,
                                Aspect_VKeyFlags             theModifiers,
                                bool                         theIsDoubleClick)
  {
    (void)thePoint;
    (void)theButton;
    (void)theModifiers;
    (void)theIsDoubleClick;
    return false;
  }

  //! Returns true if the presentation manager highlights selections corresponding to the selection
  //! mode.
  virtual bool IsHilighted(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                           const int                                      theMode = 0) const
  {
    return mySelectable != nullptr && thePrsMgr->IsHighlighted(mySelectable, theMode);
  }

  //! Highlights selectable object's presentation with display mode in presentation manager with
  //! given highlight style. Also a check for auto-highlight is performed - if selectable object
  //! manages highlighting on its own, execution will be passed to
  //! SelectMgr_SelectableObject::HilightOwnerWithColor method.
  Standard_EXPORT virtual void HilightWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
    const occ::handle<Prs3d_Drawer>&               theStyle,
    const int                                      theMode = 0);

  //! Removes highlighting from the owner of a detected selectable object in the presentation
  //! manager. This object could be the owner of a sensitive primitive.
  //! @param thePrsMgr presentation manager
  //! @param theMode   obsolete argument for compatibility, should be ignored by implementations
  virtual void Unhilight(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                         const int                                      theMode = 0)
  {
    (void)theMode;
    if (mySelectable != nullptr)
    {
      thePrsMgr->Unhighlight(mySelectable);
    }
  }

  //! Clears the owners matching the value of the selection
  //! mode aMode from the presentation manager object aPM.
  virtual void Clear(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                     const int                                      theMode = 0) /// TODO
  {
    (void)thePrsMgr;
    (void)theMode;
  }

  //! Returns TRUE if selectable has transformation.
  virtual bool HasLocation() const
  {
    return mySelectable != nullptr && mySelectable->HasTransformation();
  }

  //! Returns transformation of selectable.
  virtual TopLoc_Location Location() const
  {
    return mySelectable != nullptr && mySelectable->HasTransformation()
             ? TopLoc_Location(mySelectable->Transformation())
             : TopLoc_Location();
  }

  //! Change owner location (callback for handling change of location of selectable object).
  virtual void SetLocation(const TopLoc_Location& theLocation) { (void)theLocation; }

  //! @return true if the owner is selected.
  bool IsSelected() const { return myIsSelected; }

  //! Set the state of the owner.
  //! @param[in] theIsSelected  shows if owner is selected.
  void SetSelected(const bool theIsSelected) { myIsSelected = theIsSelected; }

  //! If the object needs to be selected, it returns true.
  //! @param[in] theSelScheme  selection scheme
  //! @param[in] theIsDetected flag of object detection
  Standard_EXPORT bool Select(const AIS_SelectionScheme theSelScheme,
                              const bool                theIsDetected) const;

  //! Returns selection state.
  Standard_DEPRECATED("Deprecated method - IsSelected() should be used instead")
  int State() const { return myIsSelected ? 1 : 0; }

  //! Set the state of the owner.
  //! The method is deprecated. Use SetSelected() instead.
  void State(const int theStatus) { myIsSelected = (theStatus == 1); }

  //! if owner is not auto hilighted, for group contains many such owners will be called one method
  //! HilightSelected of SelectableObject
  virtual bool IsAutoHilight() const
  {
    return mySelectable == nullptr || mySelectable->IsAutoHilight();
  }

  //! if this method returns TRUE the owner will always call method Hilight for SelectableObject
  //! when the owner is detected. By default it always return FALSE.
  virtual bool IsForcedHilight() const { return false; }

  //! Set Z layer ID and update all presentations.
  virtual void SetZLayer(const Graphic3d_ZLayerId theLayerId) { (void)theLayerId; }

  //! Implements immediate application of location transformation of parent object to dynamic
  //! highlight structure
  virtual void UpdateHighlightTrsf(const occ::handle<V3d_Viewer>&                 theViewer,
                                   const occ::handle<PrsMgr_PresentationManager>& theManager,
                                   const int                                      theDispMode)
  {
    if (mySelectable != nullptr)
    {
      theManager->UpdateHighlightTrsf(theViewer, mySelectable, theDispMode);
    }
  }

  //! Returns true if pointer to selectable object of this owner is equal to the given one
  bool IsSameSelectable(const occ::handle<SelectMgr_SelectableObject>& theOther) const
  {
    return mySelectable == theOther.get();
  }

  //! Returns TRUE if this owner points to a part of object and FALSE for entire object.
  bool ComesFromDecomposition() const { return myFromDecomposition; }

  //! Sets flag indicating this owner points to a part of object (TRUE) or to entire object (FALSE).
  void SetComesFromDecomposition(const bool theIsFromDecomposition)
  {
    myFromDecomposition = theIsFromDecomposition;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

public:
  //! Sets the selectable object.
  Standard_DEPRECATED("Deprecated method - SetSelectable() should be used instead")
  void Set(const occ::handle<SelectMgr_SelectableObject>& theSelObj) { SetSelectable(theSelObj); }

  //! sets the selectable priority of the owner
  Standard_DEPRECATED("Deprecated method - SetPriority() should be used instead")
  void Set(const int thePriority) { SetPriority(thePriority); }

protected:
  SelectMgr_SelectableObject* mySelectable; //!< raw pointer to selectable object
  // clang-format off
  int            mypriority;          //!< selection priority (for result with the same depth)
  bool            myIsSelected;        //!< flag indicating selected state
  bool            myFromDecomposition; //!< flag indicating this owner points to a part of object (TRUE) or to entire object (FALSE)
  // clang-format on
};

#endif // _SelectMgr_EntityOwner_HeaderFile
