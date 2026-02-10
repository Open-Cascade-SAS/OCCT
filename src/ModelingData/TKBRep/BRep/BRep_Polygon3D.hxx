// Created on: 1995-03-09
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

#ifndef _BRep_Polygon3D_HeaderFile
#define _BRep_Polygon3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_CurveRepresentation.hxx>
class Poly_Polygon3D;
class TopLoc_Location;

//! Representation by a 3D polygon.
class BRep_Polygon3D : public BRep_CurveRepresentation
{

public:
  Standard_EXPORT BRep_Polygon3D(const occ::handle<Poly_Polygon3D>& P, const TopLoc_Location& L);

  Standard_EXPORT const occ::handle<Poly_Polygon3D>& Polygon3D() const override;

  Standard_EXPORT void Polygon3D(const occ::handle<Poly_Polygon3D>& P) override;

  //! Return a copy of this representation.
  Standard_EXPORT occ::handle<BRep_CurveRepresentation> Copy() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_Polygon3D, BRep_CurveRepresentation)

private:
  occ::handle<Poly_Polygon3D> myPolygon3D;
};

#endif // _BRep_Polygon3D_HeaderFile
