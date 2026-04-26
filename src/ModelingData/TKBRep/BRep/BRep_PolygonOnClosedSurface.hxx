// Created on: 1995-03-15
// Created by: Laurent PAINNOT
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

#ifndef _BRep_PolygonOnClosedSurface_HeaderFile
#define _BRep_PolygonOnClosedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_PolygonOnSurface.hxx>
class Poly_Polygon2D;
class Geom_Surface;
class TopLoc_Location;
class BRep_CurveRepresentation;

//! Representation by two 2d polygons in the parametric
//! space of a surface.
class BRep_PolygonOnClosedSurface : public BRep_PolygonOnSurface
{

public:
  Standard_EXPORT BRep_PolygonOnClosedSurface(const occ::handle<Poly_Polygon2D>& P1,
                                              const occ::handle<Poly_Polygon2D>& P2,
                                              const occ::handle<Geom_Surface>&   S,
                                              const TopLoc_Location&             L);

  Standard_EXPORT const occ::handle<Poly_Polygon2D>& Polygon2() const override;

  Standard_EXPORT void Polygon2(const occ::handle<Poly_Polygon2D>& P) override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_PolygonOnClosedSurface, BRep_PolygonOnSurface)

private:
  occ::handle<Poly_Polygon2D> myPolygon2;
};

#endif // _BRep_PolygonOnClosedSurface_HeaderFile
