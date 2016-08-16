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

#ifndef _IntPatch_WLineTool_HeaderFile
#define _IntPatch_WLineTool_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Macro.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <IntSurf_Quadric.hxx>
class TopoDS_Face;
class GeomAdaptor_HSurface;
class GeomInt_LineConstructor;
class IntTools_Context;
class Adaptor3d_TopolTool;
class Adaptor3d_HSurface;

//! IntPatch_WLineTool provides set of static methods related to walking lines.
class IntPatch_WLineTool
{
public:

  DEFINE_STANDARD_ALLOC

  //! I
  //! Removes equal points (leave one of equal points) from theWLine
  //! and recompute vertex parameters.
  //!
  //! II
  //! Removes point out of borders in case of non periodic surfaces.
  //! This step is done only if theRestrictLine is true.
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
                                              const Handle(Adaptor3d_HSurface) &theS1,
                                              const Handle(Adaptor3d_HSurface) &theS2,
                                              const Handle(Adaptor3d_TopolTool)  &theDom1,
                                              const Handle(Adaptor3d_TopolTool)  &theDom2,
                                              const Standard_Boolean      theRestrictLine);

//! Joins all WLines from theSlin to one if it is possible and records 
//! the result into theSlin again. Lines will be kept to be splitted if:
//! a) they are separated (has no common points);
//! b) resulted line (after joining) go through seam-edges or surface boundaries.
//!
//! In addition, if points in theSPnt lies at least in one of the line in theSlin,
//! this point will be deleted.
  Standard_EXPORT static void JoinWLines(IntPatch_SequenceOfLine& theSlin,
                                         IntPatch_SequenceOfPoint& theSPnt,
                                         const Standard_Real theTol3D,
                                         const Standard_Real theU1Period,
                                         const Standard_Real theU2Period,
                                         const Standard_Real theV1Period,
                                         const Standard_Real theV2Period,
                                         const Standard_Real theUfSurf1,
                                         const Standard_Real theUlSurf1,
                                         const Standard_Real theVfSurf1,
                                         const Standard_Real theVlSurf1,
                                         const Standard_Real theUfSurf2,
                                         const Standard_Real theUlSurf2,
                                         const Standard_Real theVfSurf2,
                                         const Standard_Real theVlSurf2);


//! Extends every line from theSlin (if it is possible) to be started/finished
//! in strictly determined point (in the place of joint of two lines).
//! As result, some gaps between two lines will vanish.
//! The Walking lines are supposed (algorithm will do nothing for not-Walking line)
//! to be computed as a result of intersection of two quadrics.
//! The quadrics definition is accepted in input parameters.
  Standard_EXPORT static void ExtendTwoWlinesToEachOther(IntPatch_SequenceOfLine& theSlin,
                                                         const IntSurf_Quadric& theS1,
                                                         const IntSurf_Quadric& theS2,
                                                         const Standard_Real theToler3D,
                                                         const Standard_Real theU1Period,
                                                         const Standard_Real theU2Period,
                                                         const Standard_Real theV1Period,
                                                         const Standard_Real theV2Period,
                                                         const Bnd_Box2d& theBoxS1,
                                                         const Bnd_Box2d& theBoxS2);
};

#endif