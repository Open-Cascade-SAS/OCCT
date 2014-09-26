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

#ifndef _BRepMesh_EdgeTessellationExtractor_HeaderFile
#define _BRepMesh_EdgeTessellationExtractor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <BRepMesh_IEdgeTool.hxx>
#include <BRepMesh_EdgeParameterProvider.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <Handle_Poly_PolygonOnTriangulation.hxx>
#include <Handle_Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>

class Poly_Triangulation;
class Poly_PolygonOnTriangulation;
class TopoDS_Edge;
class TopoDS_Face;

//! Auxiliary class implements functionality retrieving tessellated
//! representation of an edge stored in polygon.
class BRepMesh_EdgeTessellationExtractor : public BRepMesh_IEdgeTool
{
public:

  //! Constructor.
  //! Initializes extractor.
  BRepMesh_EdgeTessellationExtractor(
    const TopoDS_Edge&                          theEdge,
    const Handle(Geom2d_Curve)&                 thePCurve,
    const TopoDS_Face&                          theFace,
    const Handle(Poly_Triangulation)&           theTriangulation,
    const Handle(Poly_PolygonOnTriangulation)&  thePolygon,
    const TopLoc_Location&                      theLocation);

  //! Returns number of dicretization points.
  virtual Standard_Integer NbPoints() const
  {
    return myIndices.Length();
  }

  //! Returns parameters of solution with the given index.
  //! @param theIndex index of tessellation point.
  //! @param theParameter parameters on PCurve corresponded to the solution.
  //! @param thePoint tessellation point.
  //! @param theUV coordinates of tessellation point in parametric space of face.
  virtual void Value(const Standard_Integer theIndex,
                     Standard_Real&         theParameter,
                     gp_Pnt&                thePoint,
                     gp_Pnt2d&              theUV);

  DEFINE_STANDARD_RTTI(BRepMesh_EdgeTessellationExtractor)

private:

  //! Assignment operator.
  void operator =(const BRepMesh_EdgeTessellationExtractor& /*theOther*/)
  {
  }

private:

  BRepMesh_EdgeParameterProvider myProvider;
  const Handle(Geom2d_Curve)&    myPCurve;
  const TColgp_Array1OfPnt&      myNodes;
  const TColStd_Array1OfInteger& myIndices;
  const TopLoc_Location          myLoc;
};

DEFINE_STANDARD_HANDLE(BRepMesh_EdgeTessellationExtractor, BRepMesh_IEdgeTool)

#endif
