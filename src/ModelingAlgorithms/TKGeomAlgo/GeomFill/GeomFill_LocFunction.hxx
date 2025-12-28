// Created on: 1998-02-02
// Created by: Philippe MANGIN
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

#ifndef _GeomFill_LocFunction_HeaderFile
#define _GeomFill_LocFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Mat.hxx>
#include <Standard_Integer.hxx>
class GeomFill_LocationLaw;

class GeomFill_LocFunction
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_LocFunction(const occ::handle<GeomFill_LocationLaw>& Law);

  //! compute the section for v = param
  Standard_EXPORT bool D0(const double Param,
                                      const double First,
                                      const double Last);

  //! compute the first derivative in v direction of the
  //! section for v = param
  Standard_EXPORT bool D1(const double Param,
                                      const double First,
                                      const double Last);

  //! compute the second derivative in v direction of the
  //! section for v = param
  Standard_EXPORT bool D2(const double Param,
                                      const double First,
                                      const double Last);

  Standard_EXPORT void DN(const double    Param,
                          const double    First,
                          const double    Last,
                          const int Order,
                          double&         Result,
                          int&      Ier);

private:
  occ::handle<GeomFill_LocationLaw> myLaw;
  NCollection_Array1<gp_Vec>           V;
  NCollection_Array1<gp_Vec>           DV;
  NCollection_Array1<gp_Vec>           D2V;
  gp_Mat                       M;
  gp_Mat                       DM;
  gp_Mat                       D2M;
};

#endif // _GeomFill_LocFunction_HeaderFile
