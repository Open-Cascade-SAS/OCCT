// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _AIS_Dimension_Headerfile
#define _AIS_Dimension_Headerfile

#include <AIS_DimensionDisplayMode.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_DisplaySpecialSymbol.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <AIS_KindOfDimension.hxx>
#include <AIS_KindOfSurface.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pln.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Select3D_ListOfSensitive.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

DEFINE_STANDARD_HANDLE(AIS_Dimension, AIS_InteractiveObject)

class AIS_Dimension : public AIS_InteractiveObject
{
protected:

  // Specifies supported at base level horizontal and vertical
  // label positions for drawing extension lines and centered text.
  enum LabelPosition
  {
    LabelPosition_None    = 0x00,

    LabelPosition_Left    = 0x01,
    LabelPosition_Right   = 0x02,
    LabelPosition_HCenter = 0x04,
    LabelPosition_HMask  = LabelPosition_Left | LabelPosition_Right | LabelPosition_HCenter,

    LabelPosition_Above   = 0x10,
    LabelPosition_Below   = 0x20,
    LabelPosition_VCenter = 0x40,
    LabelPosition_VMask  = LabelPosition_Above | LabelPosition_Below | LabelPosition_VCenter
  };

public:

  //! Constructor with default parameters values
  Standard_EXPORT  AIS_Dimension();

  //! Gets dimension value
  Standard_EXPORT  Standard_Real GetValue() const;

  //! Sets dimension value
  //! Attention! This method is used ONLY to set custom value.
  //! To set value internally, use <myValue>.
  Standard_EXPORT  void SetCustomValue (const Standard_Real theValue);

  //! Gets working plane.
  Standard_EXPORT  const gp_Pln& GetWorkingPlane() const;

  //! Sets working plane.
  Standard_EXPORT  void SetWorkingPlane (const gp_Pln& thePlane);

  Standard_EXPORT  void SetFirstPoint (const gp_Pnt& thePoint);

  Standard_EXPORT  void SetSecondPoint (const gp_Pnt& thePoint);

  Standard_EXPORT  void SetFirstShape (const TopoDS_Shape& theFirstShape);

  Standard_EXPORT  void SetSecondShape (const TopoDS_Shape& theSecondShape);

  //! Gets the dimension aspect from AIS object drawer.
  //! Dimension aspect contains aspects of line, text and arrows for dimension presentation.
  Standard_EXPORT   Handle(Prs3d_DimensionAspect) DimensionAspect() const;

  //! Sets new length aspect in the interactive object drawer.
  Standard_EXPORT   void SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theDimensionAspect);

  //! Returns the kind of dimension
  Standard_EXPORT  AIS_KindOfDimension KindOfDimension() const;

  //! Returns the kind of interactive
  Standard_EXPORT  virtual  AIS_KindOfInteractive Type() const;

  //! Sets the kind of dimension
  Standard_EXPORT  virtual void SetKindOfDimension (const AIS_KindOfDimension theKindOfDimension);

  //! Returns true if the class of objects accepts the display mode theMode.
  //! The interactive context can have a default mode of
  //! representation for the set of Interactive Objects. This
  //! mode may not be accepted by object
  Standard_EXPORT   virtual  Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const;

  // Selection computing if it is needed here
  Standard_EXPORT   virtual  void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                    const Standard_Integer theMode);

  //! Reset working plane to default.
  Standard_EXPORT  void ResetWorkingPlane();

  //! specifies dimension special symbol display options
  Standard_EXPORT  void SetDisplaySpecialSymbol (const AIS_DisplaySpecialSymbol theDisplaySpecSymbol);

  //! shows dimension special symbol display options
  Standard_EXPORT  AIS_DisplaySpecialSymbol DisplaySpecialSymbol() const;

  //! specifies special symbol
  Standard_EXPORT  void SetSpecialSymbol (const Standard_ExtCharacter theSpecialSymbol);

  //! returns special symbol
  Standard_EXPORT  Standard_ExtCharacter SpecialSymbol() const;

  //! shows if Units are to be displayed along with dimension value
  Standard_EXPORT  Standard_Boolean IsUnitsDisplayed() const;

  //! sets to display units along with the dimansion value or no
  Standard_EXPORT  void MakeUnitsDisplayed (const Standard_Boolean toDisplayUnits);

  //! returns the current type of units
  Standard_EXPORT  TCollection_AsciiString UnitsQuantity() const;

  //! sets the current type of units
  Standard_EXPORT  void SetUnitsQuantity (const TCollection_AsciiString& theUnitsQuantity);

  //! returns the current model units
  Standard_EXPORT  TCollection_AsciiString ModelUnits() const;

  //! sets the current model units
  Standard_EXPORT  void SetModelUnits (const TCollection_AsciiString& theUnits);

  //! returns the current display units
  Standard_EXPORT  TCollection_AsciiString DisplayUnits() const;

  //! sets the current display units
  Standard_EXPORT  void SetDisplayUnits (const TCollection_AsciiString& theUnits);

  //! Important! Only for 3d text </br>
  //! 3d text is oriented relative to the attachment points order </br>
  //! By default, text direction vector is oriented from the first attachment point </br>
  //! to the second one. This method checks if text direction is to be default or </br>
  //! should be reversed.
  Standard_EXPORT  Standard_Boolean IsTextReversed() const;

  //! Important! Only for 3d text
  //! 3d text is oriented relative to the attachment points order </br>
  //! By default, text direction vector is oriented from the first attachment point </br>
  //! to the second one. This method sets value that shows if text direction </br>
  //! should be reversed or not.
  Standard_EXPORT  void MakeTextReversed (const Standard_Boolean isTextReversed);

  //! Sets selection tolerance for text2d:
  //! For 2d text selection detection sensitive point with tolerance is used
  //! to change this tolerance use this method
  //! Important! Only for 2d text
  Standard_EXPORT  void SetSelToleranceForText2d (const Standard_Real theTol);

  //! Returns selection tolerance for text2d:
  //! For 2d text selection detection sensitive point with tolerance is used
  //! Important! Only for 2d text
  Standard_EXPORT  Standard_Real SelToleranceForText2d() const;

  //! Sets flyout size for dimension.
  Standard_EXPORT void SetFlyout (const Standard_Real theFlyout);

  //! @return flyout size for dimension.
  Standard_Real GetFlyout() const
  {
    return myFlyout;
  }

public:

  DEFINE_STANDARD_RTTI(AIS_Dimension)

protected:

  Standard_EXPORT void getTextWidthAndString (Quantity_Length& theWidth,
                                              TCollection_ExtendedString& theString) const;

  Standard_EXPORT Standard_Real valueToDisplayUnits();

  //! Reset working plane to default.
  Standard_EXPORT void resetWorkingPlane (const gp_Pln& theNewDefaultPlane);

  //! Count default plane 
  Standard_EXPORT virtual void countDefaultPlane();

  //! Computes dimension value in display units
  Standard_EXPORT virtual void computeValue();

  //! Performs drawing of 2d or 3d arrows on the working plane
  Standard_EXPORT void drawArrow (const Handle(Prs3d_Presentation)& thePresentation,
                                  const gp_Pnt& theLocation,
                                  const gp_Dir& theDirection);

  //! Performs drawing of 2d or 3d text on the working plane
  //! @return text width relative to the dimension working plane. For 2d text this value will be zero.
  Standard_EXPORT Standard_Real drawText (const Handle(Prs3d_Presentation)& thePresentation,
                                          const gp_Dir& theTextDir,
                                          const TCollection_ExtendedString theText,
                                          const AIS_DimensionDisplayMode theMode,
                                          const Standard_Integer theLabelPosition);

  //! Performs computing of dimension linear extension with text
  //! @param thePresentation [in] the presentation to fill with graphical primitives.
  //! @param theExtensionSize [in] the size of extension line.
  //! @param theExtensionStart [in] the point where extension line connects to dimension.
  //! @param theExtensionDir [in] the direction of extension line.
  //! @param theValueString [in] the string with value.
  //! @param theMode [in] the display mode.
  //! @param theLabelPosition [in] position flags for the text label.
  Standard_EXPORT void drawExtension (const Handle(Prs3d_Presentation)& thePresentation,
                                      const Standard_Real theExtensionSize,
                                      const gp_Pnt& theExtensionStart,
                                      const gp_Dir& theExtensionDir,
                                      const TCollection_ExtendedString& theValueString,
                                      const AIS_DimensionDisplayMode theMode,
                                      const Standard_Integer theLabelPosition);

  //! Performs computing of linear dimension (for length, diameter, radius and so on)
  Standard_EXPORT void drawLinearDimension (const Handle(Prs3d_Presentation)& thePresentation,
                                            const AIS_DimensionDisplayMode theMode,
                                            const Standard_Boolean isOneSideDimension = Standard_False);

  //! If it's possible computes circle from planar face
  Standard_EXPORT  Standard_Boolean circleFromPlanarFace (const TopoDS_Face& theFace,
                                                          Handle(Geom_Curve)& theCurve,
                                                          gp_Pnt & theFirstPoint,
                                                          gp_Pnt & theLastPoint);

  //! Performs initialization of circle and points from given shape
  //! (for radius, diameter and so on)
  Standard_EXPORT  Standard_Boolean initCircularDimension (const TopoDS_Shape& theShape,
                                                           gp_Circ& theCircle,
                                                           gp_Pnt& theMiddleArcPoint,
                                                           gp_Pnt& theOppositeDiameterPoint);
  Standard_EXPORT Standard_Boolean isComputed() const;

  Standard_EXPORT void setComputed (Standard_Boolean isComputed);

  Standard_EXPORT gp_Pnt textPosition() const;

  Standard_EXPORT void setTextPosition (const gp_Pnt thePosition);

  Standard_EXPORT void resetGeom();

  //! Fills sensitive entity for flyouts and adds it to the selection.
  Standard_EXPORT virtual void computeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                       const Handle(AIS_DimensionOwner)& theOwner);

protected: //! @name Working plane properties

  //! Dimension default plane
  gp_Pln myDefaultPlane;

  //! Shows if working plane is set custom
  Standard_Boolean myIsWorkingPlaneCustom;

protected: //! @name Value properties

  //! Dimension value which is displayed with dimension lines
  Standard_Real myValue;

  //! Shows if the value is set by user and is no need to count it automatically
  Standard_Boolean myIsValueCustom;

protected: // !@name Units properties

  //! The quantity of units for the value computation
  TCollection_AsciiString myUnitsQuantity;

  //! Units of the model
  TCollection_AsciiString myModelUnits;

  //! Units in which the displayed value will be converted
  TCollection_AsciiString myDisplayUnits;

  //! Determines if units is to be displayed along with the value
  Standard_Boolean myToDisplayUnits;

  //! Special symbol for some kind of dimensions (for diameter, radius and so on)
  Standard_ExtCharacter mySpecialSymbol;

  //! Special symbol display options
  AIS_DisplaySpecialSymbol myDisplaySpecialSymbol;

protected: //! @name Geometry properties

  //! Geometry of dimensions, needs for advanced selection
  //! Geometry is computed in Compute() method and is used 
  //! in ComputeSelection() method.
  //! If it is computed successfully, myIsComputed = Standard_True.
  //! to check computing result use IsComputed() method
  struct DimensionGeom
  {
    //! Text position
    gp_Pnt myTextPosition;

    //! Text bounding box, stored for advanced selection
    Bnd_Box myTextBndBox;

    //! Sensitive point tolerance for 2d text selection
    Standard_Real mySelToleranceForText2d;

    //! For advanced dimension line selection
    Select3D_ListOfSensitive mySensitiveSegments;

    //! Shows if attachment points were computed
    Standard_Boolean myIsComputed;

  public: 

    DimensionGeom () : myIsComputed (Standard_False) {}
  };

  //! Shows if text is inverted
  Standard_Boolean myIsTextReversed;

  //! Points that are base for dimension.
  //! My first point of dimension attach (belongs to shape for which dimension is computed)
  gp_Pnt myFirstPoint;

  //! My second point of dimension attach (belongs to shape for which dimension is computed)
  gp_Pnt mySecondPoint;

  //! Shows if attach points are initialized correctly
  Standard_Boolean myIsInitialized;

  //! First shape (can be vertex, edge or face)
  TopoDS_Shape myFirstShape;

  //! Second shape (can be vertex, edge or face)
  TopoDS_Shape mySecondShape;

  //! Number of shapes
  Standard_Integer myShapesNumber;

  //! Defines flyout lines and direction
  //! Flyout direction in the working plane.
  //! Can be negative, or positive and is defined by the sign of myFlyout value.
  //! The direction vector is counting using the working plane.
  //! myFlyout value defined the size of flyout.
  Standard_Real myFlyout;

  //! Geometry of dimensions, needs for advanced selection
  //! Geometry is computed in Compute() method and is used 
  //! in ComputeSelection() method.
  //! If it is computed successfully, myIsComputed = Standard_True.
  //! to check computing result use IsComputed() method
  DimensionGeom myGeom;

private:

  //! Type of dimension
  AIS_KindOfDimension myKindOfDimension;

  //! Dimension working plane, is equal to <myDefaultPlane> if it can be computed automatically.
  gp_Pln myWorkingPlane;
};
#endif
