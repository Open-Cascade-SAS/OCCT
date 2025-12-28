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
//! occ::handle<AIS_Manipulator> aManipulator = new AIS_Manipulator();
//! aManipulator->SetPart (0, AIS_Manipulator::Scaling, false);
//! aManipulator->SetPart (1, AIS_Manipulator::Rotation, false);
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
//! aManipulator->SetModeActivationOnDetection (true);
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
//! aManipulator->StopTransform(/*bool toApply*/);
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

  //! Constructs a manipulator object with input location and positions of axes and all parts to be
  //! displayed.
  Standard_EXPORT AIS_Manipulator(const gp_Ax2& thePosition);

  //! Disable or enable visual parts for translation, rotation or scaling for some axis.
  //! By default all parts are enabled (will be displayed).
  //! @warning Enabling or disabling of visual parts of manipulator does not manage the manipulation
  //! (selection) mode.
  //! @warning Raises program error if axis index is < 0 or > 2.
  Standard_EXPORT void SetPart(const int                 theAxisIndex,
                               const AIS_ManipulatorMode theMode,
                               const bool                theIsEnabled);

  //! Disable or enable visual parts for translation, rotation or scaling for ALL axes.
  //! By default all parts are enabled (will be displayed).
  //! @warning Enabling or disabling of visual parts of manipulator does not manage the manipulation
  //! (selection) mode.
  //! @warning Raises program error if axis index is < 0 or > 2.
  Standard_EXPORT void SetPart(const AIS_ManipulatorMode theMode, const bool theIsEnabled);

  //! Behavior settings to be applied when performing transformation:
  //! - FollowTranslation - whether the manipulator will be moved together with an object.
  //! - FollowRotation - whether the manipulator will be rotated together with an object.
  struct OptionsForAttach
  {

    OptionsForAttach()
        : AdjustPosition(true),
          AdjustSize(false),
          EnableModes(true)
    {
    }

    OptionsForAttach& SetAdjustPosition(const bool theApply)
    {
      AdjustPosition = theApply;
      return *this;
    }

    OptionsForAttach& SetAdjustSize(const bool theApply)
    {
      AdjustSize = theApply;
      return *this;
    }

    OptionsForAttach& SetEnableModes(const bool theApply)
    {
      EnableModes = theApply;
      return *this;
    }

    bool AdjustPosition;
    bool AdjustSize;
    bool EnableModes;
  };

  //! Attaches himself to the input interactive object and become displayed in the same context.
  //! It is placed in the center of object bounding box, and its size is adjusted to the object
  //! bounding box.
  Standard_EXPORT void Attach(const occ::handle<AIS_InteractiveObject>& theObject,
                              const OptionsForAttach& theOptions = OptionsForAttach());

  //! Attaches himself to the input interactive object group and become displayed in the same
  //! context. It become attached to the first object, baut manage manipulation of the whole group.
  //! It is placed in the center of object bounding box, and its size is adjusted to the object
  //! bounding box.
  Standard_EXPORT void Attach(
    const occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>>& theObject,
    const OptionsForAttach& theOptions = OptionsForAttach());

  //! Enable manipualtion mode.
  //! @warning It activates selection mode in the current context.
  //! If manipulator is not displayed, no mode will be activated.
  Standard_EXPORT void EnableMode(const AIS_ManipulatorMode theMode);

  //! Enables mode activation on detection (highlighting).
  //! By default, mode is activated on selection of manipulator part.
  //! @warning If this mode is enabled, selection of parts does nothing.
  void SetModeActivationOnDetection(const bool theToEnable)
  {
    myIsActivationOnDetection = theToEnable;
  }

  //! @return true if manual mode activation is enabled.
  bool IsModeActivationOnDetection() const { return myIsActivationOnDetection; }

public:
  //! Drag object in the viewer.
  //! @param[in] theCtx       interactive context
  //! @param[in] theView      active View
  //! @param[in] theOwner     the owner of detected entity
  //! @param[in] theDragFrom  drag start point
  //! @param[in] theDragTo    drag end point
  //! @param[in] theAction    drag action
  //! @return FALSE if object rejects dragging action (e.g. AIS_DragAction_Start)
  Standard_EXPORT bool ProcessDragging(const occ::handle<AIS_InteractiveContext>& theCtx,
                                       const occ::handle<V3d_View>&               theView,
                                       const occ::handle<SelectMgr_EntityOwner>&  theOwner,
                                       const NCollection_Vec2<int>&               theDragFrom,
                                       const NCollection_Vec2<int>&               theDragTo,
                                       const AIS_DragAction theAction) override;

  //! Init start (reference) transformation.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  //! and is used only for custom transform set. If Transform(const int, const
  //! int) is used, initial data is set automatically, and it is reset on
  //! DeactivateCurrentMode call if it is not reset yet.
  Standard_EXPORT void StartTransform(const int                    theX,
                                      const int                    theY,
                                      const occ::handle<V3d_View>& theView);

  //! Apply to the owning objects the input transformation.
  //! @remark The transformation is set using SetLocalTransformation for owning objects.
  //! The location of the manipulator is stored also in Local Transformation,
  //! so that there's no need to redisplay objects.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  //! and is used only for custom transform set.
  //! @warning It will does nothing if transformation is not initiated (with StartTransform() call).
  Standard_EXPORT void Transform(const gp_Trsf& aTrsf);

  //! Apply camera transformation to flat skin manipulator
  Standard_EXPORT void RecomputeTransformation(
    const occ::handle<Graphic3d_Camera>& theCamera) override;

  //! Recomputes sensitive primitives for the given selection mode.
  //! @param theMode selection mode to recompute sensitive primitives
  Standard_EXPORT void RecomputeSelection(const AIS_ManipulatorMode theMode);

  //! Reset start (reference) transformation.
  //! @param[in] theToApply  option to apply or to cancel the started transformation.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  //! and is used only for custom transform set.
  Standard_EXPORT void StopTransform(const bool theToApply = true);

  //! Apply transformation made from mouse moving from start position
  //! (save on the first Transform() call and reset on DeactivateCurrentMode() call.)
  //! to the in/out mouse position (theX, theY)
  Standard_EXPORT gp_Trsf Transform(const int                    theX,
                                    const int                    theY,
                                    const occ::handle<V3d_View>& theView);

  //! Computes transformation of parent object according to the active mode and input motion vector.
  //! You can use this method to get object transformation according to current mode or use own
  //! algorithm to implement any other transformation for modes.
  //! @return transformation of parent object.
  Standard_EXPORT bool ObjectTransformation(const int                    theX,
                                            const int                    theY,
                                            const occ::handle<V3d_View>& theView,
                                            gp_Trsf&                     theTrsf);

  //! Make inactive the current selected manipulator part and reset current axis index and current
  //! mode. After its call HasActiveMode() returns false.
  //! @sa HasActiveMode()
  Standard_EXPORT void DeactivateCurrentMode();

  //! Detaches himself from the owner object, and removes itself from context.
  Standard_EXPORT void Detach();

  //! @return all owning objects.
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> Objects()
    const;

  //! @return the first (leading) object of the owning objects.
  Standard_EXPORT occ::handle<AIS_InteractiveObject> Object() const;

  //! @return one of the owning objects.
  //! @warning raises program error if theIndex is more than owning objects count or less than 1.
  Standard_EXPORT occ::handle<AIS_InteractiveObject> Object(const int theIndex) const;

  //! @return true if manipulator is attached to some interactive object (has owning object).
  bool IsAttached() const { return HasOwner(); }

  //! @return true if some part of manipulator is selected (transformation mode is active, and
  //! owning object can be transformed).
  bool HasActiveMode() const { return IsAttached() && myCurrentMode != AIS_MM_None; }

  bool HasActiveTransformation() { return myHasStartedTransformation; }

  gp_Trsf StartTransformation() const
  {
    return !myStartTrsfs.IsEmpty() ? myStartTrsfs.First() : gp_Trsf();
  }

  gp_Trsf StartTransformation(int theIndex) const
  {
    Standard_ProgramError_Raise_if(
      theIndex < 1 || theIndex > Objects()->Upper(),
      "AIS_Manipulator::StartTransformation(): theIndex is out of bounds");
    return !myStartTrsfs.IsEmpty() ? myStartTrsfs(theIndex) : gp_Trsf();
  }

public: //! @name Configuration of graphical transformations
  //! Enable or disable zoom persistence mode for the manipulator. With
  //! this mode turned on the presentation will keep fixed screen size.
  //! @warning when turned on this option overrides transform persistence
  //! properties and local transformation to achieve necessary visual effect.
  //! @warning revise use of AdjustSize argument of of \sa AttachToObjects method
  //! when enabling zoom persistence.
  Standard_EXPORT void SetZoomPersistence(const bool theToEnable);

  //! Returns state of zoom persistence mode, whether it turned on or off.
  bool ZoomPersistence() const { return myIsZoomPersistentMode; }

  //! Redefines transform persistence management to setup transformation for sub-presentation of
  //! axes.
  //! @warning this interactive object does not support custom transformation persistence when
  //! using \sa ZoomPersistence mode. In this mode the transformation persistence flags for
  //! presentations are overridden by this class.
  //! @warning Invokes debug assertion to catch incompatible usage of the method with \sa
  //! ZoomPersistence mode, silently does nothing in release mode.
  //! @warning revise use of AdjustSize argument of of \sa AttachToObjects method
  //! when enabling zoom persistence.
  Standard_EXPORT void SetTransformPersistence(
    const occ::handle<Graphic3d_TransformPers>& theTrsfPers) override;

public: //! @name Setters for parameters
  enum ManipulatorSkin
  {
    ManipulatorSkin_Shaded,
    ManipulatorSkin_Flat
  };

  //! @return current manipulator skin mode.
  ManipulatorSkin SkinMode() const { return mySkinMode; }

  //! Sets skin mode for the manipulator.
  Standard_EXPORT void SetSkinMode(const ManipulatorSkin theSkinMode);

  AIS_ManipulatorMode ActiveMode() const { return myCurrentMode; }

  int ActiveAxisIndex() const { return myCurrentIndex; }

  //! @return poition of manipulator interactive object.
  const gp_Ax2& Position() const { return myPosition; }

  //! Sets position of the manipulator object.
  Standard_EXPORT void SetPosition(const gp_Ax2& thePosition);

  float Size() const { return myAxes[0].Size(); }

  //! Sets size (length of side of the manipulator cubic bounding box.
  Standard_EXPORT void SetSize(const float theSideLength);

  //! Sets gaps between translator, scaler and rotator sub-presentations.
  Standard_EXPORT void SetGap(const float theValue);

public:
  //! Behavior settings to be applied when performing transformation:
  //! - FollowTranslation - whether the manipulator will be moved together with an object.
  //! - FollowRotation - whether the manipulator will be rotated together with an object.
  struct BehaviorOnTransform
  {

    BehaviorOnTransform()
        : FollowTranslation(true),
          FollowRotation(true),
          FollowDragging(true)
    {
    }

    BehaviorOnTransform& SetFollowTranslation(const bool theApply)
    {
      FollowTranslation = theApply;
      return *this;
    }

    BehaviorOnTransform& SetFollowRotation(const bool theApply)
    {
      FollowRotation = theApply;
      return *this;
    }

    BehaviorOnTransform& SetFollowDragging(const bool theApply)
    {
      FollowDragging = theApply;
      return *this;
    }

    bool FollowTranslation;
    bool FollowRotation;
    bool FollowDragging;
  };

  //! Sets behavior settings for transformation action carried on the manipulator,
  //! whether it translates, rotates together with the transformed object or not.
  void SetTransformBehavior(const BehaviorOnTransform& theSettings)
  {
    myBehaviorOnTransform = theSettings;
  }

  //! @return behavior settings for transformation action of the manipulator.
  BehaviorOnTransform& ChangeTransformBehavior() { return myBehaviorOnTransform; }

  //! @return behavior settings for transformation action of the manipulator.
  const BehaviorOnTransform& TransformBehavior() const { return myBehaviorOnTransform; }

public: //! @name Presentation computation
  //! Fills presentation.
  //! @note Manipulator presentation does not use display mode and for all modes has the same
  //! presentation.
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                               const occ::handle<Prs3d_Presentation>&         thePrs,
                               const int                                      theMode = 0) override;

  //! Computes selection sensitive zones (triangulation) for manipulator.
  //! @param[in] theNode  Selection mode that is treated as transformation mode.
  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                        const int                               theMode) override;

  //! Disables auto highlighting to use HilightSelected() and HilightOwnerWithColor() overridden
  //! methods.
  bool IsAutoHilight() const override { return false; }

  //! Method which clear all selected owners belonging
  //! to this selectable object (for fast presentation draw).
  Standard_EXPORT void ClearSelected() override;

  //! Method which draws selected owners (for fast presentation draw).
  Standard_EXPORT void HilightSelected(
    const occ::handle<PrsMgr_PresentationManager>&                  thePM,
    const NCollection_Sequence<occ::handle<SelectMgr_EntityOwner>>& theSeq) override;

  //! Method which hilight an owner belonging to
  //! this selectable object (for fast presentation draw).
  Standard_EXPORT void HilightOwnerWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePM,
    const occ::handle<Prs3d_Drawer>&               theStyle,
    const occ::handle<SelectMgr_EntityOwner>&      theOwner) override;

protected:
  Standard_EXPORT void init();

  Standard_EXPORT void updateTransformation();

  Standard_EXPORT occ::handle<Prs3d_Presentation> getHighlightPresentation(
    const occ::handle<SelectMgr_EntityOwner>& theOwner) const;

  Standard_EXPORT occ::handle<Graphic3d_Group> getGroup(const int                 theIndex,
                                                        const AIS_ManipulatorMode theMode) const;

  Standard_EXPORT void attachToPoint(const gp_Pnt& thePoint);

  Standard_EXPORT void attachToBox(const Bnd_Box& theBox);

  Standard_EXPORT void adjustSize(const Bnd_Box& theBox);

  Standard_EXPORT void setTransformPersistence(
    const occ::handle<Graphic3d_TransformPers>& theTrsfPers);

  //! Redefines local transformation management method to inform user of improper use.
  //! @warning this interactive object does not support setting custom local transformation,
  //! this class solely uses this property to implement visual positioning of the manipulator
  //! without need for recomputing presentation.
  //! @warning Invokes debug assertion in debug to catch incompatible usage of the
  //! method, silently does nothing in release mode.
  Standard_EXPORT void setLocalTransformation(const occ::handle<TopLoc_Datum3D>& theTrsf) override;
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

    const occ::handle<Poly_Triangulation>& Triangulation() const { return myTriangulation; }

    const occ::handle<Graphic3d_ArrayOfTriangles>& Array() const { return myArray; }

  protected:
    occ::handle<Poly_Triangulation>         myTriangulation;
    occ::handle<Graphic3d_ArrayOfTriangles> myArray;
  };

  class Disk : public Quadric
  {
  public:
    Disk()
        : Quadric(),
          myInnerRad(0.0f),
          myOuterRad(1.0f)
    {
    }

    ~Disk() override = default;

    void Init(const float   theInnerRadius,
              const float   theOuterRadius,
              const gp_Ax1& thePosition,
              const double  theAngle,
              const int     theSlicesNb = 20,
              const int     theStacksNb = 20);

  protected:
    gp_Ax1 myPosition;
    float  myInnerRad;
    float  myOuterRad;
  };

  class Sphere : public Quadric
  {
  public:
    Sphere()
        : Quadric(),
          myRadius(1.0f)
    {
    }

    void Init(const float           theRadius,
              const gp_Pnt&         thePosition,
              const ManipulatorSkin theSkinMode,
              const int             theSlicesNb = 20,
              const int             theStacksNb = 20);

  protected:
    gp_Pnt myPosition;
    float  myRadius;
  };

  class Cube
  {
  public:
    Cube() = default;

    ~Cube() = default;

    void Init(const gp_Ax1& thePosition, const float myBoxSize, const ManipulatorSkin theSkinMode);

    const occ::handle<Poly_Triangulation>& Triangulation() const { return myTriangulation; }

    const occ::handle<Graphic3d_ArrayOfTriangles>& Array() const { return myArray; }

  private:
    void addTriangle(const int     theIndex,
                     const gp_Pnt& theP1,
                     const gp_Pnt& theP2,
                     const gp_Pnt& theP3,
                     const gp_Dir& theNormal);

  protected:
    occ::handle<Poly_Triangulation>         myTriangulation;
    occ::handle<Graphic3d_ArrayOfTriangles> myArray;
  };

  class Sector : public Quadric
  {
  public:
    Sector()
        : Quadric(),
          myRadius(0.0f)
    {
    }

    ~Sector() override = default;

    void Init(const float           theRadius,
              const gp_Ax1&         thePosition,
              const gp_Dir&         theXDirection,
              const ManipulatorSkin theSkinMode,
              const int             theSlicesNb = 5,
              const int             theStacksNb = 5);

  protected:
    gp_Ax1 myPosition;
    float  myRadius;
  };

  //! The class describes on axis sub-object.
  //! It includes sub-objects itself:
  //! -rotator
  //! -translator
  //! -scaler
  class Axis
  {
  public:
    Axis(const gp_Ax1&         theAxis   = gp_Ax1(),
         const Quantity_Color& theColor  = Quantity_Color(),
         const float           theLength = 10.0f);

    void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                 const occ::handle<Prs3d_Presentation>&         thePrs,
                 const occ::handle<Prs3d_ShadingAspect>&        theAspect,
                 const ManipulatorSkin                          theSkinMode);

    const gp_Ax1& ReferenceAxis() const { return myReferenceAxis; }

    void SetPosition(const gp_Ax1& thePosition) { myPosition = thePosition; }

    const gp_Ax1& Position() const { return myPosition; }

    void SetTransformPersistence(const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
    {
      if (!myHighlightTranslator.IsNull())
      {
        myHighlightTranslator->SetTransformPersistence(theTrsfPers);
      }

      if (!myHighlightScaler.IsNull())
      {
        myHighlightScaler->SetTransformPersistence(theTrsfPers);
      }

      if (!myHighlightRotator.IsNull())
      {
        myHighlightRotator->SetTransformPersistence(theTrsfPers);
      }

      if (!myHighlightDragger.IsNull())
      {
        myHighlightDragger->SetTransformPersistence(theTrsfPers);
      }
    }

    void Transform(const occ::handle<TopLoc_Datum3D>& theTransformation)
    {
      if (!myHighlightTranslator.IsNull())
      {
        myHighlightTranslator->SetTransformation(theTransformation);
      }

      if (!myHighlightScaler.IsNull())
      {
        myHighlightScaler->SetTransformation(theTransformation);
      }

      if (!myHighlightRotator.IsNull())
      {
        myHighlightRotator->SetTransformation(theTransformation);
      }

      if (!myHighlightDragger.IsNull())
      {
        myHighlightDragger->SetTransformation(theTransformation);
      }
    }

    bool HasTranslation() const { return myHasTranslation; }

    bool HasRotation() const { return myHasRotation; }

    bool HasScaling() const { return myHasScaling; }

    bool HasDragging() const { return myHasDragging; }

    void SetTranslation(const bool theIsEnabled) { myHasTranslation = theIsEnabled; }

    void SetRotation(const bool theIsEnabled) { myHasRotation = theIsEnabled; }

    void SetScaling(const bool theIsEnabled) { myHasScaling = theIsEnabled; }

    void SetDragging(const bool theIsEnabled) { myHasDragging = theIsEnabled; }

    Quantity_Color Color() const { return myColor; }

    float AxisLength() const { return myLength; }

    float BoxSize() const { return myBoxSize; }

    float AxisRadius() const { return myAxisRadius; }

    float Indent() const { return myIndent; }

    void SetAxisRadius(const float theValue) { myAxisRadius = theValue; }

    const occ::handle<Prs3d_Presentation>& TranslatorHighlightPrs() const
    {
      return myHighlightTranslator;
    }

    const occ::handle<Prs3d_Presentation>& RotatorHighlightPrs() const
    {
      return myHighlightRotator;
    }

    const occ::handle<Prs3d_Presentation>& ScalerHighlightPrs() const { return myHighlightScaler; }

    const occ::handle<Prs3d_Presentation>& DraggerHighlightPrs() const
    {
      return myHighlightDragger;
    }

    const occ::handle<Graphic3d_Group>& TranslatorGroup() const { return myTranslatorGroup; }

    const occ::handle<Graphic3d_Group>& RotatorGroup() const { return myRotatorGroup; }

    const occ::handle<Graphic3d_Group>& ScalerGroup() const { return myScalerGroup; }

    const occ::handle<Graphic3d_Group>& DraggerGroup() const { return myDraggerGroup; }

    const occ::handle<Graphic3d_ArrayOfTriangles>& TriangleArray() const { return myTriangleArray; }

    void SetIndent(const float theValue) { myIndent = theValue; }

    float Size() const { return myInnerRadius + myDiskThickness + myIndent * 2; }

    float InnerRadius() const { return myInnerRadius + myIndent * 2.0f; }

    gp_Pnt ScalerCenter(const gp_Pnt& theLocation) const
    {
      return theLocation.XYZ()
             + myPosition.Direction().XYZ() * (myLength + myIndent + myBoxSize * 0.5f);
    }

    void SetSize(const float theValue)
    {
      if (myIndent > theValue * 0.1f)
      {
        myLength        = theValue * 0.7f;
        myBoxSize       = theValue * 0.15f;
        myDiskThickness = theValue * 0.05f;
        myIndent        = theValue * 0.05f;
      }
      else // use pre-set value of predent
      {
        float aLength   = theValue - 2 * myIndent;
        myLength        = aLength * 0.8f;
        myBoxSize       = aLength * 0.15f;
        myDiskThickness = aLength * 0.05f;
      }
      myInnerRadius = myIndent * 2 + myBoxSize + myLength;
      myAxisRadius  = myBoxSize / 4.0f;
    }

    int FacettesNumber() const { return myFacettesNumber; }

  public:
    const gp_Pnt& TranslatorTipPosition() const { return myArrowTipPos; }

    const Sector& DraggerSector() const { return mySector; }

    const Disk& RotatorDisk() const { return myCircle; }

    float RotatorDiskRadius() const { return myCircleRadius; }

    const Cube& ScalerCube() const { return myCube; }

    const gp_Pnt& ScalerCubePosition() const { return myCubePos; }

  protected:
    gp_Ax1         myReferenceAxis; //!< Returns reference axis assignment.
    gp_Ax1         myPosition;      //!< Position of the axis including local transformation.
    Quantity_Color myColor;

    bool  myHasTranslation;
    float myLength; //!< Length of translation axis.
    float myAxisRadius;

    bool  myHasScaling;
    float myBoxSize; //!< Size of scaling cube.

    bool  myHasRotation;
    float myInnerRadius; //!< Radius of rotation circle.
    float myDiskThickness;
    float myIndent; //!< Gap between visual part of the manipulator.

    bool myHasDragging;

  protected:
    int myFacettesNumber;

    gp_Pnt myArrowTipPos;
    Sector mySector;
    Disk   myCircle;
    float  myCircleRadius;
    Cube   myCube;
    gp_Pnt myCubePos;

    occ::handle<Graphic3d_Group> myTranslatorGroup;
    occ::handle<Graphic3d_Group> myScalerGroup;
    occ::handle<Graphic3d_Group> myRotatorGroup;
    occ::handle<Graphic3d_Group> myDraggerGroup;

    occ::handle<Prs3d_Presentation> myHighlightTranslator;
    occ::handle<Prs3d_Presentation> myHighlightScaler;
    occ::handle<Prs3d_Presentation> myHighlightRotator;
    occ::handle<Prs3d_Presentation> myHighlightDragger;

    occ::handle<Graphic3d_ArrayOfTriangles> myTriangleArray;
  };

protected:
  Axis   myAxes[3];               //!< Tree axes of the manipulator.
  Sphere myCenter;                //!< Visual part displaying the center sphere of the manipulator.
                                  // clang-format off
  gp_Ax2 myPosition; //!< Position of the manipulator object. it displays its location and position of its axes.

  Disk                    myCircle; //!< Outer circle
  occ::handle<Graphic3d_Group> myCircleGroup;

  Disk                    mySector; //!< Sector indicating the rotation angle
  occ::handle<Graphic3d_Group> mySectorGroup;

  int myCurrentIndex; //!< Index of active axis.
  AIS_ManipulatorMode myCurrentMode; //!< Name of active manipulation mode.
  ManipulatorSkin mySkinMode; //!< Name of active skin mode.

  bool myIsActivationOnDetection; //!< Manual activation of modes (not on parts selection).
  bool myIsZoomPersistentMode; //!< Zoom persistence mode activation.
  BehaviorOnTransform myBehaviorOnTransform; //!< Behavior settings applied on manipulator when transforming an object.

protected: //! @name Fields for interactive transformation. Fields only for internal needs. They do not have public interface.

  NCollection_Sequence<gp_Trsf> myStartTrsfs; //!< Owning object transformation for start. It is used internally.
  bool myHasStartedTransformation; //!< Shows if transformation is processed (sequential calls of Transform()).
                                  // clang-format on
  gp_Ax2 myStartPosition;         //! Start position of manipulator.
  gp_Pnt myStartPick;             //! 3d point corresponding to start mouse pick.
  double myPrevState;             //! Previous value of angle during rotation.

  //! Aspect used to color current detected part and current selected part.
  occ::handle<Prs3d_ShadingAspect> myHighlightAspect;

  //! Aspect used to color sector part when it's selected.
  occ::handle<Prs3d_ShadingAspect> myDraggerHighlight;

public:
  DEFINE_STANDARD_RTTIEXT(AIS_Manipulator, AIS_InteractiveObject)
};
#endif // _AIS_Manipulator_HeaderFile
