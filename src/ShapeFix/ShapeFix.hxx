// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeFix_HeaderFile
#define _ShapeFix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>

#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <Message_ProgressRange.hxx>

class TopoDS_Shape;
class ShapeExtend_BasicMsgRegistrator;
class ShapeBuild_ReShape;

//! This package provides algorithms for fixing
//! problematic (violating Open CASCADE requirements) shapes.
//! Tools from package ShapeAnalysis are used for detecting the problems. The
//! detecting and fixing is done taking in account various
//! criteria implemented in BRepCheck package.
//! Each class of package ShapeFix deals with one
//! certain type of shapes or with some family of problems.
class ShapeFix
{
public:
  DEFINE_STANDARD_ALLOC

  //! Runs SameParameter from BRepLib with these adaptations :
  //! <enforce> forces computations, else they are made only on
  //! Edges with flag SameParameter false
  //! <preci>, if not precised, is taken for each EDge as its own
  //! Tolerance
  //! Returns True when done, False if an exception has been raised
  //! In case of exception anyway, as many edges as possible have
  //! been processed. The passed progress indicator allows user
  //! to consult the current progress stage and abort algorithm
  //! if needed.
  Standard_EXPORT static Standard_Boolean SameParameter(
    const TopoDS_Shape&                            shape,
    const Standard_Boolean                         enforce,
    const Standard_Real                            preci       = 0.0,
    const Message_ProgressRange&                   theProgress = Message_ProgressRange(),
    const Handle(ShapeExtend_BasicMsgRegistrator)& theMsgReg   = 0);

  //! Runs EncodeRegularity from BRepLib taking into account
  //! shared components of assemblies, so that each component
  //! is processed only once
  Standard_EXPORT static void EncodeRegularity(const TopoDS_Shape& shape,
                                               const Standard_Real tolang = 1.0e-10);

  //! Removes edges which are less than given tolerance from shape
  //! with help of ShapeFix_Wire::FixSmall()
  Standard_EXPORT static TopoDS_Shape RemoveSmallEdges(TopoDS_Shape&               shape,
                                                       const Standard_Real         Tolerance,
                                                       Handle(ShapeBuild_ReShape)& context);

  //! Fix position of the vertices having tolerance more tnan specified one.;
  Standard_EXPORT static Standard_Boolean FixVertexPosition(
    TopoDS_Shape&                     theshape,
    const Standard_Real               theTolerance,
    const Handle(ShapeBuild_ReShape)& thecontext);

  //! Calculate size of least edge;
  Standard_EXPORT static Standard_Real LeastEdgeSize(TopoDS_Shape& theshape);
};

#endif // _ShapeFix_HeaderFile
