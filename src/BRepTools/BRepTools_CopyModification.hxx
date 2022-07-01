// Copyright (c) 1999-2022 OPEN CASCADE SAS
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

#ifndef _BRepTools_CopyModification_HeaderFile
#define _BRepTools_CopyModification_HeaderFile

#include <BRepTools_Modification.hxx>

class BRepTools_CopyModification;
DEFINE_STANDARD_HANDLE(BRepTools_CopyModification, BRepTools_Modification)

//! Tool class implementing necessary functionality for copying geometry and triangulation.
class BRepTools_CopyModification : public BRepTools_Modification
{
public:
  //! Constructor.
  //! \param[in] theCopyGeom  indicates that the geomtery (surfaces and curves) should be copied
  //! \param[in] theCopyMesh  indicates that the triangulation should be copied
  Standard_EXPORT explicit BRepTools_CopyModification(const Standard_Boolean theCopyGeom = Standard_True,
                                                      const Standard_Boolean theCopyMesh = Standard_True);
  
  //! Returns true if theFace has been modified.
  //! If the face has been modified:
  //! - theSurf is the new geometry of the face,
  //! - theLoc is its new location, and
  //! - theTol is the new tolerance.
  //! theRevWires, theRevFace are always set to false, because the orientaion is not changed.
  Standard_EXPORT Standard_Boolean NewSurface(const TopoDS_Face&    theFace,
                                              Handle(Geom_Surface)& theSurf,
                                              TopLoc_Location&      theLoc,
                                              Standard_Real&        theTol,
                                              Standard_Boolean&     theRevWires,
                                              Standard_Boolean&     theRevFace) Standard_OVERRIDE;

  //! Returns true if theEdge has been modified.
  //! If the edge has been modified:
  //! - theCurve is the new geometric support of the edge,
  //! - theLoc is the new location, and
  //! - theTol is the new tolerance.
  //! If the edge has not been modified, this function
  //! returns false, and the values of theCurve, theLoc and theTol are not significant.
  Standard_EXPORT Standard_Boolean NewCurve(const TopoDS_Edge&  theEdge,
                                            Handle(Geom_Curve)& theCurve,
                                            TopLoc_Location&    theLoc,
                                            Standard_Real&      theTol) Standard_OVERRIDE;
  
  //! Returns true if theVertex has been modified.
  //! If the vertex has been modified:
  //! - thePnt is the new geometry of the vertex, and
  //! - theTol is the new tolerance.
  //! If the vertex has not been modified this function
  //! returns false, and the values of thePnt and theTol are not significant.
  Standard_EXPORT Standard_Boolean NewPoint(const TopoDS_Vertex& theVertex, gp_Pnt& thePnt, Standard_Real& theTol) Standard_OVERRIDE;
  
  //! Returns true if theEdge has a new curve on surface on theFace.
  //! If a new curve exists:
  //! - theCurve is the new geometric support of the edge,
  //! - theTol the new tolerance.
  //! If no new curve exists, this function returns false, and
  //! the values of theCurve and theTol are not significant.
  Standard_EXPORT Standard_Boolean NewCurve2d(const TopoDS_Edge&    theEdge,
                                              const TopoDS_Face&    theFace,
                                              const TopoDS_Edge&    theNewEdge,
                                              const TopoDS_Face&    theNewFace,
                                              Handle(Geom2d_Curve)& theCurve,
                                              Standard_Real&        theTol) Standard_OVERRIDE;
  
  //! Returns true if theVertex has a new parameter on theEdge.
  //! If a new parameter exists:
  //! - thePnt is the parameter, and
  //! - theTol is the new tolerance.
  //! If no new parameter exists, this function returns false,
  //! and the values of thePnt and theTol are not significant.
  Standard_EXPORT Standard_Boolean NewParameter(const TopoDS_Vertex& theVertex,
                                                const TopoDS_Edge&   theEdge,
                                                Standard_Real&       thePnt,
                                                Standard_Real&       theTol) Standard_OVERRIDE;
  
  //! Returns the continuity of theNewEdge between theNewFace1 and theNewFace2.
  //!
  //! theNewEdge is the new edge created from theEdge.  theNewFace1
  //! (resp. theNewFace2) is the new face created from theFace1 (resp. theFace2).
  Standard_EXPORT GeomAbs_Shape Continuity(const TopoDS_Edge& theEdge,
                                           const TopoDS_Face& theFace1,
                                           const TopoDS_Face& theFace2,
                                           const TopoDS_Edge& theNewEdge,
                                           const TopoDS_Face& theNewFace1,
                                           const TopoDS_Face& theNewFace2) Standard_OVERRIDE;

  //! Returns true if the face has been modified according to changed triangulation.
  //! If the face has been modified:
  //! - theTri is a new triangulation on the face
  Standard_EXPORT Standard_Boolean NewTriangulation(const TopoDS_Face& theFace, Handle(Poly_Triangulation)& theTri) Standard_OVERRIDE;

  //! Returns true if the edge has been modified according to changed polygon.
  //! If the edge has been modified:
  //! - thePoly is a new polygon
  Standard_EXPORT Standard_Boolean NewPolygon(const TopoDS_Edge& theEdge, Handle(Poly_Polygon3D)& thePoly) Standard_OVERRIDE;

  //! Returns true if the edge has been modified according to changed polygon on triangulation.
  //! If the edge has been modified:
  //! - thePoly is a new polygon on triangulation
  Standard_EXPORT Standard_Boolean NewPolygonOnTriangulation(const TopoDS_Edge&                   theEdge,
                                                             const TopoDS_Face&                   theFace,
                                                             Handle(Poly_PolygonOnTriangulation)& thePoly) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepTools_CopyModification, BRepTools_Modification)

private:
  Standard_Boolean myCopyGeom;
  Standard_Boolean myCopyMesh;
};

#endif // _BRepTools_CopyModification_HeaderFile
