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

#ifndef _ShapeAnalysis_WireOrder_HeaderFile
#define _ShapeAnalysis_WireOrder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HSequenceOfXY.hxx>
#include <TColgp_HSequenceOfXYZ.hxx>
#include <Standard_Integer.hxx>
class gp_XYZ;
class gp_XY;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! This class is intended to control and, if possible, redefine
//! the order of a list of edges which define a wire
//! Edges are not given directly, but as their bounds (start,end)
//!
//! This allows to use this tool, either on existing wire, or on
//! data just taken from a file (coordinates are easy to get)
//!
//! It can work, either in 2D, or in 3D, or miscible mode
//! The tolerance for each mode is fixed
//!
//! Two phases : firstly add the couples (start, end)
//! secondly perform then get the result
class ShapeAnalysis_WireOrder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_WireOrder();

  //! Creates a WireOrder.
  //! Flag <theMode3D> defines 3D or 2d mode.
  //! Flag <theModeBoth> defines miscible mode and the flag <theMode3D> is ignored.
  //! Warning: Parameter <theTolerance> is not used in algorithm.
  Standard_EXPORT ShapeAnalysis_WireOrder(const Standard_Boolean theMode3D,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theModeBoth = Standard_False);

  //! Sets new values.
  //! Clears the edge list if the mode (<theMode3D> or <theModeBoth> ) changes.
  //! Clears the connexion list.
  //! Warning: Parameter <theTolerance> is not used in algorithm.
  Standard_EXPORT void SetMode(const Standard_Boolean theMode3D,
                               const Standard_Real    theTolerance,
                               const Standard_Boolean theModeBoth = Standard_False);

  //! Returns the working tolerance
  Standard_EXPORT Standard_Real Tolerance() const;

  //! Clears the list of edges, but not mode and tol
  Standard_EXPORT void Clear();

  //! Adds a couple of points 3D (start, end)
  Standard_EXPORT void Add(const gp_XYZ& theStart3d, const gp_XYZ& theEnd3d);

  //! Adds a couple of points 2D (start, end)
  Standard_EXPORT void Add(const gp_XY& theStart2d, const gp_XY& theEnd2d);

  //! Adds a couple of points 3D and 2D (start, end)
  Standard_EXPORT void Add(const gp_XYZ& theStart3d,
                           const gp_XYZ& theEnd3d,
                           const gp_XY&  theStart2d,
                           const gp_XY&  theEnd2d);

  //! Returns the count of added couples of points (one per edges)
  Standard_EXPORT Standard_Integer NbEdges() const;

  //! If this mode is True method perform does not sort edges of
  //! different loops. The resulting order is first loop, second
  //! one etc...
  Standard_EXPORT Standard_Boolean& KeepLoopsMode();

  //! Computes the better order
  //! Optimised if the couples were already in order
  //! The criterium is : two couples in order if distance between
  //! end-prec and start-cur is less then starting tolerance <tol>
  //! Else, the smallest distance is reached
  //! Warning: Parameter <closed> not used
  Standard_EXPORT void Perform(const Standard_Boolean closed = Standard_True);

  //! Tells if Perform has been done
  //! Else, the following methods returns original values
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the status of the order (0 if not done) :
  //! 0 : all edges are direct and in sequence
  //! 1 : all edges are direct but some are not in sequence
  //! -1 : some edges are reversed, but no gap remain
  //! 3 : edges in sequence are just shifted in forward or reverse manner
  Standard_EXPORT Standard_Integer Status() const;

  //! Returns the number of original edge which correspond to the
  //! newly ordered number <n>
  //! Warning : the returned value is NEGATIVE if edge should be reversed
  Standard_EXPORT Standard_Integer Ordered(const Standard_Integer theIdx) const;

  //! Returns the values of the couple <num>, as 3D values
  Standard_EXPORT void XYZ(const Standard_Integer theIdx,
                           gp_XYZ&                theStart3D,
                           gp_XYZ&                theEnd3D) const;

  //! Returns the values of the couple <num>, as 2D values
  Standard_EXPORT void XY(const Standard_Integer theIdx, gp_XY& theStart2D, gp_XY& theEnd2D) const;

  //! Returns the gap between a couple and its preceding
  //! <num> is considered ordered
  //! If <num> = 0 (D), returns the greatest gap found
  Standard_EXPORT Standard_Real Gap(const Standard_Integer num = 0) const;

  //! Determines the chains inside which successive edges have a gap
  //! less than a given value. Queried by NbChains and Chain
  Standard_EXPORT void SetChains(const Standard_Real gap);

  //! Returns the count of computed chains
  Standard_EXPORT Standard_Integer NbChains() const;

  //! Returns, for the chain n0 num, starting and ending numbers of
  //! edges. In the list of ordered edges (see Ordered for originals)
  Standard_EXPORT void Chain(const Standard_Integer num,
                             Standard_Integer&      n1,
                             Standard_Integer&      n2) const;

  //! Determines the couples of edges for which end and start fit
  //! inside a given gap. Queried by NbCouples and Couple
  //! Warning: function isn't implemented
  Standard_EXPORT void SetCouples(const Standard_Real gap);

  //! Returns the count of computed couples
  Standard_EXPORT Standard_Integer NbCouples() const;

  //! Returns, for the couple n0 num, the two implied edges
  //! In the list of ordered edges
  Standard_EXPORT void Couple(const Standard_Integer num,
                              Standard_Integer&      n1,
                              Standard_Integer&      n2) const;

protected:
private:
  // the mode in which the algorithm works
  enum ModeType
  {
    Mode2D,
    Mode3D,
    ModeBoth
  };

  Handle(TColStd_HArray1OfInteger) myOrd;
  Handle(TColStd_HArray1OfInteger) myChains;
  Handle(TColStd_HArray1OfInteger) myCouples;
  Handle(TColgp_HSequenceOfXYZ)    myXYZ;
  Handle(TColgp_HSequenceOfXY)     myXY;
  Standard_Real                    myTol;
  Standard_Real                    myGap;
  Standard_Integer                 myStat;
  Standard_Boolean                 myKeepLoops;
  ModeType                         myMode;
};

#endif // _ShapeAnalysis_WireOrder_HeaderFile
