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

GeomHash_SurfaceHasher::GeomHash_SurfaceHasher(double theCompTolerance, double theHashTolerance)
    : CompTolerance(theCompTolerance),
      HashTolerance(theHashTolerance)
{
}

//=================================================================================================

std::size_t GeomHash_SurfaceHasher::operator()(
  const occ::handle<Geom_Surface>& theSurface) const noexcept
{
  if (theSurface.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual surface type using DynamicType check first (cheaper than down_cast)
  const Handle(Standard_Type)& aType = theSurface->DynamicType();
  if (aType == STANDARD_TYPE(Geom_Plane))
  {
    return GeomHash_PlaneHasher{CompTolerance,
                                HashTolerance}(occ::down_cast<Geom_Plane>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    return GeomHash_CylindricalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_CylindricalSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    return GeomHash_ConicalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_ConicalSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    return GeomHash_SphericalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_SphericalSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    return GeomHash_ToroidalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_ToroidalSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    return GeomHash_SurfaceOfRevolutionHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_SurfaceOfRevolution>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
    return GeomHash_SurfaceOfLinearExtrusionHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_BezierSurface))
  {
    return GeomHash_BezierSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BezierSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    return GeomHash_BSplineSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BSplineSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    return GeomHash_RectangularTrimmedSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface));
  }
  else if (aType == STANDARD_TYPE(Geom_OffsetSurface))
  {
    return GeomHash_OffsetSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_OffsetSurface>(theSurface));
  }

  // Unknown surface type - hash the type name
  return Standard_CStringHasher{}(aType->Name());
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

  // Dispatch based on actual surface type using DynamicType check (already confirmed types match)
  const Handle(Standard_Type)& aType = theSurface1->DynamicType();
  if (aType == STANDARD_TYPE(Geom_Plane))
  {
    return GeomHash_PlaneHasher{CompTolerance,
                                HashTolerance}(occ::down_cast<Geom_Plane>(theSurface1),
                                               occ::down_cast<Geom_Plane>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    return GeomHash_CylindricalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_CylindricalSurface>(theSurface1),
      occ::down_cast<Geom_CylindricalSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    return GeomHash_ConicalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_ConicalSurface>(theSurface1),
      occ::down_cast<Geom_ConicalSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    return GeomHash_SphericalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_SphericalSurface>(theSurface1),
      occ::down_cast<Geom_SphericalSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    return GeomHash_ToroidalSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_ToroidalSurface>(theSurface1),
      occ::down_cast<Geom_ToroidalSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    return GeomHash_SurfaceOfRevolutionHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_SurfaceOfRevolution>(theSurface1),
      occ::down_cast<Geom_SurfaceOfRevolution>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
    return GeomHash_SurfaceOfLinearExtrusionHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface1),
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_BezierSurface))
  {
    return GeomHash_BezierSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BezierSurface>(theSurface1),
      occ::down_cast<Geom_BezierSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    return GeomHash_BSplineSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BSplineSurface>(theSurface1),
      occ::down_cast<Geom_BSplineSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    return GeomHash_RectangularTrimmedSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface1),
      occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface2));
  }
  else if (aType == STANDARD_TYPE(Geom_OffsetSurface))
  {
    return GeomHash_OffsetSurfaceHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_OffsetSurface>(theSurface1),
      occ::down_cast<Geom_OffsetSurface>(theSurface2));
  }

  // Unknown surface type - compare by pointer
  return theSurface1.get() == theSurface2.get();
}
