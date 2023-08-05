// Created on: 2017-07-25
// Created by: Anastasia BOBYLEVA
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _AIS_ViewCube_HeaderFile
#define _AIS_ViewCube_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_Vec2.hxx>
#include <Prs3d_DatumParts.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>

class AIS_AnimationCamera;
class AIS_ViewCubeOwner;
class Graphic3d_ArrayOfTriangles;
class V3d_View;

//! Interactive object for displaying the view manipulation cube.
//!
//! View cube consists of several parts that are responsible for different camera manipulations:
//! @li Cube sides represent main views: top, bottom, left, right, front and back.
//! @li Edges represent rotation of one of main views on 45 degrees.
//! @li Vertices represent rotation of one of man views in two directions.
//!
//! The object is expected to behave like a trihedron in the view corner,
//! therefore its position should be defined using transformation persistence flags:
//! @code SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_TriedronPers, Aspect_TOTP_LEFT_LOWER, Graphic3d_Vec2i (100, 100)); @endcode
//!
//! View Cube parts are sensitive to detection, or dynamic highlighting (but not selection),
//! and every its owner AIS_ViewCubeOwner corresponds to camera transformation.
//! @code
//!   for (aViewCube->StartAnimation (aDetectedOwner); aViewCube->HasAnimation(); )
//!   {
//!     aViewCube->UpdateAnimation();
//!     ... // updating of application window
//!   }
//! @endcode
//! or
//! @code aViewCube->HandleClick (aDetectedOwner); @endcode
//! that includes transformation loop.
//! This loop allows external actions like application updating. For this purpose AIS_ViewCube has virtual interface onAfterAnimation(),
//! that is to be redefined on application level.
class AIS_ViewCube : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_ViewCube, AIS_InteractiveObject)
public:

  //! Return TRUE if specified orientation belongs to box side.
  Standard_EXPORT static bool IsBoxSide (V3d_TypeOfOrientation theOrient);

  //! Return TRUE if specified orientation belongs to box edge.
  Standard_EXPORT static bool IsBoxEdge (V3d_TypeOfOrientation theOrient);

  //! Return TRUE if specified orientation belongs to box corner (vertex).
  Standard_EXPORT static bool IsBoxCorner (V3d_TypeOfOrientation theOrient);

public:

  //! Empty constructor.
  Standard_EXPORT AIS_ViewCube();

  //! Return view animation.
  const Handle(AIS_AnimationCamera)& ViewAnimation() const { return myViewAnimation; }

  //! Set view animation.
  void SetViewAnimation (const Handle(AIS_AnimationCamera)& theAnimation) { myViewAnimation = theAnimation; }

  //! Return TRUE if automatic camera transformation on selection (highlighting) is enabled; TRUE by default.
  Standard_Boolean ToAutoStartAnimation() const { return myToAutoStartAnim; }

  //! Enable/disable automatic camera transformation on selection (highlighting).
  //! The automatic logic can be disabled if application wants performing action manually
  //! basing on picking results (AIS_ViewCubeOwner).
  void SetAutoStartAnimation (bool theToEnable) { myToAutoStartAnim = theToEnable; }

  //! Return TRUE if camera animation should be done in uninterruptible loop; TRUE by default.
  Standard_Boolean IsFixedAnimationLoop() const { return myIsFixedAnimation; }

  //! Set if camera animation should be done in uninterruptible loop.
  void SetFixedAnimationLoop (bool theToEnable) { myIsFixedAnimation = theToEnable; }

  //! Reset all size and style parameters to default.
  //! @warning It doesn't reset position of View Cube
  Standard_EXPORT void ResetStyles();

protected:

  //! Set default visual attributes
  Standard_EXPORT void setDefaultAttributes();

  //! Set default dynamic highlight properties
  Standard_EXPORT void setDefaultHighlightAttributes();

public: //! @name Geometry management API

  //! @return size (width and height) of View cube sides; 100 by default.
  Standard_Real Size() const { return mySize; }

  //! Sets size (width and height) of View cube sides.
  //! @param theToAdaptAnother if TRUE, then other parameters will be adapted to specified size
  Standard_EXPORT void SetSize (Standard_Real theValue,
                                Standard_Boolean theToAdaptAnother = true);

  //! Return box facet extension to edge/corner facet split; 10 by default.
  Standard_Real BoxFacetExtension() const { return myBoxFacetExtension; }

  //! Set new value of box facet extension.
  void SetBoxFacetExtension (Standard_Real theValue)
  {
    if (Abs (myBoxFacetExtension - theValue) > Precision::Confusion())
    {
      myBoxFacetExtension = theValue;
      SetToUpdate();
    }
  }

  //! Return padding between axes and 3D part (box); 10 by default.
  Standard_Real AxesPadding() const { return myAxesPadding; }

  //! Set new value of padding between axes and 3D part (box).
  void SetAxesPadding (Standard_Real theValue)
  {
    if (Abs (myAxesPadding - theValue) > Precision::Confusion())
    {
      myAxesPadding = theValue;
      SetToUpdate();
    }
  }

  //! Return gap between box edges and box sides; 0 by default.
  Standard_Real BoxEdgeGap() const { return myBoxEdgeGap; }

  //! Set new value of box edges gap.
  void SetBoxEdgeGap (Standard_Real theValue)
  {
    if (Abs (myBoxEdgeGap - theValue) > Precision::Confusion())
    {
      myBoxEdgeGap = theValue;
      SetToUpdate();
    }
  }

  //! Return minimal size of box edge; 2 by default.
  Standard_Real BoxEdgeMinSize() const { return myBoxEdgeMinSize; }

  //! Set new value of box edge minimal size.
  void SetBoxEdgeMinSize (Standard_Real theValue)
  {
    if (Abs (myBoxEdgeMinSize - theValue) > Precision::Confusion())
    {
      myBoxEdgeMinSize = theValue;
      SetToUpdate();
    }
  }

  //! Return minimal size of box corner; 2 by default.
  Standard_Real BoxCornerMinSize() const { return myCornerMinSize; }

  //! Set new value of box corner minimal size.
  void SetBoxCornerMinSize (Standard_Real theValue)
  {
    if (Abs (myCornerMinSize - theValue) > Precision::Confusion())
    {
      myCornerMinSize = theValue;
      SetToUpdate();
    }
  }

  //! Return relative radius of side corners (round rectangle); 0.0 by default.
  //! The value in within [0, 0.5] range meaning absolute radius = RoundRadius() / Size().
  Standard_Real RoundRadius() const { return myRoundRadius; }

  //! Set relative radius of View Cube sides corners (round rectangle).
  //! The value should be within [0, 0.5] range.
  Standard_EXPORT void SetRoundRadius (const Standard_Real theValue);

  //! Returns radius of axes of the trihedron; 1.0 by default.
  Standard_Real AxesRadius() const { return myAxesRadius; }

  //! Sets radius of axes of the trihedron.
  void SetAxesRadius (const Standard_Real theRadius)
  {
    if (Abs (myAxesRadius - theRadius) > Precision::Confusion())
    {
      myAxesRadius = theRadius;
      SetToUpdate();
    }
  }

  //! Returns radius of cone of axes of the trihedron; 3.0 by default.
  Standard_Real AxesConeRadius() const { return myAxesConeRadius; }

  //! Sets radius of cone of axes of the trihedron.
  void SetAxesConeRadius (Standard_Real theRadius)
  {
    if (Abs (myAxesConeRadius - theRadius) > Precision::Confusion())
    {
      myAxesConeRadius = theRadius;
      SetToUpdate();
    }
  }

  //! Returns radius of sphere (central point) of the trihedron; 4.0 by default.
  Standard_Real AxesSphereRadius() const { return myAxesSphereRadius; }

  //! Sets radius of sphere (central point) of the trihedron.
  void SetAxesSphereRadius (Standard_Real theRadius)
  {
    if (Abs (myAxesSphereRadius - theRadius) > Precision::Confusion())
    {
      myAxesSphereRadius = theRadius;
      SetToUpdate();
    }
  }

  //! @return TRUE if trihedron is drawn; TRUE by default.
  Standard_Boolean ToDrawAxes() const { return myToDisplayAxes; }

  //! Enable/disable drawing of trihedron.
  void SetDrawAxes (Standard_Boolean theValue)
  {
    if (myToDisplayAxes != theValue)
    {
      myToDisplayAxes = theValue;
      SetToUpdate();
    }
  }

  //! @return TRUE if edges of View Cube is drawn; TRUE by default.
  Standard_Boolean ToDrawEdges() const { return myToDisplayEdges; }

  //! Enable/disable drawing of edges of View Cube.
  void SetDrawEdges (Standard_Boolean theValue)
  {
    if (myToDisplayEdges != theValue)
    {
      myToDisplayEdges = theValue;
      SetToUpdate();
    }
  }

  //! Return TRUE if vertices (vertex) of View Cube is drawn; TRUE by default.
  Standard_Boolean ToDrawVertices() const { return myToDisplayVertices; }

  //! Enable/disable drawing of vertices (corners) of View Cube.
  void SetDrawVertices (Standard_Boolean theValue)
  {
    if (myToDisplayVertices != theValue)
    {
      myToDisplayVertices = theValue;
      SetToUpdate();
    }
  }

  //! Return TRUE if application expects Y-up viewer orientation instead of Z-up; FALSE by default.
  Standard_Boolean IsYup() const { return myIsYup; }

  //! Set if application expects Y-up viewer orientation instead of Z-up.
  Standard_EXPORT void SetYup (Standard_Boolean theIsYup,
                               Standard_Boolean theToUpdateLabels = Standard_True);

public: //! @name Style management API

  //! Return shading style of box sides.
  const Handle(Prs3d_ShadingAspect)& BoxSideStyle() const { return myDrawer->ShadingAspect(); }

  //! Return shading style of box edges.
  const Handle(Prs3d_ShadingAspect)& BoxEdgeStyle() const { return myBoxEdgeAspect; }

  //! Return shading style of box corners.
  const Handle(Prs3d_ShadingAspect)& BoxCornerStyle() const { return myBoxCornerAspect; }

  //! Return value of front color for the 3D part of object.
  const Quantity_Color& BoxColor() const { return myDrawer->ShadingAspect()->Color(); }

  //! Set new value of front color for the 3D part of object.
  //! @param theColor [in] input color value.
  void SetBoxColor (const Quantity_Color& theColor)
  {
    if (!myDrawer->ShadingAspect()->Color().IsEqual (theColor)
     || !myBoxEdgeAspect  ->Color().IsEqual (theColor)
     || !myBoxCornerAspect->Color().IsEqual (theColor))
    {
      myDrawer->ShadingAspect()->SetColor (theColor);
      myBoxEdgeAspect->SetColor (theColor);
      myBoxCornerAspect->SetColor (theColor);
      SynchronizeAspects();
    }
  }

  //! Return transparency for 3D part of object.
  Standard_Real BoxTransparency() const { return myDrawer->ShadingAspect()->Transparency(); }

  //! Set new value of transparency for 3D part of object.
  //! @param theValue [in] input transparency value
  void SetBoxTransparency (Standard_Real theValue)
  {
    if (Abs (myDrawer->ShadingAspect()->Transparency() - theValue) > Precision::Confusion()
     || Abs (myBoxEdgeAspect  ->Transparency() - theValue) > Precision::Confusion()
     || Abs (myBoxCornerAspect->Transparency() - theValue) > Precision::Confusion())
    {
      myDrawer->ShadingAspect()->SetTransparency (theValue);
      myBoxEdgeAspect->SetTransparency (theValue);
      myBoxCornerAspect->SetTransparency (theValue);
      SynchronizeAspects();
    }
  }

  //! Return color of sides back material.
  const Quantity_Color& InnerColor() const { return myDrawer->ShadingAspect()->Color (Aspect_TOFM_BACK_SIDE); }

  //! Set color of sides back material. Alias for:
  //! @code Attributes()->ShadingAspect()->Aspect()->ChangeBackMaterial().SetColor() @endcode
  void SetInnerColor (const Quantity_Color& theColor)
  {
    myDrawer->ShadingAspect()->SetColor (theColor, Aspect_TOFM_BACK_SIDE);
    SynchronizeAspects();
  }

  //! Return box side label or empty string if undefined.
  //! Default labels: FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM.
  TCollection_AsciiString BoxSideLabel (V3d_TypeOfOrientation theSide) const
  {
    const TCollection_AsciiString* aLabel = myBoxSideLabels.Seek (theSide);
    return aLabel != NULL ? *aLabel : TCollection_AsciiString();
  }

  //! Set box side label.
  void SetBoxSideLabel (const V3d_TypeOfOrientation theSide,
                        const TCollection_AsciiString& theLabel)
  {
    if (!IsBoxSide (theSide))
    {
      throw Standard_ProgramError ("AIS_ViewCube::SetBoxSideLabel(), invalid enumeration value");
    }
    myBoxSideLabels.Bind (theSide, theLabel);
    SetToUpdate();
  }

  //! Return text color of labels of box sides; BLACK by default.
  const Quantity_Color& TextColor() const { return myDrawer->TextAspect()->Aspect()->Color(); }

  //! Set color of text labels on box sides. Alias for:
  //! @code Attributes()->TextAspect()->SetColor() @endcode
  void SetTextColor (const Quantity_Color& theColor)
  {
    myDrawer->TextAspect()->SetColor (theColor);
    SynchronizeAspects();
  }

  //! Return font name that is used for displaying of sides and axes text. Alias for:
  //! @code Attributes()->TextAspect()->Aspect()->SetFont() @endcode
  const TCollection_AsciiString& Font() const { return myDrawer->TextAspect()->Aspect()->Font(); }

  //! Set font name that is used for displaying of sides and axes text. Alias for:
  //! @code Attributes()->TextAspect()->SetFont() @endcode
  void SetFont (const TCollection_AsciiString& theFont)
  {
    myDrawer->TextAspect()->Aspect()->SetFont (theFont);
    SynchronizeAspects();
  }

  //! Return height of font
  Standard_Real FontHeight() const { return myDrawer->TextAspect()->Height(); }

  //! Change font height. Alias for:
  //! @code Attributes()->TextAspect()->SetHeight() @endcode
  void SetFontHeight (Standard_Real theValue)
  {
    if (Abs (myDrawer->TextAspect()->Height() - theValue) > Precision::Confusion())
    {
      myDrawer->TextAspect()->SetHeight (theValue);
      SetToUpdate();
    }
  }

  //! Return axes labels or empty string if undefined.
  //! Default labels: X, Y, Z.
  TCollection_AsciiString AxisLabel (Prs3d_DatumParts theAxis) const
  {
    const TCollection_AsciiString* aLabel = myAxesLabels.Seek (theAxis);
    return aLabel != NULL ? *aLabel : TCollection_AsciiString();
  }

  //! Set axes labels.
  void SetAxesLabels (const TCollection_AsciiString& theX,
                      const TCollection_AsciiString& theY,
                      const TCollection_AsciiString& theZ)
  {
    myAxesLabels.Bind (Prs3d_DatumParts_XAxis, theX);
    myAxesLabels.Bind (Prs3d_DatumParts_YAxis, theY);
    myAxesLabels.Bind (Prs3d_DatumParts_ZAxis, theZ);
    SetToUpdate();
  }

public:

  //! Set new value of color for the whole object.
  //! @param theColor [in] input color value.
  virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE
  {
    SetBoxColor (theColor);
  }

  //! Reset color for the whole object.
  virtual void UnsetColor() Standard_OVERRIDE
  {
    myDrawer->ShadingAspect()->SetColor (Quantity_NOC_WHITE);
    myBoxEdgeAspect  ->SetColor (Quantity_NOC_GRAY30);
    myBoxCornerAspect->SetColor (Quantity_NOC_GRAY30);
    SynchronizeAspects();
  }

  //! Set new value of transparency for the whole object.
  //! @param theValue [in] input transparency value.
  virtual void SetTransparency (const Standard_Real theValue) Standard_OVERRIDE
  {
    SetBoxTransparency (theValue);
  }

  //! Reset transparency for the whole object.
  virtual void UnsetTransparency() Standard_OVERRIDE
  {
    SetBoxTransparency (0.0f);
  }

  //! Sets the material for the interactive object.
  virtual void SetMaterial (const Graphic3d_MaterialAspect& theMat) Standard_OVERRIDE
  {
    myDrawer->ShadingAspect()->SetMaterial (theMat);
    myBoxEdgeAspect  ->SetMaterial (theMat);
    myBoxCornerAspect->SetMaterial (theMat);
    SynchronizeAspects();
  }

  //! Sets the material for the interactive object.
  virtual void UnsetMaterial() Standard_OVERRIDE
  {
    Graphic3d_MaterialAspect aMat (Graphic3d_NameOfMaterial_UserDefined);
    aMat.SetColor (Quantity_NOC_WHITE);
    aMat.SetAmbientColor (Quantity_NOC_GRAY60);
    myDrawer->ShadingAspect()->SetMaterial (aMat);
    myBoxEdgeAspect  ->SetMaterial (aMat);
    myBoxEdgeAspect  ->SetColor (Quantity_NOC_GRAY30);
    myBoxCornerAspect->SetMaterial (aMat);
    myBoxCornerAspect->SetColor (Quantity_NOC_GRAY30);
    SynchronizeAspects();
  }

public: //! @name animation methods

  //! Return duration of animation in seconds; 0.5 sec by default
  Standard_EXPORT Standard_Real Duration() const;

  //! Set duration of animation.
  //! @param theValue [in] input value of duration in seconds
  Standard_EXPORT void SetDuration (Standard_Real theValue);

  //! Return TRUE if new camera Up direction should be always set to default value for a new camera Direction; FALSE by default.
  //! When this flag is FALSE, the new camera Up will be set as current Up orthogonalized to the new camera Direction,
  //! and will set to default Up on second click.
  Standard_Boolean ToResetCameraUp() const { return myToResetCameraUp; }

  //! Set if new camera Up direction should be always set to default value for a new camera Direction.
  void SetResetCamera (Standard_Boolean theToReset) { myToResetCameraUp = theToReset; }

  //! Return TRUE if animation should fit selected objects and FALSE to fit entire scene; TRUE by default.
  Standard_Boolean ToFitSelected() const { return myToFitSelected; }

  //! Set if animation should fit selected objects or to fit entire scene.
  void SetFitSelected (Standard_Boolean theToFitSelected) { myToFitSelected = theToFitSelected; }

  //! @return TRUE if View Cube has unfinished animation of view camera.
  Standard_EXPORT Standard_Boolean HasAnimation() const;

  //! Start camera transformation corresponding to the input detected owner.
  //! @param theOwner [in] detected owner.
  Standard_EXPORT virtual void StartAnimation (const Handle(AIS_ViewCubeOwner)& theOwner);

  //! Perform one step of current camera transformation.
  //! theToUpdate [in] enable/disable update of view.
  //! @return TRUE if animation is not stopped.
  Standard_EXPORT virtual Standard_Boolean UpdateAnimation (const Standard_Boolean theToUpdate);

  //! Perform camera transformation corresponding to the input detected owner.
  Standard_EXPORT virtual void HandleClick (const Handle(AIS_ViewCubeOwner)& theOwner);

protected:

  //! Perform internal single step of animation.
  //! @return FALSE if animation has been finished
  Standard_EXPORT Standard_Boolean updateAnimation();

  //! Fit selected/all into view.
  //! @param theView [in] view definition to retrieve scene bounding box
  //! @param theCamera [in,out] camera definition
  Standard_EXPORT virtual void viewFitAll (const Handle(V3d_View)& theView,
                                           const Handle(Graphic3d_Camera)& theCamera);

protected: //! @name protected virtual API

  //! Method that is called after one step of transformation.
  virtual void onAfterAnimation() {}

  //! Method that is called after transformation finish.
  virtual void onAnimationFinished() {}

public: //! @name Presentation computation

  //! Return TRUE for supported display mode.
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Global selection has no meaning for this class.
  virtual Handle(SelectMgr_EntityOwner) GlobalSelOwner() const Standard_OVERRIDE { return Handle(SelectMgr_EntityOwner)(); }

  //! Compute 3D part of View Cube.
  //! @param thePrsMgr [in] presentation manager.
  //! @param thePrs [in] input presentation that is to be filled with flat presentation primitives.
  //! @param theMode [in] display mode.
  //! @warning this object accept only 0 display mode.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode = 0) Standard_OVERRIDE;

  //! Redefine computing of sensitive entities for View Cube.
  //! @param theSelection [in] input selection object that is to be filled with sensitive entities.
  //! @param theMode [in] selection mode.
  //! @warning object accepts only 0 selection mode.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  //! Disables auto highlighting to use HilightSelected() and HilightOwnerWithColor() overridden methods.
  virtual Standard_Boolean IsAutoHilight() const Standard_OVERRIDE { return Standard_False; }

  //! Method which clear all selected owners belonging to this selectable object.
  //! @warning this object does not support selection.
  virtual void ClearSelected() Standard_OVERRIDE {}

  //! Method which highlights input owner belonging to this selectable object.
  //! @param thePM [in] presentation manager
  //! @param theStyle [in] style for dynamic highlighting.
  //! @param theOwner [in] input entity owner.
  Standard_EXPORT virtual void HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                                      const Handle(Prs3d_Drawer)& theStyle,
                                                      const Handle(SelectMgr_EntityOwner)& theOwner) Standard_OVERRIDE;

  //! Method which draws selected owners.
  Standard_EXPORT virtual void HilightSelected (const Handle(PrsMgr_PresentationManager)& thePM,
                                                const SelectMgr_SequenceOfOwner& theSeq) Standard_OVERRIDE;

  //! Set default parameters for visual attributes
  //! @sa Attributes()
  virtual void UnsetAttributes() Standard_OVERRIDE
  {
    setDefaultAttributes();
    SetToUpdate();
  }

  //! Set default parameters for dynamic highlighting attributes, reset highlight attributes
  virtual void UnsetHilightAttributes() Standard_OVERRIDE
  {
    myHilightDrawer.Nullify();
    setDefaultHighlightAttributes();
    SetToUpdate();
  }

protected: //! @name Auxiliary classes to fill presentation with proper primitives

  //! Create triangulation for a box part - for presentation and selection purposes.
  //! @param theTris    [in,out] triangulation to fill, or NULL to return size
  //! @param theNbNodes [in,out] should be incremented by a number of nodes defining this triangulation
  //! @param theNbTris  [in,out] should be incremented by a number of triangles defining this triangulation
  //! @param theDir     [in] part to define
  Standard_EXPORT virtual void createBoxPartTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                                       Standard_Integer& theNbNodes,
                                                       Standard_Integer& theNbTris,
                                                       V3d_TypeOfOrientation theDir) const;

  //! Create triangulation for a box side.
  //! @param theTris    [in,out] triangulation to fill, or NULL to return size
  //! @param theNbNodes [in,out] should be incremented by a number of nodes defining this triangulation
  //! @param theNbTris  [in,out] should be incremented by a number of triangles defining this triangulation
  //! @param theDir     [in] part to define
  Standard_EXPORT virtual void createBoxSideTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                                       Standard_Integer& theNbNodes,
                                                       Standard_Integer& theNbTris,
                                                       V3d_TypeOfOrientation theDir) const;

  //! Create triangulation for a box edge.
  //! @param theTris    [in,out] triangulation to fill, or NULL to return size
  //! @param theNbNodes [in,out] should be incremented by a number of nodes defining this triangulation
  //! @param theNbTris  [in,out] should be incremented by a number of triangles defining this triangulation
  //! @param theDir     [in] part to define
  Standard_EXPORT virtual void createBoxEdgeTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                                       Standard_Integer& theNbNodes,
                                                       Standard_Integer& theNbTris,
                                                       V3d_TypeOfOrientation theDir) const;

  //! Create triangulation for a box corner (vertex).
  //! @param theTris    [in,out] triangulation to fill, or NULL to return size
  //! @param theNbNodes [in,out] should be incremented by a number of nodes defining this triangulation
  //! @param theNbTris  [in,out] should be incremented by a number of triangles defining this triangulation
  //! @param theDir     [in] part to define
  Standard_EXPORT virtual void createBoxCornerTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                                         Standard_Integer& theNbNodes,
                                                         Standard_Integer& theNbTris,
                                                         V3d_TypeOfOrientation theDir) const;

protected:

  //! Create triangulation for a rectangle with round corners.
  //! @param theTris    [in,out] triangulation to fill, or NULL to return size
  //! @param theNbNodes [in,out] should be incremented by a number of nodes defining this triangulation
  //! @param theNbTris  [in,out] should be incremented by a number of triangles defining this triangulation
  //! @param theSize    [in] rectangle dimensions
  //! @param theRadius  [in] radius at corners
  //! @param theTrsf    [in] transformation
  Standard_EXPORT static void createRoundRectangleTriangles (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                                             Standard_Integer& theNbNodes,
                                                             Standard_Integer& theNbTris,
                                                             const gp_XY& theSize,
                                                             Standard_Real theRadius,
                                                             const gp_Trsf& theTrsf);

protected:

  NCollection_DataMap<V3d_TypeOfOrientation, TCollection_AsciiString>
                                myBoxSideLabels;     //!< map with box side labels
  NCollection_DataMap<Prs3d_DatumParts, TCollection_AsciiString>
                                myAxesLabels;        //!< map with axes labels
  Handle(Prs3d_ShadingAspect)   myBoxEdgeAspect;     //!< style for box edges
  Handle(Prs3d_ShadingAspect)   myBoxCornerAspect;   //!< style for box corner

  Standard_Real                 mySize;              //!< size of box side, length of one axis
  Standard_Real                 myBoxEdgeMinSize;    //!< minimal size of box edge
  Standard_Real                 myBoxEdgeGap;        //!< gap between box side and box edge
  Standard_Real                 myBoxFacetExtension; //!< box facet extension
  Standard_Real                 myAxesPadding;       //!< Padding between box and axes
  Standard_Real                 myAxesRadius;        //!< radius of axes of the trihedron; 1.0 by default
  Standard_Real                 myAxesConeRadius;    //!< radius of cone of axes of the trihedron; 3.0 by default
  Standard_Real                 myAxesSphereRadius;  //!< radius of sphere (central point) of the trihedron; 4.0 by default
  Standard_Real                 myCornerMinSize;     //!< minimal size of box corner
  Standard_Real                 myRoundRadius;       //!< relative round radius within [0; 0.5] range
  Standard_Boolean              myToDisplayAxes;     //!< trihedron visibility
  Standard_Boolean              myToDisplayEdges;    //!< box edges visibility
  Standard_Boolean              myToDisplayVertices; //!< box corners (vertices) visibility
  Standard_Boolean              myIsYup;             //!< flag indicating that application expects Y-up viewer orientation instead of Z-up

protected: //! @name Animation options

  Handle(AIS_AnimationCamera)   myViewAnimation;     //!< Camera animation object
  Handle(Graphic3d_Camera)      myStartState;        //!< Start state of view camera
  Handle(Graphic3d_Camera)      myEndState;          //!< End state of view camera
  Standard_Boolean              myToAutoStartAnim;   //!< start animation automatically on click
  Standard_Boolean              myIsFixedAnimation;  //!< fixed-loop animation
  Standard_Boolean              myToFitSelected;     //!< fit selected or fit entire scene
  Standard_Boolean              myToResetCameraUp;   //!< always reset camera up direction to default

};

//! Redefined entity owner that is highlighted when owner is detected,
//! even if Interactive Context highlighted on last detection procedure.
class AIS_ViewCubeOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTIEXT(AIS_ViewCubeOwner, SelectMgr_EntityOwner)
public:

  //! Main constructor.
  AIS_ViewCubeOwner (const Handle(AIS_ViewCube)& theObject,
                     V3d_TypeOfOrientation theOrient,
                     Standard_Integer thePriority = 5)
  : SelectMgr_EntityOwner ((const Handle(SelectMgr_SelectableObject)& )theObject, thePriority),
    myMainOrient (theOrient)
  {
    myFromDecomposition = true;
  }

  //! @return TRUE. This owner will always call method
  //! Hilight for its Selectable Object when the owner is detected.
  virtual Standard_Boolean IsForcedHilight() const Standard_OVERRIDE { return Standard_True; }

  //! Return new orientation to set.
  V3d_TypeOfOrientation MainOrientation() const { return myMainOrient; }

  //! Handle mouse button click event.
  virtual Standard_Boolean HandleMouseClick (const Graphic3d_Vec2i& thePoint,
                                             Aspect_VKeyMouse theButton,
                                             Aspect_VKeyFlags theModifiers,
                                             bool theIsDoubleClick) Standard_OVERRIDE
  {
    (void )thePoint; (void )theButton; (void )theModifiers; (void )theIsDoubleClick;
    AIS_ViewCube* aCubePrs = dynamic_cast<AIS_ViewCube* >(mySelectable);
    aCubePrs->HandleClick (this);
    return Standard_True;
  }

protected:

  V3d_TypeOfOrientation myMainOrient; //!< new orientation to set

};

//! Simple sensitive element for picking by point only.
class AIS_ViewCubeSensitive : public Select3D_SensitivePrimitiveArray
{
  DEFINE_STANDARD_RTTIEXT(AIS_ViewCubeSensitive, Select3D_SensitivePrimitiveArray)
public:

  //! Constructor.
  Standard_EXPORT AIS_ViewCubeSensitive (const Handle(SelectMgr_EntityOwner)& theOwner,
                                         const Handle(Graphic3d_ArrayOfTriangles)& theTris);

  //! Checks whether element overlaps current selecting volume.
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

protected:

  //! Checks if picking ray can be used for detection.
  Standard_EXPORT bool isValidRay (const SelectBasics_SelectingVolumeManager& theMgr) const;

};

#endif // _AIS_ViewCube_HeaderFile
