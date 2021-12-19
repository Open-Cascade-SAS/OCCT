// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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

#ifndef _Poly_HeaderFile
#define _Poly_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Poly_ListOfTriangulation.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Real.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

class Poly_Triangulation;
class Poly_Polygon3D;
class Poly_Polygon2D;
class Poly_Triangle;


//! This  package  provides  classes  and services  to
//! handle :
//!
//! * 3D triangular polyhedrons.
//!
//! * 3D polygons.
//!
//! * 2D polygon.
//!
//! * Tools to dump, save and restore those objects.
class Poly 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes and  stores  the    link from   nodes  to
  //! triangles     and from triangles to   neighbouring
  //! triangles.
  //! This tool is obsolete, replaced by Poly_CoherentTriangulation
  //! Algorithm to make minimal loops in a graph
  //! Join several triangulations to one new triangulation object.
  //! The new triangulation is just a mechanical sum of input
  //! triangulations, without node sharing. UV coordinates are
  //! dropped in the result.
  Standard_EXPORT static Handle(Poly_Triangulation) Catenate (const Poly_ListOfTriangulation& lstTri);
  
  //! Writes the content of the triangulation <T> on the
  //! stream <OS>. If <Compact> is true this is a "save"
  //! format  intended  to  be read back   with the Read
  //! method. If compact is False  it is a "Dump" format
  //! intended to be informative.
  Standard_EXPORT static void Write (const Handle(Poly_Triangulation)& T, Standard_OStream& OS, const Standard_Boolean Compact = Standard_True);
  
  //! Writes  the  content  of the 3D polygon <P> on the
  //! stream <OS>. If <Compact> is true this is a "save"
  //! format  intended  to  be read back   with the Read
  //! method. If compact is False  it is a "Dump" format
  //! intended to be informative.
  Standard_EXPORT static void Write (const Handle(Poly_Polygon3D)& P, Standard_OStream& OS, const Standard_Boolean Compact = Standard_True);
  
  //! Writes the  content  of the 2D polygon  <P> on the
  //! stream <OS>. If <Compact> is true this is a "save"
  //! format  intended  to  be read back   with the Read
  //! method. If compact is False  it is a "Dump" format
  //! intended to be informative.
  Standard_EXPORT static void Write (const Handle(Poly_Polygon2D)& P, Standard_OStream& OS, const Standard_Boolean Compact = Standard_True);
  
  //! Dumps  the triangulation.  This   is a call to  the
  //! previous method with Comapct set to False.
  Standard_EXPORT static void Dump (const Handle(Poly_Triangulation)& T, Standard_OStream& OS);
  
  //! Dumps  the  3D  polygon.  This   is a call to  the
  //! previous method with Comapct set to False.
  Standard_EXPORT static void Dump (const Handle(Poly_Polygon3D)& P, Standard_OStream& OS);
  
  //! Dumps  the  2D  polygon.  This   is a call to  the
  //! previous method with Comapct set to False.
  Standard_EXPORT static void Dump (const Handle(Poly_Polygon2D)& P, Standard_OStream& OS);
  
  //! Reads a triangulation from the stream <IS>.
  Standard_EXPORT static Handle(Poly_Triangulation) ReadTriangulation (Standard_IStream& IS);
  
  //! Reads a 3d polygon from the stream <IS>.
  Standard_EXPORT static Handle(Poly_Polygon3D) ReadPolygon3D (Standard_IStream& IS);
  
  //! Reads a 2D polygon from the stream <IS>.
  Standard_EXPORT static Handle(Poly_Polygon2D) ReadPolygon2D (Standard_IStream& IS);
  
  //! Compute node normals for face triangulation
  //! as mean normal of surrounding triangles
  Standard_EXPORT static void ComputeNormals (const Handle(Poly_Triangulation)& Tri);
  
  //! Computes parameters of the point P on triangle
  //! defined by points P1, P2, and P3, in 2d.
  //! The parameters U and V are defined so that
  //! P = P1 + U * (P2 - P1) + V * (P3 - P1),
  //! with U >= 0, V >= 0, U + V <= 1.
  //! If P is located outside of triangle, or triangle
  //! is degenerated, the returned parameters correspond
  //! to closest point, and returned value is square of
  //! the distance from original point to triangle (0 if
  //! point is inside).
  Standard_EXPORT static Standard_Real PointOnTriangle (const gp_XY& P1, const gp_XY& P2, const gp_XY& P3, const gp_XY& P, gp_XY& UV);

  //! Computes the intersection between axis and triangulation.
  //! @param theTri  [in] input triangulation
  //! @param theAxis [in] intersecting ray
  //! @param theIsClosest [in] finds the closest intersection when TRUE, finds the farthest otherwise
  //! @param theTriangle [out] intersected triangle
  //! @param theDistance [out] distance along ray to intersection point
  //! @return TRUE if intersection takes place, FALSE otherwise.
  Standard_EXPORT static Standard_Boolean Intersect (const Handle(Poly_Triangulation)& theTri,
                                                     const gp_Ax1& theAxis,
                                                     const Standard_Boolean theIsClosest,
                                                     Poly_Triangle& theTriangle,
                                                     Standard_Real& theDistance);

  //! Computes the intersection between a triangle defined by three vertexes and a line.
  //! @param theStart [in] picking ray origin
  //! @param theDir   [in] picking ray direction
  //! @param theV0    [in] first triangle node
  //! @param theV1    [in] second triangle node
  //! @param theV2    [in] third triangle node
  //! @param theParam [out] param on line of the intersection point
  //! @return 1 if intersection was found, 0 otherwise.
  Standard_EXPORT static Standard_Integer IntersectTriLine  (const gp_XYZ& theStart,
                                                             const gp_Dir& theDir,
                                                             const gp_XYZ& theV0,
                                                             const gp_XYZ& theV1,
                                                             const gp_XYZ& theV2,
                                                             Standard_Real& theParam);

  //! Returns area and perimeter of 2D-polygon given by its vertices.
  //! theArea will be negative if the polygon is bypassed clockwise
  //! and will be positive, otherwise. thePerimeter will always be positive.
  //!
  //! ATTENTION!!!
  //! The container theSeqPnts of 2D-points gp_Pnt2d must have definition
  //! for following methods: Length(), Lower(), Upper() and Value(Standard_Integer)
  //! (e.g. it can be either NCollection_Sequence<gp_Pnt2d> or
  //! NCollection_Array1<gp_Pnt2d>).
  template <class TypeSequencePnts>
  Standard_EXPORT static
    Standard_Boolean PolygonProperties(const TypeSequencePnts& theSeqPnts,
                                       Standard_Real& theArea,
                                       Standard_Real& thePerimeter)
  {
    if (theSeqPnts.Length() < 2)
    {
      theArea = thePerimeter = 0.0;
      return Standard_True;
    }

    Standard_Integer aStartIndex = theSeqPnts.Lower();
    const gp_XY &aRefPnt = theSeqPnts.Value(aStartIndex++).XY();
    gp_XY aPrevPt = theSeqPnts.Value(aStartIndex++).XY() - aRefPnt, aCurrPt;

    theArea = 0.0;
    thePerimeter = aPrevPt.Modulus();

    for (Standard_Integer i = aStartIndex; i <= theSeqPnts.Upper(); i++)
    {
      aCurrPt = theSeqPnts.Value(i).XY() - aRefPnt;
      const Standard_Real aDelta = aPrevPt.Crossed(aCurrPt);

      theArea += aDelta;
      thePerimeter += (aPrevPt - aCurrPt).Modulus();
      aPrevPt = aCurrPt;
    }

    thePerimeter += aPrevPt.Modulus();
    theArea *= 0.5;
    return Standard_True;
  }

};

#endif // _Poly_HeaderFile
