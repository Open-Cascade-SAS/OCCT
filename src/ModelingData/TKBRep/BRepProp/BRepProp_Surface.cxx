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

#include <BRepProp_Surface.hxx>

#include <TopoDS_Face.hxx>

//==================================================================================================

void BRepProp_Surface::Initialize(const TopoDS_Face& theFace)
{
  if (theFace.IsNull())
  {
    myOwned.Nullify();
    myPtr = nullptr;
    return;
  }
  myOwned = new BRepAdaptor_Surface(theFace);
  myPtr   = myOwned.get();
}

//==================================================================================================

void BRepProp_Surface::Initialize(const BRepAdaptor_Surface& theSurface)
{
  myOwned.Nullify();
  myPtr = &theSurface;
}

//==================================================================================================

void BRepProp_Surface::Initialize(const occ::handle<BRepAdaptor_Surface>& theSurface)
{
  myOwned = theSurface;
  myPtr   = myOwned.get();
}

//==================================================================================================

GeomProp::SurfaceNormalResult BRepProp_Surface::Normal(const double theU,
                                                       const double theV,
                                                       const double theTol) const
{
  if (!IsInitialized())
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V;
  myPtr->D1(theU, theV, aPnt, aD1U, aD1V);
  return GeomProp::ComputeSurfaceNormal(aD1U, aD1V, theTol);
}

//==================================================================================================

GeomProp::SurfaceCurvatureResult BRepProp_Surface::Curvatures(const double theU,
                                                              const double theV,
                                                              const double theTol) const
{
  if (!IsInitialized())
  {
    return {};
  }
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  myPtr->D2(theU, theV, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
  return GeomProp::ComputeSurfaceCurvatures(aD1U, aD1V, aD2U, aD2V, aD2UV, theTol);
}

//==================================================================================================

GeomProp::MeanGaussianResult BRepProp_Surface::MeanGaussian(const double theU,
                                                            const double theV,
                                                            const double theTol) const
{
  if (!IsInitialized())
  {
    return {};
  }
  gp_Pnt aPnt;
  gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
  myPtr->D2(theU, theV, aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
  return GeomProp::ComputeMeanGaussian(aD1U, aD1V, aD2U, aD2V, aD2UV, theTol);
}
