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

#ifndef _gce_MakeDir_HeaderFile
#define _gce_MakeDir_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir.hxx>
#include <gce_Root.hxx>
class gp_Vec;
class gp_XYZ;
class gp_Pnt;

//! This class implements the following algorithms used
//! to create a Dir from gp.
//! * Create a Dir parallel to another and passing
//! through a point.
//! * Create a Dir passing through 2 points.
//! * Create a Dir from its axis (Ax1 from gp).
//! * Create a Dir from a point and a direction.
class gce_MakeDir : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Normalizes the vector V and creates a direction.
  //! Status is "NullVector" if V.Magnitude() <= Resolution.
  //! @param[in] V direction vector
  Standard_EXPORT gce_MakeDir(const gp_Vec& V);

  //! Creates a direction from a triplet of coordinates.
  //! Status is "NullVector" if Coord.Modulus() <=
  //! Resolution from gp.
  //! @param[in] Coord coordinate vector
  Standard_EXPORT gce_MakeDir(const gp_XYZ& Coord);

  //! Creates a direction with its 3 cartesian coordinates.
  //! Status is "NullVector" if std::sqrt(Xv*Xv + Yv*Yv + Zv*Zv)
  //! <= Resolution
  //! @param[in] Xv X coordinate value
  //! @param[in] Yv Y coordinate value
  //! @param[in] Zv Z coordinate value
  Standard_EXPORT gce_MakeDir(const double Xv, const double Yv, const double Zv);

  //! Make a Dir from gp <TheDir> passing through 2
  //! Pnt <P1>,<P2>.
  //! Status is "ConfusedPoints" if <p1> and <P2> are confused.
  //! Warning
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_ConfusedPoints if points P1 and P2 are coincident, or
  //! -   gce_NullVector if one of the following is less
  //! than or equal to gp::Resolution():
  //! -   the magnitude of vector V,
  //! -   the modulus of Coord,
  //! -   std::sqrt(Xv*Xv + Yv*Yv + Zv*Zv).
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakeDir(const gp_Pnt& P1, const gp_Pnt& P2);

  //! Returns the constructed unit vector.
  //! Exceptions StdFail_NotDone if no unit vector is constructed.
  //! @return resulting direction
  Standard_EXPORT const gp_Dir& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Dir Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Dir() const { return Operator(); }

private:
  gp_Dir TheDir;
};

#endif // _gce_MakeDir_HeaderFile
