// Created on: 1996-07-12
// Created by: Stagiaire Mary FABIEN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepTools_NurbsConvertModification_HeaderFile
#define _BRepTools_NurbsConvertModification_HeaderFile

#include <Standard.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <BRepTools_CopyModification.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
class TopoDS_Face;
class Geom_Surface;
class TopLoc_Location;
class TopoDS_Edge;
class Geom_Curve;
class TopoDS_Vertex;
class gp_Pnt;
class Geom2d_Curve;


class BRepTools_NurbsConvertModification;
DEFINE_STANDARD_HANDLE(BRepTools_NurbsConvertModification, BRepTools_CopyModification)

//! Defines a modification of the  geometry by a  Trsf
//! from gp. All methods return True and transform the
//! geometry.
class BRepTools_NurbsConvertModification : public BRepTools_CopyModification
{

public:

  
  Standard_EXPORT BRepTools_NurbsConvertModification();
  
  //! Returns Standard_True  if  the face  <F> has  been
  //! modified.  In this  case, <S> is the new geometric
  //! support of  the  face, <L> the  new location,<Tol>
  //! the new  tolerance.<RevWires> has  to  be set   to
  //! Standard_True   when the modification reverses the
  //! normal of  the   surface.(the wires   have  to  be
  //! reversed).   <RevFace>   has   to   be   set    to
  //! Standard_True if  the orientation  of the modified
  //! face changes in the  shells which contain  it.  --
  //! Here, <RevFace>  will  return Standard_True if the
  //! -- gp_Trsf is negative.
  Standard_EXPORT Standard_Boolean NewSurface (const TopoDS_Face& F, Handle(Geom_Surface)& S, TopLoc_Location& L, Standard_Real& Tol, Standard_Boolean& RevWires, Standard_Boolean& RevFace) Standard_OVERRIDE;
  
  //! Returns Standard_True  if  the edge  <E> has  been
  //! modified.  In this case,  <C> is the new geometric
  //! support of the  edge, <L> the  new location, <Tol>
  //! the         new    tolerance.   Otherwise, returns
  //! Standard_False,    and  <C>,  <L>,   <Tol> are not
  //! significant.
  Standard_EXPORT Standard_Boolean NewCurve (const TopoDS_Edge& E, Handle(Geom_Curve)& C, TopLoc_Location& L, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns  Standard_True if the  vertex <V> has been
  //! modified.  In this  case, <P> is the new geometric
  //! support of the vertex,   <Tol> the new  tolerance.
  //! Otherwise, returns Standard_False, and <P>,  <Tol>
  //! are not significant.
  Standard_EXPORT Standard_Boolean NewPoint (const TopoDS_Vertex& V, gp_Pnt& P, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns Standard_True if  the edge  <E> has a  new
  //! curve on surface on the face <F>.In this case, <C>
  //! is the new geometric support of  the edge, <L> the
  //! new location, <Tol> the new tolerance.
  //! Otherwise, returns  Standard_False, and <C>,  <L>,
  //! <Tol> are not significant.
  Standard_EXPORT Standard_Boolean NewCurve2d (const TopoDS_Edge& E, const TopoDS_Face& F, const TopoDS_Edge& NewE, const TopoDS_Face& NewF, Handle(Geom2d_Curve)& C, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns Standard_True if the Vertex  <V> has a new
  //! parameter on the  edge <E>. In  this case,  <P> is
  //! the parameter,    <Tol>  the     new    tolerance.
  //! Otherwise, returns Standard_False, and <P>,  <Tol>
  //! are not significant.
  Standard_EXPORT Standard_Boolean NewParameter (const TopoDS_Vertex& V, const TopoDS_Edge& E, Standard_Real& P, Standard_Real& Tol) Standard_OVERRIDE;
  
  //! Returns the  continuity of  <NewE> between <NewF1>
  //! and <NewF2>.
  //!
  //! <NewE> is the new  edge created from <E>.  <NewF1>
  //! (resp. <NewF2>) is the new  face created from <F1>
  //! (resp. <F2>).
  Standard_EXPORT GeomAbs_Shape Continuity (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2, const TopoDS_Edge& NewE, const TopoDS_Face& NewF1, const TopoDS_Face& NewF2) Standard_OVERRIDE;

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

  Standard_EXPORT const TopTools_ListOfShape& GetUpdatedEdges() const;


  DEFINE_STANDARD_RTTIEXT(BRepTools_NurbsConvertModification,BRepTools_CopyModification)

protected:




private:


  TopTools_ListOfShape myled;
  TColStd_ListOfTransient mylcu;
  TColStd_IndexedDataMapOfTransientTransient myMap;
  TopTools_ListOfShape myUpdatedEdges;


};







#endif // _BRepTools_NurbsConvertModification_HeaderFile
