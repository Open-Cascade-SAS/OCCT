// Created on: 2015-12-23
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _AIS_Manipulator_HeaderFile
#define _AIS_Manipulator_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_ManipulatorMode.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Group.hxx>
#include <NCollection_HSequence.hxx>
#include <Poly_Triangulation.hxx>
#include <V3d_View.hxx>
#include <Standard_DefineHandle.hxx>

NCOLLECTION_HSEQUENCE(AIS_ManipulatorObjectSequence, Handle(AIS_InteractiveObject))

DEFINE_STANDARD_HANDLE (AIS_Manipulator, AIS_InteractiveObject)

//! Interactive object class to manipulate local transformation of another interactive
//! object or a group of objects via mouse.
//! It manages three types of manipulations in 3D space:
//! - translation through axis
//! - scaling within axis
//! - rotation around axis
//! To enable one of this modes, selection mode (from 1 to 3) is to be activated.
//! There are three orthogonal transformation axes defined by position property of
//! the manipulator. Particular transformation mode can be disabled for each
//! of the axes or all of them. Furthermore each of the axes can be hidden or
//! made visible.
//! The following steps demonstrate how to attach, configure and use manipulator
//! for an interactive object:
//! Step 1. Create manipulator object and adjust it appearance:
//! @code
//! Handle(AIS_Manipulator) aManipulator = new AIS_Manipulator();
//! aManipulator->SetPart (0, AIS_Manipulator::Scaling, Standard_False);
//! aManipulator->SetPart (1, AIS_Manipulator::Rotation, Standard_False);
//! // Attach manipulator to already displayed object and manage manipulation modes
//! aManipulator->AttachToObject (anAISObject);
//! aManipulator->EnableMode (AIS_Manipulator::Translation);
//! aManipulator->EnableMode (AIS_Manipulator::Rotation);
//! aManipulator->EnableMode (AIS_Manipulator::Scaling);
//! @endcode
//! Note that you can enable only one manipulation mode but have all visual parts displayed.
//! This code allows you to view manipulator and select its manipulation parts.
//! Note that manipulator activates mode on part selection.
//! If this mode is activated, no selection will be performed for manipulator.
//! It can be activated with highlighting. To enable this:
//! @code
//! aManipulator->SetModeActivationOnDetection (Standard_True);
//! @endcode
//! Step 2. To perform transformation of object use next code in your event processing chain:
//! @code
//! // catch mouse button down event
//! if (aManipulator->HasActiveMode())
//! {
//!   aManipulator->StartTransform (anXPix, anYPix, aV3dView);
//! }
//! ...
//! // or track mouse move event
//! if (aManipulator->HasActiveMode())
//! {
//!   aManipulator->Transform (anXPix, anYPix, aV3dView);
//!   aV3dView->Redraw();
//! }
//! ...
//! // or catch mouse button up event (apply) or escape event (cancel)
//! aManipulator->StopTransform(/*Standard_Boolean toApply*/);
//! @endcode
//! Step 3. To deactivate current manipulation mode use:
//! @code aManipulator->DeactivateCurrentMode();
//! @endcode
//! Step 4. To detach manipulator from object use:
//! @code
//! aManipulator->Detach();
//! @endcode
//! The last method erases manipulator object.
class AIS_Manipulator : public AIS_InteractiveObject
{
public:

  //! Constructs a manipulator object with default placement and all parts to be displayed.
  Standard_EXPORT AIS_Manipulator();

  //! Constructs a manipulator object with input location and positions of axes and all parts to be displayed.
  Standard_EXPORT AIS_Manipulator (const gp_Ax2& thePosition);

  //! Disable or enable visual parts for translation, rotation or scaling for some axis.
  //! By default all parts are enabled (will be displayed).
  //! @warning Enabling or disabling of visual parts of manipulator does not manage the manipulation (selection) mode.
  //! @warning Raises program error if axis index is < 0 or > 2.
  Standard_EXPORT void SetPart (const Standard_Integer theAxisIndex, const AIS_ManipulatorMode theMode, const Standard_Boolean theIsEnabled);

  //! Disable or enable visual parts for translation, rotation or scaling for ALL axes.
  //! By default all parts are enabled (will be displayed).
  //! @warning Enabling or disabling of visual parts of manipulator does not manage the manipulation (selection) mode.
  //! @warning Raises program error if axis index is < 0 or > 2.
  Standard_EXPORT void SetPart (const AIS_ManipulatorMode theMode, const Standard_Boolean theIsEnabled);

  //! Behavior settings to be applied when performing transformation:
  //! - FollowTranslation - whether the manipulator will be moved together with an object.
  //! - FollowRotation - whether the manipulator will be rotated together with an object.
  struct OptionsForAttach {

    OptionsForAttach() : AdjustPosition (Standard_True), AdjustSize (Standard_False), EnableModes (Standard_True) {}
    OptionsForAttach& SetAdjustPosition (const Standard_Boolean theApply) { AdjustPosition = theApply; return *this; }
    OptionsForAttach& SetAdjustSize     (const Standard_Boolean theApply) { AdjustSize     = theApply; return *this; }
    OptionsForAttach& SetEnableModes    (const Standard_Boolean theApply) { EnableModes    = theApply; return *this; }

    Standard_Boolean AdjustPosition;
    Standard_Boolean AdjustSize;
    Standard_Boolean EnableModes;
  };

  //! Attaches himself to the input interactive object and become displayed in the same context.
  //! It is placed in the center of object bounding box, and its size is adjusted to the object bounding box.
  Standard_EXPORT void Attach (const Handle(AIS_InteractiveObject)& theObject, const OptionsForAttach& theOptions = OptionsForAttach());

  //! Attaches himself to the input interactive object group and become displayed in the same context.
  //! It become attached to the first object, baut manage manipulation of the whole group.
  //! It is placed in the center of object bounding box, and its size is adjusted to the object bounding box.
  Standard_EXPORT void Attach (const Handle(AIS_ManipulatorObjectSequence)& theObject, const OptionsForAttach& theOptions = OptionsForAttach());

  //! Enable manipualtion mode.
  //! @warning It activates selection mode in the current context.
  //! If manipulator is not displayed, no mode will be activated.
  Standard_EXPORT void EnableMode (const AIS_ManipulatorMode theMode);

  //! Enables mode activation on detection (highlighting).
  //! By default, mode is activated on selection of manipulator part.
  //! @warning If this mode is enabled, selection of parts does nothing.
  void SetModeActivationOnDetection (const Standard_Boolean theToEnable)
  {
    myIsActivationOnDetection = theToEnable;
  }

  //! @return true if manual mode activation is enabled.
  Standard_Boolean IsModeActivationOnDetection() const
  {
    return myIsActivationOnDetection;
  }

public:
  //! Drag object in the viewer.
  //! @param theCtx      [in] interactive context
  //! @param theView     [in] active View
  //! @param theOwner    [in] the owner of detected entity
  //! @param theDragFrom [in] drag start point
  //! @param theDragTo   [in] drag end point
  //! @param theAction   [in] drag action
  //! @return FALSE if object rejects dragging action (e.g. AIS_DragAction_Start)
  Standard_EXPORT virtual Standard_Boolean ProcessDragging (const Handle(AIS_InteractiveContext)& theCtx,
                                                            const Handle(V3d_View)& theView,
                                                            const Handle(SelectMgr_EntityOwner)& theOwner,
                                                            const Graphic3d_Vec2i& theDragFrom,
                                                            const Graphic3d_Vec2i& theDragTo,
                                                            const AIS_DragAction theAction) Standard_OVERRIDE;

  //! Init start (reference) transformation.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  //! and is used only for custom transform set. If Transform(const Standard_Integer, const Standard_Integer) is used,
  //! initial data is set automatically, and it is reset on DeactivateCurrentMode call if it is not reset yet.
  Standard_EXPORT void StartTransform (const Standard_Integer theX, const Standard_Integer theY, const Handle(V3d_View)& theView);

  //! Apply to the owning objects the input transformation.
  //! @remark The transformation is set using SetLocalTransformation for owning objects.
  //! The location of the manipulator is stored also in Local Transformation,
  //! so that there's no need to redisplay objects.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  //! and is used only for custom transform set.
  //! @warning It will does nothing if transformation is not initiated (with StartTransform() call).
  Standard_EXPORT void Transform (const gp_Trsf& aTrsf);

  //! Reset start (reference) transformation.
  //! @param theToApply [in] option to apply or to cancel the started transformation.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  //! and is used only for custom transform set.
  Standard_EXPORT void StopTransform (const Standard_Boolean theToApply = Standard_True);

  //! Apply transformation made from mouse moving from start position
  //! (save on the first Transform() call and reset on DeactivateCurrentMode() call.)
  //! to the in/out mouse position (theX, theY)
  Standard_EXPORT gp_Trsf Transform (const Standard_Integer theX, const Standard_Integer theY,
                                     const Handle(V3d_View)& theView);

  //! Computes transformation of parent object according to the active mode and input motion vector.
  //! You can use this method to get object transformation according to current mode or use own algorithm
  //! to implement any other transformation for modes.
  //! @return transformation of parent object.
  Standard_EXPORT Standard_Boolean ObjectTransformation (const Standard_Integer theX, const Standard_Integer theY,
                                                         const Handle(V3d_View)& theView, gp_Trsf& theTrsf);

  //! Make inactive the current selected manipulator part and reset current axis index and current mode.
  //! After its call HasActiveMode() returns false.
  //! @sa HasActiveMode()
  Standard_EXPORT void DeactivateCurrentMode();

  //! Detaches himself from the owner object, and removes itself from context.
  Standard_EXPORT void Detach();

  //! @return all owning objects.
  Standard_EXPORT Handle(AIS_ManipulatorObjectSequence) Objects() const;

  //! @return the first (leading) object of the owning objects.
  Standard_EXPORT Handle(AIS_InteractiveObject) Object() const;

  //! @return one of the owning objects.
  //! @warning raises program error if theIndex is more than owning objects count or less than 1.
  Standard_EXPORT Handle(AIS_InteractiveObject) Object (const Standard_Integer theIndex) const;

  //! @return true if manipulator is attached to some interactive object (has owning object).
  Standard_Boolean IsAttached() const { return HasOwner(); }

  //! @return true if some part of manipulator is selected (transformation mode is active, and owning object can be transformed).
  Standard_Boolean HasActiveMode() const { return IsAttached() && myCurrentMode != AIS_MM_None; }

  Standard_Boolean HasActiveTransformation() { return myHasStartedTransformation; }

  gp_Trsf StartTransformation() const { return !myStartTrsfs.IsEmpty() ? myStartTrsfs.First() : gp_Trsf(); }

  gp_Trsf StartTransformation (Standard_Integer theIndex) const
  {
    Standard_ProgramError_Raise_if (theIndex < 1 || theIndex > Objects()->Upper(),
      "AIS_Manipulator::StartTransformation(): theIndex is out of bounds");
    return !myStartTrsfs.IsEmpty() ? myStartTrsfs (theIndex) : gp_Trsf();
  }

public: //! @name Configuration of graphical transformations

  //! Enable or disable zoom persistence mode for the manipulator. With
  //! this mode turned on the presentation will keep fixed screen size.
  //! @warning when turned on this option overrides transform persistence
  //! properties and local transformation to achieve necessary visual effect.
  //! @warning revise use of AdjustSize argument of of \sa AttachToObjects method
  //! when enabling zoom persistence.
  Standard_EXPORT void SetZoomPersistence (const Standard_Boolean theToEnable);

  //! Returns state of zoom persistence mode, whether it turned on or off.
  Standard_Boolean ZoomPersistence() const { return myIsZoomPersistentMode; }

  //! Redefines transform persistence management to setup transformation for sub-presentation of axes.
  //! @warning this interactive object does not support custom transformation persistence when
  //! using \sa ZoomPersistence mode. In this mode the transformation persistence flags for
  //! presentations are overridden by this class.
  //! @warning Invokes debug assertion to catch incompatible usage of the method with \sa ZoomPersistence mode,
  //! silently does nothing in release mode.
  //! @warning revise use of AdjustSize argument of of \sa AttachToObjects method
  //! when enabling zoom persistence.
  Standard_EXPORT virtual void SetTransformPersistence (const  Handle(Graphic3d_TransformPers)& theTrsfPers) Standard_OVERRIDE;

public: //! @name Setters for parameters

  AIS_ManipulatorMode ActiveMode() const { return myCurrentMode; }

  Standard_Integer ActiveAxisIndex() const { return myCurrentIndex; }

  //! @return poition of manipulator interactive object.
  const gp_Ax2& Position() const { return myPosition; }

  //! Sets position of the manipulator object.
  Standard_EXPORT void SetPosition (const gp_Ax2& thePosition);

  Standard_ShortReal Size() const { return myAxes[0].Size(); }

  //! Sets size (length of side of the manipulator cubic bounding box.
  Standard_EXPORT void SetSize (const Standard_ShortReal theSideLength);

  //! Sets gaps between translator, scaler and rotator sub-presentations.
  Standard_EXPORT void SetGap (const Standard_ShortReal theValue);

public:

  //! Behavior settings to be applied when performing transformation:
  //! - FollowTranslation - whether the manipulator will be moved together with an object.
  //! - FollowRotation - whether the manipulator will be rotated together with an object.
  struct BehaviorOnTransform {

    BehaviorOnTransform() : FollowTranslation (Standard_True), FollowRotation (Standard_True), FollowDragging (Standard_True) {}
    BehaviorOnTransform& SetFollowTranslation (const Standard_Boolean theApply) { FollowTranslation = theApply; return *this; }
    BehaviorOnTransform& SetFollowRotation    (const Standard_Boolean theApply) { FollowRotation    = theApply; return *this; }
    BehaviorOnTransform& SetFollowDragging    (const Standard_Boolean theApply) { FollowDragging    = theApply; return *this; }

    Standard_Boolean FollowTranslation;
    Standard_Boolean FollowRotation;
    Standard_Boolean FollowDragging;
  };

  //! Sets behavior settings for transformation action carried on the manipulator,
  //! whether it translates, rotates together with the transformed object or not.
  void SetTransformBehavior (const BehaviorOnTransform& theSettings) { myBehaviorOnTransform = theSettings; }

  //! @return behavior settings for transformation action of the manipulator.
  BehaviorOnTransform& ChangeTransformBehavior() { return myBehaviorOnTransform; }

  //! @return behavior settings for transformation action of the manipulator.
  const BehaviorOnTransform& TransformBehavior() const { return myBehaviorOnTransform; }

public: //! @name Presentation computation

  //! Fills presentation.
  //! @note Manipulator presentation does not use display mode and for all modes has the same presentation.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode = 0) Standard_OVERRIDE;

  //! Computes selection sensitive zones (triangulation) for manipulator.
  //! @param theNode [in] Selection mode that is treated as transformation mode.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  //! Disables auto highlighting to use HilightSelected() and HilightOwnerWithColor() overridden methods.
  virtual Standard_Boolean IsAutoHilight() const Standard_OVERRIDE
  {
    return Standard_False;
  }

  //! Method which clear all selected owners belonging
  //! to this selectable object ( for fast presentation draw ).
  Standard_EXPORT virtual void ClearSelected() Standard_OVERRIDE;

  //! Method which draws selected owners ( for fast presentation draw ).
  Standard_EXPORT virtual void HilightSelected (const Handle(PrsMgr_PresentationManager)& thePM, const SelectMgr_SequenceOfOwner& theSeq) Standard_OVERRIDE;

  //! Method which hilight an owner belonging to
  //! this selectable object  ( for fast presentation draw ).
  Standard_EXPORT virtual void HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                                      const Handle(Prs3d_Drawer)& theStyle,
                                                      const Handle(SelectMgr_EntityOwner)& theOwner) Standard_OVERRIDE;

protected:

  Standard_EXPORT void init();

  Standard_EXPORT void updateTransformation();

  Standard_EXPORT Handle(Prs3d_Presentation) getHighlightPresentation (const Handle(SelectMgr_EntityOwner)& theOwner) const;

  Standard_EXPORT Handle(Graphic3d_Group) getGroup (const Standard_Integer theIndex, const AIS_ManipulatorMode theMode) const;

  Standard_EXPORT void attachToBox (const Bnd_Box& theBox);

  Standard_EXPORT void adjustSize (const Bnd_Box& theBox);

  Standard_EXPORT void setTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers);

  //! Redefines local transformation management method to inform user of improper use.
  //! @warning this interactive object does not support setting custom local transformation,
  //! this class solely uses this property to implement visual positioning of the manipulator
  //! without need for recomputing presentation.
  //! @warning Invokes debug assertion in debug to catch incompatible usage of the
  //! method, silently does nothing in release mode.
  Standard_EXPORT virtual void setLocalTransformation (const Handle(TopLoc_Datum3D)& theTrsf) Standard_OVERRIDE;
  using AIS_InteractiveObject::SetLocalTransformation; // hide visibility

protected: //! @name Auxiliary classes to fill presentation with proper primitives

  class Quadric
  {
  public:

    virtual ~Quadric()
    {
      myTriangulation.Nullify();
      myArray.Nullify();
    }


    const Handle(Poly_Triangulation)& Triangulation() const { return myTriangulation; }

    const Handle(Graphic3d_ArrayOfTriangles)& Array() const { return myArray; }

  protected:

    Handle(Poly_Triangulation) myTriangulation;
    Handle(Graphic3d_ArrayOfTriangles) myArray;
  };

  class Disk : public Quadric
  {
  public:

    Disk()
      : Quadric(),
      myInnerRad(0.0f),
      myOuterRad(1.0f)
    { }

    ~Disk() { }

    void Init (const Standard_ShortReal theInnerRadius,
               const Standard_ShortReal theOuterRadius,
               const gp_Ax1& thePosition,
               const Standard_Integer theSlicesNb = 20,
               const Standard_Integer theStacksNb = 20);

  protected:

    gp_Ax1             myPosition;
    Standard_ShortReal myInnerRad;
    Standard_ShortReal myOuterRad;
  };

  class Sphere : public Quadric
  {
  public:
    Sphere()
      : Quadric(),
      myRadius(1.0f)
    {}

    void Init (const Standard_ShortReal theRadius,
               const gp_Pnt& thePosition,
               const Standard_Integer theSlicesNb = 20,
               const Standard_Integer theStacksNb = 20);

  protected:

    gp_Pnt myPosition;
    Standard_ShortReal myRadius;
  };

  class Cube
  {
  public:

    Cube() { }
    ~Cube() { }

    void Init (const gp_Ax1& thePosition, const Standard_ShortReal myBoxSize);

    const Handle(Poly_Triangulation)& Triangulation() const { return myTriangulation; }

    const Handle(Graphic3d_ArrayOfTriangles)& Array() const { return myArray; }

  private:

    void addTriangle (const Standard_Integer theIndex, const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3,
                      const gp_Dir& theNormal);

  protected:

    Handle(Poly_Triangulation) myTriangulation;
    Handle(Graphic3d_ArrayOfTriangles) myArray;
  };

  class Sector : public Quadric
  {
  public:

    Sector()
      : Quadric(),
      myRadius(0.0f)
    { }

    ~Sector() { }

    void Init(const Standard_ShortReal theRadius,
              const gp_Ax1&            thePosition,
              const gp_Dir&            theXDirection,
              const Standard_Integer   theSlicesNb = 5,
              const Standard_Integer   theStacksNb = 5);

  protected:

    gp_Ax1             myPosition;
    Standard_ShortReal myRadius;
  };

  //! The class describes on axis sub-object.
  //! It includes sub-objects itself:
  //! -rotator
  //! -translator
  //! -scaler
  class Axis
  {
  public:

    Axis (const gp_Ax1& theAxis              = gp_Ax1(),
          const Quantity_Color& theColor     = Quantity_Color(),
          const Standard_ShortReal theLength = 10.0f);

    void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                  const Handle(Prs3d_Presentation)& thePrs,
                  const Handle(Prs3d_ShadingAspect)& theAspect);

    const gp_Ax1& ReferenceAxis() const { return myReferenceAxis; }

    void SetPosition (const gp_Ax1& thePosition) { myPosition = thePosition; }

    const gp_Ax1& Position() const { return myPosition; }

    void SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
    {
      if (!myHighlightTranslator.IsNull())
      {
        myHighlightTranslator->SetTransformPersistence (theTrsfPers);
      }

      if (!myHighlightScaler.IsNull())
      {
        myHighlightScaler->SetTransformPersistence (theTrsfPers);
      }

      if (!myHighlightRotator.IsNull())
      {
        myHighlightRotator->SetTransformPersistence (theTrsfPers);
      }

      if (!myHighlightDragger.IsNull())
      {
        myHighlightDragger->SetTransformPersistence(theTrsfPers);
      }
    }

    void Transform (const Handle(TopLoc_Datum3D)& theTransformation)
    {
      if (!myHighlightTranslator.IsNull())
      {
        myHighlightTranslator->SetTransformation (theTransformation);
      }

      if (!myHighlightScaler.IsNull())
      {
        myHighlightScaler->SetTransformation (theTransformation);
      }

      if (!myHighlightRotator.IsNull())
      {
        myHighlightRotator->SetTransformation (theTransformation);
      }

      if (!myHighlightDragger.IsNull())
      {
        myHighlightDragger->SetTransformation(theTransformation);
      }
    }

    Standard_Boolean HasTranslation() const { return myHasTranslation; }

    Standard_Boolean HasRotation() const { return myHasRotation; }

    Standard_Boolean HasScaling() const { return myHasScaling; }

    Standard_Boolean HasDragging() const { return myHasDragging; }

    void SetTranslation (const Standard_Boolean theIsEnabled) { myHasTranslation = theIsEnabled; }

    void SetRotation (const Standard_Boolean theIsEnabled) { myHasRotation = theIsEnabled; }

    void SetScaling (const Standard_Boolean theIsEnabled) { myHasScaling = theIsEnabled; }

    void SetDragging(const Standard_Boolean theIsEnabled) { myHasDragging = theIsEnabled; }

    Quantity_Color Color() const { return myColor; }

    Standard_ShortReal AxisLength() const { return myLength; }

    Standard_ShortReal AxisRadius() const { return myAxisRadius; }

    void SetAxisRadius (const Standard_ShortReal theValue) { myAxisRadius = theValue; }

    const Handle(Prs3d_Presentation)& TranslatorHighlightPrs() const { return myHighlightTranslator; }

    const Handle(Prs3d_Presentation)& RotatorHighlightPrs() const { return myHighlightRotator; }

    const Handle(Prs3d_Presentation)& ScalerHighlightPrs() const { return myHighlightScaler; }

    const Handle(Prs3d_Presentation)& DraggerHighlightPrs() const { return myHighlightDragger; }

    const Handle(Graphic3d_Group)& TranslatorGroup() const { return myTranslatorGroup; }

    const Handle(Graphic3d_Group)& RotatorGroup() const { return myRotatorGroup; }

    const Handle(Graphic3d_Group)& ScalerGroup() const { return myScalerGroup; }

    const Handle(Graphic3d_Group)& DraggerGroup() const { return myDraggerGroup; }

    const Handle(Graphic3d_ArrayOfTriangles)& TriangleArray() const { return myTriangleArray; }

    void SetIndent (const Standard_ShortReal theValue) { myIndent = theValue; }

    Standard_ShortReal Size() const { return myLength + myBoxSize + myDiskThickness + myIndent * 2.0f; }

    gp_Pnt ScalerCenter (const gp_Pnt& theLocation) const { return theLocation.XYZ() + myPosition.Direction().XYZ() * (myLength + myIndent + myBoxSize * 0.5f); }

    void SetSize (const Standard_ShortReal theValue)
    {
      if (myIndent > theValue * 0.1f)
      {
        myLength = theValue * 0.7f;
        myBoxSize = theValue * 0.15f;
        myDiskThickness = theValue * 0.05f;
        myIndent = theValue * 0.05f;
      }
      else // use pre-set value of predent
      {
        Standard_ShortReal aLength = theValue - 2 * myIndent;
        myLength = aLength * 0.8f;
        myBoxSize = aLength * 0.15f;
        myDiskThickness = aLength * 0.05f;
      }
      myInnerRadius = myIndent * 2 + myBoxSize + myLength;
      myAxisRadius = myBoxSize / 4.0f;
    }

    Standard_Integer FacettesNumber() const { return myFacettesNumber; }

  public:

    const gp_Pnt& TranslatorTipPosition() const { return myArrowTipPos; }
    const Sector& DraggerSector() const { return mySector; }
    const Disk& RotatorDisk() const { return myCircle; }
    float RotatorDiskRadius() const { return myCircleRadius; }
    const Cube& ScalerCube() const { return myCube; }
    const gp_Pnt& ScalerCubePosition() const { return myCubePos; }

  protected:

    gp_Ax1 myReferenceAxis; //!< Returns reference axis assignment.
    gp_Ax1 myPosition; //!< Position of the axis including local transformation.
    Quantity_Color myColor;

    Standard_Boolean myHasTranslation;
    Standard_ShortReal myLength; //!< Length of translation axis.
    Standard_ShortReal myAxisRadius;

    Standard_Boolean myHasScaling;
    Standard_ShortReal myBoxSize; //!< Size of scaling cube.

    Standard_Boolean myHasRotation;
    Standard_ShortReal myInnerRadius; //!< Radius of rotation circle.
    Standard_ShortReal myDiskThickness;
    Standard_ShortReal myIndent; //!< Gap between visual part of the manipulator.

    Standard_Boolean myHasDragging;

  protected:

    Standard_Integer myFacettesNumber;

    gp_Pnt   myArrowTipPos;
    Sector   mySector;
    Disk     myCircle;
    float    myCircleRadius;
    Cube     myCube;
    gp_Pnt   myCubePos;

    Handle(Graphic3d_Group) myTranslatorGroup;
    Handle(Graphic3d_Group) myScalerGroup;
    Handle(Graphic3d_Group) myRotatorGroup;
    Handle(Graphic3d_Group) myDraggerGroup;

    Handle(Prs3d_Presentation) myHighlightTranslator;
    Handle(Prs3d_Presentation) myHighlightScaler;
    Handle(Prs3d_Presentation) myHighlightRotator;
    Handle(Prs3d_Presentation) myHighlightDragger;

    Handle(Graphic3d_ArrayOfTriangles) myTriangleArray;

  };

protected:

  Axis myAxes[3]; //!< Tree axes of the manipulator.
  Sphere myCenter; //!< Visual part displaying the center sphere of the manipulator.
  gp_Ax2 myPosition; //!< Position of the manipulator object. it displays its location and position of its axes.

  Standard_Integer myCurrentIndex; //!< Index of active axis.
  AIS_ManipulatorMode myCurrentMode; //!< Name of active manipulation mode.

  Standard_Boolean myIsActivationOnDetection; //!< Manual activation of modes (not on parts selection).
  Standard_Boolean myIsZoomPersistentMode; //!< Zoom persistence mode activation.
  BehaviorOnTransform myBehaviorOnTransform; //!< Behavior settings applied on manipulator when transforming an object.

protected: //! @name Fields for interactive transformation. Fields only for internal needs. They do not have public interface.

  NCollection_Sequence<gp_Trsf> myStartTrsfs; //!< Owning object transformation for start. It is used internally.
  Standard_Boolean myHasStartedTransformation; //!< Shows if transformation is processed (sequential calls of Transform()).
  gp_Ax2 myStartPosition; //! Start position of manipulator.
  gp_Pnt myStartPick; //! 3d point corresponding to start mouse pick.
  Standard_Real myPrevState; //! Previous value of angle during rotation.

  //! Aspect used to color current detected part and current selected part.
  Handle(Prs3d_ShadingAspect) myHighlightAspect;

  //! Aspect used to color sector part when it's selected.
  Handle(Prs3d_ShadingAspect) myDraggerHighlight;
public:

  DEFINE_STANDARD_RTTIEXT(AIS_Manipulator, AIS_InteractiveObject)
};
#endif // _AIS_Manipulator_HeaderFile
