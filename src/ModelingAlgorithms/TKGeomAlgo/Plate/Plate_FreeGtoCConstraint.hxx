// Created on: 1998-03-26
// Created by: # Andre LIEUTIER
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

#ifndef _Plate_FreeGtoCConstraint_HeaderFile
#define _Plate_FreeGtoCConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_XY.hxx>
#include <Standard_Integer.hxx>
#include <Plate_LinearScalarConstraint.hxx>
class Plate_D1;
class Plate_D2;
class Plate_D3;

//! define a G1, G2 or G3 constraint on the Plate using weaker
//! constraint than GtoCConstraint
class Plate_FreeGtoCConstraint
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Plate_FreeGtoCConstraint(const gp_XY&    point2d,
                                           const Plate_D1& D1S,
                                           const Plate_D1& D1T,
                                           const double    IncrementalLoad = 1.0,
                                           const int       orientation     = 0);

  Standard_EXPORT Plate_FreeGtoCConstraint(const gp_XY&    point2d,
                                           const Plate_D1& D1S,
                                           const Plate_D1& D1T,
                                           const Plate_D2& D2S,
                                           const Plate_D2& D2T,
                                           const double    IncrementalLoad = 1.0,
                                           const int       orientation     = 0);

  Standard_EXPORT Plate_FreeGtoCConstraint(const gp_XY&    point2d,
                                           const Plate_D1& D1S,
                                           const Plate_D1& D1T,
                                           const Plate_D2& D2S,
                                           const Plate_D2& D2T,
                                           const Plate_D3& D3S,
                                           const Plate_D3& D3T,
                                           const double    IncrementalLoad = 1.0,
                                           const int       orientation     = 0);

  const int& nb_PPC() const;

  const Plate_PinpointConstraint& GetPPC(const int Index) const;

  const int& nb_LSC() const;

  const Plate_LinearScalarConstraint& LSC(const int Index) const;

private:
  gp_XY                        pnt2d;
  int                          nb_PPConstraints;
  int                          nb_LSConstraints;
  Plate_PinpointConstraint     myPPC[5];
  Plate_LinearScalarConstraint myLSC[4];
};

#include <Plate_FreeGtoCConstraint.lxx>

#endif // _Plate_FreeGtoCConstraint_HeaderFile
