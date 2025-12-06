// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Extrema_POnCurv_HeaderFile
#define _Extrema_POnCurv_HeaderFile

#include <gp_Pnt.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Definition of a point on curve.
class Extrema_POnCurv
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creation of an indefinite point on curve.
  Extrema_POnCurv()
      : myU(0.0)
  {
  }

  //! Creation of a point on curve with a parameter
  //! value on the curve and a Pnt from gp.
  Extrema_POnCurv(const double theU, const gp_Pnt& theP)
      : myU(theU),
        myP(theP)
  {
  }

  //! Sets the point and parameter values.
  void SetValues(const double theU, const gp_Pnt& theP)
  {
    myU = theU;
    myP = theP;
  }

  //! Returns the point.
  const gp_Pnt& Value() const { return myP; }

  //! Returns the parameter on the curve.
  double Parameter() const { return myU; }

private:
  double myU;
  gp_Pnt myP;
};

#endif // _Extrema_POnCurv_HeaderFile
