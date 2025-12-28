// Created on: 2020-09-07
// Created by: Maria KRYLOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _AIS_LightSource_HeaderFile
#define _AIS_LightSource_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <SelectMgr_EntityOwner.hxx>

class Select3D_SensitiveSphere;

//! Interactive object for a light source.
//! Each type of light source has it's own presentation:
//! - Ambient light is displayed as a sphere at view corner;
//! - Positional light is represented by a sphere or marker;
//! - Spot light is represented by a cone;
//! - Directional light is represented by a set of arrows at the corner of view.
//! In addition, light source name could be displayed, and clicking on presentation will
//! enable/disable light.
class AIS_LightSource : public AIS_InteractiveObject
{
  friend class AIS_LightSourceOwner;
  DEFINE_STANDARD_RTTIEXT(AIS_LightSource, AIS_InteractiveObject)
public:
  //! Initializes the light source by copying Graphic3d_CLight settings.
  Standard_EXPORT AIS_LightSource(const occ::handle<Graphic3d_CLight>& theLightSource);

  //! Returns the light.
  const occ::handle<Graphic3d_CLight>& Light() const { return myLightSource; }

  //! Set the light.
  void SetLight(const occ::handle<Graphic3d_CLight>& theLight)
  {
    myLightSource = theLight;
    SetToUpdate();
  }

public: //! @name Light properties
  //! Returns TRUE if the light source name should be displayed; TRUE by default.
  bool ToDisplayName() const { return myToDisplayName; }

  //! Show/hide light source name.
  void SetDisplayName(bool theToDisplay)
  {
    if (myToDisplayName != theToDisplay)
    {
      myToDisplayName = theToDisplay;
      SetToUpdate();
    }
  }

  //! Returns TRUE to display light source range as sphere (positional light) or cone (spot light);
  //! TRUE by default. Has no effect for non-zoomable presentation.
  bool ToDisplayRange() const { return myToDisplayRange; }

  //! Show/hide light source range shaded presentation.
  void SetDisplayRange(bool theToDisplay)
  {
    if (myToDisplayRange != theToDisplay)
    {
      myToDisplayRange = theToDisplay;
      SetToUpdate();
    }
  }

  //! Returns the size of presentation; 50 by default.
  double Size() const { return mySize; }

  //! Sets the size of presentation.
  void SetSize(double theSize)
  {
    if (mySize != theSize)
    {
      mySize = theSize;
      SetToUpdate();
    }
  }

  //! Returns Sensitive sphere arc size in pixels; 20 by default.
  int ArcSize() const { return mySensSphereArcSize; }

  //! Sets the size of sensitive sphere arc.
  void SetArcSize(int theSize)
  {
    if (mySensSphereArcSize != theSize)
    {
      mySensSphereArcSize = theSize;
      SetToUpdate();
    }
  }

  //! Returns TRUE if transform-persistence is allowed;
  //! TRUE by default for Ambient and Directional lights
  //! and FALSE by default for Positional and Spot lights.
  bool IsZoomable() const { return myIsZoomable; }

  //! Sets if transform-persistence is allowed.
  void SetZoomable(bool theIsZoomable)
  {
    if (myIsZoomable != theIsZoomable)
    {
      myIsZoomable = theIsZoomable;
      SetToUpdate();
    }
  }

  //! Sets if dragging is allowed.
  void SetDraggable(bool theIsDraggable)
  {
    if (myIsDraggable != theIsDraggable)
    {
      myIsDraggable = theIsDraggable;
    }
  }

  //! Returns TRUE if mouse click will turn light on/off; TRUE by default.
  bool ToSwitchOnClick() const { return myToSwitchOnClick; }

  //! Sets if mouse click should turn light on/off.
  void SetSwitchOnClick(bool theToHandle) { myToSwitchOnClick = theToHandle; }

  //! Returns a number of directional light arrows to display; 5 by default.
  int NbArrows() const { return myNbArrows; }

  //! Returns a number of directional light arrows to display (supported values: 1, 3, 5, 9).
  void SetNbArrows(int theNbArrows)
  {
    if (myNbArrows != theNbArrows)
    {
      myNbArrows = theNbArrows;
      SetToUpdate();
    }
  }

  //! Returns light source icon.
  //! @param[in] theIsEnabled  marker index for enabled/disabled light source states
  const occ::handle<Graphic3d_MarkerImage>& MarkerImage(bool theIsEnabled) const
  {
    return myMarkerImages[theIsEnabled ? 1 : 0];
  }

  //! Returns light source icon.
  //! @param[in] theIsEnabled  marker index for enabled/disabled light source states
  Aspect_TypeOfMarker MarkerType(bool theIsEnabled) const
  {
    return myMarkerTypes[theIsEnabled ? 1 : 0];
  }

  //! Sets custom icon to light source.
  void SetMarkerImage(const occ::handle<Graphic3d_MarkerImage>& theImage, bool theIsEnabled)
  {
    myMarkerImages[theIsEnabled ? 1 : 0] = theImage;
    myMarkerTypes[theIsEnabled ? 1 : 0]  = !theImage.IsNull()
                                             ? Aspect_TOM_USERDEFINED
                                             : (theIsEnabled ? Aspect_TOM_O_POINT : Aspect_TOM_O_X);
  }

  //! Sets standard icon to light source.
  void SetMarkerType(Aspect_TypeOfMarker theType, bool theIsEnabled)
  {
    myMarkerTypes[theIsEnabled ? 1 : 0] = theType;
  }

  //! Returns tessellation level for quadric surfaces; 30 by default.
  int NbSplitsQuadric() const { return myNbSplitsQuadric; }

  //! Sets tessellation level for quadric surfaces.
  void SetNbSplitsQuadric(int theNbSplits) { myNbSplitsQuadric = theNbSplits; }

  //! Returns tessellation level for arrows; 20 by default.
  int NbSplitsArrow() const { return myNbSplitsArrow; }

  //! Sets tessellation level for arrows.
  void SetNbSplitsArrow(int theNbSplits) { myNbSplitsArrow = theNbSplits; }

  //! Returns kind of the object.
  virtual AIS_KindOfInteractive Type() const override { return AIS_KindOfInteractive_LightSource; }

protected:
  //! Return true if specified display mode is supported: 0 for main presentation and 1 for
  //! highlight.
  virtual bool AcceptDisplayMode(const int theMode) const override
  {
    return theMode == 0 || theMode == 1;
  }

  //! Computes selection sensitive zones(triangulation) for light source presentation.
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  //! Fills presentation.
  Standard_EXPORT virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  //! Drag object in the viewer.
  //! @param[in] theCtx      interactive context
  //! @param[in] theView     active View
  //! @param[in] theOwner    the owner of detected entity
  //! @param[in] theDragFrom drag start point
  //! @param[in] theDragTo   drag end point
  //! @param[in] theAction   drag action
  //! @return FALSE if object rejects dragging action (e.g. AIS_DragAction_Start)
  Standard_EXPORT virtual bool ProcessDragging(const occ::handle<AIS_InteractiveContext>& theCtx,
                                               const occ::handle<V3d_View>&               theView,
                                               const occ::handle<SelectMgr_EntityOwner>&  theOwner,
                                               const NCollection_Vec2<int>& theDragFrom,
                                               const NCollection_Vec2<int>& theDragTo,
                                               const AIS_DragAction         theAction) override;

  //! Sets new local transformation, which is propagated to Graphic3d_CLight instance.
  Standard_EXPORT virtual void setLocalTransformation(
    const occ::handle<TopLoc_Datum3D>& theTrsf) override;

  //! Updates local transformation basing on a type of light source.
  Standard_EXPORT virtual void updateLightLocalTransformation();

  //! Updates transform persistence basing on a type of light source.
  Standard_EXPORT virtual void updateLightTransformPersistence();

  //! Sets color of light.
  Standard_EXPORT virtual void updateLightAspects();

  //! Compute ambient light source presentation as a sphere at view corner.
  Standard_EXPORT virtual void computeAmbient(const occ::handle<Prs3d_Presentation>& thePrs,
                                              const int                              theMode);

  //! Compute directional light source presentation as a set of arrows at view corner.
  Standard_EXPORT virtual void computeDirectional(const occ::handle<Prs3d_Presentation>& thePrs,
                                                  const int                              theMode);

  //! Compute positional light source presentation as a sphere of either fixed size (no range) or of
  //! size representing a maximum range.
  Standard_EXPORT virtual void computePositional(const occ::handle<Prs3d_Presentation>& thePrs,
                                                 const int                              theMode);

  //! Compute spot light source presentation as a cone.
  Standard_EXPORT virtual void computeSpot(const occ::handle<Prs3d_Presentation>& thePrs,
                                           const int                              theMode);

protected:
  occ::handle<Graphic3d_CLight> myLightSource; //!< displayed light source

  // clang-format off
  occ::handle<Graphic3d_AspectMarker3d> myDisabledMarkerAspect;  //!< disabled light source marker style
  occ::handle<Graphic3d_AspectLine3d>   myArrowLineAspectShadow; //!< arrow shadow style
  occ::handle<Graphic3d_MarkerImage>    myMarkerImages[2];       //!< icon of disabled (0) and enabled (1) light
  occ::handle<Select3D_SensitiveSphere> mySensSphere;            //!< sensitive sphere of directional light source
  Aspect_TypeOfMarker              myMarkerTypes[2];        //!< icon of disabled (0) and enabled (1) light
  Aspect_TypeOfMarker              myCodirMarkerType;       //!< icon of arrow co-directional to camera direction (look from)
  Aspect_TypeOfMarker              myOpposMarkerType;       //!< icon of arrow opposite to camera direction (look at)

  gp_Trsf          myLocTrsfStart;      //!< object transformation before transformation
  double    mySize;              //!< presentation size
  int myNbArrows;          //!< number of directional light arrows
  int myNbSplitsQuadric;   //!< tessellation level for quadric surfaces
  int myNbSplitsArrow;     //!< tessellation level for arrows
  int mySensSphereArcSize; //! sensitive sphere arc size in pixels
  bool myIsZoomable;        //!< flag to allow/disallow transform-persistence when possible
  bool myIsDraggable;       //!< flag to allow/disallow rotate directional light source by dragging
  // clang-format on
  bool myToDisplayName;   //!< flag to show/hide name
  bool myToDisplayRange;  //!< flag to show/hide range of positional/spot light
  bool myToSwitchOnClick; //!< flag to handle mouse click to turn light on/off
};

//! Owner of AIS_LightSource presentation.
class AIS_LightSourceOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTIEXT(AIS_LightSourceOwner, SelectMgr_EntityOwner)
public:
  //! Main constructor.
  Standard_EXPORT AIS_LightSourceOwner(const occ::handle<AIS_LightSource>& theObject,
                                       int                                 thePriority = 5);

  //! Handle mouse button click event.
  Standard_EXPORT virtual bool HandleMouseClick(const NCollection_Vec2<int>& thePoint,
                                                Aspect_VKeyMouse             theButton,
                                                Aspect_VKeyFlags             theModifiers,
                                                bool theIsDoubleClick) override;

  //! Highlights selectable object's presentation with display mode in presentation manager with
  //! given highlight style. Also a check for auto-highlight is performed - if selectable object
  //! manages highlighting on its own, execution will be passed to
  //! SelectMgr_SelectableObject::HilightOwnerWithColor method.
  Standard_EXPORT virtual void HilightWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
    const occ::handle<Prs3d_Drawer>&               theStyle,
    const int                                      theMode) override;

  //! Always update dynamic highlighting.
  Standard_EXPORT virtual bool IsForcedHilight() const override;
};

#endif // _AIS_LightSource_HeaderFile
