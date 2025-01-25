// Created on: 1994-08-25
// Created by: Jacques GOUSSARD
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

#ifndef _BRepTools_Modification_HeaderFile
#define _BRepTools_Modification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
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
class Poly_Triangulation;

class Poly_Polygon3D;
class Poly_PolygonOnTriangulation;

class BRepTools_Modification;
DEFINE_STANDARD_HANDLE(BRepTools_Modification, Standard_Transient)

//! Defines geometric modifications to a shape, i.e.
//! changes to faces, edges and vertices.
class BRepTools_Modification : public Standard_Transient
{

public:
  //! Returns true if the face, F, has been modified.
  //! If the face has been modified:
  //! - S is the new geometry of the face,
  //! - L is its new location, and
  //! - Tol is the new tolerance.
  //! The flag, RevWires, is set to true when the
  //! modification reverses the normal of the surface, (i.e.
  //! the wires have to be reversed).
  //! The flag, RevFace, is set to true if the orientation of
  //! the modified face changes in the shells which contain it.
  //! If the face has not been modified this function returns
  //! false, and the values of S, L, Tol, RevWires and
  //! RevFace are not significant.
  Standard_EXPORT virtual Standard_Boolean NewSurface(const TopoDS_Face&    F,
                                                      Handle(Geom_Surface)& S,
                                                      TopLoc_Location&      L,
                                                      Standard_Real&        Tol,
                                                      Standard_Boolean&     RevWires,
                                                      Standard_Boolean&     RevFace) = 0;

  //! Returns true if the face has been modified according to changed triangulation.
  //! If the face has been modified:
  //! - T is a new triangulation on the face
  Standard_EXPORT virtual Standard_Boolean NewTriangulation(const TopoDS_Face&          F,
                                                            Handle(Poly_Triangulation)& T);

  //! Returns true if the edge, E, has been modified.
  //! If the edge has been modified:
  //! - C is the new geometry associated with the edge,
  //! - L is its new location, and
  //! - Tol is the new tolerance.
  //! If the edge has not been modified, this function
  //! returns false, and the values of C, L and Tol are not significant.
  Standard_EXPORT virtual Standard_Boolean NewCurve(const TopoDS_Edge&  E,
                                                    Handle(Geom_Curve)& C,
                                                    TopLoc_Location&    L,
                                                    Standard_Real&      Tol) = 0;

  //! Returns true if the edge has been modified according to changed polygon.
  //! If the edge has been modified:
  //! - P is a new polygon
  Standard_EXPORT virtual Standard_Boolean NewPolygon(const TopoDS_Edge&      E,
                                                      Handle(Poly_Polygon3D)& P);

  //! Returns true if the edge has been modified according to changed polygon on triangulation.
  //! If the edge has been modified:
  //! - P is a new polygon on triangulation
  Standard_EXPORT virtual Standard_Boolean NewPolygonOnTriangulation(
    const TopoDS_Edge&                   E,
    const TopoDS_Face&                   F,
    Handle(Poly_PolygonOnTriangulation)& P);

  //! Returns true if the vertex V has been modified.
  //! If V has been modified:
  //! - P is the new geometry of the vertex, and
  //! - Tol is the new tolerance.
  //! If the vertex has not been modified this function
  //! returns false, and the values of P and Tol are not significant.
  Standard_EXPORT virtual Standard_Boolean NewPoint(const TopoDS_Vertex& V,
                                                    gp_Pnt&              P,
                                                    Standard_Real&       Tol) = 0;

  //! Returns true if the edge, E, has a new curve on
  //! surface on the face, F.
  //! If a new curve exists:
  //! - C is the new geometry of the edge,
  //! - L is the new location, and
  //! - Tol is the new tolerance.
  //! NewE is the new edge created from E, and NewF is
  //! the new face created from F.
  //! If there is no new curve on the face, this function
  //! returns false, and the values of C, L and Tol are not significant.
  Standard_EXPORT virtual Standard_Boolean NewCurve2d(const TopoDS_Edge&    E,
                                                      const TopoDS_Face&    F,
                                                      const TopoDS_Edge&    NewE,
                                                      const TopoDS_Face&    NewF,
                                                      Handle(Geom2d_Curve)& C,
                                                      Standard_Real&        Tol) = 0;

  //! Returns true if the vertex V has a new parameter on the edge E.
  //! If a new parameter exists:
  //! - P is the parameter, and
  //! - Tol is the new tolerance.
  //! If there is no new parameter this function returns
  //! false, and the values of P and Tol are not significant.
  Standard_EXPORT virtual Standard_Boolean NewParameter(const TopoDS_Vertex& V,
                                                        const TopoDS_Edge&   E,
                                                        Standard_Real&       P,
                                                        Standard_Real&       Tol) = 0;

  //! Returns the  continuity of  <NewE> between <NewF1>
  //! and <NewF2>.
  //! <NewE> is the new  edge created from <E>.  <NewF1>
  //! (resp. <NewF2>) is the new  face created from <F1>
  //! (resp. <F2>).
  Standard_EXPORT virtual GeomAbs_Shape Continuity(const TopoDS_Edge& E,
                                                   const TopoDS_Face& F1,
                                                   const TopoDS_Face& F2,
                                                   const TopoDS_Edge& NewE,
                                                   const TopoDS_Face& NewF1,
                                                   const TopoDS_Face& NewF2) = 0;

  DEFINE_STANDARD_RTTIEXT(BRepTools_Modification, Standard_Transient)

protected:
private:
};

#endif // _BRepTools_Modification_HeaderFile
