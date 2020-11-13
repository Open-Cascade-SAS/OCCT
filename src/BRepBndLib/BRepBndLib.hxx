// Created on: 1993-07-22
// Created by: Isabelle GRIGNON
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

#ifndef _BRepBndLib_HeaderFile
#define _BRepBndLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class TopoDS_Shape;
class Bnd_Box;
class Bnd_OBB;


//! This package provides the bounding boxes for curves
//! and surfaces from BRepAdaptor.
//! Functions to add a topological shape to a bounding box
class BRepBndLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the shape S to the bounding box B.
  //! More precisely are successively added to B:
  //! -   each face of S; the triangulation of the face is used if it exists,
  //! -   then each edge of S which does not belong to a face,
  //! the polygon of the edge is used if it exists
  //! -   and last each vertex of S which does not belong to an edge.
  //! After each elementary operation, the bounding box B is
  //! enlarged by the tolerance value of the relative sub-shape.
  //! When working with the triangulation of a face this value of
  //! enlargement is the sum of the triangulation deflection and
  //! the face tolerance. When working with the
  //! polygon of an edge this value of enlargement is
  //! the sum of the polygon deflection and the edge tolerance.
  //! Warning
  //! -   This algorithm is time consuming if triangulation has not
  //! been inserted inside the data structure of the shape S.
  //! -   The resulting bounding box may be somewhat larger than the object.
  Standard_EXPORT static void Add (const TopoDS_Shape& S, Bnd_Box& B, const Standard_Boolean useTriangulation = Standard_True);
  
  //! Adds the shape S to the bounding box B.
  //! This is a quick algorithm but only works if the shape S is
  //! composed of polygonal planar faces, as is the case if S is
  //! an approached polyhedral representation of an exact
  //! shape. Pay particular attention to this because this
  //! condition is not checked and, if it not respected, an error
  //! may occur in the algorithm for which the bounding box is built.
  //! Note that the resulting bounding box is not enlarged by the
  //! tolerance value of the sub-shapes as is the case with the
  //! Add function. So the added part of the resulting bounding
  //! box is closer to the shape S.
  Standard_EXPORT static void AddClose (const TopoDS_Shape& S, Bnd_Box& B);

  //! Adds the shape S to the bounding box B.
  //! This algorithm builds precise bounding box,
  //! which differs from exact geometry boundaries of shape only on shape entities tolerances
  //! Algorithm is the same as for method Add(..), but uses more precise methods for building boxes 
  //! for geometry objects.
  //! If useShapeTolerance = True, bounding box is enlardged by shape tolerances and 
  //! these tolerances are used for numerical methods of bounding box size calculations, 
  //! otherwise bounding box is built according to sizes of uderlined geometrical entities,
  //! numerical calculation use tolerance Precision::Confusion().
  Standard_EXPORT static void AddOptimal (const TopoDS_Shape& S, Bnd_Box& B, 
                                          const Standard_Boolean useTriangulation = Standard_True,
                                          const Standard_Boolean useShapeTolerance = Standard_False);


  //! Computes the Oriented Bounding box for the shape <theS>.
  //! Two independent methods of computation are implemented:
  //! first method based on set of points (so, it demands the
  //! triangulated shape or shape with planar faces and linear edges).
  //! The second method is based on use of inertia axes and is called
  //! if use of the first method is impossible.
  //! If theIsTriangulationUsed == FALSE then the triangulation will
  //! be ignored at all. 
  //! If theIsShapeToleranceUsed == TRUE then resulting box will be
  //! extended on the tolerance of the shape.
  //! theIsOptimal flag defines whether to look for the more tight
  //! OBB for the cost of performance or not.
  Standard_EXPORT static 
    void AddOBB(const TopoDS_Shape& theS,
                Bnd_OBB& theOBB,
                const Standard_Boolean theIsTriangulationUsed = Standard_True,
                const Standard_Boolean theIsOptimal = Standard_False,
                const Standard_Boolean theIsShapeToleranceUsed = Standard_True);

protected:





private:





};







#endif // _BRepBndLib_HeaderFile
