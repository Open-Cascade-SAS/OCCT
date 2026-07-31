// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _GeomHash_Polygon3DHasher_HeaderFile
#define _GeomHash_Polygon3DHasher_HeaderFile

#include <Precision.hxx>
#include <Standard_Handle.hxx>

#include <cstddef>

class Poly_Polygon3D;

struct GeomHash_Polygon3DHasher
{
  double CompTolerance;
  double HashTolerance;

  Standard_EXPORT GeomHash_Polygon3DHasher(
    const double theCompTolerance = Precision::Computational(),
    const double theHashTolerance = Precision::Computational());

  Standard_EXPORT std::size_t operator()(const occ::handle<Poly_Polygon3D>& thePoly) const noexcept;

  Standard_EXPORT bool operator()(const occ::handle<Poly_Polygon3D>& thePoly1,
                                  const occ::handle<Poly_Polygon3D>& thePoly2) const noexcept;
};

#endif // _GeomHash_Polygon3DHasher_HeaderFile
