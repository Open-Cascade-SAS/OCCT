// Created on: 2014-08-13
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_EdgeTessellator_HeaderFile
#define _BRepMesh_EdgeTessellator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <BRepMesh.hxx>
#include <BRepMesh_IEdgeTool.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

class Adaptor3d_Surface;
class TopoDS_Edge;
class BRepAdaptor_HSurface;

//! Auxiliary class implements functionality producing tessellated
//! representation of an edge based on edge geometry.
class BRepMesh_EdgeTessellator : public BRepMesh_IEdgeTool
{
public:

  //! Constructor.
  //! Automatically performs tessellation of the edge according to the
  //! given parameters.
  BRepMesh_EdgeTessellator(
    const TopoDS_Edge&                               theEdge,
    const Handle(BRepMesh_FaceAttribute)&            theFaceAttribute,
    const TopTools_IndexedDataMapOfShapeListOfShape& theMapOfSharedFaces,
    const Standard_Real                              theLinDeflection,
    const Standard_Real                              theAngDeflection,
    const Standard_Real                              theMinSize);

  //! Returns number of dicretization points.
  virtual Standard_Integer NbPoints() const Standard_OVERRIDE
  {
    return myTool->NbPoints();
  }

  //! Returns parameters of solution with the given index.
  //! @param theIndex index of tessellation point.
  //! @param theParameter parameters on PCurve corresponded to the solution.
  //! @param thePoint tessellation point.
  //! @param theUV coordinates of tessellation point in parametric space of face.
  //! @return True in case of valid result, false elewhere.
  virtual Standard_Boolean Value(
    const Standard_Integer theIndex,
    Standard_Real&         theParameter,
    gp_Pnt&                thePoint,
    gp_Pnt2d&              theUV) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_EdgeTessellator,BRepMesh_IEdgeTool)

private:

  //! 
  void splitSegment(const Adaptor3d_Surface&    theSurf,
                    const Geom2dAdaptor_Curve&  theCurve2d,
                    const Standard_Real         theFirst,
                    const Standard_Real         theLast,
                    const Standard_Integer      theNbIter);

private:
  NCollection_Handle<BRepMesh_GeomTool> myTool;
  Handle(BRepAdaptor_HSurface)          mySurface;
  BRepAdaptor_Curve                     myCOnS;
  Geom2dAdaptor_Curve                   myCurve2d;
  Standard_Real                         mySquareEdgeDef;
  Standard_Real                         mySquareMinSize;
  Standard_Real                         myEdgeSqTol;
  Standard_Real                         myFaceRangeU[2];
  Standard_Real                         myFaceRangeV[2];
};

DEFINE_STANDARD_HANDLE(BRepMesh_EdgeTessellator, BRepMesh_IEdgeTool)

#endif
