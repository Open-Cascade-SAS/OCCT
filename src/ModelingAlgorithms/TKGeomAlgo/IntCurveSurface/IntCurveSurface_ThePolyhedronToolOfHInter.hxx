// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntCurveSurface_ThePolyhedronToolOfHInter_HeaderFile
#define _IntCurveSurface_ThePolyhedronToolOfHInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <gp_Pnt.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>

class IntCurveSurface_ThePolyhedronToolOfHInter
{
public:
  DEFINE_STANDARD_ALLOC

  //! Give the bounding box of the PolyhedronTool.
  static const Bnd_Box& Bounding(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh)
  {
    return thePolyh.Bounding();
  }

  //! Give the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  static const Handle(Bnd_HArray1OfBox)& ComponentsBounding(
    const IntCurveSurface_ThePolyhedronOfHInter& thePolyh)
  {
    return thePolyh.ComponentsBounding();
  }

  //! Give the tolerance of the polygon.
  static Standard_Real DeflectionOverEstimation(
    const IntCurveSurface_ThePolyhedronOfHInter& thePolyh)
  {
    return thePolyh.DeflectionOverEstimation();
  }

  //! Give the number of triangles in this polyhedral surface.
  static Standard_Integer NbTriangles(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh)
  {
    return thePolyh.NbTriangles();
  }

  //! Give the indices of the 3 points of the triangle of
  //! address Index in the PolyhedronTool.
  static void Triangle(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh,
                       const Standard_Integer                       Index,
                       Standard_Integer&                            P1,
                       Standard_Integer&                            P2,
                       Standard_Integer&                            P3)
  {
    thePolyh.Triangle(Index, P1, P2, P3);
  }

  //! Give the point of index i in the polyhedral surface.
  static const gp_Pnt& Point(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh,
                             const Standard_Integer                       Index)
  {
    return thePolyh.Point(Index);
  }

  //! Give the address Tricon of the triangle connexe to
  //! the triangle of address Triang by the edge Pivot Pedge
  //! and the third point of this connexe triangle. When we
  //! are on a free edge TriCon==0 but the function return
  //! the value of the triangle in the other side of Pivot
  //! on the free edge. Used to turn around a vertex.
  static Standard_Integer TriConnex(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh,
                                    const Standard_Integer                       Triang,
                                    const Standard_Integer                       Pivot,
                                    const Standard_Integer                       Pedge,
                                    Standard_Integer&                            TriCon,
                                    Standard_Integer&                            OtherP)
  {
    return thePolyh.TriConnex(Triang, Pivot, Pedge, TriCon, OtherP);
  }

  //! This method returns true if the edge based on points with
  //! indices Index1 and Index2 represents a boundary edge. It is
  //! necessary to take into account the boundary deflection for
  //! this edge.
  static Standard_Boolean IsOnBound(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh,
                                    const Standard_Integer                       Index1,
                                    const Standard_Integer                       Index2)
  {
    return thePolyh.IsOnBound(Index1, Index2);
  }

  //! This method returns a border deflection of the polyhedron.
  static Standard_Real GetBorderDeflection(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh)
  {
    return thePolyh.GetBorderDeflection();
  }

  Standard_EXPORT static void Dump(const IntCurveSurface_ThePolyhedronOfHInter& thePolyh);
};

#endif // _IntCurveSurface_ThePolyhedronToolOfHInter_HeaderFile
