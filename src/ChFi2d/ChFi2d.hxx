// Created on: 1995-06-12
// Created by: Joelle CHAUVET
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

#ifndef _ChFi2d_HeaderFile
#define _ChFi2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <ChFi2d_ConstructionError.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class ChFi2d_Builder;


//! This package contains the algorithms used to build
//! fillets or chamfers on planar wire.
//!
//! This package provides two algorithms for 2D fillets:
//! ChFi2d_Builder - it constructs a fillet or chamfer
//! for linear and circular edges of a face.
//! ChFi2d_FilletAPI - it encapsulates two algorithms:
//! ChFi2d_AnaFilletAlgo - analytical constructor of the fillet.
//! It works only for linear and circular edges,
//! having a common point.
//! ChFi2d_FilletAlgo - iteration recursive method constructing
//! the fillet edge for any type of edges including
//! ellipses and b-splines.
//! The edges may even have no common point.
//! ChFi2d_ChamferAPI - an algoroithm for construction of chamfers
//! between two linear edges of a plane.
//!
//! The algorithms ChFi2d_AnaFilletAlgo and ChFi2d_FilletAlgo may be used directly
//! or via the interface class ChFi2d_FilletAPI.
class ChFi2d 
{
public:

  DEFINE_STANDARD_ALLOC





protected:





private:

  
  Standard_EXPORT static Standard_Boolean CommonVertex (const TopoDS_Edge& E1, const TopoDS_Edge& E2, TopoDS_Vertex& V);
  
  Standard_EXPORT static ChFi2d_ConstructionError FindConnectedEdges (const TopoDS_Face& F, const TopoDS_Vertex& V, TopoDS_Edge& E1, TopoDS_Edge& E2);



friend class ChFi2d_Builder;

};







#endif // _ChFi2d_HeaderFile
