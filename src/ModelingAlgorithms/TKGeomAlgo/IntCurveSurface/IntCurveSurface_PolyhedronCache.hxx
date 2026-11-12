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

#ifndef _IntCurveSurface_PolyhedronCache_HeaderFile
#define _IntCurveSurface_PolyhedronCache_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class IntCurveSurface_ThePolyhedronOfHInter;

//! Keeps the polyhedra of ONE surface built by IntCurveSurface_HInter for the intersection of
//! lines with a surface that is not analytic (see IntCurveSurface_HInter::SetPolyhedronCache()),
//! so that the intersections of many lines with the same surface build each polyhedron once.
//! A polyhedron depends only on the surface, its discretization and its parametric bounds,
//! and is reused only when all of them are equal: the results do not change.
//! The cache must be used with one surface only, the user is responsible for that.
class IntCurveSurface_PolyhedronCache
{
public:
  DEFINE_STANDARD_ALLOC

  IntCurveSurface_PolyhedronCache() = default;

  Standard_EXPORT ~IntCurveSurface_PolyhedronCache();

  IntCurveSurface_PolyhedronCache(const IntCurveSurface_PolyhedronCache&)            = delete;
  IntCurveSurface_PolyhedronCache& operator=(const IntCurveSurface_PolyhedronCache&) = delete;

  //! Returns the polyhedron of the surface with theNbU x theNbV samples on the given bounds,
  //! building it on the first request.
  Standard_EXPORT const IntCurveSurface_ThePolyhedronOfHInter& Polyhedron(
    const occ::handle<Adaptor3d_Surface>& theSurface,
    const int                             theNbU,
    const int                             theNbV,
    const double                          theU1,
    const double                          theV1,
    const double                          theU2,
    const double                          theV2);

  //! Returns the number of polyhedra kept.
  int NbPolyhedra() const { return myEntries.Length(); }

  //! Releases all polyhedra.
  Standard_EXPORT void Clear();

private:
  struct Entry
  {
    int                                    NbU;
    int                                    NbV;
    double                                 U1;
    double                                 V1;
    double                                 U2;
    double                                 V2;
    IntCurveSurface_ThePolyhedronOfHInter* Polyhedron;
  };

  NCollection_DynamicArray<Entry> myEntries;
};

#endif // _IntCurveSurface_PolyhedronCache_HeaderFile
