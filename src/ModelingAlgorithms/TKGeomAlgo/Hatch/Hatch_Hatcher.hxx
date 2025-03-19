// Created on: 1992-08-19
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Hatch_Hatcher_HeaderFile
#define _Hatch_Hatcher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Hatch_SequenceOfLine.hxx>
#include <Hatch_LineForm.hxx>
#include <Standard_Integer.hxx>
class gp_Lin2d;
class gp_Dir2d;
class gp_Pnt2d;

//! The Hatcher   is  an algorithm  to   compute cross
//! hatchings in a 2d plane. It is mainly dedicated to
//! display purpose.
//!
//! Computing cross hatchings is a 3 steps process :
//!
//! 1.  The users stores in the   Hatcher a set  of 2d
//! lines to   be  trimmed. Methods   in  the  "Lines"
//! category.
//!
//! 2.  The user trims the lines with a boundary.  The
//! inside of a boundary is on the left side.  Methods
//! in the "Trimming" category.
//!
//! 3. The user reads  back the trimmed lines. Methods
//! in the "Results" category.
//!
//! The result is a set of parameter intervals  on the
//! line. The first  parameter of an  Interval may  be
//! RealFirst() and the last may be RealLast().
//!
//! A line can be a line parallel to the axis (X  or Y
//! line or a 2D line.
//!
//! The Hatcher has two modes :
//!
//! *  The "Oriented" mode,  where the  orientation of
//! the trimming curves is  considered. The  hatch are
//! kept on  the left of  the  trimming curve. In this
//! mode infinite hatch can be computed.
//!
//! *   The "UnOriented"  mode,  where  the  hatch are
//! always finite.
class Hatch_Hatcher
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a empty  hatcher.  <Tol> is the  tolerance
  //! for intersections.
  Standard_EXPORT Hatch_Hatcher(const Standard_Real    Tol,
                                const Standard_Boolean Oriented = Standard_True);

  void Tolerance(const Standard_Real Tol);

  Standard_Real Tolerance() const;

  //! Add a line <L>  to  be trimmed.   <T> the  type is
  //! only kept from information. It is not used  in the
  //! computation.
  Standard_EXPORT void AddLine(const gp_Lin2d& L, const Hatch_LineForm T = Hatch_ANYLINE);

  //! Add an infinite line on  direction <D> at distance
  //! <Dist> from the origin  to be  trimmed. <Dist> may
  //! be negative.
  //!
  //! If O  is the origin  of the  2D plane, and   V the
  //! vector perpendicular to D (in the direct direction).
  //!
  //! A point P is on the line if :
  //! OP dot V = Dist
  //! The parameter of P on the line is
  //! OP dot D
  Standard_EXPORT void AddLine(const gp_Dir2d& D, const Standard_Real Dist);

  //! Add an infinite line   parallel to the Y-axis   at
  //! abciss <X>.
  Standard_EXPORT void AddXLine(const Standard_Real X);

  //! Add an infinite line   parallel to the X-axis   at
  //! ordinate <Y>.
  Standard_EXPORT void AddYLine(const Standard_Real Y);

  //! Trims the lines at intersections with  <L>.
  Standard_EXPORT void Trim(const gp_Lin2d& L, const Standard_Integer Index = 0);

  //! Trims the lines at intersections  with <L>  in the
  //! parameter range <Start>, <End>
  Standard_EXPORT void Trim(const gp_Lin2d&        L,
                            const Standard_Real    Start,
                            const Standard_Real    End,
                            const Standard_Integer Index = 0);

  //! Trims the line at  intersection with  the oriented
  //! segment P1,P2.
  Standard_EXPORT void Trim(const gp_Pnt2d&        P1,
                            const gp_Pnt2d&        P2,
                            const Standard_Integer Index = 0);

  //! Returns the total number  of intervals on  all the
  //! lines.
  Standard_EXPORT Standard_Integer NbIntervals() const;

  //! Returns the number of lines.
  Standard_EXPORT Standard_Integer NbLines() const;

  //! Returns the line of index <I>.
  Standard_EXPORT const gp_Lin2d& Line(const Standard_Integer I) const;

  //! Returns  the type of the  line   of  index <I>.
  Standard_EXPORT Hatch_LineForm LineForm(const Standard_Integer I) const;

  //! Returns  True if the  line   of  index <I>  has  a
  //! constant X value.
  Standard_Boolean IsXLine(const Standard_Integer I) const;

  //! Returns  True if the  line   of  index <I>  has  a
  //! constant Y value.
  Standard_Boolean IsYLine(const Standard_Integer I) const;

  //! Returns the X or Y coordinate of the line of index
  //! <I> if it is a X or a Y line.
  Standard_EXPORT Standard_Real Coordinate(const Standard_Integer I) const;

  //! Returns the number of intervals on line of index <I>.
  Standard_EXPORT Standard_Integer NbIntervals(const Standard_Integer I) const;

  //! Returns the first   parameter of  interval <J>  on
  //! line  <I>.
  Standard_EXPORT Standard_Real Start(const Standard_Integer I, const Standard_Integer J) const;

  //! Returns the first Index and Par2 of  interval <J>  on
  //! line  <I>.
  Standard_EXPORT void StartIndex(const Standard_Integer I,
                                  const Standard_Integer J,
                                  Standard_Integer&      Index,
                                  Standard_Real&         Par2) const;

  //! Returns the last   parameter of  interval <J>  on
  //! line  <I>.
  Standard_EXPORT Standard_Real End(const Standard_Integer I, const Standard_Integer J) const;

  //! Returns the last Index and Par2 of  interval <J>  on
  //! line  <I>.
  Standard_EXPORT void EndIndex(const Standard_Integer I,
                                const Standard_Integer J,
                                Standard_Integer&      Index,
                                Standard_Real&         Par2) const;

protected:
private:
  Standard_Real        myToler;
  Hatch_SequenceOfLine myLines;
  Standard_Boolean     myOrient;
};

#include <Hatch_Hatcher.lxx>

#endif // _Hatch_Hatcher_HeaderFile
