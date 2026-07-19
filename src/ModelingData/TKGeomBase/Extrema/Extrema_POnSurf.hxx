// Created on: 1991-02-21
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

#ifndef _Extrema_POnSurf_HeaderFile
#define _Extrema_POnSurf_HeaderFile

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Definition of a point on surface.
class Extrema_POnSurf
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creation of an indefinite point on surface.
  Extrema_POnSurf()
      : myU(Precision::Infinite()),
        myV(Precision::Infinite()),
        myP(gp_Pnt(Precision::Infinite(), Precision::Infinite(), Precision::Infinite()))
  {
  }

  //! Creation of a point on surface with parameter
  //! values on the surface and a Pnt from gp.
  Extrema_POnSurf(const double theU, const double theV, const gp_Pnt& theP)
      : myU(theU),
        myV(theV),
        myP(theP)
  {
  }

  //! Returns the 3d point.
  const gp_Pnt& Value() const { return myP; }

  //! Sets the params of current POnSurf instance.
  //! (e.g. to the point to be projected).
  void SetParameters(const double theU, const double theV, const gp_Pnt& thePnt)
  {
    myU = theU;
    myV = theV;
    myP = thePnt;
  }

  //! Returns the parameter values on the surface.
  void Parameter(double& theU, double& theV) const
  {
    theU = myU;
    theV = myV;
  }

private:
  double myU;
  double myV;
  gp_Pnt myP;
};

#endif // _Extrema_POnSurf_HeaderFile
