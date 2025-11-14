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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
class gp_Pnt;

class Extrema_POnCurv
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creation of an indefinite point on curve.
  Standard_EXPORT Extrema_POnCurv();

  //! Creation of a point on curve with a parameter
  //! value on the curve and a Pnt from gp.
  Standard_EXPORT Extrema_POnCurv(const Standard_Real U, const gp_Pnt& P);

  //! sets the point and parameter values.
  Standard_EXPORT void SetValues(const Standard_Real U, const gp_Pnt& P);

  //! Returns the point.
  const gp_Pnt& Value() const;

  //! Returns the parameter on the curve.
  Standard_Real Parameter() const;

protected:
private:
  Standard_Real myU;
  gp_Pnt        myP;
};

//=================================================================================================
// Inline implementations
//=================================================================================================

inline Standard_Real Extrema_POnCurv::Parameter() const
{
  return myU;
}

inline const gp_Pnt& Extrema_POnCurv::Value() const
{
  return myP;
}

#endif // _Extrema_POnCurv_HeaderFile
