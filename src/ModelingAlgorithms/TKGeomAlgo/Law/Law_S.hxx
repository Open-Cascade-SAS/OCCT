// Created on: 1993-12-24
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Law_S_HeaderFile
#define _Law_S_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Law_BSpFunc.hxx>

class Law_S;
DEFINE_STANDARD_HANDLE(Law_S, Law_BSpFunc)

//! Describes an "S" evolution law.
class Law_S : public Law_BSpFunc
{

public:
  //! Constructs an empty "S" evolution law.
  Standard_EXPORT Law_S();

  //! Defines this S evolution law by assigning both:
  //! -   the bounds Pdeb and Pfin of the parameter, and
  //! -   the values Valdeb and Valfin of the function at these
  //! two parametric bounds.
  //! The function is assumed to have the first derivatives
  //! equal to 0 at the two parameter points Pdeb and Pfin.
  Standard_EXPORT void Set(const Standard_Real Pdeb,
                           const Standard_Real Valdeb,
                           const Standard_Real Pfin,
                           const Standard_Real Valfin);

  //! Defines this S evolution law by assigning
  //! -   the bounds Pdeb and Pfin of the parameter,
  //! -   the values Valdeb and Valfin of the function at these
  //! two parametric bounds, and
  //! -   the values Ddeb and Dfin of the first derivative of the
  //! function at these two parametric bounds.
  Standard_EXPORT void Set(const Standard_Real Pdeb,
                           const Standard_Real Valdeb,
                           const Standard_Real Ddeb,
                           const Standard_Real Pfin,
                           const Standard_Real Valfin,
                           const Standard_Real Dfin);

  DEFINE_STANDARD_RTTIEXT(Law_S, Law_BSpFunc)

protected:
private:
};

#endif // _Law_S_HeaderFile
