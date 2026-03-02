// Created on: 1992-08-26
// Created by: Remi GILET
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

#ifndef _gce_MakeLin2d_HeaderFile
#define _gce_MakeLin2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Lin2d.hxx>
#include <gce_Root.hxx>
class gp_Ax2d;
class gp_Pnt2d;
class gp_Dir2d;

//! This class implements the following algorithms used
//! to create Lin2d from gp.
//!
//! * Create a Lin2d parallel to another and passing
//! through a point.
//! * Create a Lin2d parallel to another at the distance
//! Dist.
//! * Create a Lin2d passing through 2 points.
//! * Create a Lin2d from its axis (Ax1 from gp).
//! * Create a Lin2d from a point and a direction.
//! * Create a Lin2d from its equation.
class gce_MakeLin2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a line located with A.
  //! @param[in] A local coordinate system
  Standard_EXPORT gce_MakeLin2d(const gp_Ax2d& A);

  //! <P> is the location point (origin) of the line and
  //! <V> is the direction of the line.
  //! @param[in] P point
  //! @param[in] V direction vector
  Standard_EXPORT gce_MakeLin2d(const gp_Pnt2d& P, const gp_Dir2d& V);

  //! Creates the line from the equation A*X + B*Y + C = 0.0
  //! the status is "NullAxis"if std::sqrt(A*A + B*B) <= Resolution from gp.
  //! @param[in] A local coordinate system
  //! @param[in] B equation coefficient B
  //! @param[in] C equation coefficient C
  Standard_EXPORT gce_MakeLin2d(const double A, const double B, const double C);

  //! Make a Lin2d from gp <TheLin> parallel to another
  //! Lin2d <Lin> at a distance <Dist>.
  //! If Dist is greater than zero the result is on the
  //! right of the Line <Lin>, else the result is on the
  //! left of the Line <Lin>.
  //! @param[in] Lin source line
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakeLin2d(const gp_Lin2d& Lin, const double Dist);

  //! Make a Lin2d from gp <TheLin> parallel to another
  //! Lin2d <Lin> and passing through a Pnt2d <Point>.
  //! @param[in] Lin source line
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakeLin2d(const gp_Lin2d& Lin, const gp_Pnt2d& Point);

  //! Make a Lin2d from gp <TheLin> passing through 2
  //! Pnt2d <P1>,<P2>.
  //! It returns false if <P1> and <P2> are confused.
  //! Warning
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NullAxis if std::sqrt(A*A + B*B) is less
  //! than or equal to gp::Resolution(), or
  //! -   gce_ConfusedPoints if points P1 and P2 are coincident.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakeLin2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2);

  //! Returns the constructed line.
  //! Exceptions StdFail_NotDone if no line is constructed.
  //! @return resulting line
  Standard_EXPORT const gp_Lin2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Lin2d Operator() const
  {
    return Value();
  }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Lin2d() const
  {
    return Operator();
  }

private:
  gp_Lin2d TheLin2d;
};

#endif // _gce_MakeLin2d_HeaderFile
