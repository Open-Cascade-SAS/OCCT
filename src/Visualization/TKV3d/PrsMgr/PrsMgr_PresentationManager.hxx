// Created on: 1995-01-25
// Created by: Jean-Louis Frenkel
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

#ifndef _PrsMgr_PresentationManager_HeaderFile
#define _PrsMgr_PresentationManager_HeaderFile

#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <NCollection_List.hxx>
#include <Prs3d_Presentation.hxx>

class Graphic3d_Structure;
typedef Graphic3d_Structure Prs3d_Presentation;

class TopLoc_Datum3D;
class Prs3d_Drawer;
class PrsMgr_Presentation;
class PrsMgr_PresentableObject;
class V3d_Viewer;

//! A framework to manage 3D displays, graphic entities and their updates.
//! Used in the AIS package (Application Interactive Services), to enable the advanced user to
//! define the default display mode of a new interactive object which extends the list of signatures
//! and types. Definition of new display types is handled by calling the presentation algorithms
//! provided by the StdPrs package.
class PrsMgr_PresentationManager : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(PrsMgr_PresentationManager, Standard_Transient)
public:
  //! Creates a framework to manage displays and graphic entities with the 3D view
  //! theStructureManager.
  Standard_EXPORT PrsMgr_PresentationManager(
    const occ::handle<Graphic3d_StructureManager>& theStructureManager);

  //! Displays the presentation of the object in the given Presentation manager with the given mode.
  //! The mode should be enumerated by the object which inherits PresentableObject.
  Standard_EXPORT void Display(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                               const int                                    theMode = 0);

  //! erases the presentation of the object in the given
  //! Presentation manager with the given mode.
  //! If @theMode is -1, then erases all presentations of the object.
  Standard_EXPORT void Erase(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                             const int                                    theMode = 0);

  //! Clears the presentation of the presentable object thePrsObject in this framework with the
  //! display mode theMode.
  Standard_EXPORT virtual void Clear(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                     const int                                    theMode = 0);

  //! Sets the visibility of presentable object.
  Standard_EXPORT void SetVisibility(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                     const int                                    theMode,
                                     const bool                                   theValue);

  //! Removes highlighting from the presentation of the presentable object.
  Standard_EXPORT void Unhighlight(const occ::handle<PrsMgr_PresentableObject>& thePrsObject);

  //! Sets the display priority theNewPrior of the
  //! presentable object thePrsObject in this framework with the display mode theMode.
  Standard_EXPORT void SetDisplayPriority(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                          const int                                    theMode,
                                          const Graphic3d_DisplayPriority theNewPrior) const;

  //! Returns the display priority of the presentable object
  //! thePrsObject in this framework with the display mode theMode.
  Standard_EXPORT Graphic3d_DisplayPriority
    DisplayPriority(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                    const int                                    theMode) const;

  //! Set Z layer ID for all presentations of the object.
  Standard_EXPORT void SetZLayer(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                 const Graphic3d_ZLayerId                     theLayerId);

  //! Get Z layer ID assigned to all presentations of the object.
  //! Method returns -1 value if object has no presentations and is
  //! impossible to get layer index.
  Standard_EXPORT Graphic3d_ZLayerId
    GetZLayer(const occ::handle<PrsMgr_PresentableObject>& thePrsObject) const;

  Standard_EXPORT bool IsDisplayed(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                   const int                                    theMode = 0) const;

  //! Returns true if the presentation of the presentable
  //! object thePrsObject in this framework with the display mode theMode is highlighted.
  Standard_EXPORT bool IsHighlighted(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                     const int theMode = 0) const;

  //! Updates the presentation of the presentable object
  //! thePrsObject in this framework with the display mode theMode.
  Standard_EXPORT void Update(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                              const int                                    theMode = 0) const;

  //! Resets the transient list of presentations previously displayed in immediate mode
  //! and begins accumulation of new list by following AddToImmediateList()/Color()/Highlight()
  //! calls.
  Standard_EXPORT void BeginImmediateDraw();

  //! Resets the transient list of presentations previously displayed in immediate mode.
  Standard_EXPORT void ClearImmediateDraw();

  //! Stores thePrs in the transient list of presentations to be displayed in immediate mode.
  //! Will be taken in account in EndImmediateDraw method.
  Standard_EXPORT void AddToImmediateList(const occ::handle<Prs3d_Presentation>& thePrs);

  //! Allows rapid drawing of the each view in theViewer by avoiding an update of the whole
  //! background.
  Standard_EXPORT void EndImmediateDraw(const occ::handle<V3d_Viewer>& theViewer);

  //! Clears and redisplays immediate structures of the viewer taking into account its affinity.
  Standard_EXPORT void RedrawImmediate(const occ::handle<V3d_Viewer>& theViewer);

  //! Returns true if Presentation Manager is accumulating transient list of presentations to be
  //! displayed in immediate mode.
  bool IsImmediateModeOn() const { return myImmediateModeOn > 0; }

  //! Highlights the graphic object thePrsObject in the color theColor.
  //! thePrsObject has the display mode theMode;
  //! this has the default value of 0, that is, the wireframe display mode.
  Standard_EXPORT void Color(
    const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
    const occ::handle<Prs3d_Drawer>&             theStyle,
    const int                                    theMode   = 0,
    const occ::handle<PrsMgr_PresentableObject>& theSelObj = nullptr,
    const Graphic3d_ZLayerId theImmediateStructLayerId     = Graphic3d_ZLayerId_Topmost);

  Standard_EXPORT void Connect(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                               const occ::handle<PrsMgr_PresentableObject>& theOtherObject,
                               const int                                    theMode      = 0,
                               const int                                    theOtherMode = 0);

  //! Sets the transformation theTransformation for the presentable object thePrsObject.
  //! thePrsObject has the display mode theMode; this has the default value of 0, that is, the
  //! wireframe display mode.
  Standard_EXPORT void Transform(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                 const occ::handle<TopLoc_Datum3D>&           theTransformation,
                                 const int                                    theMode = 0);

  //! Returns the structure manager.
  const occ::handle<Graphic3d_StructureManager>& StructureManager() const
  {
    return myStructureManager;
  }

  //! Returns true if there is a presentation of the
  //! presentable object thePrsObject in this framework, thePrsObject having the display mode
  //! theMode.
  Standard_EXPORT bool HasPresentation(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                       const int theMode = 0) const;

  //! Returns the presentation Presentation of the presentable object thePrsObject in this
  //! framework. When theToCreate is true - automatically creates presentation for specified mode
  //! when not exist. Optional argument theSelObj specifies parent decomposed object to inherit its
  //! view affinity.
  Standard_EXPORT occ::handle<PrsMgr_Presentation> Presentation(
    const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
    const int                                    theMode     = 0,
    const bool                                   theToCreate = false,
    const occ::handle<PrsMgr_PresentableObject>& theSelObj   = nullptr) const;

  //! Allows to apply location transformation to shadow highlight presentation immediately.
  //! @param theObj defines the base object, it local transformation will be applied to
  //! corresponding highlight structure
  //! @param theMode defines display mode of the base object
  //! @param theSelObj defines the object produced after decomposition of the base object for local
  //! selection
  Standard_EXPORT void UpdateHighlightTrsf(
    const occ::handle<V3d_Viewer>&               theViewer,
    const occ::handle<PrsMgr_PresentableObject>& theObj,
    const int                                    theMode   = 0,
    const occ::handle<PrsMgr_PresentableObject>& theSelObj = nullptr);

protected:
  //! Removes a presentation of the presentable object thePrsObject to this framework. thePrsObject
  //! has the display mode theMode.
  Standard_EXPORT bool RemovePresentation(const occ::handle<PrsMgr_PresentableObject>& thePrsObject,
                                          const int                                    theMode = 0);

private:
  //! Handles the structures from <myImmediateList> and displays it separating view-dependent
  //! structures and taking into account structure visibility by setting proper affinity.
  void displayImmediate(const occ::handle<V3d_Viewer>& theViewer);

protected:
  occ::handle<Graphic3d_StructureManager>           myStructureManager;
  int                                               myImmediateModeOn;
  NCollection_List<occ::handle<Prs3d_Presentation>> myImmediateList;
  NCollection_List<occ::handle<Prs3d_Presentation>> myViewDependentImmediateList;
};

#endif // _PrsMgr_PresentationManager_HeaderFile
