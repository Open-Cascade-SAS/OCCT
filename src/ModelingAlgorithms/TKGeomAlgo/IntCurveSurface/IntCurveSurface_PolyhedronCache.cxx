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

#include <IntCurveSurface_PolyhedronCache.hxx>

#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>

//=================================================================================================

IntCurveSurface_PolyhedronCache::~IntCurveSurface_PolyhedronCache()
{
  Clear();
}

//=================================================================================================

const IntCurveSurface_ThePolyhedronOfHInter& IntCurveSurface_PolyhedronCache::Polyhedron(
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const int                             theNbU,
  const int                             theNbV,
  const double                          theU1,
  const double                          theV1,
  const double                          theU2,
  const double                          theV2)
{
  for (const Entry& anEntry : myEntries)
  {
    if (anEntry.NbU == theNbU && anEntry.NbV == theNbV && anEntry.U1 == theU1 && anEntry.V1 == theV1
        && anEntry.U2 == theU2 && anEntry.V2 == theV2)
    {
      return *anEntry.Polyhedron;
    }
  }
  Entry anEntry;
  anEntry.NbU        = theNbU;
  anEntry.NbV        = theNbV;
  anEntry.U1         = theU1;
  anEntry.V1         = theV1;
  anEntry.U2         = theU2;
  anEntry.V2         = theV2;
  anEntry.Polyhedron = new IntCurveSurface_ThePolyhedronOfHInter(theSurface,
                                                                 theNbU,
                                                                 theNbV,
                                                                 theU1,
                                                                 theV1,
                                                                 theU2,
                                                                 theV2);
  myEntries.Append(anEntry);
  return *anEntry.Polyhedron;
}

//=================================================================================================

void IntCurveSurface_PolyhedronCache::Clear()
{
  for (const Entry& anEntry : myEntries)
  {
    delete anEntry.Polyhedron;
  }
  myEntries.Clear();
}
