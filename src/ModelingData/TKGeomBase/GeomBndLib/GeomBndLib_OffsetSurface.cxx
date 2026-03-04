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

#include <GeomBndLib_OffsetSurface.hxx>

#include <GeomBndLib_Surface.hxx>
#include <Geom_Surface.hxx>

#include <cmath>

//=================================================================================================

Bnd_Box GeomBndLib_OffsetSurface::Box(double theTol) const
{
  double aU1 = 0., aU2 = 0., aV1 = 0., aV2 = 0.;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  return Box(aU1, aU2, aV1, aV2, theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_OffsetSurface::Box(double theUMin,
                                      double theUMax,
                                      double theVMin,
                                      double theVMax,
                                      double theTol) const
{
  const occ::handle<Geom_Surface> anEquiv = myGeom->Surface();
  if (!anEquiv.IsNull())
  {
    GeomBndLib_Surface aSurfEval(anEquiv);
    if (aSurfEval.GetType() != GeomAbs_OtherSurface)
    {
      return aSurfEval.Box(theUMin, theUMax, theVMin, theVMax, theTol);
    }
  }

  // Conservative fallback: basis surface box enlarged by |offset|.
  const occ::handle<Geom_Surface>& aBasis   = myGeom->BasisSurface();
  const double                     anOffset = std::abs(myGeom->Offset());
  GeomBndLib_Surface               aSurfEval(aBasis);
  Bnd_Box aLocalBox = aSurfEval.Box(theUMin, theUMax, theVMin, theVMax, 0.);

  aLocalBox.Enlarge(anOffset);
  aLocalBox.Enlarge(theTol);
  return aLocalBox;
}
