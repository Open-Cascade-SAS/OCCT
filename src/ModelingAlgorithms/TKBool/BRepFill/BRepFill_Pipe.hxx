// Created on: 1994-06-07
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepFill_Pipe_HeaderFile
#define _BRepFill_Pipe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepFill_DataMapOfShapeHArray2OfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomFill_Trihedron.hxx>
class BRepFill_LocationLaw;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Vertex;
class gp_Pnt;
class BRepFill_Sweep;

//! Create a  shape by sweeping a shape  (the profile)
//! along a wire (the spine).
//!
//! For each edge  or vertex from the spine  the  user
//! may ask for the shape generated from each subshape
//! of the profile.
class BRepFill_Pipe
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_Pipe();

  Standard_EXPORT BRepFill_Pipe(const TopoDS_Wire&       Spine,
                                const TopoDS_Shape&      Profile,
                                const GeomFill_Trihedron aMode         = GeomFill_IsCorrectedFrenet,
                                const Standard_Boolean   ForceApproxC1 = Standard_False,
                                const Standard_Boolean   GeneratePartCase = Standard_False);

  Standard_EXPORT void Perform(const TopoDS_Wire&     Spine,
                               const TopoDS_Shape&    Profile,
                               const Standard_Boolean GeneratePartCase = Standard_False);

  Standard_EXPORT const TopoDS_Shape& Spine() const;

  Standard_EXPORT const TopoDS_Shape& Profile() const;

  Standard_EXPORT const TopoDS_Shape& Shape() const;

  Standard_EXPORT Standard_Real ErrorOnSurface() const;

  Standard_EXPORT const TopoDS_Shape& FirstShape() const;

  Standard_EXPORT const TopoDS_Shape& LastShape() const;

  //! Returns the  list   of shapes generated   from the
  //! shape <S>.
  Standard_EXPORT void Generated(const TopoDS_Shape& S, TopTools_ListOfShape& L);

  //! Returns the face created from an edge of the spine
  //! and an edge of the profile.
  //! if the edges are not in the spine or the profile
  Standard_EXPORT TopoDS_Face Face(const TopoDS_Edge& ESpine, const TopoDS_Edge& EProfile);

  //! Returns the edge created from an edge of the spine
  //! and a vertex of the profile.
  //! if the edge or the vertex are not in  the spine or
  //! the profile.
  Standard_EXPORT TopoDS_Edge Edge(const TopoDS_Edge& ESpine, const TopoDS_Vertex& VProfile);

  //! Returns  the shape created from the profile at the
  //! position of the vertex VSpine.
  //! if the vertex is not in the Spine
  Standard_EXPORT TopoDS_Shape Section(const TopoDS_Vertex& VSpine) const;

  //! Create a Wire by sweeping the Point along the <spine>
  //! if the <Spine> is undefined
  Standard_EXPORT TopoDS_Wire PipeLine(const gp_Pnt& Point);

protected:
private:
  //! Auxiliary  recursive  method  used  to  build  the
  //! result.
  Standard_EXPORT TopoDS_Shape MakeShape(const TopoDS_Shape& S,
                                         const TopoDS_Shape& theOriginalS,
                                         const TopoDS_Shape& FirstShape,
                                         const TopoDS_Shape& LastShape);

  //! Auxiliary recursive method used to find the edge's index
  Standard_EXPORT Standard_Integer FindEdge(const TopoDS_Shape& S,
                                            const TopoDS_Edge&  E,
                                            Standard_Integer&   Init) const;

  Standard_EXPORT Standard_Integer FindVertex(const TopoDS_Shape&  S,
                                              const TopoDS_Vertex& V,
                                              Standard_Integer&    Init) const;

  Standard_EXPORT void DefineRealSegmax();

  Standard_EXPORT void RebuildTopOrBottomFace(const TopoDS_Shape&    aFace,
                                              const Standard_Boolean IsTop) const;

  Standard_EXPORT void BuildHistory(const BRepFill_Sweep& theSweep, const TopoDS_Shape& theSection);

  TopoDS_Wire                           mySpine;
  TopoDS_Shape                          myProfile;
  TopoDS_Shape                          myShape;
  gp_Trsf                               myTrsf;
  Handle(BRepFill_LocationLaw)          myLoc;
  Handle(TopTools_HArray2OfShape)       mySections;
  Handle(TopTools_HArray2OfShape)       myFaces;
  Handle(TopTools_HArray2OfShape)       myEdges;
  TopTools_MapOfShape                   myReversedEdges;
  BRepFill_DataMapOfShapeHArray2OfShape myTapes;
  BRepFill_DataMapOfShapeHArray2OfShape myRails;
  Standard_Integer                      myCurIndexOfSectionEdge;
  TopoDS_Shape                          myFirst;
  TopoDS_Shape                          myLast;
  TopTools_DataMapOfShapeListOfShape    myGenMap;
  Standard_Integer                      myDegmax;
  Standard_Integer                      mySegmax;
  GeomAbs_Shape                         myContinuity;
  GeomFill_Trihedron                    myMode;
  Standard_Boolean                      myForceApproxC1;
  Standard_Real                         myErrorOnSurf;
};

#endif // _BRepFill_Pipe_HeaderFile
