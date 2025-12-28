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

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XY.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
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
  Standard_EXPORT ShapeAnalysis_WireOrder(const bool theMode3D,
                                          const double    theTolerance,
                                          const bool theModeBoth = false);

  //! Sets new values.
  //! Clears the edge list if the mode (<theMode3D> or <theModeBoth> ) changes.
  //! Clears the connexion list.
  //! Warning: Parameter <theTolerance> is not used in algorithm.
  Standard_EXPORT void SetMode(const bool theMode3D,
                               const double    theTolerance,
                               const bool theModeBoth = false);

  //! Returns the working tolerance
  Standard_EXPORT double Tolerance() const;

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
  Standard_EXPORT int NbEdges() const;

  //! If this mode is True method perform does not sort edges of
  //! different loops. The resulting order is first loop, second
  //! one etc...
  Standard_EXPORT bool& KeepLoopsMode();

  //! Computes the better order
  //! Optimised if the couples were already in order
  //! The criterium is : two couples in order if distance between
  //! end-prec and start-cur is less then starting tolerance <tol>
  //! Else, the smallest distance is reached
  //! Warning: Parameter <closed> not used
  Standard_EXPORT void Perform(const bool closed = true);

  //! Tells if Perform has been done
  //! Else, the following methods returns original values
  Standard_EXPORT bool IsDone() const;

  //! Returns the status of the order (0 if not done) :
  //! 0 : all edges are direct and in sequence
  //! 1 : all edges are direct but some are not in sequence
  //! -1 : some edges are reversed, but no gap remain
  //! 3 : edges in sequence are just shifted in forward or reverse manner
  Standard_EXPORT int Status() const;

  //! Returns the number of original edge which correspond to the
  //! newly ordered number <n>
  //! Warning : the returned value is NEGATIVE if edge should be reversed
  Standard_EXPORT int Ordered(const int theIdx) const;

  //! Returns the values of the couple <num>, as 3D values
  Standard_EXPORT void XYZ(const int theIdx,
                           gp_XYZ&                theStart3D,
                           gp_XYZ&                theEnd3D) const;

  //! Returns the values of the couple <num>, as 2D values
  Standard_EXPORT void XY(const int theIdx, gp_XY& theStart2D, gp_XY& theEnd2D) const;

  //! Returns the gap between a couple and its preceding
  //! <num> is considered ordered
  //! If <num> = 0 (D), returns the greatest gap found
  Standard_EXPORT double Gap(const int num = 0) const;

  //! Determines the chains inside which successive edges have a gap
  //! less than a given value. Queried by NbChains and Chain
  Standard_EXPORT void SetChains(const double gap);

  //! Returns the count of computed chains
  Standard_EXPORT int NbChains() const;

  //! Returns, for the chain n0 num, starting and ending numbers of
  //! edges. In the list of ordered edges (see Ordered for originals)
  Standard_EXPORT void Chain(const int num,
                             int&      n1,
                             int&      n2) const;

  //! Determines the couples of edges for which end and start fit
  //! inside a given gap. Queried by NbCouples and Couple
  //! Warning: function isn't implemented
  Standard_EXPORT void SetCouples(const double gap);

  //! Returns the count of computed couples
  Standard_EXPORT int NbCouples() const;

  //! Returns, for the couple n0 num, the two implied edges
  //! In the list of ordered edges
  Standard_EXPORT void Couple(const int num,
                              int&      n1,
                              int&      n2) const;

private:
  // the mode in which the algorithm works
  enum ModeType
  {
    Mode2D,
    Mode3D,
    ModeBoth
  };

  occ::handle<NCollection_HArray1<int>> myOrd;
  occ::handle<NCollection_HArray1<int>> myChains;
  occ::handle<NCollection_HArray1<int>> myCouples;
  occ::handle<NCollection_HSequence<gp_XYZ>>    myXYZ;
  occ::handle<NCollection_HSequence<gp_XY>>     myXY;
  double                    myTol;
  double                    myGap;
  int                 myStat;
  bool                 myKeepLoops;
  ModeType                         myMode;
};

#endif // _ShapeAnalysis_WireOrder_HeaderFile
