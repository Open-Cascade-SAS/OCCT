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

#include <GeomHash_SurfaceHasher.hxx>

#include <Standard_CStringHasher.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>

#include <GeomHash_PlaneHasher.pxx>
#include <GeomHash_CylindricalSurfaceHasher.pxx>
#include <GeomHash_ConicalSurfaceHasher.pxx>
#include <GeomHash_SphericalSurfaceHasher.pxx>
#include <GeomHash_ToroidalSurfaceHasher.pxx>
#include <GeomHash_SurfaceOfRevolutionHasher.pxx>
#include <GeomHash_SurfaceOfLinearExtrusionHasher.pxx>
#include <GeomHash_BezierSurfaceHasher.pxx>
#include <GeomHash_BSplineSurfaceHasher.pxx>
#include <GeomHash_RectangularTrimmedSurfaceHasher.pxx>
#include <GeomHash_OffsetSurfaceHasher.pxx>

//=================================================================================================

std::size_t GeomHash_SurfaceHasher::operator()(
  const occ::handle<Geom_Surface>& theSurface) const noexcept
{
  if (theSurface.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual surface type
  if (occ::handle<Geom_Plane> aPlane = occ::down_cast<Geom_Plane>(theSurface))
  {
    return GeomHash_PlaneHasher{}(aPlane);
  }
  if (occ::handle<Geom_CylindricalSurface> aCylinder =
        occ::down_cast<Geom_CylindricalSurface>(theSurface))
  {
    return GeomHash_CylindricalSurfaceHasher{}(aCylinder);
  }
  if (occ::handle<Geom_ConicalSurface> aCone = occ::down_cast<Geom_ConicalSurface>(theSurface))
  {
    return GeomHash_ConicalSurfaceHasher{}(aCone);
  }
  if (occ::handle<Geom_SphericalSurface> aSphere = occ::down_cast<Geom_SphericalSurface>(theSurface))
  {
    return GeomHash_SphericalSurfaceHasher{}(aSphere);
  }
  if (occ::handle<Geom_ToroidalSurface> aTorus = occ::down_cast<Geom_ToroidalSurface>(theSurface))
  {
    return GeomHash_ToroidalSurfaceHasher{}(aTorus);
  }
  if (occ::handle<Geom_SurfaceOfRevolution> aRevol =
        occ::down_cast<Geom_SurfaceOfRevolution>(theSurface))
  {
    return GeomHash_SurfaceOfRevolutionHasher{}(aRevol);
  }
  if (occ::handle<Geom_SurfaceOfLinearExtrusion> aExtr =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface))
  {
    return GeomHash_SurfaceOfLinearExtrusionHasher{}(aExtr);
  }
  if (occ::handle<Geom_BezierSurface> aBezier = occ::down_cast<Geom_BezierSurface>(theSurface))
  {
    return GeomHash_BezierSurfaceHasher{}(aBezier);
  }
  if (occ::handle<Geom_BSplineSurface> aBSpline = occ::down_cast<Geom_BSplineSurface>(theSurface))
  {
    return GeomHash_BSplineSurfaceHasher{}(aBSpline);
  }
  if (occ::handle<Geom_RectangularTrimmedSurface> aTrimmed =
        occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface))
  {
    return GeomHash_RectangularTrimmedSurfaceHasher{}(aTrimmed);
  }
  if (occ::handle<Geom_OffsetSurface> aOffset = occ::down_cast<Geom_OffsetSurface>(theSurface))
  {
    return GeomHash_OffsetSurfaceHasher{}(aOffset);
  }

  // Unknown surface type - hash the type name
  return Standard_CStringHasher{}(theSurface->DynamicType()->Name());
}

//=================================================================================================

bool GeomHash_SurfaceHasher::operator()(const occ::handle<Geom_Surface>& theSurface1,
                                        const occ::handle<Geom_Surface>& theSurface2) const noexcept
{
  if (theSurface1.IsNull() || theSurface2.IsNull())
  {
    return theSurface1.IsNull() && theSurface2.IsNull();
  }

  if (theSurface1 == theSurface2)
  {
    return true;
  }

  // Must be same type
  if (theSurface1->DynamicType() != theSurface2->DynamicType())
  {
    return false;
  }

  // Dispatch based on actual surface type
  if (occ::handle<Geom_Plane> aPlane1 = occ::down_cast<Geom_Plane>(theSurface1))
  {
    return GeomHash_PlaneHasher{}(aPlane1, occ::down_cast<Geom_Plane>(theSurface2));
  }
  if (occ::handle<Geom_CylindricalSurface> aCyl1 =
        occ::down_cast<Geom_CylindricalSurface>(theSurface1))
  {
    return GeomHash_CylindricalSurfaceHasher{}(
      aCyl1,
      occ::down_cast<Geom_CylindricalSurface>(theSurface2));
  }
  if (occ::handle<Geom_ConicalSurface> aCone1 = occ::down_cast<Geom_ConicalSurface>(theSurface1))
  {
    return GeomHash_ConicalSurfaceHasher{}(aCone1,
                                           occ::down_cast<Geom_ConicalSurface>(theSurface2));
  }
  if (occ::handle<Geom_SphericalSurface> aSph1 = occ::down_cast<Geom_SphericalSurface>(theSurface1))
  {
    return GeomHash_SphericalSurfaceHasher{}(aSph1,
                                             occ::down_cast<Geom_SphericalSurface>(theSurface2));
  }
  if (occ::handle<Geom_ToroidalSurface> aTor1 = occ::down_cast<Geom_ToroidalSurface>(theSurface1))
  {
    return GeomHash_ToroidalSurfaceHasher{}(aTor1,
                                            occ::down_cast<Geom_ToroidalSurface>(theSurface2));
  }
  if (occ::handle<Geom_SurfaceOfRevolution> aRev1 =
        occ::down_cast<Geom_SurfaceOfRevolution>(theSurface1))
  {
    return GeomHash_SurfaceOfRevolutionHasher{}(
      aRev1,
      occ::down_cast<Geom_SurfaceOfRevolution>(theSurface2));
  }
  if (occ::handle<Geom_SurfaceOfLinearExtrusion> aExt1 =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface1))
  {
    return GeomHash_SurfaceOfLinearExtrusionHasher{}(
      aExt1,
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface2));
  }
  if (occ::handle<Geom_BezierSurface> aBez1 = occ::down_cast<Geom_BezierSurface>(theSurface1))
  {
    return GeomHash_BezierSurfaceHasher{}(aBez1, occ::down_cast<Geom_BezierSurface>(theSurface2));
  }
  if (occ::handle<Geom_BSplineSurface> aBSpl1 = occ::down_cast<Geom_BSplineSurface>(theSurface1))
  {
    return GeomHash_BSplineSurfaceHasher{}(aBSpl1,
                                           occ::down_cast<Geom_BSplineSurface>(theSurface2));
  }
  if (occ::handle<Geom_RectangularTrimmedSurface> aTrim1 =
        occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface1))
  {
    return GeomHash_RectangularTrimmedSurfaceHasher{}(
      aTrim1,
      occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface2));
  }
  if (occ::handle<Geom_OffsetSurface> aOff1 = occ::down_cast<Geom_OffsetSurface>(theSurface1))
  {
    return GeomHash_OffsetSurfaceHasher{}(aOff1, occ::down_cast<Geom_OffsetSurface>(theSurface2));
  }

  // Unknown surface type - compare by pointer
  return theSurface1.get() == theSurface2.get();
}
