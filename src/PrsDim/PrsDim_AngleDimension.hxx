// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _PrsDim_AngleDimension_HeaderFile
#define _PrsDim_AngleDimension_HeaderFile

#include <PrsDim_Dimension.hxx>
#include <PrsDim_TypeOfAngle.hxx>
#include <PrsDim_TypeOfAngleArrowVisibility.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

DEFINE_STANDARD_HANDLE(PrsDim_AngleDimension, PrsDim_Dimension)

//! Angle dimension. Can be constructed:
//! - on two intersected edges.
//! - on three points or vertices.
//! - on conical face.
//! - between two intersected faces.
//!
//! In case of three points or two intersected edges the dimension plane
//! (on which dimension presentation is built) can be computed uniquely
//! as through three defined points can be built only one plane.
//! Therefore, if user-defined plane differs from this one, the dimension can't be built.
//!
//! In cases of two planes automatic plane by default is built on point of the
//! origin of parametric space of the first face (the basis surface) so, that
//! the working plane and two faces intersection forms minimal angle between the faces.
//! User can define the other point which the dimension plane should pass through
//! using the appropriate constructor. This point can lay on the one of the faces or not.
//! Also user can define his own plane but it should pass through the three points
//! computed on the geometry initialization step (when the constructor or SetMeasuredGeometry() method
//! is called). 
//!
//! In case of the conical face the center point of the angle is the apex of the conical surface.
//! The attachment points are points of the first and the last parameter of the basis circle of the cone.
class PrsDim_AngleDimension : public PrsDim_Dimension
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_AngleDimension, PrsDim_Dimension)
public:

  //! Constructs minimum angle dimension between two linear edges (where possible).
  //! These two edges should be intersected by each other. Otherwise the geometry is not valid.
  //! @param theFirstEdge [in] the first edge.
  //! @param theSecondEdge [in] the second edge.
  Standard_EXPORT PrsDim_AngleDimension (const TopoDS_Edge& theFirstEdge,
                                         const TopoDS_Edge& theSecondEdge);

  //! Constructs the angle display object defined by three points.
  //! @param theFirstPoint [in] the first point (point on first angle flyout).
  //! @param theSecondPoint [in] the center point of angle dimension.
  //! @param theThirdPoint [in] the second point (point on second angle flyout).
  Standard_EXPORT PrsDim_AngleDimension (const gp_Pnt& theFirstPoint,
                                         const gp_Pnt& theSecondPoint,
                                         const gp_Pnt& theThirdPoint);

  //! Constructs the angle display object defined by three vertices.
  //! @param theFirstVertex [in] the first vertex (vertex for first angle flyout).
  //! @param theSecondVertex [in] the center vertex of angle dimension.
  //! @param theThirdPoint [in] the second vertex (vertex for second angle flyout).
  Standard_EXPORT PrsDim_AngleDimension (const TopoDS_Vertex& theFirstVertex,
                                         const TopoDS_Vertex& theSecondVertex,
                                         const TopoDS_Vertex& theThirdVertex);

  //! Constructs angle dimension for the cone face.
  //! @param theCone [in] the conical face.
  Standard_EXPORT PrsDim_AngleDimension (const TopoDS_Face& theCone);

  //! Constructs angle dimension between two planar faces.
  //! @param theFirstFace [in] the first face.
  //! @param theSecondFace [in] the second face.
  Standard_EXPORT PrsDim_AngleDimension (const TopoDS_Face& theFirstFace,
                                         const TopoDS_Face& theSecondFace);

  //! Constructs angle dimension between two planar faces.
  //! @param theFirstFace [in] the first face.
  //! @param theSecondFace [in] the second face.
  //! @param thePoint [in] the point which the dimension plane should pass through.
  //! This point can lay on the one of the faces or not.
  Standard_EXPORT PrsDim_AngleDimension (const TopoDS_Face& theFirstFace,
                                         const TopoDS_Face& theSecondFace,
                                         const gp_Pnt& thePoint);

public:

  //! @return first point forming the angle.
  const gp_Pnt& FirstPoint() const { return myFirstPoint; }

  //! @return second point forming the angle.
  const gp_Pnt& SecondPoint() const { return mySecondPoint; }

  //! @return center point forming the angle.
  const gp_Pnt& CenterPoint() const { return myCenterPoint; }

  //! @return first argument shape.
  const TopoDS_Shape& FirstShape() const { return myFirstShape; }

  //! @return second argument shape.
  const TopoDS_Shape& SecondShape() const { return mySecondShape; }

  //! @return third argument shape.
  const TopoDS_Shape& ThirdShape() const { return myThirdShape; }

public:

  //! Measures minimum angle dimension between two linear edges.
  //! These two edges should be intersected by each other. Otherwise the geometry is not valid.
  //! @param theFirstEdge [in] the first edge.
  //! @param theSecondEdge [in] the second edge.
  Standard_EXPORT void SetMeasuredGeometry (const TopoDS_Edge& theFirstEdge,
                                            const TopoDS_Edge& theSecondEdge);

  //! Measures angle defined by three points.
  //! @param theFirstPoint [in] the first point (point on first angle flyout).
  //! @param theSecondPoint [in] the center point of angle dimension.
  //! @param theThirdPoint [in] the second point (point on second angle flyout).
  Standard_EXPORT void SetMeasuredGeometry (const gp_Pnt& theFirstPoint,
                                            const gp_Pnt& theSecondPoint,
                                            const gp_Pnt& theThridPoint);

  //! Measures angle defined by three vertices.
  //! @param theFirstVertex [in] the first vertex (vertex for first angle flyout).
  //! @param theSecondVertex [in] the center vertex of angle dimension.
  //! @param theThirdPoint [in] the second vertex (vertex for second angle flyout).
  Standard_EXPORT void SetMeasuredGeometry (const TopoDS_Vertex& theFirstVertex,
                                            const TopoDS_Vertex& theSecondVertex,
                                            const TopoDS_Vertex& theThirdVertex);

  //! Measures angle of conical face.
  //! @param theCone [in] the shape to measure.
  Standard_EXPORT void SetMeasuredGeometry (const TopoDS_Face& theCone);

  //! Measures angle between two planar faces.
  //! @param theFirstFace [in] the first face.
  //! @param theSecondFace [in] the second face..
  Standard_EXPORT void SetMeasuredGeometry (const TopoDS_Face& theFirstFace,
                                            const TopoDS_Face& theSecondFace);

  //! Measures angle between two planar faces.
  //! @param theFirstFace [in] the first face.
  //! @param theSecondFace [in] the second face.
  //! @param thePoint [in] the point which the dimension plane should pass through.
  //! This point can lay on the one of the faces or not.
  Standard_EXPORT void SetMeasuredGeometry (const TopoDS_Face& theFirstFace,
                                            const TopoDS_Face& theSecondFace,
                                            const gp_Pnt& thePoint);

  //! @return the display units string.
  Standard_EXPORT virtual const TCollection_AsciiString& GetDisplayUnits() const Standard_OVERRIDE;
  
  //! @return the model units string.
  Standard_EXPORT virtual const TCollection_AsciiString& GetModelUnits() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetDisplayUnits (const TCollection_AsciiString& theUnits) Standard_OVERRIDE;

  Standard_EXPORT virtual void SetModelUnits (const TCollection_AsciiString& theUnits) Standard_OVERRIDE;

  //! Principle of horizontal text alignment settings:
  //! - divide circle into two halves according to attachment points
  //! - if aTextPos is between attach points -> Center + positive flyout
  //! - if aTextPos is not between attach points but in this half -> Left or Right + positive flyout
  //! - if aTextPos is between reflections of attach points -> Center + negative flyout
  //! - if aTextPos is not between reflections of attach points -> Left or Right + negative flyout
  Standard_EXPORT virtual void SetTextPosition (const gp_Pnt& theTextPos) Standard_OVERRIDE;

  Standard_EXPORT virtual gp_Pnt GetTextPosition () const Standard_OVERRIDE;

  //! Sets angle type.
  //! @param theType [in] the type value.
  void SetType (const PrsDim_TypeOfAngle theType) { myType = theType; }

  //! @return the current angle type.
  PrsDim_TypeOfAngle GetType() const { return myType; }

  //! Sets visible arrows type
  //! @param theType [in] the type of visibility of arrows.
  void SetArrowsVisibility (const PrsDim_TypeOfAngleArrowVisibility& theType) { myArrowsVisibility = theType; }

  //! @return the type of visibility of arrows.
  PrsDim_TypeOfAngleArrowVisibility GetArrowsVisibility() const { return myArrowsVisibility; }

protected:

  //! Initialization of fields that is common to all constructors. 
  Standard_EXPORT void Init();

  //! Gets plane normal for minimal angle.
  //! Dimension computation is based on three attach points and plane normal.
  //! Based on this normal angle arc, arrows and extensions are constructed.
  gp_Dir GetNormalForMinAngle() const;

  //! @param theFirstAttach [in] the first attachment point.
  //! @param theSecondAttach [in] the second attachment point.
  //! @param theCenter [in] the center point (center point of the angle).  
  //! @return the center of the dimension arc (the main dimension line in case of angle). 
  Standard_EXPORT gp_Pnt GetCenterOnArc (const gp_Pnt& theFirstAttach,
                                         const gp_Pnt& theSecondAttach,
                                         const gp_Pnt& theCenter) const;

  //! Draws main dimension line (arc).
  //! @param thePresentation [in] the dimension presentation.
  //! @param theFirstAttach [in] the first attachment point.
  //! @param theSecondAttach [in] the second attachment point.
  //! @param theCenter [in] the center point (center point of the angle).
  //! @param theRadius [in] the radius of the dimension arc.
  //! @param theMode [in] the display mode.
  Standard_EXPORT void DrawArc (const Handle(Prs3d_Presentation)& thePresentation,
                                const gp_Pnt& theFirstAttach,
                                const gp_Pnt& theSecondAttach,
                                const gp_Pnt& theCenter,
                                const Standard_Real theRadius,
                                const Standard_Integer theMode);

  //! Draws main dimension line (arc) with text.
  //! @param thePresentation [in] the dimension presentation.
  //! @param theFirstAttach [in] the first attachment point.
  //! @param theSecondAttach [in] the second attachment point.
  //! @param theCenter [in] the center point (center point of the angle).
  //! @param theText [in] the text label string.
  //! @param theTextWidth [in] the text label width. 
  //! @param theMode [in] the display mode.
  //! @param theLabelPosition [in] the text label vertical and horizontal positioning option
  //! respectively to the main dimension line. 
  Standard_EXPORT void DrawArcWithText (const Handle(Prs3d_Presentation)& thePresentation,
                                        const gp_Pnt& theFirstAttach,
                                        const gp_Pnt& theSecondAttach,
                                        const gp_Pnt& theCenter,
                                        const TCollection_ExtendedString& theText,
                                        const Standard_Real theTextWidth,
                                        const Standard_Integer theMode,
                                        const Standard_Integer theLabelPosition);

  //! Fits text alignment relatively to the dimension line;
  //! it computes the value of label position and arrow orientation
  //! according set in the aspect and dimension properties.
  //! @param theHorizontalTextPos [in] the horizontal alignment for text position.
  //! @param theLabelPosition [out] the label position, contains bits that defines
  //! vertical and horizontal alignment. (for internal usage in count text position).
  //! @param theIsArrowExternal [out] is the arrows external,
  //! if arrow orientation in the dimension aspect is Prs3d_DAO_Fit, it fits arrow
  //! orientation automatically.
  Standard_EXPORT void FitTextAlignment (const Prs3d_DimensionTextHorizontalPosition& theHorizontalTextPos,
                                         Standard_Integer& theLabelPosition,
                                         Standard_Boolean& theIsArrowsExternal) const;

  //! Adjusts aspect parameters according the text position:
  //! extension size, vertical text alignment and flyout.
  //! @param theTextPos [in] the user defined 3d point of text position.
  //! @param theExtensionSize [out] the adjusted extension size.
  //! @param theAlignment [out] the horizontal label alignment.
  //! @param theFlyout [out] the adjusted value of flyout.
  Standard_EXPORT void AdjustParameters (const gp_Pnt& theTextPos,
                                         Standard_Real& theExtensionSize,
                                         Prs3d_DimensionTextHorizontalPosition& theAlignment,
                                         Standard_Real& theFlyout) const;

protected:

  Standard_EXPORT virtual void ComputePlane();

  //! Checks if the plane includes three angle points to build dimension.
  Standard_EXPORT virtual Standard_Boolean CheckPlane (const gp_Pln& thePlane) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Real ComputeValue() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePM,
                                        const Handle(Prs3d_Presentation)& thePresentation,
                                        const Standard_Integer theMode = 0) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                       const Handle(SelectMgr_EntityOwner)& theOwner) Standard_OVERRIDE;

protected:

  //! Init angular dimension to measure angle between two linear edges.
  //! @return TRUE if the angular dimension can be constructured
  //!         for the passed edges.
  Standard_EXPORT Standard_Boolean InitTwoEdgesAngle (gp_Pln& theComputedPlane);

  //! Init angular dimension to measure angle between two planar faces.
  //! there is no user-defined poisitoning. So attach points are set
  //! according to faces geometry (in origin of the first face basis surface).
  //! @return TRUE if the angular dimension can be constructed
  //!         for the passed faces.
  Standard_EXPORT Standard_Boolean InitTwoFacesAngle();

  //! Init angular dimension to measure angle between two planar faces.
  //! @param thePointOnFirstFace [in] the point which the dimension plane should pass through.
  //! This point can lay on the one of the faces or not.
  //! It will be projected on the first face and this point will be set
  //! as the first point attach point.
  //! It defines some kind of dimension positioning over the faces.
  //! @return TRUE if the angular dimension can be constructed
  //!         for the passed faces.
  Standard_EXPORT Standard_Boolean InitTwoFacesAngle (const gp_Pnt& thePointOnFirstFace);

  //! Init angular dimension to measure cone face.
  //! @return TRUE if the angular dimension can be constructed
  //!              for the passed cone.
  Standard_EXPORT Standard_Boolean InitConeAngle();

  //! Check that the points forming angle are valid.
  //! @return TRUE if the points met the following requirements:
  //!         The (P1, Center), (P2, Center) can be built.
  //!         The angle between the vectors > Precision::Angular().
  Standard_EXPORT Standard_Boolean IsValidPoints (const gp_Pnt& theFirstPoint,
                                                  const gp_Pnt& theCenterPoint,
                                                  const gp_Pnt& theSecondPoint) const;


  //! Returns true if the arrow should be visible
  //! @param theArrowType an arrow type
  //! @return TRUE if the arrow should be visible
  Standard_EXPORT Standard_Boolean isArrowVisible (const PrsDim_TypeOfAngleArrowVisibility theArrowType) const;

private:
  PrsDim_TypeOfAngle myType; //!< type of angle
  PrsDim_TypeOfAngleArrowVisibility myArrowsVisibility; //!< type of arrows visibility

  gp_Pnt myFirstPoint;
  gp_Pnt mySecondPoint;
  gp_Pnt myCenterPoint;
  TopoDS_Shape myFirstShape;
  TopoDS_Shape mySecondShape;
  TopoDS_Shape myThirdShape;
};

#endif // _PrsDim_AngleDimension_HeaderFile
