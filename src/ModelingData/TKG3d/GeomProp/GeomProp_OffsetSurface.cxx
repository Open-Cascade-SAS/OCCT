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

#include <GeomProp_OffsetSurface.hxx>

//=================================================================================================

GeomProp::SurfaceNormalResult GeomProp_OffsetSurface::Normal(const double theU,
                                                             const double theV,
                                                             const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V;
  myAdaptor->D1(theU, theV, aPnt, aD1U, aD1V);
  return GeomProp::ComputeSurfaceNormal(aD1U, aD1V, theTol);
}

//=================================================================================================

GeomProp::SurfaceCurvatureResult GeomProp_OffsetSurface::Curvatures(const double theU,
                                                                    const double theV,
                                                                    const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {};
  }
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  myAdaptor->D2(theU, theV, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
  return GeomProp::ComputeSurfaceCurvatures(aD1U, aD1V, aD2U, aD2V, aD2UV, theTol);
}

//=================================================================================================

GeomProp::MeanGaussianResult GeomProp_OffsetSurface::MeanGaussian(const double theU,
                                                                  const double theV,
                                                                  const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {};
  }
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  myAdaptor->D2(theU, theV, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
  return GeomProp::ComputeMeanGaussian(aD1U, aD1V, aD2U, aD2V, aD2UV, theTol);
}
