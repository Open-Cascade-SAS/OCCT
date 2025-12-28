// Created on: 1993-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_PolyhedronTool_HeaderFile
#define _IntPatch_PolyhedronTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class Bnd_Box;
class IntPatch_Polyhedron;
class gp_Pnt;

//! Describe the signature of a polyhedral surface with
//! only triangular facets and the necessary information
//! to compute the interferences.
class IntPatch_PolyhedronTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Give the bounding box of the Polyhedron.
  static const Bnd_Box& Bounding(const IntPatch_Polyhedron& thePolyh);

  //! Give the array of boxes. The box <n> corresponding
  //! to the triangle <n>.
  static const occ::handle<NCollection_HArray1<Bnd_Box>>& ComponentsBounding(const IntPatch_Polyhedron& thePolyh);

  //! Give the tolerance of the polygon.
  static double DeflectionOverEstimation(const IntPatch_Polyhedron& thePolyh);

  //! Give the number of triangles in this polyhedral surface.
  static int NbTriangles(const IntPatch_Polyhedron& thePolyh);

  //! Give the indices of the 3 points of the triangle of
  //! address Index in the Polyhedron.
  static void Triangle(const IntPatch_Polyhedron& thePolyh,
                       const int     Index,
                       int&          P1,
                       int&          P2,
                       int&          P3);

  //! Give the point of index i in the polyhedral surface.
  static const gp_Pnt& Point(const IntPatch_Polyhedron& thePolyh, const int Index);

  //! Gives the address Tricon of the triangle connexe to
  //! the triangle of address Triang by the edge Pivot Pedge
  //! and the third point of this connexe triangle. When we
  //! are on a free edge TriCon==0 but the function return
  //! the value of the triangle in the other side of Pivot
  //! on the free edge. Used to turn around a vertex.
  static int TriConnex(const IntPatch_Polyhedron& thePolyh,
                                    const int     Triang,
                                    const int     Pivot,
                                    const int     Pedge,
                                    int&          TriCon,
                                    int&          OtherP);

};

#include <IntPatch_PolyhedronTool.lxx>

#endif // _IntPatch_PolyhedronTool_HeaderFile
