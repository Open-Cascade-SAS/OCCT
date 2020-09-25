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

#include <Aspect_TypeOfFacingModel.hxx>
#include <gp_GTrsf.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr_ListOfPresentableObjects.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_Presentations.hxx>
#include <PrsMgr_DisplayStatus.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <TColStd_ListOfInteger.hxx>

class PrsMgr_PresentationManager;
Standard_DEPRECATED("Deprecated alias to PrsMgr_PresentationManager")
typedef PrsMgr_PresentationManager PrsMgr_PresentationManager3d;

//! A framework to supply the Graphic3d structure of the object to be presented.
//! On the first display request, this structure is created by calling the appropriate algorithm and
//! retaining this framework for further display. This abstract framework is inherited in
//! Application Interactive Services (AIS), notably by AIS_InteractiveObject. Consequently, 3D
//! presentation should be handled by the relevant daughter classes and their member functions in
//! AIS. This is particularly true in the creation of new interactive objects.
//!
//! Key interface methods to be implemented by every Selectable Object:
//! - AcceptDisplayMode() accepting display modes implemented by this object;
//! - Compute() computing presentation for the given display mode index.
//!
//! Warning! Methods managing standard attributes (SetColor(), SetWidth(), SetMaterial()) have
//! different meaning for objects of different type (or no meaning at all). Sub-classes might
//! override these methods to modify Prs3d_Drawer or class properties providing a convenient
//! short-cut depending on application needs. For more sophisticated configuring, Prs3d_Drawer
//! should be modified directly, while short-cuts might be left unimplemented.
class PrsMgr_PresentableObject : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(PrsMgr_PresentableObject, Standard_Transient)
  friend class PrsMgr_Presentation;
  friend class PrsMgr_PresentationManager;

public:
  //! Return presentations.
  PrsMgr_Presentations& Presentations() { return myPresentations; }

  //! Get ID of Z layer for main presentation.
  Graphic3d_ZLayerId ZLayer() const { return myDrawer->ZLayer(); }

  //! Set Z layer ID and update all presentations of the presentable object.
  //! The layers mechanism allows drawing objects in higher layers in overlay of objects in lower
  //! layers.
  Standard_EXPORT virtual void SetZLayer(const Graphic3d_ZLayerId theLayerId);

  //! Returns true if object has mutable nature (content or location are be changed regularly).
  //! Mutable object will be managed in different way than static onces (another optimizations).
  Standard_Boolean IsMutable() const { return myIsMutable; }

  //! Sets if the object has mutable nature (content or location will be changed regularly).
  //! This method should be called before object displaying to take effect.
  Standard_EXPORT virtual void SetMutable(const Standard_Boolean theIsMutable);

  //! Return view affinity mask.
  const Handle(Graphic3d_ViewAffinity)& ViewAffinity() const { return myViewAffinity; }

  //! Returns true if the Interactive Object has display mode setting overriding global setting
  //! (within Interactive Context).
  Standard_Boolean HasDisplayMode() const { return myDrawer->DisplayMode() != -1; }

  //! Returns the display mode setting of the Interactive Object.
  //! The range of supported display mode indexes should be specified within object definition and
  //! filtered by AccepDisplayMode().
  //! @sa AcceptDisplayMode()
  Standard_Integer DisplayMode() const { return myDrawer->DisplayMode(); }

  //! Sets the display mode for the interactive object.
  //! An object can have its own temporary display mode, which is different from that proposed by
  //! the interactive context.
  //! @sa AcceptDisplayMode()
  void SetDisplayMode(const Standard_Integer theMode)
  {
    if (AcceptDisplayMode(theMode))
    {
      myDrawer->SetDisplayMode(theMode);
    }
  }

  //! Removes display mode settings from the interactive object.
  void UnsetDisplayMode() { myDrawer->SetDisplayMode(-1); }

  //! Returns true if the Interactive Object is in highlight mode.
  //! @sa HilightAttributes()
  Standard_Boolean HasHilightMode() const
  {
    return !myHilightDrawer.IsNull() && myHilightDrawer->DisplayMode() != -1;
  }

  //! Returns highlight display mode.
  //! This is obsolete method for backward compatibility - use ::HilightAttributes() and
  //! ::DynamicHilightAttributes() instead.
  //! @sa HilightAttributes()
  Standard_Integer HilightMode() const
  {
    return !myHilightDrawer.IsNull() ? myHilightDrawer->DisplayMode() : -1;
  }

  //! Sets highlight display mode.
  //! This is obsolete method for backward compatibility - use ::HilightAttributes() and
  //! ::DynamicHilightAttributes() instead.
  //! @sa HilightAttributes()
  Standard_EXPORT void SetHilightMode(const Standard_Integer theMode);

  //! Unsets highlight display mode.
  //! @sa HilightAttributes()
  void UnsetHilightMode()
  {
    if (!myHilightDrawer.IsNull())
    {
      myHilightDrawer->SetDisplayMode(-1);
    }
    if (!myDynHilightDrawer.IsNull())
    {
      myDynHilightDrawer->SetDisplayMode(-1);
    }
  }

  //! Returns true if the class of objects accepts specified display mode index.
  //! The interactive context can have a default mode of representation for the set of Interactive
  //! Objects. This mode may not be accepted by a given class of objects. Consequently, this virtual
  //! method allowing us to get information about the class in question must be implemented. At
  //! least one display mode index should be accepted by this method. Although subclass can leave
  //! default implementation, it is highly desired defining exact list of supported modes instead,
  //! which is usually an enumeration for one object or objects class sharing similar list of
  //! display modes.
  virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer theMode) const
  {
    (void)theMode;
    return Standard_True;
  }

  //! Returns the default display mode.
  virtual Standard_Integer DefaultDisplayMode() const { return 0; }

  //! Returns TRUE if any active presentation has invalidation flag.
  //! @param theToIncludeHidden when TRUE, also checks hidden presentations
  Standard_EXPORT Standard_Boolean
    ToBeUpdated(Standard_Boolean theToIncludeHidden = Standard_False) const;

  //! Flags presentation to be updated; UpdatePresentations() will recompute these presentations.
  //! @param theMode presentation (display mode) to invalidate, or -1 to invalidate them all
  Standard_EXPORT void SetToUpdate(Standard_Integer theMode);

  //! flags all the Presentations to be Updated.
  void SetToUpdate() { SetToUpdate(-1); }

  //! Returns true if the interactive object is infinite; FALSE by default.
  //! This flag affects various operations operating on bounding box of graphic presentations of
  //! this object. For instance, infinite objects are not taken in account for View FitAll. This
  //! does not necessarily means that object is actually infinite, auxiliary objects might be also
  //! marked with this flag to achieve desired behavior.
  Standard_Boolean IsInfinite() const { return myInfiniteState; }

  //! Sets if object should be considered as infinite.
  Standard_EXPORT void SetInfiniteState(const Standard_Boolean theFlag = Standard_True);

  //! Returns information on whether the object accepts display in HLR mode or not.
  PrsMgr_TypeOfPresentation3d TypeOfPresentation3d() const { return myTypeOfPresentation3d; }

  //! Set type of presentation.
  Standard_EXPORT void SetTypeOfPresentation(const PrsMgr_TypeOfPresentation3d theType);

  //! Return presentation display status; PrsMgr_DisplayStatus_None by default.
  PrsMgr_DisplayStatus DisplayStatus() const { return myDisplayStatus; }

public: //! @name presentation attributes
  //! Returns the attributes settings.
  const Handle(Prs3d_Drawer)& Attributes() const { return myDrawer; }

  //! Initializes the drawing tool theDrawer.
  virtual void SetAttributes(const Handle(Prs3d_Drawer)& theDrawer) { myDrawer = theDrawer; }

  //! Returns the hilight attributes settings.
  //! When not NULL, overrides both Prs3d_TypeOfHighlight_LocalSelected and
  //! Prs3d_TypeOfHighlight_Selected defined within AIS_InteractiveContext::HighlightStyle().
  //! @sa AIS_InteractiveContext::HighlightStyle()
  const Handle(Prs3d_Drawer)& HilightAttributes() const { return myHilightDrawer; }

  //! Initializes the hilight drawing tool theDrawer.
  virtual void SetHilightAttributes(const Handle(Prs3d_Drawer)& theDrawer)
  {
    myHilightDrawer = theDrawer;
  }

  //! Returns the hilight attributes settings.
  //! When not NULL, overrides both Prs3d_TypeOfHighlight_LocalDynamic and
  //! Prs3d_TypeOfHighlight_Dynamic defined within AIS_InteractiveContext::HighlightStyle().
  //! @sa AIS_InteractiveContext::HighlightStyle()
  const Handle(Prs3d_Drawer)& DynamicHilightAttributes() const { return myDynHilightDrawer; }

  //! Initializes the dynamic hilight drawing tool.
  virtual void SetDynamicHilightAttributes(const Handle(Prs3d_Drawer)& theDrawer)
  {
    myDynHilightDrawer = theDrawer;
  }

  //! Clears settings provided by the hilight drawing tool theDrawer.
  virtual void UnsetHilightAttributes() { myHilightDrawer.Nullify(); }

  //! Synchronize presentation aspects after their modification.
  //!
  //! This method should be called after modifying primitive aspect properties (material, texture,
  //! shader) so that modifications will take effect on already computed presentation groups (thus
  //! avoiding re-displaying the object).
  Standard_EXPORT void SynchronizeAspects();

public: //! @name object transformation
  //! Returns Transformation Persistence defining a special Local Coordinate system where this
  //! presentable object is located or NULL handle if not defined. Position of the object having
  //! Transformation Persistence is mutable and depends on camera position. The same applies to a
  //! bounding box of the object.
  //! @sa Graphic3d_TransformPers class description
  const Handle(Graphic3d_TransformPers)& TransformPersistence() const
  {
    return myTransformPersistence;
  }

  //! Sets up Transform Persistence defining a special Local Coordinate system where this object
  //! should be located. Note that management of Transform Persistence object is more expensive than
  //! of the normal one, because it requires its position being recomputed basing on camera position
  //! within each draw call / traverse.
  //! @sa Graphic3d_TransformPers class description
  Standard_EXPORT virtual void SetTransformPersistence(
    const Handle(Graphic3d_TransformPers)& theTrsfPers);

  //! Return the local transformation.
  //! Note that the local transformation of the object having Transformation Persistence
  //! is applied within Local Coordinate system defined by this Persistence.
  const Handle(TopLoc_Datum3D)& LocalTransformationGeom() const { return myLocalTransformation; }

  //! Sets local transformation to theTransformation.
  //! Note that the local transformation of the object having Transformation Persistence
  //! is applied within Local Coordinate system defined by this Persistence.
  void SetLocalTransformation(const gp_Trsf& theTrsf)
  {
    setLocalTransformation(new TopLoc_Datum3D(theTrsf));
  }

  //! Sets local transformation to theTransformation.
  //! Note that the local transformation of the object having Transformation Persistence
  //! is applied within Local Coordinate system defined by this Persistence.
  void SetLocalTransformation(const Handle(TopLoc_Datum3D)& theTrsf)
  {
    setLocalTransformation(theTrsf);
  }

  //! Returns true if object has a transformation that is different from the identity.
  Standard_Boolean HasTransformation() const
  {
    return !myTransformation.IsNull() && myTransformation->Form() != gp_Identity;
  }

  //! Return the transformation taking into account transformation of parent object(s).
  //! Note that the local transformation of the object having Transformation Persistence
  //! is applied within Local Coordinate system defined by this Persistence.
  const Handle(TopLoc_Datum3D)& TransformationGeom() const { return myTransformation; }

  //! Return the local transformation.
  //! Note that the local transformation of the object having Transformation Persistence
  //! is applied within Local Coordinate system defined by this Persistence.
  const gp_Trsf& LocalTransformation() const
  {
    return !myLocalTransformation.IsNull() ? myLocalTransformation->Trsf() : getIdentityTrsf();
  }

  //! Return the transformation taking into account transformation of parent object(s).
  //! Note that the local transformation of the object having Transformation Persistence
  //! is applied within Local Coordinate system defined by this Persistence.
  const gp_Trsf& Transformation() const
  {
    return !myTransformation.IsNull() ? myTransformation->Trsf() : getIdentityTrsf();
  }

  //! Return inversed transformation.
  const gp_GTrsf& InversedTransformation() const { return myInvTransformation; }

  //! Return combined parent transformation.
  const Handle(TopLoc_Datum3D)& CombinedParentTransformation() const
  {
    return myCombinedParentTransform;
  }

  //! resets local transformation to identity.
  Standard_EXPORT virtual void ResetTransformation();

  //! Updates final transformation (parent + local) of presentable object and its presentations.
  Standard_EXPORT virtual void UpdateTransformation();

  //! Calculates object presentation for specific camera position.
  //! Each of the views in the viewer and every modification such as rotation, for example, entails
  //! recalculation.
  //! @param theProjector [in] view orientation
  virtual void RecomputeTransformation(const Handle(Graphic3d_Camera)& theProjector)
  {
    (void)theProjector;
  }

public: //! @name clipping planes
  //! Get clip planes.
  //! @return set of previously added clip planes for all display mode presentations.
  const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const { return myClipPlanes; }

  //! Set clip planes for graphical clipping for all display mode presentations.
  //! The composition of clip planes truncates the rendering space to convex volume.
  //! Please be aware that number of supported clip plane is limited.
  //! The planes which exceed the limit are ignored.
  //! Besides of this, some planes can be already set in view where the object is shown:
  //! the number of these planes should be subtracted from limit to predict the maximum
  //! possible number of object clipping planes.
  Standard_EXPORT virtual void SetClipPlanes(
    const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! Adds clip plane for graphical clipping for all display mode
  //! presentations. The composition of clip planes truncates the rendering
  //! space to convex volume. Please be aware that number of supported
  //! clip plane is limited. The planes which exceed the limit are ignored.
  //! Besides of this, some planes can be already set in view where the object
  //! is shown: the number of these planes should be subtracted from limit
  //! to predict the maximum possible number of object clipping planes.
  //! @param[in] thePlane  the clip plane to be appended to map of clip planes.
  Standard_EXPORT virtual void AddClipPlane(const Handle(Graphic3d_ClipPlane)& thePlane);

  //! Removes previously added clip plane.
  //! @param[in] thePlane  the clip plane to be removed from map of clip planes.
  Standard_EXPORT virtual void RemoveClipPlane(const Handle(Graphic3d_ClipPlane)& thePlane);

public: //! @name parent/children properties
  //! Returns parent of current object in scene hierarchy.
  PrsMgr_PresentableObject* Parent() const { return myParent; }

  //! Returns children of the current object.
  const PrsMgr_ListOfPresentableObjects& Children() const { return myChildren; }

  //! Makes theObject child of current object in scene hierarchy.
  Standard_EXPORT virtual void AddChild(const Handle(PrsMgr_PresentableObject)& theObject);

  //! Makes theObject child of current object in scene hierarchy with keeping the current global
  //! transformation So the object keeps the same position/orientation in the global CS.
  Standard_EXPORT void AddChildWithCurrentTransformation(
    const Handle(PrsMgr_PresentableObject)& theObject);

  //! Removes theObject from children of current object in scene hierarchy.
  Standard_EXPORT virtual void RemoveChild(const Handle(PrsMgr_PresentableObject)& theObject);

  //! Removes theObject from children of current object in scene hierarchy with keeping the current
  //! global transformation. So the object keeps the same position/orientation in the global CS.
  Standard_EXPORT void RemoveChildWithRestoreTransformation(
    const Handle(PrsMgr_PresentableObject)& theObject);

  //! Returns true if object should have own presentations.
  Standard_Boolean HasOwnPresentations() const { return myHasOwnPresentations; }

  //! Returns bounding box of object correspondingly to its current display mode.
  //! This method requires presentation to be already computed, since it relies on bounding box of
  //! presentation structures, which are supposed to be same/close amongst different display modes
  //! of this object.
  Standard_EXPORT virtual void BoundingBox(Bnd_Box& theBndBox);

protected: //! @name interface methods
  //! Protected empty constructor.
  Standard_EXPORT PrsMgr_PresentableObject(
    const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

  //! Destructor.
  Standard_EXPORT virtual ~PrsMgr_PresentableObject();

  //! Fills the given 3D view presentation for specified display mode using Compute() method.
  //! In addition, configures other properties of presentation (transformation, clipping planes).
  //! @param thePrsMgr presentation manager where presentation has been created
  //! @param thePrs    presentation to fill
  //! @param theMode   display mode to compute; can be any number accepted by AcceptDisplayMode()
  //! method
  Standard_EXPORT virtual void Fill(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                    const Handle(PrsMgr_Presentation)&        thePrs,
                                    const Standard_Integer                    theMode);

  //! Calculates the 3D view presentation for specified display mode.
  //! This is a key interface for implementing Presentable Object interface.
  //! @param thePrsMgr presentation manager where presentation has been created
  //! @param thePrs    presentation to fill
  //! @param theMode   display mode to compute; can be any number accepted by AcceptDisplayMode()
  //! method
  //! @sa AcceptDisplayMode()
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer                    theMode) = 0;

  //! Calculates hidden line removal presentation for specific camera position.
  //! Each of the views in the viewer and every modification such as rotation, for example, entails
  //! recalculation. Default implementation throws Standard_NotImplemented exception Warning! The
  //! transformation must be applied to the object before computation.
  //! @param[in] theProjector  view orientation
  //! @param[in] theTrsf  additional transformation, or NULL if undefined
  //! @param[in] thePrs   presentation to fill
  Standard_EXPORT virtual void computeHLR(const Handle(Graphic3d_Camera)&   theProjector,
                                          const Handle(TopLoc_Datum3D)&     theTrsf,
                                          const Handle(Prs3d_Presentation)& thePrs);

  //! Recomputes invalidated presentations of the object.
  //! @param theToIncludeHidden if TRUE, then even hidden invalidated presentations will be updated
  //! @return TRUE if some presentations were recomputed
  Standard_EXPORT Standard_Boolean
    UpdatePresentations(Standard_Boolean theToIncludeHidden = Standard_False);

  //! General virtual method for internal update of presentation state
  //! when some modifications on list of clip planes occurs. Base
  //! implementation propagate clip planes to every presentation.
  Standard_EXPORT virtual void UpdateClipping();

  //! Sets myCombinedParentTransform to theTransformation. Thus object receives transformation
  //! from parent node and able to derive its own.
  Standard_EXPORT virtual void SetCombinedParentTransform(const Handle(TopLoc_Datum3D)& theTrsf);

  //! Sets local transformation to theTransformation.
  Standard_EXPORT virtual void setLocalTransformation(
    const Handle(TopLoc_Datum3D)& theTransformation);

  //! Return the identity transformation.
  Standard_EXPORT static const gp_Trsf& getIdentityTrsf();

  //! Recompute computed (HLR) presentations (when view is in computed mode).
  Standard_EXPORT void recomputeComputed() const;

  //! Replace aspects of existing (computed) presentation groups,
  //! so that the new aspects can be applied without recomputing presentation.
  //! It is NOT recommended approach, because user has to fill such map and then search for each
  //! occurrence in computed groups. The recommended approach is computing presentation with
  //! necessary customized aspects, and then modify them directly followed by SynchronizeAspects()
  //! call.
  Standard_EXPORT void replaceAspects(const Graphic3d_MapOfAspectsToAspects& theMap);

public: //! @name simplified presentation properties API
  //! Enables or disables on-triangulation build of isolines according to the flag given.
  void SetIsoOnTriangulation(const Standard_Boolean theIsEnabled)
  {
    myDrawer->SetIsoOnTriangulation(theIsEnabled);
  }

  //! Returns the current facing model which is in effect.
  Aspect_TypeOfFacingModel CurrentFacingModel() const { return myCurrentFacingModel; }

  //! change the current facing model apply on polygons for SetColor(), SetTransparency(),
  //! SetMaterial() methods default facing model is Aspect_TOFM_TWO_SIDE. This mean that attributes
  //! is applying both on the front and back face.
  void SetCurrentFacingModel(const Aspect_TypeOfFacingModel theModel = Aspect_TOFM_BOTH_SIDE)
  {
    myCurrentFacingModel = theModel;
  }

  //! Returns true if the Interactive Object has color.
  Standard_Boolean HasColor() const { return hasOwnColor; }

  //! Returns the color setting of the Interactive Object.
  virtual void Color(Quantity_Color& theColor) const { theColor = myDrawer->Color(); }

  //! Only the interactive object knowns which Drawer attribute is affected by the color, if any
  //! (ex: for a wire,it's the wireaspect field of the drawer, but for a vertex, only the point
  //! aspect field is affected by the color). WARNING : Do not forget to set the corresponding
  //! fields here (hasOwnColor and myDrawer->SetColor())
  virtual void SetColor(const Quantity_Color& theColor)
  {
    myDrawer->SetColor(theColor);
    hasOwnColor = Standard_True;
  }

  //! Removes color settings. Only the Interactive Object
  //! knows which Drawer attribute is   affected by the color
  //! setting. For a wire, for example, wire aspect is the
  //! attribute affected. For a vertex, however, only point
  //! aspect is affected by the color setting.
  virtual void UnsetColor() { hasOwnColor = Standard_False; }

  //! Returns true if the Interactive Object has width.
  Standard_Boolean HasWidth() const { return myOwnWidth != 0.0f; }

  //! Returns the width setting of the Interactive Object.
  Standard_Real Width() const { return myOwnWidth; }

  //! Allows you to provide the setting aValue for width.
  //! Only the Interactive Object knows which Drawer attribute is affected by the width setting.
  virtual void SetWidth(const Standard_Real theWidth) { myOwnWidth = (Standard_ShortReal)theWidth; }

  //! Reset width to default value.
  virtual void UnsetWidth() { myOwnWidth = 0.0f; }

  //! Returns true if the Interactive Object has a setting for material.
  Standard_Boolean HasMaterial() const { return hasOwnMaterial; }

  //! Returns the current material setting as enumeration value.
  Standard_EXPORT virtual Graphic3d_NameOfMaterial Material() const;

  //! Sets the material aMat defining this display attribute
  //! for the interactive object.
  //! Material aspect determines shading aspect, color and
  //! transparency of visible entities.
  Standard_EXPORT virtual void SetMaterial(const Graphic3d_MaterialAspect& aName);

  //! Removes the setting for material.
  Standard_EXPORT virtual void UnsetMaterial();

  //! Returns true if there is a transparency setting.
  Standard_Boolean IsTransparent() const { return myDrawer->Transparency() > 0.005f; }

  //! Returns the transparency setting.
  //! This will be between 0.0 and 1.0.
  //! At 0.0 an object will be totally opaque, and at 1.0, fully transparent.
  virtual Standard_Real Transparency() const
  {
    return (myDrawer->Transparency() <= 0.005f ? 0.0 : myDrawer->Transparency());
  }

  //! Attributes a setting aValue for transparency.
  //! The transparency value should be between 0.0 and 1.0.
  //! At 0.0 an object will be totally opaque, and at 1.0, fully transparent.
  //! Warning At a value of 1.0, there may be nothing visible.
  Standard_EXPORT virtual void SetTransparency(const Standard_Real aValue = 0.6);

  //! Removes the transparency setting. The object is opaque by default.
  Standard_EXPORT virtual void UnsetTransparency();

  //! Returns Standard_True if <myDrawer> has non-null shading aspect
  Standard_EXPORT virtual Standard_Boolean HasPolygonOffsets() const;

  //! Retrieves current polygon offsets settings from <myDrawer>.
  Standard_EXPORT virtual void PolygonOffsets(Standard_Integer&   aMode,
                                              Standard_ShortReal& aFactor,
                                              Standard_ShortReal& aUnits) const;

  //! Sets up polygon offsets for this object.
  //! @sa Graphic3d_Aspects::SetPolygonOffsets()
  Standard_EXPORT virtual void SetPolygonOffsets(const Standard_Integer   aMode,
                                                 const Standard_ShortReal aFactor = 1.0,
                                                 const Standard_ShortReal aUnits  = 0.0);

  //! Clears settings provided by the drawing tool aDrawer.
  Standard_EXPORT virtual void UnsetAttributes();

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const;

public: //! @name deprecated methods
  //! gives the list of modes which are flagged "to be updated".
  Standard_DEPRECATED("This method is deprecated - UpdatePresentations() should be called instead")
  Standard_EXPORT void ToBeUpdated(TColStd_ListOfInteger& ListOfMode) const;

  //! Get value of the flag "propagate visual state"
  //! It means that the display/erase/color visual state is propagated automatically to all
  //! children; by default, the flag is true
  Standard_Boolean ToPropagateVisualState() const { return myToPropagateVisualState; }

  //! Change the value of the flag "propagate visual state"
  void SetPropagateVisualState(const Standard_Boolean theFlag)
  {
    myToPropagateVisualState = theFlag;
  }

protected:
  //! Recomputes all presentations of the object.
  Standard_DEPRECATED("This method is deprecated - SetToUpdate() + UpdatePresentations() should be "
                      "called instead")

  void Update(Standard_Boolean theToIncludeHidden = Standard_False)
  {
    SetToUpdate();
    UpdatePresentations(theToIncludeHidden);
  }

  //! Recomputes the presentation in the given mode.
  //! @param theMode presentation (display mode) to recompute
  //! @param theToClearOther when TRUE, other presentations (display modes) will be removed
  Standard_DEPRECATED(
    "This method is deprecated - SetToUpdate() + UpdatePresentations() should be called instead")
  Standard_EXPORT void Update(Standard_Integer theMode, Standard_Boolean theToClearOther);

protected:
  // clang-format off
  PrsMgr_PresentableObject*              myParent;                  //!< pointer to the parent object
  PrsMgr_Presentations                   myPresentations;           //!< list of presentations
  Handle(Graphic3d_ViewAffinity)         myViewAffinity;            //!< view affinity mask
  Handle(Graphic3d_SequenceOfHClipPlane) myClipPlanes;              //!< sequence of object-specific clipping planes
  Handle(Prs3d_Drawer)                   myDrawer;                  //!< main presentation attributes
  Handle(Prs3d_Drawer)                   myHilightDrawer;           //!< (optional) custom presentation attributes for highlighting selected object
  Handle(Prs3d_Drawer)                   myDynHilightDrawer;        //!< (optional) custom presentation attributes for highlighting detected object
  Handle(Graphic3d_TransformPers)        myTransformPersistence;    //!< transformation persistence
  Handle(TopLoc_Datum3D)                 myLocalTransformation;     //!< local transformation relative to parent object
  Handle(TopLoc_Datum3D)                 myTransformation;          //!< absolute transformation of this object (combined parents + local transformations)
  Handle(TopLoc_Datum3D)                 myCombinedParentTransform; //!< transformation of parent object (combined for all parents)
  PrsMgr_ListOfPresentableObjects        myChildren;                //!< list of children
  gp_GTrsf                               myInvTransformation;       //!< inversion of absolute transformation (combined parents + local transformations)
  PrsMgr_TypeOfPresentation3d            myTypeOfPresentation3d;    //!< presentation type
  PrsMgr_DisplayStatus                   myDisplayStatus;           //!< presentation display status

  Aspect_TypeOfFacingModel               myCurrentFacingModel;      //!< current facing model
  Standard_ShortReal                     myOwnWidth;                //!< custom width value
  Standard_Boolean                       hasOwnColor;               //!< own color flag
  Standard_Boolean                       hasOwnMaterial;            //!< own material flag

  Standard_Boolean                       myInfiniteState;           //!< infinite flag
  Standard_Boolean                       myIsMutable;               //!< mutable flag
  Standard_Boolean                       myHasOwnPresentations;     //!< flag indicating if object should have own presentations

  Standard_Boolean                       myToPropagateVisualState;  //!< flag indicating if visual state (display/erase/color) should be propagated to all children
  // clang-format on
};

DEFINE_STANDARD_HANDLE(PrsMgr_PresentableObject, Standard_Transient)

#endif // _PrsMgr_PresentableObject_HeaderFile
