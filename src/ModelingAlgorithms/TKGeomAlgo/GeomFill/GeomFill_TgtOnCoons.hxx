// Created on: 1995-12-04
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

#ifndef _GeomFill_TgtOnCoons_HeaderFile
#define _GeomFill_TgtOnCoons_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <GeomFill_TgtField.hxx>
class GeomFill_CoonsAlgPatch;
class gp_Vec;

//! Defines an algorithmic tangents field on a
//! boundary of a CoonsAlgPatch.
class GeomFill_TgtOnCoons : public GeomFill_TgtField
{

public:
  Standard_EXPORT GeomFill_TgtOnCoons(const occ::handle<GeomFill_CoonsAlgPatch>& K, const int I);

  //! Computes the value of the field of tangency at
  //! parameter W.
  Standard_EXPORT gp_Vec Value(const double W) const override;

  //! Computes the derivative of the field of tangency at
  //! parameter W.
  Standard_EXPORT gp_Vec D1(const double W) const override;

  //! Computes the value and the derivative of the field of
  //! tangency at parameter W.
  Standard_EXPORT void D1(const double W, gp_Vec& T, gp_Vec& DT) const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_TgtOnCoons, GeomFill_TgtField)

private:
  occ::handle<GeomFill_CoonsAlgPatch> myK;
  int                                 ibound;
};

#endif // _GeomFill_TgtOnCoons_HeaderFile
