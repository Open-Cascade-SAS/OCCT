// Created on: 1995-02-03
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Law_Interpol_HeaderFile
#define _Law_Interpol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Law_BSpFunc.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

class Law_Interpol;
DEFINE_STANDARD_HANDLE(Law_Interpol, Law_BSpFunc)

//! Provides an evolution law that interpolates a set
//! of parameter and value pairs (wi, radi)
class Law_Interpol : public Law_BSpFunc
{

public:
  //! Constructs an empty interpolative evolution law.
  //! The function Set is used to define the law.
  Standard_EXPORT Law_Interpol();

  //! Defines this evolution law by interpolating the set of 2D
  //! points ParAndRad. The Y coordinate of a point of
  //! ParAndRad is the value of the function at the parameter
  //! point given by its X coordinate.
  //! If Periodic is true, this function is assumed to be periodic.
  //! Warning
  //! -   The X coordinates of points in the table ParAndRad
  //! must be given in ascendant order.
  //! -   If Periodic is true, the first and last Y coordinates of
  //! points in the table ParAndRad are assumed to be
  //! equal. In addition, with the second syntax, Dd and Df
  //! are also assumed to be equal. If this is not the case,
  //! Set uses the first value(s) as last value(s).
  Standard_EXPORT void Set(const TColgp_Array1OfPnt2d& ParAndRad,
                           const Standard_Boolean      Periodic = Standard_False);

  Standard_EXPORT void SetInRelative(const TColgp_Array1OfPnt2d& ParAndRad,
                                     const Standard_Real         Ud,
                                     const Standard_Real         Uf,
                                     const Standard_Boolean      Periodic = Standard_False);

  //! Defines this evolution law by interpolating the set of 2D
  //! points ParAndRad. The Y coordinate of a point of
  //! ParAndRad is the value of the function at the parameter
  //! point given by its X coordinate.
  //! If Periodic is true, this function is assumed to be periodic.
  //! In the second syntax, Dd and Df define the values of
  //! the first derivative of the function at its first and last points.
  //! Warning
  //! -   The X coordinates of points in the table ParAndRad
  //! must be given in ascendant order.
  //! -   If Periodic is true, the first and last Y coordinates of
  //! points in the table ParAndRad are assumed to be
  //! equal. In addition, with the second syntax, Dd and Df
  //! are also assumed to be equal. If this is not the case,
  //! Set uses the first value(s) as last value(s).
  Standard_EXPORT void Set(const TColgp_Array1OfPnt2d& ParAndRad,
                           const Standard_Real         Dd,
                           const Standard_Real         Df,
                           const Standard_Boolean      Periodic = Standard_False);

  Standard_EXPORT void SetInRelative(const TColgp_Array1OfPnt2d& ParAndRad,
                                     const Standard_Real         Ud,
                                     const Standard_Real         Uf,
                                     const Standard_Real         Dd,
                                     const Standard_Real         Df,
                                     const Standard_Boolean      Periodic = Standard_False);

  DEFINE_STANDARD_RTTIEXT(Law_Interpol, Law_BSpFunc)

protected:
private:
};

#endif // _Law_Interpol_HeaderFile
