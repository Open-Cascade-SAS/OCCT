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

#ifndef _IntTools_WLineTool_HeaderFile
#define _IntTools_WLineTool_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Macro.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_SequenceOfLine.hxx>
class TopoDS_Face;
class GeomAdaptor_HSurface;
class GeomInt_LineConstructor;
class IntTools_Context;
class Adaptor3d_TopolTool;

//! IntTools_WLineTool provides set of static methods related to walking lines.
class IntTools_WLineTool
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static
    Standard_Boolean NotUseSurfacesForApprox(const TopoDS_Face& aF1,
                                             const TopoDS_Face& aF2,
                                             const Handle(IntPatch_WLine)& WL,
                                             const Standard_Integer ifprm,
                                             const Standard_Integer ilprm);

  //! I
  //! Removes equal points (leave one of equal points) from theWLine
  //! and recompute vertex parameters.
  //!
  //! II
  //! Removes point out of borders in case of non periodic surfaces.
  //!
  //! III
  //! Removes exceed points using tube criteria:
  //! delete 7D point if it lies near to expected lines in 2d and 3d.
  //! Each task (2d, 2d, 3d) have its own tolerance and checked separately.
  //!
  //! Returns new WLine or null WLine if the number
  //! of the points is less than 2.
  Standard_EXPORT static
    Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)       &theWLine,
                                              const Handle(GeomAdaptor_HSurface) &theS1,
                                              const Handle(GeomAdaptor_HSurface) &theS2,
                                              const Handle(Adaptor3d_TopolTool)  &theDom1,
                                              const Handle(Adaptor3d_TopolTool)  &theDom2);

  Standard_EXPORT static
  Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine) &theWLine,
                                            const Standard_Integer       ideb,
                                            const Standard_Integer       ifin,
                                            const Standard_Boolean       onFirst);

  Standard_EXPORT static
  Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
                                        const Handle(GeomAdaptor_HSurface)&            theSurface1, 
                                        const Handle(GeomAdaptor_HSurface)&            theSurface2,
                                        const TopoDS_Face&                             theFace1,
                                        const TopoDS_Face&                             theFace2,
                                        const GeomInt_LineConstructor&                 theLConstructor,
                                        const Standard_Boolean                         theAvoidLConstructor,
                                        IntPatch_SequenceOfLine&                       theNewLines,
                                        Standard_Real&                                 theReachedTol3d,
                                        const Handle(IntTools_Context)& );
};

#endif