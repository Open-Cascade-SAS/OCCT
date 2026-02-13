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

#ifndef _BRep_CurveOnClosedSurface_HeaderFile
#define _BRep_CurveOnClosedSurface_HeaderFile

#include <Standard.hxx>

#include <BRep_CurveOnSurface.hxx>
class Geom2d_Curve;
class Geom_Surface;
class TopLoc_Location;
class BRep_CurveRepresentation;

//! Representation of a curve by two pcurves on
//! a closed surface.
class BRep_CurveOnClosedSurface : public BRep_CurveOnSurface
{

public:
  Standard_EXPORT BRep_CurveOnClosedSurface(const occ::handle<Geom2d_Curve>& PC1,
                                            const occ::handle<Geom2d_Curve>& PC2,
                                            const occ::handle<Geom_Surface>& S,
                                            const TopLoc_Location&           L,
                                            const GeomAbs_Shape              C);

  void SetUVPoints2(const gp_Pnt2d& P1, const gp_Pnt2d& P2);

  void UVPoints2(gp_Pnt2d& P1, gp_Pnt2d& P2) const;

  //! A curve on two surfaces (continuity).
  Standard_EXPORT bool IsRegularity(const occ::handle<Geom_Surface>& S1,
                                    const occ::handle<Geom_Surface>& S2,
                                    const TopLoc_Location&           L1,
                                    const TopLoc_Location&           L2) const override;

  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurve2() const override;

  //! Returns Surface()
  Standard_EXPORT const occ::handle<Geom_Surface>& Surface2() const override;

  //! Returns Location()
  Standard_EXPORT const TopLoc_Location& Location2() const override;

  Standard_EXPORT const GeomAbs_Shape& Continuity() const override;

  Standard_EXPORT void Continuity(const GeomAbs_Shape C) override;

  Standard_EXPORT void PCurve2(const occ::handle<Geom2d_Curve>& C) override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Recomputes any derived data after a modification.
  //! This is called when the range is modified.
  Standard_EXPORT void Update() override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_CurveOnClosedSurface, BRep_CurveOnSurface)

private:
  occ::handle<Geom2d_Curve> myPCurve2;
  GeomAbs_Shape             myContinuity;
  gp_Pnt2d                  myUV21;
  gp_Pnt2d                  myUV22;
};

#include <BRep_CurveOnClosedSurface.lxx>

#endif // _BRep_CurveOnClosedSurface_HeaderFile
