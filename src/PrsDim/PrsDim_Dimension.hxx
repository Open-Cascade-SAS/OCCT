// Created on: 2013-11-11
// Created by: Anastasia BORISOVA
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _PrsDim_Dimension_HeaderFile
#define _PrsDim_Dimension_HeaderFile

#include <PrsDim_DimensionOwner.hxx>
#include <PrsDim_DisplaySpecialSymbol.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <PrsDim_KindOfDimension.hxx>
#include <Geom_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Handle.hxx>

DEFINE_STANDARD_HANDLE(PrsDim_Dimension, AIS_InteractiveObject)

//! PrsDim_Dimension is a base class for 2D presentations of linear (length, diameter, radius)
//! and angular dimensions.
//!
//! The dimensions provide measurement of quantities, such as lengths or plane angles.
//! The measurement of dimension "value" is done in model space "as is".
//! These "value" are said to be represented in "model units", which can be specified by user.
//! During the display the measured value converted from "model units" to "display units".
//! The display and model units are stored in common Prs3d_Drawer (drawer of the context)
//! to share it between all dimensions.
//! The specified by user units are stored in the dimension's drawer.
//!
//! As a drawing, the dimension is composed from the following components:
//! - Attachment (binding) points. The points where the dimension lines attaches to, for
//!   length dimensions the distances are measured between these points.
//! - Main dimension line. The which extends from the attachment points in "up" direction,
//!   and which contains text label on it with value string.
//! - Flyouts. The lines connecting the attachment points with main dimension line.
//! - Extension. The lines used to extend the main dimension line in the cases when text
//!   or arrows do not fit into the main dimension line due to their size.
//! - Arrows.
//!
//! <pre>
//!  Linear dimensions:
//!
//!  extension
//!   line                                     arrow
//!       -->|------- main dimension line -------|<--
//!          |                                   |
//!          |flyout                       flyout|
//!          |                                   |
//!          +-----------------------------------+
//! attachment                                attachment
//!  point                                       point
//!
//!  Angular dimensions:
//!
//!                  extension
//!                     line
//!                        -->|+++++
//!                     arrow |     +++
//!                           |        90(deg) - main dimension line
//!                    flyout |         +++
//!                           |           +
//!                           o---flyout---
//!                         center         ^ 
//!                         point          | extension
//!                                          line
//! </pre>
//!
//! Being a 2D drawings, the dimensions are created on imaginary plane, called "dimension plane",
//! which can be thought of as reference system of axes (X,Y,N) for constructing the presentation.
//!
//! The role of axes of the dimension plane is to guide you through the encapsualted automations
//! of presentation building to help you understand how is the presentation will look and how it
//! will be oriented in model space during construction.
//! 
//! Orientation of dimension line in model space relatively to the base shapes is defined 
//! with the flyouts. Flyouts specify length of flyout lines and their orientation relatively
//! to the attachment points on the working plane.
//! For linear dimensions: 
//!   Direction of flyouts is specified with direction of main dimension line
//!   (vector from the first attachment to the second attachment) and the normal of the dimension plane.
//!   Positive direction of flyouts is defined by vector multiplication: AttachVector * PlaneNormal.
//! For angular dimensions:
//!   Flyouts are defined by vectors from the center point to the attachment points.
//!   These vectors directions are supposed to be the positive directions of flyouts.
//!   Negative flyouts directions means that these vectors should be reversed
//!   (and dimension will be built out of the angle constructed with center and two attach points).
//!
//! The dimension plane can be constructed automatically by application (where possible,
//! it depends on the measured geometry).
//! It can be also set by user. However, if the user-defined plane does not fit the
//! geometry of the dimension (attach points do not belong to it), the dimension could not
//! be built.
//! If it is not possible to compute automatic plane (for example, in case of length between 
//! two points) the user is supposed to specify the custom plane.
//!
//! Since the dimensions feature automated construction procedures from an arbitrary shapes,
//! the interfaces to check the validness are also implemented. Once the measured geometry is
//! specified, the one can inquire the validness status by calling "IsValid()" method. If the result
//! is TRUE, then all of public parameters should be pre-computed and ready. The presentation
//! should be also computable. Otherwise, the parameters may return invalid values. In this case,
//! the presentation will not be computed and displayed.
//! 
//! The dimension support two local selection modes: main dimension line selection and text label
//! selection. These modes can be used to develop interactive modification of dimension presentations.
//! The component highlighting in these selection modes is provided by PrsDim_DimensionOwner class.
//! Please note that selection is unavailable until the presentation is computed.
//! 
//! The specific drawing attributes are controlled through Prs3d_DimensionAspect. The one can change
//! color, arrows, text and arrow style and specify positioning of value label by setting corresponding
//! values to the aspect.
//!
//! Such set of parameters that consists of:
//! - flyout size and direction,
//! - user-defined  dimension plane,
//! - horizontal and vertical text alignment
//! can be uniquely replaced with text position in 3d space. Therefore, there are methods to convert
//! this set of parameters to the text position and vice versa:
//!
//! - If the fixed text position is defined by user, called SetTextPosition (theTextPos) method converts
//! this 3d point to the set of parameters including adjusting of the dimension plane (this plane will be
//! automatic plane, NOT user-defined one).
//! If the fixed text position is set, the flag myIsFixedTextPosition is set to TRUE.
//! ATTENTION! myIsFixedTextPosition fixes all parameters of the set from recomputing inside
//! SetMeasureGeometry() methods. Parameters in dimension aspect (they are horizontal text position
//! and extension size) are adjusted on presentation computing step, user-defined values in
//! dimension aspect are not changed.
//! But plane and flyout as dimension position parameters are changed by SetTextPosition() method
//! according with user-defined text position.
//! If parameters from the set are changed by user with calls of setters, it leads to disabling of
//! fixed text position (myIsFixedTextPosition is set to FALSE).
//! If the fixed text position is set and geometry is changed by user (SetMeasureGeometry() method
//! is called) and the geometry doesn't satisfy computed dimension plane, the dimension is not valid.
//!
//! - If the set of parameters was set by user (may be without the user-defined plane or with it),
//! it can be converted to the text position by calling the method GetTextPosition(). In this case
//! the text position is NOT fixed, and SetMeasureGeometry() without user-defined plane adjusts
//! the automatic plane according input geometry (if it is possible).
class PrsDim_Dimension : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_Dimension, AIS_InteractiveObject)
protected:

  //! Geometry type defines type of shapes on which the dimension is to be built.
  //! Some type of geometry allows automatic plane computing and
  //! can be built without user-defined plane
  //! Another types can't be built without user-defined plane.
  enum GeometryType
  {
    GeometryType_UndefShapes,
    GeometryType_Edge,
    GeometryType_Face,
    GeometryType_Points,
    GeometryType_Edges,
    GeometryType_Faces,
    GeometryType_EdgeFace,
    GeometryType_EdgeVertex
  };

  //! Specifies supported at base level horizontal and vertical
  //! label positions for drawing extension lines and centered text.
  enum LabelPosition
  {
    LabelPosition_None    = 0x00,

    LabelPosition_Left    = 0x01,
    LabelPosition_Right   = 0x02,
    LabelPosition_HCenter = 0x04,
    LabelPosition_HMask   = LabelPosition_Left | LabelPosition_Right | LabelPosition_HCenter,

    LabelPosition_Above   = 0x10,
    LabelPosition_Below   = 0x20,
    LabelPosition_VCenter = 0x40,
    LabelPosition_VMask   = LabelPosition_Above | LabelPosition_Below | LabelPosition_VCenter
  };

  enum ValueType
  {
    ValueType_Computed,
    ValueType_CustomReal,
    ValueType_CustomText
  };

public:

  //! Specifies supported presentation compute modes.
  //! Used to compute only parts of presentation for
  //! advanced highlighting.
  enum ComputeMode
  {
    ComputeMode_All  = 0, //!< "0" is reserved as neutral mode
    ComputeMode_Line = 1, //!< corresponds to selection mode
    ComputeMode_Text = 2  //!< corresponds to selection mode
  };

public:

  //! Constructor with default parameters values.
  //! @param theType [in] the type of dimension.
  Standard_EXPORT PrsDim_Dimension (const PrsDim_KindOfDimension theType);

  //! Gets dimension measurement value. If the value to display is not
  //! specified by user, then the dimension object is responsible to
  //! compute it on its own in model space coordinates.
  //! @return the dimension value (in model units) which is used
  //! during display of the presentation.
  Standard_Real GetValue() const
  {
    return myValueType == ValueType_CustomReal ? myCustomValue : ComputeValue();
  }

  //! Sets computed dimension value. Resets custom value mode if it was set.
  void SetComputedValue ()
  {
    myValueType = ValueType_Computed;
  }

  //! Sets user-defined dimension value.
  //! The user-defined dimension value is specified in model space,
  //! and affect by unit conversion during the display.
  //! @param theValue [in] the user-defined value to display.
  Standard_EXPORT void SetCustomValue (const Standard_Real theValue);

  //! Sets user-defined dimension value.
  //! Unit conversion during the display is not applied.
  //! @param theValue [in] the user-defined value to display.
  Standard_EXPORT void SetCustomValue (const TCollection_ExtendedString& theValue);

  //! Gets user-defined dimension value.
  //! @return dimension value string.
  const TCollection_ExtendedString& GetCustomValue() const { return myCustomStringValue; }

  //! Get the dimension plane in which the 2D dimension presentation is computed.
  //! By default, if plane is not defined by user, it is computed automatically
  //! after dimension geometry is computed.
  //! If computed dimension geometry (points) can't be placed on the user-defined
  //! plane, dimension geometry was set as invalid (validity flag is set to false)
  //! and dimension presentation will not be computed.
  //! If user-defined plane allow geometry placement on it, it will be used for
  //! computing of the dimension presentation.
  //! @return dimension plane used for presentation computing.
  const gp_Pln& GetPlane() const { return myPlane; }

  //! Geometry type defines type of shapes on which the dimension is to be built. 
  //! @return type of geometry on which the dimension will be built.
  Standard_Integer GetGeometryType () const { return myGeometryType; }

  //! Sets user-defined plane where the 2D dimension presentation will be placed.
  //! Checks validity of this plane if geometry has been set already.
  //! Validity of the plane is checked according to the geometry set
  //! and has different criteria for different kinds of dimensions.
  Standard_EXPORT virtual void SetCustomPlane (const gp_Pln& thePlane);

  //! Unsets user-defined plane. Therefore the plane for dimension will be
  //! computed automatically.
  void UnsetCustomPlane() { myIsPlaneCustom = Standard_False; }

  //! @return TRUE if text position is set by user with method SetTextPosition().
  Standard_Boolean IsTextPositionCustom() const
  {
    return myIsTextPositionFixed;
  }

  //! Fixes the absolute text position and adjusts flyout, plane and text alignment
  //! according to it. Updates presentation if the text position is valid.
  //! ATTENTION! It does not change vertical text alignment.
  //! @param theTextPos [in] the point of text position.
  virtual void SetTextPosition (const gp_Pnt& /*theTextPos*/) { }

  //! Computes absolute text position from dimension parameters
  //! (flyout, plane and text alignment).
  virtual gp_Pnt GetTextPosition () const { return gp_Pnt(); }

public:

  //! Gets the dimension aspect from AIS object drawer.
  //! Dimension aspect contains aspects of line, text and arrows for dimension presentation.
  Handle(Prs3d_DimensionAspect) DimensionAspect() const
  {
    return myDrawer->DimensionAspect();
  }

  //! Sets new dimension aspect for the interactive object drawer.
  //! The dimension aspect provides dynamic properties which are generally
  //! used during computation of dimension presentations.
  Standard_EXPORT void SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theDimensionAspect);

  //! @return the kind of dimension.
  PrsDim_KindOfDimension KindOfDimension() const { return myKindOfDimension; }

  //! @return the kind of interactive.
  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE { return AIS_KindOfInteractive_Dimension; }

  //! Returns true if the class of objects accepts the display mode theMode.
  //! The interactive context can have a default mode of representation for
  //! the set of Interactive Objects. This mode may not be accepted by object.
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE
  {
    return theMode == ComputeMode_All;
  }

public:

  //! @return dimension special symbol display options.
  PrsDim_DisplaySpecialSymbol DisplaySpecialSymbol() const { return myDisplaySpecialSymbol; }

  //! Specifies whether to display special symbol or not.
  Standard_EXPORT void SetDisplaySpecialSymbol (const PrsDim_DisplaySpecialSymbol theDisplaySpecSymbol);

  //! @return special symbol.
  Standard_ExtCharacter SpecialSymbol() const
  {
    return mySpecialSymbol;
  }

  //! Specifies special symbol.
  Standard_EXPORT void SetSpecialSymbol (const Standard_ExtCharacter theSpecialSymbol);

  Standard_EXPORT virtual const TCollection_AsciiString& GetDisplayUnits() const;

  Standard_EXPORT virtual const TCollection_AsciiString& GetModelUnits() const;

  virtual void SetDisplayUnits (const TCollection_AsciiString& /*theUnits*/) { }

  virtual void SetModelUnits (const TCollection_AsciiString& /*theUnits*/) { }

  //! Unsets user defined text positioning and enables text positioning
  //!  by other parameters: text alignment, extension size, flyout and custom plane.
  Standard_EXPORT void UnsetFixedTextPosition();

public:

  //! Returns selection tolerance for text2d:
  //! For 2d text selection detection sensitive point with tolerance is used
  //! Important! Only for 2d text.
  Standard_Real SelToleranceForText2d() const
  {
    return mySelToleranceForText2d;
  }

  //! Sets selection tolerance for text2d:
  //! For 2d text selection detection sensitive point with tolerance is used
  //! to change this tolerance use this method
  //! Important! Only for 2d text.
  Standard_EXPORT void SetSelToleranceForText2d (const Standard_Real theTol);

  //! @return flyout value for dimension.
  Standard_Real GetFlyout() const
  {
    return myFlyout;
  }

  //! Sets flyout value for dimension.
  Standard_EXPORT void SetFlyout (const Standard_Real theFlyout);

  //! Check that the input geometry for dimension is valid and the
  //! presentation can be successfully computed.
  //! @return TRUE if dimension geometry is ok.
  virtual Standard_Boolean IsValid() const
  {
    return myIsGeometryValid && CheckPlane (GetPlane());
  }

protected:

  Standard_EXPORT Standard_Real ValueToDisplayUnits() const;

  //! Get formatted value string and its model space width.
  //! @param theWidth [out] the model space with of the string.
  //! @return formatted dimension value string.
  Standard_EXPORT TCollection_ExtendedString GetValueString (Standard_Real& theWidth) const;

  //! Performs drawing of 2d or 3d arrows on the working plane
  //! @param theLocation [in] the location of the arrow tip.
  //! @param theDirection [in] the direction from the tip to the bottom of the arrow.
  Standard_EXPORT void DrawArrow (const Handle(Prs3d_Presentation)& thePresentation,
                                  const gp_Pnt& theLocation,
                                  const gp_Dir& theDirection);

  //! Performs drawing of 2d or 3d text on the working plane
  //! @param theTextPos [in] the position of the text label.
  //! @param theTestDir [in] the direction of the text label.
  //! @param theText [in] the text label string.
  //! @param theLabelPosition [in] the text label vertical and horizontal positioning option
  //! respectively to the main dimension line. 
  //! @return text width relative to the dimension working plane. For 2d text this value will be zero.
  Standard_EXPORT void drawText (const Handle(Prs3d_Presentation)& thePresentation,
                                 const gp_Pnt& theTextPos,
                                 const gp_Dir& theTextDir,
                                 const TCollection_ExtendedString& theText,
                                 const Standard_Integer theLabelPosition);

  //! Performs computing of dimension linear extension with text
  //! @param thePresentation [in] the presentation to fill with graphical primitives.
  //! @param theExtensionSize [in] the size of extension line.
  //! @param theExtensionStart [in] the point where extension line connects to dimension.
  //! @param theExtensionDir [in] the direction of extension line.
  //! @param theLabelString [in] the string with value.
  //! @param theLabelWidth [in] the geometrical width computed for value string.
  //! @param theMode [in] the display mode.
  //! @param theLabelPosition [in] position flags for the text label.
  Standard_EXPORT void DrawExtension (const Handle(Prs3d_Presentation)& thePresentation,
                                      const Standard_Real theExtensionSize,
                                      const gp_Pnt& theExtensionStart,
                                      const gp_Dir& theExtensionDir,
                                      const TCollection_ExtendedString& theLabelString,
                                      const Standard_Real theLabelWidth,
                                      const Standard_Integer theMode,
                                      const Standard_Integer theLabelPosition);

  //! Performs computing of linear dimension (for length, diameter, radius and so on).
  //! Please note that this method uses base dimension properties, like working plane
  //! flyout length, drawer attributes.
  //! @param thePresentation [in] the presentation to fill with primitives.
  //! @param theMode [in] the presentation compute mode.
  //! @param theFirstPoint [in] the first attach point of linear dimension.
  //! @param theSecondPoint [in] the second attach point of linear dimension.
  //! @param theIsOneSide [in] specifies whether the dimension has only one flyout line.
  Standard_EXPORT void DrawLinearDimension (const Handle(Prs3d_Presentation)& thePresentation,
                                            const Standard_Integer theMode,
                                            const gp_Pnt& theFirstPoint,
                                            const gp_Pnt& theSecondPoint,
                                            const Standard_Boolean theIsOneSide = Standard_False);

  //! Computes points bounded the flyout line for linear dimension.
  //! @param theFirstPoint [in] the first attach point of linear dimension.
  //! @param theSecondPoint [in] the second attach point of linear dimension.
  //! @param theLineBegPoint [out] the first attach point of linear dimension.
  //! @param theLineEndPoint [out] the second attach point of linear dimension.
  Standard_EXPORT virtual void ComputeFlyoutLinePoints (const gp_Pnt& theFirstPoint, const gp_Pnt& theSecondPoint,
                                                        gp_Pnt& theLineBegPoint, gp_Pnt& theLineEndPoint);

  //! Compute selection sensitives for linear dimension flyout lines (length, diameter, radius).
  //! Please note that this method uses base dimension properties: working plane and flyout length.
  //! @param theSelection [in] the selection structure to fill with selection primitives.
  //! @param theOwner [in] the selection entity owner.
  //! @param theFirstPoint [in] the first attach point of linear dimension.
  //! @param theSecondPoint [in] the second attach point of linear dimension.
  Standard_EXPORT void ComputeLinearFlyouts (const Handle(SelectMgr_Selection)& theSelection,
                                             const Handle(SelectMgr_EntityOwner)& theOwner,
                                             const gp_Pnt& theFirstPoint,
                                             const gp_Pnt& theSecondPoint);


  //! Performs initialization of circle and middle arc point from the passed
  //! shape which is assumed to contain circular geometry.
  //! @param theShape [in] the shape to explore.
  //! @param theCircle [out] the circle geometry.
  //! @param theMiddleArcPoint [out] the middle point of the arc.
  //! @param theIsClosed [out] returns TRUE if the geometry is closed circle.
  //! @return TRUE if the circle is successfully returned from the input shape.
  Standard_EXPORT Standard_Boolean InitCircularDimension (const TopoDS_Shape& theShape,
                                                          gp_Circ& theCircle,
                                                          gp_Pnt& theMiddleArcPoint,
                                                          Standard_Boolean& theIsClosed);
 
  //! Produce points for triangular arrow face.
  //! @param thePeakPnt [in] the arrow peak position.
  //! @param theDirection [in] the arrow direction.
  //! @param thePlane [in] the face plane.
  //! @param theArrowLength [in] the length of arrow.
  //! @param theArrowAngle [in] the angle of arrow.
  //! @param theSidePnt1 [out] the first side point.
  //! @param theSidePnt2 [out] the second side point.
  Standard_EXPORT void PointsForArrow (const gp_Pnt& thePeakPnt,
                                       const gp_Dir& theDirection,
                                       const gp_Dir& thePlane,
                                       const Standard_Real theArrowLength,
                                       const Standard_Real theArrowAngle,
                                       gp_Pnt& theSidePnt1,
                                       gp_Pnt& theSidePnt2);

  //! Compute point of text position for dimension parameters
  //! for linear kinds of dimensions (length, radius, diameter).
  Standard_EXPORT gp_Pnt GetTextPositionForLinear (const gp_Pnt& theFirstPoint,
                                                   const gp_Pnt& theSecondPoint,
                                                   const Standard_Boolean theIsOneSide = Standard_False) const;

  //! Fits text alignment relatively to the dimension line.
  //! @param theFirstPoint [in] the first attachment point.
  //! @param theSecondPoint [in] the second attachment point.
  //! @param theIsOneSide [in] is the arrow displayed only on the one side of the dimension.
  //! @param theHorizontalTextPos [in] the text horizontal position (alignment).
  //! @param theLabelPosition [out] the label position, contains bits that defines
  //! vertical and horizontal alignment. (for internal usage in count text position)
  //! @param theIsArrowExternal [out] is the arrows external,
  //! if arrow orientation in the dimension aspect is Prs3d_DAO_Fit, it fits arrow
  //! orientation automatically.
  Standard_EXPORT void FitTextAlignmentForLinear (const gp_Pnt& theFirstPoint,
                                                  const gp_Pnt& theSecondPoint,
                                                  const Standard_Boolean theIsOneSide,
                                                  const Prs3d_DimensionTextHorizontalPosition& theHorizontalTextPos,
                                                  Standard_Integer& theLabelPosition,
                                                  Standard_Boolean& theIsArrowsExternal) const;

  //! Adjusts aspect parameters according the text position:
  //! extension size, vertical text alignment and flyout.
  //! @param theTextPos [in] the user defined 3d point of text position
  //! @param theFirstPoint [in] the first point of linear measurement.
  //! @param theSecondPoint [in] the second point of linear measurement.
  //! @param theExtensionSize [out] the adjusted extension size
  //! @param theAlignment [out] the horizontal label alignment.
  //! @param theFlyout [out] the adjusted value of flyout.
  //! @param thePlane [out] the new plane that contains theTextPos and attachment points.
  //! @param theIsPlaneOld [out] shows if new plane is computed.
  Standard_EXPORT Standard_Boolean AdjustParametersForLinear (const gp_Pnt& theTextPos,
                                                              const gp_Pnt& theFirstPoint,
                                                              const gp_Pnt& theSecondPoint,
                                                              Standard_Real& theExtensionSize,
                                                              Prs3d_DimensionTextHorizontalPosition& theAlignment,
                                                              Standard_Real& theFlyout,
                                                              gp_Pln& thePlane,
                                                              Standard_Boolean& theIsPlaneOld) const;

protected: //! @name Static auxilliary methods for geometry extraction

  //! If it is possible extracts circle from planar face.
  //! @param theFace        [in] the planar face
  //! @param theCurve       [out] the circular curve
  //! @param theFirstPoint  [out] the point of the first parameter of the circlular curve
  //! @param theSecondPoint [out] the point of the last parameter of the circlular curve
  //! @return TRUE in case of successful circle extraction
  static Standard_Boolean CircleFromPlanarFace (const TopoDS_Face&  theFace,
                                                Handle(Geom_Curve)& theCurve,
                                                gp_Pnt&             theFirstPoint,
                                                gp_Pnt&             theLastPoint);

  //! If it is possible extracts circle from the edge.
  //! @param theEdge        [in] input edge to extract circle from
  //! @param theCircle      [out] circle
  //! @param theFirstPoint  [out] the point of the first parameter of the circlular curve
  //! @param theSecondPoint [out] the point of the last parameter of the circlular curve
  //! @return TRUE in case of successful circle extraction.
  static Standard_Boolean CircleFromEdge (const TopoDS_Edge& theEdge,
                                          gp_Circ&           theCircle,
                                          gp_Pnt&            theFirstPoint,
                                          gp_Pnt&            theLastPoint);

protected: //! @name Behavior to implement

  //! Override this method to check if user-defined plane
  //! is valid for the dimension geometry.
  //! @param thePlane [in] the working plane for positioning every
  //! dimension in the application.
  //! @return true is the plane is suitable for building dimension
  //! with computed dimension geometry.
  virtual Standard_Boolean CheckPlane (const gp_Pln& /*thePlane*/) const { return Standard_True; }

  //! Override this method to computed value of dimension.
  //! @return value from the measured geometry.
  virtual Standard_Real ComputeValue() const 
  {
    return 0.0;
  }

  //! Override this method to compute selection primitives for
  //! flyout lines (if the dimension provides it).
  //! This callback is a only a part of base selection
  //! computation routine.
  virtual void ComputeFlyoutSelection (const Handle(SelectMgr_Selection)&,
                                       const Handle(SelectMgr_EntityOwner)&) {}


  //! Base procedure of computing selection (based on selection geometry data).
  //! @param theSelection [in] the selection structure to will with primitives.
  //! @param theMode [in] the selection mode.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

protected: //! @name Selection geometry

  //! Selection geometry of dimension presentation. The structure is filled with data
  //! during compute of presentation, then this data is used to generate selection
  //! sensitives when computing selection.
  struct SelectionGeometry
  {
    //! Arrows are represented by directed triangles.
    struct Arrow
    {
      gp_Pnt Position;
      gp_Dir Direction;
    };
    typedef NCollection_Sequence<gp_Pnt> Curve;
    typedef NCollection_Handle<Curve>    HCurve;
    typedef NCollection_Handle<Arrow>    HArrow;
    typedef NCollection_Sequence<HCurve> SeqOfCurves;
    typedef NCollection_Sequence<HArrow> SeqOfArrows;

    gp_Pnt           TextPos;            //!< Center of text label.
    gp_Dir           TextDir;            //!< Direction of text label.
    Standard_Real    TextWidth;          //!< Width of text label.
    Standard_Real    TextHeight;         //!< Height of text label.
    SeqOfCurves      DimensionLine;      //!< Sequence of points for composing the segments of dimension line.
    SeqOfArrows      Arrows;             //!< Sequence of arrow geometries.
    Standard_Boolean IsComputed;         //!< Shows if the selection geometry was filled.

  public:

    //! Clear geometry of sensitives for the specified compute mode.
    //! @param theMode [in] the compute mode to clear.
    void Clear (const Standard_Integer theMode)
    {
      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        DimensionLine.Clear();
        Arrows.Clear();
      }

      if (theMode == ComputeMode_All || theMode == ComputeMode_Text)
      {
        TextPos    = gp::Origin();
        TextDir    = gp::DX();
        TextWidth  = 0.0;
        TextHeight = 0.0;
      }

      IsComputed = Standard_False;
    }

    //! Add new curve entry and return the reference to populate it.
    Curve& NewCurve()
    {
      DimensionLine.Append( new Curve );
      HCurve& aLastCurve = DimensionLine.ChangeLast();
      return *aLastCurve;
    }

    //! Add new arrow entry and return the reference to populate it.
    Arrow& NewArrow()
    {
      Arrows.Append( new Arrow );
      HArrow& aLastArrow = Arrows.ChangeLast();
      return *aLastArrow;
    }
  } mySelectionGeom;

  Standard_Real mySelToleranceForText2d; //!< Sensitive point tolerance for 2d text selection.

protected: //! @name Value properties

  ValueType        myValueType; //! type of value (computed or user-defined)
  Standard_Real    myCustomValue;   //!< Value of the dimension (computed or user-defined).

  TCollection_ExtendedString myCustomStringValue; //!< Value of the dimension (computed or user-defined).

protected: //! @name Fixed text position properties

  gp_Pnt                  myFixedTextPosition;   //!< Stores text position fixed by user.
  Standard_Boolean        myIsTextPositionFixed; //!< Is the text label position fixed by user.

protected: //! @name Units properties

  Standard_ExtCharacter       mySpecialSymbol;        //!< Special symbol.
  PrsDim_DisplaySpecialSymbol myDisplaySpecialSymbol; //!< Special symbol display options.

protected: //! @name Geometrical properties

  GeometryType myGeometryType;  //!< defines type of shapes on which the dimension is to be built. 

  gp_Pln           myPlane;           //!< Plane where dimension will be built (computed or user defined).
  Standard_Boolean myIsPlaneCustom;   //!< Is plane defined by user (otherwise it will be computed automatically).
  Standard_Real    myFlyout;          //!< Flyout distance.
  Standard_Boolean myIsGeometryValid; //!< Is dimension geometry properly defined.

private:

  PrsDim_KindOfDimension myKindOfDimension;
};

#endif // _PrsDim_Dimension_HeaderFile
