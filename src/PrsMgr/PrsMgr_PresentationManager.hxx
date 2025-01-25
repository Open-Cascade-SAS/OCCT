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
#include <PrsMgr_ListOfPresentations.hxx>

class Graphic3d_Structure;
typedef Graphic3d_Structure Prs3d_Presentation;

class TopLoc_Datum3D;
class Prs3d_Drawer;
class PrsMgr_Presentation;
class PrsMgr_PresentableObject;
class V3d_Viewer;

DEFINE_STANDARD_HANDLE(PrsMgr_PresentationManager, Standard_Transient)

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
    const Handle(Graphic3d_StructureManager)& theStructureManager);

  //! Displays the presentation of the object in the given Presentation manager with the given mode.
  //! The mode should be enumerated by the object which inherits PresentableObject.
  Standard_EXPORT void Display(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                               const Standard_Integer                  theMode = 0);

  //! erases the presentation of the object in the given
  //! Presentation manager with the given mode.
  //! If @theMode is -1, then erases all presentations of the object.
  Standard_EXPORT void Erase(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                             const Standard_Integer                  theMode = 0);

  //! Clears the presentation of the presentable object thePrsObject in this framework with the
  //! display mode theMode.
  Standard_EXPORT virtual void Clear(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                     const Standard_Integer                  theMode = 0);

  //! Sets the visibility of presentable object.
  Standard_EXPORT void SetVisibility(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                     const Standard_Integer                  theMode,
                                     const Standard_Boolean                  theValue);

  //! Removes highlighting from the presentation of the presentable object.
  Standard_EXPORT void Unhighlight(const Handle(PrsMgr_PresentableObject)& thePrsObject);

  //! Sets the display priority theNewPrior of the
  //! presentable object thePrsObject in this framework with the display mode theMode.
  Standard_EXPORT void SetDisplayPriority(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                          const Standard_Integer                  theMode,
                                          const Graphic3d_DisplayPriority theNewPrior) const;

  //! Returns the display priority of the presentable object
  //! thePrsObject in this framework with the display mode theMode.
  Standard_EXPORT Graphic3d_DisplayPriority
    DisplayPriority(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                    const Standard_Integer                  theMode) const;

  //! Set Z layer ID for all presentations of the object.
  Standard_EXPORT void SetZLayer(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                 const Graphic3d_ZLayerId                theLayerId);

  //! Get Z layer ID assigned to all presentations of the object.
  //! Method returns -1 value if object has no presentations and is
  //! impossible to get layer index.
  Standard_EXPORT Graphic3d_ZLayerId
    GetZLayer(const Handle(PrsMgr_PresentableObject)& thePrsObject) const;

  Standard_EXPORT Standard_Boolean IsDisplayed(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                               const Standard_Integer theMode = 0) const;

  //! Returns true if the presentation of the presentable
  //! object thePrsObject in this framework with the display mode theMode is highlighted.
  Standard_EXPORT Standard_Boolean
    IsHighlighted(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                  const Standard_Integer                  theMode = 0) const;

  //! Updates the presentation of the presentable object
  //! thePrsObject in this framework with the display mode theMode.
  Standard_EXPORT void Update(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                              const Standard_Integer                  theMode = 0) const;

  //! Resets the transient list of presentations previously displayed in immediate mode
  //! and begins accumulation of new list by following AddToImmediateList()/Color()/Highlight()
  //! calls.
  Standard_EXPORT void BeginImmediateDraw();

  //! Resets the transient list of presentations previously displayed in immediate mode.
  Standard_EXPORT void ClearImmediateDraw();

  //! Stores thePrs in the transient list of presentations to be displayed in immediate mode.
  //! Will be taken in account in EndImmediateDraw method.
  Standard_EXPORT void AddToImmediateList(const Handle(Prs3d_Presentation)& thePrs);

  //! Allows rapid drawing of the each view in theViewer by avoiding an update of the whole
  //! background.
  Standard_EXPORT void EndImmediateDraw(const Handle(V3d_Viewer)& theViewer);

  //! Clears and redisplays immediate structures of the viewer taking into account its affinity.
  Standard_EXPORT void RedrawImmediate(const Handle(V3d_Viewer)& theViewer);

  //! Returns true if Presentation Manager is accumulating transient list of presentations to be
  //! displayed in immediate mode.
  Standard_Boolean IsImmediateModeOn() const { return myImmediateModeOn > 0; }

  //! Highlights the graphic object thePrsObject in the color theColor.
  //! thePrsObject has the display mode theMode;
  //! this has the default value of 0, that is, the wireframe display mode.
  Standard_EXPORT void Color(
    const Handle(PrsMgr_PresentableObject)& thePrsObject,
    const Handle(Prs3d_Drawer)&             theStyle,
    const Standard_Integer                  theMode                   = 0,
    const Handle(PrsMgr_PresentableObject)& theSelObj                 = NULL,
    const Graphic3d_ZLayerId                theImmediateStructLayerId = Graphic3d_ZLayerId_Topmost);

  Standard_EXPORT void Connect(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                               const Handle(PrsMgr_PresentableObject)& theOtherObject,
                               const Standard_Integer                  theMode      = 0,
                               const Standard_Integer                  theOtherMode = 0);

  //! Sets the transformation theTransformation for the presentable object thePrsObject.
  //! thePrsObject has the display mode theMode; this has the default value of 0, that is, the
  //! wireframe display mode.
  Standard_EXPORT void Transform(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                                 const Handle(TopLoc_Datum3D)&           theTransformation,
                                 const Standard_Integer                  theMode = 0);

  //! Returns the structure manager.
  const Handle(Graphic3d_StructureManager)& StructureManager() const { return myStructureManager; }

  //! Returns true if there is a presentation of the
  //! presentable object thePrsObject in this framework, thePrsObject having the display mode
  //! theMode.
  Standard_EXPORT Standard_Boolean
    HasPresentation(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                    const Standard_Integer                  theMode = 0) const;

  //! Returns the presentation Presentation of the presentable object thePrsObject in this
  //! framework. When theToCreate is true - automatically creates presentation for specified mode
  //! when not exist. Optional argument theSelObj specifies parent decomposed object to inherit its
  //! view affinity.
  Standard_EXPORT Handle(PrsMgr_Presentation) Presentation(
    const Handle(PrsMgr_PresentableObject)& thePrsObject,
    const Standard_Integer                  theMode     = 0,
    const Standard_Boolean                  theToCreate = Standard_False,
    const Handle(PrsMgr_PresentableObject)& theSelObj   = NULL) const;

  //! Allows to apply location transformation to shadow highlight presentation immediately.
  //! @param theObj defines the base object, it local transformation will be applied to
  //! corresponding highlight structure
  //! @param theMode defines display mode of the base object
  //! @param theSelObj defines the object produced after decomposition of the base object for local
  //! selection
  Standard_EXPORT void UpdateHighlightTrsf(
    const Handle(V3d_Viewer)&               theViewer,
    const Handle(PrsMgr_PresentableObject)& theObj,
    const Standard_Integer                  theMode   = 0,
    const Handle(PrsMgr_PresentableObject)& theSelObj = NULL);

protected:
  //! Removes a presentation of the presentable object thePrsObject to this framework. thePrsObject
  //! has the display mode theMode.
  Standard_EXPORT Standard_Boolean
    RemovePresentation(const Handle(PrsMgr_PresentableObject)& thePrsObject,
                       const Standard_Integer                  theMode = 0);

private:
  //! Handles the structures from <myImmediateList> and displays it separating view-dependent
  //! structures and taking into account structure visibility by setting proper affinity.
  void displayImmediate(const Handle(V3d_Viewer)& theViewer);

protected:
  Handle(Graphic3d_StructureManager) myStructureManager;
  Standard_Integer                   myImmediateModeOn;
  PrsMgr_ListOfPresentations         myImmediateList;
  PrsMgr_ListOfPresentations         myViewDependentImmediateList;
};

#endif // _PrsMgr_PresentationManager_HeaderFile
