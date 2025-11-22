// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _GeomHash_SurfaceHasher_HeaderFile
#define _GeomHash_SurfaceHasher_HeaderFile

#include <Standard_Handle.hxx>
#include <cstddef>

class Geom_Surface;

//! Polymorphic hasher for Geom_Surface using RTTI dispatch.
//! Used for geometry deduplication.
struct GeomHash_SurfaceHasher
{
  // Hashes any Geom_Surface by dispatching to the appropriate specific hasher.
  Standard_EXPORT std::size_t operator()(const Handle(Geom_Surface)& theSurface) const noexcept;

  // Compares two surfaces using polymorphic dispatch.
  Standard_EXPORT bool operator()(const Handle(Geom_Surface)& theSurface1,
                                  const Handle(Geom_Surface)& theSurface2) const noexcept;
};

#endif // _GeomHash_SurfaceHasher_HeaderFile
