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

#ifndef _AIS_LengthDimension_HeaderFile
#define _AIS_LengthDimension_HeaderFile

#include <AIS.hxx>
#include <AIS_Dimension.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_KindOfDimension.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_ExtendedString.hxx>

class TopoDS_Face;
class TCollection_ExtendedString;
class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Shape;
class Geom_Plane;
class PrsMgr_PresentationManager3d;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class SelectMgr_Selection;
class AIS_Drawer;
class Bnd_Box;
class gp_Dir;
class TopoDS_Vertex;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(AIS_Relation);
class AIS_LengthDimension;

DEFINE_STANDARD_HANDLE(AIS_LengthDimension,AIS_Dimension)

//! A dimention  to display lengths. <br>
//! These can be lengths along a face or edge, or <br>
//! between two faces or two edges.
class AIS_LengthDimension : public AIS_Dimension
{
public:

  Standard_EXPORT  AIS_LengthDimension (const gp_Pnt& theFirstPoint,
                                        const gp_Pnt& theSecondPoint,
                                        const gp_Pln& theDimensionPlane);

  //! Constructs a length dimension between two shapes(vertices, edges, shapes) with custom working plane
  Standard_EXPORT AIS_LengthDimension (const TopoDS_Shape& theFirstShape,
                                       const TopoDS_Shape& theSecondShape,
                                       const gp_Pln& theWorkingPlane);

  Standard_EXPORT  AIS_LengthDimension (const TopoDS_Edge& theEdge,
                                        const gp_Pln& theWorkingPlane);

  Standard_EXPORT AIS_LengthDimension (const TopoDS_Face& theFirstFace,
                                       const TopoDS_Face& theSecondFace);

  Standard_EXPORT AIS_LengthDimension (const TopoDS_Face& theFace,
                                       const TopoDS_Edge& theEdge);

public:

  DEFINE_STANDARD_RTTI(AIS_LengthDimension)

private:

  Standard_Boolean initTwoEdgesLength (const TopoDS_Edge & theFirstEdge,
                                       const TopoDS_Edge& theSecondEdge,
                                       gp_Dir& theDirAttach);

  //! Auxiliary method for <InitTwoShapesPoints()>
  //! in case of the distance between edge and vertex
  Standard_Boolean initEdgeVertexLength (const TopoDS_Edge & theEdge,
                                         const TopoDS_Vertex & theVertex,
                                         gp_Dir & theDirAttach,
                                         Standard_Boolean isInfinite);

  //! Auxiliary method for <InitTwoShapesPoints()>
  //! in case of the distance between face and edge
  Standard_Boolean initEdgeFaceLength (const TopoDS_Edge& theEdge,
                                       const TopoDS_Face& theFace,
                                       gp_Dir& theDirAttach);

  //! Initialization of two attach points in case of two owner shapes
  Standard_Boolean initTwoShapesPoints (const TopoDS_Shape& theFirstShape,
                                        const TopoDS_Shape& theSecondShape);

  //! Initialization of two attach points in case of one owner shape
  Standard_Boolean initOneShapePoints (const TopoDS_Shape& theShape);

  //! Compute length in display units.
  virtual void computeValue();

  virtual  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                         const Handle(Prs3d_Presentation)& thePresentation,
                         const Standard_Integer theMode = 0) ;
};

#endif
