// Created on: 1996-12-11
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

#ifndef _AIS_InteractiveObject_HeaderFile
#define _AIS_InteractiveObject_HeaderFile

#include <AIS_KindOfInteractive.hxx>
#include <AIS_DragAction.hxx>
#include <SelectMgr_SelectableObject.hxx>

class AIS_InteractiveContext;
class Prs3d_BasicAspect;
class V3d_View;

//! Defines a class of objects with display and selection services.
//! Entities which are visualized and selected are Interactive Objects.
//! Specific attributes of entities such as arrow aspect for dimensions must be loaded in a
//! Prs3d_Drawer.
//!
//! You can make use of classes of standard Interactive Objects for which all necessary methods have
//! already been programmed, or you can implement your own classes of Interactive Objects. Key
//! interface methods to be implemented by every Interactive Object:
//! * Presentable Object (PrsMgr_PresentableObject)
//!   Consider defining an enumeration of supported Display Mode indexes for particular Interactive
//!   Object or class of Interactive Objects.
//!   - AcceptDisplayMode() accepting display modes implemented by this object;
//!   - Compute() computing presentation for the given display mode index;
//! * Selectable Object (SelectMgr_SelectableObject)
//!   Consider defining an enumeration of supported Selection Mode indexes for particular
//!   Interactive Object or class of Interactive Objects.
//!   - ComputeSelection() computing selectable entities for the given selection mode index.
class AIS_InteractiveObject : public SelectMgr_SelectableObject
{
  friend class AIS_InteractiveContext;
  DEFINE_STANDARD_RTTIEXT(AIS_InteractiveObject, SelectMgr_SelectableObject)
public:
  //! Returns the kind of Interactive Object; AIS_KindOfInteractive_None by default.
  virtual AIS_KindOfInteractive Type() const { return AIS_KindOfInteractive_None; }

  //! Specifies additional characteristics of Interactive Object of Type(); -1 by default.
  //! Among the datums, this signature is attributed to the shape.
  //! The remaining datums have the following default signatures:
  //! - Point          signature 1
  //! - Axis           signature 2
  //! - Trihedron      signature 3
  //! - PlaneTrihedron signature 4
  //! - Line           signature 5
  //! - Circle         signature 6
  //! - Plane          signature 7.
  virtual Standard_Integer Signature() const { return -1; }

  //! Updates the active presentation; if <AllModes> = Standard_True
  //! all the presentations inside are recomputed.
  //! IMPORTANT: It is preferable to call Redisplay method of
  //! corresponding AIS_InteractiveContext instance for cases when it
  //! is accessible. This method just redirects call to myCTXPtr,
  //! so this class field must be up to date for proper result.
  Standard_EXPORT void Redisplay(const Standard_Boolean AllModes = Standard_False);

  //! Indicates whether the Interactive Object has a pointer to an interactive context.
  Standard_Boolean HasInteractiveContext() const { return myCTXPtr != NULL; }

  //! Returns the context pointer to the interactive context.
  AIS_InteractiveContext* InteractiveContext() const { return myCTXPtr; }

  //! Sets the interactive context aCtx and provides a link
  //! to the default drawing tool or "Drawer" if there is none.
  Standard_EXPORT virtual void SetContext(const Handle(AIS_InteractiveContext)& aCtx);

  //! Returns true if the object has an owner attributed to it.
  //! The owner can be a shape for a set of sub-shapes or a sub-shape for sub-shapes which it is
  //! composed of, and takes the form of a transient.
  Standard_Boolean HasOwner() const { return !myOwner.IsNull(); }

  //! Returns the owner of the Interactive Object.
  //! The owner can be a shape for a set of sub-shapes or
  //! a sub-shape for sub-shapes which it is composed of,
  //! and takes the form of a transient.
  //! There are two types of owners:
  //! -   Direct owners, decomposition shapes such as
  //! edges, wires, and faces.
  //! -   Users, presentable objects connecting to sensitive
  //! primitives, or a shape which has been decomposed.
  const Handle(Standard_Transient)& GetOwner() const { return myOwner; }

  //! Allows you to attribute the owner theApplicativeEntity to
  //! an Interactive Object. This can be a shape for a set of
  //! sub-shapes or a sub-shape for sub-shapes which it
  //! is composed of. The owner takes the form of a transient.
  void SetOwner(const Handle(Standard_Transient)& theApplicativeEntity)
  {
    myOwner = theApplicativeEntity;
  }

  //! Each Interactive Object has methods which allow us to attribute an Owner to it in the form of
  //! a Transient. This method removes the owner from the graphic entity.
  void ClearOwner() { myOwner.Nullify(); }

  //! Drag object in the viewer.
  //! @param[in] theCtx       interactive context
  //! @param[in] theView      active View
  //! @param[in] theOwner     the owner of detected entity
  //! @param[in] theDragFrom  drag start point
  //! @param[in] theDragTo    drag end point
  //! @param[in] theAction    drag action
  //! @return FALSE if object rejects dragging action (e.g. AIS_DragAction_Start)
  Standard_EXPORT virtual Standard_Boolean ProcessDragging(
    const Handle(AIS_InteractiveContext)& theCtx,
    const Handle(V3d_View)&               theView,
    const Handle(SelectMgr_EntityOwner)&  theOwner,
    const Graphic3d_Vec2i&                theDragFrom,
    const Graphic3d_Vec2i&                theDragTo,
    const AIS_DragAction                  theAction);

public:
  //! Returns the context pointer to the interactive context.
  Standard_EXPORT Handle(AIS_InteractiveContext) GetContext() const;

  //! Returns TRUE when this object has a presentation in the current DisplayMode()
  Standard_EXPORT Standard_Boolean HasPresentation() const;

  //! Returns the current presentation of this object according to the current DisplayMode()
  Standard_EXPORT Handle(Prs3d_Presentation) Presentation() const;

  //! Sets the graphic basic aspect to the current presentation.
  Standard_DEPRECATED("Deprecated method, results might be undefined")
  Standard_EXPORT void SetAspect(const Handle(Prs3d_BasicAspect)& anAspect);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  //! The TypeOfPresention3d means that the interactive object
  //! may have a presentation dependent on the view of Display.
  Standard_EXPORT AIS_InteractiveObject(
    const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

  //! Set presentation display status.
  Standard_EXPORT void SetDisplayStatus(PrsMgr_DisplayStatus theStatus);

protected:
  // clang-format off
  AIS_InteractiveContext*    myCTXPtr; //!< pointer to Interactive Context, where object is currently displayed; @sa SetContext()
  // clang-format on
  Handle(Standard_Transient) myOwner; //!< application-specific owner object
};

DEFINE_STANDARD_HANDLE(AIS_InteractiveObject, SelectMgr_SelectableObject)

#endif // _AIS_InteractiveObject_HeaderFile
