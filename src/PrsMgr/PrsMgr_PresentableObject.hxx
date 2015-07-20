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

#ifndef _PrsMgr_PresentableObject_HeaderFile
#define _PrsMgr_PresentableObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PrsMgr_Presentations.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Standard_Boolean.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <PrsMgr_PresentableObjectPointer.hxx>
#include <gp_Trsf.hxx>
#include <PrsMgr_ListOfPresentableObjects.hxx>
#include <MMgt_TShared.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Integer.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Graphic3d_ClipPlane.hxx>
class Standard_NotImplemented;
class PrsMgr_Presentation;
class PrsMgr_PresentationManager;
class Graphic3d_Structure;
class Graphic3d_DataStructureManager;
class Geom_Transformation;
class Prs3d_Presentation;
class Prs3d_Projector;
class gp_Pnt;
class gp_Trsf;


class PrsMgr_PresentableObject;
DEFINE_STANDARD_HANDLE(PrsMgr_PresentableObject, MMgt_TShared)

//! A framework to supply the Graphic3d
//! structure of the object to be presented. On the first
//! display request, this structure is created by calling the
//! appropriate algorithm and retaining this frameworkfor
//! further display.
//! This abstract framework is inherited in Application
//! Interactive Services (AIS), notably in:
//! -   AIS_InteractiveObject
//! -   AIS_ConnectedInteractive
//! -   AIS_MultipleConnectedInteractive
//! -   AIS_Shape
//! Consequently, 3D presentation should be handled by
//! the relevant daughter classes and their member
//! functions in AIS. This is particularly true in the
//! creation of new interactive objects.
class PrsMgr_PresentableObject : public MMgt_TShared
{

public:

  
  Standard_EXPORT PrsMgr_Presentations& Presentations();
  
  //! Returns information on whether the object accepts display in HLR mode or not.
  Standard_EXPORT PrsMgr_TypeOfPresentation3d TypeOfPresentation3d() const;
  
  //! Sets up Transform Persistence Mode for this object.
  //! This function used to lock in object position, rotation and / or zooming relative to camera position.
  //! Object will be drawn in the origin setted by APoint parameter (except Graphic3d_TMF_TriedronPers flag
  //! - see description later). aFlag should be:
  //! -   Graphic3d_TMF_None - no persistence attributes (reset);
  //! -   Graphic3d_TMF_PanPers - object doesn't move;
  //! -   Graphic3d_TMF_ZoomPers - object doesn't resize;
  //! -   Graphic3d_TMF_RotatePers - object doesn't rotate;
  //! -   Graphic3d_TMF_FullPers - pan, zoom and rotate transform persistence;
  //! -   Graphic3d_TMF_TriedronPers - object behaves like trihedron;
  //! -   combination (Graphic3d_TMF_PanPers | Graphic3d_TMF_ZoomPers);
  //! -   combination (Graphic3d_TMF_PanPers | Graphic3d_TMF_RotatePers);
  //! -   combination (Graphic3d_TMF_ZoomPers | Graphic3d_TMF_RotatePers).
  //! If Graphic3d_TMF_TriedronPers or Graphic3d_TMF_2d persistence mode selected APoint coordinates X and Y means:
  //! -   X = 0.0, Y = 0.0 - center of view window;
  //! -   X > 0.0, Y > 0.0 - right upper corner of view window;
  //! -   X > 0.0, Y < 0.0 - right lower corner of view window;
  //! -   X < 0.0, Y > 0.0 - left  upper corner of view window;
  //! -   X < 0.0, Y < 0.0 - left  lower corner of view window.
  //! And Z coordinate defines the gap from border of view window (except center position).
  Standard_EXPORT virtual void SetTransformPersistence (const Graphic3d_TransModeFlags& aFlag, const gp_Pnt& APoint);
  
  //! Calls previous method  with  point  (0,0,0)
  Standard_EXPORT void SetTransformPersistence (const Graphic3d_TransModeFlags& aFlag);
  
  //! Gets  Transform  Persistence Mode  for  this  object
  Standard_EXPORT Graphic3d_TransModeFlags GetTransformPersistenceMode() const;
  
  //! Gets  point  of  transform  persistence for  this  object
  Standard_EXPORT gp_Pnt GetTransformPersistencePoint() const;

  //! @return transform persistence of the presentable object.
  const Graphic3d_TransformPers& TransformPersistence() const;

  Standard_EXPORT void SetTypeOfPresentation (const PrsMgr_TypeOfPresentation3d aType);
  
  //! flags the Prs of mode <AMode> to be Updated.
  //! the Update will be done when needed.
  Standard_EXPORT void SetToUpdate (const Standard_Integer aMode);
  
  //! flags all the Presentations to be Updated.
  Standard_EXPORT void SetToUpdate();
  
  //! gives the list of modes which are flagged "to be updated".
  Standard_EXPORT void ToBeUpdated (TColStd_ListOfInteger& ListOfMode) const;
  
  //! Sets local transformation to theTransformation.
  Standard_EXPORT virtual void SetLocalTransformation (const gp_Trsf& theTransformation);
  
  //! Returns true if object has a transformation that is different from the identity.
  Standard_EXPORT Standard_Boolean HasTransformation() const;
  
    const gp_Trsf& LocalTransformation() const;
  
    const gp_Trsf& Transformation() const;
  
    const gp_Trsf& InversedTransformation() const;
  
  //! resets local transformation to identity.
  Standard_EXPORT virtual void ResetTransformation();
  
  Standard_EXPORT virtual void UpdateTransformation();
  
  Standard_EXPORT virtual void UpdateTransformation (const Handle(Prs3d_Presentation)& P);
  
  //! Set Z layer ID and update all presentations of the presentable object.
  //! The layers mechanism allows drawing objects in higher layers in overlay of objects in lower layers.
  Standard_EXPORT virtual void SetZLayer (const Graphic3d_ZLayerId theLayerId);
  
  //! Get ID of Z layer.
  Standard_EXPORT Graphic3d_ZLayerId ZLayer() const;
  
  //! Adds clip plane for graphical clipping for all display mode
  //! presentations. The composition of clip planes truncates the rendering
  //! space to convex volume. Please be aware that number of supported
  //! clip plane is limited. The planes which exceed the limit are ignored.
  //! Besides of this, some planes can be already set in view where the object
  //! is shown: the number of these planes should be substracted from limit
  //! to predict the maximum possible number of object clipping planes.
  //! @param thePlane [in] the clip plane to be appended to map of clip planes.
  Standard_EXPORT virtual void AddClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane);
  
  //! Removes previously added clip plane.
  //! @param thePlane [in] the clip plane to be removed from map of clip planes.
  Standard_EXPORT virtual void RemoveClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane);
  
  //! Set clip planes for graphical clipping for all display mode presentations.
  //! The composition of clip planes truncates the rendering space to convex
  //! volume. Please be aware that number of supported clip plane is limited.
  //! The planes which exceed the limit are ignored. Besides of this, some
  //! planes can be already set in view where the object is shown: the number
  //! of these planes should be substracted from limit to predict the maximum
  //! possible number of object clipping planes.
  Standard_EXPORT virtual void SetClipPlanes (const Graphic3d_SequenceOfHClipPlane& thePlanes);
  
  //! Get clip planes.
  //! @return set of previously added clip planes for all display mode presentations.
    const Graphic3d_SequenceOfHClipPlane& GetClipPlanes() const;
  
  //! Sets if the object has mutable nature (content or location will be changed regularly).
  //! This method should be called before object displaying to take effect.
  Standard_EXPORT virtual void SetMutable (const Standard_Boolean theIsMutable);
  
  //! Returns true if object has mutable nature (content or location are be changed regularly).
  //! Mutable object will be managed in different way than static onces (another optimizations).
  Standard_EXPORT const Standard_Boolean IsMutable() const;
  
  //! Makes theObject child of current object in scene hierarchy.
  Standard_EXPORT virtual void AddChild (const Handle(PrsMgr_PresentableObject)& theObject);
  
  //! Removes theObject from children of current object in scene hierarchy.
  Standard_EXPORT virtual void RemoveChild (const Handle(PrsMgr_PresentableObject)& theObject);
  
  //! Returns children of the current object.
  Standard_EXPORT const PrsMgr_ListOfPresentableObjects& Children() const;
  
  //! Returns true if object should have own presentations.
  Standard_EXPORT const Standard_Boolean HasOwnPresentations() const;
  
  //! Returns parent of current object in scene hierarchy.
  Standard_EXPORT const PrsMgr_PresentableObjectPointer Parent() const;


friend class PrsMgr_Presentation;
friend class PrsMgr_PresentationManager;
friend   
  Standard_EXPORT Handle(Graphic3d_Structure) PrsMgr_Presentation::Compute (const Handle(Graphic3d_DataStructureManager)& theProjector);
friend   
  Standard_EXPORT void PrsMgr_Presentation::Compute (const Handle(Graphic3d_DataStructureManager)& theProjector, const Handle(Graphic3d_Structure)& theGivenStruct);
friend   
  Standard_EXPORT Handle(Graphic3d_Structure) PrsMgr_Presentation::Compute (const Handle(Graphic3d_DataStructureManager)& theProjector, const Handle(Geom_Transformation)& theTrsf);
friend   
  Standard_EXPORT void PrsMgr_Presentation::Compute (const Handle(Graphic3d_DataStructureManager)& theProjector, const Handle(Geom_Transformation)& theTrsf, const Handle(Graphic3d_Structure)& theGivenStruct);


  DEFINE_STANDARD_RTTI(PrsMgr_PresentableObject,MMgt_TShared)

protected:

  
  Standard_EXPORT PrsMgr_PresentableObject(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);
Standard_EXPORT virtual ~PrsMgr_PresentableObject();
  
  //! Calculates the 3D view aPresentation and its
  //! updates. The latter are managed by aPresentationManager.
  //! aPresentableObject has the display mode aMode;
  //! this has the default value of 0, that is, the wireframe display mode.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0);
  
  //! Calculates the 3D view aPresentation and its
  //! updates. The latter are managed by
  //! aPresentationManager. Each of the views in the
  //! viewer and every modification such as rotation, for
  //! example, entails recalculation.
  //! It must be redefined to implement hidden line removal
  //! for the object. The user never calls this method
  //! himself. This is done via the InteractiveContext object
  //! and is dependent on the point of view from which the
  //! object is displayed.
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation);
  
  //! Calculates the 3D view aPresentation and its
  //! updates. The latter are managed by
  //! aPresentationManager. A point of view is provided
  //! by the projector aProjector, and the geometric
  //! transformation which has transformed associated
  //! presentable objects is specified by aTrsf.
  //! This function is to be used in case where a hidden
  //! line removal display cannot be calculated
  //! automatically. This occurs   when associated
  //! presentable objects have been transformed
  //! geometrically, but not translated.
  //! Warning
  //! The transformation aTrsf must be applied to the
  //! object before computation.
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation);
  
  //! recomputes all presentations of the object.
  Standard_EXPORT void Update (const Standard_Boolean AllModes = Standard_False);
  
  //! Recomputes the presentation in the given mode.
  //! If ClearOther is true, other presentation will be cleared.
  Standard_EXPORT void Update (const Standard_Integer aMode, const Standard_Boolean ClearOther);
  
  //! High-level interface for controlling polygon offsets
  Standard_EXPORT virtual void Fill (const Handle(PrsMgr_PresentationManager)& aPresentationManager, const Handle(PrsMgr_Presentation)& aPresentation, const Standard_Integer aMode = 0);
  
  //! Sets myCombinedParentTransform to theTransformation. Thus object receives transformation
  //! from parent node and able to derive its own.
  Standard_EXPORT virtual void SetCombinedParentTransform (const gp_Trsf& theTransformation);
  
  //! General virtual method for internal update of presentation state
  //! when some modifications on list of clip planes occurs. Base
  //! implementation propagate clip planes to every presentation.
  Standard_EXPORT virtual void UpdateClipping();

  PrsMgr_Presentations myPresentations;
  PrsMgr_TypeOfPresentation3d myTypeOfPresentation3d;
  Graphic3d_SequenceOfHClipPlane myClipPlanes;
  Standard_Boolean myIsMutable;
  Graphic3d_ZLayerId myZLayer;
  Standard_Boolean myHasOwnPresentations;


private:


  Graphic3d_TransformPers myTransformPersistence;
  PrsMgr_PresentableObjectPointer myParent;
  gp_Trsf myLocalTransformation;
  gp_Trsf myTransformation;
  gp_Trsf myInvTransformation;
  gp_Trsf myCombinedParentTransform;
  PrsMgr_ListOfPresentableObjects myChildren;


};


#include <PrsMgr_PresentableObject.lxx>





#endif // _PrsMgr_PresentableObject_HeaderFile
