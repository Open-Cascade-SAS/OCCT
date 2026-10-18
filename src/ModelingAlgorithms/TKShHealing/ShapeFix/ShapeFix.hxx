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
  Standard_EXPORT static bool SameParameter(
    const TopoDS_Shape&                                 shape,
    const bool                                          enforce,
    const double                                        preci       = 0.0,
    const Message_ProgressRange&                        theProgress = Message_ProgressRange(),
    const occ::handle<ShapeExtend_BasicMsgRegistrator>& theMsgReg   = nullptr);

  //! Fixes same parameterization through a replacement context.
  //! Changed edges and vertices are recorded in the context. Unchanged topology and
  //! geometry remain shared. A null context uses the in-place behavior of the overload above.
  //! Already recorded context results can be changed in place.
  //! @param[in] shape shape to process
  //! @param[in] enforce recompute parameterization even when its flags are set
  //! @param[in] preci precision argument of the in-place overload
  //! @param[in] theProgress progress range for cancellation
  //! @param[in] theMsgReg optional warning registrator; messages identify input edges
  //! @param[in] theContext replacement context; apply it to obtain the resulting shape
  //! @return true if processing completed and every edge has its SameParameter flag set
  Standard_EXPORT static bool SameParameter(
    const TopoDS_Shape&                                 shape,
    const bool                                          enforce,
    const double                                        preci,
    const Message_ProgressRange&                        theProgress,
    const occ::handle<ShapeExtend_BasicMsgRegistrator>& theMsgReg,
    const occ::handle<ShapeBuild_ReShape>&              theContext);

  //! Runs EncodeRegularity from BRepLib taking into account
  //! shared components of assemblies, so that each component
  //! is processed only once
  Standard_EXPORT static void EncodeRegularity(const TopoDS_Shape& shape,
                                               const double        tolang = 1.0e-10);

  //! Removes edges which are less than given tolerance from shape
  //! with help of ShapeFix_Wire::FixSmall()
  Standard_EXPORT static TopoDS_Shape RemoveSmallEdges(TopoDS_Shape&                    shape,
                                                       const double                     Tolerance,
                                                       occ::handle<ShapeBuild_ReShape>& context);

  //! Fix position of the vertices having tolerance more tnan specified one.;
  Standard_EXPORT static bool FixVertexPosition(TopoDS_Shape&                          theshape,
                                                const double                           theTolerance,
                                                const occ::handle<ShapeBuild_ReShape>& thecontext);

  //! Calculate size of least edge;
  Standard_EXPORT static double LeastEdgeSize(TopoDS_Shape& theshape);
};

#endif // _ShapeFix_HeaderFile
