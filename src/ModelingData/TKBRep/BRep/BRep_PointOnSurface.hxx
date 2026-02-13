// Created on: 1993-08-10
// Created by: Remi LEQUETTE
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

#ifndef _BRep_PointOnSurface_HeaderFile
#define _BRep_PointOnSurface_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <BRep_PointsOnSurface.hxx>
class Geom_Surface;
class TopLoc_Location;

//! Representation by two parameters on a surface.
class BRep_PointOnSurface : public BRep_PointsOnSurface
{

public:
  Standard_EXPORT BRep_PointOnSurface(const double                     P1,
                                      const double                     P2,
                                      const occ::handle<Geom_Surface>& S,
                                      const TopLoc_Location&           L);

  Standard_EXPORT bool IsPointOnSurface(const occ::handle<Geom_Surface>& S,
                                        const TopLoc_Location&           L) const override;

  Standard_EXPORT double Parameter2() const override;

  Standard_EXPORT void Parameter2(const double P) override;

  DEFINE_STANDARD_RTTIEXT(BRep_PointOnSurface, BRep_PointsOnSurface)

private:
  double myParameter2;
};

#endif // _BRep_PointOnSurface_HeaderFile
