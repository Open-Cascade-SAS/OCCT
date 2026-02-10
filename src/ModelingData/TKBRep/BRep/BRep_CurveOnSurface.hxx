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

#ifndef _BRep_CurveOnSurface_HeaderFile
#define _BRep_CurveOnSurface_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt2d.hxx>
#include <BRep_GCurve.hxx>
#include <Standard_Real.hxx>
class Geom2d_Curve;
class Geom_Surface;
class TopLoc_Location;
class gp_Pnt;
class BRep_CurveRepresentation;

//! Representation of a curve by a curve in the
//! parametric space of a surface.
class BRep_CurveOnSurface : public BRep_GCurve
{

public:
  Standard_EXPORT BRep_CurveOnSurface(const occ::handle<Geom2d_Curve>& PC,
                                      const occ::handle<Geom_Surface>& S,
                                      const TopLoc_Location&           L);

  void SetUVPoints(const gp_Pnt2d& P1, const gp_Pnt2d& P2);

  void UVPoints(gp_Pnt2d& P1, gp_Pnt2d& P2) const;

  //! Computes the point at parameter U.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const override;

  //! A curve in the parametric space of a surface.
  Standard_EXPORT bool IsCurveOnSurface(const occ::handle<Geom_Surface>& S,
                                        const TopLoc_Location&           L) const override;

  Standard_EXPORT const occ::handle<Geom_Surface>& Surface() const override;

  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurve() const override;

  Standard_EXPORT void PCurve(const occ::handle<Geom2d_Curve>& C) override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Recomputes any derived data after a modification.
  //! This is called when the range is modified.
  Standard_EXPORT void Update() override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_CurveOnSurface, BRep_GCurve)

protected:
  gp_Pnt2d myUV1;
  gp_Pnt2d myUV2;

private:
  occ::handle<Geom2d_Curve> myPCurve;
  occ::handle<Geom_Surface> mySurface;
};

#include <BRep_CurveOnSurface.lxx>

#endif // _BRep_CurveOnSurface_HeaderFile
