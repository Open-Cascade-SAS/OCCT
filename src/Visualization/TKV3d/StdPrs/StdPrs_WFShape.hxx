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

#ifndef StdPrs_WFShape_HeaderFile
#define StdPrs_WFShape_HeaderFile

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopTools_ListOfShape.hxx>

class Graphic3d_ArrayOfPoints;

//! Tool for computing wireframe presentation of a TopoDS_Shape.
class StdPrs_WFShape : public Prs3d_Root
{
public:
  //! Computes wireframe presentation of a shape.
  //! @param[in] thePresentation  the presentation.
  //! @param[in] theShape  the shape.
  //! @param[in] theDrawer  the draw settings.
  //! @param[in] theIsParallel  perform algorithm using multiple threads
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& thePresentation,
                                  const TopoDS_Shape&               theShape,
                                  const Handle(Prs3d_Drawer)&       theDrawer,
                                  Standard_Boolean                  theIsParallel = Standard_False);

  //! Compute free and boundary edges on a triangulation of each face in the given shape.
  //! @param[in] theShape               the list of triangulated faces
  //! @param[in] theToExcludeGeometric  flag indicating that Faces with defined Surface should be
  //! skipped
  Standard_EXPORT static Handle(Graphic3d_ArrayOfPrimitives) AddEdgesOnTriangulation(
    const TopoDS_Shape&    theShape,
    const Standard_Boolean theToExcludeGeometric = Standard_True);

  //! Compute free and boundary edges on a triangulation of each face in the given shape.
  //! @param[in] theSegments            the sequence of points defining segments
  //! @param[in] theShape               the list of triangulated faces
  //! @param[in] theToExcludeGeometric  flag indicating that Faces with defined Surface should be
  //! skipped
  Standard_EXPORT static void AddEdgesOnTriangulation(
    TColgp_SequenceOfPnt&  theSegments,
    const TopoDS_Shape&    theShape,
    const Standard_Boolean theToExcludeGeometric = Standard_True);

  //! Compute all edges (wire, free, unfree) and put them into single primitive array.
  //! @param[in] theShape  the shape
  //! @param[in] theDrawer  the drawer settings (deviation angle and maximal parameter value)
  Standard_EXPORT static Handle(Graphic3d_ArrayOfPrimitives) AddAllEdges(
    const TopoDS_Shape&         theShape,
    const Handle(Prs3d_Drawer)& theDrawer);

  //! Compute vertex presentation for a shape.
  //! @param[in] theShape  the shape
  //! @param[in] theVertexMode  vertex filter
  Standard_EXPORT static Handle(Graphic3d_ArrayOfPoints) AddVertexes(
    const TopoDS_Shape&  theShape,
    Prs3d_VertexDrawMode theVertexMode);

private:
  //! Compute edge presentations for a shape.
  //! @param[in] theShape  the shape
  //! @param[in] theDrawer  the drawer settings (deviation angle and maximal parameter value)
  //! @param[in] theShapeDeflection  the deflection for the wireframe shape
  //! @param[out] theWire  output polylines for lonely wires
  //! @param[out] theFree  output polylines for free edges
  //! @param[out] theUnFree  output polylines for non-free edges
  Standard_EXPORT static void addEdges(const TopoDS_Shape&         theShape,
                                       const Handle(Prs3d_Drawer)& theDrawer,
                                       Standard_Real               theShapeDeflection,
                                       Prs3d_NListOfSequenceOfPnt* theWire,
                                       Prs3d_NListOfSequenceOfPnt* theFree,
                                       Prs3d_NListOfSequenceOfPnt* theUnFree);

  //! Compute edge presentations for a shape.
  //! @param[in] theEdges  the list of edges
  //! @param[in] theDrawer  the drawer settings (deviation angle and maximal parameter value)
  //! @param[in] theShapeDeflection  the deflection for the wireframe shape
  //! @param[out] thePolylines  output polylines
  static void addEdges(const TopTools_ListOfShape& theEdges,
                       const Handle(Prs3d_Drawer)& theDrawer,
                       const Standard_Real         theShapeDeflection,
                       Prs3d_NListOfSequenceOfPnt& thePolylines);
};

#endif // _StdPrs_WFShape_H__
