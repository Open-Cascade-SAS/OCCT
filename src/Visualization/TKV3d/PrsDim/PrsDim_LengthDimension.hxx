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

#ifndef _PrsDim_LengthDimension_HeaderFile
#define _PrsDim_LengthDimension_HeaderFile

#include <PrsDim_Dimension.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <TopoDS.hxx>

//! Length dimension. Can be constructed:
//! - Between two generic points.
//! - Between two vertices.
//! - Between two faces.
//! - Between two parallel edges.
//! - Between face and edge.
//!
//! In case of two points (vertices) or one linear edge the user-defined plane
//! that includes this geometry is necessary to be set.
//!
//! In case of face-edge, edge-vertex or face-face lengths the automatic plane
//! computing is allowed. For this plane the third point is found on the
//! edge or on the face.
//!
//! Please note that if the inappropriate geometry is defined
//! or the distance between measured points is less than
//! Precision::Confusion(), the dimension is invalid and its
//! presentation can not be computed.
class PrsDim_LengthDimension : public PrsDim_Dimension
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_LengthDimension, PrsDim_Dimension)
public:
  //! Construct an empty length dimension.
  //! @sa SetMeasuredGeometry(), SetMeasuredShapes() for initialization.
  Standard_EXPORT PrsDim_LengthDimension();

  //! Construct length dimension between face and edge.
  //! Here dimension can be built without user-defined plane.
  //! @param[in] theFace  the face (first shape).
  //! @param[in] theEdge  the edge (second shape).
  Standard_EXPORT PrsDim_LengthDimension(const TopoDS_Face& theFace, const TopoDS_Edge& theEdge);

  //! Construct length dimension between two faces.
  //! @param[in] theFirstFace  the first face (first shape).
  //! @param[in] theSecondFace  the second face (second shape).
  Standard_EXPORT PrsDim_LengthDimension(const TopoDS_Face& theFirstFace,
                                         const TopoDS_Face& theSecondFace);

  //! Construct length dimension between two points in
  //! the specified plane.
  //! @param[in] theFirstPoint  the first point.
  //! @param[in] theSecondPoint  the second point.
  //! @param[in] thePlane  the plane to orient dimension.
  Standard_EXPORT PrsDim_LengthDimension(const gp_Pnt& theFirstPoint,
                                         const gp_Pnt& theSecondPoint,
                                         const gp_Pln& thePlane);

  //! Construct length dimension between two arbitrary shapes in
  //! the specified plane.
  //! @param[in] theFirstShape  the first shape.
  //! @param[in] theSecondShape  the second shape.
  //! @param[in] thePlane  the plane to orient dimension.
  Standard_EXPORT PrsDim_LengthDimension(const TopoDS_Shape& theFirstShape,
                                         const TopoDS_Shape& theSecondShape,
                                         const gp_Pln&       thePlane);

  //! Construct length dimension of linear edge.
  //! @param[in] theEdge  the edge to measure.
  //! @param[in] thePlane  the plane to orient dimension.
  Standard_EXPORT PrsDim_LengthDimension(const TopoDS_Edge& theEdge, const gp_Pln& thePlane);

public:
  //! @return first attachment point.
  const gp_Pnt& FirstPoint() const { return myFirstPoint; }

  //! @return second attachment point.
  const gp_Pnt& SecondPoint() const { return mySecondPoint; }

  //! @return first attachment shape.
  const TopoDS_Shape& FirstShape() const { return myFirstShape; }

  //! @return second attachment shape.
  const TopoDS_Shape& SecondShape() const { return mySecondShape; }

public:
  //! Measure distance between two points.
  //! The dimension will become invalid if the new distance between
  //! attachment points is less than Precision::Confusion().
  //! @param[in] theFirstPoint  the first point.
  //! @param[in] theSecondPoint  the second point.
  //! @param[in] thePlane  the user-defined plane
  Standard_EXPORT void SetMeasuredGeometry(const gp_Pnt& theFirstPoint,
                                           const gp_Pnt& theSecondPoint,
                                           const gp_Pln& thePlane);

  //! Measure length of edge.
  //! The dimension will become invalid if the new length of edge
  //! is less than Precision::Confusion().
  //! @param[in] theEdge  the edge to measure.
  //! @param[in] thePlane  the user-defined plane
  Standard_EXPORT void SetMeasuredGeometry(const TopoDS_Edge& theEdge, const gp_Pln& thePlane);

  //! Measure distance between two faces.
  //! The dimension will become invalid if the distance can not
  //! be measured or it is less than Precision::Confusion().
  //! @param[in] theFirstFace  the first face (first shape).
  //! @param[in] theSecondFace  the second face (second shape).
  Standard_EXPORT void SetMeasuredGeometry(const TopoDS_Face& theFirstFace,
                                           const TopoDS_Face& theSecondFace);

  //! Measure distance between face and edge.
  //! The dimension will become invalid if the distance can not
  //! be measured or it is less than Precision::Confusion().
  //! @param[in] theFace  the face (first shape).
  //! @param[in] theEdge  the edge (second shape).
  Standard_EXPORT void SetMeasuredGeometry(const TopoDS_Face& theFace, const TopoDS_Edge& theEdge);

  //! Measure distance between generic pair of shapes (edges, vertices, length),
  //! where measuring is applicable.
  //! @param[in] theFirstShape  the first shape.
  //! @param[in] theSecondShape  the second shape.
  Standard_EXPORT void SetMeasuredShapes(const TopoDS_Shape& theFirstShape,
                                         const TopoDS_Shape& theSecondShape);

  //! @return the display units string.
  Standard_EXPORT const TCollection_AsciiString& GetDisplayUnits() const override;

  //! @return the model units string.
  Standard_EXPORT const TCollection_AsciiString& GetModelUnits() const override;

  Standard_EXPORT void SetDisplayUnits(const TCollection_AsciiString& theUnits) override;

  Standard_EXPORT void SetModelUnits(const TCollection_AsciiString& theUnits) override;

  Standard_EXPORT void SetTextPosition(const gp_Pnt& theTextPos) override;

  Standard_EXPORT gp_Pnt GetTextPosition() const override;

  //! Set custom direction for dimension. If it is not set, the direction is obtained
  //! from the measured geometry (e.g. line between points of dimension)
  //! The direction does not change flyout direction of dimension.
  //! @param[in] theDirection  the dimension direction.
  //! @param[in] theUseDirection  boolean value if custom direction should be used.
  Standard_EXPORT void SetDirection(const gp_Dir& theDirection, const bool theUseDirection = true);

protected:
  //! Checks if the plane includes first and second points to build dimension.
  Standard_EXPORT bool CheckPlane(const gp_Pln& thePlane) const override;

  Standard_EXPORT virtual gp_Pln ComputePlane(const gp_Dir& theAttachDir) const;

  //! Computes distance between dimension points. If custom direction is defined, the distance
  //! is a projection value of the distance between points to this direction
  //! @return dimension value
  Standard_EXPORT double ComputeValue() const override;

  Standard_EXPORT void Compute(
    const occ::handle<PrsMgr_PresentationManager>& thePresentationManager,
    const occ::handle<Prs3d_Presentation>&         thePresentation,
    const int                                      theMode = 0) override;

  //! Computes points bounded the flyout line for linear dimension.
  //! Direction of flyout line equal to the custom direction of dimension if defined or
  //! parallel to the main direction line
  //! @param[in] theFirstPoint  the first attach point of linear dimension.
  //! @param[in] theSecondPoint  the second attach point of linear dimension.
  //! @param[out] theLineBegPoint  the first attach point of linear dimension.
  //! @param[out] theLineEndPoint  the second attach point of linear dimension.
  Standard_EXPORT void ComputeFlyoutLinePoints(const gp_Pnt& theFirstPoint,
                                                       const gp_Pnt& theSecondPoint,
                                                       gp_Pnt&       theLineBegPoint,
                                                       gp_Pnt&       theLineEndPoint) override;

  Standard_EXPORT void ComputeFlyoutSelection(
    const occ::handle<SelectMgr_Selection>&   theSelection,
    const occ::handle<SelectMgr_EntityOwner>& theEntityOwner) override;

protected:
  //! Checks that distance between two points is valid.
  //! @param[in] theFirstPoint  the first point.
  //! @param[in] theSecondPoint  the second point.
  Standard_EXPORT bool IsValidPoints(const gp_Pnt& theFirstPoint,
                                     const gp_Pnt& theSecondPoint) const;

  Standard_EXPORT bool InitTwoEdgesLength(const TopoDS_Edge& theFirstEdge,
                                          const TopoDS_Edge& theSecondEdge,
                                          gp_Dir&            theEdgeDir);

  //! Auxiliary method for InitTwoShapesPoints()
  //! in case of the distance between edge and vertex.
  //! Finds the point on the edge that is the closest one to <theVertex>.
  //! @param[out] theEdgeDir  is the direction on the edge to build automatic plane.
  Standard_EXPORT bool InitEdgeVertexLength(const TopoDS_Edge&   theEdge,
                                            const TopoDS_Vertex& theVertex,
                                            gp_Dir&              theEdgeDir,
                                            bool                 isInfinite);

  //! Auxiliary method for InitTwoShapesPoints()
  //! in case of the distance between face and edge.
  //! The first attachment point is first parameter point from <theEdge>.
  //! Find the second attachment point which belongs to <theFace>
  //! Iterate over the edges of the face and find the closest point according
  //! to found point on edge.
  //! @param[out] theEdgeDir  is the direction on the edge to build automatic plane.
  Standard_EXPORT bool InitEdgeFaceLength(const TopoDS_Edge& theEdge,
                                          const TopoDS_Face& theFace,
                                          gp_Dir&            theEdgeDir);

  //! Initialization of two attach points in case of two owner shapes.
  Standard_EXPORT bool InitTwoShapesPoints(const TopoDS_Shape& theFirstShape,
                                           const TopoDS_Shape& theSecondShape,
                                           gp_Pln&             theComputedPlane,
                                           bool&               theIsPlaneComputed);

  //! Initialization of two attach points in case of one owner shape.
  Standard_EXPORT bool InitOneShapePoints(const TopoDS_Shape& theShape);

protected:
  gp_Pnt       myFirstPoint;
  gp_Pnt       mySecondPoint;
  TopoDS_Shape myFirstShape;
  TopoDS_Shape mySecondShape;
  gp_Dir       myDirection;
  bool         myHasCustomDirection;
};

#endif // _PrsDim_LengthDimension_HeaderFile
