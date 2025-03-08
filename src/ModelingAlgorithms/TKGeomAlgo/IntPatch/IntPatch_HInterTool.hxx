// Created on: 1995-07-02
// Created by: Laurent BUCHARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _IntPatch_HInterTool_HeaderFile
#define _IntPatch_HInterTool_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>

class gp_Pnt2d;
class Adaptor3d_HVertex;
class gp_Pnt;

//! Tool for the intersection between 2 surfaces.
//! Regroupe pour l instant les methodes hors Adaptor3d...
class IntPatch_HInterTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_HInterTool();

  static Standard_Boolean SingularOnUMin(const Handle(Adaptor3d_Surface)& S);

  static Standard_Boolean SingularOnUMax(const Handle(Adaptor3d_Surface)& S);

  static Standard_Boolean SingularOnVMin(const Handle(Adaptor3d_Surface)& S);

  static Standard_Boolean SingularOnVMax(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT static Standard_Integer NbSamplesU(const Handle(Adaptor3d_Surface)& S,
                                                     const Standard_Real              u1,
                                                     const Standard_Real              u2);

  Standard_EXPORT static Standard_Integer NbSamplesV(const Handle(Adaptor3d_Surface)& S,
                                                     const Standard_Real              v1,
                                                     const Standard_Real              v2);

  Standard_EXPORT Standard_Integer NbSamplePoints(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT void SamplePoint(const Handle(Adaptor3d_Surface)& S,
                                   const Standard_Integer           Index,
                                   Standard_Real&                   U,
                                   Standard_Real&                   V) const;

  //! Returns True if all the intersection point and edges
  //! are known on the Arc.
  //! The intersection point are given as vertices.
  //! The intersection edges are given as intervals between
  //! two vertices.
  Standard_EXPORT static Standard_Boolean HasBeenSeen(const Handle(Adaptor2d_Curve2d)& C);

  //! returns the number of points which is used to make
  //! a sample on the arc. this number is a function of
  //! the Surface and the CurveOnSurface complexity.
  Standard_EXPORT static Standard_Integer NbSamplesOnArc(const Handle(Adaptor2d_Curve2d)& A);

  //! Returns the parametric limits on the arc C.
  //! These limits must be finite : they are either
  //! the real limits of the arc, for a finite arc,
  //! or a bounding box for an infinite arc.
  Standard_EXPORT static void Bounds(const Handle(Adaptor2d_Curve2d)& C,
                                     Standard_Real&                   Ufirst,
                                     Standard_Real&                   Ulast);

  //! Projects the point P on the arc C.
  //! If the methods returns Standard_True, the projection is
  //! successful, and Paramproj is the parameter on the arc
  //! of the projected point, Ptproj is the projected Point.
  //! If the method returns Standard_False, Param proj and Ptproj
  //! are not significant.
  Standard_EXPORT static Standard_Boolean Project(const Handle(Adaptor2d_Curve2d)& C,
                                                  const gp_Pnt2d&                  P,
                                                  Standard_Real&                   Paramproj,
                                                  gp_Pnt2d&                        Ptproj);

  //! Returns the parametric tolerance used to consider
  //! that the vertex and another point meet, i-e
  //! if Abs(parameter(Vertex) - parameter(OtherPnt))<=
  //! Tolerance, the points are "merged".
  Standard_EXPORT static Standard_Real Tolerance(const Handle(Adaptor3d_HVertex)& V,
                                                 const Handle(Adaptor2d_Curve2d)& C);

  //! Returns the parameter of the vertex V on the arc A.
  Standard_EXPORT static Standard_Real Parameter(const Handle(Adaptor3d_HVertex)& V,
                                                 const Handle(Adaptor2d_Curve2d)& C);

  //! Returns the number of intersection points on the arc A.
  Standard_EXPORT static Standard_Integer NbPoints(const Handle(Adaptor2d_Curve2d)& C);

  //! Returns the value (Pt), the tolerance (Tol), and
  //! the parameter (U) on the arc A , of the intersection
  //! point of range Index.
  Standard_EXPORT static void Value(const Handle(Adaptor2d_Curve2d)& C,
                                    const Standard_Integer           Index,
                                    gp_Pnt&                          Pt,
                                    Standard_Real&                   Tol,
                                    Standard_Real&                   U);

  //! Returns True if the intersection point of range Index
  //! corresponds with a vertex on the arc A.
  Standard_EXPORT static Standard_Boolean IsVertex(const Handle(Adaptor2d_Curve2d)& C,
                                                   const Standard_Integer           Index);

  //! When IsVertex returns True, this method returns the
  //! vertex on the arc A.
  Standard_EXPORT static void Vertex(const Handle(Adaptor2d_Curve2d)& C,
                                     const Standard_Integer           Index,
                                     Handle(Adaptor3d_HVertex)&       V);

  //! returns the number of part of A solution of the
  //! of intersection problem.
  Standard_EXPORT static Standard_Integer NbSegments(const Handle(Adaptor2d_Curve2d)& C);

  //! Returns True when the segment of range Index is not
  //! open at the left side. In that case, IndFirst is the
  //! range in the list intersection points (see NbPoints)
  //! of the one which defines the left bound of the segment.
  //! Otherwise, the method has to return False, and IndFirst
  //! has no meaning.
  Standard_EXPORT static Standard_Boolean HasFirstPoint(const Handle(Adaptor2d_Curve2d)& C,
                                                        const Standard_Integer           Index,
                                                        Standard_Integer&                IndFirst);

  //! Returns True when the segment of range Index is not
  //! open at the right side. In that case, IndLast is the
  //! range in the list intersection points (see NbPoints)
  //! of the one which defines the right bound of the segment.
  //! Otherwise, the method has to return False, and IndLast
  //! has no meaning.
  Standard_EXPORT static Standard_Boolean HasLastPoint(const Handle(Adaptor2d_Curve2d)& C,
                                                       const Standard_Integer           Index,
                                                       Standard_Integer&                IndLast);

  //! Returns True when the whole restriction is solution
  //! of the intersection problem.
  Standard_EXPORT static Standard_Boolean IsAllSolution(const Handle(Adaptor2d_Curve2d)& C);

protected:
private:
  Standard_Real uinf;
  Standard_Real vinf;
  Standard_Real usup;
  Standard_Real vsup;
};

#include <IntPatch_HInterTool.lxx>

#endif // _IntPatch_HInterTool_HeaderFile
