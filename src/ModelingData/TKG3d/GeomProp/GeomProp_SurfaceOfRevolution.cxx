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

#include <GeomProp_SurfaceOfRevolution.hxx>
#include <GeomProp_SurfaceAnalysisTools.pxx>

//=================================================================================================

GeomProp::SurfaceNormalResult GeomProp_SurfaceOfRevolution::Normal(const double theU,
                                                                   const double theV,
                                                                   const double theTol) const
{
  return GeomProp_SurfaceAnalysisTools::EvaluateNormalCached(
    myAdaptor, theU, theV, theTol, myRequestedOrder, myCache);
}

//=================================================================================================

GeomProp::SurfaceCurvatureResult GeomProp_SurfaceOfRevolution::Curvatures(const double theU,
                                                                          const double theV,
                                                                          const double theTol) const
{
  return GeomProp_SurfaceAnalysisTools::EvaluateCurvaturesCached(
    myAdaptor, theU, theV, theTol, myRequestedOrder, myCache);
}

//=================================================================================================

GeomProp::MeanGaussianResult GeomProp_SurfaceOfRevolution::MeanGaussian(const double theU,
                                                                        const double theV,
                                                                        const double theTol) const
{
  return GeomProp_SurfaceAnalysisTools::EvaluateMeanGaussianCached(
    myAdaptor, theU, theV, theTol, myRequestedOrder, myCache);
}
