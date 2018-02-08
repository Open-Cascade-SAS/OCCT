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

#include <Standard_DefineAlloc.hxx>

#include <IntPatch_SequenceOfLine.hxx>
#include <IntPatch_WLine.hxx>

class Adaptor3d_TopolTool;

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
                                              const Handle(Adaptor3d_TopolTool)  &theDom2);

//! Joins all WLines from theSlin to one if it is possible and records 
//! the result into theSlin again. Lines will be kept to be splitted if:
//! a) they are separated (has no common points);
//! b) resulted line (after joining) go through seam-edges or surface boundaries.
//!
//! In addition, if points in theSPnt lies at least in one of the line in theSlin,
//! this point will be deleted.
  Standard_EXPORT static void JoinWLines(IntPatch_SequenceOfLine& theSlin,
                                         IntPatch_SequenceOfPoint& theSPnt,
                                         Handle(Adaptor3d_HSurface) theS1,
                                         Handle(Adaptor3d_HSurface) theS2,
                                         const Standard_Real theTol3D);

//! Extends every line from theSlin (if it is possible) to be started/finished
//! in strictly determined point (in the place of joint of two lines).
//! As result, some gaps between two lines will vanish.
//! The Walking lines are supposed (algorithm will do nothing for not-Walking line)
//! to be computed as a result of intersection. Both theS1 and theS2 
//! must be quadrics. Other cases are not supported.
//! theArrPeriods must be filled as follows (every value must not be negative;
//! if the surface is not periodic the period must be equal to 0.0 strictly):
//! {<U-period of 1st surface>, <V-period of 1st surface>,
//!               <U-period of 2nd surface>, <V-period of 2nd surface>}.
  Standard_EXPORT static void
            ExtendTwoWLines(IntPatch_SequenceOfLine& theSlin,
                            const Handle(Adaptor3d_HSurface)& theS1,
                            const Handle(Adaptor3d_HSurface)& theS2,
                            const Standard_Real theToler3D,
                            const Standard_Real* const theArrPeriods,
                            const Bnd_Box2d& theBoxS1,
                            const Bnd_Box2d& theBoxS2);

  //! Max angle to concatenate two WLines to avoid result with C0-continuity
  static const Standard_Real myMaxConcatAngle;
};

#endif