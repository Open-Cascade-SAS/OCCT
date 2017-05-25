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

#ifndef _StdPrs_WFShape_H__
#define _StdPrs_WFShape_H__

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

//! Tool for computing wireframe presentation of a TopoDS_Shape.
class StdPrs_WFShape : public Prs3d_Root
{
public:

  //! Computes wireframe presentation of a shape.
  //! @param thePresentation [in] the presentation.
  //! @param theShape [in] the shape.
  //! @param theDrawer [in] the draw settings.
  //! @param theIsParallel [in] perform algorithm using multiple threads
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& thePresentation,
                                   const TopoDS_Shape& theShape,
                                   const Handle(Prs3d_Drawer)& theDrawer,
                                   Standard_Boolean theIsParallel = Standard_False);

  //! Compute free and boundary edges on a triangulation of each face in the given shape.
  //! @param theShape              [in] the list of triangulated faces
  //! @param theToExcludeGeometric [in] flag indicating that Faces with defined Surface should be skipped
  Standard_EXPORT static Handle(Graphic3d_ArrayOfPrimitives) AddEdgesOnTriangulation (const TopoDS_Shape&    theShape,
                                                                                      const Standard_Boolean theToExcludeGeometric = Standard_True);

  //! Compute free and boundary edges on a triangulation of each face in the given shape.
  //! @param theSegments           [in] the sequence of points defining segments
  //! @param theShape              [in] the list of triangulated faces
  //! @param theToExcludeGeometric [in] flag indicating that Faces with defined Surface should be skipped
  Standard_EXPORT static void AddEdgesOnTriangulation (TColgp_SequenceOfPnt&  theSegments,
                                                       const TopoDS_Shape&    theShape,
                                                       const Standard_Boolean theToExcludeGeometric = Standard_True);

private:

  //! Compute edge presentations for a shape.
  //! @param theEdges [in] the list of edges.
  //! @param theDrawer [in] the drawer settings.
  //! @param theShapeDeflection [in] the deflection for the wireframe shape.
  static void addEdges (const TopTools_ListOfShape& theEdges,
                        const Handle(Prs3d_Drawer)& theDrawer,
                        const Standard_Real         theShapeDeflection,
                        Prs3d_NListOfSequenceOfPnt& thePolylines);

  //! Compute vertex presentation for a shape.
  //! @param thePresentation [in] the presentation.
  //! @param theVertices [in] the list of points.
  //! @param theAspect [in] the point drawing aspect.
  static void addVertices (const Handle (Prs3d_Presentation)& thePresentation,
                           const TColgp_SequenceOfPnt&        theVertices,
                           const Handle (Prs3d_PointAspect)&  theAspect);
};

#endif // _StdPrs_WFShape_H__
