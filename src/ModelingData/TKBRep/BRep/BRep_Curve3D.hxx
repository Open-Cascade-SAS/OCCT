// Created on: 1993-07-06
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

#ifndef _BRep_Curve3D_HeaderFile
#define _BRep_Curve3D_HeaderFile

#include <Standard.hxx>

#include <BRep_GCurve.hxx>
#include <Standard_Real.hxx>
class Geom_Curve;
class TopLoc_Location;
class gp_Pnt;
class BRep_CurveRepresentation;

//! Representation of a curve by a 3D curve.
class BRep_Curve3D : public BRep_GCurve
{

public:
  Standard_EXPORT BRep_Curve3D(const occ::handle<Geom_Curve>& C, const TopLoc_Location& L);

  //! Computes the point at parameter U.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const override;

  Standard_EXPORT const occ::handle<Geom_Curve>& Curve3D() const override;

  Standard_EXPORT void Curve3D(const occ::handle<Geom_Curve>& C) override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_Curve3D, BRep_GCurve)

private:
  occ::handle<Geom_Curve> myCurve;
};

#endif // _BRep_Curve3D_HeaderFile
