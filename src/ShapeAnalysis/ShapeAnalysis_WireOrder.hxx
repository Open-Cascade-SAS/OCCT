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

#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HSequenceOfXYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Standard_TypeMismatch;
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
//! It can work, either in 2D, or in 3D, but not miscible
//! Warning about tolerance : according to the mode (2D/3D), it
//! must be given as 2D or 3D (i.e. metric) tolerance, uniform
//! on the whole list
//!
//! Two phases : firstly add the couples (start,end)
//! secondly perform then get the result
class ShapeAnalysis_WireOrder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_WireOrder();
  
  //! Creates a WireOrder in 3D (if mode3d is True) or 2D (if False)
  //! with a tolerance
  Standard_EXPORT ShapeAnalysis_WireOrder(const Standard_Boolean mode3d, const Standard_Real tol);
  
  //! Sets new values. Clears the connexion list
  //! If <mode3d> changes, also clears the edge list (else, doesn't)
  Standard_EXPORT void SetMode (const Standard_Boolean mode3d, const Standard_Real tol);
  
  //! Returns the working tolerance
  Standard_EXPORT Standard_Real Tolerance() const;
  
  //! Clears the list of edges, but not mode and tol
  Standard_EXPORT void Clear();
  
  //! Adds a couple of points 3D (start,end)
  Standard_EXPORT void Add (const gp_XYZ& start3d, const gp_XYZ& end3d);
  
  //! Adds a couple of points 2D (start,end)
  Standard_EXPORT void Add (const gp_XY& start2d, const gp_XY& end2d);
  
  //! Returns the count of added couples of points (one per edges)
  Standard_EXPORT Standard_Integer NbEdges() const;
  
  //! If this mode is True method perform does not sort edges of
  //! different loops. The resulting order is first loop, second
  //! one etc...
  Standard_EXPORT Standard_Boolean& KeepLoopsMode();
  
  //! Computes the better order
  //! If <closed> is True (D) considers also closure
  //! Optimised if the couples were already in order
  //! The criterium is : two couples in order if distance between
  //! end-prec and start-cur is less then starting tolerance <tol>
  //! Else, the smallest distance is reached
  //! Gap corresponds to a smallest distance greater than <tol>
  Standard_EXPORT void Perform (const Standard_Boolean closed = Standard_True);
  
  //! Tells if Perform has been done
  //! Else, the following methods returns original values
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the status of the order (0 if not done) :
  //! 0 : all edges are direct and in sequence
  //! 1 : all edges are direct but some are not in sequence
  //! 2 : in addition, unresolved gaps remain
  //! -1 : some edges are reversed, but no gap remain
  //! -2 : some edges are reversed and some gaps remain
  //! -10 : COULD NOT BE RESOLVED, Failure on Reorder
  //! gap : regarding starting <tol>
  Standard_EXPORT Standard_Integer Status() const;
  
  //! Returns the number of original edge which correspond to the
  //! newly ordered number <n>
  //! Warning : the returned value is NEGATIVE if edge should be reversed
  Standard_EXPORT Standard_Integer Ordered (const Standard_Integer n) const;
  
  //! Returns the values of the couple <num>, as 3D values
  Standard_EXPORT void XYZ (const Standard_Integer num, gp_XYZ& start3d, gp_XYZ& end3d) const;
  
  //! Returns the values of the couple <num>, as 2D values
  Standard_EXPORT void XY (const Standard_Integer num, gp_XY& start2d, gp_XY& end2d) const;
  
  //! Returns the gap between a couple and its preceding
  //! <num> is considered ordered
  //! If <num> = 0 (D), returns the greatest gap found
  Standard_EXPORT Standard_Real Gap (const Standard_Integer num = 0) const;
  
  //! Determines the chains inside which successive edges have a gap
  //! less than a given value. Queried by NbChains and Chain
  Standard_EXPORT void SetChains (const Standard_Real gap);
  
  //! Returns the count of computed chains
  Standard_EXPORT Standard_Integer NbChains() const;
  
  //! Returns, for the chain n0 num, starting and ending numbers of
  //! edges. In the list of ordered edges (see Ordered for originals)
  Standard_EXPORT void Chain (const Standard_Integer num, Standard_Integer& n1, Standard_Integer& n2) const;
  
  //! Determines the couples of edges for which end and start fit
  //! inside a given gap. Queried by NbCouples and Couple
  Standard_EXPORT void SetCouples (const Standard_Real gap);
  
  //! Returns the count of computed couples
  Standard_EXPORT Standard_Integer NbCouples() const;
  
  //! Returns, for the couple n0 num, the two implied edges
  //! In the list of ordered edges
  Standard_EXPORT void Couple (const Standard_Integer num, Standard_Integer& n1, Standard_Integer& n2) const;




protected:





private:



  Standard_Boolean myKeepLoops;
  Handle(TColStd_HArray1OfInteger) myOrd;
  Handle(TColStd_HArray1OfInteger) myChains;
  Handle(TColStd_HArray1OfInteger) myCouples;
  Handle(TColgp_HSequenceOfXYZ) myXYZ;
  Standard_Real myTol;
  Standard_Real myGap;
  Standard_Integer myStat;
  Standard_Boolean myMode;


};







#endif // _ShapeAnalysis_WireOrder_HeaderFile
