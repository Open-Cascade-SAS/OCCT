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

#ifndef _BRep_CurveOn2Surfaces_HeaderFile
#define _BRep_CurveOn2Surfaces_HeaderFile

#include <Standard.hxx>

#include <BRep_CurveRepresentation.hxx>
#include <Standard_Real.hxx>
class Geom_Surface;
class gp_Pnt;

//! Defines a continuity between two surfaces.
class BRep_CurveOn2Surfaces : public BRep_CurveRepresentation
{

public:
  Standard_EXPORT BRep_CurveOn2Surfaces(const occ::handle<Geom_Surface>& S1,
                                        const occ::handle<Geom_Surface>& S2,
                                        const TopLoc_Location&           L1,
                                        const TopLoc_Location&           L2,
                                        const GeomAbs_Shape              C);

  //! A curve on two surfaces (continuity).
  Standard_EXPORT bool IsRegularity(const occ::handle<Geom_Surface>& S1,
                                    const occ::handle<Geom_Surface>& S2,
                                    const TopLoc_Location&           L1,
                                    const TopLoc_Location&           L2) const override;

  //! Raises an error.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const;

  Standard_EXPORT const occ::handle<Geom_Surface>& Surface() const override;

  Standard_EXPORT const occ::handle<Geom_Surface>& Surface2() const override;

  Standard_EXPORT const TopLoc_Location& Location2() const override;

  Standard_EXPORT const GeomAbs_Shape& Continuity() const override;

  Standard_EXPORT void Continuity(const GeomAbs_Shape C) override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_CurveOn2Surfaces, BRep_CurveRepresentation)

private:
  occ::handle<Geom_Surface> mySurface;
  occ::handle<Geom_Surface> mySurface2;
  TopLoc_Location           myLocation2;
  GeomAbs_Shape             myContinuity;
};

#endif // _BRep_CurveOn2Surfaces_HeaderFile
