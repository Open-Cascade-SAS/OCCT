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

#ifndef _BRep_PolygonOnClosedTriangulation_HeaderFile
#define _BRep_PolygonOnClosedTriangulation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_PolygonOnTriangulation.hxx>
class Poly_PolygonOnTriangulation;
class Poly_Triangulation;
class TopLoc_Location;
class BRep_CurveRepresentation;

//! A representation by two arrays of nodes on a
//! triangulation.
class BRep_PolygonOnClosedTriangulation : public BRep_PolygonOnTriangulation
{

public:
  Standard_EXPORT BRep_PolygonOnClosedTriangulation(
    const occ::handle<Poly_PolygonOnTriangulation>& P1,
    const occ::handle<Poly_PolygonOnTriangulation>& P2,
    const occ::handle<Poly_Triangulation>&          Tr,
    const TopLoc_Location&                          L);

  Standard_EXPORT void PolygonOnTriangulation2(
    const occ::handle<Poly_PolygonOnTriangulation>& P2) override;

  Standard_EXPORT const occ::handle<Poly_PolygonOnTriangulation>& PolygonOnTriangulation2()
    const override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_PolygonOnClosedTriangulation, BRep_PolygonOnTriangulation)

private:
  occ::handle<Poly_PolygonOnTriangulation> myPolygon2;
};

#endif // _BRep_PolygonOnClosedTriangulation_HeaderFile
