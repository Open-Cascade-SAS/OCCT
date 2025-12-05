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
  const Handle(Geom_Surface)& theSurface) const noexcept
{
  if (theSurface.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual surface type
  if (Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(theSurface))
  {
    return GeomHash_PlaneHasher{}(aPlane);
  }
  if (Handle(Geom_CylindricalSurface) aCylinder =
        Handle(Geom_CylindricalSurface)::DownCast(theSurface))
  {
    return GeomHash_CylindricalSurfaceHasher{}(aCylinder);
  }
  if (Handle(Geom_ConicalSurface) aCone = Handle(Geom_ConicalSurface)::DownCast(theSurface))
  {
    return GeomHash_ConicalSurfaceHasher{}(aCone);
  }
  if (Handle(Geom_SphericalSurface) aSphere = Handle(Geom_SphericalSurface)::DownCast(theSurface))
  {
    return GeomHash_SphericalSurfaceHasher{}(aSphere);
  }
  if (Handle(Geom_ToroidalSurface) aTorus = Handle(Geom_ToroidalSurface)::DownCast(theSurface))
  {
    return GeomHash_ToroidalSurfaceHasher{}(aTorus);
  }
  if (Handle(Geom_SurfaceOfRevolution) aRevol =
        Handle(Geom_SurfaceOfRevolution)::DownCast(theSurface))
  {
    return GeomHash_SurfaceOfRevolutionHasher{}(aRevol);
  }
  if (Handle(Geom_SurfaceOfLinearExtrusion) aExtr =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theSurface))
  {
    return GeomHash_SurfaceOfLinearExtrusionHasher{}(aExtr);
  }
  if (Handle(Geom_BezierSurface) aBezier = Handle(Geom_BezierSurface)::DownCast(theSurface))
  {
    return GeomHash_BezierSurfaceHasher{}(aBezier);
  }
  if (Handle(Geom_BSplineSurface) aBSpline = Handle(Geom_BSplineSurface)::DownCast(theSurface))
  {
    return GeomHash_BSplineSurfaceHasher{}(aBSpline);
  }
  if (Handle(Geom_RectangularTrimmedSurface) aTrimmed =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(theSurface))
  {
    return GeomHash_RectangularTrimmedSurfaceHasher{}(aTrimmed);
  }
  if (Handle(Geom_OffsetSurface) aOffset = Handle(Geom_OffsetSurface)::DownCast(theSurface))
  {
    return GeomHash_OffsetSurfaceHasher{}(aOffset);
  }

  // Unknown surface type - hash the type name
  return Standard_CStringHasher{}(theSurface->DynamicType()->Name());
}

//=================================================================================================

bool GeomHash_SurfaceHasher::operator()(const Handle(Geom_Surface)& theSurface1,
                                        const Handle(Geom_Surface)& theSurface2) const noexcept
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
  if (Handle(Geom_Plane) aPlane1 = Handle(Geom_Plane)::DownCast(theSurface1))
  {
    return GeomHash_PlaneHasher{}(aPlane1, Handle(Geom_Plane)::DownCast(theSurface2));
  }
  if (Handle(Geom_CylindricalSurface) aCyl1 =
        Handle(Geom_CylindricalSurface)::DownCast(theSurface1))
  {
    return GeomHash_CylindricalSurfaceHasher{}(
      aCyl1,
      Handle(Geom_CylindricalSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_ConicalSurface) aCone1 = Handle(Geom_ConicalSurface)::DownCast(theSurface1))
  {
    return GeomHash_ConicalSurfaceHasher{}(aCone1,
                                           Handle(Geom_ConicalSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_SphericalSurface) aSph1 = Handle(Geom_SphericalSurface)::DownCast(theSurface1))
  {
    return GeomHash_SphericalSurfaceHasher{}(aSph1,
                                             Handle(Geom_SphericalSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_ToroidalSurface) aTor1 = Handle(Geom_ToroidalSurface)::DownCast(theSurface1))
  {
    return GeomHash_ToroidalSurfaceHasher{}(aTor1,
                                            Handle(Geom_ToroidalSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_SurfaceOfRevolution) aRev1 =
        Handle(Geom_SurfaceOfRevolution)::DownCast(theSurface1))
  {
    return GeomHash_SurfaceOfRevolutionHasher{}(
      aRev1,
      Handle(Geom_SurfaceOfRevolution)::DownCast(theSurface2));
  }
  if (Handle(Geom_SurfaceOfLinearExtrusion) aExt1 =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theSurface1))
  {
    return GeomHash_SurfaceOfLinearExtrusionHasher{}(
      aExt1,
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theSurface2));
  }
  if (Handle(Geom_BezierSurface) aBez1 = Handle(Geom_BezierSurface)::DownCast(theSurface1))
  {
    return GeomHash_BezierSurfaceHasher{}(aBez1, Handle(Geom_BezierSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_BSplineSurface) aBSpl1 = Handle(Geom_BSplineSurface)::DownCast(theSurface1))
  {
    return GeomHash_BSplineSurfaceHasher{}(aBSpl1,
                                           Handle(Geom_BSplineSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_RectangularTrimmedSurface) aTrim1 =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(theSurface1))
  {
    return GeomHash_RectangularTrimmedSurfaceHasher{}(
      aTrim1,
      Handle(Geom_RectangularTrimmedSurface)::DownCast(theSurface2));
  }
  if (Handle(Geom_OffsetSurface) aOff1 = Handle(Geom_OffsetSurface)::DownCast(theSurface1))
  {
    return GeomHash_OffsetSurfaceHasher{}(aOff1, Handle(Geom_OffsetSurface)::DownCast(theSurface2));
  }

  // Unknown surface type - compare by pointer
  return theSurface1.get() == theSurface2.get();
}
